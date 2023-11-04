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


#include "vms-syscalls.h"
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

/* Implement a few syscalls missing from OpenVMS (copied from MVS port). */

int scandir(const char* maindir, struct dirent*** namelist,
            int (*filter)(const struct dirent*),
            int (*compar)(const struct dirent**,
            const struct dirent **)) {
  struct dirent** nl;
  struct dirent** nl_copy;
  struct dirent* dirent;
  unsigned count;
  size_t allocated;
  DIR* mdir;

  nl = NULL;
  count = 0;
  allocated = 0;
  mdir = opendir(maindir);
  if (!mdir)
    return -1;

  for (;;) {
    dirent = readdir(mdir);
    if (!dirent)
      break;
    if (!filter || filter(dirent)) {
      struct dirent* copy;
      copy = (struct dirent*) uv__malloc(sizeof(*copy));
      if (!copy)
        goto error;
      memcpy(copy, dirent, sizeof(*copy));

      nl_copy = (struct dirent**) uv__realloc(nl, sizeof(*copy) * (count + 1));
      if (nl_copy == NULL) {
        uv__free(copy);
        goto error;
      }

      nl = nl_copy;
      nl[count++] = copy;
    }
  }

  qsort(nl, count, sizeof(struct dirent *),
       (int (*)(const void *, const void *)) compar);

  closedir(mdir);

  *namelist = nl;
  return count;

error:
  while (count > 0) {
    dirent = nl[--count];
    uv__free(dirent);
  }
  uv__free(nl);
  closedir(mdir);
  errno = ENOMEM;
  return -1;
}


char* mkdtemp(char* path) {
  static const char* tempchars =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  static const size_t num_chars = 62;
  static const size_t num_x = 6;
  char *ep, *cp;
  unsigned int tries, i;
  size_t len;
  uint64_t v;
  int fd;
  int retval;
  int saved_errno;

  len = strlen(path);
  ep = path + len;
  if (len < num_x || strncmp(ep - num_x, "XXXXXX", num_x)) {
    errno = EINVAL;
    return NULL;
  }

  fd = open("/dev/urandom", O_RDONLY);
  if (fd == -1)
    return NULL;

  tries = TMP_MAX;
  retval = -1;
  do {
    if (read(fd, &v, sizeof(v)) != sizeof(v))
      break;

    cp = ep - num_x;
    for (i = 0; i < num_x; i++) {
      *cp++ = tempchars[v % num_chars];
      v /= num_chars;
    }

    if (mkdir(path, S_IRWXU) == 0) {
      retval = 0;
      break;
    }
    else if (errno != EEXIST)
      break;
  } while (--tries);

  saved_errno = errno;
  uv__close(fd);
  if (tries == 0) {
    errno = EEXIST;
    return NULL;
  }

  if (retval == -1) {
    errno = saved_errno;
    return NULL;
  }

  return path;
}
