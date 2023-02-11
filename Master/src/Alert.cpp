#include "Alert.h"

AlertClass::AlertClass()
{
}

AlertClass::~AlertClass()
{
}

void AlertClass::loop()
{
    if (!active )
    {
        return;
    }
    
    if (dataEtang.ratioNiveauEtang < niveauMin)
    {
        /* code */
    }
    
    
}



AlertClass Alert;