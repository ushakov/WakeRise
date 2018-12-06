#include "alarm.h"

#include <EEPROM.h>

#include "player.h"
#include "leds.h"

#define HOUR_ADDR 0
#define MINUTE_ADDR 1
#define ENABLED_ADDR 2

const int kTotalMinutes = 1;

bool alarm_ringing = false;
long start_millis = 0;
int hour;
int minute;
bool enabled;

void alarm_off();

void save_eeprom() {
  EEPROM.write(HOUR_ADDR, hour);
  EEPROM.write(MINUTE_ADDR, minute);
  EEPROM.write(ENABLED_ADDR, enabled ? 1 : 0);
}

void alarm_begin() {
  hour = EEPROM.read(HOUR_ADDR);
  minute = EEPROM.read(MINUTE_ADDR);
  int int_enabled = EEPROM.read(ENABLED_ADDR);

  if (hour < 0 || hour > 23 || minute < 0 || minute > 59 ||
      (int_enabled != 0 && int_enabled != 1)) {
    Serial.println(F("Bad EEPROM values, set defaults"));
    hour = 12;
    minute = 0;
    enabled = false;
    save_eeprom();
  } else {
    enabled = (int_enabled == 1);
  }
  save_eeprom();

  leds_begin();
  player_begin();
}

int get_hour() { return hour; }
int get_minute() { return minute; }
bool get_enabled() { return enabled; }

void alarm_set(int hour_, int minute_, bool enabled_) {
  hour = hour_;
  minute = minute_;
  enabled = enabled_;
  save_eeprom();
  alarm_off();
}

void alarm_tick(unsigned long millis) {
  if (!alarm_ringing) {
    return;
  }
  double timepos = double(long(millis) - start_millis) / (kTotalMinutes * 60000);
  if (timepos < 0) {
    timepos = 0;
  }
  if (timepos > 1) {
    timepos = 1;
  }

  leds_set(timepos);
  player_set(timepos);
}

void alarm_clock_tick(RtcDateTime now) {
  if (!enabled) {
    return;
  }
  int nowminute = now.Hour() * 60 + now.Minute();
  int alarmminute = hour * 60 + minute;
  if (alarmminute < nowminute) {
    alarmminute += 24*60;
  }
  // After this, alarmminute == nearest future alarm point

  int startminute = alarmminute - kTotalMinutes;

  if (nowminute >= startminute && nowminute <= alarmminute) {
    if (!alarm_ringing) {
      alarm_ringing = true;
      start_millis = millis() - (nowminute - startminute) * 60000;
    }
  } else {
    if (alarm_ringing) {
      alarm_disable();
    }
  }
}

void alarm_off() {
  leds_off();
  player_off();
  alarm_ringing = false;
}

void alarm_disable() {
  alarm_off();
  enabled = false;
  save_eeprom();
}