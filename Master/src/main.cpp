#include <Arduino.h>
#include <configuration.h>
#include <configGeneral.h>
#include <configVariables.h>
#include <AsyncTelegram2.h>
#include "main.h"
#include "motorState.h"
#include "AlertNiveau.h"

#include "LoRaFileUploader.h"

#include <FS.h>
#include <TelegramCredentials.h>

#include <WifiApp.h>
#include <Adafruit_SSD1306.h>

#ifdef USE_TFT
#include <Adafruit_ST7735.h>
#endif
#include <ArduinoOTA.h>
#include <WiFiClientSecure.h>

#include <RadioLib.h>
#include "Ecran.h"
#include "LoRa.h"
#include "Timer.h"
#include <LList.h>

#include <digitalInput.h>
#include <parser.h>

#include "basicController.h"
#include "PIDController.h"
#include "manuelController.h"
  
#include <NTPClient.h>
#include <ProgrammatedTasks.h>

#include <EnergieMeter.h>

#define LED 7
#define POT  6

#define LEDNOTIF 35
#define PINTONE 5

#ifdef USE_TFT
SPIClass spitft;
#define TFT_CS        26
#define TFT_RST       16 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        47
#define TFT_BACKLIGHT PIN_A3 // Display backlight pin
#define TFT_MOSI      33  // Data out
#define TFT_SCLK      34  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735 (TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
#endif

#define pinBattery 1
float batteryReadings ;

//Adafruit_SSD1306* display;
int potRaw = 0;
int potValue = 0;
unsigned long ledReceptionMessage = 0;

int displayNum = 0;
int maxDisplay = 4;

bool ledNotif = false;

unsigned long startDeepSleep = 0;
unsigned long startReboot = 0;

String bufferActionToSend;


dataTurbine_t dataTurbine;
nodeStatus_t TurbineStatus;

dataEtang_t dataEtang;
nodeStatus_t EtangStatus;

dataNodeTest_t dataNodeTest;
nodeStatus_t nodeTest;

LList<nodeStatus_t*> listNodes = LList<nodeStatus_t*>();
int lastNode = -1;

WiFiClientSecure telegramClient;
AsyncTelegram2 TelegramBot(telegramClient);
unsigned long telegramBot_lastTime = 0;
bool telegramCheckMessage = true;
ReplyKeyboard myreplykbd;
InlineKeyboard myinlinekbd;
bool iskeyboardactive = false;

Preferences Prefs;

digitalInput btnPRG(0,INPUT_PULLUP);

Timer timerEnvoi(10000);
Timer timerEnvoiWS(1000);
Ecran Ec(&Wire);

Timer TftTimer (2500);

const char* EnteteCSV = "Date,Tachy,Niveau,CibleVanne,OuvertureVanne,Tension,Intensite,Power,Energie";

EnergieMeter Em;

//sauvegarde des données
unsigned long lastSaveData = 0;


manuelController *manuelC = new manuelController();
basicController *bC = new basicController();
PIDController *pidC = new PIDController();
int modeActuel = 0;
LList<IController*> modes = LList<IController*>();

WiFiUDP ntpUDP;
const char* ntpServer = "europe.pool.ntp.org";
NTPClient timeClient(ntpUDP,ntpServer,3600,3600);

const int pinAnalogTest = 6;


bool OtaUpdate = false;

//battery mesure

#define FBattery 3700 //The default battery is 3700mv when the battery is fully charged.
float XS = 0.0025f;
uint16_t MMUL = 1000;
uint16_t MUL = 100;


void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
}
float readBatLevel() {
  pinMode(37,OUTPUT);
  digitalWrite(37, LOW);
  delay(10); // let GPIO stabilize
  int analogValue = analogRead(1);
  float voltage = 0.00403532794741887 * analogValue;
  return voltage;
}

String timeElapsedToString(unsigned long timeS){
  

  //temps inferieur à 1 min
  if (timeS < 60  )
  {
    return (String)timeS + "s";
  }
  //temps inferieur à 1 h
  if (timeS < 60 * 60  )
  {
    return (String)(timeS / 60) + "min" + (String)(timeS%60);
  }

  //temps inferieur à 1 jour
  // if (timeS < 24 * 3600)
  // {
    return (String)(timeS/3600) + "h" +(String)((timeS%3600) / 60) + "min" + (String)((timeS%3600)%60);
  //}
  
  

}




void arduinoOtaSetup(void){
  ArduinoOTA
    .onStart([]() {
      Ec.wakeUp();
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
      OtaUpdate = true;
      WifiApp.monitorClients("OTA UPDATE: " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
      OtaUpdate = false;
      ESP.restart();
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      Ec.getDisplay()->clearDisplay();
      Ec.getDisplay()->setCursor(40,10);
      

      
      Ec.getDisplay()->println("OTA Update");
      Ec.getDisplay()->printf("Progress: %u%%\r\n", (progress / (total / 100)));
      Ec.getDisplay()->println(ArduinoOTA.getCommand() == U_FLASH ? "sketch":"fs");

      Ec.getDisplay()->display();
    })
    .onError([](ota_error_t error) {
      OtaUpdate = false;
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void handleTelegramMessage(int numNewMessages){
  
}



bool saveDataCsV(void){
	File myFile;
	if (!SPIFFS.exists("/data.csv"))
	{
		Serial.println("Creation data.csv");
		File myFile = SPIFFS.open("/data.csv",FILE_WRITE);
		myFile.print(EnteteCSV);
		myFile.close();
		
	}
	myFile = SPIFFS.open("/data.csv",FILE_APPEND);
	
  
	//myFile.print("\n"+String(timeClient.getEpochTime())+","+String(dataTurbine.tacky)+","+String(dataEtang.ratioNiveauEtang)+","+ String(dataTurbine.targetPositionVanne)+","+String(dataTurbine.positionVanne)+"," + String(dataTurbine.U)+"," + String(dataTurbine.I)+"," + String(dataTurbine.getPower()));
	
  size_t test;
  
  test = myFile.printf("\n%u,%.1f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f",timeClient.getEpochTime(),dataTurbine.tacky,dataEtang.ratioNiveauEtang,dataTurbine.targetPositionVanne,dataTurbine.positionVanne,dataTurbine.U,dataTurbine.I, Em.getEnergie());
  myFile.close();
  
  Serial.println("Sauvegarde : " + (String)test);
	return true;
}


String LoRaOnMsgStatut(){
  return "";
}

void LoRaMessage(LoRaPacket header, String msg)
{
  ledReceptionMessage = millis();
  switch (header.Code)
  {
  case LoRaMessageCode::FileAck :
    LoRaFileUpl.nextPacket();
    return;
    break;
  case LoRaMessageCode::DataReponse:
    WifiApp.monitorClients(msg);
    break;
  default:
    break;
  }
  if (ledNotif)
  {
    digitalWrite(LEDNOTIF,HIGH);
    
  }
  
  Serial.print("cb: ");
  Serial.println(msg);
  if (header.Emetteur == TURBINE)
  {
    TurbineStatus.RSSI = header.RSSI;
    TurbineStatus.dernierMessage = millis();
    TurbineStatus.SNR = header.SNR;

    String key;
    String val;
    
    int posSeparat = msg.indexOf(",");
    while (msg.indexOf(",") != -1)
    {
      int posDeli = msg.indexOf(":");
      posSeparat = msg.indexOf(",");
      key = msg.substring(0,posDeli);
      val = msg.substring(posDeli +1,posSeparat);
      //Serial.printf("key: %s val: %s\n",key ,val);
      msg.remove(0,posSeparat+1);

      if (key == "PV"){
        dataTurbine.positionVanne = val.toFloat();
      }
      if (key == "PM"){
        dataTurbine.positionMoteur = val.toFloat();
      }
      if (key == "target"){
        dataTurbine.targetPositionVanne = val.toFloat();
      }
      if (key == "tachy"){
        dataTurbine.tacky = val.toFloat();
      }
      if (key == "U"){
        dataTurbine.U = val.toFloat();
      }
      if (key == "I"){
        dataTurbine.I = val.toFloat();
      }
      if (key == "UB"){
        dataTurbine.UB = val.toFloat()  / 1000;
      }
      if (key == "motorState"){
        dataTurbine.motorState = (MotorState)val.toInt();
      }
    }
    Em.update(dataTurbine.I * dataTurbine.U);

  } else if (header.Emetteur == ETANG)
  {
    EtangStatus.RSSI = header.RSSI;
    EtangStatus.dernierMessage = millis();
    EtangStatus.SNR = header.SNR;

    String key;
    String val;
    //char message[20];
    int posSeparat = msg.indexOf(",");
    while (msg.indexOf(",") != -1)
    {
      int posDeli = msg.indexOf(":");
      posSeparat = msg.indexOf(",");
      key = msg.substring(0,posDeli);
      val = msg.substring(posDeli +1,posSeparat);
      //Serial.printf("key: %s val: %s\n",key ,val);
      msg.remove(0,posSeparat+1);

      if (key == "NE")
      {
        dataEtang.niveauEtang = val.toInt();
      }
      if (key == "NEVide")
      {
        dataEtang.niveauEtangVide = val.toInt();;
      }
      if (key == "NERempli")
      {
        dataEtang.niveauEtangRempli = val.toInt();;
      }
      if (key == "NETropPlein")
      {
        dataEtang.niveauEtangTroPlein = val.toInt();;
      }
      if (key == "ratio")
      {
        dataEtang.ratioNiveauEtang = (val.toFloat())*100; //TODO 
      }
      if (key == "timingBudget")
      {
        dataEtang.timingBudget = val.toInt();
      }
      if (key == "RoiC")
      {
        dataEtang.RoiCenter = val.toInt();
      }
      if (key == "RoiXY")
      {
        dataEtang.RoiX = val.substring(0,val.indexOf("|")).toInt();
        dataEtang.RoiY = val.substring(val.indexOf("|")+1).toInt();
      }
      if (key == "distanceMode")
      {
        dataEtang.distanceMode = val.toInt();
        
      }
      
      AlertNiv.updateNiveau(dataEtang.ratioNiveauEtang);
    }
    



    
  } else if (header.Emetteur == 0x04)
  {
    nodeTest.RSSI = header.RSSI;
    nodeTest.dernierMessage = millis();
    nodeTest.SNR = header.SNR;
    String key;
    String val;
    
    int posSeparat = msg.indexOf(",");
    while (msg.indexOf(",") != -1)
    {
      int posDeli = msg.indexOf(":");
      posSeparat = msg.indexOf(",");
      key = msg.substring(0,posDeli);
      val = msg.substring(posDeli +1,posSeparat);
      //Serial.printf("key: %s val: %s\n",key ,val);
      msg.remove(0,posSeparat+1);

      if (key == "temp"){
        dataNodeTest.temp = val.toFloat();
        
      }
    }
  }
  
  WifiApp.notifyClients();
  
}

void LoRaNoReply(lastSend_t* packet){
  //byte emetteur = packet->id;
  if (packet->id == LoRaFileUpl.id && LoRaFileUpl.initialized)
  {
    Serial.println("NoReply");
    LoRaFileUpl.sendPacket();
  }
  
  
}

/*
* scan i2c bus
*/
void scanI2C(TwoWire* bus){
  byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for(address = 1; address < 127; address++ )
	{
    Serial.print("I2C device begin at address 0x");
    Serial.println(address,HEX);
		bus->beginTransmission(address);
		error = bus->endTransmission();

//		Wire1.beginTransmission(address);
//		error = Wire1.endTransmission();

		if (error == 0)
		{
			Serial.print("I2C device found at address 0x");
			if (address<16)
			Serial.print("0");
			Serial.print(address,HEX);
			Serial.println("  !");

			nDevices++;
		}
		else if (error==4)
		{
			Serial.print("Unknown error at address 0x");
			if (address<16)
				Serial.print("0");
			Serial.println(address,HEX);
		}
	}
	if (nDevices == 0)
	Serial.println("No I2C devices found\n");
	else
	Serial.println("done\n");
}

void displayData(){
  if (OtaUpdate)
  {
    return;
  }
  Ec.getDisplay()->clearDisplay();
  switch (displayNum)
  {
  case 0:
    Ec.getDisplay()->setCursor(0,0);
    // Ec.getDisplay()->print("PV: ");
    // Ec.getDisplay()->println(dataTurbine.positionVanne);


    // Ec.getDisplay()->setCursor(0,12);
    // Ec.getDisplay()->print("target: ");
    // Ec.getDisplay()->println(dataTurbine.targetPositionVanne);

    Ec.getDisplay()->println("[" + String(EtangStatus.active?"x":" ") +"] Etang  : " + (String)EtangStatus.RSSI);
    Ec.getDisplay()->print(lastNode == 0 ? (String)LoRa.lastSend.attempt:" ");
    Ec.getDisplay()->println(" : " + timeElapsedToString((millis() - EtangStatus.dernierMessage)/1000));
    Ec.getDisplay()->println("[" + String(TurbineStatus.active?"x":" ") +"] Turbine: " + (String)TurbineStatus.RSSI);
    Ec.getDisplay()->print(lastNode == 1 ? (String)LoRa.lastSend.attempt:" ");
    Ec.getDisplay()->println(" : " + timeElapsedToString((millis() - TurbineStatus.dernierMessage)/1000));
    Ec.getDisplay()->println("[" + String(nodeTest.active?"x":" ") +"] Node Test: " + (String)nodeTest.RSSI);
    Ec.getDisplay()->print(lastNode == 2 ? (String)LoRa.lastSend.attempt:" ");
    Ec.getDisplay()->println(" : " + timeElapsedToString((millis() - nodeTest.dernierMessage)/1000));
    Ec.getDisplay()->println("num " + (String)lastNode);
    
    break;
  case 1:
    Ec.getDisplay()->setCursor(0,0);
    Ec.getDisplay()->println("Niveau: " + (String)(dataEtang.ratioNiveauEtang ) + " %");
    Ec.getDisplay()->println("Vanne: " + (String)(dataTurbine.positionVanne) + " %");
    Ec.getDisplay()->println("Cible Vanne: " + (String)(dataTurbine.targetPositionVanne) + " %");

    Ec.drawBVProgressBar(114,4,5,50,(dataEtang.ratioNiveauEtang ));

    int posxTargetVanne;
    posxTargetVanne = 4 + 106 * pidC->vanne / 100;
    Ec.getDisplay()->setCursor(posxTargetVanne,45);
    Ec.getDisplay()->println("v");
    Ec.drawProgressBar(4,55,106,5,(dataTurbine.positionVanne ));
    break;
  case 2:
    Ec.getDisplay()->setCursor(0,0);
    Ec.getDisplay()->println("WifiStatus: " + (String)WiFi.status());
    if (WiFi.getMode() == wifi_mode_t::WIFI_MODE_STA)
    {
      Ec.getDisplay()->println("STA " + (String) WiFi.localIP().toString());
    } else if (WiFi.getMode() == WiFiMode_t::WIFI_MODE_APSTA)
    {
      Ec.getDisplay()->println("APSTA " + (String) WiFi.softAPIP().toString());
      
    }
    
    
    
    
    
    
    Ec.getDisplay()->println("Battery   : " + (String) readBatLevel());
    
    
    // Ec.getDisplay()->println("Battery: " + (String) analogRead(36));
    // Ec.getDisplay()->println("Battery: " + (String) (analogRead(36) * 0.769 +150));
    
    Ec.getDisplay()->printf("Spiffs: %i / %i\n" , SPIFFS.usedBytes(), SPIFFS.totalBytes() );
    Ec.getDisplay()->printf("Spiffs: %f %\n" ,  (SPIFFS.usedBytes() / float(SPIFFS.totalBytes())) *100 );

    //Ec.getDisplay()->println(WifiApp.server.);

    break;
  default:
    //Ec.drawVProgressBar(30,4,10,40,(50));
    //Ec.drawBVProgressBar(60,4,10,40,(50));
    Ec.getDisplay()->setCursor(0,0);
    Ec.getDisplay()->println(modes.get(modeActuel)->name);
    for (size_t i = 0; i < modes.size(); i++)
    {
      Ec.getDisplay()->print("[" + String(i==modeActuel?"x":" ") + "] ");
      Ec.getDisplay()->println(modes.get(i)->name);
    }
    
    switch (modes.get(modeActuel)->type)
    {
    case typeController::manuel :
      
      break;
    case typeController::basic :
      Ec.getDisplay()->setCursor(70,10);
      Ec.getDisplay()->printf("min: %i", bC->niveauMin);
      Ec.getDisplay()->setCursor(70,22);
      Ec.getDisplay()->printf("max: %i", bC->niveauMax);
      Ec.getDisplay()->setCursor(70,32);
      Ec.getDisplay()->printf("etat: %s", (String)etangStateToString(bC->etat));
      break;
    case typeController::PID :
      Ec.getDisplay()->setCursor(70,10);
      Ec.getDisplay()->printf("kp: %.3f", pidC->kp);
      Ec.getDisplay()->setCursor(70,22);
      Ec.getDisplay()->printf("ki: %.3f", pidC->ki);
      Ec.getDisplay()->setCursor(70,34);
      Ec.getDisplay()->printf("kd: %.3f", pidC->kd);
      break;
    
    default:
      break;
    }
    

    Ec.getDisplay()->setCursor(0,40);
    Ec.getDisplay()->println("Niveau " + (String)modes.get(modeActuel)->niveau);
    Ec.getDisplay()->println("Vanne " + (String)modes.get(modeActuel)->vanne);
    
    break;
  }

  Ec.getDisplay()->display();

  
}

void initNodes(){
  
  EtangStatus.addr = ETANG;
  EtangStatus.Name = "Etang";


  TurbineStatus.addr = TURBINE;
  TurbineStatus.Name = "Turbine";

  nodeTest.addr = 0x04;
  nodeTest.Name = "NodeTest";

  listNodes.add(&EtangStatus);
  listNodes.add(&TurbineStatus);
  listNodes.add(&nodeTest);
}

bool initPref() {
  if (Prefs.begin("Master", false)) {
    AlertNiv.active = Prefs.getBool("AlertNivActiv");
    AlertNiv.max = Prefs.getInt("AlertNivMax");
    AlertNiv.min = Prefs.getInt("AlertNivMin");
    modeActuel = Prefs.getInt("modeVanne",0);
    ledNotif = Prefs.getBool("LedNotif",ledNotif);
    if (Prefs.isKey(nodeTest.Name.c_str()))
    {
      nodeTest.active = Prefs.getBool(nodeTest.Name.c_str(),true);
    }
    return true;
  }
  return false;

}

bool savePref() {
  Prefs.putBool("AlertNivActiv", AlertNiv.active);
  Prefs.putInt("AlertNivMax", AlertNiv.max);
  Prefs.putInt("AlertNivMin", AlertNiv.min);
  Prefs.putInt("modeVanne",modeActuel);
  Prefs.putInt("LedNotif",ledNotif);
  WifiApp.monitorClients("Save Pref Ok");
  return true;
}

void setupAnalogMesure()
{
  //analogSetClockDiv(255); // 1338mS
  //analogSetCycles(8);                   // Set number of cycles per sample, default is 8 and provides an optimal result, range is 1 - 255
  //analogSetSamples(1);                  // Set number of samples in the range, default is 1, it has an effect on sensitivity has been multiplied
   
  // analogSetClockDiv(1);                 // Set the divider for the ADC clock, default is 1, range is 1 - 255
  // analogSetAttenuation(ADC_11db);       // Sets the input attenuation for ALL ADC inputs, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  // analogSetPinAttenuation(36,ADC_11db); // Sets the input attenuation, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  // analogSetPinAttenuation(37,ADC_11db);
}

void TelegramGetMessage() {
  if (!telegramCheckMessage)
  {
    return;
  }
  
  TBMessage msg;
  if (WiFi.status() != WL_CONNECTED && WiFi.getMode() != WiFiMode_t::WIFI_MODE_STA)
  {
    return;
  }
  
  
  telegramBot_lastTime = millis();
  
  if (TelegramBot.getNewMessage(msg)) {
    Serial.printf("[telegram] %s\n",msg.text);
    switch (msg.messageType)
    {
      case MessageText:
        if (msg.text.equalsIgnoreCase("/html"))
        {
          TelegramBot.setFormattingStyle(AsyncTelegram2::FormatStyle::HTML);
          TelegramBot.sendMessage(msg,"<a href=\"http://www.example.com/\">inline URL</a>");
        }
        // check if is show keyboard command
        if (msg.text.equalsIgnoreCase("/reply_keyboard")) {
          // the user is asking to show the reply keyboard --> show it
          TelegramBot.sendMessage(msg, "This is reply keyboard:", myreplykbd);
          iskeyboardactive = true;
        }
        else if (msg.text.equalsIgnoreCase("/inline_keyboard")) {
          TelegramBot.sendMessage(msg, "This is inline keyboard:", myinlinekbd);
        }

        // check if the reply keyboard is active
        else if (iskeyboardactive) {
          // is active -> manage the text messages sent by pressing the reply keyboard buttons
          if (msg.text.equalsIgnoreCase("/hide_keyboard")) {
            // sent the "hide keyboard" message --> hide the reply keyboard
            TelegramBot.removeReplyKeyboard(msg, "Reply keyboard removed");
            iskeyboardactive = false;
          } else {
            // print every others messages received
            TelegramBot.sendMessage(msg, msg.text);
          }
        }
      break;

      case MessageQuery:
        if (msg.callbackQueryData.equalsIgnoreCase("test"))
        {
          TelegramBot.endQuery(msg,"test,true");
        }
        
        
      break;
    }
    
  
  }
}
// put your setup code here, to run once:
void setup() {
  VextON();
  Serial.begin(115200);
  Wire.begin(SDA_OLED,SCL_OLED,400000);
  //scanI2C(&Wire);
  pinMode(LEDNOTIF,OUTPUT);

  pinMode(POT,INPUT);


  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  modes.add(manuelC);
  modes.add(bC);
  modes.add(pidC);


  initNodes();

  initPref();

  Em.begin();
  
  if (!Ec.begin())
  {
    Serial.println("Ecan failed init");
    while (true)
    {
      /* code */
    }
    
  }
  Ec.getDisplay()->display();

  delay(100);

  Ec.getDisplay()->clearDisplay();


  Ec.getDisplay()->setCursor(0,0);
  Ec.getDisplay()->printf("Node Id %i \n", NODE_ID);
  LoRa.setNodeID(NODE_ID);
  LoRa.onMessage(LoRaMessage);
  LoRa.onNoReply(LoRaNoReply);
  if (LoRa.begin()!= RADIOLIB_ERR_NONE)
  {
    Serial.println("Error init loRa");
    while (true)
    {
    }
    
  }
  Ec.getDisplay()->println("LoRa init Ok !");
  Ec.getDisplay()->display();

#ifdef PINTONE
  pinMode(PINTONE, OUTPUT);
#endif

#ifdef pinBattery
  //pinMode(pinBattery, INPUT);
  //pinMode(36,ANALOG);
#endif

  pinMode(pinAnalogTest,INPUT);
  ledcSetup(1,1000,8);
  ledcAttachPin(LED,1);

#ifdef USE_TFT
 tft.initR();
 
 //tft.drawCircle(60,60,10,ST77XX_RED);
 tft.fillScreen(ST77XX_BLACK);
 tft.drawCircle(20,20,10,ST7735_WHITE);
 tft.setTextColor(ST7735_BLUE);
 //tft.drawCircle(20,-20,10,ST7735_ORANGE);
 //tft.drawCircle(-20,-20,10,ST7735_RED);
 //tft.drawCircle(-20,20,10,ST7735_BLUE);

  
#endif
  
  Ec.getDisplay()->setCursor(0,10);
  Ec.getDisplay()->print("LoRa Ok");
  Ec.getDisplay()->setCursor(60,10);
  Ec.getDisplay()->println("!");
  
  Ec.getDisplay()->display();
  
  if (!SPIFFS.begin(false))
  {
    Serial.println("Spiffs begin failed");
    Ec.getDisplay()->println("Spiffs failed");
  }
  if (!SPIFFS.exists("/data.csv"))
	{
		
		File myFile = SPIFFS.open("/data.csv",FILE_WRITE);
		myFile.print(EnteteCSV);
		myFile.close();
		
	}
  

  Ec.getDisplay()->clearDisplay();
  Ec.getDisplay()->display();


  

  WifiApp.begin();

  arduinoOtaSetup();

  if (WiFi.getMode() == WiFiMode_t::WIFI_MODE_STA)
  {
    telegramClient.setCACert(telegram_cert);
    TelegramBot.setTelegramToken(BOTtoken);
    TelegramBot.setUpdateTime(4000);
    TelegramBot.begin() ? Serial.println("[Telegram] begin OK") : Serial.println("[Telegram] begin NOK");

    myreplykbd.addButton("Button1");
    myreplykbd.addButton("Button2");
    myreplykbd.addRow();
    myreplykbd.addButton("/hide_keyboard");
    myreplykbd.enableResize();

    myinlinekbd.addButton("ON","test", KeyboardButtonQuery);
    myinlinekbd.addButton("GitHub", "https://github.com/cotestatnt/AsyncTelegram2/", KeyboardButtonURL);
    
  }
  

  // Send a welcome message to user when ready
  // char welcome_msg[128];
  // snprintf(welcome_msg, sizeof(welcome_msg),
  //         "BOT @%s online.\n/help for command list.\n/inline_keyboard",
  //         TelegramBot.getBotName());

  // // Check the userid with the help of bot @JsonDumpBot or @getidsbot (work also with groups)
  // // https://t.me/JsonDumpBot  or  https://t.me/getidsbot
  // TelegramBot.sendTo(CHAT_ID, welcome_msg);

  //lireTacheProgrammer();
  ProgTasks.begin(&timeClient);
  ProgTasks.initTask();

  //timerEnvoi.reset();

  timerEnvoiWS.reset();

  modes.get(modeActuel)->startMode();

  setupAnalogMesure();


  Serial.println("fin setup");
}

// put your main code here, to run repeatedly:
void loop() {
  //Serial.println("readBatlevel " + (String)readBatLevel());
  ArduinoOTA.handle();

  //batteryReadings = analogRead(pinBattery);
  btnPRG.loop();
  LoRa.loop();
  Ec.loop();


  if (Serial.available()>0)
  {
    String cmd = Serial.readStringUntil('\n');
    if (cmd.startsWith("F"))
    {
      LoRaFileUpl.beginTransmit("/testTransmitt.txt",4);

    }
    if (cmd.startsWith("T"))
    {
      telegramCheckMessage = !telegramCheckMessage;
      Serial.println("[Telegram] check message " + (String) telegramCheckMessage);
    }
    
    
  }
  
  LoRaFileUpl.loop();

  // pidC->niveau = map(analogRead(pinAnalogTest),0,4095,0,100);
  // pidC->loop();

  //modes.get(modeActuel)->niveau = map(analogRead(pinAnalogTest),0,4095,0,100);

  modes.get(modeActuel)->niveau = dataEtang.ratioNiveauEtang;
  modes.get(modeActuel)->loop();

  if (bufferActionToSend != "")
  {
    if (bufferActionToSend.indexOf(";") != -1)
    {
      int posDeli = bufferActionToSend.indexOf(":");
      int posSeparat = bufferActionToSend.indexOf(";");
      String node  = bufferActionToSend.substring(0,posDeli);
      String action = bufferActionToSend.substring(posDeli +1,posSeparat);
      
      bufferActionToSend.remove(0,posSeparat+1);

      if (node == "ETANG")
      {
        LoRa.sendData(ETANG,LoRaMessageCode::Data,action);
      } else if (node == "TURBINE")
      {
        LoRa.sendData(TURBINE,LoRaMessageCode::Data,action);
        
      }
    }
    
  }
    
  if (millis()> ledReceptionMessage +300 && ledReceptionMessage != 0)
  {
    ledReceptionMessage = 0;
    digitalWrite(LEDNOTIF,LOW);
  }
  
  potRaw = analogRead(POT);
  potValue = map(potRaw,0,4095,0,100);

  if (btnPRG.frontDesceandant())
  {
    #ifdef PINTONE
      tone (PINTONE, 600); // allume le buzzer actif arduino
      delay(100);
      tone(PINTONE, 900); // allume le buzzer actif arduino
      delay(100);
      noTone(PINTONE);  // désactiver le buzzer actif arduino
    #endif
    if (Ec.getState() == EcranState::EcranState_IDLE)
    {
      displayNum = (displayNum+1)%maxDisplay;
    }
    Ec.wakeUp();
    
  }
  
  if (!LoRaFileUpl.initialized)
  {
    if (timerEnvoi.isOver())
    {
      lastNode++;
      lastNode = lastNode % listNodes.size();
      //i = (i++%3)+2;
      if (listNodes.get(lastNode)->active)
      {
        WifiApp.monitorClients("Demande statut lora à " + (String)listNodes.get(lastNode)->Name);
        //Serial.println("Demande statut lora à " + (String)listNodes.get(lastNode)->Name);
        //lastNode++;
        lastNode = lastNode % listNodes.size();
        
        LoRa.sendData(listNodes.get(lastNode)->addr,LoRaMessageCode::DemandeStatut,"demandeStatut");
      }
      
    }
    
  }
  

  // if (timerEnvoiWS.isOver())
  // {
  //   WifiApp.notifyClients();
  // }
  
  TelegramGetMessage();
  
  
  
  
  #ifdef USE_TFT
  if (TftTimer.isOver())
  {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0,10);
    tft.println("Etang  : " + (String)EtangStatus.RSSI);
    tft.println("Turbine: " + (String)TurbineStatus.RSSI);
  }
  #endif

  WifiApp.loop();

  //Sauvegarde des données
	if (millis() > lastSaveData + 30000)
	{
		lastSaveData = millis();
    
		saveDataCsV();
	}

  ProgTasks.loop();
  
  displayData();

  if (millis() > startReboot && startReboot != 0)
  {
    startReboot = 0;
    ESP.restart();
  }
  
  if (millis() > startDeepSleep && startDeepSleep !=0)
  {
    startDeepSleep = 0;

    WifiApp.close();

    esp_sleep_enable_timer_wakeup(10e6);
    esp_deep_sleep_start();
  }
  
  delay(10);
}