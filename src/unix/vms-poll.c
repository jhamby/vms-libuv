/* Copyright libuv project contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "uv.h"
#include "internal.h"

/* POSIX defines poll() as a portable way to wait on file descriptors.
 * Here we maintain a dynamically sized array of file descriptors and
 * events to pass as the first argument to poll(). In order to wake up
 * on AST and worker thread completions more quickly, a worker thread
 * is created on the first poll(), controlled through a socket that it
 * polls on in addition to the fds in the list. The thread stays blocked
 * on the fd list until the list changes or the loop exits, so that it
 * can remain asleep while the event loop thread is being awoken by the
 * local event flag for AST and worker thread completions.
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>

#define __NEW_STARLET 1

#include <gen64def.h>
#include <lib$routines.h>
#include <psldef.h>
#include <starlet.h>
#include <stsdef.h>

/* States for the poll thread state machine */
#define POLL_THREAD_REQ_POLL            0
#define POLL_THREAD_REQ_PAUSE           1
#define POLL_THREAD_REQ_EXIT            2

static uv_once_t once = UV_ONCE_INIT;

static void uv__poll_thread(void* arg);

/* Free local event flags 1-23 at startup so we can reserve them later.
 * Flags 1-23 are reserved by the system for the benefit of old programs
 * that may have used those flags without reserving them. This is in
 * contrast to flag 0, the default if not specified, or flags 24-31,
 * which are always reserved to VMS. Note that lib$get_ef() currently
 * returns flags in decreasing order from 63.
 */
static void uv__platform_free_efs(void) {
  for (unsigned int i = 1; i < 24; i++) {
    lib$free_ef(&i);
  }
}

int uv__platform_loop_init(uv_loop_t* loop) {
  /* free event flags 1-23 */
  uv_once(&once, uv__platform_free_efs);

  /* create sockfd pair for poll thread */
  if (uv__make_pipe(loop->poll_thread_sockfd, UV_NONBLOCK_PIPE) < 0) {
    abort();
  }

  /* reserve an event flag for async, AST, and poll thread wakeups */
  if (!$VMS_STATUS_SUCCESS(lib$get_ef(&loop->async_ef))) {
    return UV_ENOMEM;
  }

  /* reserve a second event flag for timer events */
  if (!$VMS_STATUS_SUCCESS(lib$get_ef(&loop->timer_ef))) {
    lib$free_ef(&loop->async_ef);
    return UV_ENOMEM;
  }

  /* If the two event flags aren't in the same cluster, request
   * a third event flag to swap with one of the other two,
   * freeing whichever original flag is now mismatched first.
   */
  if (loop->async_ef >= 32 && loop->timer_ef < 32) {
    unsigned int third_ef;
    if (!$VMS_STATUS_SUCCESS(lib$get_ef(&third_ef))) {
      lib$free_ef(&loop->async_ef);
      lib$free_ef(&loop->timer_ef);
      return UV_ENOMEM;
    }
    if (third_ef >= 32) {
      lib$free_ef(&loop->timer_ef);
      loop->timer_ef = third_ef;
    } else {
      lib$free_ef(&loop->async_ef);
      loop->async_ef = third_ef;
    }
  } else if (loop->async_ef < 32 && loop->timer_ef >= 32) {
    unsigned int third_ef;
    if (!$VMS_STATUS_SUCCESS(lib$get_ef(&third_ef))) {
      lib$free_ef(&loop->async_ef);
      lib$free_ef(&loop->timer_ef);
      return UV_ENOMEM;
    }
    if (third_ef < 32) {
      lib$free_ef(&loop->timer_ef);
      loop->timer_ef = third_ef;
    } else {
      lib$free_ef(&loop->async_ef);
      loop->async_ef = third_ef;
    }
  }

  /* Clear the local event flag for this loop's async wakeups here.
   * The call to sys$setimr will clear its event flag before use.
   */
  sys$clref(loop->async_ef);

  loop->poll_thread = 0;
  loop->poll_fds_iterating = 0;
  loop->poll_thread_paused = 0;
  loop->poll_fds = NULL;
  loop->poll_fds_used = 0;
  loop->poll_fds_size = 0;
  loop->ast_async_tail = NULL;
  loop->async_pending_flags = 0;
  return 0;
}

void uv__platform_loop_delete(uv_loop_t* loop) {
  /* send exit request to poll thread and wait for exit */
  if (loop->poll_thread) {
    const char req_exit = POLL_THREAD_REQ_EXIT;
    write(loop->poll_thread_sockfd[1], &req_exit, 1);
    pthread_join(loop->poll_thread, NULL);
    loop->poll_thread = 0;
  }
  /* now that poll thread has exited, delete socketpair */
  close(loop->poll_thread_sockfd[0]);
  close(loop->poll_thread_sockfd[1]);
  uv__free(loop->poll_fds);
  loop->poll_fds = NULL;
  /* free the event flags for reuse */
  lib$free_ef(&loop->async_ef);
  lib$free_ef(&loop->timer_ef);
}

/* Poll fd array has changed; pause poll thread if running. */
static void uv__poll_thread_pause_if_running(uv_loop_t* loop) {
  if (loop->poll_thread && !loop->poll_thread_paused) {
    loop->poll_thread_paused = 1;
    const char req_pause = POLL_THREAD_REQ_PAUSE;
    write(loop->poll_thread_sockfd[1], &req_pause, 1);
  }
}

/* Allocate or dynamically resize our poll fds array.  */
static void uv__pollfds_maybe_resize(uv_loop_t* loop) {
  size_t i;
  size_t n;
  struct pollfd* p;

  if (loop->poll_fds_used < loop->poll_fds_size)
    return;

  n = loop->poll_fds_size ? loop->poll_fds_size * 2 : 64;
  p = (struct pollfd*) uv__reallocf(loop->poll_fds, n * sizeof(*loop->poll_fds));
  if (p == NULL)
    abort();

  loop->poll_fds = p;
  for (i = loop->poll_fds_size; i < n; i++) {
    loop->poll_fds[i].fd = -1;
    loop->poll_fds[i].events = 0;
    loop->poll_fds[i].revents = 0;
  }
  loop->poll_fds_size = n;
}

/* Primitive swap operation on poll fds array elements.  */
static void uv__pollfds_swap(uv_loop_t* loop, size_t l, size_t r) {
  struct pollfd pfd;
  pfd = loop->poll_fds[l];
  loop->poll_fds[l] = loop->poll_fds[r];
  loop->poll_fds[r] = pfd;
}

/* Add a watcher's fd to our poll fds array with its pending events.  */
static void uv__pollfds_add(uv_loop_t* loop, uv__io_t* w) {
  size_t i;
  struct pollfd* pe;

  /* If the fd is already in the set just update its events.  */
  assert(!loop->poll_fds_iterating);
  for (i = 0; i < loop->poll_fds_used; ++i) {
    if (loop->poll_fds[i].fd == w->fd) {
      if (loop->poll_fds[i].events != w->pevents) {
        uv__poll_thread_pause_if_running(loop);
        loop->poll_fds[i].events = w->pevents;
      }
      return;
    }
  }

  uv__poll_thread_pause_if_running(loop);
  /* Otherwise, allocate a new slot in the set for the fd.  */
  uv__pollfds_maybe_resize(loop);
  /* Add the poll thread's command sockfd as the first entry. */
  if (loop->poll_fds_used == 0) {
    loop->poll_fds[0].fd = loop->poll_thread_sockfd[0];
    loop->poll_fds[0].events = POLLIN;
    loop->poll_fds_used++;
  }
  pe = &loop->poll_fds[loop->poll_fds_used++];
  pe->fd = w->fd;
  pe->events = w->pevents;
}

/* Remove a watcher's fd from our poll fds array.  */
static void uv__pollfds_del(uv_loop_t* loop, int fd) {
  size_t i;
  assert(!loop->poll_fds_iterating);

  for (i = 0; i < loop->poll_fds_used;) {
    if (loop->poll_fds[i].fd == fd) {
      uv__poll_thread_pause_if_running(loop);
      /* swap to last position and remove */
      --loop->poll_fds_used;
      uv__pollfds_swap(loop, i, loop->poll_fds_used);
      loop->poll_fds[loop->poll_fds_used].fd = -1;
      loop->poll_fds[loop->poll_fds_used].events = 0;
      loop->poll_fds[loop->poll_fds_used].revents = 0;
      /* This method is called with an fd of -1 to purge the invalidated fds,
       * so we may possibly have multiples to remove.
       */
      if (-1 != fd)
        return;
    } else {
      /* We must only increment the loop counter when the fds do not match.
       * Otherwise, when we are purging an invalidated fd, the value just
       * swapped here from the previous end of the array will be skipped.
       */
       ++i;
    }
  }
}


static void uv__poll_thread(void* arg) {
  uv_loop_t* loop = (uv_loop_t*) arg;
  char buf[16];

  char state = POLL_THREAD_REQ_POLL;
  struct pollfd poll1 = { loop->poll_thread_sockfd[0], POLLIN, 0 };

  for (;;) {
    for (;;) {
      int r = read(loop->poll_thread_sockfd[0], buf, sizeof(buf));

      /* New state will be the last byte read from the event loop thread. */
      if (r > 0)
        state = buf[r - 1];

      if (r == sizeof(buf))
        continue;

      if (r != -1)
        break;

      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break;

      /* if we get any other errno, the socket is probably closing */
      if (errno != EINTR) {
        return;
      }
    }

    int nfds = 0;
    atomic_int* pending = (atomic_int*) &loop->async_pending_flags;
    switch (state) {
      case POLL_THREAD_REQ_POLL:
        if (atomic_load_explicit(pending, memory_order_acquire) &
                UV__LOOP_PENDING_POLL) {
          /* Wait for another request, as the event thread still has
           * waiting events from the last call to poll() to process.
           */
          state = POLL_THREAD_REQ_PAUSE;
          break;
        }
        /* Poll will return with commands from the event loop thread and/or
         * events that the event loop thread cares about. If we receive any
         * of the latter, set our event pending atomic flag and then go into
         * pause mode until the new mode is sent by the event loop thread.
         */
        nfds = poll(loop->poll_fds, loop->poll_fds_used, -1);

        /* only report events that aren't requests to this thread */
        if (nfds > 1 || (nfds > 0 && (loop->poll_fds[0].revents == 0))) {
          /* pause until the event loop thread tells us to resume */
          state = POLL_THREAD_REQ_PAUSE;
          /* wake up event loop thread if we're still in polling state */
          if (atomic_fetch_or_explicit(pending, UV__LOOP_PENDING_POLL,
                                       memory_order_relaxed) == 0) {
            sys$setef(loop->async_ef);
          }
        }
        break;

      case POLL_THREAD_REQ_PAUSE:
        poll(&poll1, 1, -1);
        break;

      case POLL_THREAD_REQ_EXIT:
      default:
        return;
    }
  }
}


void uv__io_poll(uv_loop_t* loop, int timeout) {
  uv__loop_internal_fields_t* lfields;
  uint64_t time_base;
  uint64_t time_diff;
  struct uv__queue* q;
  uv__io_t* w;
  size_t i;
  struct pollfd* pe;
  int fd;

  lfields = uv__get_internal_fields(loop);

  /* Take queued watchers and add their fds to our poll fds array.  */
  while (!uv__queue_empty(&loop->watcher_queue)) {
    q = uv__queue_head(&loop->watcher_queue);
    uv__queue_remove(q);
    uv__queue_init(q);

    w = uv__queue_data(q, uv__io_t, watcher_queue);
    assert(w->pevents != 0);
    assert(w->fd >= 0);
    assert(w->fd < (int) loop->nwatchers);

    uv__pollfds_add(loop, w);

    w->events = w->pevents;
  }

  assert(timeout >= -1);
  time_base = loop->time;

  /* Loop calls to poll() and processing of results.  If we get some
   * results from poll() but they turn out not to be interesting to
   * our caller then we need to loop around and poll() again.
   */
  for (;;) {
    /* Only need to set the provider_entry_time if timeout != 0. The function
     * will return early if the loop isn't configured with UV_METRICS_IDLE_TIME.
     */
    if (timeout != 0)
      uv__metrics_set_provider_entry_time(loop);

    /* Store the current timeout in a location that's globally accessible so
     * other locations like uv__work_done() can determine whether the queue
     * of events in the callback were waiting when poll was called.
     */
    lfields->current_timeout = timeout;

    if (loop->poll_fds_used > 1) {
      /* Start poll thread if needed, or tell existing thread to
       * restart poll() if it was paused by the event thread's
       * change of interested fd events.
       */
      if (loop->poll_thread_paused) {
        loop->poll_thread_paused = 0;
        const char req_poll = POLL_THREAD_REQ_POLL;
        write(loop->poll_thread_sockfd[1], &req_poll, 1);
      } else if (!loop->poll_thread) {
        if (uv_thread_create(&loop->poll_thread, uv__poll_thread, loop) < 0) {
          abort();
        }
      }
    }

    if (timeout == -1) {
      /* Wait for async event, AST callback, and/or poll thread events. */
      if (!$VMS_STATUS_SUCCESS(sys$waitfr(loop->async_ef))) {
        abort();
      }
    } else {
      /* Set timer ef to wake on timeout, then sleep on either event flag. */
      struct _generic_64 delta_timeout = { (uint64_t) (-((int64_t) timeout * 10000)) };
      if (!$VMS_STATUS_SUCCESS(sys$setimr(loop->timer_ef, &delta_timeout,
                        NULL, (int64_t) loop->timer_ef, 0))) {
        abort();
      }

      uint64_t ef_or_mask = (1ull << loop->async_ef) | (1ull << loop->timer_ef);
      if (loop->async_ef >= 32) {
        ef_or_mask >>= 32;
      }

      if (!$VMS_STATUS_SUCCESS(sys$wflor(loop->async_ef, (uint32_t) ef_or_mask))) {
        abort();
      }

      /* Cancel the timer event, whether or not it expired. */
      if (!$VMS_STATUS_SUCCESS(sys$cantim((int64_t) loop->timer_ef, PSL$C_USER))) {
        abort();
      }
    }

    /* Update loop->time unconditionally. It's tempting to skip the update when
     * timeout == 0 (i.e. non-blocking poll) but there is no guarantee that the
     * operating system didn't reschedule our process while in the syscall.
     */
    SAVE_ERRNO(uv__update_time(loop));

    /* Clear local event flag before atomically swapping pending events flags.
     * It's possible that someone sets the local ef, then sets an async event
     * in between this call to sys$clref and the atomic exchange, in which
     * case we'll get no events on the next poll and have to wait again.
     */
    if (!$VMS_STATUS_SUCCESS(sys$clref(loop->async_ef))) {
      abort();
    }

    /* Initialize a count of events that we care about.  */
    int nevents = 0;
    int have_signals = 0;

    atomic_int* pending = (atomic_int*) &loop->async_pending_flags;
    int new_events = atomic_exchange_explicit(pending, 0, memory_order_acquire);

    /* add the stack of AST callbacks to the head of the async queue */
    if (new_events & UV__LOOP_PENDING_AST) {
      atomic_uintptr_t* tail = (atomic_uintptr_t*) &loop->ast_async_tail;
      uv_async_t* ast_tail = (uv_async_t*) atomic_exchange_explicit(tail,
                                (uintptr_t) 0, memory_order_release);
      while (ast_tail) {
        uv__queue_insert_head(&loop->async_handles, &ast_tail->queue);
        ast_tail = (uv_async_t*) ast_tail->queue.next;
      }
    }

    /* call back async events, like uv__async_io in POSIX async.c */
    if (new_events & (UV__LOOP_PENDING_AST | UV__LOOP_PENDING_ASYNC)) {
      struct uv__queue queue;

      uv__queue_move(&loop->async_handles, &queue);
      while (!uv__queue_empty(&queue)) {
        struct uv__queue* q = uv__queue_head(&queue);
        uv_async_t* h = uv__queue_data(q, uv_async_t, queue);

        uv__queue_remove(q);
        uv__queue_insert_tail(&loop->async_handles, q);

        /* Atomically fetch and clear pending flag */
        atomic_int* pending = (atomic_int*) &h->pending;
        if (atomic_exchange_explicit(pending, 0, memory_order_relaxed) == 0)
          continue;

        if (h->async_cb == NULL)
          continue;

        h->async_cb(h);
        nevents++;
      }
    }

    if (new_events & UV__LOOP_PENDING_POLL) {
      /* Tell uv__platform_invalidate_fd not to manipulate our array
       * while we are iterating over it.
       */
      loop->poll_fds_iterating = 1;

      /* Assume the poll thread is now paused until we restart it. */
      loop->poll_thread_paused = 1;

      /* Loop over the entire poll fds array looking for returned events.  */
      /* Skip over the first entry with the sockfd command channel. */
      for (i = 1; i < loop->poll_fds_used; i++) {
        pe = loop->poll_fds + i;
        fd = pe->fd;

        /* Skip invalidated events, see uv__platform_invalidate_fd.  */
        if (fd == -1)
          continue;

        assert(fd >= 0);
        assert((unsigned int) fd < loop->nwatchers);

        w = loop->watchers[fd];

        if (w == NULL) {
          /* File descriptor that we've stopped watching, ignore.  */
          uv__platform_invalidate_fd(loop, fd);
          continue;
        }

        /* Filter out events that user has not requested us to watch
         * (e.g. POLLNVAL).
         */
        pe->revents &= w->pevents | POLLERR | POLLHUP;

        if (pe->revents != 0) {
          /* Run signal watchers last.  */
          if (w == &loop->signal_io_watcher) {
            have_signals = 1;
          } else {
            uv__metrics_update_idle_time(loop);
            w->cb(loop, w, pe->revents);
          }

          nevents++;
        }
      }
      loop->poll_fds_iterating = 0;
    }

    uv__metrics_inc_events(loop, nevents);

    if (have_signals != 0) {
      uv__metrics_update_idle_time(loop);
      loop->signal_io_watcher.cb(loop, &loop->signal_io_watcher, POLLIN);
    }

    /* Purge invalidated fds from our poll fds array.  */
    uv__pollfds_del(loop, -1);

    if (have_signals != 0)
      return;  /* Event loop should cycle now so don't poll again. */

    if (nevents != 0)
      return;

    if (timeout == 0)
      return;

    if (timeout == -1)
      continue;

    assert(timeout > 0);

    time_diff = loop->time - time_base;
    if (time_diff >= (uint64_t) timeout)
      return;

    timeout -= time_diff;
  }
}

/* Remove the given fd from our poll fds array because no one
 * is interested in its events anymore.
 */
void uv__platform_invalidate_fd(uv_loop_t* loop, int fd) {
  size_t i;

  assert(fd >= 0);

  if (loop->poll_fds_iterating) {
    /* uv__io_poll is currently iterating.  Just invalidate fd.  */
    for (i = 0; i < loop->poll_fds_used; i++)
      if (loop->poll_fds[i].fd == fd) {
        uv__poll_thread_pause_if_running(loop);
        loop->poll_fds[i].fd = -1;
        loop->poll_fds[i].events = 0;
        loop->poll_fds[i].revents = 0;
      }
  } else {
    /* uv__io_poll is not iterating.  Delete fd from the set.  */
    uv__pollfds_del(loop, fd);
  }
}

/* Check whether the given fd is supported by poll().  */
int uv__io_check_fd(uv_loop_t* loop, int fd) {
  struct pollfd p[1];
  int rv;

  p[0].fd = fd;
  p[0].events = POLLIN;

  do
    rv = poll(p, 1, 0);
  while (rv == -1 && (errno == EINTR || errno == EAGAIN));

  if (rv == -1)
    return UV__ERR(errno);

  if (p[0].revents & POLLNVAL)
    return UV_EINVAL;

  return 0;
}
