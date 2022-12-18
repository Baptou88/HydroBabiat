

#include "Moteur.h"


Moteur::Moteur(/* args */)
{
}

Moteur::~Moteur()
{
}


void Moteur::begin(byte pinCW, byte pinCCW)
{
    _pinCCW = pinCCW;
    _pinCW = pinCW;
    mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0A,pinCCW);
    mcpwm_gpio_init(MCPWM_UNIT_0,MCPWM0B,pinCW);
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 1000;    //frequency,
    pwm_config.cmpr_a = 0;    		//duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    		//duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings

    PIDMoteur.SetMode(AUTOMATIC);
    PIDMoteur.SetOutputLimits(-100,100);
}

void Moteur::setSpeed(int speed){
    _speed = speed;
}

void Moteur::setSpeedLimit(int vmin, int vmax)
{
    _minSpeed = vmin;
    _maxSpeed = vmax;
}

void Moteur::setSpeedLimits(){
    if (_speed >= _maxSpeed)
    {
        _speed = _maxSpeed;
    }
    if (_speed <= -1 * _maxSpeed)
    {
        _speed = -1 * _maxSpeed;
    }

    if (_speed > 0 && _speed < _minSpeed) _speed = 0;
    
    if (_speed < 0 && _speed > -_minSpeed) _speed = 0;
    
    
    

}


void Moteur::setPID(float kp,float ki,float kd)
{
    PIDMoteur.SetTunings(kp,ki,kd);
}


void Moteur::setPosition(int Position){
    _position = Position;
}
int Moteur::getTarget()
{
    return _target;
}
float Moteur::getTargetP()
{
    return (_target / (float)ouvertureMax) *100;
}
void Moteur::setTarget(int Target)
{
    _target = Target;
}

void Moteur::setEndstop(digitalInput *fcf, digitalInput *fco)
{
    _fcf = fcf;
    _fco = fco;
}

void Moteur::updateIntensiteMoteur(float Intensite){
    IntensiteMoteur = Intensite;
}
void Moteur::loop(){
    switch (_state)
    {
    case IDLE:
        // if (_position > _target)
        // {
        //     _speed = -100;
        // } else if (_position < _target)
        // {
        //     _speed = 100;
        // } else 
        // {
        //     _speed = 0;
        // }
        
        PIDMoteur.Compute();

        setSpeedLimits();


        if (_speed>0)
        {
            ouvrirVanne(_speed);
        } else if (_speed<0)
        {
            fermeeVanne(abs(_speed));
        }else 
        {
            stopMoteur();
        }
        break;
    
    case OVERLOAD:
        mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_B);
        mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A);
        break;
    case INIT_POS_MIN:
        initPosMin();
        
        break;
    case INIT_POS_MAX:
        initPosMax();
        
        break;
    case WAIT_INIT:
        static unsigned long waiting_time = millis();
        
        
        if (millis() - waiting_time > 2000  )
        {
            _state= INIT_POS_MIN;
        }
        
        break;
    default:
        break;
    }
    
}

MotorState Moteur::getState(void)
{
    return _state;
}
void Moteur::setState(MotorState state)
{
    _state = state;
}


void Moteur::ouvrirVanne(int speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_B);
    mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A,speed);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}
void Moteur::fermeeVanne(int speed)
{
    mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A);
    mcpwm_set_duty(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_B,speed);
    mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
}
void Moteur::stopMoteur()
{
    mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_B);
    mcpwm_set_signal_low(MCPWM_UNIT_0,MCPWM_TIMER_0,MCPWM_OPR_A);
    //mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);
}
void Moteur::initPosMin(){
    static int state = -1;

    if (state == -1 && _fcf->isReleased())
    {
        
        state = 1;
    }
    if (state == -1 && _fcf->isPressed())
    {
       
        state = 0;
    }
    if (state == 0 && _fcf->isReleased())
    {
        
        state = 1;
    }
    
    if (state == 1 && _fcf->isPressed())
    {
        state =2;
    }
    switch (state)
    {
    case 0:
        //Ouvrir Vanne
        ouvrirVanne();
        break;
    case 1:
        //fermee Vanne
        fermeeVanne();
        break;
    case 2:
        //fin 
        stopMoteur();
        EncoderVanne::setZeroPos();
        _state = MotorState::IDLE;
    default:
        break;
    }
}
void Moteur::initPosMax(){
    static int state = -1;

    if (state == -1 && _fco->isReleased())
    {
        ouvrirVanne();
    } else
    {
        stopMoteur();
        ouvertureMax = _position;
        _state = MotorState::IDLE;
        _target = _position;

    }
    
    
}