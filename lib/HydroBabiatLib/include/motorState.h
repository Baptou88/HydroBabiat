#include <Arduino.h>
#ifndef  __MOTORSTATE_H__
#define  __MOTORSTATE_H__
enum MotorState{
    IDLE,
    OVERLOAD,
    INIT_POS_MIN,
    INIT_POS_MAX,
    WAIT_INIT,
    UNKOWN,
    FERMETURE_TOTALE,

    //rotation turbine trop Importante
    OVERSPEED,
    AUTOTUNE
};

String MotorStateToString(MotorState state);
#endif // __MOTORSTATE_H__