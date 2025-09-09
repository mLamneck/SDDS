#ifndef USERIALSPIKE_H
#define USERIALSPIKE_H

#include "uPlainCommHandler.h"

class TserialStream : public Tstream{
	dtypes::uint32 Fbaud;
	void init() override { Serial.begin(Fbaud); }
	void write(const char* _str) override { Serial.print(_str); }
	void write(char _char) override { Serial.print(_char); }
	//void write(int _int) override { Serial.print(_int); }
	void flush() override { Serial.println(""); }
	public:
		TserialStream(dtypes::uint32 _baud){
			Fbaud = _baud;
		}
};

void* __TserialSpikeInstance = nullptr;

class TserialSpike{
	String Fbuffer;
	TplainCommHandler FcommHandler;
	TserialStream Fstream;
	public:
		TserialSpike(TmenuHandle& _root, dtypes::uint32 _baudrate = 115200):
			FcommHandler(_root,Fstream)
		,Fstream(_baudrate)
		{
			__TserialSpikeInstance = this;
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
	if (__TserialSpikeInstance){
		static_cast<TserialSpike*>(__TserialSpikeInstance)->read();
	}
}


#endif
