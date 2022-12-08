#ifndef WIFIAPP_H
#define WIFIAPP_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <configVariables.h>
#include "main.h"


extern int potValue;
extern bool etatLed;

// #define WIFIAPP_SERVER_PROVIDE_FILE(filename) \
// server.on("#filename" ,HTTP_GET,[](AsyncWebServerRequest *request) { \
//     request->send(SPIFFS,"#filename"); \
// });
    
class WifiAppClass
{
private:
    static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

    AsyncWebServer server= AsyncWebServer(80);
    AsyncWebSocket ws = AsyncWebSocket("/ws");
    void SPIFFS_provide_file(const char* filename);
public:
    WifiAppClass(/* args */);
    ~WifiAppClass();
    bool begin();
    void notifyClients();

    static String templateProcessor(const String& var);
};


extern WifiAppClass WifiApp;

#endif // WIFIAPP_H