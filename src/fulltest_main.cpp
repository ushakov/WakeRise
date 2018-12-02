#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#include "rtc.h"

SoftwareSerial player_serial(8, 9);  // RX, TX
DFRobotDFPlayerMini player;

void printDetail(uint8_t type, int value);
void printDateTime(const RtcDateTime& dt);

#define countof(a) (sizeof(a) / sizeof(a[0]))

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
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  player.volume(30);  // Set volume value. From 0 to 30
  player.loop(1);     // Play the first mp3

  rtc_begin();
}

void loop() {
  if (Serial1.available() > 0) {
    Serial.print(F("Got from BT: "));
    auto cmd = Serial1.readStringUntil('\n');
    if (cmd[cmd.length() - 1] == '\r') {
      cmd = cmd.substring(0, cmd.length() - 1);
    }
    Serial.println(cmd);
    if (cmd == "stop") {
      Serial.println(F("Stop"));
      player.stop();
    } else if (cmd == "start") {
      Serial.println(F("Start @1"));
      player.loop(1);
    } else if (cmd == "volup") {
      Serial.println(F("Volume up"));
      player.volumeUp();
    } else if (cmd == "voldown") {
      Serial.println(F("Volume down"));
      player.volumeDown();
    } else if (cmd == "next") {
      player.next();
    }
    Serial.print(F("Playing "));
    Serial.print(player.readCurrentFileNumber());
    Serial.print(F(" at volume "));
    Serial.println(player.readVolume());
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