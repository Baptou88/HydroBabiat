#include "WCS1800.h"

wcs1800::wcs1800(int pin,float sensitivity,float zeroOffset, float vRef)
{
    _pin = pin;
    _sensitivity = sensitivity;
    _zeroOffset = zeroOffset;
    _vRef = vRef;
}

wcs1800::~wcs1800()
{
}

void wcs1800::setSensitivity(float sensitivity)
{
    _sensitivity = sensitivity;
}

void wcs1800::setZeroOffset(float zeroOffset)
{
    _zeroOffset = zeroOffset;
}

float wcs1800::calibrate(int numReadings)
{
    int readings = 0;
    for (size_t i = 0; i < numReadings; i++)
    {   
        readings += analogRead(_pin);
        delay(5);
    }
    float average = readings / numReadings;
    Serial.println("[WCS1800] calibrate zero offset from " + (String)_zeroOffset + " to " + (String) average);
    _zeroOffset = average;
    return average;
}

float wcs1800::getCurrent()
{
    analogRead(_pin);
    int reading = analogRead(_pin);
    float voltage = reading * (_vRef / 4096);
    float current = (voltage - (_zeroOffset *(_vRef/4096))) / _sensitivity;

return current;
}
