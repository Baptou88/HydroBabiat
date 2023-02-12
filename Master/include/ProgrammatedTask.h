#include <Arduino.h>

#if !defined(__PROGRAMMATEDTASK_H__)
#define __PROGRAMMATEDTASK_H__
class ProgrammatedTask
{
private:
    bool _active = false;
    
    
public:
    ProgrammatedTask(byte heures,byte minutes, String Name);
    ~ProgrammatedTask();
    void activate();
    void deactivate();
    void execute();
    bool isActive();
    String getHours();
    String getMinutes();
    String name;
    byte h;
    byte m;
    int targetVanne = 0;
    double deepsleep = 0;
};
#endif // __PROGRAMMATEDTASK_H__