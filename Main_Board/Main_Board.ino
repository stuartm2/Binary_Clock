
#include <Wire.h>
#include "RTClib.h"
#include <Adafruit_SSD1306.h>

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
#define PIN_SET_MODE 13
#define PIN_BRT 5
#define PIN_LIGHT_SENS A3

#define MODE_SET_TIME = 1
#define MODE_SET_DATE = 0

const char* MONTHS[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* ORDS[31] = {"st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th",
                        "th", "th", "th", "th", "th", "th", "th", "th", "th", "th",
                        "st", "nd", "rd", "th", "th", "th", "th", "th", "th", "th",
                        "st"};

int hrs = 0,
    mins = 0,
    secs = 0,
    y = 2000,
    m = 1,
    d = 1;

RTC_PCF8523 rtc;

#define OLED_RESET 4
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastUpdate = 0;
unsigned long lastHrsPressed = 0;
unsigned long lastMinsPressed = 0;
unsigned long lastSecsPressed = 0;

void setup() {
  Serial.begin(9600);
  Serial.print("Starting ...\n");

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
  pinMode(PIN_SET_MODE, INPUT);
  pinMode(PIN_BRT, OUTPUT);
  
  if (!rtc.begin()) {
    while (1);
  }

  if (!rtc.initialized()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  analogWrite(PIN_BRT, 0);
}

void loop() {
  unsigned long now = millis();
  bool setDate = digitalRead(PIN_SET_MODE) == LOW;

  if (digitalRead(PIN_BTN_HRS) == LOW) {
    if ((now - lastHrsPressed) > 250) {
      if (setDate) {
        d = incrementDays();
      } else {
        hrs = incrementHrs();
      }
      lastHrsPressed = now;
    }
  } else if (digitalRead(PIN_BTN_MIN) == LOW) {
    if ((now - lastMinsPressed) > 250) {
      if (setDate) {
        m = incrementMonths();
      } else {
        mins = incrementMins();
      }
      lastMinsPressed = now;
    }
  } else if (digitalRead(PIN_BTN_SEC) == LOW) {
    if (setDate) {
      if ((now - lastSecsPressed) > 250) {
        y = incrementYears();
        lastSecsPressed = now;
      }
    } else {
      secs = resetSeconds();
    }
  }

  if ((now - lastUpdate) > 200) {
    lastUpdate = now;
    DateTime rtcNow = rtc.now();
    hrs = rtcNow.hour();
    mins = rtcNow.minute();
    secs = rtcNow.second();
    y = rtcNow.year();
    m = rtcNow.month();
    d = rtcNow.day();

//    Serial.print(hrs, DEC);
//    Serial.print(":");
//    Serial.print(mins, DEC);
//    Serial.print(":");
//    Serial.print(secs, DEC);
//    Serial.println();
    displayExtra(y, m, d, hrs, mins, secs);
  }

  displayTime(hrs, mins, secs);

  //Serial.println(digitalRead(PIN_SET_MODE));

  int lightVal = analogRead(PIN_LIGHT_SENS);
  lightVal = constrain(lightVal, 300, 800);
  lightVal = map(lightVal, 400, 800, 10, 255);
  analogWrite(PIN_BRT, lightVal);
}

int incrementDays() {
  DateTime now = rtc.now();
  int newDay = now.day() + 1;
  if (newDay > 31) newDay = 1;
  rtc.adjust(DateTime(now.year(), now.month(), newDay, now.hour(), now.minute(), now.second()));
  return newDay;
}

int incrementMonths() {
  DateTime now = rtc.now();
  int newMonth = now.month() + 1;
  if (newMonth > 12) newMonth = 1;
  rtc.adjust(DateTime(now.year(), newMonth, now.day(), now.hour(), now.minute(), now.second()));
  return newMonth;
}

int incrementYears() {
  DateTime now = rtc.now();
  int newYear = now.year() + 1;
  if (newYear > 2100) newYear = 2000;
  rtc.adjust(DateTime(newYear, now.month(), now.day(), now.hour(), now.minute(), now.second()));
  return newYear;
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

void displayExtra(int y, int m, int d, int hrs, int mins, int secs) {
  char timeStr[10],
       dateStr[20];

  sprintf(timeStr, "%02d:%02d:%02d", hrs, mins, secs);
  sprintf(dateStr, "%d%s %s, %d", d, ORDS[d-1], MONTHS[m-1], y);
  Serial.print(d, DEC);
  Serial.print(ORDS[d-1]);
  Serial.print(", ");
  Serial.print(MONTHS[m-1]);
  Serial.print(" ");
  Serial.print(y, DEC);
  Serial.print("\n");
}
