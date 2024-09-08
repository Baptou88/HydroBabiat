/*
Classe qui permet de connaitre l'evolution du niveau de l'etang
*/
#include <Arduino.h>

class NiveauDerivee
{
private:
    unsigned long lastMillis =  -1;
    unsigned long lastNiveau = -1;
    int niveau = -1;
    int intervallMs = 60 * 60 * 1000; //par default 1heure
public:
    NiveauDerivee(/* args */);
    ~NiveauDerivee();
    void update(int niveau);
};

NiveauDerivee::NiveauDerivee(/* args */)
{
}

NiveauDerivee::~NiveauDerivee()
{
}

void NiveauDerivee::update(int niveau)
{
    if (lastMillis == -1 && lastNiveau == -1)
    {
        //première acquisition
        lastMillis = millis();
        lastNiveau = niveau;
        this->niveau = niveau;
    }
    
}
