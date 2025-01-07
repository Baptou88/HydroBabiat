#include "digitalInput.h"


unsigned long digitalInput::pressedTime(){
    if (isPressed())
    {
        return millis() -pressedSince ;
    }
    return 0;
    
}
bool digitalInput::getState(){
    return _state;
}
digitalInput::digitalInput(uint8_t pin, uint8_t mode)
{
   _pin = pin;
   _mode = mode; 
   pinMode(_pin , _mode);
}

bool digitalInput::begin()
{
    if (_pin == -1  )
    {
        return false;
    }
    pinMode(_pin , _mode);
    loop();
    return true;
}

void digitalInput::loop(){
    
    int reading  = digitalRead(_pin);
    // if (reading != _previousState) {
    //     // reset the debouncing timer
    //     lastDebounceTime = millis();
    // }
    if ((millis() - lastDebounceTime) > _debonceTime) {
        lastDebounceTime = millis();
        if (reading != _previousState && reading == LOW)
        {
           pressedSince = millis();
        }
        
        _previousState = _state;
        _state = reading;
    }
      
}
bool digitalInput::isPressed(){
    if (_mode == INPUT_PULLUP)
    {
        if (_state == LOW)
        {
            return true;
        }
        return false;
    }else //if (_mode == INPUT_PULLDOWN)
    {
        if (_state == LOW)
        {
            return false;
        }
        return true;
    }
    return _state;
}
bool digitalInput::isReleased(){
    if (_mode == INPUT_PULLUP)
    {
        if (_state == HIGH)
        {
            return true;
        }
        return false;
    }else //if (_mode == INPUT_PULLDOWN)
    {
        if (_state == HIGH)
        {
            return false;
        }
        return true;
    }
    return _state;
}
bool  digitalInput::frontDesceandant(){
    return (_previousState == HIGH && _state == LOW); 
}
bool  digitalInput::frontMontant(){
    return (_previousState == LOW && _state == HIGH); 
}
digitalInput::~digitalInput()
{
}
