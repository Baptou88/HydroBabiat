#include "motorState.h"

String MotorStateToString(MotorState state){
    switch (state)
    {
    case IDLE:
        return "IDLE";
        break;
    case OVERLOAD:
        return "OVERLOAD";
        break;
    case INIT_POS_MIN:
        return "INIT_POS_MIN";
        break;
    case INIT_POS_MAX:
        return "INIT_POS_MAX";
        break;
    case WAIT_INIT:
        return "WAIT_INIT";
        break;
    case UNKOWN:
        return "UNKOWN";
        break;
    case FERMETURE_TOTALE:
        return "FERMETURE_TOTALE";
        break;
    default:
        return "Default motorStateToString";
        break;
    }
}