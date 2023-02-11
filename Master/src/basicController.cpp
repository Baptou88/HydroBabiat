#include <Arduino.h>
#include <basicController.h>

String etangStateToString(etangState st){
    switch (st)
    {
    case VIDER:
        return "Vider";
        break;
    case REMPLIR:
        return "Remplir";
    default:
        return "etangState default";
        break;
    }
}


basicController::basicController(/* args */)
{
    type = typeController::basic;
    name = "Basic";
}

basicController::~basicController()
{
}

void basicController::startMode() {

}

void basicController::endMode() 
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
        vanne = vanneMax;
        tempsRemplissage = millis() - _millis;
        Serial.println("je change , j'allume");
        Serial.println((tempsRemplissage / 1000));
        break;
       case REMPLIR:
        tempsvidage = millis() - _millis;
        //digitalWrite(7, LOW);
        vanne = vanneMin;
        Serial.println("je change , j etaind");
        Serial.println((tempsvidage / 1000));
        break;
       
       default:
        break;
       }
       TelegramBot.sendTo((int64_t)CHAT_ID,"Basic Controller: changement mode:" + etangStateToString(etat) + "\n ratio vanne Appliqué: " + String(vanne) + " %");
       bufferActionToSend += "TURBINE:TargetVanne=" + (String)vanne;
       _millis = millis();
        output();
    }
}