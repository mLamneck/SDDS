//set to 1 on ESP boards if you want to use the webSpike
//but make sure to have SDDS_ESP_EXTENSION library installed from here https://github.com/mLamneck/SDDS_ESP_Extension
#define USE_WEB_SPIKE 0
#define WLAN_SSID "ENTER_YOUR_SSID_HERE"
#define WLAN_PW "ENTER_YOUR_PASSWORD_HERE"
#define HOSTNAME "myEspWithSDDS"
#define WLAN_AP 0   //to be set if you want to use it as access point instead of connecting to your wifi

#include "uTypedef.h"
#include "uMultask.h"

ENUM(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;	//this is added
    public:
        sdds_struct(
            sdds_var(TonOffState,ledSwitch);
            sdds_var(TonOffState,blinkSwitch)
            sdds_var(Tuint16,onTime,sdds::opt::saveval,500);
            sdds_var(Tuint16,offTime,sdds::opt::saveval,500);
        )
        Tled(){
            pinMode(LED_BUILTIN, OUTPUT);

            on(ledSwitch){
                if (ledSwitch == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,1);
                else digitalWrite(LED_BUILTIN,0);
            };

            // code from here is new
            on(blinkSwitch){
                if (blinkSwitch == TonOffState::dtype::ON) timer.start(0);
                else timer.stop();
            };

            on(timer){
                if (ledSwitch == TonOffState::dtype::ON){
                    ledSwitch = TonOffState::dtype::OFF;
                    timer.start(offTime);
                } 
                else {
                    ledSwitch = TonOffState::dtype::ON;
                    timer.start(onTime);
                }
            };
        }
};

class TuserStruct : public TmenuHandle{
    public:
    sdds_struct(
        sdds_var(Tled,led);
    )
    public:
        TuserStruct(){
        }
} userStruct;

//make available through serial communication
#include "uSerialSpike.h"
TserialSpike serialHandler(userStruct,115200);

#if USE_WEB_SPIKE == 1
//make it available for Websockets
  #include "uWebSpike.h"
  TwebSpike webSpike(userStruct);
#endif

void setup(){
  #if USE_WEB_SPIKE == 1
    WiFi.setHostname(HOSTNAME);
    #if WLAN_AP == 0
      WiFi.begin(WLAN_SSID, WLAN_PW);
      while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
      }
      // Print ESP Local IP Address
      Serial.println(WiFi.localIP());
    #else
      WiFi.mode(WIFI_AP);
      WiFi.softAP(WLAN_SSID, WLAN_PW);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
    #endif
  #endif
}

void loop(){
  TtaskHandler::handleEvents();
}
