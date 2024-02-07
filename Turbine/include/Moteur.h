#ifndef __MOTEUR_H__
#define __MOTEUR_H__
#include "Arduino.h"
#include "driver/mcpwm.h"
#include "PID_v2.h"
#include "digitalInput.h"
#include "Encoder.h"
#include "pidautotuner.h"

#include "motorState.h"
#include "main.h"

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
    MotorState _prevState = MotorState::WAIT_INIT;
    unsigned long stateTime = 0;

public:
    PID PIDMoteur = PID(&_position,&_speed,&_target,0,0,0,DIRECT);
    float maxItensiteMoteur = 9000;
    float maxItensiteMoteurOuverture = 15000;
    double _position = 0;
    int ouvertureMax = 6000;
    float IntensiteMoteur = 0;
    double _target = 0;
    
    Moteur(/* args */);
    ~Moteur();
    void begin(byte pinCW, byte pinCCW);
    void setSpeed(int speed);
    void setSpeedLimit(int vmin, int vmax);
    void loop();
    
    void autotune();
    int getTarget();
    float getTargetP();
    void setTarget(int Target);
    void setPosition(int Position);
    void updateIntensiteMoteur(float intensite);
    void setPID(float kp,float ki,float kd);
    void setEndstop(digitalInput *fcf, digitalInput *fco);
    MotorState getState(void);
    void setState(MotorState state);
    int getSpeed();

    void closeAndRestore(bool init = false);
};











#endif // __MOTEUR_H__