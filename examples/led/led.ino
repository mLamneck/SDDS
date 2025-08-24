#include "uTypedef.h"
#include "uMultask.h"
#include "uParamSave.h"

sdds_enum(OFF,ON) TonOffState;

class Tled : public TmenuHandle{
    Ttimer timer;
    public:
      sdds_struct(
          sdds_var(TonOffState,ledSwitch,sdds::opt::saveval)
          sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval)
          sdds_var(Tuint16,onTime,sdds::opt::saveval,500)
          sdds_var(Tuint16,offTime,sdds::opt::saveval,500)
      )
      Tled(){
          pinMode(LED_BUILTIN, OUTPUT);

          on(ledSwitch){
              if (ledSwitch == TonOffState::ON) digitalWrite(LED_BUILTIN,0);
              else digitalWrite(LED_BUILTIN,1);
          };

          on(blinkSwitch){
              if (blinkSwitch == TonOffState::ON) timer.start(0);
              else timer.stop();
          };

          on(timer){
              if (ledSwitch == TonOffState::ON){
                  ledSwitch = TonOffState::OFF;
                  timer.start(offTime);
              } 
              else {
                  ledSwitch = TonOffState::ON;
                  timer.start(onTime);
              }
          };
      }
};

class TuserStruct : public TmenuHandle{
  public:
    sdds_struct(
        sdds_var(Tled,led)
        sdds_var(TparamSaveMenu,params)
    )
    TuserStruct(){
        //you application code goes here... 
    }
} userStruct;

#include "uSerialSpike.h"
TserialSpike serialHandler(userStruct,115200);

void setup(){

}

void loop(){
  TtaskHandler::handleEvents();
}