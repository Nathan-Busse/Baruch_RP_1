#include <SoftwareSerial.h>

// HC-05 digital pin assignments
int RX = 10;
int TX = 11;

SoftwareSerial BTSerial(RX, TX);  // RX | TX

// Boot message timing
unsigned long previousMillis = 0;
int messageIndex = 0;
bool bootComplete = false;

// Echo toggle
bool echoEnabled = true;

// Boot messages sequence
const char* bootMessages[] = {
  "Created by Nathan-Busse",
  "",
  "Baruch robot System core Layer: 1",
  "",
  "Wireless EEPROM flashing",
  "",
  "Enter the following listed AT commands:",
  "",
  "AT Command Reference",
  "-------------------------------------------------------",
  "| Command                | Description                |",
  "|------------------------|----------------------------|",
  "|====================== PART 1 =======================|",
  "| AT                     | Test connection            |",
  "| AT+VERSION?            | Get firmware version       |",
  "| AT+NAME?               | Get Bluetooth name         |",
  "| AT+PSWD?               | Get pairing password       |",
  "| AT+UART?               | Get baud rate              |",
  "| AT+ROLE?               | Get as slave               |",
  "| AT+ORGL                | Restore factory defaults   |",
  "|====================== PART 2 =======================|",
  "| AT                     | Test connection            |",
  "| AT+ROLE=0              | Set module to slave mode   |",
  "| AT+UART=115200,0,0     | Set baud to 115200, N, 1   |",
  "| AT+NAME=Baruch         | Set the Bluetooth name     |",
  "| AT+PSWD=Baruch         | Set pairing password       |",
  "| AT+RESET               | Restart module             |",
  "-------------------------------------------------------"
};
const int numMessages = sizeof(bootMessages) / sizeof(bootMessages[0]);

void setup() {
  Serial.begin(9600);
  BTSerial.begin(38400);
}

void loop() {
  unsigned long currentMillis = millis();

  // Sequentially print boot messages without delay()
  if (!bootComplete && currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    Serial.println(bootMessages[messageIndex]);
    messageIndex++;
    if (messageIndex >= numMessages) {
      bootComplete = true;
    }
  }

  // Handle Bluetooth → Serial
  if (BTSerial.available()) {
    char c = BTSerial.read();
    if (echoEnabled) Serial.write(c);
  }

  // Handle Serial → Bluetooth with command filter
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equalsIgnoreCase("echo on")) {
      echoEnabled = true;
      Serial.println("🔊 Echoing enabled.");
    } else if (input.equalsIgnoreCase("echo off")) {
      echoEnabled = false;
      Serial.println("🔇 Echoing disabled.");
    } else {
      BTSerial.println(input);  // Send AT command
    }
  }
}
