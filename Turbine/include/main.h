#ifndef __MAIN_H__
#define __MAIN_H__
#include "Tachymetre.h"

void commandProcess(String cmd);
bool savePreferences();

float calibrateADS(int channel, int sample = 20, int delaybetweensample = 5);

#define VOLTAGE_ADS_CHANNEL 0
#define CURRENT_ADS_CHANNEL 1

extern Tachymetre tachy;
/// @brief vitesse max de la turbine
extern int maxSpeed ;

#endif // __MAIN_H__