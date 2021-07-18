//
// os/common/local_time.h
//
// Functions for getting current time.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#ifndef __os_local_time_h__
#define __os_local_time_h__
#include "common.h"
#include <stddef.h>

#ifdef _WIN32
#include <windows.h>
#endif

struct time_stamp {
  unsigned year;
  unsigned month;
  unsigned day;
  unsigned hour;
  unsigned min;
  unsigned sec;

  unsigned week_day;
};

void get_local_time(struct time_stamp *ts, int32_t offset_seconds);

int32_t get_offset_seconds(const struct time_stamp * ts);

static inline uint8_t byte2bcd(unsigned byte) {
  byte %= 100;
  return ((byte / 10) << 4) | (byte % 10);
}

static inline uint8_t bcd2byte(uint8_t bcd) {
    uint8_t hi = (bcd & 0xF0) >> 4;
    uint8_t lo = bcd & 0x0F;
    return (hi * 10) + lo;
}

#endif
