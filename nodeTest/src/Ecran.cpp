#include "Ecran.h"



Ecran::Ecran(TwoWire* iic)
{
  
    _iic = iic;

    _display = new Adafruit_SSD1306 (128,64,_iic,RST_OLED);
    
}

Ecran::~Ecran()
{
}

bool Ecran::begin()
{
    
    
    //Serial.println("RSTOLED " + String(RST_OLED));
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
        //_millis = millis();
        setSleep();
    }
    
}

void Ecran::drawProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress){
    uint16_t radius = height / 2;
    uint16_t xRadius = x + radius;
    uint16_t yRadius = y + radius;
    uint16_t doubleRadius = 2 * radius;
    uint16_t innerRadius = radius - 2;

    
    drawCircleQuads(xRadius, yRadius, radius, 0b00000110);
    _display->drawFastHLine(xRadius, y, width - doubleRadius + 1,SSD1306_WHITE);
    _display->drawFastHLine(xRadius, y + height, width - doubleRadius + 1,SSD1306_WHITE);
    drawCircleQuads(x + width - radius, yRadius, radius, 0b00001001);

    uint16_t maxProgressWidth = (width - doubleRadius + 1) * progress / 100;

    _display->fillCircle(xRadius, yRadius, innerRadius,SSD1306_WHITE);
    _display->fillRect(xRadius + 1, y + 2, maxProgressWidth, height - 3,SSD1306_WHITE);
    _display->fillCircle(xRadius + maxProgressWidth, yRadius, innerRadius,SSD1306_WHITE);
}

void Ecran::drawVProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress){
    uint16_t radius = width / 2;
    uint16_t xRadius = x + radius;
    uint16_t yRadius = y + radius;
    uint16_t doubleRadius = 2 * radius;
    uint16_t innerRadius = radius - 2;

    
    drawCircleQuads(xRadius, yRadius, radius, 0b00000011);
    _display->drawFastVLine(x , y + radius, height - doubleRadius + 1,SSD1306_WHITE);
    _display->drawFastVLine(x + doubleRadius, y + radius , height - doubleRadius + 1,SSD1306_WHITE);
    drawCircleQuads(xRadius , y+height - radius, radius, 0b00001100);

    uint16_t maxProgressHeight = (height - doubleRadius + 1) * progress / 100;

    _display->fillCircle(xRadius, yRadius, innerRadius,SSD1306_WHITE);
    _display->fillRect(x + 2, yRadius + 1, width -3 ,maxProgressHeight ,SSD1306_WHITE);
    _display->fillCircle(xRadius , yRadius + maxProgressHeight, innerRadius,SSD1306_WHITE);
}

void Ecran::drawBVProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t progress){
    uint16_t radius = width / 2;
    uint16_t xRadius = x + radius;
    uint16_t yRadius = y + radius;
    uint16_t doubleRadius = 2 * radius;
    uint16_t innerRadius = radius - 2;

    
    drawCircleQuads(xRadius, yRadius, radius, 0b00000011);
    _display->drawFastVLine(x , y + radius, height - doubleRadius + 1,SSD1306_WHITE);
    _display->drawFastVLine(x + doubleRadius, y + radius , height - doubleRadius + 1,SSD1306_WHITE);
    drawCircleQuads(xRadius , y+height - radius, radius, 0b00001100);

    uint16_t maxProgressHeight = (height - doubleRadius + 1) * progress / 100;

    _display->fillCircle(xRadius , y + height - radius, innerRadius,SSD1306_WHITE);
    _display->fillRect(x + 2 , y + height - radius - maxProgressHeight, width -3 ,maxProgressHeight -1 ,SSD1306_WHITE);
    _display->fillCircle(xRadius  , y +  height - radius - maxProgressHeight, innerRadius,SSD1306_WHITE);
}

void Ecran::drawCircleQuads(int16_t x0, int16_t y0, int16_t radius, uint8_t quads) {
  int16_t x = 0, y = radius;
  int16_t dp = 1 - radius;
  while (x < y) {
    if (dp < 0)
      dp = dp + (x++) * 2 + 3;
    else
      dp = dp + (x++) * 2 - (y--) * 2 + 5;
    if (quads & 0x1) {
      _display->drawPixel(x0 + x, y0 - y,SSD1306_WHITE);
      _display->drawPixel(x0 + y, y0 - x,SSD1306_WHITE);
    }
    if (quads & 0x2) {
      _display->drawPixel(x0 - y, y0 - x,SSD1306_WHITE);
      _display->drawPixel(x0 - x, y0 - y,SSD1306_WHITE);
    }
    if (quads & 0x4) {
      _display->drawPixel(x0 - y, y0 + x,SSD1306_WHITE);
      _display->drawPixel(x0 - x, y0 + y,SSD1306_WHITE);
    }
    if (quads & 0x8) {
      _display->drawPixel(x0 + x, y0 + y,SSD1306_WHITE);
      _display->drawPixel(x0 + y, y0 + x,SSD1306_WHITE);
    }
  }
  if (quads & 0x1 && quads & 0x8) {
    _display->drawPixel(x0 + radius, y0,SSD1306_WHITE);
  }
  if (quads & 0x4 && quads & 0x8) {
    _display->drawPixel(x0, y0 + radius,SSD1306_WHITE);
  }
  if (quads & 0x2 && quads & 0x4) {
    _display->drawPixel(x0 - radius, y0,SSD1306_WHITE);
  }
  if (quads & 0x1 && quads & 0x2) {
    _display->drawPixel(x0, y0 - radius,SSD1306_WHITE);
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
