//
// si/rtc.c: RTC routines
//
// CEN64: Cycle-Accurate Nintendo 64 Emulator.
// Copyright (C) 2016, Mike Ryan.
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include "common.h"
#include "local_time.h"
#include "si/controller.h"

void rtc_init(struct rtc * rtc) {
  /* Write-protected, timer enabled */
  rtc->control = 0x0304;
  get_local_time(&rtc->now);
}

static inline uint8_t rtc_control_status(struct rtc * rtc) {
  return (rtc->control & 0x0004) ? 0x80 : 0x00;
}

int rtc_status(struct rtc * rtc,
    uint8_t *send_buf, uint8_t send_bytes,
    uint8_t *recv_buf, uint8_t recv_bytes) {
  // Check send/recv buffer lengths
  assert(send_bytes == 1);
  assert(recv_bytes == 3);

  recv_buf[0] = 0x00;
  recv_buf[1] = 0x10;
  recv_buf[2] = rtc_control_status(rtc);

  return 0;
}

int rtc_read(struct rtc * rtc,
    uint8_t *send_buf, uint8_t send_bytes,
    uint8_t *recv_buf, uint8_t recv_bytes) {
  // Check send/recv buffer lengths
  assert(send_bytes == 2);
  assert(recv_bytes == 9);

  // Zero out the response buffer
  memset(recv_buf, 0, recv_bytes);

  // read RTC block
  switch (send_buf[1]) {
    case 0:
      recv_buf[0] = (uint8_t)(rtc->control);
      recv_buf[1] = (uint8_t)(rtc->control >> 8);
      recv_buf[8] = rtc_control_status(rtc);
      break;

    case 1:
      debug("RTC read block 1 is not implemented\n");
      recv_buf[8] = rtc_control_status(rtc);
      break;

    case 2:
      // Update date/time if timer is enabled
      if (rtc->control & 0x0004) {
        get_local_time(&rtc->now);
      }
      recv_buf[0] = byte2bcd(rtc->now.sec);
      recv_buf[1] = byte2bcd(rtc->now.min);
      recv_buf[2] = 0x80 + byte2bcd(rtc->now.hour);
      recv_buf[3] = byte2bcd(rtc->now.day);
      recv_buf[4] = byte2bcd(rtc->now.week_day);
      recv_buf[5] = byte2bcd(rtc->now.month);
      recv_buf[6] = byte2bcd(rtc->now.year);
      recv_buf[7] = byte2bcd(rtc->now.year / 100);
      recv_buf[8] = rtc_control_status(rtc);
      break;

    default:
      debug("RTC read invalid block\n");
      return 1;
  }

  return 0;
}

int rtc_write(struct rtc * rtc,
    uint8_t *send_buf, uint8_t send_bytes,
    uint8_t *recv_buf, uint8_t recv_bytes) {
  // Check send/recv buffer lengths
  assert(send_bytes == 10);
  assert(recv_bytes == 1);
  
  // write RTC block
  switch (send_buf[1]) {
    case 0:
      rtc->control = (uint16_t)(send_buf[3] << 8) | send_buf[2];
      break;

    case 1:
      if ((rtc->control & 0x0100)) {
        debug("RTC write block 1 is write-protected\n");
      } else {
        debug("RTC write block 1 is not implemented\n");
      }
      break;

    case 2:
      if ((rtc->control & 0x0200)) {
        debug("RTC write block 2 is write-protected\n");
        break;
      }
      rtc->now.sec = bcd2byte(send_buf[2]);
      rtc->now.min = bcd2byte(send_buf[3]);
      rtc->now.hour = bcd2byte(send_buf[4] - 0x80);
      rtc->now.day = bcd2byte(send_buf[5]);
      rtc->now.week_day = bcd2byte(send_buf[6]);
      rtc->now.month = bcd2byte(send_buf[7]);
      rtc->now.year = bcd2byte(send_buf[8]) + (bcd2byte(send_buf[9]) * 100);
      break;

    default:
      debug("RTC write invalid block\n");
      return 1;
  }

  recv_buf[0] = rtc_control_status(rtc);

  return 0;
}
