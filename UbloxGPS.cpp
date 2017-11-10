/******************************************************************************
UbloxGPS.cpp
Library for Ublox SAM M8Q GPS on TLog (but Ublox commands should be general to all Ublox with >15 firmware)
Bobby Schulz @ Northern Widget LLC
11/10/2017
https://github.com/NorthernWidget/UbloxGPS

The SAM M8Q is a self contained GPS unit developed by Ublox, this unit interfaces to the data logger via I2C
and provides the ability to obtain location, altitude, and time information using NAVSTAR L1 band GPS satellite system.
This device allows for the logger to determine its location for the purpose of data interpretation and recording, as well as
radio network generation (to be developed)

"And into the forest I go, to lose my mind and find my soul"
-John Muir

Distributed as-is; no warranty is given.
******************************************************************************/

#include <Wire.h> // Wire library is used for I2C
#include <math.h>

UbloxGPS::UbloxGPS(int _ADR)
// Base library type I2C
{
	ADR = _ADR;
}

void UbloxGPS::Begin(void)
{
	Wire.begin(); // Arduino Wire library initializer
	//FIX write values to EEPROM using ifdef??
}

int UbloxGPS::SendUBX(unsigned char Message[], int Length, int ADR) {
  Wire.beginTransmission(ADR);
  for(int i = 0; i < Length; i+=2){
      Wire.write(Message[i]);
      Wire.write(Message[i + 1]);
    }
    return Wire.endTransmission();
}


int UbloxGPS::UBX(int Class, int ID, char Payload[], int Length) {
  // int PayloadLength = 0;
  // int PayloadBytes = 0;
  // if(Payload != 0) { //Check payload is not 0 so log10() does not freak out
  //  PayloadLength = floor(log10(abs(Payload))) + 1; //Find length of Payload in bits
  //  PayloadBytes = ceil(float(PayloadLength)/float(8)); //Find the length of Payload in bytes
  // }

  unsigned char Message[Length + 8] = {0}; //Initiialize message range to calculate the checksum, length is length of payload plus 4 bytes for CLASS, ID, LENGTH, and Start identifier
  int MsgLen = sizeof(Message);
  unsigned char CheckSumA = 0;
  unsigned char CheckSumB = 0;

  // long PayloadTemp = Payload;
  int y = Length;
  for(int x = MsgLen - 2; x > 5; x--) {   //Place payload bytes into message buffer, starting at rightmost byte
    Message[x] = Payload[y];
//    Serial.println(y);
    y--;
  }

  Message[0] = 0xB5;
  Message[1] = 0x62;
  Message[2] = Class;
  Message[3] = ID;
  Message[4] = (Length & 0xFF); //Use little-endian
  Message[5] = (Length & 0xFF00) >> 8;
//  Serial.print("i Values = \n");
  for(int i = 2; i < MsgLen - 2; i++) {
    CheckSumA = CheckSumA + Message[i];
    CheckSumB = CheckSumB + CheckSumA;
//    Serial.println(i);
  }

  Message[MsgLen - 1] = CheckSumB;
  Message[MsgLen - 2] = CheckSumA;
//  Serial.println(MsgLen);
//  Serial.println(Message[0], HEX);
//  Serial.print('\n');
//  for(int i = 0; i < MsgLen; i++) {
//    Serial.println(Message[i], HEX);
//  }
//  Serial.print("\n\n");
  int Result = SendUBX(Message, MsgLen, ADR);
  return Result;
}

int UbloxGPS::SleepTime(unsigned long Time){
  unsigned long Time_ms = Time*1000;

  unsigned char TempPayload[8] = {0};
  TempPayload[4] = 0x02; //Set BACKUP = ON
//  Serial.print('\n');
//  for(int i = 4; i > 0; i--) {
//    TempPayload[i] = Time_ms & 0xFF;
//    Time_ms = Time_ms >> 8;
//    Serial.println(Time_ms);
//  }
//  Serial.print("\n\n");

//  Serial.print('\n');
  for(int i = 0; i < 4; i++) {
    TempPayload[i] = Time_ms & 0xFF;
    Time_ms = Time_ms >> 8;
//    Serial.println(Time_ms);
  }
//  Serial.print("\n\n");

//  Serial.print('\n');
  for(int i = 0; i < sizeof(TempPayload); i++) {
//    Serial.println(TempPayload[i], HEX);
  }
//  Serial.print("\n\n");

//  char Message[16] = {0};
//  Serial.print("Payload size = ");
//  Serial.println(sizeof(TempPayload));
  return UBX(0x02, 0x41, TempPayload, sizeof(TempPayload));
}

int UbloxGPS::PowerSaveMode(int Mode){	//options for MODE 0, 1, 4
	unsigned char TempPayload[2] = {0};
  	TempPayload[0] = 0x00;
  	TempPayload[1] = Mode; //Set BACKUP = ON

  	return UBX(0x06, 0x11, TempPayload, sizeof(TempPayload));
}

int UbloxGPS::WriteConstants(void)	//Write constants for messages to EEPROM
{

}





