#ifndef __ROTARYENCODEUR_H__
#define __ROTARYENCODEUR_H__
#include <digitalInput.h>

class rotaryEncodeur
{
private:
    digitalInput _r;
    digitalInput _l;

public:
    rotaryEncodeur();
    rotaryEncodeur(digitalInput left,digitalInput right);
    ~rotaryEncodeur();
    void loop();
    bool isClockwize();
    bool isCounterClockwize();
};

rotaryEncodeur::rotaryEncodeur()
{

}
rotaryEncodeur::rotaryEncodeur(digitalInput left,digitalInput right)
{
    _r = right;
    _l = left;
}

rotaryEncodeur::~rotaryEncodeur()
{
}

void rotaryEncodeur::loop()
{
    _r.loop();
    _l.loop();
}

bool rotaryEncodeur::isClockwize()
{
    if (_r.frontMontant() && _l.isReleased())
    {
        return true;
    }
    return false;
    
}

bool rotaryEncodeur::isCounterClockwize()
{
    return false; //TODO implement
}

#endif // __ROTARYENCODEUR_H__