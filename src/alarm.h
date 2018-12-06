#ifndef ALARM_H
#define ALARM_H

#include <Arduino.h>
#include <RtcDateTime.h>

void alarm_begin();

int get_hour();
int get_minute();
bool get_enabled();

void alarm_set(int hour, int minute, bool enabled);

void alarm_tick(unsigned long millis);
void alarm_clock_tick(RtcDateTime now);

void alarm_disable();

#endif  // ALARM_H