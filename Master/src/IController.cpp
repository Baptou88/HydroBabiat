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
    ledcWrite(1,vanne);
}