#include <Arduino.h>
//#include "pinsDef.h"
#include "configGeneral.h"
#include "configuration.h"

#include "Adafruit_INA260.h"
#include "Adafruit_INA219.h"
#include "Preferences.h"


#include "digitalInput.h"
#include "AnalogInput.h"

#include "LoRa.h"
#include "Ecran.h"
#include "Encoder.h"
#include "Moteur.h"
#include "transmission.h"
#include "Tachymetre.h"
#include "main.h"

#define LED_DEBUG true

unsigned long receptionMessage = 0;
unsigned long dernierMessage = 0;
float msgRSSI = 0;
float msgSNR = 0;

Ecran Ec = Ecran();
Moteur mot;

Preferences pref;

Tachymetre tachy;

int displayNum = 0;

#if defined(LED_BUILTIN)
  #undef LED_BUILTIN
  #define LED_BUILTIN 35
#endif



//Fin de course Fermee
digitalInput FCF(PIN_FC_F,INPUT_PULLUP);
//Fin de course Ouvert
digitalInput FCO(PIN_FC_O,INPUT_PULLUP);

digitalInput btnPRG(0,INPUT_PULLUP);

AnalogInput VoltageOutput(PIN_VOLTAGE_OUTPUT,1,0);
AnalogInput CurrentOutput(PIN_CURRENT_OUTPUT,(0.0245f),-2048);

Adafruit_INA260 ina260 = Adafruit_INA260(); 
Adafruit_INA219 ina219 = Adafruit_INA219(); 

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
  toSend += "target:" + (String) mot.getTarget() + ",";
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
  
  
  #if defined(LED_DEBUG) && LED_DEBUG == true
    digitalWrite(LED_BUILTIN,HIGH);
  #endif

  commandProcess(msg);

  //TODO lorsque je recois un message je constate que le mcu perd ces infos
  pinMode(PIN_FC_O,INPUT_PULLUP);
  pinMode(PIN_FC_F,INPUT_PULLUP);
}

void commandProcess(String cmd){
  if (cmd.startsWith("TargetVanne="))
  {
    cmd.replace("TargetVanne=","");
    mot.setTarget(transmission::ratioToTarget((cmd.toInt() /100.0),mot));
  }
  if (cmd.startsWith("ZV")) //set Zero to Voltage mesure
  {
    VoltageOutput.setZero();
  }
  if (cmd.startsWith("ZC")) //set Zero to Current mesure
  {
    VoltageOutput.setZero();
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

  return true;
}

void acquisitionEntree(){
  FCF.loop();
  FCO.loop();
  btnPRG.loop();
  mot.updateIntensiteMoteur(ina260.readCurrent());
  VoltageOutput.loop();
  CurrentOutput.loop();
  VoltageBattery = ina260.readBusVoltage();
  
}
// put your setup code here, to run once:
void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_OLED,SCL_OLED);
  
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

  Serial.println("CPU  Freq: " + (String)getCpuFrequencyMhz());
  Serial.println("XTAL Freq: " + (String)getXtalFrequencyMhz());
  //setCpuFrequencyMhz(80);
  Serial.println("CPU Freq: " + (String)getCpuFrequencyMhz());
  delay(1000);
  
}

// put your main code here, to run repeatedly:
void loop() {
  LoRa.loop();
  Ec.loop();
  acquisitionEntree();

  if (btnPRG.frontDesceandant())
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

  displayData();
}