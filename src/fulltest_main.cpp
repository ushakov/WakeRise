#include <Arduino.h>

#include "rtc.h"
#include "player.h"
#include "alarm.h"


void printDetail(uint8_t type, int value);
void printDateTime(const RtcDateTime& dt);

#define countof(a) (sizeof(a) / sizeof(a[0]))

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  delay(3000);
  Serial.println("Hello world");

  rtc_begin();
  alarm_begin();
}

void sendPadded(Print* p, int d) {
  if (d < 10) {
    p->print('0');
  }
  p->print(d);
}

void sendStatus() {
  Serial.println("Sending stats");
  Serial1.print("stat");
  sendPadded(&Serial1, get_hour());
  sendPadded(&Serial1, get_minute());
  Serial1.println(int(get_enabled()));
}

void loop() {
  if (Serial1.available() > 0) {
    Serial.print(F("Got from BT: "));
    auto cmd = Serial1.readStringUntil('\n');
    if (cmd[cmd.length() - 1] == '\r') {
      cmd = cmd.substring(0, cmd.length() - 1);
    }
    Serial.println(cmd);
    String verb = cmd.substring(0, 4);
    if (verb == "stat") {
      sendStatus();
    } else if (verb == "sttm") {
      int newhour = cmd.substring(4, 6).toInt();
      int newminute = cmd.substring(6, 8).toInt();
      if (newhour >= 0 && newhour < 24 && newminute >= 0 && newminute < 60) {
        alarm_set(newhour, newminute, get_enabled());
      }
      Serial.print(F("New time:"));
      int hour = get_hour();
      sendPadded(&Serial, hour);
      Serial.print(F(":"));
      sendPadded(&Serial, get_minute());
      Serial.println("");
      sendStatus();
    } else if (verb == "swtc") {
      if (cmd[4] == '0' || cmd[4] == '1') {
        alarm_set(get_hour(), get_minute(), cmd[4] == '1');
      }
      Serial.print(F("New enabled: "));
      Serial.println(int(get_enabled()));
      sendStatus();
    }
  }

  static unsigned long timer = millis();
  unsigned long now = millis();

  if (now - timer > 1000) {
    timer = now;
    RtcDateTime nowrtc = rtc_now();
    printDateTime(nowrtc);
    Serial.println();
    alarm_clock_tick(nowrtc);
  }

  alarm_tick(now);
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring, countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(),
             dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
  Serial.print(datestring);
}