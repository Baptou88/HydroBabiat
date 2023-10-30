#include <Arduino.h>
#include <main.h>

#ifndef ENERGIEMETER_H
#define ENERGIEMETER_H
class EnergieMeter
{
private:
    /* data */
    long energie = 0;
public:
    EnergieMeter(/* args */);
    ~EnergieMeter();

    void begin();
    void update(long watt);
    void update(long intensite ,  long tension);
    void save();
    void reset();
    long getEnergie();
};



#endif //ENERGIEMETER_H