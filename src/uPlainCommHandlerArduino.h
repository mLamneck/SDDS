#ifndef UPLAINCOMMHANDLERARDUINO_H
#define UPLAINCOMMHANDLERARDUINO_H

#include "uPlainCommHandler.h"

class TserialStream : public Tstream{
    void write(const char* _str) override { Serial.print(_str); }
    void write(char _char) override { Serial.print(_char); } 
    void write(int _int) override { Serial.print(_int); }
    void flush() override { Serial.println(""); }
    public:
      TserialStream(int _baud){
        Serial.begin(_baud);
      }
};

void* __TserialPlainCommHandlerInstance = nullptr;

class TserialPlainCommHandler{
  String Fbuffer;
  TplainCommHandler FcommHandler;
  TserialStream Fstream;
  public:
    TserialPlainCommHandler(TmenuHandle& _root):
      Fstream(115200)
      ,FcommHandler(_root,Fstream)
    {
		__TserialPlainCommHandlerInstance = this;
    }

    void read(){
      while (Serial.available()) {
      char inChar = (char)Serial.read();
      if (inChar == '\n') {
        FcommHandler.handleMessage(Fbuffer.c_str());
        Fbuffer="";
      }
      else{
        Fbuffer += inChar;
      }
    }
  }
};

void serialEvent(){
	Serial.print(millis());
	Serial.print(" serialEvent taskName= ");
	Serial.println(pcTaskGetName( nullptr ));

	if (__TserialPlainCommHandlerInstance){
		static_cast<TserialPlainCommHandler*>(__TserialPlainCommHandlerInstance)->read();
	}
}


#endif