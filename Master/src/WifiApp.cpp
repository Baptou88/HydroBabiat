#include <WifiApp.h>
#include "wifiCredentials.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>

extern int modeActuel ;
extern PIDController* pidC;
extern manuelController* manuelC;
extern basicController* bC;
extern AsyncTelegram2 TelegramBot;

#define WIFIAPP_SERVER_PROVIDE_FILE(filename) \
Serial.println(#filename);\
Serial.println("#filename");\
server.on(#filename ,HTTP_GET,[](AsyncWebServerRequest *request) { \
  Serial.println(#filename);\
        request->send(SPIFFS,#filename);\
    });


WifiAppClass::WifiAppClass(/* args */)
{
    
}

WifiAppClass::~WifiAppClass()
{
}
void WifiAppClass::notifyClients(){

  String message = "{";

  message += "\"Mode\":" +  (String)modeActuel +",";
  message += dataTurbine.toJson()+",";
  message += TurbineStatus.toJson()+",";
  message += dataEtang.toJson()+",";
  message += EtangStatus.toJson();
 
  message += "}";
  //Serial.print("[WiFiAPP] notif: ");
  //Serial.println(message);
  WifiApp.ws.textAll(String(message));
}
void WifiAppClass::notifyClient(uint32_t clientId){

  String message = "{";

  message += "\"Mode\":" +  (String)modeActuel +",";
  message += dataTurbine.toJson()+",";
  message += TurbineStatus.toJson()+",";
  message += dataEtang.toJson()+",";
  message += EtangStatus.toJson();
 
  message += "}";
  //Serial.print("[WiFiAPP] notif: ");
  //Serial.println(message);
  WifiApp.ws.text(clientId,message);
}

String WifiAppClass::templateProcessor(const String& var) {
  if (var == "RangePosVanneTarget")
  {
    return (String)dataTurbine.targetPositionVanne;
  }
  if (var == "positionVanne")
  {
    return (String) dataTurbine.positionVanne;
  }
  
  if (var == "niveauEtang")
  {
    return (String) dataEtang.niveauEtang;
  }
  if (var == "ratioNiveauEtang")
  {
    return (String) (dataEtang.ratioNiveauEtang );
  }
  if (var == "mode")
  {
    //return "test";
    return (String) modes[modeActuel]->name;
  }
  if (var == "modes")
  {
    String retour = "";
    for (size_t i = 0; i < modes.size(); i++)
    {
      retour += "<li class=\"list-group-item "+ String((i == modeActuel)? "active" : "") + "\" data-num=\"" + String(i) + "\">"+(String) modes.get(i)->name ;
      if (modes.get(i)->type == typeController::PID)
      {
        PIDController* c ;
        c = reinterpret_cast<PIDController*> (modes.get(i));
        retour += "<p>" + (String)c->targetEtang + "</p>";
        retour += "<img src=\"icons/PID.svg\" alt=\"\"";
        
      } else if (modes.get(i)->type == typeController::basic)

        
        {
            basicController *c;
            c = reinterpret_cast<basicController *>(modes.get(i));
            retour += "<p>" + String(c->niveauMin) + " " + String(c->niveauMax) + "</p>";
            retour += "<img src=\"icons/Basic.svg\" alt=\"\"";
        }
      retour += +"</li>\n";
      
    }
    return retour;
  }
  if (var == "pidc.kp") 
  {
    return (String)pidC->kp; 
  }
  if (var == "pidc.ki") 
  {
    return (String)pidC->ki; 
  }
  if (var == "pidc.kd") 
  {
    return (String)pidC->kd; 
  }
  if (var == "bc.min")
  {
    return (String)bC->niveauMin;
  }
  if (var == "bc.max")
  {
    return (String)bC->niveauMax;
  }
  if (var == "tacky")
  {
    return (String)dataTurbine.tacky;
  }
  if (var == "tensionBatterie")
  {
    return (String)dataTurbine.UB;
  }
  if (var == "tension")
  {
    return (String)dataTurbine.U;
  }
  if (var == "motorState")
  {
    return (String)MotorStateToString(dataTurbine.motorState);
  }
  
  
  
  
  
 return "templateProcesor default: " + var;
}

void WifiAppClass::onNotFound(AsyncWebServerRequest *request){
  //Handle Unknown Request
  request->send(404);
}

bool WifiAppClass::begin()
{
    
    
    if(!SPIFFS.begin(true)){
      Serial.println("[WiFiApp] SPIFFS begin failed");
      return false;
    }
    WiFi.begin(WIFISSID,WIFIPASSWORD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("[WiFiApp] Wifi begin failed");
      return false;
    }
    Serial.println("[WiFiApp] IP: " + (String)WiFi.localIP().toString());
    server.on("/" ,HTTP_GET,[](AsyncWebServerRequest *request) {
      //Serial.println("[SERVER] get root");
      request->send(SPIFFS,"/home.html","text/html",false,WifiApp.templateProcessor);
    });
    server.on("/icons/hydro-elec-512.svg" ,HTTP_GET,[](AsyncWebServerRequest *request) {
      request->send(SPIFFS,"/icons/hydro-elec-512.svg");
    });
    server.on("/icons/favicon.ico" ,HTTP_GET,[](AsyncWebServerRequest *request) {
      request->send(SPIFFS,"/icons/favicon.ico");
    });

    server.on("/mode",HTTP_GET,[](AsyncWebServerRequest * request){
      if (!request->authenticate("admin","admin"))
      {
        return request->requestAuthentication();
      }
      
      if(request->hasParam("modeNum")){
        
        AsyncWebParameter* p = request->getParam("modeNum");
        Serial.println("mode num : "+ (String) p->value().toInt());
        TelegramBot.sendTo(CHAT_ID,"Changement de Mode");

        modes.get(modeActuel)->endMode();
        modeActuel=p->value().toInt();
        modes.get(modeActuel)->startMode();

      }
      request->send(200, "text/plain", "mode ok");
    });

    server.on("/dataEtang",HTTP_GET,[](AsyncWebServerRequest *request){
      String Response = "{"; 
      Response += "\"niveauEtang\":" + (String)dataEtang.niveauEtang + ",";
      Response += "\"niveauEtangP\":" + (String)dataEtang.ratioNiveauEtang;
      Response += "}";
      request->send(200,"application/json",Response);

    });

    SPIFFS_provide_file("/app.js");

    SPIFFS_provide_file("/icons/Basic.svg");
    SPIFFS_provide_file("/icons/PID.svg");


    server.onNotFound([](AsyncWebServerRequest *request){
      return request->send(404);
    });

    ws.onEvent(WifiApp.onEvent);
    server.addHandler(&ws);
    server.begin();
    return true;
}

void WifiAppClass::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      WifiApp.notifyClient(client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
void WifiAppClass::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    Serial.print("[WiFiApp] WS received: ");
    Serial.print((char*)data);
    Serial.println(".");
    String dataStr = (char*) data;
    if ((char*)data == "toggle")
    {
      Serial.println("ws changement led");
      ledNotif != ledNotif;
    } else if (dataStr.startsWith("RangePosVanneTarget"))
    {
      dataStr.replace("RangePosVanneTarget ","");
      Serial.println("niveu vanne: "+ (String)dataStr.toInt());
      dataTurbine.targetPositionVanne = dataStr.toInt();
      bufferActionToSend += "TURBINE:TargetVanne=" + (String)dataStr.toInt()+";";
    } else if (dataStr.startsWith("Action:"))
    {
      dataStr.replace("Action:","");
      Serial.println("Action: "+ dataStr);
      bufferActionToSend += dataStr;
    } else if (dataStr.startsWith("pidc."))
    {
    dataStr.replace("pidc.","");
    if (dataStr.startsWith("kp="))
    {
      dataStr.replace("kp=","");
      pidC->kp = dataStr.toFloat();
    }
    if (dataStr.startsWith("ki="))
    {
      dataStr.replace("ki=","");
      pidC->ki = dataStr.toFloat();
    }
    if (dataStr.startsWith("kd="))
    {
      dataStr.replace("kd=","");
      pidC->kd = dataStr.toFloat();
    }
    
    } else if (dataStr.startsWith("bc."))
    {
    dataStr.replace("bc.","");
    if (dataStr.startsWith("min="))
    {
      dataStr.replace("min=" , "");
      bC->niveauMin = dataStr.toInt();
    }
    if (dataStr.startsWith("max="))
    {
      dataStr.replace("max=" , "");
      bC->niveauMin = dataStr.toInt();
    }
   
  }
    
  else
  {
    Serial.println("ah bon ??");
  }
   
   
    WifiApp.notifyClients();
  }
}

void WifiAppClass::SPIFFS_provide_file(const char* filename)
{
  server.on(filename ,HTTP_GET,[filename](AsyncWebServerRequest *request) {
    request->send(SPIFFS,filename);
  });
}

void WifiAppClass::loop(){
  ws.cleanupClients();
}

WifiAppClass WifiApp;