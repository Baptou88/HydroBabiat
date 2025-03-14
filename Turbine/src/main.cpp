#include <Arduino.h>
// #include "pinsDef.h"
#include "configGeneral.h"
#include "configuration.h"

#include "Adafruit_INA260.h"
#include "Adafruit_INA219.h"
#include "Adafruit_ADS1X15.h"
#include "Preferences.h"
#include "ESPAsyncWebServer.h"
#include <ElegantOTA.h>
#include <WiFi.h>

#include <menu/menunu.h>

#include "digitalInput.h"
// #include "AnalogInput.h"

#include "LoRa.h"
#include "Ecran.h"
#include "Encoder.h"
#include "Moteur.h"
#include "transmission.h"

#include "main.h"
#include "wifiCredentials.h"

#include "moyenneGlissante.h"

#define ms_to_s_factor 1000
#define us_to_s_factor 1000000

AsyncWebServer server(80);

unsigned long wifiActivation = 0;
unsigned long ledReceptionMessage = 0;
unsigned long dernierMessage = 0;

unsigned long shouldReboot = 0;

unsigned long messageReponse = 0;
String msgReponse = "";

float msgRSSI = 0;
float msgSNR = 0;

int timeDeepSleeping = 0;

Adafruit_ADS1115 ads;
bool calibrationADS = false;

Ecran Ec = Ecran();

Moteur mot;

Preferences pref;

Tachymetre tachy;
/// @brief vitesse max de la turbine
int maxSpeed = 300;
float rpm = 0;

enum displayMode_e
{
  AFFICHAGE_DEFAULT,
  MENU,
  InitPos
};
int displayNum = 0;
displayMode_e displayMode = AFFICHAGE_DEFAULT;

menunu *menuParam;
menuItemList *menuRoot;

int16_t rawCurrentADS = 0;
int16_t rawTensionADS = 0;

#if defined(LED_BUILTIN)
  #undef LED_BUILTIN
  #define LED_BUILTIN 35
#endif

float moteurKp = 2.2;
float moteurKi = 0;
float moteurKd = 0.4;


float voltage_coefA = 1 / 63.9f;
float voltage_base = -40;
float voltageOutput = 0;
moyenneGlissante voltageOutputMoy(50);


float current_coefA = 1 / 356.25f;
float current_base = -13705;
float currentOutput = 0;

// Fin de course Fermee
digitalInput FCF(PIN_FC_F, INPUT_PULLUP);
// Fin de course Ouvert
digitalInput FCO(PIN_FC_O, INPUT_PULLUP);

digitalInput btnPRG(0, INPUT_PULLUP);

digitalInput encodeurDT(PIN_ROTARY_DT, INPUT_PULLUP);
digitalInput encodeurCLK(PIN_ROTARY_CLK, INPUT_PULLUP);
digitalInput encodeurSW(PIN_ROTARY_SW, INPUT_PULLUP);

// AnalogInput VoltageOutput(PIN_VOLTAGE_OUTPUT,voltage_coefA,voltage_base);
// AnalogInput CurrentOutput(PIN_CURRENT_OUTPUT,current_coefA,current_base);

Adafruit_INA260 ina260 = Adafruit_INA260();
Adafruit_INA219 ina219(0x44);
float currentSysteme = 0;

bool ledNotif = true;

float VoltageBattery = 0;

int16_t currentSCT = 0;
unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void displayData()
{
  ADAFRUIT_DISPLAY* display = Ec.getDisplay();
  display->clearDisplay();

  if (mot.getState() == MotorState::WAIT_INIT)
  {
    display->setCursor(0,0);
    display->println("Attente avant init");
    display->display();
    return;
  }
  
  switch (displayNum)
  {
    case 0:
      display->setCursor(0, 0);
      display->print("pos:");
      display->setCursor(50, 0);
      display->print(EncoderVanne::getPos());

      display->setCursor(0, 12);
      display->print("con:");
      display->setCursor(50, 12);
      display->print((String)mot._target);

      display->setCursor(0, 24);
      display->print("IM :");
      display->setCursor(50, 24);
      display->print((String)mot.IntensiteMoteur);

      display->setCursor(0, 36);
      display->print("om :");
      display->setCursor(50, 36);
      display->print((String)mot.ouvertureMax);

      display->setCursor(20, 50);
      display->print(MotorStateToString(mot.getState()));

      display->setCursor(5, 50);
      display->print(FCF.getState());
      display->setCursor(115, 50);
      display->print(FCO.getState());

      break;
    case 1:
      display->setCursor(0, 0);
      display->println((String)((millis() - dernierMessage) / 1000) + " s");
      display->println("RSSI: " + (String)(msgRSSI) + " dbm");
      display->println("SNR : " + (String)(msgSNR) + " db");

      break;
    case 2:
      display->setCursor(0, 0);
      display->println("Tachy: " + (String)rpm + " rpm");
      display->println("Tachy: " + (String)tachy.getHz() + " hz");


      display->println("UB: " + (String)VoltageBattery + " mV");
      display->println("Isysteme: " + (String)currentSysteme + " mA");

      break;

    case 3:
      display->setCursor(0, 0);
      display->println("U moyenne: " + (String)voltageOutputMoy.get() + " V");
      display->println("U        : " + (String)voltageOutput + " V");
      display->println("U ads    : " + (String)ads.computeVolts(ads.readADC_SingleEnded(VOLTAGE_ADS_CHANNEL)) + " V");
      display->println("I ads    : " + (String)ads.computeVolts(ads.readADC_SingleEnded(CURRENT_ADS_CHANNEL)) + " A");
      display->println("I: " + (String)currentOutput + " A");
      break;
    case 4:
      display->setCursor(0, 0);
      display->println("CLK: " + (String) encodeurCLK.frontDesceandant() + " " + (String) encodeurCLK.frontDesceandant()+ " " + (String)encodeurCLK.getState());
      display->println("DT : " + (String) encodeurDT.frontDesceandant() + " " + (String) encodeurDT.frontDesceandant()+ " " + (String)encodeurDT.getState());
      
      break;
    default:
      break;
  }

  display->display();
}

String LoRaMesageStatut()
{
  Serial.println("DemandeStatut");
  String toSend = "";
  toSend += "PM:" + (String)EncoderVanne::getPos() + ",";
  toSend += "PV:" + (String)transmission::ratiOuverture(mot) + ",";
  toSend += "target:" + (String)mot.getTargetP() + ",";
  toSend += "U:" + String(voltageOutputMoy.get(),1) + ",";
  toSend += "I:" + String(currentOutput,1) + ",";
  toSend += "tachy:" + String(rpm,1) + ",";
  toSend += "UB:" + String(VoltageBattery,0) + ",";
  toSend += "motorState:" + (String)mot.getState() + ",";
  toSend += "ZV:" + String(voltage_base,3) + ",";
  toSend += "AV:" + String(voltage_coefA,3) + ",";
  toSend += "ZC:" + String(current_base,3) + ",";
  toSend += "AC:" + String(current_coefA,3) + ",";
  toSend += "MS:" + (String)mot.getSpeed() + ",";
  toSend += "currentSyst:" + String(currentSysteme,0);

  return toSend;
}

void LoRaMessage(LoRaPacket header, String msg)
{
  ledReceptionMessage = millis();
  dernierMessage = millis();
  msgRSSI = header.RSSI;
  msgSNR = header.SNR;
  // message = msg;
  // digitalWrite(LED_BUILTIN,HIGH);
  Serial.println(msg);

  msgReponse = "";

  if (ledNotif)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  commandProcess(msg);

  if (header.Code == LoRaMessageCode::Data)
  {
    messageReponse = millis() + 100;
  }
}

float calibrateADS(int channel, int sample , int delaybetweensample )
{
  ads.readADC_SingleEnded(channel);

  int sum = 0;
  for (size_t i = 0; i < sample; i++)
  {
    sum += ads.readADC_SingleEnded(channel);
    delay(delaybetweensample);
  }
  float average = sum / sample;
  return average;
}

void printWakeUpReason()
{
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    Serial.println("Wakeup caused by touchpad");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  default:
    Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
    break;
  }
}

void commandProcess(String cmd)
{

  if (cmd.startsWith("Reboot"))
  {
    shouldReboot = millis() + 3000;
    msgReponse += "Reboot en cours";
  }
  if (cmd.startsWith("CpuFreq"))
  {
    cmd.replace("CpuFreq", "");
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      setCpuFrequencyMhz(cmd.toInt());
    }

    Serial.println("CpuFreq: " + (String)getCpuFrequencyMhz());
    msgReponse += "CpuFreq:" + (String)getCpuFrequencyMhz();
  }

  if (cmd.startsWith("DeepSleep"))
  {
    // delai avant de dormir, default 10 secondes
    int delayUs = 10e6;
    cmd.replace("DeepSleep", "");
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      delayUs = cmd.toInt();
    }
    timeDeepSleeping = delayUs;
  }
  if (cmd.startsWith("LightSleep"))
  {
    Serial.println("Exec Light Sleep");
    digitalWrite(LED_BUILTIN, HIGH);
    // esp_sleep_enable_ext0_wakeup(GPIO_NUM_14,HIGH);
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_13, HIGH);
    esp_light_sleep_start();
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("Finish Light Sleep");
  }

  if (cmd.startsWith("TargetVanne="))
  {
    cmd.replace("TargetVanne=", "");
    int consigne = transmission::ratioToTarget((cmd.toInt()), mot);
    Serial.printf("consigne %i\n", consigne);
    mot.setTarget(transmission::ratioToTarget((cmd.toInt()), mot));
    msgReponse += "Consigne Vanne : " + (String)cmd.toInt();
  }

  if (cmd.startsWith("ZV")) // set Zero to Voltage mesure
  {
    // VoltageOutput.setZero();
    // VoltageOutput.calibrate();
  }
  // if (cmd.startsWith("ZC")) //set Zero to Current mesure
  // {
  //   //CurrentOutput.setZero();
  //   CurrentOutput.calibrate();
  // }
  if (cmd.startsWith("AV"))
  {
    cmd.replace("AV", "");
    Serial.printf("voltage coef: %f \n", voltage_coefA);
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      voltage_coefA = cmd.toFloat();
      Serial.printf("voltage coef: %f \n", voltage_coefA);
    }
  }
  if (cmd.startsWith("AC"))
  {
    cmd.replace("AC", "");
    Serial.printf("current coef: %f \n", current_coefA);
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      current_coefA = cmd.toFloat();
      Serial.printf("current coef: %f \n", current_coefA);
    }
  }
  // if (cmd.startsWith("AC="))
  // {
  //   cmd.replace("AC=","");
  //   CurrentOutput.changeAparam(cmd.toFloat());
  // }
  if (cmd.startsWith("calibrate"))
  {
    Serial.println("calibrate: ");
    calibrationADS = true;
    msgReponse += "Calibration ...";
    // VoltageOutput.calibrate();
    // CurrentOutput.calibrate();
  }

  if (cmd.startsWith("OM"))
  {
    int om = 6000;
    cmd.replace("OM", "");
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      om = cmd.toInt();
    }
    mot.ouvertureMax = om;
    msgReponse += "Ouverture Max: " + (String)om;
  }
  if (cmd.startsWith("FT"))
  {
    cmd.replace("FT", "");
    mot.setState(MotorState::FERMETURE_TOTALE);
    msgReponse += "Fermeture Totale ...";
  }
  if (cmd.startsWith("OT"))
  {
    cmd.replace("OT", "");
    mot.setState(MotorState::OUVERTURE_TOTALE);
    msgReponse += "Ouverture Totale ...";
  }
  if (cmd.startsWith("SavePref"))
  {
    cmd.replace("SavePref", "");
    msgReponse += "Sauvegarde Pref: " + (String)savePreferences();
  }

  if (cmd.startsWith("initVanne"))
  {
    mot.setState(MotorState::INIT_POS_MIN);
    msgReponse += "initVanne";
  }
  if (cmd.startsWith("idle"))
  {
    mot.setState(MotorState::IDLE);
    msgReponse += "mode idle";
  }
  if (cmd.startsWith("autotune"))
  {
    mot.setState(MotorState::AUTOTUNE);
  }
  if (cmd.startsWith("PID"))
  {
    cmd.replace("PID", "");

    Serial.printf("P %f I %f D %f\n", mot.PIDMoteur.GetKp(), mot.PIDMoteur.GetKi(), mot.PIDMoteur.GetKd());
    if (cmd.startsWith("P"))
    {
      cmd.replace("P", "");
      moteurKp = cmd.toFloat();
      mot.PIDMoteur.SetTunings(moteurKp, moteurKi, moteurKd);
      Serial.printf("P %f I %f D %f\n", mot.PIDMoteur.GetKp(), mot.PIDMoteur.GetKi(), mot.PIDMoteur.GetKd());
    }
    if (cmd.startsWith("I"))
    {
      cmd.replace("I", "");
      moteurKi = cmd.toFloat();
      mot.PIDMoteur.SetTunings(moteurKp, moteurKi, moteurKd);
      Serial.printf("P %f I %f D %f\n", mot.PIDMoteur.GetKp(), mot.PIDMoteur.GetKi(), mot.PIDMoteur.GetKd());
    }
    if (cmd.startsWith("D"))
    {
      cmd.replace("D", "");
      moteurKd = cmd.toFloat();
      mot.PIDMoteur.SetTunings(moteurKp, moteurKi, moteurKd);
      Serial.printf("P %f I %f D %f\n", mot.PIDMoteur.GetKp(), mot.PIDMoteur.GetKi(), mot.PIDMoteur.GetKd());
    }
  }

  if (cmd.startsWith("IMM")) // intensité max moteur
  {
    cmd.replace("IMM", "");
    Serial.printf("IMM %f\n", mot.maxItensiteMoteur);
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      mot.maxItensiteMoteur = cmd.toFloat();
    }
    msgReponse += "Intensite max Moteur " + (String)mot.maxItensiteMoteur;
  }

  if (cmd.startsWith("MaxSpeed"))
  {
    cmd.replace("MaxSpeed", "");
    Serial.println("Turbine MaxSpeed: " + (String)maxSpeed);
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      maxSpeed = cmd.toInt();
      Serial.println("Turbine MaxSpeed: " + (String)maxSpeed);
    } else if (cmd.startsWith("!"))
    {
      mot.setState(MotorState::OVERSPEED);
    }
    
    msgReponse += "Max Speed: " + (String)maxSpeed;
  }

  if (cmd.startsWith("tackyDebounce"))
  {
    cmd.replace("tackyDebounce","");
    if (cmd.startsWith("="))
    {
      cmd.replace("=", "");
      tachy.setDebounceTime(cmd.toInt());
    }
    msgReponse += "tackyDebounce " + (String)tachy.getDebounceTime();
    
  }
  if (cmd.startsWith("printPreferences"))
  {
    printPreferences();
  }
  if (cmd.startsWith("stop"))
  {
    mot.setState(MotorState::STOP);
    msgReponse += "STOP";
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

bool initPreferences()
{

  if (!pref.begin("Turbine"))
  {
    return false;
  }
  setCpuFrequencyMhz(pref.getInt("CpuFreq", 240));

  Serial.printf("cpu freq: %i \n", getCpuFrequencyMhz());
  mot.ouvertureMax = pref.getInt("ouvertureMax", mot.ouvertureMax);

  moteurKp = pref.getFloat("moteurKp", moteurKp);
  moteurKi = pref.getFloat("moteurKi", moteurKi);
  moteurKd = pref.getFloat("moteurKd", moteurKd);
  mot.setPID(moteurKp, moteurKi, moteurKd);

  ledNotif = pref.getBool("ledNotif", ledNotif);

  voltage_coefA = pref.getFloat("voltage_coefA", voltage_coefA);
  voltage_base = pref.getFloat("voltage_base", voltage_base);
  current_coefA = pref.getFloat("current_coefA", current_coefA);
  current_base = pref.getFloat("current_base", current_base);

  mot.maxItensiteMoteur = pref.getFloat("MaxIMot", mot.maxItensiteMoteur);

  maxSpeed = pref.getInt("maxSpeed", maxSpeed);

  tachy.setDebounceTime( pref.getLong("tackyDebounce",1000));
  return true;
}

bool savePreferences()
{

  Serial.println("save pref !");
  pref.putInt("CpuFreq", getCpuFrequencyMhz());
  pref.putInt("ouvertureMax", mot.ouvertureMax);
  pref.putFloat("moteurKp", mot.PIDMoteur.GetKp());
  pref.putFloat("moteurKi", mot.PIDMoteur.GetKi());
  pref.putFloat("moteurKd", mot.PIDMoteur.GetKd());
  pref.putBool("ledNotif", ledNotif);

  pref.putFloat("voltage_coefA", voltage_coefA);
  pref.putFloat("voltage_base", voltage_base);
  pref.putFloat("current_coefA", current_coefA);
  pref.putFloat("current_base", current_base);

  pref.putFloat("MaxIMot", mot.maxItensiteMoteur);

  pref.putInt("maxSpeed", maxSpeed);

  pref.putLong("tackyDebounce",tachy.getDebounceTime());
  return true;
}
void printPreferences()
{

  Serial.println("print pref :");
  Serial.println("CpuFreq " + (String) pref.getInt("CpuFreq"));
  Serial.println("ouvertureMax " + (String) pref.getInt("ouvertureMax"));
  Serial.println("moteurKp " + (String) pref.getFloat("moteurKp"));
  Serial.println("moteurKi " + (String) pref.getFloat("moteurKi"));
  Serial.println("moteurKd " + (String) pref.getFloat("moteurKd"));
  Serial.println("ledNotif " + (String) pref.getBool("ledNotif"));
  Serial.println("voltage_coefA " + (String) pref.getFloat("voltage_coefA"));
  Serial.println("voltage_base " + (String) pref.getFloat("voltage_base"));
  Serial.println("current_coefA " + (String) pref.getFloat("current_coefA"));
  Serial.println("current_base " + (String) pref.getFloat("current_base"));
  Serial.println("MaxIMot " + (String) pref.getFloat("MaxIMot"));
  Serial.println("maxSpeed " + (String) pref.getInt("maxSpeed"));
  Serial.println("tackyDebounce " + (String) pref.getLong("tackyDebounce"));
  
}

void acquisitionEntree()
{
  FCF.loop();
  FCO.loop();
  btnPRG.loop();
  encodeurCLK.loop();
  encodeurDT.loop();
  encodeurSW.loop();
  rpm = tachy.getRPM();
  
  // currentSCT = ads.readADC_Differential_2_3()*0.0625f*30;
  mot.updateIntensiteMoteur(ina260.readCurrent());
  // VoltageOutput.loop();
  // CurrentOutput.loop();
  VoltageBattery = ina260.readBusVoltage();
  currentSysteme = ina219.getCurrent_mA();

  //ads.readADC_SingleEnded(VOLTAGE_ADS_CHANNEL);
  rawTensionADS = ads.readADC_SingleEnded(VOLTAGE_ADS_CHANNEL);
  // Serial.println("rawtensionAds " + (String))
  voltageOutput = voltage_coefA * (rawTensionADS + voltage_base);
  voltageOutputMoy.add(voltageOutput);
  rawCurrentADS = ads.readADC_SingleEnded(CURRENT_ADS_CHANNEL);
  currentOutput = current_coefA * (rawCurrentADS + current_base);
}

void menuSaveCalleback(ADAFRUIT_DISPLAY *display, bool firstTime)

{
  if (firstTime)
  {
    savePreferences();
  }

  display->clearDisplay();
  display->setCursor(0, 0);
  display->println("Save ok !");
  display->display();
}


void menuSavePosMotCalleback(ADAFRUIT_DISPLAY *display, bool firstTime)

{
  if (firstTime)
  {
    pref.putDouble("posMot", mot._position);
  }

  display->clearDisplay();
  display->setCursor(0, 0);
  display->println("Save Pos Mot ok !");
  display->display();
}


void menuVZCalleback(ADAFRUIT_DISPLAY *display, bool firstTime)

{
  if (firstTime)
  {
    voltage_base = -calibrateADS(VOLTAGE_ADS_CHANNEL);
  }

  display->clearDisplay();
  display->setCursor(0, 0);
  display->println("Voltage set Zero ok !");
  display->display();
}


void menuCZCalleback(ADAFRUIT_DISPLAY *display, bool firstTime)
{
  if (firstTime)
  {
    current_base = -calibrateADS(CURRENT_ADS_CHANNEL);
  }

  display->clearDisplay();
  display->setCursor(0, 0);
  display->println("current set Zero ok !");
  display->display();
}

void menuWifiServerCalleback(ADAFRUIT_DISPLAY *display, bool firstTime)
{
  if (firstTime)
  {
    wifiActivation = millis();
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/plain", "Hi! I am ESP32."); });

    ElegantOTA.begin(&server); // Start ElegantOTA
    ElegantOTA.onStart(onOTAStart);
    ElegantOTA.onProgress(onOTAProgress);
    ElegantOTA.onEnd(onOTAEnd);
    server.begin();
  }
  display->setCursor(0, 0);
  display->println(SSID);
  if (WiFi.status() == WL_CONNECTED)
  {
    display->println(WiFi.localIP().toString());
  }
  else
  {
    display->println("Not Connected");
  }
}


#ifdef PIN_TACHY
IRAM_ATTR void tachyTick(){
  tachy.tick();
}
#endif

// put your setup code here, to run once:
void setup()
{

  Serial.begin(115200);

  Wire.begin(SDA_OLED, SCL_OLED);


  FCO.begin();
  FCF.begin();
  btnPRG.begin();
  encodeurCLK.begin();
  encodeurDT.begin();
  encodeurSW.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.println(WiFi.setSleep(true) ? "Wifi in sleep Mode" : "Wifi NOT in sleep Mode");
  Serial.println(btStop() ? "Bluetooth in sleep Mode" : "Bluetooth NOT in sleep Mode");

  // VoltageOutput.begin();
  // CurrentOutput.begin();

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

  mot.begin(pinMoteurCW, pinMoteurCCW);
  mot.setPID(2.2, 0, 0.4);
  mot.setSpeedLimit(20, 100);
  mot.setEndstop(&FCF, &FCO);

  printWakeUpReason();

#ifdef PIN_TACHY
  tachy.setTimeout(2E6);
  pinMode(PIN_TACHY, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_TACHY), tachyTick, FALLING);//TODO a changer par FALLING ou CHANGE???

#endif

  initLoRa();

  if (!initPreferences())
  {
    Serial.println("initPreferences failed ");
    while (true)
    {
    }
  }
  Ec.getDisplay()->println("Pref Ok!");

  if (esp_sleep_get_wakeup_cause() != esp_sleep_wakeup_cause_t::ESP_SLEEP_WAKEUP_UNDEFINED)
  {
    Serial.println("TODO: ignorer init");
    mot.setState(MotorState::IDLE);
    Serial.println("au reveil : " + String(pref.getDouble("posMot", 0)));
    mot._position = pref.getDouble("posMot", 0);
    EncoderVanne::setPosition(pref.getDouble("posMot", 0));
  }

  if (!ina260.begin())
  {
    Ec.getDisplay()->println("failed init Ina260");
    Serial.println("failed init Ina260");
    Ec.getDisplay()->display();
    while (true)
    {
    }
  }
  Ec.getDisplay()->println("ok init Ina260");
  Ec.getDisplay()->display();

  if (!ina219.begin())
  {
    Ec.getDisplay()->println("failed init Ina219");
    Serial.println("failed init Ina219");
    Ec.getDisplay()->display();
    while (true)
    {
    }
  }
  Ec.getDisplay()->println("ok init Ina219");
  Ec.getDisplay()->display();

  if (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    while (1)
      ;
  }

  // TODO Essayer de regler l'ADS plus finement , réglage du gain, echantillonage ...
  ads.setGain(adsGain_t::GAIN_ONE);
  ads.setDataRate(128);
  Serial.println("ADS datat rate: " + (String)ads.getDataRate());

  Ec.getDisplay()->println("ok init ads");
  Ec.getDisplay()->display();

  menuParam = new menunu(Ec.getDisplay());

  menuRoot = new menuItemList((char *)"Param", menuParam);
  menuRoot->addItem(menuParam, new menuItembool((char *)"LED notif", &ledNotif));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"moteur Kp ", &moteurKp, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"moteur Ki ", &moteurKi, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"moteur Kd ", &moteurKd, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"voltage ka ", &voltage_coefA, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"voltage z ", &voltage_base, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"current ka ", &current_coefA, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"current z ", &current_base, 0, 100));
  menuRoot->addItem(menuParam, new menuItemFloat((char *)"Max It Mot ", &mot.maxItensiteMoteur, 0, 100));
  menuRoot->addItem(menuParam, new menuItemCalleback((char *)"voltage set z", menuVZCalleback));
  menuRoot->addItem(menuParam, new menuItemCalleback((char *)"current set z", menuCZCalleback));
  menuRoot->addItem(menuParam, new menuItemCalleback((char *)"Save", menuSaveCalleback));
  menuRoot->addItem(menuParam, new menuItemCalleback((char *)"SavePosMot", menuSavePosMotCalleback));
  menuRoot->addItem(menuParam, new menuItemCalleback((char *)"Wifi", menuWifiServerCalleback));
  menuParam->actual = menuRoot;
  delay(1000);

  Serial.println("CPU  Freq: " + (String)getCpuFrequencyMhz());
  Serial.println("XTAL Freq: " + (String)getXtalFrequencyMhz());

  Serial.println("CPU Freq: " + (String)getCpuFrequencyMhz());
}

// put your main code here, to run repeatedly:
void loop()
{
  LoRa.loop();
  Ec.loop();
  acquisitionEntree();
  ElegantOTA.loop();
  if (btnPRG.frontMontant())
  {
    if (Ec.getState() == EcranState::EcranState_Sleep)
    {
      Ec.wakeUp();
    }
    else
    {
      if (mot.getState() == MotorState::WAIT_INIT)
      {
        mot.setState(MotorState::IDLE);
      } else
      {
        displayNum = (displayNum + 1) % 5;
      }
      
    }
  }
  static unsigned long lastprgmillis = 0;

  if (btnPRG.pressedTime() > 2000 && millis() > lastprgmillis + 1000)
  {
    lastprgmillis = millis();
    if (displayMode == AFFICHAGE_DEFAULT)
    {
      displayMode = MENU;
    }
    else
    {
      displayMode = AFFICHAGE_DEFAULT;
    }
  }
  // if (mot.getState() == MotorState::WAIT_INIT)
  // {
  //   displayMode = displayMode_e::InitPos;
  // }

  if (millis() > ledReceptionMessage + 200 && ledReceptionMessage != 0)
  {
    ledReceptionMessage = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }

  mot.setPosition(EncoderVanne::getPos());

  if (Serial.available() > 0)
  {
    String test = Serial.readStringUntil('\n');

    commandProcess(test);
  }
  mot.loop();

  // gestion wifi
  if (millis() > wifiActivation + 60000 && wifiActivation != 0)
  {
    Serial.println("desactivation wifi");
    wifiActivation = 0;
    WiFi.mode(WIFI_OFF);
    server.end();
  }

  // gestion deep sleep
  if (timeDeepSleeping != 0)
  {
    unsigned long _millis = millis();

    while (millis() < _millis + 5000)
    {
      Ec.getDisplay()->clearDisplay();
      Ec.getDisplay()->setCursor(0, 0);
      Ec.getDisplay()->println("ça va etre tout noir !");
      Ec.getDisplay()->printf("%i \n", (_millis + 5000) - millis());
      Ec.getDisplay()->display();
      delay(100);
    }
    Ec.setSleep();
    double positionMoteur;
    positionMoteur = mot._position;
    pref.putDouble("posMot", positionMoteur);

    //WiFi.disconnect(true);
    //LoRa.getRadio().sleep(false);
    esp_sleep_enable_timer_wakeup(timeDeepSleeping);

    esp_deep_sleep_start();
  }

// gestion reboot
  if (shouldReboot != 0 && millis() > shouldReboot)
  {
    shouldReboot = 0;
    ESP.restart();
  }

 
  if (rpm > maxSpeed && rpm < 20000)
  {
    Serial.println("ERrrror : " + (String)rpm + " rpm");
    //mot.setState(MotorState::OVERSPEED);
  }

  if (millis() > messageReponse && messageReponse != 0)
  {
    messageReponse = 0;
    LoRa.sendData(MASTER, LoRaMessageCode::DataReponse, msgReponse);
  }

  if (calibrationADS)
  {
    Serial.printf("[calibration ADS] current_base %f voltage_base %f\n", current_base, voltage_base);
    calibrationADS = false;
    voltage_base = -calibrateADS(VOLTAGE_ADS_CHANNEL, 30, 10);

    current_base = -calibrateADS(CURRENT_ADS_CHANNEL);
    Serial.printf("[calibration ADS] current_base %f voltage_base %f\n", current_base, voltage_base);
  }

  switch (displayMode)
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
    // Serial.println("menuloop");
    Ec.getDisplay()->clearDisplay();
    menuParam->loop();
    Ec.getDisplay()->display();

    break;
  case displayMode_e::InitPos:
    Ec.getDisplay()->clearDisplay();
    Ec.getDisplay()->println("Init Moteur");
    Ec.getDisplay()->display();
    break;
  default:
    break;
  }
}