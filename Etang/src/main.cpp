#include <main.h>
#include <Arduino.h>
#include "configGeneral.h"
#include "configuration.h"

#include <Preferences.h>

#include <Adafruit_VL53L1X.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <digitalInput.h>
#include <Adafruit_BMP280.h>

#include "Ecran.h"
#include "LoRa.h"

Preferences pref;

Ecran Ec(&Wire);
Adafruit_INA219 ina219;

Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(12, 13);
const int ledvl53 = 13;
bool ledvl53state = false;

float consoCourrant = 0;
// #define RADIOLIB_DEBUG

bool ledNotif = false;

unsigned long ledReceptionMessage = 0;
String message = "Default";

Adafruit_SSD1306 *display;

int niveauEtangVide = 0;
int niveauEtangRempli = 0;
int niveauEtangTropPlein = 0;

int16_t NiveauEtang = 0;

int numDisplay = 0;
int maxDisplay = 2;

String msgReponse = "";
unsigned long tMsgReponse = 0;

unsigned long rebootTime = 0;

digitalInput btnPRG(0, INPUT_PULLUP);

Adafruit_BMP280 bmp;

String LoRaMesageStatut()
{
  Serial.println("DemandeStatut");
  String retour = "";

  retour += "NE:" + (String)NiveauEtang + ",";
  retour += "NEVide:" + (String)niveauEtangVide + ",";
  retour += "NERempli:" + (String)niveauEtangRempli + ",";
  retour += "NETropPlein:" + (String)niveauEtangTropPlein + ",";
  retour += "ratio:" + (String)ratioEtang() + ",";
  retour += "consoCourant:" + (String)consoCourrant + ",";
  retour += "status:" + (String)vl53.vl_status + ",";
  retour += "timingBudget:" + (String)vl53.getTimingBudget() + ",";
  uint8_t roiCenter;
  vl53.VL53L1X_GetROICenter(&roiCenter);
  retour += "RoiC:" + (String)roiCenter + ",";
  uint16_t roix;
  uint16_t roiy;
  vl53.VL53L1X_GetROI_XY(&roix, &roiy);
  retour += "RoiXY:" + (String)roix + "|" + (String)roiy + ",";
  uint16_t distanceMode;
  vl53.VL53L1X_GetDistanceMode(&distanceMode);
  retour += "distanceMode:" + (String)distanceMode +",";

  return retour;
}

void executeCmd(String msg)
{
  if (msg.startsWith("Reboot"))
  {
    rebootTime = millis() + 500;
    msgReponse = "Redemarrage en cours !";
  }

  if (msg.startsWith("setNiveauFull"))
  {
    msg.replace("setNiveauFull", "");
    if (msg.startsWith("="))
    {
      msg.replace("=", "");
      niveauEtangRempli = msg.toInt();
    }
    else
    {
      niveauEtangRempli = NiveauEtang;
    }
    msgReponse += "setNiveauFull " + (String)niveauEtangRempli;
  }
  if (msg.startsWith("setNiveauEmpty"))
  {
    msg.replace("setNiveauEmpty", "");
    if (msg.startsWith("="))
    {
      msg.replace("=", "");
      niveauEtangVide = msg.toInt();
    }
    else
    {
      niveauEtangVide = NiveauEtang;
    }
    msgReponse += "setNiveauEmpty " + (String)niveauEtangVide;
  }
  if (msg.startsWith("setNiveauTP"))
  {
    msg.replace("setNiveauTP", "");
    if (msg.startsWith("="))
    {
      msg.replace("=", "");
      niveauEtangTropPlein = msg.toInt();
    }
    else
    {
      niveauEtangTropPlein = NiveauEtang;
    }
    msgReponse += "setNiveauTP " + (String)niveauEtangTropPlein;
  }

  if (msg.startsWith("CpuFreq"))
  {
    msg.replace("CpuFreq", "");
    if (msg.startsWith("="))
    {
      msg.replace("=", "");
      setCpuFrequencyMhz(msg.toInt());
      Serial.printf("Changement CpuFreq : %i MHz \n", msg.toInt());
    }
    msgReponse += "CpuFreq " + (String)getCpuFrequencyMhz();
  }

  if (msg.startsWith("TimingBudget="))
  {
    msg.replace("TimingBudget=", "");
    Serial.println(vl53.setTimingBudget(msg.toInt()));
    msgReponse += "TimingBudget " + (String)vl53.getTimingBudget();
  }
  if (msg.startsWith("DistanceMode"))
  {
    int retour;
    msg.replace("DistanceMode","");
    if (msg.startsWith("="))
    {
      msg.replace("=","");
      retour = vl53.VL53L1X_SetDistanceMode(msg.toInt());
      msgReponse = "Distance Mode erreur :" + (String)retour;

    } else
    {
      uint16_t dm;
      VL53L1X_ERROR err;
      err = vl53.VL53L1X_GetDistanceMode(&dm);
      msgReponse = "Distance Mode :" + (String)dm + " Erreur: " + (String)err;
     
    }
    
  }

  if (msg.startsWith("LedVL53"))
  {
    msg.replace("LedVL53", "");
    if (msg.startsWith("="))
    {
      msg.replace("=", "");
      ledvl53state = msg.toInt();
    }
    else
    {
      ledvl53state = !ledvl53state;
    }

    digitalWrite(ledvl53, ledvl53state);
    msgReponse += "LedVL53 " + (String)ledvl53state;
  }

  if (msg.startsWith("ledNotif"))
  {
    ledNotif = !ledNotif;
  }
  if (msg.startsWith("savePref"))
  {
    SauvegardePref();
    msgReponse += "savePref Ok";
  }
  if (msg.startsWith("RoiC="))
  {
    msg.replace("RoiC=", "");
    uint8_t roic = msg.toInt();
    Serial.println(vl53.VL53L1X_GetROICenter(&roic));
  }
  if (msg.startsWith("RoiX"))
  {
    msg.replace("RoiX=", "");
    uint16_t roiX = msg.toInt();
    Serial.println(vl53.VL53L1X_SetROI(roiX, roiX));
    vl53.VL53L1X_GetROI_XY(&roiX, &roiX);
    Serial.println("nouveau RoiX: " + (String)roiX);
  }
}

void LoRaMessage(LoRaPacket header, String msg)
{
  ledReceptionMessage = millis();
  message = msg;

  if (ledNotif)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  Serial.println("RSSI : " + (String)header.RSSI);

  msgReponse = "";
  executeCmd(msg);

  switch (header.Code)
  {
  case LoRaMessageCode::Data:
    tMsgReponse = millis() + 100;

    break;

  default:
    break;
  }
}

void acquisition()
{
  btnPRG.loop();

  consoCourrant = ina219.getCurrent_mA();
  // Serial.printf("Temp %f °C\n",bmp.readTemperature());
}

void scanI2C()
{
  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++)
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    // Wire1.beginTransmission(address);
    // error = Wire1.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
}

void loadPref()
{
  niveauEtangRempli = pref.getInt("levelRempli", niveauEtangRempli);
  niveauEtangVide = pref.getInt("levelVide", niveauEtangVide);
  niveauEtangTropPlein = pref.getInt("levelTp", niveauEtangTropPlein);
  ledNotif = pref.getBool("ledNotif");

  ledvl53state = pref.getBool("LedVL53", false);

  Serial.println("load pref !");
}

void SauvegardePref()
{
  pref.putInt("levelRempli", niveauEtangRempli);
  pref.putInt("levelVide", niveauEtangVide);
  pref.putInt("levelTp", niveauEtangTropPlein);
  pref.putBool("ledNotif", ledNotif);

  pref.putUInt("timingBudget", vl53.getTimingBudget());
  uint16_t distanceMode;
  vl53.VL53L1X_GetDistanceMode(&distanceMode);
  pref.putInt("distanceMode", distanceMode);
  Serial.println("save Prek ok");

  pref.putBool("LedVL53", ledvl53state);
}

float ratioEtang()
{
  return (float(niveauEtangVide - NiveauEtang)) / (float(niveauEtangVide - niveauEtangRempli));
}

void displayNiveauEtang()
{
  Ec.getDisplay()->drawRect(114, 10, 14, 54, SSD1306_WHITE);
  int16_t posy = 14 + (1 - ratioEtang()) * 54;
  Ec.getDisplay()->drawLine(114, posy, 124, posy, SSD1306_WHITE);
  Ec.getDisplay()->setCursor(50, 10);
  Ec.getDisplay()->println("R : " + (String)niveauEtangRempli);
  Ec.getDisplay()->setCursor(50, 22);
  Ec.getDisplay()->println("TP: " + (String)niveauEtangTropPlein);
  Ec.getDisplay()->setCursor(50, 50);
  Ec.getDisplay()->println("V: " + (String)niveauEtangVide);
  Ec.getDisplay()->setCursor(10, 35);
  Ec.getDisplay()->println("%: " + (String)(ratioEtang() * 100));
}
void displayData(void)
{

  Ec.getDisplay()->clearDisplay();

  switch (numDisplay)
  {
  case 0:
    Ec.getDisplay()->setCursor(0, 0);

    Ec.getDisplay()->println("Niveau: " + (String)NiveauEtang + " mm");
    Ec.getDisplay()->println("Rempli: " + (String)niveauEtangRempli + " mm");
    Ec.getDisplay()->println("Vide  : " + (String)niveauEtangVide + " mm");
    Ec.getDisplay()->println("TropPlein: " + (String)niveauEtangTropPlein + " mm");

    Ec.getDisplay()->setCursor(0, 36);
    Ec.getDisplay()->println(message);
    Ec.getDisplay()->setCursor(0, 48);
    Ec.getDisplay()->print(consoCourrant);
    Ec.getDisplay()->println(" mA");

    break;
  case 1:
    displayNiveauEtang();

    break;
  default:
    break;
  }
  Ec.getDisplay()->display();
}

void setup()
{
  // put your setup code here, to run once:

  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Wire.begin(SDA_OLED, SCL_OLED);
  delay(2000);
  Serial.println("debut setup);");
  pinMode(LED_BUILTIN, OUTPUT);

  if (!Ec.begin())
  {
    Serial.println("Failed to Init preference");
    while (true)
      delay(10);
  }

  Ec.getDisplay()->setCursor(0, 12);
  // scanI2C();

  if (!pref.begin("Etang", false))
  {
    Serial.println("Failed to Init preference");
    Ec.getDisplay()->println("failed Init Ecran ");
    Ec.getDisplay()->display();
    while (1)
      delay(10);
  }

  loadPref();

  pinMode(ledvl53, OUTPUT);
  digitalWrite(ledvl53, ledvl53state);

  Ec.getDisplay()->println("Ok Init Preference !");
  Ec.getDisplay()->display();

  pinMode(LED_BUILTIN, OUTPUT);

  delay(200);

  // Vl53L1X

  if (!vl53.begin(0x29, &Wire, true))
  {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    Ec.getDisplay()->println("failed Init VL53 ");
    Ec.getDisplay()->display();
    while (1)
      delay(10);
  }

  Serial.println(F("Ok VL53L1X sensor !"));
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (!vl53.startRanging())
  {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)
      delay(10);
  }
  Ec.getDisplay()->println("Ok Init VL53 ");
  Ec.getDisplay()->display();

  vl53.setTimingBudget(pref.getUInt("timingBudget", 100));
  vl53.VL53L1X_SetDistanceMode(pref.getUInt("distanceMode", 2));
  // Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  // vl53.setTimingBudget(100);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  delay(200);

  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("INA219 init ok");
  Ec.getDisplay()->println("Ok Init INA219 !");
  Ec.getDisplay()->display();

  unsigned status;
  // status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin(BMP280_ADDRESS_ALT);
  if (!status)
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(bmp.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    // while (1) delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

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
  delay(500);
}

void loop()
{
  // put your main code here, to run repeatedly:

  Ec.loop();
  acquisition();

  if (Serial.available() > 0)
  {
    String str = Serial.readStringUntil('\n');
    Serial.println("cmd: " + str);
    executeCmd(str);
    if (str.startsWith("V"))
    {
      str.replace("V", "");
      niveauEtangVide = str.toInt();
    }
    if (str.startsWith("R"))
    {
      str.replace("R", "");
      niveauEtangRempli = str.toInt();
    }
    if (str.startsWith("S"))
    {
      str.replace("S", "");
      SauvegardePref();
      Serial.println("pref saved");
    }
    if (str.startsWith("T"))
    {
      str.replace("T", "");
      niveauEtangTropPlein = str.toInt();
    }
    if (str.startsWith("A"))
    {
      Ec.getDisplay()->startscrollleft(0x00, 0x0f);
    }
    if (str.startsWith("I2C"))
    {
      scanI2C();
    }
  }

  if (btnPRG.frontDesceandant())
  {
    Serial.println("fd");
    if (Ec.getState() == EcranState::EcranState_Sleep)
    {
      Ec.wakeUp();
    }
    else
    {
      numDisplay = (numDisplay + 1) % maxDisplay;
      Serial.println(numDisplay);
    }
  }

  if (millis() > ledReceptionMessage + 200 && ledReceptionMessage != 0)
  {
    ledReceptionMessage = 0;
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (vl53.dataReady())
  {
    // new measurement for the taking!
    NiveauEtang = vl53.distance();
    if (NiveauEtang == -1)
    {
      // something went wrong!
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
    }
    // Serial.println("Distance: " +(String) distance);
  }

  if (tMsgReponse != 0 && millis() > tMsgReponse)
  {
    tMsgReponse = 0;
    LoRa.sendData(0x01, LoRaMessageCode::DataReponse, msgReponse);
  }

  if (rebootTime != 0 && millis() > rebootTime)
  {
    rebootTime = 0;
    ESP.restart();
  }

  displayData();
  LoRa.loop();
  delay(50);
}