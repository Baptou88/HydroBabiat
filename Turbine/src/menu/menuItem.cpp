#include <Arduino.h>

#include "menu/menunu.h"
#include "menu/menuItem.h"

menuItemBase::menuItemBase(){}
menuItemBase::menuItemBase(char* title)
{
    m_title = title;
}

// menuItemBase::~menuItemBase()
// {
// }

void menuItemBase::sayHello(){
    Serial.println("hello");
}



menuItemnew::menuItemnew(char* title)
{
    m_title = title;
}

menuItemnew::~menuItemnew()
{
}


menuItembool::menuItembool(char* title , bool * param)
{
    m_title = title;
    _param = param;
}

menuItembool::~menuItembool()
{
}
void  menuItembool::draw(Adafruit_SSD1306* display)  {
    display->setCursor(0,0);
    display->println("Boolean");
    display->println(m_title);
    display->drawRect(10,20,110,20,SSD1306_WHITE);
    
    if (*_param == true)
    {
        display->fillRect(10,20,60,20,SSD1306_WHITE);
       
        //display->setColor(SSD1306_BLACK);
        
        display->setTextColor(SSD1306_BLACK);
        display->setCursor(20,25);
        display->print("ON");
        
        display->setTextColor(SSD1306_WHITE);
    }else
    {
        display->setCursor(90,25);
        display->print("OFF");

        
    }
    display->setCursor(20,50);
    display->print(*_param);
    
    
}

void menuItembool::select()
{
    if (_m->previous != NULL)
    {
        _m->actual = _m->previous;
    }
    
}

void menuItembool::menuItembool::right()
{

  *_param = !*_param ;  
}



menuItemInt::menuItemInt(char* title,int* param,int min,int max)
{
    m_title = title;
    m_param = param;
    m_max = max;
    m_min = min;
}

void menuItemInt::draw(Adafruit_SSD1306* display)
{
    display->setCursor(40,30);
    display->print(*m_param);
    
}

void menuItemInt::select()
{
   if (_m->previous != NULL)
    {
        _m->actual = _m->previous;
    } 
}

void menuItemInt::right()
{

    *m_param = *m_param+1;

}

void menuItemInt::left()
{
    *m_param = *m_param-1;
}

menuItemFloat::menuItemFloat(char* title,float* param,float min,float max)
{
    m_title = title;
    m_param = param;
    m_max = max;
    m_min = min;
}

int menuItemFloat::countDigit(float num)
{
    int digits=0;
    double ori=num;//storing original number
    long num2=num;
    while(num2>0)//count no of digits before floating point
    {
        digits++;
        num2=num2/10;
    }
    if(ori==0)
        digits=1;
    num=ori;
    double no_float;
    no_float=ori*(pow(10, (8-digits)));
    long long int total=(long long int)no_float;
    int no_of_digits, extrazeroes=0;
    for(int i=0; i<8; i++)
    {
        int dig;
        dig=total%10;
        total=total/10;
        if(dig!=0)
            break;
        else
            extrazeroes++;
    }
    no_of_digits=8-extrazeroes;
    return no_of_digits;
}

void menuItemFloat::draw(Adafruit_SSD1306* display)
{
    display->setCursor(0,15);
    display->printf("Valeur: %f",*m_param);
    display->setCursor(0,30);
    display->printf("count digit: %i",countDigit(*m_param));
    display->setCursor(0,45);
    display->printf("mul: %f",multiplicateur);
    
}

void menuItemFloat::select()
{
    if (multiplicateur > 0.01)
    {
        multiplicateur =  multiplicateur / 10;
        return;
    }
    if (_m->previous != NULL)

    {
        multiplicateur = 100;
        _m->actual = _m->previous;
    }
    
}

void menuItemFloat::right()
{
    *m_param += multiplicateur;
    
}

void menuItemFloat::left()
{
     *m_param -= multiplicateur;
}

menuItemList::~menuItemList(){
    
}
menuItemList::menuItemList(char* title,menunu *m){
    _m = m;
}
void menuItemList::select(){


    
    if (_m == NULL)
    {

    }else
    {
        _m->actual = items.get(selected);
        
    }
    _m->previous = this;
    
    
}
void menuItemList::right(){
    // if (maxItems + first_el >= items.size())
    // {
    //     Serial.printf("par là %i %i %i \n",maxItems,first_el,items.size());
    //     return;
    // }
    // selected++;
    // if (selected >= maxItems + first_el)
    // {
    //     first_el++;
    // }
    
    if (selected < items.size()-1)
    {
        selected++;

        if (selected - first_el >= maxItems)
        {
            first_el++;

        }
        
    }
    
    
    
}
void menuItemList::left(){
    if (selected <= 0)
    {
        return;
    }
    selected--;
    if (selected <  first_el)
    {
        first_el--;
    }
    
    
}
void menuItemList::addItem(menunu *m,menuItemBase* item){
    // nbItem++;
    
    // Serial.println("mlk");
    // items = (menuItemBase**)realloc(items,sizeof(menuItemBase) * nbItem);

    // //item->draw(); //fonctionne trés bien
    // if(items!=NULL)
    // {
    //     items[nbItem-1] = item;
    //     Serial.printf("po %i \n" , nbItem-1);
    //     //items[nbItem-1].draw();  //Crash
        
    // } else
    // {
    //     free(items);
    //     Serial.println("error");
    // }
    item->parent = this;
    item->_m = m;
    if (item->_m == NULL)
    {
        Serial.println("item null");
    }
    
    items.add(item);
}
void menuItemList::draw(Adafruit_SSD1306* display){
   if (display == NULL)
   {

    return;
   }
   
    display->setCursor(0,0);
    display->print("List  ");    
    
    display->print(items.size());
    display->setCursor(50,0);
    display->print(selected);

    for (size_t i = 0; i < (items.size()> maxItems ? maxItems:items.size()-1); i++)
    {
        display->setCursor(10,i*12+12);
        display->print(items.get(i+first_el)->m_title);


        if (i+first_el == selected)
        {
            display->setCursor(0,i*12+12);
            display->print(">");
            
        }
        
    }
   
    
    
    
}

menuItemCalleback::menuItemCalleback(char* title, void (*cb)(Adafruit_SSD1306* display, bool firstTime) ){
    m_title = title;
    calleback = cb;
}
void menuItemCalleback::draw(Adafruit_SSD1306* display){ 
    if (calleback!=NULL)
    {
        calleback(display, firstTime);
    }
    firstTime = false;

    
}
void menuItemCalleback::select(){
    if (_m->previous != NULL)

    {
        _m->actual = _m->previous;
        firstTime = true;
    }
}