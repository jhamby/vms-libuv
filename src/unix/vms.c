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

#include "uv.h"
#include "internal.h"

#include <assert.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#define __NEW_STARLET 1

#include <gen64def.h>
#include <starlet.h>

/* Difference between UNIX and VMS epochs (to delay wrapping around). */
#define VMS_EPOCH_OFFSET 35067168005400000ULL

uint64_t uv__hrtime(uv_clocktype_t type) {
  uint64_t now;
  (void) sys$gettim_prec(&now);
  return (now - VMS_EPOCH_OFFSET) * 100;
}


void uv_loadavg(double avg[3]) {
  /* TODO: use load average device */
  avg[0] = 0.0;
  avg[1] = 0.0;
  avg[2] = 0.0;
}


int uv_exepath(char* buffer, size_t* size) {
  return UV_EINVAL;
}


uint64_t uv_get_free_memory(void) {
  return 0;
}


uint64_t uv_get_total_memory(void) {
  return 0;
}


uint64_t uv_get_constrained_memory(void) {
  return 0;  /* Memory constraints are unknown. */
}


uint64_t uv_get_available_memory(void) {
  return uv_get_free_memory();
}


int uv_resident_set_memory(size_t* rss) {
  return UV_EINVAL;
}


int uv_uptime(double* uptime) {
  return UV_EINVAL;
}


int uv_cpu_info(uv_cpu_info_t** cpu_infos, int* count) {
  return UV_EINVAL;
}
