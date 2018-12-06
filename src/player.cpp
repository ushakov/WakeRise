#include "player.h"

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

const double kStartSoundAt = 0.7;

SoftwareSerial player_serial(8, 9);  // RX, TX
DFRobotDFPlayerMini player;
bool player_started = false;

void player_begin(){
  player_serial.begin(9600);
  if (!player.begin(
          player_serial)) {  // Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin"));
    while (true) {
      delay(0);  // Code to compatible with ESP8266 watch dog.
    }
  }
  player.volume(0);  // Set volume value. From 0 to 30
  player_started = false;
}

void player_set(double timepos) {
  if (timepos < kStartSoundAt) {
    return;
  }
  if (!player_started) {
    player.volume(0);
    player.play(1);
    player.loop(2);
    player.start();
    player_started = true;
    Serial.println("Player starting");
  }
  timepos = (timepos - kStartSoundAt) / (1 - kStartSoundAt);
  int volume = int(timepos * 30);
  player.volume(volume);
  Serial.print("Player volume set to "); Serial.println(volume);
}

void player_off() {
    player.stop();
    player_started = false;
}


// void printDetail(uint8_t type, int value) {
//   switch (type) {
//     case TimeOut:
//       Serial.println(F("Time Out!"));
//       break;
//     case WrongStack:
//       Serial.println(F("Stack Wrong!"));
//       break;
//     case DFPlayerCardInserted:
//       Serial.println(F("Card Inserted!"));
//       break;
//     case DFPlayerCardRemoved:
//       Serial.println(F("Card Removed!"));
//       break;
//     case DFPlayerCardOnline:
//       Serial.println(F("Card Online!"));
//       break;
//     case DFPlayerUSBInserted:
//       Serial.println("USB Inserted!");
//       break;
//     case DFPlayerUSBRemoved:
//       Serial.println("USB Removed!");
//       break;
//     case DFPlayerPlayFinished:
//       Serial.print(F("Number:"));
//       Serial.print(value);
//       Serial.println(F(" Play Finished!"));
//       break;
//     case DFPlayerError:
//       Serial.print(F("DFPlayerError:"));
//       switch (value) {
//         case Busy:
//           Serial.println(F("Card not found"));
//           break;
//         case Sleeping:
//           Serial.println(F("Sleeping"));
//           break;
//         case SerialWrongStack:
//           Serial.println(F("Get Wrong Stack"));
//           break;
//         case CheckSumNotMatch:
//           Serial.println(F("Check Sum Not Match"));
//           break;
//         case FileIndexOut:
//           Serial.println(F("File Index Out of Bound"));
//           break;
//         case FileMismatch:
//           Serial.println(F("Cannot Find File"));
//           break;
//         case Advertise:
//           Serial.println(F("In Advertise"));
//           break;
//         default:
//           break;
//       }
//       break;
//     default:
//       break;
//   }
// }
