
#include <SoftSerial.h>
#include <Tiny4kOLED.h>

#define SERIAL_PIN 1
#define I2C_ADDRESS 0x3C

SoftSerial MySerial(SERIAL_PIN, SERIAL_PIN);

char buf[30];
int bufInd = 0;
char prevBuf[30];

void setup() {
  MySerial.begin(9600);
  oled.begin();
  oled.clear();
  oled.on();
  oled.switchRenderFrame();
}

void loop() {
  if(MySerial.available()) {
    char c = MySerial.read();
    if (c == 0xA) { // "\n"
      if (!strsEqual(buf, prevBuf)) {
        strcpy(prevBuf, buf);
        oled.clear();
        oled.setFont(FONT8X16);
        oled.setCursor(8, 0);
        oled.print(buf);
        oled.switchFrame();
      }

      memset(buf, 0, sizeof(buf));
      bufInd = 0;
    } else {
      buf[bufInd++] = c;
    }
  }
}

bool strsEqual(char prev[30], char cur[30]) {
  for (int i = 0; i < 30; i++) {
    if (prev[i] != cur[i]) {
      return false;
    }
  }
  return true;
}
