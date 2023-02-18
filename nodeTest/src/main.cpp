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
unsigned long reponse = 0;

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
    fd.numberpacket = msg.substring(separator+1).toInt();
    
    nexfile = SPIFFS.open(fd.fileName, FILE_WRITE);
    reponse = millis();
    break;
  case LoRaMessageCode::Data :

    Serial.println("data : " + (String)msg);
    break;
  
  case LoRaMessageCode::FilePacket :
    packetrecu++;
    stater = PROGRESS;
    reponse = millis();
    Serial.println("data : " + (String)msg);
    break;
  
  case LoRaMessageCode::FileEnd :
    stater = ENDE;
    Serial.println("packet ende");
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

  if (millis() > reponse + 500 && reponse != 0)
  {
    reponse = 0;
    LoRa.sendData(0x01,LoRaMessageCode::FileAck,"ACK");
  }
  
  Ec.getDisplay()->clearDisplay();

  Ec.getDisplay()->setCursor(0,0);

  Ec.getDisplay()->println(stateReceptiontoString(stater));
  Ec.getDisplay()->println((String)packetrecu + " / " + (String)fd.numberpacket);

  Ec.getDisplay()->display();

}