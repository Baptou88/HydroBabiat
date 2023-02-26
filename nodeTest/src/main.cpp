#include <Arduino.h>

#include <RadioLib.h>
#include "Ecran.h"
#include "LoRa.h"
#include <SPIFFS.h>

struct fileDescription_t
{
    String fileName;
    int numberpacket ;
    int fileSize;
    bool OtaUpdate = false;
};

fileDescription_t fd;

enum stateReception {
  NONE,
  BEGUN,
  PROGRESS,
  ENDE
};

String stateReceptiontoString(stateReception s){
  switch (s)
  {
  case stateReception::NONE :
    return "NONE";
    break;
  case stateReception::BEGUN :
    return "BEGUN";
    break;
  case stateReception::PROGRESS :
    return "PROGRESS";
    break;
  case stateReception::ENDE :
    return "ENDE";
    break;
  default:
  return "default";
    break;
  }
}
stateReception stater = NONE;
Ecran Ec(&Wire);
String file;
File nexfile;
int separator ;

int packetrecu = 0;
int packetNum = 0;
unsigned long reponse = 0;
float vitesseTranfert = 0;
unsigned long transfertTime = 0;
unsigned long startTransfertTime = 0;



void LoRaMessage(LoRaPacket header, String msg)
{
  //Serial.println(msg);

  switch (header.Code )
  {
  case LoRaMessageCode::FileInit :
    stater = BEGUN;
    packetrecu = 0;
    separator = msg.indexOf(',');
    
    fd.fileName =  msg.substring(0,separator);
    fd.fileSize = msg.substring(separator+1,  msg.indexOf(',',separator+1)).toInt();
    separator = msg.indexOf(',',separator+1);
    Serial.println("sep "+ (String)separator);
    fd.numberpacket = msg.substring(separator+1,  msg.indexOf(',',separator+1)).toInt();
    separator = msg.indexOf(',',separator+1);
    fd.OtaUpdate = msg.substring(separator +1).toInt();
    
    nexfile = SPIFFS.open(fd.fileName, FILE_WRITE);
    reponse = millis();
    startTransfertTime = millis();
    break;
  case LoRaMessageCode::Data :

    Serial.println("data : " + (String)msg);
    if (fd.OtaUpdate)
    {
      
      Serial.println("OTA : " + (String)msg);
    }
    
    break;
  
  case LoRaMessageCode::FilePacket :
    stater = PROGRESS;

    reponse = millis();
    packetNum = msg.substring(0,msg.indexOf(",")).toInt();
    msg.remove(0,msg.indexOf(",")+1);
    Serial.println("lenght " + String(msg.length()) + String(millis()-transfertTime));
    
    Serial.printf("Num %i  recu:%i \n",packetNum,packetrecu);
    if (packetNum != packetrecu)
    {
      Serial.println("packet deja recu");
      break;
    }
    packetrecu++;
    
    Serial.println("data : " + (String)msg);
    break;
  
  case LoRaMessageCode::FileEnd :
    stater = ENDE;
    Serial.println("packet ende");
    transfertTime = millis() - startTransfertTime;
    vitesseTranfert = (float) fd.fileSize / transfertTime *1000;
    Serial.println("Speed: "+ String(vitesseTranfert));
    reponse = millis();
    break;
  default:
  Serial.println("inconnu" + (String)header.Code);
    break;
  }
}

void setup() {
  // put your setup code here, to run once:
  Wire.begin(4,15);

  Serial.begin(115200);

  if (!Ec.begin())
  {
    Serial.println("Ecan failed init");
    while (true)
    {
      /* code */
    }
    
  }
  Ec.getDisplay()->display();
  Ec.getDisplay()->println("esgrg");
  Ec.getDisplay()->display();


  LoRa.setNodeID(0x04);
  LoRa.onMessage(LoRaMessage);
  if (LoRa.begin()!= RADIOLIB_ERR_NONE)
  {
    Serial.println("Error init loRa");
    while (true)
    {
    }
    
  }
  Ec.getDisplay()->println("LoRa init Ok !");
  Ec.getDisplay()->display();
  LoRa.sendData(0x01,LoRaMessageCode::DemandeStatut,"er");
  LoRa.getRadio().startReceive();

  if (!SPIFFS.begin(true))
  {
    Serial.println("Spiffs init failed");
  }
}


void loop() {
  // put your main code here, to run repeatedly:
  LoRa.loop();

  if (millis() > reponse + 50 && reponse != 0)
  {
    reponse = 0;
    LoRa.sendData(0x01,LoRaMessageCode::FileAck,"ACK");
  }
  
  Ec.getDisplay()->clearDisplay();

  Ec.getDisplay()->setCursor(0,0);

  Ec.getDisplay()->println(stateReceptiontoString(stater));
  Ec.getDisplay()->println((String)packetrecu + " / " + (String)fd.numberpacket);
  Ec.getDisplay()->println("speed :  " + (String)vitesseTranfert);
  Ec.getDisplay()->println("ota :  " + (String)fd.OtaUpdate);

  Ec.getDisplay()->display();

}