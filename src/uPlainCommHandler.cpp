#include "uPlainCommHandler.h"

using namespace plainComm;
using namespace sdds;

void TplainCommHandler::send(const char* _msg){
    Fstream->write(_msg);
    Fstream->flush();
}

void TplainCommHandler::startSendTypes(TmenuHandle* _struct){
    Fstream->write("t ");
    Fstream->write(Fport);
    Fstream->write(' ');
    TjsonSerializer s(_struct,Fstream);
    s.serialize();
    Fstream->flush();
}

bool TplainCommHandler::sendError(Terror::e _errCode, Tport _port){
    Fstream->write("E ");
    Fstream->write(_port);
    Fstream->write(' ');
    Fstream->write(plainComm::Terror::ord(_errCode));
    Fstream->write(' ');
    Fstream->write(plainComm::Terror::c_str(_errCode));
    Fstream->flush();
    return false;
}

bool TplainCommHandler::scanTree(TstringRef& _msg){
    Fmh = Froot;
    if (_msg.hasNext()){
        //path not found???
        Tlocator l(Froot);
        if (!l.locate(_msg)) return sendError(Terror::e::pathNotFound,Fport);

        //path doesn't point to a struct???
        if (!l.result()->isStruct()) return sendError(Terror::e::pathNoStruct,Fport);

        //structs = nullptr????
        Fmh = static_cast<Tstruct*>(l.result())->value();
        if (!Fmh) return sendError(Terror::e::pathNullPtr,Fport);
    }
    return true;
}

bool TplainCommHandler::prepareConnRelatedMsg(TstringRef& _msg){
    Fconn = nullptr;
    Fmh = nullptr;

    if (!_msg.parseValue(Fport)) return sendError(Terror::e::portParseErr);
    _msg.next();    //skip separator

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

void TplainCommHandler::linkPath(TstringRef& _msg){
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

void TplainCommHandler::unlinkPath(TstringRef& _msg){
    //this does work for empty path
    if (!prepareConnRelatedMsg(_msg)) return;
    if (!Fconn){
        sendError(Terror::e::invPort,Fport);
        return;
    }
    
    Fconnections.remove(Fconn);
    delete Fconn;
    Fstream->write("u ");
    Fstream->write(Fport);
    Fstream->flush();
}

void TplainCommHandler::handleCommand(Tcmd _cmd, TstringRef& _msg){
    switch (_cmd)
    {
        case 'T':
            if (_msg.parseValue(Fport)){
                if (!_msg.next() == ' '){
                    sendError(Terror::e::portParseErr);
                    return;
                }
            } 
            else Fport = 0;
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
            sendError(Terror::e::invFunc);
            break;
    }
}

void TplainCommHandler::handleReadWrite(TstringRef& msg){
    Tlocator l(Froot);
    if (l.locate(msg)){
        if (msg.get(-1) == '='){
            l.result()->setValue(msg.pCurr());
            return;
        }
        debug::log("%s = %s",l.result()->name(),l.result()->to_string().c_str());
        Tdescr* var = l.result();
        Fstream->write(var->name());
        Fstream->write('=');
        if (var->isStruct()){
            Tstruct* s = static_cast<Tstruct*>(var);
            if (!s->value()){
                Fstream->write("NULL");
                return;
            }
            TjsonSerializer::serializeValues(Fstream,s->value(),0);
            Fstream->flush();
        }else{
            Fstream->write(var->to_string());
            Fstream->flush();
        }
    };
}


void TplainCommHandler::handleMessage(TstringRef& msg){
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

void TplainCommHandler::execute(Tevent* _ev){ 
    if (!isTaskEvent(_ev)){
        TobjectEvent* oe= static_cast<TobjectEvent*>(_ev->context());
        auto first = oe->first();
        auto last = oe->last();

        Fstream->write("l ");
        Fstream->write(oe->Ftag);
        Fstream->write(" ");
        Fstream->write(first);
        Fstream->write(" ");
        TjsonSerializer::serializeValues(Fstream,oe->Fstruct,first,last);
        Fstream->flush();
    }
    else{
        Fstream->write("B 0");
        Fstream->flush();
    }

}
