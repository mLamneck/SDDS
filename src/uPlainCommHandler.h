#ifndef UPLAINCOMMHANDLER_H
#define UPLAINCOMMHANDLER_H

#include "uPlatform.h"
#include "uMultask.h"
#include "uTypedef.h"
#include "uStrings.h"
#include "uJsonSerializer.h"
#include "uPlainCommErrors.h"

class Tconnection : public TlinkedListElement{
    public:
        ~Tconnection(){
            FobjEvent.menuHandle()->events()->remove(&FobjEvent);
            FobjEvent.event()->reclaim();
        };
        TobjectEvent FobjEvent;
        Tconnection(Tthread* _owner) : FobjEvent(_owner){}
        TmenuHandle* menuHandle() { return FobjEvent.menuHandle(); }
};

class TplainCommHandler : public Tthread{
  typedef dtypes::int8 Tcmd;
  typedef dtypes::uint8 Tport;

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

	dtypes::uint8 getPort(TobjectEvent* _oe){ return _oe->event()->args.byte0; }
	void setPort(TobjectEvent* _oe, const dtypes::uint8 _val){ _oe->event()->args.byte0 = _val; }

    void send(const char* _msg);

    void startSendTypes(TmenuHandle* _struct);

    bool sendError(plainComm::Terror::e _errCode, Tport _port = 0);

    bool scanTree(TstringRef& _msg);

    bool prepareConnRelatedMsg(TstringRef& _msg);

    void linkPath(TstringRef& _msg);

    void unlinkPath(TstringRef& _msg);

    void handleCommand(Tcmd _cmd, TstringRef& _msg);

    void handleReadWrite(TstringRef& msg);

    void handleMessage(TstringRef& msg);

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
    
    void execute(Tevent* _ev) override;

};

#endif
