#include <Arduino.h>
#include <basicController.h>

String etangStateToString(etangState st)
{
    switch (st)
    {
    case UNDIFINED:
        return "UNDIFINED";
        break;
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

void basicController::startMode() 
{
    etat = etangState::UNDIFINED;
    previousEtat = etangState::UNDIFINED;
}

void basicController::endMode() 
{
}

void basicController::loop(void)
{

    // switch (etat)
    // {
    // case VIDER:
    //     if (niveau < niveauMin && !doChange)
    //     {
    //         doChange = true;
    //         etat = REMPLIR;
    //     }

    //     break;
    // case REMPLIR:
    //     if (niveau > niveauMax && !doChange)
    //     {
    //         doChange  = true;
    //         etat =  VIDER;
    //     }
    //     break;
    // default:
    //     break;
    // }
    if (etat == UNDIFINED && EtangStatus.dernierMessage == 0 && TurbineStatus.dernierMessage == 0)
    {
        return;
    }

    if (niveau < niveauMin)
    {
        etat = REMPLIR;
    }
    if (niveau > niveauMax)
    {
        etat = VIDER;
    }

    if (etat != previousEtat)
    {
        previousEtat = etat;
        doChange = true;
    }

    if (doChange)
    {
        doChange = false;

        switch (etat)
        {
        case VIDER:
            // digitalWrite(7, HIGH);
            vanne = vanneMax;
            tempsRemplissage = millis() - _millis;
            Serial.println("je change , j'allume");
            Serial.println((tempsRemplissage / 1000));
            break;
        case REMPLIR:
            tempsvidage = millis() - _millis;
            // digitalWrite(7, LOW);
            vanne = vanneMin;
            Serial.println("je change , j etaind");
            Serial.println((tempsvidage / 1000));
            break;

        default:
            break;
        }
        Notifi.send( "Basic Controller: changement mode:" + etangStateToString(etat) + "\n ratio vanne Appliqué: " + String(vanne) + " %");
        //bufferActionToSend += "TURBINE:TargetVanne=" + (String)vanne + ";";
        _millis = millis();
        output();
    }
}