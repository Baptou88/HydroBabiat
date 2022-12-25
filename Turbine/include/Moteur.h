#ifndef __MOTEUR_H__
#define __MOTEUR_H__
#include "Arduino.h"
#include "driver/mcpwm.h"
#include "PID_v1.h"
#include "digitalInput.h"
#include "Encoder.h"

enum MotorState{
    IDLE,
    OVERLOAD,
    INIT_POS_MIN,
    INIT_POS_MAX,
    WAIT_INIT,
};

class Moteur
{
private:
    byte _pinCW = -1;
    byte _pinCCW = -1;
    digitalInput *_fcf = NULL;
    digitalInput *_fco = NULL;
    double _speed = 0;
    int _maxSpeed = 100;
    int _minSpeed = 32;
    void setSpeedLimits();
    void initPosMin();
    void initPosMax();
    void ouvrirVanne(int speed = 100);
    void fermeeVanne(int speed = 100);
    void stopMoteur();
    
    MotorState _state = MotorState::WAIT_INIT;
    PID PIDMoteur = PID(&_position,&_speed,&_target,0,0,0,DIRECT);

public:
    float maxItensiteMoteur = 9000;
    double _position = 0;
    int ouvertureMax = 5000;
    float IntensiteMoteur = 0;
    double _target = 0;
    
    Moteur(/* args */);
    ~Moteur();
    void begin(byte pinCW, byte pinCCW);
    void setSpeed(int speed);
    void setSpeedLimit(int vmin, int vmax);
    void loop();
    int getTarget();
    float getTargetP();
    void setTarget(int Target);
    void setPosition(int Position);
    void updateIntensiteMoteur(float intensite);
    void setPID(float kp,float ki,float kd);
    void setEndstop(digitalInput *fcf, digitalInput *fco);
    MotorState getState(void);
    void setState(MotorState state);
    String stateToString();
};











#endif // __MOTEUR_H__