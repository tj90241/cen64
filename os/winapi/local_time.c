//
// os/winapi/local_time.c: Time functions for Windows.
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2015, Tyler J. Stachecki.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "local_time.h"
#include <time.h>
#include <windows.h>

#define NANOSECONDS_PER_SECOND 10000000

static int64_t systemtime_to_seconds(const SYSTEMTIME * st) {
  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);
  int64_t ticks = (int64_t)(ft.dwHighDateTime) << 32 | ft.dwLowDateTime;
  return (int64_t)(ticks / NANOSECONDS_PER_SECOND);
}

static void seconds_to_systemtime(int64_t seconds, SYSTEMTIME * st) {
  FILETIME ft;
  int64_t ticks = (int64_t)(*seconds * NANOSECONDS_PER_SECOND);
  ft.dwHighDateTime = ticks >> 32;
  ft.dwLowDateTime = ticks;
  FileTimeToSystemTime(&ft, &st);
}

void get_local_time(struct time_stamp *ts, int32_t offset_seconds) {
  SYSTEMTIME sysTime;
  GetLocalTime(&sysTime);
  
  if (offset_seconds != 0) {
    int64_t seconds = systemtime_to_seconds(&sysTime);
    seconds += offset_seconds;
    seconds_to_systemtime(seconds, &sysTime);
  }

  ts->year  = sysTime.wYear;
  ts->month = sysTime.wMonth;
  ts->day   = sysTime.wDay;
  ts->hour  = sysTime.wHour;
  ts->min   = sysTime.wMinute;
  ts->sec   = sysTime.wSecond;
  ts->week_day = sysTime.wDayOfWeek - 1;
}

int32_t get_offset_seconds(const struct time_stamp * ts) {
  SYSTEMTIME then_time;
  then_time.wYear = ts->year;
  then_time.wMonth = ts->month;
  then_time.wDay = ts->day;
  then_time.wHour = ts->hour;
  then_time.wMinute = ts->min;
  then_time.wSecond = ts->sec;
  then_time.wDayOfWeek = ts->week_day + 1;

  int64_t then_seconds = systemtime_to_seconds(&then_time);

  SYSTEMTIME now_time;
  GetLocalTime(&now_time);
  int64_t now_seconds = systemtime_to_seconds(&now_time);

  return then_seconds - now_seconds;
}
