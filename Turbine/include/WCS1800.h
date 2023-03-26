#include <Arduino.h>

#ifndef WCS1800_H
#define WCS1800_H

class wcs1800
{
private:
    int _pin;
    float _sensitivity;
    float _zeroOffset;
    float _vRef;
public:
    wcs1800(int pin,float sensitivity,float zeroOffset, float vRef);
    ~wcs1800();

    void setSensitivity(float sensitivity);
    void setZeroOffset(float zeroOffset);

    float calibrate(int numReadings = 20);

    float getCurrent();
};




#endif