#include "menu/menunu.h"
#include "menu/menuItem.h"
menunu::menunu(Adafruit_SSD1306* display)
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
    Serial.println("erreur");
    
    
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
        Serial.println("NULL");
        return;
    }
    if (actual == NULL)
    {
        Serial.println("NULL");
        return;        
    }
    
   actual->draw(_display);
 
    
}
