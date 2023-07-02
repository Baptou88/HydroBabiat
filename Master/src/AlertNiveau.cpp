#include "AlertNiveau.h"

AlertNiveau::AlertNiveau()
{
}

AlertNiveau::~AlertNiveau()
{
}

void AlertNiveau::updateNiveau(int niveau)
{
    if (!active)
    {
        return;
    }
    
    if (millis() > _millis + 1000 * 30)
    {
        _millis = millis();

        if (niveau < min && previousNiveau > min && initialized)
        {
            Notifi.send("Alert: Niveau Min Déclanché " + String(niveau) + " " + String(previousNiveau) + " " + String(min) );
        }
        if (niveau > max && previousNiveau < max && initialized)
        {
            Notifi.send("Alert: Niveau Max Déclanché " + String(niveau) + " " + String(previousNiveau) + " " + String(max));
        }
        initialized = true;
        previousNiveau = niveau;
    }
    
}


AlertNiveau AlertNiv;