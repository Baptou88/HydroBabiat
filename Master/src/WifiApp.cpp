#include <WifiApp.h>

extern const char* WIFISSID ;
extern const char* WIFIPASSWORD ;
extern int modeActuel ;
extern PIDController* pidC;
extern manuelController* manuelC;
extern basicController* bC;

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
  // if (WifiApp.ws.count())
  // {
  //   /* code */
  // }
  
  String message = "{";
  message += "\"LED\":" +  (String)LED +",";
  message += "\"Mode\":" +  (String)modeActuel +",";
  message += "\"positionVanne\":" +  (String)(dataTurbine.positionVanne )+",";
  message += "\"RangePosVanneTarget\":" +  (String)(dataTurbine.targetPositionVanne )+",";
  message += "\"TurbineRSSI\":" +  (String)TurbineStatus.RSSI +",";
  message += "\"TurbineSNR\":" +  (String)TurbineStatus.SNR +",";
  message += "\"EtangRSSI\":" +  (String)EtangStatus.RSSI +",";
  message += "\"EtangSNR\":" +  (String)EtangStatus.SNR +",";
  message += "\"niveauEtang\":" +  (String)dataEtang.niveauEtang +",";
  message += "\"ratioNiveauEtang\":" +  (String)dataEtang.ratioNiveauEtang +",";
  message += "\"POT\":" +  (String)potValue;
  message += "}";
  //Serial.print("[WiFiAPP] notif: ");
  //Serial.println(message);
  WifiApp.ws.textAll(String(message));
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
    return (String) (dataEtang.ratioNiveauEtang * 100);
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
        basicController* c;
        c = reinterpret_cast<basicController*> (modes.get(i));
        retour += "<p>" + String(c->niveauMin) +" " + String(c->niveauMax) + "</p>";
        retour += "<img src=\"icons/Basic.svg\" alt=\"\"";
      }
      retour += +"</li>";
      
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
  
  
  
  
  
 return "templateProcesor default: " + var;
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
      request->send(SPIFFS,"/home.html","text/html",false,WifiApp.templateProcessor);
    });
    server.on("/icons/hydro-elec-512.svg" ,HTTP_GET,[](AsyncWebServerRequest *request) {
      request->send(SPIFFS,"/icons/hydro-elec-512.svg");
    });
    server.on("/icons/favicon.ico" ,HTTP_GET,[](AsyncWebServerRequest *request) {
      request->send(SPIFFS,"/icons/favicon.ico");
    });

    server.on("/mode",HTTP_GET,[](AsyncWebServerRequest * request){
      if(request->hasParam("modeNum")){
        
        AsyncWebParameter* p = request->getParam("modeNum");
        Serial.println("mode num : "+ (String) p->value().toInt());
        modeActuel=p->value().toInt();

      }
      request->send(200, "text/plain", "mode ok");
    });

    SPIFFS_provide_file("/app.js");

    SPIFFS_provide_file("/icons/Basic.svg");
    SPIFFS_provide_file("/icons/PID.svg");

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
      etatLed != etatLed;
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


WifiAppClass WifiApp;