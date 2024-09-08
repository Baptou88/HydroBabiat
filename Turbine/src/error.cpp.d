#include <Arduino.h>

#define PIN_TACHY 21
int a = SDA;
bool pinTachyTriggered = false;

IRAM_ATTR void isrTachy(){
    pinTachyTriggered = true;
}
void setup(){
    pinMode(PIN_TACHY,INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(PIN_TACHY),isrTachy,FALLING);
}

void loop(){
    if (pinTachyTriggered)
    {
        pinTachyTriggered = false;
        Serial.printf("pin %i triggered" , PIN_TACHY);
    }
}