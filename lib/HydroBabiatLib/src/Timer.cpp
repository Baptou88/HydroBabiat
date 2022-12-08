#include "Arduino.h"
#include "Timer.h"

Timer::Timer(int intervall)
{
  _intervall = intervall;
}
Timer::Timer(){

}
Timer::~Timer()
{
}

bool Timer::isOver(bool single){
  unsigned long temp = 0;
  temp = millis();
  if (temp>_millis + _intervall )
  {
    if (single)
    {
      _millis = temp;
    }
    
    return true;
  }
   return false;
}

void Timer::setIntervall(int intervall){
  _intervall = intervall;
}

void Timer::reset(){
  _millis = millis();
}