#include <Arduino.h>

#define PIN_TACHY 21

unsigned long _micro = 0;
unsigned long _previousMicro = 0;
unsigned long delaiAntiRebond = 5000; // 5 milliSecond
unsigned long _dt = 0;
long _timeout = 2E6; //Duree en microsecond a partir de laquelle la frequence de rotation est dite nulle

IRAM_ATTR void isrTachy(){
    _micro = micros();
    if (_micro - _previousMicro > delaiAntiRebond)
    {
        _dt = _micro - _previousMicro;
        _previousMicro = _micro;
    } else
    {
        ets_printf("Debonce %lu   %lu \n ", _micro - _previousMicro,delaiAntiRebond);
    }
}

//permet de savoir si la vitesse de rotation est nulle
bool isTimeOut()
{
  return ((micros()-_micro > _timeout) || (_previousMicro == 0));
}

float getRPM(){
  if (isTimeOut()) {
    return 0;
  }  
  float rpm = 60*1E6/_dt;
  //Serial.println("[RPM] " + String(rpm,2)+ "   [dt] " + (String(_dt)));
  return rpm;
}

float getHz()
{
  if (isTimeOut())
  {
    return 0;
  }
    return 1E6 / _dt;
  
}

void setup(){
    pinMode(PIN_TACHY,INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TACHY),isrTachy,FALLING);
}

void loop(){
    long rpm = getRPM();
    Serial.printf("Rotation:  %lu RPM" ,rpm);
    
}