#include <Arduino.h>

#include "debug.h"
#include "rtc.h"
#include "player.h"
#include "alarm.h"

bool debug = false;

#define STOP_BUTTON 15

void printDetail(uint8_t type, int value);
void printDateTime(const RtcDateTime& dt);

#define countof(a) (sizeof(a) / sizeof(a[0]))

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  pinMode(STOP_BUTTON, INPUT_PULLUP);

  delay(3000);
  debug = bool(Serial);
  if (debug) {
    Serial.println("Hello world");
  }

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
  if (debug) Serial.println(F("Sending stats"));
  Serial1.print("stat");
  sendPadded(&Serial1, get_hour());
  sendPadded(&Serial1, get_minute());
  Serial1.println(int(get_enabled()));
}

void loop() {
  if (Serial1.available() > 0) {
    auto cmd = Serial1.readStringUntil('\n');
    if (cmd[cmd.length() - 1] == '\r') {
      cmd = cmd.substring(0, cmd.length() - 1);
    }
    String verb = cmd.substring(0, 4);

    if (debug) {
      Serial.print(F("Got from BT: "));
      Serial.println(cmd);
    }
    if (verb == "stat") {
      sendStatus();
    } else if (verb == "sttm") {
      int newhour = cmd.substring(4, 6).toInt();
      int newminute = cmd.substring(6, 8).toInt();
      if (newhour >= 0 && newhour < 24 && newminute >= 0 && newminute < 60) {
        alarm_set(newhour, newminute, get_enabled());
      }
      if (debug) {
        Serial.print(F("New time:"));
        int hour = get_hour();
        sendPadded(&Serial, hour);
        Serial.print(F(":"));
        sendPadded(&Serial, get_minute());
        Serial.println("");
      }
      sendStatus();
    } else if (verb == "swtc") {
      if (cmd[4] == '0' || cmd[4] == '1') {
        alarm_set(get_hour(), get_minute(), cmd[4] == '1');
      }
      if (debug) {
        Serial.print(F("New enabled: "));
        Serial.println(int(get_enabled()));
      }
      sendStatus();
    } else if (verb == "tmnw") {
      int newhour = cmd.substring(4, 6).toInt();
      int newminute = cmd.substring(6, 8).toInt();
      int newsecond = cmd.substring(8, 10).toInt();
      if (newhour >= 0 && newhour < 24 &&
          newminute >= 0 && newminute < 60 &&
          newsecond >= 0 && newsecond < 60) {
        rtc_set_time(newhour, newminute, newsecond);
      }
    }
  }

  static unsigned long timer = millis();
  unsigned long now = millis();

  if (now - timer > 1000) {
    timer = now;
    RtcDateTime nowrtc = rtc_now();
    if (debug) {
      printDateTime(nowrtc);
      Serial.println();
    }
    alarm_clock_tick(nowrtc);
  }

  alarm_tick(now);

  if (digitalRead(STOP_BUTTON) == 0) {
    alarm_disable();
  }
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring, countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(),
             dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
  if (debug) Serial.print(datestring);
}
