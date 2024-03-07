#ifndef UPLAINCOMMHANDLER_H
#define UPLAINCOMMHANDLER_H

#include "uPlatform.h"
#include "uMultask.h"
#include "uTypedef.h"
#include "uStrings.h"
#include "uJsonSerializer.h"


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
  typedef dtypes::uint8 Tport;
  typedef dtypes::uint8 TerrorCode;

  private:
    TmenuHandle* Froot;
    Tstream* Fstream;
    TlinkedList<Tconnection> Fconnections;

    //returned by locatePath
    TmenuHandle* Fmh;
    Tconnection* Fconn;
    Tport Fport;
  public:
    TplainCommHandler(TmenuHandle* _root, Tstream* _stream){
      Froot = _root;
      Fstream = _stream;
    }
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

    void startSendTypes(TmenuHandle* _struct){
        Fstream->write("t ");
        TjsonSerializer s(_struct,Fstream);
        s.serialize();
        Fstream->flush();
        //debug::log(Fstream1.Fbuffer.c_str());
    }
	
	bool sendError(TerrorCode _errCode, Tport _port = 0){
		Fstream->write("E ");
		Fstream->write(_port);
		Fstream->write(' ');
		Fstream->write(_errCode);
		//Fstream->write(errCode);
		Fstream->flush();
		return false;
	}

    bool scanTree(TstringRef& _msg){
        Fmh = Froot;
        if (_msg.hasNext()){
            //path not found???
            Tlocator l(Froot);
            if (!l.locate(_msg)) return sendError(2,Fport);

            //path doesn't point to a struct???
            if (!l.result()->isStruct()) return sendError(3,Fport);

            //structs = nullptr????
            Fmh = static_cast<Tstruct*>(l.result())->value();
            if (!Fmh) return sendError(4,Fport);
        }
        return true;
    }

    bool prepareConnRelatedMsg(TstringRef& _msg){
        Fconn = nullptr;
        Fmh = nullptr;

        if (!_msg.parseValue(Fport)) return sendError(1);
        _msg.next();    //skip seperator

        //locate path necessary???
        if (!scanTree(_msg)) return false;

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
        if (!prepareConnRelatedMsg(_msg)) return;

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
        //this does work for empty path
        if (!prepareConnRelatedMsg(_msg)) return;
        if (!Fconn){
			sendError(5,Fport);
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
        if (!scanTree(_msg)) return;
        startSendTypes(Fmh);
        break;

      case 'L':
        linkPath(_msg);
        break;

      case 'U':
        unlinkPath(_msg);
        break;

      default:
        sendError(6);
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

    /** \brief Handle the given msg. Any trailing whitespaces will be replaced with '\0'
     *
     * \param _msg a nullterminated string without trailing whitespaces
     */
    void handleMessage(const char* _msg){
        TstringRef msg(_msg);
        handleMessage(msg);
    }

    /** \brief Handle the given msg. Any trailing whitespaces will be replaced with '\0'
     *
     * \param _msg a nullterminated string
     * \param _length of the string
     */
    void handleMessage(char* _msg, int _length){
        while (_length > 0 && isspace(_msg[_length-1])) _msg[--_length] = '\0';
        handleMessage(_msg);
    }

    /** \brief Handle the given msg. Any trailing whitespaces will be replaced with '\0'
     *
     * \param _msg
     */
    void handleMessage(dtypes::string _msg){
        int _length = _msg.length();
        while (_length > 0 && isspace(_msg[_length-1])) _msg[--_length] = '\0';
        handleMessage(_msg);
    }

    /** \brief close any open connections and don't send any messages from now
     *
     */
    void shutdown(){
        auto lconn = Fconnections.pop();
        while (lconn){
            delete lconn;
            lconn = Fconnections.pop();
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

#endif
