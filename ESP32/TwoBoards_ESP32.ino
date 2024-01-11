#include <HT1632.h>
#include <BigNumbers.h>
#include <BluetoothSerial.h>
#include "RTC.h"

#define LED_BUILTIN 2

int pinCLK = 27; // white
int pinCS = 14; // green
int pinWR = 13; // blue
int pinDATA = 12; // yellow

BluetoothSerial SerialBT;
String msg;
uint8_t sec;
int Intensity = 8;

byte digit1, digit2;
int CurrSecond = 0, LastSecond = 0;

void drawBigDigit(byte num, uint8_t y) { // y= 0, 32
  switch (num) {
    case 0:
      HT1632.drawImage(IMG_BIG_0, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 1:
      HT1632.drawImage(IMG_BIG_1, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 2:
      HT1632.drawImage(IMG_BIG_2, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 3:
      HT1632.drawImage(IMG_BIG_3, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 4:
      HT1632.drawImage(IMG_BIG_4, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 5:
      HT1632.drawImage(IMG_BIG_5, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 6:
      HT1632.drawImage(IMG_BIG_6, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 7:
      HT1632.drawImage(IMG_BIG_7, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 8:
      HT1632.drawImage(IMG_BIG_8, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
    case 9:
      HT1632.drawImage(IMG_BIG_9, IMG_BIG_WIDTH, IMG_BIG_HEIGHT, y, 0);
      break;
  }
}

void setup () {
  Serial.begin(115200);
  Serial.println("setup start");
  SerialBT.begin("SwimClock32");  //Bluetooth device name
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
    while(true) {}
  } else {
    adjustRTC(0);
    Serial.println("RTC ready");
  }	  

  HT1632.begin(pinCS, pinWR, pinDATA, pinCLK);
  HT1632.setBrightness(Intensity);
  Serial.println("Setup end");
}

void loop () {
  CurrSecond = second_now();
  if (CurrSecond != LastSecond) { // every second
    HT1632.clear();
    LastSecond = CurrSecond;
    digit1 = LastSecond % 10;
    digit2 = (LastSecond / 10) % 10;
    drawBigDigit(digit2, 0); //
    drawBigDigit(digit1, 32);
    //Serial.print(digit2);
    //Serial.println(digit1);
    HT1632.render();
  } // every second
  if (SerialBT.available()) {
    msg = SerialBT.readStringUntil('\n');
    Serial.println(msg);
    switch (msg[0]) {
      case '+':
        Intensity = (Intensity == 12 ? Intensity : Intensity + 1);
		HT1632.setBrightness(Intensity);
        break;
      case '-':
        Intensity = (Intensity == 1 ? Intensity : Intensity - 1);
		HT1632.setBrightness(Intensity);
        break;
      case '#':
        sec = (msg[1] - '0') * 10 + (msg[2] - '0');
        Serial.print("Reset to ");
        Serial.println(sec);
        adjustRTC(sec);
        break;
    }
  }
  delay(25);
} // loop
