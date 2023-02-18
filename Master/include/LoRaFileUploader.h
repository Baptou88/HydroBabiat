#include <Arduino.h>
#include <LoRa.h>
#include <SPIFFS.h>

#ifndef LORAFILEUPLOADER_H
#define LORAFILEUPLOADER_H

enum LoRaUploaderState{
    NONE,
    WAIT_ACK,
    WAIT_BEFORE_RETRANSMIT,
};
struct fileDescription_t
{
    String fileName;
    int numberpacket ;
    int fileSize;
};

class LoRaFileUploader
{
private:
    String FileName;
    long fileOffset = 0;
    int maxPaacketSize = 12;
    int fileLength = 0;
    int numPacket = 0;
    File file;
    unsigned long _millis;
    int delay_ms = 500;
    int packetNum = 0;
    bool endMessage = false;
    bool hasJustStarted = false;
    int attempt = 0;
    int maxAttempt = 3;
public:
    bool initialized = false;
    byte id;
    bool waitResponse;
    LoRaFileUploader(/* args */);
    ~LoRaFileUploader();
    void loop();
    void beginTransmit(String fileName, byte addr);
    void sendPacket();
    void nextPacket();
};

extern LoRaFileUploader LoRaFileUpl;
#endif //LORA_FILE_UPLOADER_H