/*
 * I2C Scanner – finds all devices on the bus
 * Upload this to your Arduino, open Serial Monitor (115200 baud),
 * and check which addresses appear.
 */

#include <Wire.h>

void setup() {
  Wire.begin();                // join I2C bus as master
  Serial.begin(115200);
  while (!Serial);             // wait for native USB boards

  Serial.println(F("\nI2C Scanner"));
  Serial.println(F("Scanning..."));
}

void loop() {
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print(F("I2C device found at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print(F("  (decimal: "));
      Serial.print(address);
      Serial.println(")");
      nDevices++;
    }
    else if (error == 4) {
      Serial.print(F("Unknown error at address 0x"));
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (nDevices == 0)
    Serial.println(F("No I2C devices found\n"));
  else
    Serial.println(F("Scan complete\n"));

  delay(5000);   // wait 5 seconds before next scan
}