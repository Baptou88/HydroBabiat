#include "Tachymetre.h"

bool Tachymetre::isTimeOut()
{
  return ((micros()-_micro > _timeout) || (_previousMicro == 0));
}

Tachymetre::Tachymetre(){

}
float Tachymetre::getRPM(){
  if (isTimeOut()) {
    return 0;
  }  
  float rpm = 60*1E6/_dt;
  //Serial.println("[RPM] " + String(rpm,2)+ "   [dt] " + (String(_dt)));
  return rpm;
}

float Tachymetre::getHz()
{
  if (isTimeOut())
  {
    return 0;
  }
    return 1E6 / _dt;
  
}
void Tachymetre::setTimeout(long us){
  _timeout = us;
}

void Tachymetre::setDebounceTime(long us)
{
  delaiAntiRebond = us;
}

long Tachymetre::getDebounceTime()
{
  return delaiAntiRebond;
}

void Tachymetre::tick()
{
  _micro = micros();
  if (_micro - _previousMicro > delaiAntiRebond)
  {
    _dt = _micro - _previousMicro;
    _previousMicro = _micro;
  } else
  {
    ets_printf("Debonce %lu   %lu \n ", _micro-_previousMicro,delaiAntiRebond);
  }
  
}