#include "LoRaFileUploader.h"


LoRaFileUploader::LoRaFileUploader(/* args */)
{
}

LoRaFileUploader::~LoRaFileUploader()
{
}

void LoRaFileUploader::loop()
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
        if (endMessage)
        {
            Serial.println("[LoRaUploader] - fin ");
            initialized = false;
            LoRa.sendData(id,LoRaMessageCode::FileEnd,"Ende");
            return;
        }
        
        sendPacket();
        waitResponse = true;
        // j'envoi un packet

    }
    
}

void LoRaFileUploader::beginTransmit(String fileName, byte addr)
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

    fileDescription_t packetFile;
    
    hasJustStarted = true;
    endMessage = false;
    fileOffset = 0;
    initialized = true;
    id = addr;
    fileLength = file.size();
    numPacket = ceil(fileLength / float(maxPaacketSize));
    packetNum = 0;

    packetFile.fileSize = file.size();
    packetFile.fileName = fileName;
    packetFile.numberpacket = numPacket;
    
    Serial.printf("[LoRaUploader] - numPacket %i filesize %i\n", numPacket, fileLength);

    LoRa.sendData(id,LoRaMessageCode::FileInit,fileName + "," + (String)fileLength + "," + (String)numPacket);
    waitResponse = true;


}

void LoRaFileUploader::sendPacket()
{
    attempt ++;
    if (attempt> maxAttempt)
    {
        initialized = false;
        Serial.println("[LoRaUploader] -  max attempt");
        return;
    }
    
    int packetSize = min(maxPaacketSize,fileLength - packetNum * maxPaacketSize);
    byte buffer[packetSize];
    file.seek(fileOffset);
    Serial.printf("[LoRaUploader] - send fileOffset%i \n", fileOffset);
    int bytesRead = file.read(buffer,packetSize);
    Serial.printf("[LoRaUploader] - bytesread %i packetsize %i\n", bytesRead, packetSize);
    // if (bytesRead < maxPaacketSize)
    // {
        
    //     Serial.printf("[LoRaUploader] - fin\n");
    //     endMessage = true;
    // }
    if (bytesRead < maxPaacketSize)
    {
        
        Serial.printf("[LoRaUploader] - fin1\n");
        endMessage = true;
    }
    //LoRa.getRadio().startTransmit(buffer, sizeof(buffer));
    LoRa.sendData(id,LoRaMessageCode::FilePacket,String(buffer,sizeof(buffer)));
    _millis = millis();
    for (int i = 0; i < bytesRead; i++) {
        Serial.print(buffer[i]);
        Serial.print(" ");
    }
        Serial.print("\n");
    //fileOffset += maxPaacketSize;
}

void LoRaFileUploader::nextPacket()
{
    waitResponse = false;
    attempt = 0;
    if (hasJustStarted)
    {
        hasJustStarted = false;
        return;
    }
    
    fileOffset += maxPaacketSize;
    packetNum++;
}

LoRaFileUploader LoRaFileUpl;