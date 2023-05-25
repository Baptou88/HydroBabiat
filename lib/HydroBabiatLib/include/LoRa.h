#ifndef __LORA_H__
#define __LORA_H__


#include <Arduino.h>
#include <RadioLib.h>
#include <SPI.h>

#define WAIT_TO_RESPONSE 80
#define REPLY_TIMEOUT 3000

#define LORACLASS_DEBUG_STREAM Serial
#ifdef LORACLASS_DEBUG_STREAM
#define LORACLASS_DEBUG_PRINTLN(...)                 \
{                                              \
    LORACLASS_DEBUG_STREAM.println(__VA_ARGS__); \
}
#define LORACLASS_DEBUG_PRINT(...)                 \
{                                            \
    LORACLASS_DEBUG_STREAM.print(__VA_ARGS__); \
}
#else
#define LORACLASS_DEBUG_PRINT(...)
#define LORACLASS_DEBUG_PRINTLN(...)
#endif

#ifdef ARDUINO_HELTEC_WIFI_LORA_32_V2
    #define LoRa_SS SS
    #define LoRa_SCK SCK
    #define LoRa_MOSI MOSI
    #define LoRa_MISO MISO
    #define SXClass SX1276
    
#elif defined(ARDUINO_HELTEC_WIFI_LORA_32_V3)
    #define LoRa_SS 8
    #define LoRa_SCK 9
    #define LoRa_MOSI 10
    #define LoRa_MISO 11
    #define LoRa_RST 12
    #define SXClass SX1262
       
#else
    #error UnImplemented
#endif

enum LoRaMessageCode{
    DemandeStatut,
    Data,

    FileInit,
    FilePacket,
    FileAck,
    FileEnd,

    DataReponse,
};

struct LoRaPacket{
    byte Emetteur;
    byte Destinataire;
    LoRaMessageCode Code;
    //Recorded Signal Strength Indicator in dBm
    float RSSI = 0;
    //Signal to Noise Ratio in dB
    float SNR = 0;
};
struct lastSend_t{
    byte id;
    unsigned long sendingTime =0;

};

class LoRaClass
{
private:
    
    byte nodeID = 0;

    bool reponseStatue = false;

    // disable interrupt when it's not needed
    bool enableInterrupt = true;
    // flag to indicate that a packet was sent or received
    bool operationDone = false;
    // flag to indicate transmission or reception state
    bool transmitFlag = false;
    // save transmission states between loops
    int transmissionState = RADIOLIB_ERR_NONE;

    SXClass radio = NULL;

    SPIClass LoRaSpi;

    

    unsigned long _millis = 0;
    unsigned long _millisReponseStatut = 0;
    

    void(*messageCalleBack)(LoRaPacket header, String Message);
    String(*MessageStatut)();
    void(*noReplyCalleback)(byte address);

    void checkReply();
public:
    LoRaClass(/* args */);
    ~LoRaClass();

    int begin();
    void loop();

    static void setFlag();

    SXClass getRadio(void);

    int sendData(byte address,LoRaMessageCode code,String Data);

    void onMessage(void(*cb)(LoRaPacket header, String message));
    void onMessageStatut(String(*cb)());
    void onNoReply(void(*cb)(byte address));

    void setNodeID(byte id);

    lastSend_t lastSend;
};








#endif // __LORA_H__

extern LoRaClass LoRa;