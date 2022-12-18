#ifndef __MAIN_H__
#define __MAIN_H__

struct dataTurbine_t
{
  float positionVanne = 0;
  float targetPositionVanne = 0;
  float positionMoteur = 0;
  float tacky = 0;
  float U = 0;
  float I = 0;
};

struct nodeStatus_t
{
  float RSSI = 0;
  float SNR = 0;
  unsigned long dernierMessage = 0;
  byte addr = 0;
  String Name = "";
  bool active = true;
};

struct dataEtang_t
{
  float niveauEtang = 0;
  float niveauEtangRempli = 0;
  float niveauEtangVide = 0;
  float niveauEtangTroPlein = 0;
  float ratioNiveauEtang = 0;
};

extern String bufferActionToSend;

extern dataTurbine_t dataTurbine;
extern nodeStatus_t TurbineStatus;
extern dataEtang_t dataEtang;
extern nodeStatus_t EtangStatus;
#endif // __MAIN_H__