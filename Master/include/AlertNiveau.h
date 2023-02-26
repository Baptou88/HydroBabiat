#include <Arduino.h>

#include "main.h"


#ifndef ALERTNIVEAU_H
#define ALERTNIVEAU_H

class AlertNiveau
{
private:
    bool initialized = false;
    int previousNiveau = 0;
    unsigned long _millis = 0;
    
public:
    AlertNiveau(/* args */);
    ~AlertNiveau();
    void updateNiveau(int niveau);
    void loop();

    bool active = false;
    int min = 0;
    int max = 100;
};

extern AlertNiveau AlertNiv;


#endif