#ifndef __MENU_H__
#define __MENU_H__

#include <Arduino.h>

#include <LList.h>
#include "menuItem.h"
#ifdef USE_SH1107
    #include <Adafruit_SH110X.h>
    #define ADAFRUIT_DISPLAY Adafruit_SH1107
    #define DISPLAY_WHITE SH110X_WHITE
    #define DISPLAY_BLACK SH110X_BLACK
#else
    #include "Adafruit_SSD1306.h"
    #define ADAFRUIT_DISPLAY Adafruit_SSD1306
    #define DISPLAY_WHITE SSD1306_WHITE
    #define DISPLAY_BLACK SSD1306_BLACK
#endif
//#include <../include/menuItem.h>
#ifdef USE_SH1107

#else

#endif
class menuItemBase;
class  menunu
{
private:
#ifdef USE_SH1107
    Adafruit_SH1107* _display;
#else
    Adafruit_SSD1306* _display;
#endif
public:
#ifdef USE_SH1107
    menunu(Adafruit_SH1107* display);
#else
    menunu(Adafruit_SSD1306* display);
#endif
    ~menunu();
    
    //menuItemBase *items = NULL;
    menuItemBase** items ;
    void list();
    void loop();
    void next();
    void prev();
    void select();
    menuItemBase* actual;
    menuItemBase* previous;

};




#endif // __MENU_H__