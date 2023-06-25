#ifndef __MAIN_H__
#define __MAIN_H__

//#define ASYNC_TCP_SSL_ENABLED 1

#include "AsyncTelegram2.h"
#include "TelegramCredentials.h"
#include "motorState.h"
#include <Preferences.h>
#include "motorState.h"

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
  MotorState motorState = MotorState::UNKOWN;
  String toJson(){
    String message = "";
    message += "\"positionVanne\":" +  (String)positionVanne +",";
    message += "\"RangePosVanneTarget\":" +  (String)targetPositionVanne +",";
    message += "\"tacky\":" +  (String)tacky +",";
    message += "\"tension\":" +  (String)U + ",";
    message += "\"tensionBatterie\":" +  (String)UB + ",";
    message += "\"motorStateStr\":\"" +  (String)MotorStateToString(motorState) + "\",";
    message += "\"motorState\":" +  (String)motorState + ",";
    message += "\"power\":" +  (String)getPower() + ",";
    message += "\"intensite\":" +  (String)I;
    
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
    message += "\"" + (String)Name + "RSSI\":" +  (String)RSSI +",";
    message += "\"" + (String)Name + "SNR\":" +  (String)SNR +",";
    message += "\"" + (String)Name + "Active\":" +  (String)active +",";
    message += "\"" + (String)Name + "DernierMessage\":" +  (String)((millis()-dernierMessage)/1000) ;

    return message;
  }
};

struct dataEtang_t
{
  float niveauEtang = 0;
  float niveauEtangRempli = 0;
  float niveauEtangVide = 0;
  float niveauEtangTroPlein = 0;
  float ratioNiveauEtang = 0;
  uint8_t RoiCenter = 0;
  uint16_t RoiX = 0;
  uint16_t RoiY = 0;
  uint16_t timingBudget = 0;
  int distanceMode = 0;
  String toJson(){
    String message = "";
    message += "\"niveauEtang\":" +  (String)niveauEtang +",";
    message += "\"ratioNiveauEtang\":" +  (String)ratioNiveauEtang + ",";
    message += "\"niveauEtangRempli\":" +  (String)niveauEtangRempli + ",";
    message += "\"niveauEtangVide\":" +  (String)niveauEtangVide + ",";
    message += "\"RoiCenter\":" +  (String)RoiCenter +"," ;
    message += "\"RoiX\":" +  (String)RoiX  + ",";
    message += "\"RoiY\":" +  (String)RoiY + ",";
    message += "\"timingBudget\":" +  (String)timingBudget + ",";
    message += "\"distanceMode\":" +  (String)distanceMode;
    return message;
  }
};

extern String bufferActionToSend;

extern dataTurbine_t dataTurbine;
extern nodeStatus_t TurbineStatus;
extern dataEtang_t dataEtang;
extern nodeStatus_t EtangStatus;
extern nodeStatus_t nodeTest;

extern AsyncTelegram2 TelegramBot;



#endif // __MAIN_H__