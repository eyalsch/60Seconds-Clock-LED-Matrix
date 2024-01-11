#include <HT1632.h>
#include <BigNumbers.h>
#include <Arduino.h>
#include <Wire.h>

int pinCLK = 4; // white
int pinCS = 5; // green
int pinWR = 6; // blue
int pinDATA = 7; // yellow

// DS1307 and DS3231
#define RTC_ADDRESS 0x68
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
  //Serial.begin(9600);
  //Serial.println("Start");
  pinMode(13, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(RTC_ADDRESS);
  if (Wire.endTransmission() != 0) {
    //Serial.println("Couldn't find RTC");
    digitalWrite(13, LOW);
    delay(500);
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
    digitalWrite(13, HIGH);
    while (1);
  }
  adjust();

  HT1632.begin(pinCS, pinWR, pinDATA, pinCLK);
  digitalWrite(13, LOW);
  HT1632.setBrightness(8);
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
} // loop

uint8_t bin2bcd(uint8_t val) { return val + 6 * (val / 10); }
uint8_t bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }

void adjust() {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((byte)0); // start at location 0
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(20));
  Wire.write(bin2bcd(0));
  Wire.write(bin2bcd(1));
  Wire.write(bin2bcd(1));
  Wire.write(bin2bcd(21));
  Wire.endTransmission();
}

byte second_now() {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((byte)0);
  Wire.endTransmission();

  Wire.requestFrom(RTC_ADDRESS, 1);
  uint8_t ss = bcd2bin(Wire.read() & 0x7F);

  return ss;
}
