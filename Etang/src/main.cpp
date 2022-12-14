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


#include "Ecran.h"
#include "LoRa.h"

Preferences pref;

Ecran Ec(&Wire);
Adafruit_INA219 ina219;
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X(12,13);

float consoCourrant = 0;
//#define RADIOLIB_DEBUG

unsigned long receptionMessage = 0;
String message = "Default";

Adafruit_SSD1306* display;

int niveauEtangVide = 0;
int niveauEtangRempli = 0;
int niveauEtangTropPlein = 0;

int16_t NiveauEtang = 0;

int numDisplay = 0;
int maxDisplay = 2;



digitalInput btnPRG(0,INPUT_PULLUP);

String LoRaMesageStatut(){
  Serial.println("DemandeStatut");
  String retour = "";

  retour += "NE:"+(String)NiveauEtang +",";
  retour += "NEVide:"+(String)niveauEtangVide +",";
  retour += "NERempli:"+(String)niveauEtangRempli +",";
  retour += "NETropPlein:"+(String)niveauEtangTropPlein +",";
  retour += "ratio:"+(String)ratioEtang() +",";
  retour += "consoCourant:"+(String)consoCourrant +",";
  
  return retour;
}

void SauvegardePref(){
  pref.putInt("levelRempli",niveauEtangRempli);
  pref.putInt("levelVide",niveauEtangVide);
  pref.putInt("levelTp",niveauEtangTropPlein);
  
}

void LoRaMessage(LoRaPacket header, String msg){
  receptionMessage = millis();
  message = msg;
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("RSSI : " + (String)header.RSSI);
  if (msg == "setNiveauFull")
  {
    niveauEtangRempli = NiveauEtang;
  }
  if (msg == "setNiveauEmpty")
  {
    niveauEtangVide = NiveauEtang;
  }
  if (msg == "setNiveauTP")
  {
    niveauEtangTropPlein = NiveauEtang;
  }
  if (msg == "savePref")
  {
    SauvegardePref();
  }
  
}

void acquisition(){
  btnPRG.loop();

  consoCourrant = ina219.getCurrent_mA();
}

void scanI2C(){
  byte error, address;
	int nDevices;

	Serial.println("Scanning...");

	nDevices = 0;
	for(address = 1; address < 127; address++ )
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();

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


void loadPref(){
  niveauEtangRempli = pref.getInt("levelRempli",niveauEtangRempli);
  niveauEtangVide = pref.getInt("levelVide",niveauEtangVide);
  niveauEtangTropPlein = pref.getInt("levelTp", niveauEtangTropPlein);
}
float ratioEtang(){
  return (float(niveauEtangVide - NiveauEtang)) / (float(niveauEtangVide - niveauEtangRempli));
}
void displayData(void){

  Ec.getDisplay()->clearDisplay();

  switch (numDisplay)
  {
  case 0:
    Ec.getDisplay()->setCursor(0,0);

    Ec.getDisplay()->println("Niveau: "+(String)NiveauEtang +" mm");
    Ec.getDisplay()->println("Rempli: "+(String)niveauEtangRempli +" mm");
    Ec.getDisplay()->println("Vide  : "+(String)niveauEtangVide +" mm");
    Ec.getDisplay()->println("TropPlein: "+(String)niveauEtangTropPlein +" mm");
    


    Ec.getDisplay()->setCursor(0,36);
    Ec.getDisplay()->println(message);
    Ec.getDisplay()->setCursor(0,48);
    Ec.getDisplay()->print(consoCourrant);
    Ec.getDisplay()->println(" mA");

    
    break;
  case 1:
    Ec.getDisplay()->drawRect(114,10,14,54,SSD1306_WHITE);
    Ec.getDisplay()->setCursor(50,10);
    Ec.getDisplay()->println("R : " + (String)niveauEtangRempli);
    Ec.getDisplay()->setCursor(50,22);
    Ec.getDisplay()->println("TP: " + (String)niveauEtangTropPlein);
    Ec.getDisplay()->setCursor(50,50);
    Ec.getDisplay()->println("TP: " + (String)niveauEtangVide);
    Ec.getDisplay()->setCursor(10,30);
    Ec.getDisplay()->println("%: " + (String)ratioEtang());
    break;
  default:
    break;
  }
  Ec.getDisplay()->display();
}

void setup() {
  // put your setup code here, to run once:

  setCpuFrequencyMhz(80);

  Serial.begin(115200);
  Wire.begin(SDA_OLED,SCL_OLED);
  delay(2000);
  Serial.println("debut setup);");
  pinMode(LED_BUILTIN,OUTPUT);

  if (!Ec.begin())
  {
    Serial.println("Failed to Init preference");
    while (true) delay(10);
  }

  Ec.getDisplay()->setCursor(0,12);
  //scanI2C();

  if (!pref.begin("Etang",false))
  {
    Serial.println("Failed to Init preference");
    Ec.getDisplay()->println("failed Init Ecran ");
    Ec.getDisplay()->display();
    while (1)       delay(10);
  }
  
  loadPref();
  
  Ec.getDisplay()->println("Ok Init Preference !");
  Ec.getDisplay()->display();

  pinMode(LED_BUILTIN,OUTPUT);

  
  delay(200);

  //Vl53L1X
	
  if (! vl53.begin(0x29, &Wire,true)) {
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    Ec.getDisplay()->println("failed Init VL53 ");
    Ec.getDisplay()->display();
    while (1)       delay(10);
  }
  Serial.println(F("Ok VL53L1X sensor !"));
  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (! vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1)       delay(10);
  }
  Ec.getDisplay()->println("Ok Init VL53 ");
  Ec.getDisplay()->display();
  

// Valid timing budgets: 15, 20, 33, 50, 100, 200 and 500ms!
  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());

  
  
  delay(200);

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  Serial.println("INA219 init ok");
  Ec.getDisplay()->println("Ok Init INA219 !");
  Ec.getDisplay()->display();

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


void loop() {
  // put your main code here, to run repeatedly:
  
  Ec.loop();
  acquisition();

  if (Serial.available()>0)
  {
    String str = Serial.readStringUntil('\n');
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
      Ec.getDisplay()->startscrollleft(0x00,0x0f);
    }
    
  }
  

  
  
  if (btnPRG.frontDesceandant())
  {
    Serial.println("fd");
    if (Ec.getState() == EcranState::EcranState_Sleep)
    {
      Ec.wakeUp();
    } else
    {
      numDisplay = (numDisplay+1) % maxDisplay;
      Serial.println(numDisplay);
    }
    
  
    
  }
  
  if (millis()> receptionMessage + 200 && receptionMessage != 0)
  {
    receptionMessage = 0;
    digitalWrite(LED_BUILTIN,LOW);
  }
  
  
  

  if (vl53.dataReady()) {
    // new measurement for the taking!
    NiveauEtang = vl53.distance();
    if (NiveauEtang == -1) {
      // something went wrong!
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
      
    }
    //Serial.println("Distance: " +(String) distance);
  }

  displayData();
  LoRa.loop();
  delay(50);
}