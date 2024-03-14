#ifndef UJSONSERIALIZER_H
#define UJSONSERIALIZER_H

#include "uTypedef.h"
#include "uStream.h"

class TjsonSerializer{
  private:
    TmenuHandle* Froot;
    Tstream* Fstream;
    bool FwithValue;
  public:
    TjsonSerializer(TmenuHandle* _root, Tstream* _stream, bool _withValue=true);

    static void encodeString(Tstream* _stream, const char* _str);
    
    static void serializeAsString(Tstream* _stream, const char* _str);

    static void serializeAsString(Tstream* _stream, Tdescr* _d);

    void serializeAsString(Tdescr* _d);
    
    static void serializeValues(Tstream* _stream, TmenuHandle* _struct, TrangeItem _first=0, TrangeItem _last=TrangeItem_max);

    void serializeTypeDescr(Tdescr* _d);
    
    void serialize(TmenuHandle* _curr);

    void serialize(){ serialize(Froot); }
};

class TjsonStringSerializer : public TjsonSerializer{
  TstringStream FstringStream;
  public:
    TstringStream stream() { return FstringStream; }
    const char* c_str() { return FstringStream.data(); }
    TjsonStringSerializer(TmenuHandle* _root, bool _withValue=true) 
      : TjsonSerializer(_root,&FstringStream,_withValue)
    {

    }
};

#endif