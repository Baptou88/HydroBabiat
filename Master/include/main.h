#ifndef __MAIN_H__
#define __MAIN_H__

//#define ASYNC_TCP_SSL_ENABLED 1

#include "motorState.h"
#include <Preferences.h>
#include "motorState.h"
#include "Notifier.h"
#include "time.h" 
#define TZ_Europe_Paris	PSTR("CET-1CEST,M3.5.0,M10.5.0/3")

// Preference

#define MODEVANNE "modeVanne"


bool savePref();

extern unsigned long startDeepSleep;
extern unsigned long startReboot ;

extern Preferences Prefs;
struct dataTurbine_t
{
  float positionVanne = 0;
  float targetPositionVanne = 0;
  float positionMoteur = 0;
  float tacky = 0;
  float U = 0;
  float I = 0;
  float UB = 0;
  float ZV =0;
  float AV = 0;
  float ZC = 0;
  float AC = 0;
  float currentSyst = 0;
  MotorState motorState = MotorState::UNKOWN;
  MotorState prevMotorState = MotorState::UNKOWN;
  String toJson(){
    String message = "\"Turbine\":{";
    message += "\"positionVanne\":" +  (String)positionVanne +",";
    message += "\"PositionVanneTarget\":" +  (String)targetPositionVanne +",";
    message += "\"tacky\":" +  (String)tacky +",";
    message += "\"tension\":" +  (String)U + ",";
    message += "\"tensionBatterie\":" +  (String)UB + ",";
    message += "\"motorStateStr\":\"" +  (String)MotorStateToString(motorState) + "\",";
    message += "\"motorState\":" +  (String)motorState + ",";
    message += "\"power\":" +  (String)getPower() + ",";
    message += "\"ZV\":" +  (String)ZV + ",";
    message += "\"AV\":" +  (String)AV + ",";
    message += "\"ZC\":" +  (String)ZC + ",";
    message += "\"AC\":" +  (String)AC + ",";
    message += "\"CurrentSyst\":" +  (String)currentSyst + ",";
    message += "\"intensite\":" +  (String)I;
    message += "}";
    return message;
  }
  float getPower(){
    //Serial.println("u " + (String)U + " I " + (String) I + (String)(U*I));
    return U*I;
  }
};

struct nodeStatus_t
{
  float RSSI = 0;
  float SNR = 0;
  unsigned long dernierMessage = 0;
  byte addr = 0;
  String Name = "";
  bool active = true;
  String toJson(){
    String message = "" ;
    message += "\"RSSI\":" +  (String)RSSI +",";
    message += "\"SNR\":" +  (String)SNR +",";
    message += "\"Active\":" +  (String)active +",";
    message += "\"DernierMessage\":" +  (String)((millis()-dernierMessage)/1000) ;

    return message;
  }
};

struct dataEtang_t
{
  float niveauEtang = 0;
  float niveauEtangRempli = 0;
  float niveauEtangVide = 0;
  float niveauEtangTroPlein = 0;
  float niveauEtangP = 0;
  uint8_t RoiCenter = 0;
  uint16_t RoiX = 0;
  uint16_t RoiY = 0;
  uint16_t timingBudget = 0;
  int distanceMode = 0;
  int vl53Status = 0;
  String toJson(){
    String message = "\"Etang\":{";
    message += "\"niveauEtang\":" +  (String)niveauEtang +",";
    message += "\"niveauEtangP\":" +  (String)niveauEtangP + ",";
    message += "\"niveauEtangRempli\":" +  (String)niveauEtangRempli + ",";
    message += "\"niveauEtangVide\":" +  (String)niveauEtangVide + ",";
    message += "\"RoiCenter\":" +  (String)RoiCenter +"," ;
    message += "\"RoiX\":" +  (String)RoiX  + ",";
    message += "\"RoiY\":" +  (String)RoiY + ",";
    message += "\"timingBudget\":" +  (String)timingBudget + ",";
    message += "\"vl53Status\":" +  (String)vl53Status + ",";
    message += "\"distanceMode\":" +  (String)distanceMode;

    message += "}";
    return message;
  }
};

struct dataRadiateur_t
{
  float temp = 0;
  
  bool Rad1 = false;
  bool Rad2 = false;

  String toJson(){
    String message = "\"Radiateur\":{";
    message += "\"temp\":" +  (String)temp +",";
    message += "\"rad1\":" +  (String)Rad1 +",";
    message += "\"rad2\":" +  (String)Rad2 ;
    message += "}";
    return message;
  }
};

extern String bufferActionToSend;

extern dataTurbine_t dataTurbine;
extern nodeStatus_t TurbineStatus;

extern dataEtang_t dataEtang;
extern nodeStatus_t EtangStatus;

extern nodeStatus_t RadiateurStatus;
extern dataRadiateur_t dataNodeTest;





#endif // __MAIN_H__