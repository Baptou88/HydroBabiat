#include "IController.h"
#include "Arduino.h"

IController::IController(/* args */)
{
}

IController::~IController()
{
}

void IController::output()
{
    int out = map(vanne,0,100,0,255);
    ledcWrite(1,out);
}