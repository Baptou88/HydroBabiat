#include <Arduino.h>
#include <configuration.h>
#include <configGeneral.h>
#include <configVariables.h>
#include <UniversalTelegramBot.h>
#include "main.h"


#include <FS.h>
#include <TelegramCredentials.h>

#include <WifiApp.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ST7735.h>
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
  

#define LED 7
#define POT  6

#define LEDNOTIF 35
#define PINTONE 5

SPIClass spitft;
#define TFT_CS        26
#define TFT_RST       16 // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC        47
#define TFT_BACKLIGHT PIN_A3 // Display backlight pin
#define TFT_MOSI      33  // Data out
#define TFT_SCLK      34  // Clock out
Adafruit_ST7735 tft = Adafruit_ST7735 (TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);



//Adafruit_SSD1306* display;
int potRaw = 0;
int potValue = 0;
unsigned long receptionMessage = 0;

int displayNum = 0;
int maxDisplay = 4;

String bufferActionToSend;


dataTurbine_t dataTurbine;
nodeStatus_t TurbineStatus;

dataEtang_t dataEtang;
nodeStatus_t EtangStatus;

nodeStatus_t nodeTest;

WiFiClientSecure telegramClient;
UniversalTelegramBot TelegramBot(BOTtoken,telegramClient);
unsigned long telegramBot_lastTime = 0;


digitalInput btnPRG(0,INPUT_PULLUP);

Timer timerEnvoi(10000);
Timer timerEnvoiWS(1000);
Ecran Ec(&Wire);

Timer TftTimer (2500);

manuelController *manuelC = new manuelController();
basicController *bC = new basicController();
PIDController *pidC = new PIDController();

int modeActuel = 0;
//IController *modes[] = {bC,pidC};
LList<IController*> modes = LList<IController*>();

const int pinAnalogTest = 6;

int i = 0;

bool OtaUpdate = false;

void arduinoOtaSetup(void){
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
      OtaUpdate = true;
    })
    .onEnd([]() {
      Serial.println("\nEnd");
      OtaUpdate = false;
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      Ec.getDisplay()->clearDisplay();
      Ec.getDisplay()->setCursor(40,10);
      

      
      Ec.getDisplay()->println("OTA Update");
      Ec.getDisplay()->printf("Progress: %u%%\r\n", (progress / (total / 100)));
      Ec.getDisplay()->println(ArduinoOTA.getCommand() == U_FLASH ? "sketck":"fs");

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
  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = TelegramBot.messages[i].chat_id;
    String text = TelegramBot.messages[i].text;

    String from_name = TelegramBot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/open")
    {
      String keyboardJson = "[[{ \"text\" : \"Go to App\", \"url\" : \"http://hydro.hydro-babiat.ovh\" }],[{ \"text\" : \"Send\", \"callback_data\" : \"This was sent by inline\" }]]";
      TelegramBot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
    }

    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Flash Led Bot example.\n\n";
      welcome += "/ledon : to switch the Led ON\n";
      welcome += "/ledoff : to switch the Led OFF\n";
      welcome += "/status : Returns current status of LED\n";
      TelegramBot.sendMessage(chat_id, welcome, "Markdown");
    }

  }
}

String LoRaOnMsgStatut(){
  return "";
}

void LoRaMessage(LoRaPacket header, String msg)
{
  receptionMessage = millis();
  digitalWrite(LEDNOTIF,HIGH);
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
    }

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
        dataEtang.ratioNiveauEtang = val.toFloat();;
      }
      
    }
    
    
    
    
  } else if (header.Emetteur == 0x04)
  {
    nodeTest.RSSI = header.RSSI;
    nodeTest.dernierMessage = millis();
    nodeTest.SNR = header.SNR;
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
    Ec.getDisplay()->println("  : " + (String)((millis() - EtangStatus.dernierMessage)/1000));
    Ec.getDisplay()->println("[" + String(TurbineStatus.active?"x":" ") +"] Turbine: " + (String)TurbineStatus.RSSI);
    Ec.getDisplay()->println("  : " + (String)((millis() - TurbineStatus.dernierMessage)/1000));
    Ec.getDisplay()->println("[" + String(nodeTest.active?"x":" ") +"] Node Test: " + (String)nodeTest.RSSI);
    Ec.getDisplay()->println("  : " + (String)((millis() - nodeTest.dernierMessage)/1000));
    
    
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
    Ec.getDisplay()->println(WiFi.status());
    Ec.getDisplay()->println(WiFi.localIP().toString());
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
  EtangStatus.Name = "ETANG";


  TurbineStatus.addr = TURBINE;
  TurbineStatus.Name = "Turbine";

  nodeTest.addr = 0x04;
  nodeTest.Name = "NodeTest";
}

// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_OLED,SCL_OLED,400000);
  Serial.println("RSTOLED " + String(RST_OLED));
  //scanI2C(&Wire);
  pinMode(LEDNOTIF,OUTPUT);

  pinMode(POT,INPUT);

  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);

  modes.add(manuelC);
  modes.add(bC);
  modes.add(pidC);


  initNodes();
  
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

  pinMode(pinAnalogTest,INPUT);
  ledcSetup(1,1000,8);
  ledcAttachPin(LED,1);


 tft.initR();
 
 //tft.drawCircle(60,60,10,ST77XX_RED);
 tft.fillScreen(ST77XX_BLACK);
 tft.drawCircle(20,20,10,ST7735_WHITE);
 tft.setTextColor(ST7735_BLUE);
 //tft.drawCircle(20,-20,10,ST7735_ORANGE);
 //tft.drawCircle(-20,-20,10,ST7735_RED);
 //tft.drawCircle(-20,20,10,ST7735_BLUE);

  
  
  
  Ec.getDisplay()->setCursor(0,10);
  Ec.getDisplay()->print("LoRa Ok");
  Ec.getDisplay()->setCursor(60,10);
  Ec.getDisplay()->print("!");
  delay(1000);
  Ec.getDisplay()->display();
  
  Ec.getDisplay()->clearDisplay();
  Ec.getDisplay()->display();
  Serial.println("fin setup");

  WifiApp.begin();

  arduinoOtaSetup();

  telegramClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  timerEnvoi.reset();

  timerEnvoiWS.reset();


}

// put your main code here, to run repeatedly:
void loop() {

  ArduinoOTA.handle();

  btnPRG.loop();
  LoRa.loop();
  Ec.loop();

  // pidC->niveau = map(analogRead(pinAnalogTest),0,4095,0,100);
  // pidC->loop();

  modes.get(modeActuel)->niveau = map(analogRead(pinAnalogTest),0,4095,0,100);
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
    
  if (millis()> receptionMessage +300 && receptionMessage != 0)
  {
    receptionMessage = 0;
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
  
  if (timerEnvoi.isOver())
  {
    LoRa.sendData((i++%3)+2,LoRaMessageCode::DemandeStatut,"bonjour" + (String)random(0,100));
  }

  if (timerEnvoiWS.isOver())
  {
    WifiApp.notifyClients();
  }
  
  if (millis()> telegramBot_lastTime + 1000)
  {

    Serial.printf("%i [Telegram] debut\n",millis());
    int numNewMessages = TelegramBot.getUpdates(TelegramBot.last_message_received + 1);
    Serial.printf("%i [Telegram] %i\n",millis(),numNewMessages);
    while (numNewMessages)
    {
      handleTelegramMessage(numNewMessages);
      numNewMessages = TelegramBot.getUpdates(TelegramBot.last_message_received+1);

    }
    
    
    telegramBot_lastTime = millis();
  }
  
  
  if (TftTimer.isOver())
  {
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0,10);
    tft.println("Etang  : " + (String)EtangStatus.RSSI);
    tft.println("Turbine: " + (String)TurbineStatus.RSSI);
  }
  

  displayData();
  delay(10);
}