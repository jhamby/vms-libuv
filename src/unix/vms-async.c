/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
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

/* This file contains both the uv__async internal infrastructure and the
 * user-facing uv_async_t functions.
 */

#include "uv.h"
#include "internal.h"

#include <errno.h>
#ifdef __cplusplus
#include <atomic>
using namespace std;
#else
#include <stdatomic.h>
#endif
#include <stdio.h>  /* snprintf() */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sched.h>  /* sched_yield() */

#define __NEW_STARLET 1

#include <starlet.h>

static void uv__async_send(uv_loop_t* loop);
static void uv__cpu_relax(void);


int uv_async_init(uv_loop_t* loop, uv_async_t* handle, uv_async_cb async_cb) {
  uv__handle_init(loop, (uv_handle_t*)handle, UV_ASYNC);
  handle->async_cb = async_cb;
  handle->pending = 0;
  handle->u.fd = 0; /* This will be used as a busy flag. */

  uv__queue_insert_tail(&loop->async_handles, &handle->queue);
  uv__handle_start(handle);

  return 0;
}


int uv_async_send(uv_async_t* handle) {
#ifdef __cplusplus
  atomic_int* pending;
  atomic_int* busy;

  pending = (atomic_int*) &handle->pending;
  busy = (atomic_int*) &handle->u.fd;
#else
  _Atomic int* pending;
  _Atomic int* busy;

  pending = (_Atomic int*) &handle->pending;
  busy = (_Atomic int*) &handle->u.fd;
#endif

  /* Do a cheap read first. */
  if (atomic_load_explicit(pending, memory_order_relaxed) != 0)
    return 0;

  /* Set the loop to busy. */
  atomic_fetch_add_explicit(busy, 1, memory_order_acquire);

  /* Wake up the other thread's event loop. */
  if (atomic_exchange_explicit(pending, 1, memory_order_relaxed) == 0)
    uv__async_send(handle->loop);

  /* Set the loop to not-busy. */
  atomic_fetch_add_explicit(busy, -1, memory_order_release);

  return 0;
}


/* Wait for the busy flag to clear before closing.
 * Only call this from the event loop thread. */
static void uv__async_spin(uv_async_t* handle) {
#ifdef __cplusplus
  atomic_int* pending;
  atomic_int* busy;
  int i;

  pending = (atomic_int*) &handle->pending;
  busy = (atomic_int*) &handle->u.fd;
#else
  _Atomic int* pending;
  _Atomic int* busy;
  int i;

  pending = (_Atomic int*) &handle->pending;
  busy = (_Atomic int*) &handle->u.fd;
#endif

  /* Set the pending flag first, so no new events will be added by other
   * threads after this function returns. */
  atomic_store_explicit(pending, 1, memory_order_relaxed);

  for (;;) {
    /* 997 is not completely chosen at random. It's a prime number, acyclic by
     * nature, and should therefore hopefully dampen sympathetic resonance.
     */
    for (i = 0; i < 997; i++) {
      if (atomic_load_explicit(busy, memory_order_relaxed) == 0)
        return;

      /* Other thread is busy with this handle, spin until it's done. */
      uv__cpu_relax();
    }

    /* Yield the CPU. We may have preempted the other thread while it's
     * inside the critical section and if it's running on the same CPU
     * as us, we'll just burn CPU cycles until the end of our time slice.
     */
    sched_yield();
  }
}


void uv__async_close(uv_async_t* handle) {
  uv__async_spin(handle);
  uv__queue_remove(&handle->queue);
  uv__handle_stop(handle);
}


static void uv__async_send(uv_loop_t* loop) {
  atomic_int* pending = (atomic_int*) &loop->async_pending_flags;
  if (atomic_fetch_or_explicit(pending, UV__LOOP_PENDING_ASYNC,
                                memory_order_relaxed) == 0) {
    sys$setef(loop->async_ef);
  }
}


static void uv__cpu_relax(void) {
#if defined(__i386__) || defined(__x86_64__)
  __asm__ __volatile__ ("rep; nop" ::: "memory");  /* a.k.a. PAUSE */
#elif (defined(__arm__) && __ARM_ARCH >= 7) || defined(__aarch64__)
  __asm__ __volatile__ ("yield" ::: "memory");
#endif
}
