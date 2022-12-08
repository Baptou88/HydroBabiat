#ifndef __TIMER_H__
#define __TIMER_H__

class Timer
{
private:
  int _intervall = 0;
  unsigned long _millis = 0;
public:
  Timer(int intervall);
  Timer();
  ~Timer();
  void setIntervall(int intervall);
  bool isOver(bool single = true);
  void reset();
};

#endif // __TIMER_H__