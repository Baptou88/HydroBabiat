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
    FERMETURE_TOTALE
};

String MotorStateToString(MotorState state);
#endif // __MOTORSTATE_H__