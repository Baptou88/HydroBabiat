#include <WifiApp.h>
#include "wifiCredentials.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <NTPClient.h>
#include <ProgrammatedTask.h>

extern int modeActuel;
extern PIDController *pidC;
extern manuelController *manuelC;
extern basicController *bC;
extern AsyncTelegram2 TelegramBot;

extern NTPClient timeClient;

extern LList<ProgrammatedTask *> *ProgrammatedTasks;


#define WIFIAPP_SERVER_PROVIDE_FILE(filename) \
  Serial.println(#filename);                  \
  Serial.println("#filename");                \
  server.on(#filename, HTTP_GET, [](AsyncWebServerRequest *request) { \
  Serial.println(#filename);\
        request->send(SPIFFS,#filename); });

WifiAppClass::WifiAppClass(/* args */)
{
}

WifiAppClass::~WifiAppClass()
{
}
void WifiAppClass::notifyClients()
{

  String message = "{\"data\":{";

  message += "\"Mode\":" + (String)modeActuel + ",";
  message += "\"freeheap\":" + (String)ESP.getFreeHeap() + ",";
  message += dataTurbine.toJson() + ",";
  message += TurbineStatus.toJson() + ",";
  message += dataEtang.toJson() + ",";
  message += EtangStatus.toJson() + ",";
  message += nodeTest.toJson();

  message += "}}";
  // Serial.print("[WiFiAPP] notif: ");
  // Serial.println(message);
  WifiApp.ws.textAll(String(message));
}
void WifiAppClass::notifyClient(uint32_t clientId)
{

  String message = "{\"data\":{";

  message += "\"Mode\":" + (String)modeActuel + ",";
  message += "\"freeheap\":" + (String)ESP.getFreeHeap() + ",";
  message += dataTurbine.toJson() + ",";
  message += TurbineStatus.toJson() + ",";
  message += dataEtang.toJson() + ",";
  message += EtangStatus.toJson();

  message += "}}";
  // Serial.print("[WiFiAPP] notif: ");
  // Serial.println(message);
  WifiApp.ws.text(clientId, message);
}

void WifiAppClass::monitorClients(String message)
{
  String msg = "{\"monitor\":\" "+ message + "\"}";
  WifiApp.ws.textAll(msg);
}

String WifiAppClass::templateProcessor(const String &var)
{
  if (var == "RangePosVanneTarget")
  {
    return (String)dataTurbine.targetPositionVanne;
  }
  if (var == "positionVanne")
  {
    return (String)dataTurbine.positionVanne;
  }
  if (var == "niveauEtang")
  {
    return (String)dataEtang.niveauEtang;
  }
  if (var == "ratioNiveauEtang")
  {
    return (String)(dataEtang.ratioNiveauEtang);
  }
  if (var == "niveauEtangRempli")
  {
    return (String)(dataEtang.niveauEtangRempli);
  }
  if (var == "niveauEtangVide")
  {
    return (String)(dataEtang.niveauEtangVide);
  }
  if (var == "mode")
  {
    // return "test";
    return (String)modes[modeActuel]->name;
  }
  if (var == "modes")
  {
    String retour = "";
    for (size_t i = 0; i < modes.size(); i++)
    {
      retour += "<li class=\"list-group-item " + String((i == modeActuel) ? "active" : "") + "\" data-num=\"" + String(i) + "\">" + (String)modes.get(i)->name;
      if (modes.get(i)->type == typeController::PID)
      {
        PIDController *c;
        c = reinterpret_cast<PIDController *>(modes.get(i));
        retour += "<p>" + (String)c->targetEtang + "</p>";
        retour += "<img src=\"icons/PID.svg\" alt=\"\"";
      }
      else if (modes.get(i)->type == typeController::basic)

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
  if (var == "intensite")
  {
    return (String)dataTurbine.I;
  }
  if (var == "power")
  {
    return (String)dataTurbine.getPower();
  }
  if (var == "motorState")
  {
    return (String)MotorStateToString(dataTurbine.motorState);
  }
  if (var == "AlertNivActif")
  {
    
    if (AlertNiv.active)
    {
      return "checked";
    } else
    {
      return "";
    }
    
    
  }
  if (var == "AlertNivMax")
  {
    return (String)AlertNiv.max;
  }
  if (var == "AlertNivMin")
  {
    return (String)AlertNiv.min;
  }
  
  return "templateProcesor default: " + var;
}

bool WifiAppClass::sendInternalServerError(AsyncWebServerRequest *request)
{
  String tempStr;
  tempStr += "<html>";
  tempStr += "<head>";
  tempStr += "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css\" rel=\"stylesheet\"";
  tempStr += "</head>";
  tempStr += "<body>";
  tempStr += "<div class=\"container\">";
  tempStr += "<h1>500 Internal Server Error</h1>";
  tempStr += "<button class=\" btn btn-outline-danger\" type=\"button\" onclick=\"sendRebootCmd(this)\">";
  tempStr += "<span class=\"spinner-border spinner-border-sm visually-hidden\" role=\"status\" aria-hidden=\"true\"></span>";
  tempStr += " Restart Esp";
  tempStr += "</button>";
  tempStr += "</div>";
  tempStr += "</body>";
  tempStr += "<script>";
  tempStr += "function sendRebootCmd(element){\n";
  tempStr += "element.disabled = true\n";
  tempStr += "element.querySelector('.spinner-border').classList.remove('visually-hidden')\n";
  tempStr += "fetch('/reboot')\n";
  tempStr += ".then(response => {\n";
  tempStr += "if (response.ok) {\n";
  tempStr += "setTimeout(() => {\n";
  tempStr += "window.location.href = window.location.href; // rediriger l'utilisateur vers la même page\n";
  tempStr += "}, 5000); // attendre 5 secondes (5000 millisecondes) avant de rediriger\n";
  tempStr += "}\n";
  tempStr += "})\n";
  tempStr += ".catch(error => {\n";
  tempStr += "console.error('Une erreur est survenue :', error);\n";
  tempStr += "})\n";
  tempStr += ".finally( () => {\n";
  tempStr += "element.disabled = false \n";
  tempStr += "element.querySelector('.spinner-border').classList.add('visually-hidden')\n";
  tempStr += "})\n";
  tempStr += "}\n";


  tempStr += "</script>";
  tempStr += "</html>";
  request->send(500,"text/html",tempStr);
  return false;
}

void WifiAppClass::onNotFound(AsyncWebServerRequest *request)
{
  String retour;
  // Handle Unknown Request
  retour += ("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      retour += ("GET");
    else if(request->method() == HTTP_POST)
      retour += ("POST");
    else if(request->method() == HTTP_DELETE)
      retour += ("DELETE");
    else if(request->method() == HTTP_PUT)
      retour += ("PUT");
    else if(request->method() == HTTP_PATCH)
      retour += ("PATCH");
    else if(request->method() == HTTP_HEAD)
      retour += ("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      retour += ("OPTIONS");
    else
      retour += ("UNKNOWN");

    retour += "\n";

    retour += " http://" + (String)request->host().c_str()+ request->url().c_str() +"\n";

    if(request->contentLength()){
      retour += "_CONTENT_TYPE: " + (String)request->contentType().c_str() + "\n";
      retour += "_CONTENT_LENGTH: " + (String)request->contentLength() +"\n" ;
    }

    retour += "\n";
    retour += "Headers: \n";
    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      retour += "_HEADER[" + (String)h->name().c_str() + "]: " + h->value().c_str() + "\n";
    }

    retour += "\n";
    retour += "Params: \n";
    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        retour += "_FILE[" + (String)p->name().c_str() + "]: " + p->value().c_str() + ", size: " + p->size() + " \n" ;
      } else if(p->isPost()){
        retour += "_POST[" +  (String)p->name().c_str() +"]: " + p->value().c_str() + "\n";
      } else {
        retour += "_GET[" + (String)p->name().c_str() + "]:  " + p->value().c_str() + "\n";
      }
    }
    
  request->send(404,"text/plaintext",retour);
  
}

bool WifiAppClass::begin()
{

  if (!SPIFFS.begin(true))
  {
    Serial.println("[WiFiApp] SPIFFS begin failed");
    return false;
  }
  //WiFi.setHostname("Esp32S3_HydroBabiat");
  WiFi.begin(WIFISSID, WIFIPASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    Serial.println("[WiFiApp] Wifi begin failed");
    return false;
  }
  timeClient.begin();
  if (!timeClient.update())
  {
    timeClient.forceUpdate();
  }
  Serial.println("Time: " + (String)timeClient.getFormattedTime());

  Serial.println("[WiFiApp] IP: " + (String)WiFi.localIP().toString());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    //Serial.println("[SERVER] get root");
    if (SPIFFS.exists("/home.html"))
    {
      request->send(SPIFFS,"/home.html","text/html",false,WifiApp.templateProcessor); 
      
    } else
    {
      sendInternalServerError(request);
    }
    
    
  });
  server.on("/reboot",HTTP_GET,[](AsyncWebServerRequest *request){
    startReboot = millis() + 2000;
    request->send(200,"text/plaintext","ok");
  });
  server.on("/icons/hydro-elec-512.svg", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/icons/hydro-elec-512.svg"); 
  });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/icons/favicon.ico"); 
  });
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/icons/favicon-192.png"); 
  });

  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request)
  {
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

  server.on("/dataEtang", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    String Response = "{";
    Response += "\"niveauEtang\":" + (String)dataEtang.niveauEtang + ",";
    Response += "\"niveauEtangP\":" + (String)dataEtang.ratioNiveauEtang;
    Response += "}";
    request->send(200, "application/json", Response);
  });

  server.on("/programmateur/new", HTTP_GET, [](AsyncWebServerRequest *request)
  {
		//ProgrammatedTasks->add(new ProgrammatedTask(12,12,"test ajout"));
		ProgTasks.addTask(new ProgrammatedTask(12,12,"test ajout"));
		//request->send(SPIFFS,"/programmateur.html", "text/html", false, processor);
		request->redirect("/programmateur"); 
  });

  server.on("/programmateur/sauvegarder", HTTP_GET, [](AsyncWebServerRequest *request)
  {
		ProgTasks.saveTask();
		
		request->send(SPIFFS,"/Programmated","text/plaintext"); 
  });

  server.on("/programmateur", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/programmateur.html", "text/html", false, ProgTasks.templateProcessor); 
  });

  server.on("/updateprogrammateur", HTTP_POST , [](AsyncWebServerRequest* request) {
		
		
		int params = request->params();
		for(int i=0;i<params;i++){
			AsyncWebParameter* p = request->getParam(i);
			if(p->isFile()){ //p->isPost() is also true
				Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
			} else if(p->isPost()){
				Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
			} else {
				Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
			}
		}
		//List all parameters (Compatibility)
		int args = request->args();
		for(int i=0;i<args;i++){
			Serial.printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
		}
		ProgrammatedTask *test;
		if (request->hasParam("id",true))
		{
			//test = ProgrammatedTasks->get(request->getParam("id",true)->value().toInt());
			test = ProgTasks.getTask(request->getParam("id",true)->value().toInt());
		} else
		{
			request->send(400);
			return;
		}
    
		if (request->hasParam("name",true))
    {
      test->name = request->getParam("name",true)->value();
    }
    
		
		if (request->hasParam("active",true))
		{
			test->activate();
		} else
		{
			test->deactivate();
		}
		if (request->hasParam("appt",true))
		{
			int dp = request->getParam("appt",true)->value().indexOf(":");
			Serial.println("dp " + String(dp));
			test->h = request->getParam("appt",true)->value().substring(0,dp).toInt();
			test->m = request->getParam("appt",true)->value().substring(dp+1,10).toInt();
		}
		if (request->hasParam("targetVanne",true))
		{
			test->targetVanne = request->getParam("targetVanne",true)->value().toInt();
		}
		if (request->hasParam("deepsleep",true))
		{
			test->deepsleep = request->getParam("deepsleep",true)->value().toDouble();
		}
		
		
		
		
		request->send(200,"text/json","{\"ok\":1}");
  });

  server.on("/data.csv",HTTP_GET,[](AsyncWebServerRequest * request){
    request->send(SPIFFS,"/data.csv","text/csv");
  });
  
  SPIFFS_provide_file("/app.js");
  SPIFFS_provide_file("/theme.js");
  SPIFFS_provide_file("/fileSystem.js");
  SPIFFS_provide_file("/Programmateur.js");
  SPIFFS_provide_file("/rSlider.js");
  SPIFFS_provide_file("/rSlider.css");
  
  SPIFFS_provide_file("/icons/Basic.svg");
  SPIFFS_provide_file("/icons/PID.svg");
  //SPIFFS_provide_file("/fileSystem.html");
  server.on("/fileSystem",HTTP_ANY,[](AsyncWebServerRequest * request){
    Serial.println("request method: "+ (String)request->method());
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){ //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    if (request->method() == HTTP_GET)
    {
     
      request->send(SPIFFS,"/fileSystem.html","text/html",false,[](String var){
      String retour;
        if (var == "freeSpiffs")
        {
          retour += String(SPIFFS.totalBytes() - SPIFFS.usedBytes());
        }
        if (var == "usedSpiffs")
        {
          retour += String(SPIFFS.usedBytes());
        }
        if (var == "totalSpiffs")
        {
          retour += String(SPIFFS.totalBytes());
        }
        
      return retour;
    });
    } else {
      request->send(200,"text/plaintext","par l)");
    }
    
    
  },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
    //if (checkUserWebAuth(request)) {
      Serial.println("par ici : "+ (String)filename);
      if (request->method() != HTTP_POST)
      {
        return;
      }
      

      if (!index) {
        
        // open the file on first call and store the file handle in the request object
        Serial.println("Upload file "+ String(filename));
        request->_tempFile = SPIFFS.open("/" + filename, "w");
        
      }

      if (len) {
        // stream the incoming chunk to the opened file
        request->_tempFile.write(data, len);       
      }

      if (final) {
        // close the file handle as the upload is now done
        int filesize = request->_tempFile.size();
        request->_tempFile.close();
        return request->send(200,"application/json","{\"status\":\"ok\",\"file\":[{\"name\":\"" + String(filename) + "\",\"size\":"+ String(filesize)+"}]}");
        //request->redirect("/");
      }
    // } else {
    //   Serial.println("Auth: Failed");
      

    // }
  });

  server.on("/api/fs",HTTP_DELETE,[](AsyncWebServerRequest * request){
    
    if (request->hasParam("fileName",true))
    {
      String fileName =  (String)request->getParam("fileName",true)->value();
      if (SPIFFS.exists(fileName))
      {
        SPIFFS.remove( (String)fileName);
        request->send(200,"application/json","{\"status\":\"ok\"}");
      }
      request->send(400,"text/plaintext","file \""+ fileName +"\" doesn't exist");
    }
    
  });
  server.on("/api/fs",HTTP_GET,[](AsyncWebServerRequest * request) {
    String response = "";
    response += "{\"SPIFFS\":[";
    
    File fileRoot = SPIFFS.open("/");
    
    // Parcourt tous les fichiers et dossiers de la carte SD
    while (File file = fileRoot.openNextFile()) {
      // Ajoute le nom du fichier ou du dossier au tableau
      if (!response.endsWith("["))
      {
        response += ",";
      }
      
      response += "{\"name\": \"" + (String)file.name() + "\", \"size\": "+ (String)file.size() + ", \"path\": \""+ (String)file.path() + "\"}";
    }
    
    response += "]}";
   
    request->send(200, "application/json",response );
     
  });

  server.on("/sendFile",HTTP_POST,[](AsyncWebServerRequest * request){
    int params = request->params();
    for(int i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){ //p->isPost() is also true
        Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    bool otaUpdate = false;

    if (request->hasParam("Ota",true))
    {
        otaUpdate = true;
    }
    
    if (request->hasParam("fileName",true))
    {
      String fileName =  (String)request->getParam("fileName",true)->value();
      if (SPIFFS.exists(fileName))
      {
        Serial.println("Sending: " + String(fileName));
        //TODO lorafile transfer
        LoRaFileUpl.beginTransmit(fileName,0x04,otaUpdate);
        return request->send(200,"application/json","{\"status\":\"ok\"}");
      }
      return request->send(400,"text/plaintext","file doesn't exist");
    }
      return request->send(400,"text/plaintext","wrong filename");
    
  });

  //server.onNotFound([](AsyncWebServerRequest *request)
   //                 { return request->send(404); });

  server.onNotFound(WifiApp.onNotFound);

  ws.onEvent(WifiApp.onEvent);
  server.addHandler(&ws);
  server.begin();
  return true;
}

bool WifiAppClass::close()
{
  ws.closeAll() ;
  Serial.println("ws closed !");
  //server.removeHandler(&ws);
  Serial.println("handler removed !");
  server.end();
  Serial.println("server closed !");
  return false;
}

void WifiAppClass::onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                           void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
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
void WifiAppClass::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    Serial.print("[WiFiApp] WS received: ");
    Serial.print((char *)data);
    Serial.println(".");
    String dataStr = (char *)data;
    if ((char *)data == "toggle")
    {
      Serial.println("ws changement led");
      ledNotif != ledNotif;
    }
    else if (dataStr.startsWith("RangePosVanneTarget"))
    {
      dataStr.replace("RangePosVanneTarget ", "");
      Serial.println("niveu vanne: " + (String)dataStr.toInt());
      dataTurbine.targetPositionVanne = dataStr.toInt();
      bufferActionToSend += "TURBINE:TargetVanne=" + (String)dataStr.toInt() + ";";
    }
    else if (dataStr.startsWith("Action:"))
    {
      dataStr.replace("Action:", "");
      Serial.println("Action: " + dataStr);
      bufferActionToSend += dataStr;
    }
    else if (dataStr.startsWith("pidc."))
    {
      dataStr.replace("pidc.", "");
      if (dataStr.startsWith("kp="))
      {
        dataStr.replace("kp=", "");
        pidC->kp = dataStr.toFloat();
      }
      if (dataStr.startsWith("ki="))
      {
        dataStr.replace("ki=", "");
        pidC->ki = dataStr.toFloat();
      }
      if (dataStr.startsWith("kd="))
      {
        dataStr.replace("kd=", "");
        pidC->kd = dataStr.toFloat();
      }
    }
    else if (dataStr.startsWith("bc."))
    {
      dataStr.replace("bc.", "");
      if (dataStr.startsWith("min="))
      {
        dataStr.replace("min=", "");
        bC->niveauMin = dataStr.toInt();
      }
      if (dataStr.startsWith("max="))
      {
        dataStr.replace("max=", "");
        bC->niveauMin = dataStr.toInt();
      }
    }
    else if (dataStr.startsWith("testNotif"))
    {
      TelegramBot.sendTo(CHAT_ID,"TestNotif");
    }
    else if (dataStr.startsWith("SavePref"))
    {
      savePref();
    }
    else if (dataStr.startsWith("AlertNiv"))
    {
      dataStr.replace("AlertNiv","");

      if (dataStr.startsWith("Actif="))
      {
        dataStr.replace("Actif=","");
        if (dataStr.startsWith("true"))
        {
          AlertNiv.active = true;
        } else
        {
          AlertNiv.active = false;
        }
      }
      if (dataStr.startsWith("Min="))
      {
        dataStr.replace("Min=","");
        AlertNiv.min = dataStr.toInt();
      }
      if (dataStr.startsWith("Max="))
      {
        dataStr.replace("Max=","");
        AlertNiv.max = dataStr.toInt();
      }
      
      
      
      
      
    }
    else if (dataStr.startsWith("NodeTest."))
      {
        Serial.println(dataStr);
        dataStr.replace("NodeTest.","");
        if (dataStr.startsWith("Active="))
        {
          dataStr.replace("Active=","");
          nodeTest.active = dataStr.toInt();
          Serial.println(nodeTest.active);
        }
        
      }
     else if (dataStr.startsWith("DeepSleepServer"))
    {
      startDeepSleep = millis() + 5000;
    }
  

    WifiApp.notifyClients();
  }
}

void WifiAppClass::SPIFFS_provide_file(const char *filename)
{
  server.on(filename, HTTP_GET, [filename](AsyncWebServerRequest *request)
            { request->send(SPIFFS, filename); });
}

void WifiAppClass::loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    timeClient.update();
  }
  ws.cleanupClients();
}

WifiAppClass WifiApp;