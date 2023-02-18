#include <Arduino.h>
#include <LoRa.h>
#include <SPIFFS.h>

#ifndef __LORA_FILE_UPLOADER__
#define __LORA_FILE_UPLOADER__

enum LoRaUploaderState{
    NONE,
    WAIT_ACK,
    WAIT_BEFORE_RETRANSMIT,
};
class LoRaUploader
{
private:
    String FileName;
    byte id;
    bool initialized = false;
    long fileOffset = 0;
    int maxPaacketSize = 12;
    long fileLength = 0;
    int numPacket = 0;
    File file;
    unsigned long _millis;
    int delay_ms = 500;

public:
    bool waitResponse;
    LoRaUploader(/* args */);
    ~LoRaUploader();
    void loop();
    void beginTransmit(String fileName, byte addr);
    void sendPacket();
    void nextPacket();
};

LoRaUploader::LoRaUploader(/* args */)
{
}

LoRaUploader::~LoRaUploader()
{
}

inline void LoRaUploader::loop()
{
    if (!initialized)
    {
        return;
    }
    
    if (!waitResponse)
    {
        if (millis()< _millis + delay_ms    )
        {
            return;
        }
        sendPacket();
        waitResponse = true;
        // j'envoi un packet

    }
    
}

inline void LoRaUploader::beginTransmit(String fileName, byte addr)
{
    Serial.printf("[LoRaUploader] - begin\n");
    if (!fileName.startsWith("/"))
    {
        Serial.println("[LoRaUploader] - fileName must be start with '/' \n");
        return;
    }
    if (!SPIFFS.exists(fileName))
    {
        Serial.println("[LoRaUploader] - file doesn't exist '/' \n");
        return;
    }
    
    file = SPIFFS.open(fileName);
    
    fileOffset = 0;
    initialized = true;
    id = addr;
    fileLength = file.size();
    numPacket = ceil(fileLength / maxPaacketSize);
    Serial.printf("[LoRaUploader] - numPacket%i filesize%i\n", numPacket, fileLength);

    LoRa.sendData(id,LoRaMessageCode::FileInit,fileName + "," + (String)fileLength);
    waitResponse = true;


}

inline void LoRaUploader::sendPacket()
{
    byte buffer[maxPaacketSize];
    file.seek(fileOffset);
    Serial.printf("[LoRaUploader] - send fileOffset%i \n", fileOffset);
    int bytesRead = file.read(buffer,maxPaacketSize);
    if (bytesRead < maxPaacketSize)
    {
        initialized = false;
        Serial.printf("[LoRaUploader] - fin\n");
        
    }
    //LoRa.getRadio().startTransmit(buffer, sizeof(buffer));
    LoRa.sendData(id,LoRaMessageCode::FilePacket,String(buffer,sizeof(buffer)));
    _millis = millis();
    for (int i = 0; i < bytesRead; i++) {
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
        Serial.print("\n");
    fileOffset += maxPaacketSize;
}

inline void LoRaUploader::nextPacket()
{
    fileOffset += maxPaacketSize;
    waitResponse = false;
}

LoRaUploader LoRaFileUpl;
#endif //__LORA_FILE_UPLOADER__