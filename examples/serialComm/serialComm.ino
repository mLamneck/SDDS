#include "uTypedef.h"
#include "uPlainCommHandlerArduino.h"

ENUM(on,off) TonOffState;

class TsubStruct : public TmenuHandle{
  public:
    Ttimer timer;
  	sdds_struct(
      sdds_var(Tuint8, filter, 0, 10);      //no option, default value=10
      sdds_var(Tfloat32, value21);
      sdds_var(Tuint32, time1, 0, 1000);    
      sdds_var(TonOffState, led);
    )

    TsubStruct(){
      timer.start(1000);

      on(timer){
        timer.start(time1);
        value21 = value21 + 1.5;
        if (led == ENUMS(TonOffState)::off){
          led = ENUMS(TonOffState)::on;
        }
        else{
          led = ENUMS(TonOffState)::off;
        }
      };

      on(led){
          if (led == ENUMS(TonOffState)::off){
            digitalWrite(LED_BUILTIN,1);
          }
          else{
            digitalWrite(LED_BUILTIN,0);
          }
      };
    }
};

class TuserStruct : public TmenuHandle{
  public:
  	sdds_struct(
      sdds_var(TonOffState, cntSwitch);
      sdds_var(Tuint8, Fcnt);
      sdds_var(TsubStruct, sub);
      
      sdds_var(Tuint8, filter, 0, 10);      //no option, default value=10
      sdds_var(Tfloat32, value);
      sdds_var(Tfloat32, fValue);    
    )
    Ttimer timer;
    
    TuserStruct(){
    
      timer.start(1000);
      on(timer){
        timer.start(1000);
        if (cntSwitch == ENUMS(TonOffState)::on){
          Fcnt = Fcnt + 1;
        }
      };      
    
      on(value){
        fValue = (fValue*filter+value)/(filter+1);
      };

    }
} userStruct;

TserialStream sStream(115200);
TplainCommHandler plainComm(userStruct, sStream);

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
}

String buffer = "";
void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      plainComm.handleMessage(buffer.c_str());
      buffer="";
    }
    else{
      buffer += inChar;
    }
  }
  TtaskHandler::handleEvents();
}