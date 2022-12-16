#ifndef __MENUITEM_H__
#define __MENUITEM_H__

#include "menunu.h"
#include "Adafruit_SSD1306.h"
#include "Arduino.h"

class  menunu;
class menuItemBase
{
private:
public:
   
    menuItemBase* parent = NULL;
    
    menunu* _m = NULL;
    char* m_title;
    menuItemBase(char* title);
    menuItemBase();
    
    virtual void select();
    virtual void right();
    virtual void left();

    virtual void draw(Adafruit_SSD1306* display) ;
    void sayHello();
    

};


class menuItemnew: public menuItemBase
{
private:
    /* data */
public:
    menuItemnew(char* title);
    ~menuItemnew();
    void draw(Adafruit_SSD1306* display)  {
        Serial.println("fg");
    }
    void select(){

    }
    void right(){

    }
    void left(){

    }
};

class menuItemInt: public menuItemBase
{
private:
    int* m_param;
    int m_max;
    int m_min;
public:
    menuItemInt(char* title,int* param,int min=0,int max=100);
    ~menuItemInt();
    void draw(Adafruit_SSD1306* display);
    void select();
    void right();
    void left();
};
class menuItemFloat: public menuItemBase
{
private:
    float* m_param;
    float m_max;
    float m_min;
    float multiplicateur = 100;
public:
    menuItemFloat(char* title,float* param,float min,float max);
    ~menuItemFloat();

    int countDigit(float num);

    void draw(Adafruit_SSD1306* display);
    void select();
    void right();
    void left();
};






class menuItembool : public menuItemBase
{
private:
    bool* _param;
public:
    
    menuItembool(char* title , bool* param);
    ~menuItembool();
    void  draw(Adafruit_SSD1306* display);
    void select();
    void right();
    void left(){

    }
};

class menuItemCalleback: public menuItemBase
{
private:
    void (*calleback)(Adafruit_SSD1306* display,bool firstTime);
    bool firstTime = true;
public:
    menuItemCalleback(char* title, void (*cb)(Adafruit_SSD1306* display,bool firstTime) );
    ~menuItemCalleback();
    void draw(Adafruit_SSD1306* display);
    void select();
    void right(){

    }
    void left(){

    }
};

class menuItemList: public menuItemBase
{
    private:
        int nbItem = 0;
        int first_el = 0;
        LList<menuItemBase*> items;
        int selected = 0;
        int maxItems = 4;
    public:
    //menuItemList();
    menuItemList(char* title,menunu *m);
    ~menuItemList();
    void addItem(menunu *m, menuItemBase *item);
    void draw(Adafruit_SSD1306* display);
    void select();
    void right();
    void left();

};

#endif // __MENUITEM_H__