#include <Arduino.h>

#include <RadioLib.h>
#include "Ecran.h"
#include "LoRa.h"
#include <SPIFFS.h>


Ecran Ec(&Wire);
String file;
File nexfile;
int separator ;

void LoRaMessage(LoRaPacket header, String msg)
{
  //Serial.println(msg);

  switch (header.Code )
  {
  case LoRaMessageCode::FileInit :
    separator = msg.indexOf(',');
    Serial.println(msg.substring(0,separator));
    nexfile = SPIFFS.open(msg.substring(0,separator), FILE_WRITE);
    delay(500);
    LoRa.sendData(0x01,LoRaMessageCode::FileAck,"");
    break;
  case LoRaMessageCode::Data :
    Serial.println("data : " + (String)msg);
    break;
  
  default:
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
}