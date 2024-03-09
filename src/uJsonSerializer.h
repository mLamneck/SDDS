#ifndef UJSONSERIALIZER_H
#define UJSONSERIALIZER_H

#include "uTypedef.h"
#include "uStream.h"

#define json_key(_key) #_key ":"

class TjsonSerializer{
  private:
    TmenuHandle* Froot;
    Tstream* Fstream;
    bool FwithValue;
  public:
    TjsonSerializer(TmenuHandle* _root, Tstream* _stream, bool _withValue=true){
      Froot = _root;
      Fstream = _stream;
      FwithValue = _withValue;
    }

    static void encodeString(Tstream* _stream, const char* _str){
      while (*_str != '\0'){
        switch(*_str){
          case '"': case '\\': case '/':
          case '\b': case '\f': case '\n':
          case '\r': case '\t':
            _stream->write('\\');
            break;
        }
        _stream->write(*_str++);
      }
    }

    static void serializeAsString(Tstream* _stream, const char* _str){
      _stream->write('"');
      TjsonSerializer::encodeString(_stream,_str);
      _stream->write('"');
    }

    static void serializeAsString(Tstream* _stream, Tdescr* _d){
      TjsonSerializer::serializeAsString(_stream,_d->to_string().c_str());
    }

    void serializeAsString(Tdescr* _d){
      TjsonSerializer::serializeAsString(Fstream,_d);
    }

    static void serializeValues(Tstream* _stream, TmenuHandle* _struct, TrangeItem _first, TrangeItem _last){
        _stream->write("[");
        auto it = _struct->iterator(_first);
        if (it.hasNext()){
          do{
            Tdescr* d = it.next();
            switch(d->type()){
              case(sdds::Ttype::STRING): case(sdds::Ttype::TIME):
                TjsonSerializer::serializeAsString(_stream,d);
                break;
              default:
                _stream->write(d->to_string().c_str());
                break;
            }
            if (!it.hasNext() || (_first++ >= _last)) break;
            _stream->write(',');
          } while(true);
        }
        _stream->write("]");
    }

    void serializeTypeDescr(Tdescr* _d){
      Fstream->write(json_key("type"));
      Fstream->write(_d->typeId());
      Fstream->write(',');
      Fstream->write(json_key("opt"));
      Fstream->write(_d->option());
      Fstream->write(',');
      Fstream->write(json_key("name"));
      Fstream->write('"');
      Fstream->write(_d->name());
      Fstream->write('"');

      //for enums, write list of enums
      if (_d->type() == sdds::Ttype::ENUM){
          Fstream->write(',');
          Fstream->write(json_key("enums"));
          Fstream->write('[');
          TenumBase* en = static_cast<TenumBase*>(_d);
          auto enumCnt = en->enumCnt();
          for (auto i=0; i<enumCnt; i++){
            TjsonSerializer::serializeAsString(Fstream,en->getEnum(i));
            if (i+1 >= enumCnt) break;
            Fstream->write(',');
          }
          Fstream->write(']');
      }
    }

    void serialize(TmenuHandle* _curr){
      Fstream->write('[');
      auto it = _curr->iterator();
      while (it.hasNext()){
        Tdescr* d = it.next();

        Fstream->write('{');
        serializeTypeDescr(d);
        if (d->isStruct()){
          Fstream->write(',');
          Fstream->write(json_key("value"));
          TmenuHandle* mh = static_cast<Tstruct*>(d)->value();
          if (mh) serialize(mh);
          else Fstream->write("null");
        }

        else if (FwithValue){
          Fstream->write(',');
          Fstream->write(json_key("value"));

          switch(d->type()){
            case sdds::Ttype::ENUM: case sdds::Ttype::STRING: case sdds::Ttype::TIME:
              serializeAsString(d);
              break;
            default:
              Fstream->write(d->to_string().c_str());
          }
        }
        Fstream->write('}');
        if (it.hasNext()) Fstream->write(',');
      }
      Fstream->write(']');
    }

    void serialize(){
      serialize(Froot);
    }
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