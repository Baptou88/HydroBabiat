#ifndef __BASICCONTROLLER_H__
#define __BASICCONTROLLER_H__

#include <Arduino.h>
#include "main.h"
#include <IController.h>


enum etangState{
    UNDIFINED,
    VIDER,
    REMPLIR
};
String etangStateToString(etangState st);
class basicController: public IController
{
private:
    etangState previousEtat = REMPLIR;
    bool doChange = false;
public:
    etangState etat = REMPLIR;
    int niveauMin = 20;
    int niveauMax = 80;

    int vanneMin = 0;
    int vanneMax = 80;

    
    basicController(/* args */);
    ~basicController();

    void startMode() override;

    //void startMode() override;
    void endMode()  override;

    void loop(void);

    unsigned long tempsRemplissage = 0;
    unsigned long tempsvidage = 0;
    unsigned long _millis = 0;

};



#endif // __BASICCONTROLLER_H__