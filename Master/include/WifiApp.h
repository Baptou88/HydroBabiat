#ifndef WIFIAPP_H
#define WIFIAPP_H



#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <SPIFFS.h>
#include <configVariables.h>
#include "main.h"
#include "AsyncTelegram2.h"
#include "TelegramCredentials.h"
#include "ProgrammatedTasks.h"
#include "LoRaFileUploader.h"

#include <IController.h>
#include <basicController.h>
#include <manuelController.h>
#include <PIDController.h>
#include <LList.h>
#include "AlertNiveau.h"


#define WS_ENABLED true

extern DNSServer dnsserver;

extern int potValue;
extern bool ledNotif;
extern int modeActuel;
extern LList<IController*> modes;


// #define WIFIAPP_SERVER_PROVIDE_FILE(filename) \
// server.on("#filename" ,HTTP_GET,[](AsyncWebServerRequest *request) { \
//     request->send(SPIFFS,"#filename"); \
// });
    
#define USERNAME_ADMIN "admin"
#define PASSWORD_ADMIN "test"
#define USERNAME_User "user"
#define PASSWORD_User "user"
class WifiAppClass
{
private:
    static void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len);
    static void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);

    #ifdef ASYNC_TCP_SSL_ENABLED
    AsyncWebServer server = AsyncWebServer(5555);
    #else
    AsyncWebServer* server = new AsyncWebServer(80);
    #endif

    #if WS_ENABLED
    AsyncWebSocket* ws = new AsyncWebSocket("/ws");
    #endif

    void SPIFFS_provide_file(const char* filename);
public:
    WifiAppClass(/* args */);
    ~WifiAppClass();
    bool begin();
    bool close();

    void notifyClients();
    void notifyClient(uint32_t clientId);

    void monitorClients(String message);

    void toastClients(String title,String message,String type);

    static void onNotFound(AsyncWebServerRequest *request);

    static String templateProcessor(const String& var);
    static String templateProcessorAdmin(const String& var);
    static String templateProcessorUser(const String& var);

    static bool sendInternalServerError(AsyncWebServerRequest *request);

    void ws_Sendall(String msg);
    void ws_Send(uint32_t client_id,String msg);

    /// @brief main loop
    void loop();
};


extern WifiAppClass WifiApp;

#endif // WIFIAPP_H