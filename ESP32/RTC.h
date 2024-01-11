#include <Wire.h>

#define RTC_ADDRESS 0x68

uint8_t bin2bcd(uint8_t val) {
  return val + 6 * (val / 10);
}
uint8_t bcd2bin(uint8_t val) {
  return val - 6 * (val >> 4);
}

void adjustRTC(uint8_t seconds = 0) {
  Wire.beginTransmission(RTC_ADDRESS);
  Wire.write((byte)0);  // start at location 0
  Wire.write(bin2bcd(seconds));
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
