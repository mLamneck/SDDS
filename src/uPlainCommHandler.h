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

class Tconnection : public TlinkedListElement{
    public:
        ~Tconnection(){
            FobjEvent.menuHandle()->events()->remove(&FobjEvent);
            FobjEvent.event()->reclaim();
        };
        TobjectEvent FobjEvent;
        Tconnection(Tthread* _owner) : FobjEvent(_owner,""){}
        TmenuHandle* menuHandle() { return FobjEvent.menuHandle(); }
};

class TplainCommHandler : public Tthread{


  typedef dtypes::int8 Tcmd;

  private:
    TmenuHandle* Froot;
    Tstream* Fstream;
    TlinkedList<Tconnection> Fconnections;

    //returned by locatePath
    TmenuHandle* Fmh;
    Tconnection* Fconn;
    dtypes::uint8 Fport;
  public:
    TplainCommHandler(TmenuHandle& _root, Tstream* _stream){
      Froot = &_root;
      Fstream = _stream;
    }
    TplainCommHandler(TmenuHandle& _root, Tstream& _stream){
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

    bool locatePath(TstringRef& _msg){
        Fconn = nullptr;
        Fmh = nullptr;

        if (!_msg.parseValue(Fport)){
            send("E 1");
            return false;
        }

        Fmh = Froot;
        //locate path necessary???
        if (_msg.hasNext()){
            //path not found???
            Tlocator l(Froot);
            if (!l.locate(_msg)){
                send("E 2");
                return false;
            }

            //path doesn't point to a struct???
            if (!l.result()->isStruct()){
                send("E 3");
                return false;
            }

            //structs = nullptr????
            Fmh = static_cast<Tstruct*>(l.result())->value();
            if (!Fmh){
                send("E 4");
                return false;
            }
        }

        //find port
        for (auto it = Fconnections.iterator(); it.hasNext(); ){
            auto lconn = it.next();
            if (lconn->FobjEvent.Ftag == Fport){
                Fconn = lconn;
                TmenuHandle* lmh = Fconn->menuHandle();
                lmh->events()->remove(&Fconn->FobjEvent);
                Fconn->FobjEvent.event()->reclaim();
                break;
            }
        }
        return true;
    }

    void linkPath(TstringRef& _msg){
        if (!locatePath(_msg)) return;

        //if connection for recycling found...
        Tconnection* conn = Fconn;
        if (!conn){
            conn = new Tconnection(this);
            Fconnections.push_first(conn);
        }
        conn->FobjEvent.Fstruct = Fmh;
        conn->FobjEvent.Ftag = Fport;
        Fmh->events()->push_first(&conn->FobjEvent);
        conn->FobjEvent.signal();
    }

    void unlinkPath(TstringRef& _msg){
        if (!locatePath(_msg)) return;
        if (!Fconn){
            send("E 5");
            return;
        }
        Fconnections.remove(Fconn);
        delete Fconn;
        Fstream->write("u ");
        Fstream->write(Fport);
        Fstream->flush();
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

      case 'U':
        unlinkPath(_msg);
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

    void handleMessage(TstringRef& msg){
        char cmd = msg.next();
        if (!msg.hasNext()) handleCommand(cmd,msg);
        else{
            char blank = msg.next();
            if (blank == ' '){
                handleCommand(cmd,msg);
            }
            else{
                msg.offset(-2);
                handleReadWrite(msg);
            }
        }
    }

    void handleMessage(const char* _msg){
        TstringRef msg(_msg);
        handleMessage(msg);
    }

    void handleMessage(char* _msg, int _length){
        while (_length > 0 && isspace(_msg[_length-1])) _msg[--_length] = '\0';
        handleMessage(_msg);
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

#endif
