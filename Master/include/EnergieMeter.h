#include <Arduino.h>
#include <main.h>


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
    void save();
    long getEnergie();
};

EnergieMeter::EnergieMeter(/* args */)
{
}

EnergieMeter::~EnergieMeter()
{
}

void EnergieMeter::begin()
{
    energie = Prefs.getLong("Energie",0);
    
}

void EnergieMeter::update(long watt)
{
    energie += watt;
    save();
}

void EnergieMeter::save()
{
    Prefs.putLong("Energie",energie);
}

long EnergieMeter::getEnergie()
{
    return energie;
}
