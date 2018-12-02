#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


//DFRobotDFPlayerMini player;

SoftwareSerial bt(PIN3, PIN4);

bool waitOK() {
  char buf[10];
  int n = bt.readBytesUntil('\n', buf, 10);
  for (int i = 0; i < n; i++) {
    Serial.print(buf[i]);
  }
  Serial.println(" ---- ");
  if (n >= 3) {
    if (buf[n-3] == 'O' && 
        buf[n-2] == 'K' &&
        buf[n-1] == '\r') {
          Serial.println("OK found");
          return true;
    }
  }
  return false;
}

bool initSerial() {
  Serial.begin(9600);
  bt.begin(9600);
  for (int attempt = 0; attempt < 10; attempt++) {
    bt.println("AT");
    if (waitOK()) return true;
  }
  return false;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  if (!initSerial()) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  bt.println("AT+NAME?");
  while(waitOK());
  bt.println("AT+PIN?");
  while(waitOK());
  bt.println("AT+NAME=WakeRise");
  while(waitOK());
  bt.println("AT+PIN=0000");
  while(waitOK());
}

void loop() {
  if (bt.available() > 0) {
    char b = bt.read();
    // Serial.print("You sent ");
    Serial.print(b);
    // Serial.print(' ');
    // Serial.println(b, DEC);
  }
}