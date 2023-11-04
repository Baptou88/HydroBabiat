#include "menu/menunu.h"
#include "menu/menuItem.h"

#ifdef USE_SH1107
menunu::menunu(Adafruit_SH1107* display)
#else
menunu::menunu(Adafruit_SSD1306* display)
#endif
{
    _display = display;
}

menunu::~menunu()
{
}
void menunu::list()
{
    
    
    
}
void menunu::next(){
    if (actual != NULL)
    {
        actual->right();
        return;
    }
    
    
    
}
void menunu::select(){
    actual->select();
    

}
void menunu::prev(){
    if (actual != NULL)
    {
        actual->left();
    }
    
}
void menunu::loop(){
    if (_display == NULL)
    {
        return;
    }
    if (actual == NULL)
    {
        return;        
    }
    
   actual->draw(_display);
}
