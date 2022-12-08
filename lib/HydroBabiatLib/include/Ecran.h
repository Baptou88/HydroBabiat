#include <Adafruit_SSD1306.h>
//#include "configuration.h"

#if !defined(__Ecran_)
#define __Ecran_

#if defined (ARDUINO_HELTEC_WIFI_LORA_32_V3) && defined (INTEGRATED_OLED)

    #pragma message "V3 oled"
    #undef RST_OLED
    #define RST_OLED 21
#elif defined (ARDUINO_HELTEC_WIFI_LORA_32_V3)
    #undef RST_OLED
    #define RST_OLED -1

#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V2)
    #pragma message "V2 oled"
#else
  #undef RST_OLED
  #define RST_OLED -1

#endif 

enum EcranState{
    EcranState_None,
    EcranState_IDLE,
    EcranState_Sleep
};

class Ecran
{
private:
    TwoWire* _iic;
    Adafruit_SSD1306* _display;
    EcranState _state = EcranState_None;
    unsigned long _millis = 0;
public:
    Ecran(TwoWire* iic = &Wire);
    ~Ecran();

    bool begin();
    EcranState getState();
    void loop();
    Adafruit_SSD1306* getDisplay();
    void setSleep();
    void wakeUp();

    // Draws a rounded progress bar with the outer dimensions given by width and height. Progress is
    // a unsigned byte value between 0 and 100
    // Provient de la library : https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/src/OLEDDisplay.h
    void drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress);
    void drawVProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress);
    void drawBVProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress);
    // Draw all Quadrants specified in the quads bit mask
    // Provient de la library : https://github.com/ThingPulse/esp8266-oled-ssd1306/blob/master/src/OLEDDisplay.h
    void drawCircleQuads(int16_t x0, int16_t y0, int16_t radius, uint8_t quads);
};





#endif // __Ecran_
