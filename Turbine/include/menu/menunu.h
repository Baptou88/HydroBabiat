#ifndef __MENU_H__
#define __MENU_H__

#include <Arduino.h>

#include <LList.h>
#include "menuItem.h"
#include "Adafruit_SSD1306.h"
//#include <../include/menuItem.h>

class menuItemBase;
class  menunu
{
private:
    Adafruit_SSD1306* _display;
public:
    menunu(Adafruit_SSD1306* display);
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