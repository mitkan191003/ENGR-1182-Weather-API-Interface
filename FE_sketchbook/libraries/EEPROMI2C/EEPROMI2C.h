/*
  EEPROMI2C.h - Library for storing data in EEPROM.
*/
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define eepromi2cAddress 80

// eeprom write
template <class T> int eeWrite(int ee, const T& value)
{
    // Initialize write variable
    const byte* p = (const byte*)(const void*)&value;
    
    // Loop Counter
    unsigned int i;
    
    // Begin transmission
    Wire.beginTransmission(eepromi2cAddress);
    
    Wire.write((int)(ee >> 8)); // MSB
    
    Wire.write((int)(ee & 0xFF)); // LSB
    
    // write
    for (i = 0; i < sizeof(value); i++)
    {
        Wire.write(*p++);
    }
    
    Wire.endTransmission();
    
    return i;
}

// eeprom read
template <class T> int eeRead(int ee, T& value)
{
    // Initialize read variable
    byte* p = (byte*)(void*)&value;
    
    // Loop Counter
    unsigned int i;
    
    // Begin transmission
    Wire.beginTransmission(eepromi2cAddress);
    
    Wire.write((int)(ee >> 8)); // MSB
    
    Wire.write((int)(ee & 0xFF)); // LSB
    
    Wire.endTransmission();
    
    Wire.requestFrom(eepromi2cAddress,sizeof(value));
    
    for (i = 0; i < sizeof(value); i++)
    {
        if(Wire.available())
        {
            *p++ = Wire.read();
        }
    }
    
    return i;
}

