#ifndef UbloxGPS_h
#define UbloxGPS_h

#include "Arduino.h"

class UbloxGPS
{
  public:
    UbloxGPS(int _ADR);
    UbloxGPS();
    int Begin(void);
    int UBX(int Class, int ID, char Payload[], int Length); 
    int SleepTime(unsigned long Time);
    int PowerSaveMode(int Mode);

  private:
    int ADR;
    int SendUBX(unsigned char Message[], int Length, int ADR); 
    int WriteConstants(void); //FIX
};

#endif