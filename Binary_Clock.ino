
#include <Wire.h>
#include "RTClib.h"

#define PIN_COL_A 8
#define PIN_COL_B 7
#define PIN_COL_C 6
#define PIN_ROW_1 12
#define PIN_ROW_2 11
#define PIN_ROW_4 10
#define PIN_ROW_8 9
#define PIN_BTN_HRS 2
#define PIN_BTN_MIN 3
#define PIN_BTN_SEC 4

int hrs = 0,
    mins = 0,
    secs = 0;

RTC_PCF8523 rtc;

unsigned long lastUpdate = 0;
unsigned long lastHrsPressed = 0;
unsigned long lastMinsPressed = 0;

void setup() {
  pinMode(PIN_COL_A, OUTPUT);
  pinMode(PIN_COL_B, OUTPUT);
  pinMode(PIN_COL_C, OUTPUT);
  pinMode(PIN_ROW_1, OUTPUT);
  pinMode(PIN_ROW_2, OUTPUT);
  pinMode(PIN_ROW_4, OUTPUT);
  pinMode(PIN_ROW_8, OUTPUT);
  pinMode(PIN_BTN_HRS, INPUT_PULLUP);
  pinMode(PIN_BTN_MIN, INPUT_PULLUP);
  pinMode(PIN_BTN_SEC, INPUT_PULLUP);
  
  if (!rtc.begin()) {
    while (1);
  }

  if (!rtc.initialized()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

//  Serial.begin(57600);
}

void loop() {
  unsigned long now = millis();

  if (digitalRead(PIN_BTN_HRS) == LOW) {
    if ((now - lastHrsPressed) > 250) {
      hrs = incrementHrs();
      lastHrsPressed = now;
    }
  } else if (digitalRead(PIN_BTN_MIN) == LOW) {
    if ((now - lastMinsPressed) > 250) {
      mins = incrementMins();
      lastMinsPressed = now;
    }
  } else if (digitalRead(PIN_BTN_SEC) == LOW) {
    secs = resetSeconds();
  }

  if ((now - lastUpdate) > 100) {
    lastUpdate = now;
    DateTime rtcNow = rtc.now();
    hrs = rtcNow.hour();
    mins = rtcNow.minute();
    secs = rtcNow.second();
//    Serial.print(hrs, DEC);
//    Serial.print(":");
//    Serial.print(mins, DEC);
//    Serial.print(":");
//    Serial.print(secs, DEC);
//    Serial.println();
  }

  displayTime(hrs, mins, secs);
}

int incrementHrs() {
  DateTime now = rtc.now();
  int newHr = now.hour() + 1;
  if (newHr > 23) newHr = 0;
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHr, now.minute(), now.second()));
  return newHr;
}

int incrementMins() {
  DateTime now = rtc.now();
  int newMin = now.minute() + 1;
  if (newMin > 59) newMin = 0;
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), newMin, now.second()));
  return newMin;
}

int resetSeconds() {
  DateTime now = rtc.now();
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), 0));
  return 0;
}

void displayTime(int hrs, int mins, int secs) {
  int digits[6] = {
    secs % 10,
    secs / 10,
    mins % 10,
    mins / 10,
    hrs % 10,
    hrs / 10
  };

  for (int i = 0; i < 6; i++) {
    // Blank the output
    digitalWrite(PIN_ROW_1, LOW);
    digitalWrite(PIN_ROW_2, LOW);
    digitalWrite(PIN_ROW_4, LOW);
    digitalWrite(PIN_ROW_8, LOW);

    // Set the active column
    digitalWrite(PIN_COL_A, (i >> 0) & 1);
    digitalWrite(PIN_COL_B, (i >> 1) & 1);
    digitalWrite(PIN_COL_C, (i >> 2) & 1);

    // Set the rows with the current digit
    digitalWrite(PIN_ROW_1, (digits[i] >> 0) & 1);
    digitalWrite(PIN_ROW_2, (digits[i] >> 1) & 1);
    digitalWrite(PIN_ROW_4, (digits[i] >> 2) & 1);
    digitalWrite(PIN_ROW_8, (digits[i] >> 3) & 1);

    delay(3);
  }
}

