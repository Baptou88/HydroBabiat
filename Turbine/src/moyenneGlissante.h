#include <Arduino.h>

#ifndef MOYENNE_GLISSANTE_H
#define MOYENNE_GLISSANTE_H

class moyenneGlissante
{
private:
    float* tableau; //tableau de stockage
    int taille;   //taille du tableau
    int index;
    float somme;
public:
    moyenneGlissante(int taille);
    ~moyenneGlissante();
    void reset();
    float add(float val);
    float get();
};

#endif

