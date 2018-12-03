#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#include "rtc.h"

SoftwareSerial player_serial(8, 9);  // RX, TX
DFRobotDFPlayerMini player;

void printDetail(uint8_t type, int value);
void printDateTime(const RtcDateTime& dt);

#define countof(a) (sizeof(a) / sizeof(a[0]))

int hour = 12;
int minute = 0;
bool enabled = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello world");

  Serial1.begin(9600);
  Serial1.print("AT");
  String resp = Serial1.readString();
  Serial.print("Result: ");
  Serial.println(resp);

  player_serial.begin(9600);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!player.begin(
          player_serial)) {  // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  player.volume(0);  // Set volume value. From 0 to 30

  rtc_begin();
}

void sendPadded(Print* p, int d, int pad) {
  String s(d);
  for (int i = 0; i + int(s.length()) < pad; i++) {
    p->print('0');
  }
  p->print(s);
}

void sendStatus() {
  Serial.println("Sending stats");
  Serial1.print("stat");
  sendPadded(&Serial1, hour, 2);
  sendPadded(&Serial1, minute, 2);
  Serial1.println(int(enabled));
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
        hour = newhour;
        minute = newminute;
      }
      Serial.print(F("New time:"));
      sendPadded(&Serial, hour, 2);
      Serial.print(F(":"));
      sendPadded(&Serial, minute, 2);
      Serial.println("");
      sendStatus();
    } else if (verb == "swtc") {
      if (cmd[4] == '0' || cmd[4] == '1') {
        enabled = (cmd[4] == '1');
      }
      Serial.print(F("New enabled: "));
      Serial.println(int(enabled));
      sendStatus();
    }
  }

  static unsigned long timer = millis();

  // if (millis() - timer > 3000) {
  //   timer = millis();
  //   player.next();  // Play next mp3 every 3 second.
  // }

  if (player.available()) {
    printDetail(player.readType(),
                player.read());  // Print the detail message from DFPlayer
                                 // to handle different errors and states.
  }

  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      Serial.read();
    }
    player.next();
    Serial.print(F("Playing "));
    Serial.println(player.readCurrentFileNumber());
  }

  if (millis() - timer > 1000) {
    timer = millis();
    RtcDateTime now = rtc_now();
    printDateTime(now);
    Serial.println();
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void printDateTime(const RtcDateTime& dt) {
  char datestring[20];

  snprintf_P(datestring, countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"), dt.Month(), dt.Day(),
             dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
  Serial.print(datestring);
}