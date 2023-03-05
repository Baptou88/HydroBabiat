#include <Arduino.h>
#include "LoRa.h"


LoRaClass::LoRaClass(/* args */)
{
  
}

LoRaClass::~LoRaClass()
{
}

int LoRaClass::begin(){

#ifdef ARDUINO_HELTEC_WIFI_LORA_32_V2
    radio = new Module(SS, 26, RST_LoRa, 33);
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3)
    radio = new Module(LoRa_SS, 14, LoRa_RST, 13);
#else
    #error UnImplemented
#endif

    SPI.begin(LoRa_SCK,LoRa_MISO,LoRa_MOSI,LoRa_SS);
    



    #ifdef ARDUINO_HELTEC_WIFI_LORA_32_V2
        radio.setDio0Action(LoRaClass::setFlag);
    #elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3)
        radio.setDio1Action(LoRaClass::setFlag);
    #else
        #error UnImplemented
    #endif

    return radio.begin(868.0,125,9,7,18,20);
    
}

void LoRaClass::loop()
{
    if (millis() > _millisReponseStatut && _millisReponseStatut != 0)
    {
        _millisReponseStatut = 0;
        
        reponseStatue = false;
        String toSend = MessageStatut();
        Serial.println("reponse");
        LoRa.sendData(0x01,LoRaMessageCode::Data,toSend);
    }
    // if (reponseStatue)
    // {
    //     unsigned long attente = millis() + 200;
    //     while (millis() < attente )
    //     {
    //         Serial.println("attente");
    //         delay(50);
    //     }
        
    //     reponseStatue = false;
    //     String toSend = MessageStatut();
        
    //     LoRa.sendData(0x01,LoRaMessageCode::Data,toSend);
    // }

    checkReply();
    
    if (operationDone)
    {
        if (transmitFlag)
        {   
            //un message est en cours d'envoi
            transmitFlag = false;
            if (transmissionState == RADIOLIB_ERR_NONE) {
                // packet was successfully sent
                //LORACLASS_DEBUG_PRINTLN(F("transmission finished!"));

            } else {
                LORACLASS_DEBUG_PRINT(F("failed, code "));
                LORACLASS_DEBUG_PRINTLN(transmissionState);
                
            }
        } else
        {
            //un message est en cours de reception
            String str;
            int state = radio.readData(str);

            if (state == RADIOLIB_ERR_NONE) {
                //le message est bien recu
                // TODO Verifier que le message est pour moi
                //LORACLASS_DEBUG_PRINTLN(str)

                
                LoRaPacket packet;
                packet.RSSI = radio.getRSSI();
                packet.SNR = radio.getSNR();
                int p = str.indexOf(",");
                packet.Emetteur = str.substring(0,p).toInt();

                if (packet.Emetteur == lastSend.id)
                {
                    lastSend.id= 0;
                    LORACLASS_DEBUG_PRINTLN("[LORA] Reponse " + String(float((millis() - lastSend.sendingTime) /1000.0)) + "s")
                }
                

                //LORACLASS_DEBUG_PRINTLN("[LoRa] Emetteur: " + (String)packet.Emetteur);
                str.remove(0,p+1);

                p = str.indexOf(",");
                packet.Destinataire = str.substring(0,p).toInt();
                //LORACLASS_DEBUG_PRINTLN("[LoRa] dest: " + (String)packet.Destinataire);
                str.remove(0,p+1);

                p = str.indexOf(",");
                packet.Code = (LoRaMessageCode) str.substring(0,p).toInt();
                //LORACLASS_DEBUG_PRINTLN("[LoRa] code: " + (String)packet.Code);
                str.remove(0,p+1);


                //enleve le separateur "|"
                str.remove(0,p);

                if (packet.Destinataire == nodeID)
                {
                    //les message est pour moi
                    if (packet.Code == DemandeStatut)
                    {
                        if (MessageStatut)
                        {
                            reponseStatue = true;
                            _millisReponseStatut = millis() + WAIT_TO_RESPONSE;
                        }
                        
                    }
                    
                    if (messageCalleBack)
                    {
                        messageCalleBack(packet,str);
                    }
                } //else
                // {
                //     LORACLASS_DEBUG_PRINTLN("[LoRa] pas pour moi");
                // }
                
                
                
            }
            else
            {
                LORACLASS_DEBUG_PRINTLN("[LORA] Reception failed");
            }
            
        }
        
        
        operationDone = false;
        radio.startReceive();
        
        //LORACLASS_DEBUG_PRINTLN("[LoRa] op done "+(String) operationDone);
    }
    
    
    
    
}

void LoRaClass::setFlag()
{
    if (!LoRa.enableInterrupt)
    {
        return; 
    }
    LoRa.operationDone = true;
}

SXClass LoRaClass::getRadio(void)
{
    return radio;
}

int LoRaClass::sendData(byte address,LoRaMessageCode code, String Data)
{
    transmitFlag = true;
    String msg = String(nodeID) + "," + String(address)+"," + String(code)+",|"+String(Data);
    lastSend.id = address;
    lastSend.sendingTime = millis();

    LORACLASS_DEBUG_PRINTLN("[LORA] send msg " + (String)msg)
    int16_t retour = radio.startTransmit(msg);
    if (retour != RADIOLIB_ERR_NONE)
    {
        LORACLASS_DEBUG_PRINTLN("[LORA] send msg error: " + retour)
    }
    
    return retour;
}

void LoRaClass::onMessage(void(*cb)(LoRaPacket header, String message))
{
    messageCalleBack = cb;
}
void LoRaClass::onMessageStatut(String(*cb)())
{
    MessageStatut = cb;
}

void LoRaClass::onNoReply(void(*cb)(byte address)){
    noReplyCalleback = cb;
}

void LoRaClass::setNodeID(byte id)
{
    nodeID = id;
}

void LoRaClass::checkReply(){
    if (lastSend.id != 0 && millis()>lastSend.sendingTime + REPLY_TIMEOUT)
    {
        LORACLASS_DEBUG_PRINTLN("[LORA] pas eu de reponse de: " + (String)lastSend.id)
        if (noReplyCalleback != NULL)
        {
            noReplyCalleback(lastSend.id);
        }
        lastSend.id = 0;
        
    }
    
}
LoRaClass LoRa;