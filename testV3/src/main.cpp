#include <Arduino.h>
#include <Wire.h>


#include "digitalInput.h"


#include "LoRa.h"

extern LoRaClass LoRa;
#define NODEID 0x4
#define LED_DEBUG true

unsigned long receptionMessage = 0;
unsigned long dernierMessage = 0;
float msgRSSI = 0;
float msgSNR = 0;



#if defined(LED_BUILTIN)
  #undef LED_BUILTIN
  #define LED_BUILTIN 35
#endif





digitalInput btnPRG(0,INPUT_PULLUP);





String LoRaMesageStatut(){
  Serial.println("DemandeStatut");
  String toSend = "recu";
  

  return toSend;
}

void LoRaMessage(LoRaPacket header, String msg){
  receptionMessage = millis();
  dernierMessage = millis();
  msgRSSI= header.RSSI;
  msgSNR= header.SNR;
  //message = msg;
  digitalWrite(LED_BUILTIN,HIGH);
  
}

void initLoRa()
{
  LoRa.setNodeID(NODEID);
  LoRa.onMessageStatut(LoRaMesageStatut);
  LoRa.onMessage(LoRaMessage);

  if (LoRa.begin() != RADIOLIB_ERR_NONE)
  {
    Serial.println("[LoRa] init fail");
    while (true)
    {
      /* code */
    }
    
  }
  Serial.println("[LoRa] init success !");
  LoRa.getRadio().startReceive();
  
}



// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_OLED,SCL_OLED);

  pinMode(LED_BUILTIN,OUTPUT);

  initLoRa();


  
}

// put your main code here, to run repeatedly:
void loop() {
  LoRa.loop();
  
  if (millis()> receptionMessage + 200 && receptionMessage != 0)
  {
    receptionMessage = 0;
    digitalWrite(LED_BUILTIN,LOW);
  }


 
}