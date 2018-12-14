#ifndef RTC_H
#define RTC_H

#include <Arduino.h>
#include <RtcDateTime.h>

void rtc_begin();
RtcDateTime rtc_now();
void rtc_set_time(int hour, int minute, int second);

#endif  // RTC_H