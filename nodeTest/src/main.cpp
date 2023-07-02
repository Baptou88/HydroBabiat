#include <Arduino.h>

#include <RadioLib.h>
#include "Ecran.h"
#include <LoRa.h>
#include <SPIFFS.h>
#include <Adafruit_BMP085.h>
#include <wifiCredentials.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <Update.h>

struct fileDescription_t
{
    String fileName;
    int numberpacket ;
    int fileSize;
    bool OtaUpdate = false;
};

fileDescription_t fd;

enum stateReception {
  NONE,
  BEGUN,
  PROGRESS,
  ENDE
};

String stateReceptiontoString(stateReception s){
  switch (s)
  {
  case stateReception::NONE :
    return "NONE";
    break;
  case stateReception::BEGUN :
    return "BEGUN";
    break;
  case stateReception::PROGRESS :
    return "PROGRESS";
    break;
  case stateReception::ENDE :
    return "ENDE";
    break;
  default:
  return "default";
    break;
  }
}

stateReception stater = NONE;
Ecran Ec(&Wire);
String file;

int separator ;

int packetrecu = 0;
int packetNum = 0;
unsigned long reponse = 0;
float vitesseTranfert = 0;
unsigned long transfertTime = 0;
unsigned long startTransfertTime = 0;

Adafruit_BMP085 bmp;

AsyncWebServer server(80);

File fileUpload;

int previous_etat_btn = HIGH;
int etat_btn = HIGH;

bool blink = false;
unsigned long lastblink = 0;
bool blinkState = false;

String msgReponse= "";
unsigned long tMsgReponse = 0;

struct MYDATA
{
  int a = 12;
  //String b = "abcd";
};
struct MYDATA mydata ;

void print_wakeup_reason(){
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
String LoRaMesageStatut(){
  Serial.println("DemandeStatut");
  String toSend = "";
  toSend += "temp:" + (String) bmp.readTemperature() + ",";
  return toSend;
}
void displaySpiffs(){
  File root = SPIFFS.open("/");
  Ec.getDisplay()->clearDisplay();
  Ec.getDisplay()->setCursor(0,0);
  Serial.println("display Spiffs: ");
  File file = root.openNextFile();
  while (file)
  {
    Ec.getDisplay()->print((String)file.name() + " " + (String)file.size() + "\n");
    Serial.print((String)file.name() + " " + (String)file.size() + "\n");
    Ec.getDisplay()->display();
    delay(600);
    file = root.openNextFile();
  }
  

}
void LoRaMessage(LoRaPacket header, String msg)
{
  //Serial.println(msg);

  switch (header.Code )
  {
  case LoRaMessageCode::FileInit :
    stater = BEGUN;
    packetrecu = 0;
    separator = msg.indexOf(',');
    
    fd.fileName =  msg.substring(0,separator);
    fd.fileSize = msg.substring(separator+1,  msg.indexOf(',',separator+1)).toInt();
    separator = msg.indexOf(',',separator+1);
    
    fd.numberpacket = msg.substring(separator+1,  msg.indexOf(',',separator+1)).toInt();
    separator = msg.indexOf(',',separator+1);
    fd.OtaUpdate = msg.substring(separator +1).toInt();
    
    fileUpload =  SPIFFS.open(fd.fileName,FILE_WRITE,true);
    
    
    if (fd.OtaUpdate)
    {
      Update.begin();
    }
    
    
    reponse = millis();
    startTransfertTime = millis();
    break;
  case LoRaMessageCode::Data :
    msgReponse = "";
    Serial.println("data : " + (String)msg);
    if (fd.OtaUpdate)
    {
      //Update.
      Serial.println("OTA : " + (String)msg);
    }
    tMsgReponse = millis()+100;

    if (msg.startsWith("blink"))
    {
      msg.replace("blink","");
      blink = !blink;
      msgReponse += "blink:" + (String)blink;
      lastblink = millis();
      if (!blink)
      {
        digitalWrite(LED_BUILTIN,LOW);
      }
      
    }
    if (msg.startsWith("DEEPSLEEP"))
    {
      msg.replace("DEEPSLEEP","");
      tMsgReponse +=5000; // delai supplementaire pour laisser le Master s'endormir avant d'envoyer un msg pour le reveiller
      msgReponse += "OK";
    }
    if (msg.startsWith("ToggleScreen"))
    {
      msg.replace("ToggleScreen","");
      if (Ec.getState() == EcranState_IDLE)
      {
        Ec.setSleep();
      } else
      {
        Ec.wakeUp();
      }
      
      
      msgReponse += "OK";
    }
    
    break;
  
  case LoRaMessageCode::FilePacket :
    stater = PROGRESS;

    reponse = millis();
    packetNum = msg.substring(0,msg.indexOf(",")).toInt();
    msg.remove(0,msg.indexOf(",")+1);
    Serial.println("lenght " + String(msg.length()) + String(millis()-transfertTime));
    
    Serial.printf("Num %i  recu:%i \n",packetNum,packetrecu);
    if (packetNum != packetrecu)
    {
      Serial.println("packet deja recu");
      break;
    }
    packetrecu++;
    if (fd.OtaUpdate)
    {
      uint8_t *msg2;
      //msg2 = (uint8_t) msg.c_str();
      //Update.write(msg2,sizeof(msg2));
    }
    Serial.println("print size " + (String)fileUpload.print(msg));

    Serial.println("data : " + (String)msg);
    break;
  
  case LoRaMessageCode::FileEnd :
    stater = ENDE;
    Serial.println("packet ende");
    transfertTime = millis() - startTransfertTime;
    vitesseTranfert = (float) fd.fileSize / transfertTime *1000;
    Serial.println("Speed: "+ String(vitesseTranfert));
    reponse = millis();
    if (fd.OtaUpdate)
    {
      if(Update.end(true)){
        Serial.printf("Update Success: B\n" );
      } else {
        Update.printError(Serial);
      }
    }
    fileUpload.close();
    displaySpiffs();
    break;
  
  default:
    Serial.println("LoRa msg code inconnu: " + (String)header.Code);
    break;
  }
}

void scanWiFi(void) {
  int n = WiFi.scanNetworks();
  if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
        for (int i = 0; i < n; ++i) {
            // Print SSID and RSSI for each network found
            Serial.printf("%2d",i + 1);
            Serial.print(" | ");
            Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
            Serial.print(" | ");
            Serial.printf("%4d", WiFi.RSSI(i));
            Serial.print(" | ");
            Serial.printf("%2d", WiFi.channel(i));
            Serial.print(" | ");
            switch (WiFi.encryptionType(i))
            {
            case WIFI_AUTH_OPEN:
                Serial.print("open");
                break;
            case WIFI_AUTH_WEP:
                Serial.print("WEP");
                break;
            case WIFI_AUTH_WPA_PSK:
                Serial.print("WPA");
                break;
            case WIFI_AUTH_WPA2_PSK:
                Serial.print("WPA2");
                break;
            case WIFI_AUTH_WPA_WPA2_PSK:
                Serial.print("WPA+WPA2");
                break;
            case WIFI_AUTH_WPA2_ENTERPRISE:
                Serial.print("WPA2-EAP");
                break;
            case WIFI_AUTH_WPA3_PSK:
                Serial.print("WPA3");
                break;
            case WIFI_AUTH_WPA2_WPA3_PSK:
                Serial.print("WPA2+WPA3");
                break;
            case WIFI_AUTH_WAPI_PSK:
                Serial.print("WAPI");
                break;
            default:
                Serial.print("unknown");
            }
            Serial.println();
            delay(10);
        }
    }
    Serial.println("");

    // Delete the scan result to free memory for code below.
    WiFi.scanDelete();

    // Wait a bit before scanning again.
    delay(5000);

}

void setupWifiServer(void) {
  WiFi.setHostname("Esp32 V2");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
scanWiFi();
  WiFi.begin(WIFISSID,WIFIPASSWORD);
  
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Wifi Failed");
    //delay(5000);
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  server.serveStatic("/",SPIFFS,"/");
}

// perform the actual update from a given stream
void performUpdate(Stream &updateSource, size_t updateSize) {
   if (Update.begin(updateSize)) {      
      size_t written = Update.writeStream(updateSource);
      if (written == updateSize) {
         Serial.println("Written : " + String(written) + " successfully");
      }
      else {
         Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
      }
      if (Update.end()) {
         Serial.println("OTA done!");
         if (Update.isFinished()) {
            Serial.println("Update successfully completed. Rebooting.");
         }
         else {
            Serial.println("Update not finished? Something went wrong!");
         }
      }
      else {
         Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }

   }
   else
   {
      Serial.println("Not enough space to begin OTA");
   }
}

// check given FS for valid update.bin and perform update if available
void updateFromFS(fs::FS &fs) {
   File updateBin = fs.open("/update.bin");
   if (updateBin) {
      if(updateBin.isDirectory()){
         Serial.println("Error, update.bin is not a file");
         updateBin.close();
         return;
      }

      size_t updateSize = updateBin.size();

      if (updateSize > 0) {
         Serial.println("Try to start update");
         performUpdate(updateBin, updateSize);
      }
      else {
         Serial.println("Error, file is empty");
      }

      updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
      fs.remove("/update.bin");      
   }
   else {
      Serial.println("Could not load update.bin from sd root");
   }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  print_wakeup_reason();

  Serial.printf("%s %s\n",WIFISSID,WIFIPASSWORD);
  //setupWifiServer();
  Wire.begin(4,15);

  pinMode(0,INPUT_PULLUP);
  pinMode(LED_BUILTIN,OUTPUT);

  if (!bmp.begin()) {
	  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	  while (1) {}
  }

  if (!Ec.begin())
  {
    Serial.println("Ecan failed init");
    while (true)
    {
      /* code */
    }
    
  }
  
  
  Ec.getDisplay()->display();
  Ec.getDisplay()->println("esgrg");
  Ec.getDisplay()->display();


  LoRa.setNodeID(0x04);
  LoRa.onMessage(LoRaMessage);
  LoRa.onMessageStatut(LoRaMesageStatut);
  if (LoRa.begin()!= RADIOLIB_ERR_NONE)
  {
    Serial.println("Error init loRa");
    while (true)
    {
    }
    
  }
  Ec.getDisplay()->println("LoRa init Ok !");
  Ec.getDisplay()->display();
  //LoRa.sendData(0x01,LoRaMessageCode::DemandeStatut,"er");
  LoRa.getRadio().startReceive();

  if (!SPIFFS.begin(true))
  {
    Serial.println("Spiffs init failed");
  }
  
  updateFromFS(SPIFFS);

}


void loop() {
  // put your main code here, to run repeatedly:
  LoRa.loop();

  etat_btn = digitalRead(0);
  if (etat_btn == HIGH && previous_etat_btn == LOW)
  {
    Serial.println("Exec Light Sleep");
    Serial.flush();
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_26,HIGH);
    LoRa.getRadio().startReceive();
    Ec.setSleep();
    esp_light_sleep_start();
    Serial.println("Exec Light Sleep");
    Ec.wakeUp();
  }
  previous_etat_btn = etat_btn;

  if (millis() > reponse + 50 && reponse != 0)
  {
    reponse = 0;
    LoRa.sendData(0x01,LoRaMessageCode::FileAck,"ACK");
  }
  
  Ec.getDisplay()->clearDisplay();

  Ec.getDisplay()->setCursor(0,0);

  Ec.getDisplay()->println(stateReceptiontoString(stater));
  Ec.getDisplay()->println((String)packetrecu + " / " + (String)fd.numberpacket);
  Ec.getDisplay()->println("speed :  " + (String)vitesseTranfert);
  Ec.getDisplay()->println("ota :  " + (String)fd.OtaUpdate);
  Ec.getDisplay()->println("Temp :  " + (String)bmp.readTemperature());

  Ec.getDisplay()->display();

  if (blink)
  {
    if (millis() > lastblink +300)
    {
      
      lastblink = millis();
      blinkState = !blinkState;
      digitalWrite(LED_BUILTIN, blinkState);
    }
    
  }

  if (tMsgReponse !=0 && millis()>tMsgReponse)
  {
    tMsgReponse = 0;
    LoRa.sendData(0x01,LoRaMessageCode::DataReponse,msgReponse);
  }
  
  
}