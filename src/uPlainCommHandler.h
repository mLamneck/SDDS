#ifndef UPLAINCOMMHANDLER_H
#define UPLAINCOMMHANDLER_H

#include "uPlatform.h"
#include "uMultask.h"
#include "uTypedef.h"
#include "uStrings.h"
#include "uStream.h"

#define json_key(_key) #_key ":"


class TjsonSerializer{
  private:
    TmenuHandle* Froot;
    Tstream* Fstream;
  public:
    TjsonSerializer(TmenuHandle* _root, Tstream* _stream){
      Froot = _root;
      Fstream = _stream;
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
          else if (d->isEnum()){
            //provide as value???
            Fstream->write('"');
            Fstream->write(d->to_string().c_str());
            Fstream->write('"');
            Fstream->write(',');
            Fstream->write(json_key("enums"));
            Fstream->write('[');
            TenumBase* en = static_cast<TenumBase*>(d);
            for (int i=0; i<en->enumCnt(); i++){
                Fstream->write('"');
                Fstream->write(en->getEnum(i));
                Fstream->write('"');
                Fstream->write(',');
            }
            Fstream->write(']');
          }
          else{
            Fstream->write(d->to_string().c_str()); //toDo... strings?
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

class _TplainCommHandler : public Tthread{
  typedef dtypes::int8 Tcmd;

  private:
    TmenuHandle* Froot;
    Tstream* Fstream;
  public:
    _TplainCommHandler(TmenuHandle& _root, Tstream* _stream){
      Froot = &_root;
      Fstream = _stream;
    }
    _TplainCommHandler(TmenuHandle& _root, Tstream& _stream){
      Froot = &_root;
      Fstream = &_stream;
    }

    void send(const char* _msg){
        Fstream->write(_msg);
        Fstream->flush();
    }

    void startSendTypes(){
        TjsonSerializer s(Froot,Fstream);
        s.serialize();
        Fstream->flush();
        //debug::log(Fstream1.Fbuffer.c_str());
    }

    void linkPath(TstringRef& _msg){
        //port not specified???
        char port = _msg.next();
        char blank = _msg.next();
        if (port == '\0' || blank != ' '){
            send("E L");
            return;
        }

        TmenuHandle* mh = Froot;

        //locate path necessary???
        if (_msg.hasNext()){
            //path not found???
            Tlocator l(Froot);
            if (!l.locate(_msg)){
                send("E L");
                return;
            }

            //path doesn't point to a struct???
            if (!l.result()->isStruct()){
                send("E L");
                return;
            }

            //structs = nullptr????
            mh = static_cast<Tstruct*>(l.result())->value();
            if (!mh){
                send("E L");
                return;
            }
        }

        //struct already linked by this thread???
        for (auto it = mh->events()->iterator(); it.hasNext(); ){
            auto oe = it.next();
            if (oe->Fstruct == mh && oe->event()->owner() == this){
                send("E L");
                return;
            }
        }
        TobjectEvent* oe = new TobjectEvent(this,"hello");
        oe->Fstruct = mh;
        oe->Ftag = port;
        mh->events()->push_first(oe);
        oe->signal();
    }

    void handleCommand(Tcmd _cmd, TstringRef& _msg){
      switch (_cmd)
      {
      case 'T':
        startSendTypes();
        break;

      case 'L':
        linkPath(_msg);
        break;

      default:
        send("E C");
        break;
      }

    }

    void handleReadWrite(TstringRef& msg){
      Tlocator l(Froot);
      if (l.locate(msg)){
          if (msg.hasNext()){
              l.result()->setValue(msg.pCurr());
          }
          else{
            debug::log("%s = %s",l.result()->name(),l.result()->to_string().c_str());
            Tdescr* var = l.result();
            Fstream->write(var->name());
            Fstream->write('=');
            Fstream->write(var->to_string());
            Fstream->flush();
          }
      };
    }

    void handleMessage(const char* _msg){
      TstringRef msg(_msg);

      char cmd = msg.next();
      char blank = msg.next();
      if (blank == ' '){
        handleCommand(cmd,msg);
      }
      else{
        msg.offset(-2);
        handleReadWrite(msg);
      }
    }

    void execute(Tevent* _ev) override{

        if (!isTaskEvent(_ev)){
            TobjectEvent* oe= static_cast<TobjectEvent*>(_ev->context());
            auto first = oe->first();
            auto last = oe->last();

            Fstream->write("l ");
            Fstream->write(oe->Ftag);
            Fstream->write(" ");
            Fstream->write(first);
            Fstream->write(" ");
            auto it = oe->Fstruct->iterator(first);
            while (first++ <= last && it.hasNext()){
                Tdescr* d = it.next();
                Fstream->write(d->to_string());
                Fstream->write(',');
            }
            Fstream->flush();
        }

    }

};

template <class InterfaceType, typename StringClass=dtypes::string>
class TplainCommHandlerTemplate : public Tthread{
  private:
    InterfaceType* Finterface;
    TmenuHandle* Froot;
    StringClass Fbuffer;
  public:
    TplainCommHandlerTemplate(InterfaceType& _stream, TmenuHandle& _root){
      Finterface = &_stream;
      Froot = &_root;
    }

    void handleMessage(const char* _msg){
      TstringRef msg(_msg);
      Tlocator l(Froot);
      if (l.locate(msg)){
          if (msg.hasNext()){
              l.result()->setValue(msg.pCurr());
          }
          else{
              Finterface->print(l.result()->name());
              Finterface->print(" = ");
              Finterface->println(l.result()->to_string().c_str());
          }
      };
    }

    void run(){
      while (Finterface->available()>0){
        char c = Finterface->read();
        if (c == '\n'){
          handleMessage(Fbuffer.c_str());
          Fbuffer = "";
        } else Fbuffer+=c;
      };
    }
};

#endif
