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
  return 60*1E6/_dt;
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
void Tachymetre::Tick(){
  _micro = micros();
  if (_micro - _previousMicro > delaiAntiRebond)
  {
    _dt = _micro - _previousMicro;
    _previousMicro = _micro;
  }

}