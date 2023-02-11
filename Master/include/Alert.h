#include "Arduino.h"
#include "main.h"
#ifndef __ALERT_H__
#define __ALERT_H__

class AlertClass
{
private:
public:
    bool active = false;
    int niveauMin = 0;
    int niveuMax = 100;

    
    AlertClass(/* args */);
    ~AlertClass();
    void loop();
    
};

extern AlertClass Alert;

#endif // __ALERTClass_H__