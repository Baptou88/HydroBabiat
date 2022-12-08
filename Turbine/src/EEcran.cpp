#include "Ecran.h"



Ecran::Ecran(TwoWire* iic)
{
    #pragma message RST_OLED
    _iic = iic;

    _display = new Adafruit_SSD1306 (128,64,_iic,RST_OLED);
    
}

Ecran::~Ecran()
{
}

bool Ecran::begin()
{
    
    
    Serial.println("RSTOLED " + String(RST_OLED));
    if (!_display->begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        return false;
        
    }
  
    _display->clearDisplay();
    _display->setTextSize(1);
    _display->setTextColor(SSD1306_WHITE);
    _display->setCursor(0,0);
    _display->println("Bonjour");
    _display->display();

    _state = EcranState_IDLE;
    _millis = millis();
    return true;
}

EcranState Ecran::getState()
{
    return _state;
}

void Ecran::loop()
{
    if (millis() > _millis + 60000 && _state == EcranState::EcranState_IDLE)
    {
        
        setSleep();
    }
    
}



void Ecran::setSleep()
{
    _display->ssd1306_command(SSD1306_DISPLAYOFF);
    _state = EcranState_Sleep;
}

void Ecran::wakeUp(){
    if (_state == EcranState::EcranState_Sleep)
    {
        _display->ssd1306_command(SSD1306_DISPLAYON);
        
    }
    _millis = millis();
    _state = EcranState_IDLE;
}
Adafruit_SSD1306* Ecran::getDisplay()
{
    if (_display == NULL)
    {
        Serial.println("NUUUUUULLLL");
    }
    
    return _display;
}
