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

#include <descrip.h>
#include <gen64def.h>
#include <lib$routines.h>
#include <jpidef.h>
#include <starlet.h>
#include <stsdef.h>
#include <syidef.h>

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


/* Return free pagefile size as a proxy for free physical memory. */
uint64_t uv_get_free_memory(void) {
  int syi_pagesize      = SYI$_PAGE_SIZE;
  int syi_pagefile_free = SYI$_PAGEFILE_FREE;
  unsigned int pagesize, pagefile_free;

  if (!$VMS_STATUS_SUCCESS(lib$getsyi(&syi_pagesize, &pagesize)))
    return 0;

  if (!$VMS_STATUS_SUCCESS(lib$getsyi(&syi_pagefile_free, &pagefile_free)))
    return 0;

  return ((uint64_t) pagefile_free) * pagesize;
}


uint64_t uv_get_total_memory(void) {
  int syi_pagesize      = SYI$_PAGE_SIZE;
  int syi_memsize       = SYI$_MEMSIZE;
  unsigned int pagesize, memsize;

  if (!$VMS_STATUS_SUCCESS(lib$getsyi(&syi_pagesize, &pagesize)))
    return 0;

  if (!$VMS_STATUS_SUCCESS(lib$getsyi(&syi_memsize, &memsize)))
    return 0;

  return ((uint64_t) memsize) * pagesize;
}


uint64_t uv_get_constrained_memory(void) {
  return 0;  /* Memory constraints are unknown. */
}


uint64_t uv_get_available_memory(void) {
  return uv_get_free_memory();
}


/* Return peak working set size as a proxy for current RSS */
int uv_resident_set_memory(size_t* rss) {
  int jpi_val = JPI$_WSPEAK;
  char buffer[22];      /* all uint64_t & int64_t values are <= 20 digits */
  uint16_t result_len = 0;
  $DESCRIPTOR64(result_dsc, buffer);
  uint64_t wspeak_pagelets;

  int ret = lib$getjpi(&jpi_val, NULL, NULL, NULL, &result_dsc, &result_len);
  if (!$VMS_STATUS_SUCCESS(ret) || result_len >= sizeof(buffer)) {
    return UV_EINVAL;
  }
  buffer[result_len] = '\0';

  wspeak_pagelets = strtoull(buffer, NULL, 10);
  if (wspeak_pagelets == ULLONG_MAX && errno == ERANGE)
    return UV_EINVAL;

  *rss = (size_t) (wspeak_pagelets * 512);
  return 0;
}


int uv_uptime(double* uptime) {
  struct _generic_64 boot_time;
  if (!$VMS_STATUS_SUCCESS(sys$gettim(&boot_time, 1))) {
    return UV_EINVAL;
  }
  /* Convert from 100-ns units to seconds */
  *uptime = (double) boot_time.gen64$q_quadword / 10000000.0;
  return 0;
}


int uv_cpu_info(uv_cpu_info_t** cpu_infos, int* count) {
  unsigned int numcpus, idx = 0;
  uv_cpu_info_t* cpu_info;

  *cpu_infos = NULL;
  *count = 0;

  numcpus = sysconf(_SC_NPROCESSORS_ONLN);

  *cpu_infos = (uv_cpu_info_t*) uv__malloc(numcpus * sizeof(uv_cpu_info_t));
  if (!*cpu_infos) {
    return UV_ENOMEM;
  }

  cpu_info = *cpu_infos;
  for (idx = 0; idx < numcpus; idx++) {
    cpu_info->speed = 0;
    cpu_info->model = uv__strdup("unknown");
    cpu_info->cpu_times.user = 0;
    cpu_info->cpu_times.sys = 0;
    cpu_info->cpu_times.idle = 0;
    cpu_info->cpu_times.irq = 0;
    cpu_info->cpu_times.nice = 0;
    cpu_info++;
  }
  *count = numcpus;

  return 0;
}
