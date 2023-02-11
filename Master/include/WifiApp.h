#ifndef WIFIAPP_H
#define WIFIAPP_H

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <configVariables.h>
#include "main.h"
#include "AsyncTelegram2.h"
#include "TelegramCredentials.h"


#include <IController.h>
#include <basicController.h>
#include <manuelController.h>
#include <PIDController.h>
#include <LList.h>

extern int potValue;
extern bool ledNotif;
extern int modeActuel;
extern LList<IController*> modes;

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
    void notifyClient(uint32_t clientId);

    void onNotFound(AsyncWebServerRequest *request);

    static String templateProcessor(const String& var);

    /// @brief main loop
    void loop();
};


extern WifiAppClass WifiApp;

#endif // WIFIAPP_H