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

    void serialize(Tdescr* _d){
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
      if (!FwithValue && !_d->isStruct()) return;
      Fstream->write(',');
      Fstream->write(json_key("value"));
    }

    void serialize(TmenuHandle* _curr){
      Fstream->write('[');
      auto it = _curr->iterator();
      while (it.hasNext()){
        Tdescr* d = it.next();

        Fstream->write('{');
        serialize(d);
        if (d->isStruct()){
          TmenuHandle* mh = static_cast<Tstruct*>(d)->value();
          if (mh) serialize(mh);
          else Fstream->write("null");
        }
        else if (FwithValue){
          if (d->isEnum()){
            //provide as value???
            Fstream->write('"');
            Fstream->write(d->to_string().c_str());
            Fstream->write('"');
            Fstream->write(',');
            Fstream->write(json_key("enums"));
            Fstream->write('[');
            TenumBase* en = static_cast<TenumBase*>(d);
            auto enumCnt = en->enumCnt();
            for (auto i=0; i<enumCnt; i++){
                Fstream->write('"');
                Fstream->write(en->getEnum(i));
                Fstream->write('"');
                if (i+1 >= enumCnt) break;
                Fstream->write(',');
            }
            Fstream->write(']');
          }
          else{
            bool quotes = d->needQuotes();
            if (quotes) Fstream->write('"');
            Fstream->write(d->to_string().c_str()); //toDo... strings?
            if (quotes) Fstream->write('"');
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