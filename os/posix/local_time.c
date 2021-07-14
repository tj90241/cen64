//
// os/unix/rom_file.c
//
// Functions for mapping ROM images into the address space.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "local_time.h"
#include <time.h>

void get_local_time(struct time_stamp *ts, int32_t offset_seconds) {
  time_t now = time(NULL);
  now += offset_seconds;

  struct tm time = { 0, };
  localtime_r(&now, &time);

  ts->year  = time.tm_year;
  ts->month = time.tm_mon + 1; // month is zero-indexed in this struct
  ts->day   = time.tm_mday;
  ts->hour  = time.tm_hour;
  ts->min   = time.tm_min;
  ts->sec   = time.tm_sec;
  ts->week_day = time.tm_wday;
}

int32_t get_offset_seconds(const struct time_stamp * ts) {
  struct tm t = { 0, };
  t.tm_year = ts->year;
  t.tm_mon = ts->month - 1;
  t.tm_mday = ts->day;
  t.tm_hour = ts->hour;
  t.tm_min = ts->min;
  t.tm_sec = ts->sec;
  t.tm_wday = ts->week_day;
  t.tm_isdst = -1; // Auto-adjust for DST

  time_t then = mktime(&t);
  time_t now = time(NULL);

  return then - now;
}
