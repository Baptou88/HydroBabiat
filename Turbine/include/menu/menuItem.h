#ifndef __MENUITEM_H__
#define __MENUITEM_H__

#include "menunu.h"
#ifdef USE_SH1107
#include "Adafruit_SH110X.h"

#else
#include "Adafruit_SSD1306.h"

#endif
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
#ifdef USE_SH1107
    virtual void draw(Adafruit_SH1107* display) ;
#else
    virtual void draw(Adafruit_SSD1306* display) ;
#endif
    void sayHello();
    

};


class menuItemnew: public menuItemBase
{
private:
    /* data */
public:
    menuItemnew(char* title);
    ~menuItemnew();
    #ifdef USE_SH1107
    void draw(Adafruit_SH1107* display)  
    #else
    void draw(Adafruit_SSD1306* display)  
    #endif
    {
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
    #ifdef USE_SH1107
    void draw(Adafruit_SH1107* display);
    #else
    void draw(Adafruit_SSD1306* display);
    #endif
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
#ifdef USE_SH1107

    void draw(Adafruit_SH1107* display);
#else
    void draw(Adafruit_SSD1306* display);
#endif
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
    #ifdef USE_SH1107

    void  draw(Adafruit_SH1107* display);
    #else
    void  draw(Adafruit_SSD1306* display);

    #endif
    void select();
    void right();
    void left(){

    }
};

class menuItemCalleback: public menuItemBase
{
private:
#if USE_SH1107
    void (*calleback)(Adafruit_SH1107* display,bool firstTime);
#else

    void (*calleback)(Adafruit_SSD1306* display,bool firstTime);
#endif
    bool firstTime = true;
public:
#ifdef USE_SH1107
    menuItemCalleback(char* title, void (*cb)(Adafruit_SH1107* display,bool firstTime) );
    void draw(Adafruit_SH1107* display);

#else
    menuItemCalleback(char* title, void (*cb)(Adafruit_SSD1306* display,bool firstTime) );
    void draw(Adafruit_SSD1306* display);
#endif
    ~menuItemCalleback();
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
#ifdef USE_SH1107
    void draw(Adafruit_SH1107* display);
#else
    void draw(Adafruit_SSD1306* display);
#endif
    void select();
    void right();
    void left();

};

#endif // __MENUITEM_H__