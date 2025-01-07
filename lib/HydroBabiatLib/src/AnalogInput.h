#ifndef __ANALOGINPUT_H__
#define __ANALOGINPUT_H__
#include <Arduino.h>

class AnalogInput
{
private:
    byte _pin;
    uint16_t rawValue;
    float voltage;
public:
    float _a = 1;
    float _b = 0;
    AnalogInput(byte pin);
    AnalogInput(byte pin,float ka, float b);
    ~AnalogInput();

    void loop();
    void setAffineParam(float a, float b);
    float getValue();
    float getVoltage();
    void setZero();
    void changeAparam(float a);
    void begin();
    void calibrate(int numReadings = 20);
};

AnalogInput::AnalogInput(byte pin)
{
    
    AnalogInput(pin,1,0);

}

AnalogInput::AnalogInput(byte pin,float ka, float b)
{
    setAffineParam(ka,b);
    _pin = pin;
    //analogSetAttenuation(ADC_11db); 
    
}
void AnalogInput::begin(){
    analogReadResolution(12); 
    pinMode(_pin,INPUT_PULLUP);
    adcAttachPin( _pin);
}
AnalogInput::~AnalogInput()
{
}

void AnalogInput::loop()
{
    rawValue = analogRead(_pin);
}

void AnalogInput::setAffineParam(float a, float b)
{
    _a = a;
    _b = b;
}

float AnalogInput::getValue()
{
    return (rawValue + _b ) * _a  ;
}

float AnalogInput::getVoltage()
{
    return rawValue * 3.3 / 4095;
}

void AnalogInput::setZero()
{
    _b = -analogRead(_pin);
    // Serial.printf("a= %f b= %f\n",_a,_b);
    // _b = -_a * analogRead(_pin);
    // Serial.printf("a= %f b= %f\n",_a,_b);
}

void AnalogInput::changeAparam(float a)
{
    // Serial.printf("a= %f b= %f\n",_a,_b);
    // float dy = resultatAttendu - getValue();
    // float dx = (rawValue-(-_b/_a));
    // //_a = (resultatAttendu - _b)/ rawValue ;
    // float da = (resultatAttendu-getValue())/(rawValue-(-_b/_a));
    // float db = 
    // _a += da;
    // Serial.printf("a= %f b= %f\n",_a,_b);
    _a = a;
}

void AnalogInput::calibrate(int numReadings){
    int sum = 0;
    analogRead(_pin);
    for (size_t i = 0; i < numReadings; i++)
    {
        sum += analogRead(_pin);
        delay(5);

    }
    float average = sum / numReadings;
    _b = -average;
}

#endif // __ANALOGINPUT_H__