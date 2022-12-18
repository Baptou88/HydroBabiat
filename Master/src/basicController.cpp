#include <Arduino.h>
#include <basicController.h>

basicController::basicController(/* args */)
{
    type = typeController::basic;
    name = "Basic";
}

basicController::~basicController()
{
}

void basicController::loop(void)
{
    
    switch (etat)
    {
    case VIDER:
        if (niveau < niveauMin && !doChange)
        {
            doChange = true;
            etat = REMPLIR;
        }
        
        break;
    case REMPLIR:
        if (niveau > niveauMax && !doChange)
        {
            doChange  = true;
            etat =  VIDER;
        }
        break;
    default:
        break;
    }
    if (doChange)
    {
        doChange = false;
        
       switch (etat)
       {
       case VIDER:
        //digitalWrite(7, HIGH);
        vanne = 100;
        tempsRemplissage = millis() - _millis;
        Serial.println("je change , j'allume");
        Serial.println((tempsRemplissage / 1000));
        break;
       case REMPLIR:
        tempsvidage = millis() - _millis;
        //digitalWrite(7, LOW);
        vanne = 0;
        Serial.println("je change , j etaind");
        Serial.println((tempsvidage / 1000));
        break;
       
       default:
        break;
       }
       _millis = millis();
        output();
    }
}