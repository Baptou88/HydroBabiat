#ifndef __MANUELCONTROLLER_H__
#define __MANUELCONTROLLER_H__

#include <IController.h>

class manuelController:public IController
{
private:
    /* data */
public:
    manuelController(/* args */);
    ~manuelController();
    void loop();
};



#endif // __MANUELCONTROLLER_H__