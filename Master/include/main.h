#ifndef __MAIN_H__
#define __MAIN_H__

#include "AsyncTelegram2.h"
#include "motorState.h"

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
    message += "\"motorState\":" +  (String)motorState + ",";
    message += "\"intensite\":" +  (String)I;
    
    return message;
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
  String toJson(){
    String message = "";
    message += "\"niveauEtang\":" +  (String)niveauEtang +",";
    message += "\"ratioNiveauEtang\":" +  (String)ratioNiveauEtang;
    return message;
  }
};

extern String bufferActionToSend;

extern dataTurbine_t dataTurbine;
extern nodeStatus_t TurbineStatus;
extern dataEtang_t dataEtang;
extern nodeStatus_t EtangStatus;

extern AsyncTelegram2 TelegramBot;



#endif // __MAIN_H__