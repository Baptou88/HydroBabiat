#include <Arduino.h>
#include "moyenneGlissante.h"




moyenneGlissante::moyenneGlissante(int taille)
{
    this->taille = taille;
    tableau = new float[taille];
    reset();
}

moyenneGlissante::~moyenneGlissante()
{
    delete[] tableau;
}

void moyenneGlissante::reset()
{
    index = 0;
    somme = 0;
    memset(tableau,0,sizeof(float) * taille);
}

float moyenneGlissante::add(float val)
{
    somme -= tableau[index];
    tableau[index] = val;
    somme += val;
    index = (index + 1) % taille;
    return (float) somme / (float)taille;
}

float moyenneGlissante::get()
{
    return somme /(float)taille;
}

