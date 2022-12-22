#include <PIDController.h>

PIDController::PIDController(/* args */)
{
    name = "PID";
    type = typeController::PID;
}

PIDController::~PIDController()
{
}

void PIDController::calculPid()
{
    float erreur = 0;
    erreur = targetEtang - niveau;
    sommeErreurs += erreur;
    float variationErreur = erreur - erreurPrecedente;
    vanne = kp * erreur + ki * sommeErreurs + kd * variationErreur;

    vanne = 100 - vanne;
    
    if (vanne > 100)
    {
        vanne = 100;
    }
    if (vanne < 0)
    {
        vanne = 0;
    }
    
    erreurPrecedente = erreur;

    /*
    erreur = consigne - mesure;
    somme_erreurs += erreur;
    variation_erreur = erreur - erreur_précédente;
    commande = Kp * erreur + Ki * somme_erreurs + Kd * variation_erreur;
    erreur_précédente = erreur
    */
}

void PIDController::loop(void)
{
    if (millis() > _millis + intervallCalcul)
    {
        _millis = millis();
        calculPid();
        output();

    }
    
}
