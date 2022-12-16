#ifndef __BASICCONTROLLER_H__
#define __BASICCONTROLLER_H__

#include <Arduino.h>
#include <IController.h>

enum etangState{
    VIDER,
    REMPLIR
};
class basicController: public IController
{
private:
    int niveauMin = 20;
    int niveauMax = 80;
    etangState etat = REMPLIR;
    bool doChange = false;
public:
    
    basicController(/* args */);
    ~basicController();
    
    void loop(void);
    unsigned long tempsRemplissage = 0;
    unsigned long tempsvidage = 0;
    unsigned long _millis = 0;

};



#endif // __BASICCONTROLLER_H__