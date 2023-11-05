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


#endif