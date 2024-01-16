// 60 Clock with 3 rows max7219 and BT

#define NUM_MAX 12  // 4 cols on 3 rows
#define LED_BUILTIN 2

// ESP32 pins
#define DIN_PIN 19  // MOSI  green line
#define CS_PIN 5    // FLASH blue line
#define CLK_PIN 18  // SCLK  yellow line
#define BUZZZER_NOTE 220

#include "BluetoothSerial.h"
#include "max7219.h"
#include "RTC.h"
#include "fonts.h"
#include <toneAC.h>

BluetoothSerial SerialBT;
String msg;
uint8_t sec;
int Intensity = 1;
int CurrSecond = 0, LastSecond = 0;
bool SwimSet = false;
int intervalSS, timeSS, remainTimeSS;
bool fontwide = false;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("SwimClock");  //Bluetooth device name
  init7219();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Wire.begin();
  Wire.beginTransmission(RTC_ADDRESS);
  if (Wire.endTransmission() != 0) {
    Serial.println("Couldn't find RTC");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    while (true) {}
  } else {
    adjustRTC(0);
    Serial.println("start");
  }
}

void loop() {
  CurrSecond = second_now();
  if (CurrSecond != LastSecond) {  // every second
    if (SwimSet) {
      LastSecond = CurrSecond;
      clr();
      showDigit((LastSecond / 10) % 10, 0);
      showDigit(LastSecond % 10, 1);
      refreshAll();
      remainTimeSS--;
      if (remainTimeSS == 0) {
        remainTimeSS = timeSS;
        intervalSS--;
        delay(600);
        toneAC(BUZZZER_NOTE, 4, 300, false);
        toneAC();
        delay(100);
        adjustRTC(0);
        if (intervalSS == 1) {
          SwimSet = false;
        }
      }
    }
    clr();
    LastSecond = CurrSecond;
    showDigit((LastSecond / 10) % 10, 0);  //
    showDigit(LastSecond % 10, 1);
    refreshAll();
  }  // every second
  if (SerialBT.available()) {
    msg = SerialBT.readStringUntil('\n');
    Serial.print("Bluetooth message: ");
    Serial.println(msg);
    switch (msg[0]) {
      case '+':
        Intensity = (Intensity == 12 ? Intensity : Intensity + 1);
        sendCmdAll(CMD_INTENSITY, Intensity);
        break;
      case '-':
        Intensity = (Intensity == 1 ? Intensity : Intensity - 1);
        sendCmdAll(CMD_INTENSITY, Intensity);
        break;
      case '#':
        sec = (msg[1] - '0') * 10 + (msg[2] - '0');
        Serial.print("Reset to ");
        Serial.println(sec);
        adjustRTC(sec);
        break;
      case 'S':
        intervalSS = (msg.substring(1, msg.indexOf(':'))).toInt();
        timeSS = (msg.substring(msg.indexOf(':') + 1)).toInt();
        remainTimeSS = timeSS;
        Serial.print("intervalSS: ");
        Serial.println(intervalSS);
        Serial.print("timeSS: ");
        Serial.println(timeSS);
        delay(500);
        toneAC(BUZZZER_NOTE, 4, 250, false);
        adjustRTC(0);
        SwimSet = true;
        break;
      case 'F':
        if (msg[1] == '+') {
          fontwide = true;
        } else {
          fontwide = false;
        }
        break;
    }
    refreshAll();
  }
  delay(25);
}

void showDigit(int digit, int offset) {
  //Serial.print("digit:");
  //Serial.println(digit);
  if (fontwide) {
    for (int i = 0; i < 42; i++) {
      setCol(colIndex14x24[offset][i], font14x24[digit][i]);
      //Serial.print(colIndex14x24[offset][i]);
      //Serial.print(" ");
      //Serial.println(font14x24[digit][i]);
    }
  } else {
    for (int i = 0; i < 36; i++) {
      setCol(colIndex12x24[offset][i], font12x24[digit][i]);
      //Serial.print(colIndex12x24[offset][i]);
      //Serial.print(" ");
      //Serial.println(font12x24[digit][i]);
    }
  }
}
