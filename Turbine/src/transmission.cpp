#include "transmission.h"

namespace transmission

{
    float rapportReduction = 44.0f/18.0f ;
    int TickEncodeurParRevolution = 400;
    
    int tickFromTourVanne(float tourVanne)
    {
        return tourVanne * rapportReduction * TickEncodeurParRevolution;
    }

    int tickFromTourMoteur(float tourMoteur)
    {
        return tourMoteur * TickEncodeurParRevolution;
    }

    float ratiOuverture(Moteur mot)
    {
        return mot._position / mot.ouvertureMax *100;
    }

    
    int ratioToTarget(float ratio,Moteur mot)
    {
        return ratio * mot.ouvertureMax /100;
    }

} // namespace transmission

