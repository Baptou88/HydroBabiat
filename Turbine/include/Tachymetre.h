#ifndef __TACHYMETRE_H__
#define __TACHYMETRE_H__

#include <Arduino.h>

class Tachymetre
{
private:
  volatile  long _micro = 0;
  volatile  long _previousMicro = 0;
  volatile  long _dt = 0;
  volatile int ticks = 0;
  long delaiAntiRebond=200;
  long _timeout = 0;

  bool isTimeOut();
public:
  Tachymetre();
  void Tick();
  float getRPM();
  float getHz();

  //Duree en microsecond a partir de laquelle la frequence de rotation est dite nulle
  void setTimeout(long us);
};
#endif // __TACHYMETRE_H__