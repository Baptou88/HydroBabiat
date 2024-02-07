#include <Arduino.h>
#ifndef  __MOTORSTATE_H__
#define  __MOTORSTATE_H__
enum MotorState{
    //rotation turbine trop Importante
    TIMEOUT = -3,
    OVERSPEED = -2,
    OVERLOAD = -1,
    UNKOWN,
    IDLE,
    INIT_POS_MIN,
    INIT_POS_MAX,
    WAIT_INIT,
    FERMETURE_TOTALE,
    OUVERTURE_TOTALE,

    AUTOTUNE,
    CLOSEANDRESTORE
};

String MotorStateToString(MotorState state);
#endif // __MOTORSTATE_H__