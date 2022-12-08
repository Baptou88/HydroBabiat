#ifndef __TRANSMISSION_H__
#define __TRANSMISSION_H__

#include "Moteur.h"
namespace transmission
{
    

    int tickFromTourVanne(float tourVanne);
    int tickFromTourMoteur(float tourMoteur);

    float ratiOuverture(Moteur Mot);

     int ratioToTarget(float ratio,Moteur mot);
} // namespace transmission

#endif // __TRANSMISSION_H__