#include "uMultask.h"
#if defined(__MINGW64__)
    #include <windows.h>
#endif


/************************************************************************************
TtaskHandler
*************************************************************************************/

TtaskHandler& taskHandler(){
    static TtaskHandler taskhandler;
    return taskhandler;
}

void TtaskHandler::signalEvent(Tevent* _ev){
    FprocQ.push_first(_ev);
}

void TtaskHandler::setTimeEvent(Tevent* _ev, TsystemTime _relTime){
    unlinkTimeEvent(_ev);
    TsystemTime now = sysTime();
    TsystemTime delTime = now +_relTime;
    auto it = FtimerQ.iterator();

    while (it.hasNext()){
        auto ev = it.next();
        if (delTime < ev->deliveryTime()){ break; }
    }
    _ev->FdeliveryTime = delTime;
    #if MULTASK_DEBUG
    debug::log("insert event %s, delT=%d before event %s, delT=%d",_ev->Fname,delTime,it.prev()==&FtimerQ?"null":static_cast<Tevent*>(it.prev())->Fname,it.prev()==&FtimerQ?-1:static_cast<Tevent*>(it.prev())->deliveryTime());
    #endif
    it.insert(_ev);
}

void TtaskHandler::reclaimEvent(Tevent* _ev){
    if (FprocQ.remove(_ev)) return;
    if (FtimerQ.remove(_ev)) return;
}

void TtaskHandler::dispatchEvent(Tevent* _ev){
    FcurrTask = _ev->Fowner;
    if (_ev->Fcallback){
        _ev->Fcallback(_ev);
    }
    else if (_ev->Fowner){
        _ev->Fowner->execute(_ev);
    }
    FcurrTask = nullptr;
}

void TtaskHandler::calcTime(){
    #if MARKI_DEBUG_PLATFORM == 1
    #else
        FysTime = millis();
    #endif
}

bool TtaskHandler::_handleEvent(){
    calcTime();
    auto ev = FtimerQ.first();
    if (ev){
        TsystemTime delTime = ev->deliveryTime();
        if (delTime >= sysTime()){
            FtimerQ.pop();
            dispatchEvent(ev);
            return true;
        }
    }

    ev = FprocQ.pop();
    if (ev){
        dispatchEvent(ev);
        return true;
    }

    return false;
};

void TtaskHandler::_handleEvents(){
    while (_handleEvent()){};
    #if MARKI_DEBUG_PLATFORM == 1
    auto ev = FtimerQ.first();
    if (ev){
        TsystemTime now = sysTime();
        TsystemTime waitTime = ev->deliveryTime() - now;
        if (waitTime > 0){
            Sleep(waitTime);
            FsysTime += waitTime;
        }
    }
    #endif
}


void TtaskHandler::handleEvents(){
    taskHandler()._handleEvents();
};


/************************************************************************************
Tevent
*************************************************************************************/

Tevent::Tevent(Tthread* _owner){
    //TlinkedListElement();
    Fowner = _owner;
}
void Tevent::signal(){
    taskHandler().signalEvent(this);
}

void Tevent::setTimeEvent(TsystemTime _relTime){
    taskHandler().setTimeEvent(this, _relTime);
}

void Tevent::reclaim(){
    taskHandler().reclaimEvent(this);
}


/************************************************************************************
Tthread
*************************************************************************************/

Tthread::Tthread() : Tevent(this){
    taskHandler().FprocQ.push_first(this);
}

Tthread::Tthread(const char* _name) : Tevent(this, _name){
    taskHandler().FprocQ.push_first(this);
};
