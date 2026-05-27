#include <SoftwareSerial.h>
const byte HC12RxdPin = 10;// Recieve Pin on HC12
const byte HC12TxdPin = 11;// Transmit Pin on HC12

SoftwareSerial RSDBlue(HC12TxdPin, HC12RxdPin); // RX - 11 | TX - 10
void setup()
{
 
  Serial.begin(9600);
  RSDBlue.begin(38400);//Depends on your HC12 Module, Set Baudrate of Serial Monitor to 9600
  Serial.println("Starting Configuration..");
  Serial.println("Enter AT Commands : ");
 
}
 
void loop()
{ 
  if (RSDBlue.available())
    Serial.write(RSDBlue.read());
 
  if (Serial.available())
    RSDBlue.write(Serial.read());
}
/*
AT
AT+ROLE=0
AT+NAME=RootSaid BT Module
AT+UART=57600,0,0
AT+POLAR=1,0

The HC05 Module will receive these commands. At this point, you can remove all of the connections, which concludes Part 1.
*/