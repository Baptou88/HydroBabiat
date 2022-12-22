#ifndef __PIDCONTROLLER_H__
#define __PIDCONTROLLER_H__

#include "Arduino.h"
#include "IController.h"

class PIDController: public IController
{
private:
    unsigned long _millis = 0;
    // intervalle en ms
    int intervallCalcul = 10000;

    
    

    float sommeErreurs = 0;
    float erreurPrecedente = 0;

    void calculPid(void);
public:

    float  kp = 3;
    float  ki = .01f;
    float kd = .01f;

//pourcentage cible du niveau de l'etang
    int targetEtang = 75;
    
    PIDController(/* args */);
    ~PIDController();

    void loop(void);

};



#endif // __PIDCONTROLLER_H__