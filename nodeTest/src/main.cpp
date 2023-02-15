#include <Arduino.h>

#include <RadioLib.h>
#include "Ecran.h"
#include "LoRa.h"

Ecran Ec(&Wire);

void LoRaMessage(LoRaPacket header, String msg)
{
  Serial.println(msg);
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
  //LoRa.sendData(0x01,LoRaMessageCode::DemandeStatut,"er");
  LoRa.getRadio().startReceive();
}

void loop() {
  // put your main code here, to run repeatedly:
  LoRa.loop();
}