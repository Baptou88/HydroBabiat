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
    if (vanne != previousVanne)
    {
        previousVanne = vanne;

        int out = map(vanne,0,100,0,255);
        ledcWrite(1,out);
        bufferActionToSend +=  "TURBINE:TargetVanne=" + (String)vanne + ";";
        Notifi.send("[IController] nouvelle consigne ouverture Vanne: " + (String)vanne + "%");
    }
    
}

void IController::startMode(){

}

void IController::endMode(){
    
}