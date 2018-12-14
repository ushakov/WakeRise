#include "rtc.h"
#include <RtcDS1307.h>
#include <Wire.h>

RtcDS1307<TwoWire> rtc(Wire);

void rtc_begin() {
  rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);

  if (!rtc.IsDateTimeValid()) {
    Serial.println("RTC lost confidence in the DateTime!");
    rtc.SetDateTime(compiled);
  }

  if (!rtc.GetIsRunning()) {
    Serial.println("RTC was not actively running, starting now");
    rtc.SetIsRunning(true);
  }

  RtcDateTime now = rtc.GetDateTime();
  if (now < compiled) {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    rtc.SetDateTime(compiled);
  } else if (now > compiled) {
    Serial.println("RTC is newer than compile time. (this is expected)");
  } else if (now == compiled) {
    Serial.println(
        "RTC is the same as compile time! (not expected but all is fine)");
  }
  rtc.SetSquareWavePin(DS1307SquareWaveOut_Low);
}

RtcDateTime rtc_now() { 
    return rtc.GetDateTime(); 
}

void rtc_set_time(int hour, int minute, int second) {
  RtcDateTime old = rtc_now();
  RtcDateTime set = RtcDateTime(old.Year(), old.Month(), old.Day(), hour, minute, second);
  rtc.SetDateTime(set);
}