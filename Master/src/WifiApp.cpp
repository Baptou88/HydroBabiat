#include <WifiApp.h>
#include "wifiCredentials.h"
#include <ArduinoJson.h>
#include <AsyncJson.h>

#include <ProgrammatedTask.h>
#include "main.h"
#include "EnergieMeter.h"

extern int modeActuel;
extern PIDController *pidC;
extern manuelController *manuelC;
extern basicController *bC;
extern EnergieMeter Em;

extern bool SpectrumScan;



extern LList<ProgrammatedTask *> *ProgrammatedTasks;

DNSServer dnsserver;

#define WIFIAPP_SERVER_PROVIDE_FILE(filename) \
  Serial.println(#filename);                  \
  Serial.println("#filename");                \
  server.on(#filename, HTTP_GET, [](AsyncWebServerRequest *request) { \
  Serial.println(#filename);\
        request->send(SPIFFS,#filename); });

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    // request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  }
};

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
  message += "\"Energie\":" + (String)Em.getEnergie() + ",";
  message += "\"Notification\":" + (String)Notifi.NotifyIndividuel + ",";
  message += "\"NotificationGroup\":" + (String)Notifi.NotifyGroup + ",";
  message += dataTurbine.toJson() + ",";
  message += "\"turbineStatus\":{" + (String)TurbineStatus.toJson() + "},";
  message += dataEtang.toJson() + ",";
  message += "\"etangStatus\":{" + (String)EtangStatus.toJson() + "},";
  message += dataNodeTest.toJson() + ",";
  message += "\"nodeTestStatus\":{" + (String)RadiateurStatus.toJson() + "}";

  message += "}}";

  WifiApp.ws.textAll(String(message));
}
void WifiAppClass::notifyClient(uint32_t clientId)
{

  String message = "{\"data\":{";

  message += "\"Mode\":" + (String)modeActuel + ",";
  message += "\"freeheap\":" + (String)ESP.getFreeHeap() + ",";
  message += "\"Energie\":" + (String)Em.getEnergie() + ",";
  message += "\"Notification\":" + (String)Notifi.NotifyIndividuel + ",";
  message += "\"NotificationGroup\":" + (String)Notifi.NotifyGroup + ",";
  message += dataTurbine.toJson() + ",";
  message += "\"turbineStatus\":{" + (String)TurbineStatus.toJson() + "},";
  message += dataEtang.toJson() + ",";
  message += "\"etangStatus\":{" + (String)EtangStatus.toJson() + "},";
  message += dataNodeTest.toJson() + ",";
  message += "\"nodeTestStatus\":{" + (String)RadiateurStatus.toJson() + "}";

  message += "}}";

  WifiApp.ws.text(clientId, message);
}

void WifiAppClass::monitorClients(String message)
{
  String msg = "{\"monitor\":\" " + message + "\"}";
  //WifiApp.ws.textAll(msg); //TODO Remettre en place, juste pour tests
}
void WifiAppClass::toastClients(String title, String message, String type)
{
  String msg = "";
  StaticJsonDocument<256> doc;

  JsonObject toast = doc.createNestedObject("toast");
  toast["title"] = title;
  toast["desc"] = message;
  toast["type"] = type;
  Serial.print(msg);
  serializeJson(doc, msg);

  WifiApp.ws.textAll(msg);
}

String WifiAppClass::templateProcessor(const String &var)
{
  if (var == "PositionVanneTarget")
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
  if (var == "bc.target")
  {
    return (String)bC->vanneMax;
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
  if (var == "motorStateStr")
  {
    return (String)MotorStateToString(dataTurbine.motorState);
  }
  if (var == "motorState")
  {
    return (String)dataTurbine.motorState;
  }
  if (var == "AlertNivActif")
  {

    if (AlertNiv.active)
    {
      return "checked";
    }
    else
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
  if (var == "CpuFreq")
  {
    return (String)getCpuFrequencyMhz();
  }
  if (var == "Notification")
  {
    return (String)Notifi.NotifyIndividuel;
  }
  if (var == "NotificationGroup")
  {
    return (String)Notifi.NotifyGroup;
  }

  return "templateProcesor default: " + var;
}

String WifiAppClass::templateProcessorAdmin(const String &var)
{
  if (var == "user")
  {
    return "admin";
  }
  return templateProcessor(var);
}

String WifiAppClass::templateProcessorUser(const String &var)
{
  if (var == "user")
  {
    return "user";
  }
  return templateProcessor(var);
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
  request->send(500, "text/html", tempStr);
  return false;
}

void WifiAppClass::onNotFound(AsyncWebServerRequest *request)
{
  String retour;
  // Handle Unknown Request
  retour += ("NOT_FOUND: ");
  if (request->method() == HTTP_GET)
    retour += ("GET");
  else if (request->method() == HTTP_POST)
    retour += ("POST");
  else if (request->method() == HTTP_DELETE)
    retour += ("DELETE");
  else if (request->method() == HTTP_PUT)
    retour += ("PUT");
  else if (request->method() == HTTP_PATCH)
    retour += ("PATCH");
  else if (request->method() == HTTP_HEAD)
    retour += ("HEAD");
  else if (request->method() == HTTP_OPTIONS)
    retour += ("OPTIONS");
  else
    retour += ("UNKNOWN");

  retour += "\n";

  retour += " http://" + (String)request->host().c_str() + request->url().c_str() + "\n";

  if (request->contentLength())
  {
    retour += "_CONTENT_TYPE: " + (String)request->contentType().c_str() + "\n";
    retour += "_CONTENT_LENGTH: " + (String)request->contentLength() + "\n";
  }

  retour += "\n";
  retour += "Headers: \n";
  int headers = request->headers();
  int i;
  for (i = 0; i < headers; i++)
  {
    AsyncWebHeader *h = request->getHeader(i);
    retour += "_HEADER[" + (String)h->name().c_str() + "]: " + h->value().c_str() + "\n";
  }

  retour += "\n";
  retour += "Params: \n";
  int params = request->params();
  for (i = 0; i < params; i++)
  {
    AsyncWebParameter *p = request->getParam(i);
    if (p->isFile())
    {
      retour += "_FILE[" + (String)p->name().c_str() + "]: " + p->value().c_str() + ", size: " + p->size() + " \n";
    }
    else if (p->isPost())
    {
      retour += "_POST[" + (String)p->name().c_str() + "]: " + p->value().c_str() + "\n";
    }
    else
    {
      retour += "_GET[" + (String)p->name().c_str() + "]:  " + p->value().c_str() + "\n";
    }
  }

  request->send(404, "text/plaintext", retour);
}

bool WifiAppClass::begin()
{

  if (!SPIFFS.begin(true))
  {
    Serial.println("[WiFiApp] SPIFFS begin failed");
    return false;
  }
  if (Prefs.isKey("WIFI_SSID") && Prefs.isKey("WIFI_PSSWD"))
  {
    WiFi.mode(WiFiMode_t::WIFI_MODE_STA);
    WiFi.disconnect();
    String SSID = Prefs.getString("WIFI_SSID", "");
    String PSSWD = Prefs.getString("WIFI_PSSWD", "");

    // IPAddress local_IP(192, 168, 1, 10);
    // IPAddress gateway(192, 168, 1, 1);
    // IPAddress subnet(255, 255, 255, 0);
    // IPAddress primaryDNS(8, 8, 8, 8); // optional
    // IPAddress secondaryDNS(8, 8, 4, 4); // optional

    // // Configures static IP address
    // if (!WiFi.config(local_IP, gateway, subnet,primaryDNS)) {
    //   Serial.println("STA Failed to configure");
    // }
    WiFi.setHostname("Esp32S3_HydroBabiat");
    WiFi.begin(SSID.c_str(), PSSWD.c_str());

    // WiFi.begin(WIFISSID, WIFIPASSWORD);

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("[WiFiApp] Wifi begin failed");
      // return false;
    }
    
    
    Serial.println("[WiFiApp] IP: " + (String)WiFi.localIP().toString());
  }
  else
  {
    WiFi.mode(WiFiMode_t::WIFI_MODE_APSTA);
    WiFi.softAP("HydroBabiat");
    WiFi.scanNetworks();
    dnsserver.start(53, "*", WiFi.softAPIP());

    Serial.println("[WiFiApp AP Ip:] " + (String)WiFi.softAPIP().toString());
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->authenticate(USERNAME_ADMIN, PASSWORD_ADMIN))
              {
                if (SPIFFS.exists("/home.html"))
                {
                  return request->send(SPIFFS, "/home.html", "text/html", false, WifiApp.templateProcessorAdmin);
                }
                else
                {
                  sendInternalServerError(request);
                }
              }
              else if (request->authenticate(USERNAME_User, PASSWORD_User))
              {
                if (SPIFFS.exists("/home.html"))
                {
                  return request->send(SPIFFS, "/home.html", "text/html", false, WifiApp.templateProcessorUser);
                }
                else
                {
                  sendInternalServerError(request);
                }
              }
              else
              {
                return request->send(SPIFFS, "/notAuth.html", "text/html", false);
              } })
      .setFilter(ON_STA_FILTER);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req)
            {
              String retour;
              retour += "<html><head>";
              retour += R"(<style>
		.container {
			border: 1px solid black;
			padding: 10px;
			margin-bottom: 10px;
			cursor: pointer;
		}

		.selected {
			border: 2px solid blue;
		}
	</style>)";
              retour += "</head><body>";
              retour += "<form method=\"POST\" action =\"/wifi\">";
              int n = WiFi.scanComplete();
              for (size_t i = 0; i < n; i++)
              {
                retour += "<div class=\"container\" data-ssid=\"" + WiFi.SSID(i) + "\" onclick=\"selectElement(this)\">";
                retour += "<h3>" + WiFi.SSID(i) + "</h3>";
                retour += "<p>" + WiFi.SSID(i) + " " + (String)WiFi.RSSI(i) + "(dbm) " + (String)WiFi.encryptionType(i) + "</p>\n";
                retour += "</div>";
              }
              retour += R"(<input type="hidden" name="ssid" id="ssid">)";
              retour += R"(<label for="psswd">Password:</label><br>
  <input type="text" id="psswd" name="psswd" value=""><br>)";
              retour += "</form>";
              retour += R"(<script>
		function selectElement(element) {
			// Retirer la classe "selected" de tous les containers
			let containers = document.getElementsByClassName("container");
			for (let i = 0; i < containers.length; i++) {
				containers[i].classList.remove("selected");
			}

			// Ajouter la classe "selected" au container sélectionné
			element.classList.add("selected");

			// Mettre à jour la valeur du champ caché
			let selectedElement = document.getElementById("ssid");
			selectedElement.value = element.dataset.ssid;
		}
	</script>)";
              req->send(200, "text/html", retour); })
      .setFilter(ON_AP_FILTER);

  server.on("/wifi", HTTP_ANY, [](AsyncWebServerRequest *request)
            {
              int params = request->params();
              switch (request->method())
              {
              case HTTP_GET:

                break;
              case HTTP_POST:
                for (int i = 0; i < params; i++)
                {
                  AsyncWebParameter *p = request->getParam(i);
                  if (p->isFile())
                  { // p->isPost() is also true
                    Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
                  }
                  else if (p->isPost())
                  {
                    Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                  }
                  else
                  {
                    Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                  }
                }
                if (request->hasParam("ssid", true) && request->hasParam("psswd", true))
                {
                  Prefs.putString("WIFI_SSID", request->getParam("ssid", true)->value());
                  Prefs.putString("WIFI_PSSWD", request->getParam("psswd", true)->value());
                }

                request->redirect("/");
                break;
              case HTTP_DELETE:

                Prefs.remove("WIFI_SSID");
                Prefs.remove("WIFI_PSSWD");
                request->send(200, "text/plaintext", "ok");

                break;
              default:
                break;
              } });
  server.on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    startReboot = millis() + 2000;
    request->send(200,"text/plaintext","ok"); });
  server.on("/icons/hydro-elec-512.svg", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/icons/hydro-elec-512.svg"); });
  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/icons/favicon.ico"); });
  server.on("/logo.png", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/icons/favicon-192.png"); });

  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (!request->authenticate(USERNAME_ADMIN,PASSWORD_ADMIN))
    {
      return request->requestAuthentication();
    }
      
    if(request->hasParam("modeNum")){
      
      AsyncWebParameter* p = request->getParam("modeNum");
      Serial.println("mode num : "+ (String) p->value().toInt());
      Notifi.send("Changement de Mode");

      modes.get(modeActuel)->endMode();
      modeActuel=p->value().toInt();
      modes.get(modeActuel)->startMode();

      Prefs.putInt(MODEVANNE, modeActuel);
    }
    request->send(200, "text/plain", "mode ok"); });

  server.on("/dataEtang", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String Response = "{";
    Response += "\"niveauEtang\":" + (String)dataEtang.niveauEtang + ",";
    Response += "\"niveauEtangP\":" + (String)dataEtang.ratioNiveauEtang;
    Response += "}";
    request->send(200, "application/json", Response); });

  server.on("/dataTurbine", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    String Response = "{";
    Response +=  (String)dataTurbine.toJson();
    Response += "}";
    request->send(200, "application/json", Response); });

  server.on("/programmateur/new", HTTP_GET, [](AsyncWebServerRequest *request)
            {
		//ProgrammatedTasks->add(new ProgrammatedTask(12,12,"test ajout"));
		ProgTasks.addTask(new ProgrammatedTask(12,12,"test ajout"));
		//request->send(SPIFFS,"/programmateur.html", "text/html", false, processor);
		request->redirect("/programmateur"); });

  server.on("/programmateur/sauvegarder", HTTP_GET, [](AsyncWebServerRequest *request)
            {
		ProgTasks.saveTask();
		
		request->send(SPIFFS,"/Programmated","text/plaintext"); });

  server.on("/programmateur", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/programmateur.html", "text/html", false, ProgTasks.templateProcessor); });

  server.on("/updateprogrammateur", HTTP_POST, [](AsyncWebServerRequest *request)
            {
		
		
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
		if (request->hasParam("execOnce",true))
		{
			test->execOnce = true;
		} else
		{
			test->execOnce = false;
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
		
		
		
		
		request->send(200,"text/json","{\"ok\":1}"); });

  server.on("/data.csv", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/data.csv", "text/csv"); });

  SPIFFS_provide_file("/app.js");
  SPIFFS_provide_file("/theme.js");
  SPIFFS_provide_file("/fileSystem.js");
  SPIFFS_provide_file("/Programmateur.js");

  SPIFFS_provide_file("/style.css");

  SPIFFS_provide_file("/icons/Basic.svg");
  SPIFFS_provide_file("/icons/PID.svg");
  // SPIFFS_provide_file("/fileSystem.html");
  server.on(
      "/fileSystem", HTTP_ANY, [](AsyncWebServerRequest *request)
      {
        Serial.println("request method: " + (String)request->method());
        int params = request->params();
        for (int i = 0; i < params; i++)
        {
          AsyncWebParameter *p = request->getParam(i);
          if (p->isFile())
          { // p->isPost() is also true
            Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
          }
          else if (p->isPost())
          {
            Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
          }
          else
          {
            Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
          }
        }
        if (request->method() == HTTP_GET)
        {

          request->send(SPIFFS, "/fileSystem.html", "text/html", false, [](String var)
                        {
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
        
      return retour; });
        }
        else
        {
          request->send(200, "text/plaintext", "par l)");
        } },
      [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
      {
        // if (checkUserWebAuth(request)) {
        Serial.println("par ici : " + (String)filename);
        if (request->method() != HTTP_POST)
        {
          return;
        }

        if (!index)
        {

          // open the file on first call and store the file handle in the request object
          Serial.println("Upload file " + String(filename));
          request->_tempFile = SPIFFS.open("/" + filename, "w");
        }

        if (len)
        {
          // stream the incoming chunk to the opened file
          request->_tempFile.write(data, len);
        }

        if (final)
        {
          // close the file handle as the upload is now done
          int filesize = request->_tempFile.size();
          request->_tempFile.close();
          return request->send(200, "application/json", "{\"status\":\"ok\",\"file\":[{\"name\":\"" + String(filename) + "\",\"size\":" + String(filesize) + "}]}");
          // request->redirect("/");
        }
        // } else {
        //   Serial.println("Auth: Failed");

        // }
      });

  server.on("/api/fs", HTTP_DELETE, [](AsyncWebServerRequest *request)
            {
              if (request->hasParam("fileName", true))
              {
                String fileName = (String)request->getParam("fileName", true)->value();
                if (SPIFFS.exists(fileName))
                {
                  SPIFFS.remove((String)fileName);
                  request->send(200, "application/json", "{\"status\":\"ok\"}");
                }
                request->send(400, "text/plaintext", "file \"" + fileName + "\" doesn't exist");
              } });
  server.on("/api/fs", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String response = "";
              response += "{\"SPIFFS\":[";

              File fileRoot = SPIFFS.open("/");

              // Parcourt tous les fichiers et dossiers de la carte SD
              while (File file = fileRoot.openNextFile())
              {
                // Ajoute le nom du fichier ou du dossier au tableau
                if (!response.endsWith("["))
                {
                  response += ",";
                }

                response += "{\"name\": \"" + (String)file.name() + "\", \"size\": " + (String)file.size() + ", \"path\": \"" + (String)file.path() + "\"}";
              }

              response += "]}";

              request->send(200, "application/json", response); });

  server.on("/sendFile", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              int params = request->params();
              for (int i = 0; i < params; i++)
              {
                AsyncWebParameter *p = request->getParam(i);
                if (p->isFile())
                { // p->isPost() is also true
                  Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
                }
                else if (p->isPost())
                {
                  Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
                else
                {
                  Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
              }
              bool otaUpdate = false;

              if (request->hasParam("Ota", true))
              {
                otaUpdate = true;
              }

              if (request->hasParam("fileName", true))
              {
                String fileName = (String)request->getParam("fileName", true)->value();
                if (SPIFFS.exists(fileName))
                {
                  Serial.println("Sending: " + String(fileName));
                  // TODO lorafile transfer
                  LoRaFileUpl.beginTransmit(fileName, 0x04, otaUpdate);
                  return request->send(200, "application/json", "{\"status\":\"ok\"}");
                }
                return request->send(400, "text/plaintext", "file doesn't exist");
              }
              return request->send(400, "text/plaintext", "wrong filename"); });

  server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (!request->authenticate("bapt", "000"))
                return request->requestAuthentication();
              request->send(200, "text/plain", "Login Success!"); });
  server.on("/cmd2", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              if (request->authenticate("bapt", "000"))
              {
                request->send(200, "text/plain", "Login Success! bapt");
              }
              else if (request->authenticate("bapt2", "000"))
              {
                request->send(200, "text/plain", "Login Success! bapt2");
              }
              else
              {
                return request->requestAuthentication();
              } });
  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *req)
            { req->send(401, "text/plain", "deco"); });
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest *req)
            {
    if (req->authenticate(USERNAME_ADMIN,PASSWORD_ADMIN))
    {
      return req->redirect("/");
    } else if (req->authenticate(USERNAME_User,PASSWORD_User))
    {
      return req->redirect("/");
    }
    
    return req->requestAuthentication(); });
  // server.onNotFound([](AsyncWebServerRequest *request)
  //                  { return request->send(404); });

  server.onNotFound(WifiApp.onNotFound);

#ifdef ASYNC_TCP_SSL_ENABLED
#pragma message "tcp ssl"
  server.onSslFileRequest([](void *arg, const char *filename, uint8_t **buf) -> int
                          {
      Serial.printf("SSL File: %s\n", filename);
      File file = SPIFFS.open(filename, "r");
      if(file){
        size_t size = file.size();
        uint8_t * nbuf = (uint8_t*)malloc(size);
        if(nbuf){
          size = file.read(nbuf, size);
          file.close();
          *buf = nbuf;
          return size;
        }
        file.close();
      }
      *buf = 0;
      return 0; },
                          NULL);
#endif

  ws.onEvent(WifiApp.onEvent);
  server.addHandler(&ws);

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);

#ifdef ASYNC_TCP_SSL_ENABLED
  server.beginSecure("Cert.pem", "Key.pem", NULL);
#else
  server.begin();
#endif
  return true;
}

bool WifiAppClass::close()
{
  ws.closeAll();
  Serial.println("ws closed !");
  // server.removeHandler(&ws);
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
   if (dataStr.startsWith("resetEnergieMeter"))
   {
    Em.reset();
   }
   
    else if (dataStr.startsWith("LedNotif"))
    {
      ledNotif = !ledNotif;

      Prefs.putBool("LedNotif", ledNotif);
    }

    else if (dataStr.startsWith("PositionVanneTarget"))
    {
      dataStr.replace("PositionVanneTarget ", "");
      Serial.println("niveu vanne: " + (String)dataStr.toInt());
      dataTurbine.targetPositionVanne = dataStr.toInt();
      bufferActionToSend += "TURBINE:TargetVanne=" + (String)dataStr.toInt() + ";";
    }
    else if (dataStr.startsWith("Action:"))
    {
      dataStr.replace("Action:", "");
      Serial.println("Action: " + dataStr);
      bufferActionToSend += dataStr;
      if (dataStr.startsWith("NODETEST:DEEPSLEEP"))
      {
        startDeepSleep = millis() + 1500;
      }
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
        bC->niveauMax = dataStr.toInt();
      }
      if (dataStr.startsWith("target="))
      {
        dataStr.replace("target=", "");
        bC->vanneMax = dataStr.toInt();
      }
    }
    else if (dataStr.startsWith("testNotifGroup"))
    {
      Notifi.sendToChannel("TestNotifGroup");
    }
    else if (dataStr.startsWith("testNotif"))
    {
      Notifi.send("TestNotif");
    }
    else if (dataStr.startsWith("SavePref"))
    {
      savePref();
    }
    else if (dataStr.startsWith("AlertNiv"))
    {
      dataStr.replace("AlertNiv", "");

      if (dataStr.startsWith("Actif="))
      {
        dataStr.replace("Actif=", "");
        if (dataStr.startsWith("true"))
        {
          AlertNiv.active = true;
        }
        else
        {
          AlertNiv.active = false;
        }
      }
      if (dataStr.startsWith("Min="))
      {
        dataStr.replace("Min=", "");
        AlertNiv.min = dataStr.toInt();
      }
      if (dataStr.startsWith("Max="))
      {
        dataStr.replace("Max=", "");
        AlertNiv.max = dataStr.toInt();
      }
    }
    else if (dataStr.startsWith("NotificationGroup="))
    {
      dataStr.replace("NotificationGroup=", "");
      Notifi.NotifyGroup = !Notifi.NotifyGroup;
      Prefs.putBool("NotifGroup", Notifi.NotifyGroup);
    }
    else if (dataStr.startsWith("Notification="))
    {
      dataStr.replace("Notification=", "");
      Notifi.NotifyIndividuel = !Notifi.NotifyIndividuel;
      Prefs.putBool("Notif", Notifi.NotifyIndividuel);
    }

    else if (dataStr.startsWith("Etang."))
    {
      dataStr.replace("Etang.", "");
      if (dataStr.startsWith("Active="))
      {
        dataStr.replace("Active=", "");
        EtangStatus.active = dataStr.toInt();
      }
    }
    else if (dataStr.startsWith("Turbine."))
    {
      dataStr.replace("Turbine.", "");
      if (dataStr.startsWith("Active="))
      {
        dataStr.replace("Active=", "");
        TurbineStatus.active = dataStr.toInt();
      }
    }

    else if (dataStr.startsWith("NodeTest."))
    {
      Serial.println(dataStr);
      dataStr.replace("NodeTest.", "");
      if (dataStr.startsWith("Active="))
      {
        dataStr.replace("Active=", "");
        RadiateurStatus.active = dataStr.toInt();
        Serial.println(RadiateurStatus.active);
        Prefs.putBool(RadiateurStatus.Name.c_str(), RadiateurStatus.active); // TODO logic à deplacer
      }
    }
    else if (dataStr.startsWith("DeepSleepServer"))
    {
      startDeepSleep = millis() + 5000;
    }
    else if (dataStr.startsWith("SpectrumScan"))
    {
      SpectrumScan = true;
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
  if (WiFi.getMode() == WiFiMode_t::WIFI_MODE_STA)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      
    }
    else if (WiFi.status() == WL_DISCONNECTED)
    {
      /* code */
    }

    ws.cleanupClients();
  }
  else if (WiFi.getMode() == WiFiMode_t::WIFI_MODE_APSTA)
  {
    dnsserver.processNextRequest();
  }
}

WifiAppClass WifiApp;