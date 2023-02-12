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

  String message = "{";

  message += "\"Mode\":" + (String)modeActuel + ",";
  message += dataTurbine.toJson() + ",";
  message += TurbineStatus.toJson() + ",";
  message += dataEtang.toJson() + ",";
  message += EtangStatus.toJson();

  message += "}";
  // Serial.print("[WiFiAPP] notif: ");
  // Serial.println(message);
  WifiApp.ws.textAll(String(message));
}
void WifiAppClass::notifyClient(uint32_t clientId)
{

  String message = "{";

  message += "\"Mode\":" + (String)modeActuel + ",";
  message += dataTurbine.toJson() + ",";
  message += TurbineStatus.toJson() + ",";
  message += dataEtang.toJson() + ",";
  message += EtangStatus.toJson();

  message += "}";
  // Serial.print("[WiFiAPP] notif: ");
  // Serial.println(message);
  WifiApp.ws.text(clientId, message);
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
  if (var == "motorState")
  {
    return (String)MotorStateToString(dataTurbine.motorState);
  }

  return "templateProcesor default: " + var;
}

String WifiAppClass::templateProcessorTask(const String &var)
{
  String retour = "";

  if (var == "ListeProgram")
  {

    for (size_t i = 0; i < ProgrammatedTasks->size(); i++)
    {
      // retour += (String)ProgrammatedTasks->get(i)->name;
      retour += "<div x-data=\"{edit:false, name:'" + (String)ProgrammatedTasks->get(i)->name + "'}\" class=\"card border my-2 \" style=\"width: auto\">\n";
      retour += "<form  action =\"/updateprogrammateur\" method=\"post\">\n";
      // retour += "<h3>" + (String)ProgrammatedTasks->get(i)->name + "</h3>";
      retour += "<div class=\"card-header\">\n";
      retour += "<div class=\"form-check form-switch\" >\n";
      retour += "<input class=\"form-check-input\" type=\"checkbox\" name=\"active\" role=\"switch\" id=\"flexSwitchCheckChecked\" " + String("\%ProgrammatedTasks" + String(i) + ":isActive\%") + " onclick=\"update(this)\">";
      // retour += "<label class=\"form-check-label\" for=\"flexSwitchCheckChecked\">Checked switch checkbox input</label>";
      retour += "<button class=\"btn btn-outline-success\" x-show=\"!edit\" @click.prevent=\"edit = ! edit\">Toggle</button>\n";
      retour += "<div x-show=\"!edit\" >\n<h5  class=\"card-title\" x-text=\"name\"></h5>\n</div>\n";
      retour += "<input x-show=\"edit\" @click.outside=\"edit = false\" type=\"text\" name=\"name\" id=\"\" x-model=\"name\">\n";

      retour += "</div>\n";
      retour += "</div>\n";
      retour += "<div class=\"form-group\" hidden>\n";
      retour += "<label for=\"exampleFormControlInput1\">ID</label>\n";
      retour += "<input type=\"number\" class=\"form-control\" name=\"id\" id=\"exampleFormControlInput1\" placeholder=\"id\" value=\"" + (String)i + "\" hidden>\n";
      retour += "</div>\n";
      retour += "<label for=\"appt\">Heure du déclanchement:</label>\n";
      retour += "<input type=\"time\" id=\"appt\" name=\"appt\"  value= \"\%ProgrammatedTasks" + String(i) + ":getHours\%" + ":" + "\%ProgrammatedTasks" + String(i) + ":getMinutes\%" + "\"  required>\n";
      retour += "<label for=\"customRange1\" class=\"form-label\">Example range</label>";
      retour += "<input type=\"range\" value=\"\%ProgrammatedTasks" + String(i) + ":targetVanne\%" + "\" name=\"targetVanne\" class=\"form-range\" id=\"customRange1\">";
      retour += "<label for=\"deepsleep\">DeepSleep (ms) :</label>\n";
      retour += "<input type=\"number\" id=\"appte\" name=\"deepsleep\"  value= \"\%ProgrammatedTasks" + String(i) + ":deepsleep\%" + "\"  required>\n";
      retour += "<button class=\"btn btn-primary\" type=\"submit\">Mettre a jour</button>";
      retour += "<a href=\"/programmateur/?delete=" + (String)i + "\" class=\"btn btn-danger\">";
      retour += "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"16\" height=\"16\" fill=\"white\" class=\"bi bi-trash\" viewBox=\"0 0 16 16\">";
      retour += "<path d=\"M5.5 5.5A.5.5 0 0 1 6 6v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5zm2.5 0a.5.5 0 0 1 .5.5v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5zm3 .5a.5.5 0 0 0-1 0v6a.5.5 0 0 0 1 0V6z\"/>";
      retour += "<path fill-rule=\"evenodd\" d=\"M14.5 3a1 1 0 0 1-1 1H13v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V4h-.5a1 1 0 0 1-1-1V2a1 1 0 0 1 1-1H6a1 1 0 0 1 1-1h2a1 1 0 0 1 1 1h3.5a1 1 0 0 1 1 1v1zM4.118 4 4 4.059V13a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.059L11.882 4H4.118zM2.5 3V2h11v1h-11z\"/>";
      retour += "</svg>";
      retour += "</a>";
      retour += "</form>";
      retour += "</div>\n";
    }

    return retour;
  }
  else if (var.startsWith("ProgrammatedTasks"))
  {
    String temp = var;
    temp.replace("ProgrammatedTasks", "");
    int separateur = temp.indexOf(":");
    int num_tache = temp.substring(0, separateur).toInt();
    String methode = temp.substring(separateur + 1, 64);
    if (methode == "getHours")
    {
      return ProgrammatedTasks->get(num_tache)->getHours();
    }
    if (methode == "getMinutes")
    {
      return ProgrammatedTasks->get(num_tache)->getMinutes();
    }
    if (methode == "isActive")
    {
      return String(ProgrammatedTasks->get(num_tache)->isActive() ? "checked" : "");
    }
    if (methode == "targetVanne")
    {
      return String(ProgrammatedTasks->get(num_tache)->targetVanne);
    }
    if (methode == "deepsleep")
    {
      return String(ProgrammatedTasks->get(num_tache)->deepsleep);
    }

    return "erreur processing ProgrammatedTasks";
  }
  else
  {
    retour += "Erreur processing task";
  }

  return retour;
}

void WifiAppClass::onNotFound(AsyncWebServerRequest *request)
{
  // Handle Unknown Request
  request->send(404);
}

bool WifiAppClass::begin()
{

  if (!SPIFFS.begin(true))
  {
    Serial.println("[WiFiApp] SPIFFS begin failed");
    return false;
  }
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
    request->send(SPIFFS,"/home.html","text/html",false,WifiApp.templateProcessor); 
  });
  server.on("/icons/hydro-elec-512.svg", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/icons/hydro-elec-512.svg"); 
  });
  server.on("/icons/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/icons/favicon.ico"); 
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
		ProgrammatedTasks->add(new ProgrammatedTask(12,12,"test ajout"));
		
		//request->send(SPIFFS,"/programmateur.html", "text/html", false, processor);
		request->redirect("/programmateur"); 
  });

  server.on("/programmateur/sauvegarder", HTTP_GET, [](AsyncWebServerRequest *request)
  {
		File file = SPIFFS.open("/Programmated","w+");

		for (size_t i = 0; i < ProgrammatedTasks->size(); i++)
		{
			ProgrammatedTask *test = ProgrammatedTasks->get(i);
			
			file.print("name=" + String(test->name) + ";");
			file.print("h=" + String(test->h) + ";");
			file.print("m=" + String(test->m) + ";");
			file.print("activate=" + String(test->isActive()) + ";");
			file.print("targetVanne=" + String(test->targetVanne) + ";");
			file.println("deepsleep=" + String(test->deepsleep) + ";");
		}
		file.close();
		
		request->send(SPIFFS,"/Programmated","text/plaintext"); 
  });

  server.on("/programmateur", HTTP_GET, [](AsyncWebServerRequest *request)
  { 
    request->send(SPIFFS, "/programmateur.html", "text/html", false, WifiApp.templateProcessorTask); 
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
			test = ProgrammatedTasks->get(request->getParam("id",true)->value().toInt());
			
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

  
  SPIFFS_provide_file("/app.js");
  SPIFFS_provide_file("/Programmateur.js");
  SPIFFS_provide_file("/data.csv");
  SPIFFS_provide_file("/icons/Basic.svg");
  SPIFFS_provide_file("/icons/PID.svg");

  server.onNotFound([](AsyncWebServerRequest *request)
                    { return request->send(404); });

  ws.onEvent(WifiApp.onEvent);
  server.addHandler(&ws);
  server.begin();
  return true;
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

    else
    {
      Serial.println("ah bon ??");
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