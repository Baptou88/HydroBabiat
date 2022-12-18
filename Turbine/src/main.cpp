#include <Arduino.h>
//#include "pinsDef.h"
#include "configGeneral.h"
#include "configuration.h"

#include "Adafruit_INA260.h"
#include "Adafruit_INA219.h"
#include "Preferences.h"
#include "ESPAsyncWebServer.h"
#include <AsyncElegantOTA.h>
#include <WiFi.h>

#include <menu/menunu.h>

#include "digitalInput.h"
#include "AnalogInput.h"


#include "LoRa.h"
#include "Ecran.h"
#include "Encoder.h"
#include "Moteur.h"
#include "transmission.h"
#include "Tachymetre.h"
#include "main.h"
#include "wifiCredentials.h"

#define ms_to_s_factor 1000
#define us_to_s_factor 1000000

AsyncWebServer server(80);



unsigned long wifiActivation = 0;
unsigned long receptionMessage = 0;
unsigned long dernierMessage = 0;
float msgRSSI = 0;
float msgSNR = 0;

Ecran Ec = Ecran();

Moteur mot;

Preferences pref;

Tachymetre tachy;

enum displayMode_e{
  AFFICHAGE_DEFAULT,
  MENU
};
int displayNum = 0;
displayMode_e displayMode = AFFICHAGE_DEFAULT;

menunu* menuParam;
menuItemList* menuRoot;

#if defined(LED_BUILTIN)
  #undef LED_BUILTIN
  #define LED_BUILTIN 35
#endif

float moteurKp = 2.2;
float moteurKi = 0;
float moteurKd = 0.4;

float voltage_coefA = 1;
float voltage_base = 0;

float current_coefA = 0.0245f;
float current_base = -2048;

//Fin de course Fermee
digitalInput FCF(PIN_FC_F,INPUT_PULLUP);
//Fin de course Ouvert
digitalInput FCO(PIN_FC_O,INPUT_PULLUP);

digitalInput btnPRG(0,INPUT_PULLUP);

digitalInput encodeurDT(PIN_ROTARY_DT,INPUT_PULLUP);
digitalInput encodeurCLK(PIN_ROTARY_CLK,INPUT_PULLUP);
digitalInput encodeurSW(PIN_ROTARY_SW,INPUT_PULLUP);

AnalogInput VoltageOutput(PIN_VOLTAGE_OUTPUT,voltage_coefA,voltage_base);
AnalogInput CurrentOutput(PIN_CURRENT_OUTPUT,current_coefA,current_base);

Adafruit_INA260 ina260 = Adafruit_INA260(); 
Adafruit_INA219 ina219 = Adafruit_INA219(); 

bool ledNotif = true;

float VoltageBattery = 0;


void displayData(){
  Ec.getDisplay()->clearDisplay();

  switch (displayNum)
  {
  case 0:
    Ec.getDisplay()->setCursor(0,0);
    Ec.getDisplay()->print("pos:");
    Ec.getDisplay()->setCursor(50,0);
    Ec.getDisplay()->print(EncoderVanne::getPos());

    Ec.getDisplay()->setCursor(0,12);
    Ec.getDisplay()->print("con:");
    Ec.getDisplay()->setCursor(50,12);
    Ec.getDisplay()->print((String)mot._target);
    
    Ec.getDisplay()->setCursor(0,24);
    Ec.getDisplay()->print("IM :");
    Ec.getDisplay()->setCursor(50,24);
    Ec.getDisplay()->print((String)mot.IntensiteMoteur);
    
    Ec.getDisplay()->setCursor(0,36);
    Ec.getDisplay()->print("om :");
    Ec.getDisplay()->setCursor(50,36);
    Ec.getDisplay()->print((String)mot.ouvertureMax);
    
    


    Ec.getDisplay()->setCursor(10,50);
    Ec.getDisplay()->print(FCF.getState());
    Ec.getDisplay()->setCursor(110,50);
    Ec.getDisplay()->print(FCO.getState());
    break;
  case 1:
    Ec.getDisplay()->setCursor(0,0);
    Ec.getDisplay()->println((String)((millis()-dernierMessage)/1000) + " s");
    Ec.getDisplay()->println("RSSI: " + (String)(msgRSSI) + " dbm");
    Ec.getDisplay()->println("SNR : " + (String)(msgSNR) + " db");
    break;
  case 2:
    Ec.getDisplay()->setCursor(0,0);
    Ec.getDisplay()->println("Tachy: " + (String)tachy.getRPM() + " rpm");
    Ec.getDisplay()->println("Tachy: " + (String)tachy.getHz() + " hz");

    Ec.getDisplay()->println("U: " + (String)VoltageOutput.getVoltage() + " V");
    Ec.getDisplay()->println("I: " + (String)CurrentOutput.getValue() + " A");
    Ec.getDisplay()->println("UB: " + (String)VoltageBattery + " mV");
    
    
    break;
  default:
    break;
  }


  Ec.getDisplay()->display();
}

String LoRaMesageStatut(){
  Serial.println("DemandeStatut");
  String toSend = "";
  toSend += "PM:" + (String) EncoderVanne::getPos() + ",";
  toSend += "PV:" + (String) transmission::ratiOuverture(mot) + ",";
  toSend += "target:" + (String) mot.getTargetP() + ",";
  toSend += "U:" + (String) VoltageOutput.getValue() + ",";
  toSend += "I:" + (String) CurrentOutput.getValue() + ",";
  toSend += "tachy:" + (String) tachy.getRPM() + ",";

  return toSend;
}

void LoRaMessage(LoRaPacket header, String msg){
  receptionMessage = millis();
  dernierMessage = millis();
  msgRSSI= header.RSSI;
  msgSNR= header.SNR;
  //message = msg;
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println(msg);
  
  
  if (ledNotif)
  {
    digitalWrite(LED_BUILTIN,HIGH);
  }
  


  commandProcess(msg);

  //TODO lorsque je recois un message je constate que le mcu perd ces infos
  //pinMode(PIN_FC_O,INPUT_PULLUP);
  //pinMode(PIN_FC_F,INPUT_PULLUP);
}

void printWakeUpReason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void commandProcess(String cmd){
  if (cmd.startsWith("TargetVanne="))
  {
    cmd.replace("TargetVanne=","");
    int consigne = transmission::ratioToTarget((cmd.toInt()),mot);
    Serial.printf("consigne %i\n",consigne);
    mot.setTarget(transmission::ratioToTarget((cmd.toInt() ),mot));
  }
  
  if (cmd.startsWith("ZV")) //set Zero to Voltage mesure
  {
    VoltageOutput.setZero();
  }
  if (cmd.startsWith("ZC")) //set Zero to Current mesure
  {
    VoltageOutput.setZero();
  }
  if (cmd.startsWith("DS"))
  {
    //TODO
    esp_sleep_enable_timer_wakeup(10 * us_to_s_factor);
    pref.putDouble("positionMoteur",mot._position);
    Serial.println(mot._position);
    delay(100);
    
    Ec.setSleep();
    esp_deep_sleep_start();
  }
  
}

void initLoRa()
{
  LoRa.setNodeID(NODEID);
  LoRa.onMessageStatut(LoRaMesageStatut);
  LoRa.onMessage(LoRaMessage);

  if (LoRa.begin() != RADIOLIB_ERR_NONE)
  {
    Serial.println("[LoRa] init fail");
    while (true)
    {
      /* code */
    }
    
  }
  Serial.println("[LoRa] init success !");
  LoRa.getRadio().startReceive();
  
}

bool initPreferences(){

  if(!pref.begin("Turbine"))
  {
    return false;
  }
  mot.ouvertureMax = pref.getInt("ouvertureMax",mot.ouvertureMax);

  moteurKp = pref.getFloat("moteurKp",moteurKp);
  moteurKi = pref.getFloat("moteurKi",moteurKi);
  moteurKd = pref.getFloat("moteurKd",moteurKd);

  voltage_coefA = pref.getFloat("voltage_coefA",voltage_coefA);
  voltage_base = pref.getFloat("voltage_base",voltage_base);
  current_coefA = pref.getFloat("current_coefA",current_coefA);
  current_base = pref.getFloat("current_base",current_base);


  return true;
}

bool savePreferences(){

  Serial.println("save pref !");
  pref.putInt("ouvertureMax",mot.ouvertureMax);
  pref.putFloat("moteurKp",moteurKp);
  pref.putFloat("moteurKi",moteurKi);
  pref.putFloat("moteurKd",moteurKd);

  pref.putFloat("voltage_coefA",voltage_coefA);
  pref.putFloat("voltage_base",voltage_base);
  pref.putFloat("current_coefA",current_coefA);
  pref.putFloat("current_base",current_base);

  return true;
}

void acquisitionEntree(){
  FCF.loop();
  FCO.loop();
  btnPRG.loop();
  encodeurCLK.loop();
  encodeurDT.loop();
  encodeurSW.loop();

  mot.updateIntensiteMoteur(ina260.readCurrent());
  VoltageOutput.loop();
  CurrentOutput.loop();
  VoltageBattery = ina260.readBusVoltage();
  
}

void menuSaveCalleback(Adafruit_SSD1306* display,bool firstTime){
  if (firstTime)
  {
    savePreferences();
  }
  
  
  display->clearDisplay();
  display->setCursor(0,0);
  display->println("Save ok !");
  display->display();
  
}

void menuVZCalleback(Adafruit_SSD1306* display,bool firstTime){
  if (firstTime)
  {
    VoltageOutput.setZero();
  }
  
  display->clearDisplay();
  display->setCursor(0,0);
  display->println("Voltage set Zero ok !");
  display->display();

}

void menuCZCalleback(Adafruit_SSD1306* display,bool firstTime){
  if (firstTime)
  {
    CurrentOutput.setZero();
  }
  
  display->clearDisplay();
  display->setCursor(0,0);
  display->println("current set Zero ok !");
  display->display();

}

void menuWifiServerCalleback(Adafruit_SSD1306* display,bool firstTime)
{
  if (firstTime)
  {
    wifiActivation = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID,PASSWORD);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  }
  display->setCursor(0,0);
  display->println(SSID);
  if (WiFi.status() == WL_CONNECTED)
  {
    display->println(WiFi.localIP().toString());
  } else
  {
    display->println("Not Connected");
    
  }
  
  

}

// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_OLED,SCL_OLED);
  
  FCO.begin();
  FCF.begin();
  btnPRG.begin();
  encodeurCLK.begin();
  encodeurDT.begin();
  encodeurSW.begin();

  pinMode(LED_BUILTIN,OUTPUT);

  VoltageOutput.begin();
  CurrentOutput.begin();

  if (!Ec.begin())
  {
    Serial.println("SSD1306 init failed");
    while (true)
    {
      /* code */
    }
    
   }
  Ec.getDisplay()->display();
  
  
  EncoderVanne::setPins(PIN_ENCODEUR_CW, PIN_ENCODEUR_CCW);
  EncoderVanne::begin();
  
  mot.begin(pinMoteurCW,pinMoteurCCW);
  mot.setPID(2.2,0,0.4);
  mot.setSpeedLimit(30,100);
  mot.setEndstop(&FCF,&FCO);

  if (esp_sleep_get_wakeup_cause() != esp_sleep_wakeup_cause_t::ESP_SLEEP_WAKEUP_UNDEFINED)
  {
    Serial.println("TODO: ignorer init");
    mot.setState(MotorState::IDLE);
    Serial.println("au reveil : " + String(pref.getDouble("positionMoteur",0)));
    mot._position = pref.getDouble("positionMoteur",0);
  }
  
  printWakeUpReason();

#ifdef PIN_TACHY
tachy.setTimeout(2E6);
  pinMode(PIN_TACHY,INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(PIN_TACHY),[](){
    //ets_printf("Interrupt");
    tachy.Tick();
  },RISING);
  
  #ifdef PIN_TEST_ANALOG

  #endif
#endif


  initLoRa();

  if (!initPreferences())
  {
    Serial.println("initPreferences failed ");
    while (true)
    {

    }
    
  }
  

  Ec.getDisplay()->setCursor(0,12);
  if (!ina260.begin())
  {
    Ec.getDisplay()->println("failed init Ina260");
    Serial.println("failed init Ina260");
    Ec.getDisplay()->display();
    while (true)
    {  }
    
  }
  Ec.getDisplay()->println("ok init Ina260");
  Ec.getDisplay()->display();

  if (!ina219.begin())
  {
    Ec.getDisplay()->println("failed init Ina219");
    Serial.println("failed init Ina219");
    Ec.getDisplay()->display();
    while (true)
    {  }
    
  }
  Ec.getDisplay()->println("ok init Ina219");
  Ec.getDisplay()->display();

  menuParam = new menunu(Ec.getDisplay());

  menuRoot = new menuItemList((char*)"Param",menuParam);
  menuRoot->addItem(menuParam, new menuItembool((char*)"LED notif",&ledNotif));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"moteur Kp ",&moteurKp,0,100));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"moteur Ki ",&moteurKi,0,100));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"moteur Kd ",&moteurKd,0,100));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"voltage ka ",&voltage_coefA,0,100));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"voltage z ",&voltage_base,0,100));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"current ka ",&current_coefA,0,100));
  menuRoot->addItem(menuParam,new menuItemFloat((char*)"current z ",&current_base,0,100));
  menuRoot->addItem(menuParam,new menuItemCalleback((char*)"voltage set z",menuVZCalleback));
  menuRoot->addItem(menuParam,new menuItemCalleback((char*)"current set z",menuCZCalleback));
  menuRoot->addItem(menuParam,new menuItemCalleback((char*)"Save",menuSaveCalleback));
  menuRoot->addItem(menuParam,new menuItemCalleback((char*)"Wifi",menuWifiServerCalleback));
  menuParam->actual=menuRoot;
  delay(1000);

  Serial.println("CPU  Freq: " + (String)getCpuFrequencyMhz());
  Serial.println("XTAL Freq: " + (String)getXtalFrequencyMhz());
  //setCpuFrequencyMhz(80);
  Serial.println("CPU Freq: " + (String)getCpuFrequencyMhz());
  
}

// put your main code here, to run repeatedly:
void loop() {
  LoRa.loop();
  Ec.loop();
  acquisitionEntree();

  if (btnPRG.frontMontant())
  {
    if (Ec.getState()== EcranState::EcranState_Sleep)
    {
      Ec.wakeUp();
    } else
    {
      displayNum = (displayNum+1)%3;
      if (mot.getState() == MotorState::WAIT_INIT)
      {
        mot.setState(MotorState::IDLE);
      }
      
    }
   
  }
  static unsigned long lastprgmillis = 0;

  if (btnPRG.pressedTime()> 2000 && millis() > lastprgmillis +1000)
  {
    lastprgmillis = millis();
    if (displayMode == AFFICHAGE_DEFAULT)
    {
      displayMode = MENU;
    } else
    {
      displayMode = AFFICHAGE_DEFAULT;
    }
    
    
    
  }
  
  
  if (millis()> receptionMessage + 200 && receptionMessage != 0)
  {
    receptionMessage = 0;
    digitalWrite(LED_BUILTIN,LOW);
  }

  mot.setPosition(EncoderVanne::getPos());

  if (Serial.available()>0)
  {
    String test = Serial.readStringUntil('\n');
    
    commandProcess(test);
  }
  mot.loop();

//gestion wifi
  if (millis()> wifiActivation + 60000 && wifiActivation != 0)
  {
    Serial.println("desactivation wifi");
    wifiActivation = 0;
    WiFi.mode(WIFI_OFF);
    server.end();
    

  }


  switch ( displayMode)
  {
  case AFFICHAGE_DEFAULT:
    displayData();
    break;
  case MENU:

    if (encodeurCLK.frontDesceandant() && encodeurDT.isReleased())
    {
      
      menuParam->next();
    }
    if (encodeurCLK.isReleased() && encodeurDT.frontDesceandant())
    {
      
      menuParam->prev();
    }
    if (encodeurSW.frontDesceandant())
    {
      
      menuParam->select();
    }
    
    if (menuParam == NULL)
    {
      Serial.println("null");
      return;
    }
    //Serial.println("menuloop");
    Ec.getDisplay()->clearDisplay();
    menuParam->loop();
    Ec.getDisplay()->display();
  
    break;
  default:
    break;
  }
  
}