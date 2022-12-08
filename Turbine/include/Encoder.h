



#ifndef __ENCODER_H__
#define __ENCODER_H__
#include <Arduino.h>

namespace EncoderVanne
{
    extern byte _pinCW;
    extern byte _pinCCW;

    void begin();
    void setPins(byte pinCW,byte pinCCW);
    IRAM_ATTR void  isr();
    int getPos();
    void setZeroPos();
} // namespace EncoderVanne




#endif // __ENCODER_H__