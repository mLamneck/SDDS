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
          #define ref(var) sdds_ref(Tled)->var

          pinMode(LED_BUILTIN, OUTPUT);

          on(ledSwitch){
              if (ref(ledSwitch) == TonOffState::dtype::ON) digitalWrite(LED_BUILTIN,0);
              else digitalWrite(LED_BUILTIN,1);
          };

          on(blinkSwitch){
              if (ref(blinkSwitch) == TonOffState::dtype::ON) ref(timer).start(0);
              else ref(timer).stop();
          };

          on(timer){
              if (ref(ledSwitch) == TonOffState::dtype::ON){
                  ref(ledSwitch) = TonOffState::dtype::OFF;
                  ref(timer).start(ref(offTime));
              } 
              else {
                  ref(ledSwitch) = TonOffState::dtype::ON;
                  ref(timer).start(ref(onTime));
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