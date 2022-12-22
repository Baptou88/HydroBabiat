#ifndef __ICONTROLLER_H__
#define __ICONTROLLER_H__

#include <Arduino.h>
#include "main.h"


enum typeController{
    manuel,
    basic,
    PID
};

class IController
{
private:
    /* data */
public:
    int niveau = 0;
    int vanne = 0;
    int previousVanne = 0;
    typeController type;
    IController(/* args */);
    ~IController();

    virtual void loop(void) = 0;
    void output();

    String name = "default";
};



#endif // __ICONTROLLER_H__