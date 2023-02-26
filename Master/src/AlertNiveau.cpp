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

        if (niveau < min && previousNiveau > min)
        {
            TelegramBot.sendTo(CHAT_ID,"Alert: Niveau Min Déclanché");
        }
        if (niveau > max && previousNiveau < max)
        {
            TelegramBot.sendTo(CHAT_ID,"Alert: Niveau Max Déclanché");
        }
        previousNiveau = niveau;
    }
    
}


AlertNiveau AlertNiv;