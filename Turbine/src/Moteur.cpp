

#include "Moteur.h"


Moteur::Moteur(/* args */)
{
    PIDMoteur.SetSampleTime(50);
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
    
    
    // if (_speed != 0)
    // {
    //     Serial.println("speed Moteur " + (String)_speed);
    // }
    

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
    // gestion de la sur-Intensité moteur
    if (_target > _position)
    {
        // ouverture vanne
        if (IntensiteMoteur > maxItensiteMoteurOuverture)
        {
            setState(MotorState::OVERLOAD);
        }
    } else
    {
        //fermeturevanne 
        if (IntensiteMoteur > maxItensiteMoteur)
        {
            setState(MotorState::OVERLOAD);
        }
    }
    
    
    
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
        if (millis() > stateTime + 60000)
        {
            setState(MotorState::TIMEOUT);
            break;
        } 
        initPosMin();
        
        break;
    case INIT_POS_MAX:
        if (millis() > stateTime + 60000)
        {
            setState(MotorState::TIMEOUT);
            break;
        } 
        initPosMax();
        
        break;
    case WAIT_INIT:
        static unsigned long waiting_time = millis();
        
        
        if (millis() - waiting_time > 2000  )
        {
            _state= INIT_POS_MIN;
        }
        
        break;
    case FERMETURE_TOTALE:
        if (_fcf->isPressed())
        {
            stopMoteur();
            delay(10);
            _state = MotorState::IDLE;
            _target = _position;
            break;
            
        }
        
        fermeeVanne();

        break;
    case OUVERTURE_TOTALE:
        if (_fco->isPressed())
        {
            stopMoteur();
            delay(10);
            _state = MotorState::IDLE;
            _target = _position;
            break;
            
        }
        
        ouvrirVanne();
        break;
    case OVERSPEED:
        if (millis()>stateTime + 60000)
        {
            setState(MotorState::TIMEOUT);
            break;
        }
    
        if (_fcf->isPressed())
        {
            stopMoteur();
        } else
        {
            fermeeVanne();
        }
        
        
        break;
    case AUTOTUNE:
        autotune();
        break;
    case TIMEOUT:
        stopMoteur();
        break;
    default:
        break;
    }
    
}

void Moteur::autotune()
{
    PIDAutotuner tuner = PIDAutotuner();

    Serial.println("Starting autotune ");
    // Set the target value to tune to
    // This will depend on what you are tuning. This should be set to a value within
    // the usual range of the setpoint. For low-inertia systems, values at the lower
    // end of this range usually give better results. For anything else, start with a
    // value at the middle of the range.
    tuner.setTargetInputValue(2000);

    // Set the loop interval in microseconds
    // This must be the same as the interval the PID control loop will run at
    tuner.setLoopInterval(50000);

    // Set the output range
    // These are the minimum and maximum possible output values of whatever you are
    // using to control the system (Arduino analogWrite, for example, is 0-255)
    tuner.setOutputRange(-100, 100);

    // Set the Ziegler-Nichols tuning mode
    // Set it to either PIDAutotuner::ZNModeBasicPID, PIDAutotuner::ZNModeLessOvershoot,
    // or PIDAutotuner::ZNModeNoOvershoot. Defaults to ZNModeNoOvershoot as it is the
    // safest option.
    tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);

    // This must be called immediately before the tuning loop
    // Must be called with the current time in microseconds
    tuner.startTuningLoop(micros());
    
    // Run a loop until tuner.isFinished() returns true
    long microseconds;
    while (!tuner.isFinished()) {

        // This loop must run at the same speed as the PID control loop being tuned
        long prevMicroseconds = microseconds;
        microseconds = micros();

        // Get input value here (temperature, encoder position, velocity, etc)

        double input = EncoderVanne::getPos();
        Serial.println("position " + (String)input);

        // Call tunePID() with the input value and current time in microseconds
        double output = tuner.tunePID(input, microseconds);

        // Set the output - tunePid() will return values within the range configured
        // by setOutputRange(). Don't change the value or the tuning results will be
        // incorrect.
        //doSomethingToSetOutput(output);
        _speed = output;
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

        // This loop must run at the same speed as the PID control loop being tuned
        while (micros() - microseconds < 50000) delayMicroseconds(1);
    }

    // Turn the output off here.
    //doSomethingToSetOutput(0);
    stopMoteur();

    // Get PID gains - set your PID controller's gains to these
    double kp = tuner.getKp();
    double ki = tuner.getKi();
    double kd = tuner.getKd();
    Serial.printf("Autotune results: kp %f ki %f kd %f \n ",kp , ki,kd);

    _state = MotorState::IDLE;
}

MotorState Moteur::getState(void)
{
    return _state;
}
void Moteur::setState(MotorState state)
{
    if (state == MotorState::OVERSPEED || state == MotorState::FERMETURE_TOTALE)
    {
        this->_target = 0;
    }
    
    _prevState = _state;
    stateTime = millis();
    _state = state;
}

int Moteur::getSpeed()
{
    return _speed;
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
void Moteur::closeAndRestore(bool init )
{
    static byte state = 0;
    static int targetVanneToRestore = 0;
    static unsigned long _millis = 0;
    if (init)
    {
        state = 0;
        targetVanneToRestore = _target;
        _state = MotorState::CLOSEANDRESTORE;
        
    }
    if (state == 0)
    {
        fermeeVanne();
        if (_fcf->isPressed())
        {
            state = 1;
        }
        
    }
    if (state == 1) // attendre arret vitesse de rotation
    {
        if (tachy.getRPM() == 0)
        {
            state = 2;
            _millis = millis();
        }
        
    }
    if (state == 2) // attendre 10 sec supplementaire
    {
        if (millis()> _millis + 10000)
        {
            state = 3;
        }
        
    }
    if (state == 3)
    {
        calibrateADS(VOLTAGE_ADS_CHANNEL,30,10);
        calibrateADS(CURRENT_ADS_CHANNEL,30,10);
        state = 4;
    }
    
    if (state == 4) // restaurer l'ancien etat
    {
        _state = MotorState::IDLE;
        _target = targetVanneToRestore;

    }
    
    
    
    
    
}
void Moteur::initPosMin()
{
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
        delay(100);
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