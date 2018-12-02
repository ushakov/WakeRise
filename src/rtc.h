#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <RtcDateTime.h>

void rtc_begin();
RtcDateTime rtc_now();

#endif  // RTC_H