#include "EnergieMeter.h"

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

void EnergieMeter::update(long intensite, long tension)
{
    if (intensite <= 1)
    {
        intensite = 0;
    }
    if (tension <= 5)
    {
        tension = 0;
    }

    energie += intensite * tension;
    save();
    
}

void EnergieMeter::save()
{
    Prefs.putLong("Energie",energie);
}

void EnergieMeter::reset()
{
    energie = 0;
    save();
}

long EnergieMeter::getEnergie()
{
    return energie;
}