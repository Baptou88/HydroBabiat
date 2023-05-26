#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <Arduino.h>

#define NODEID TURBINE

//Encodeur Moteur

#define PIN_ENCODEUR_CW 26//5
#define PIN_ENCODEUR_CCW 20//4

//Moteur
#define pinMoteurCCW 45
#define pinMoteurCW 46


//Fin de course
#define PIN_FC_O 48
#define PIN_FC_F 47

//Tachymètre
#define PIN_TACHY 21


//Tension de sortie
//#define PIN_VOLTAGE_OUTPUT 3

//Intensite de sortie
//#define PIN_CURRENT_OUTPUT 2

// Encodeur 
#define PIN_ROTARY_CLK 4
#define PIN_ROTARY_DT 5
#define PIN_ROTARY_SW 6



#endif // __CONFIGURATION_H__