#include "Encoder.h"
namespace EncoderVanne
{
    byte _pinCW = -1;
    byte _pinCCW = -1;

    volatile int position = 0;

    unsigned long _millis_debounce = 0;
    int debounceTime = 2;

    void setPins(byte pinCW, byte pinCCW)
    {
        _pinCCW = pinCCW;
        _pinCW = pinCW;
    }

    void begin()
    {

        if (_pinCCW != -1)
        {
            pinMode(_pinCCW, INPUT_PULLDOWN);
            attachInterrupt(digitalPinToInterrupt(_pinCCW), EncoderVanne::isr, CHANGE);
        }
        if (_pinCW != -1)
        {
            pinMode(_pinCW, INPUT_PULLDOWN);
        }
    }

    IRAM_ATTR void isr()
    {

        if (millis() > _millis_debounce + debounceTime)
        {
            _millis_debounce = millis();
            if (digitalRead(_pinCW) == digitalRead(_pinCCW))
            {
                position++;
            }
            else
            {
                position--;
            }
        }
    }

    int getPos()
    {
        return position;
    }

    void setPosition(int pos)
    {
        position = pos;
    }

    void setZeroPos()
    {
        position = 0;
    }

} // namespace EncoderVanne
