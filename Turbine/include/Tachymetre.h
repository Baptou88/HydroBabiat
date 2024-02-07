#ifndef __TACHYMETRE_H__
#define __TACHYMETRE_H__

#include <Arduino.h>

class Tachymetre
{
private:
   long _micro = 0;
   long _previousMicro = 0;
   long _dt = 0;

  /** delai en microSecond*/
  long delaiAntiRebond = 400;
  long _timeout = 0;

  bool isTimeOut();
public:
  Tachymetre();
  void Tick();
  float getRPM();
  float getHz();

  //Duree en microsecond a partir de laquelle la frequence de rotation est dite nulle
  void setTimeout(long us);

  void setDebounceTime(long us);
  long getDebounceTime();
};
#endif // __TACHYMETRE_H__