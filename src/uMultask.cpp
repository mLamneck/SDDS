#include "uMultask.h"
#if defined(__MINGW64__)
    #include <windows.h>
	#include <chrono>
#endif

using namespace multask;

/************************************************************************************
TtaskHandler
*************************************************************************************/

TtaskHandler& taskHandler(){
    static TtaskHandler taskhandler;
    return taskhandler;
}

void TtaskHandler::setTaskPriority(Tthread* _thread, multask::Tpriority _priority, bool _transferEvents){
    if (_transferEvents){
		if (_priority < _thread->Fpriority){
			while (_thread->FtaskQ.hasElements()){
				auto ev = _thread->FtaskQ.pop();
				signalEvent(ev);
			}
		}
	}
    _thread->Fpriority = _priority;
}

void TtaskHandler::signalEvent(Tevent* _ev){
    //FprocQ.push_first(_ev);
	if (_ev->linked()) return;
    auto prior = _ev->priority();
    auto it = FprocQ.iterator();
    while (it.hasCurrent()){
        auto ev = it.current();
        if (prior > ev->priority()){ break; }
		it.jumpToNext();
    }
    it.insert(_ev);
}

void TtaskHandler::signalEventISR(Tevent* _ev){
	__sdds_isr_critical(
		FinterruptQ.push_first(_ev);
	)
}

void TtaskHandler::setTimeEvent(Tevent* _ev, const TsystemTime _relTime){
    unlinkTimeEvent(_ev);
    TsystemTime now = sysTime();
    TsystemTime delTime = now +_relTime;
    auto it = FtimerQ.iterator();

    while (it.hasCurrent()){
        auto ev = it.current();
        if (delTime < ev->deliveryTime()){ break; }
		it.jumpToNext();
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

void TtaskHandler::dispatchEvent(Tevent* _ev, bool _eventFromIsr){
    FcurrTask = _ev->Fowner;
    if (_ev->Fowner) {
		//never put an event from isr to the taskQ!!! race condition!!!
        if ((_ev->Fpriority >= _ev->Fowner->Fpriority) || _eventFromIsr ){
            _ev->beforeDispatch();
            _ev->Fowner->execute(_ev);
            _ev->afterDispatch();
        } else {
            _ev->Fowner->FtaskQ.push_first(_ev);
        }
    } else {
        _ev->beforeDispatch();
        _ev->execute();
        _ev->afterDispatch();
    } 
    FcurrTask = nullptr;
}

void TtaskHandler::calcTime(){
    #if MARKI_DEBUG_PLATFORM == 1
    #else
        FsysTime = millis();
    #endif
}

bool TtaskHandler::_handleEvent(){
    Tevent* ev = nullptr;

    if (FinterruptQ.hasElements()){
    	__sdds_isr_critical(
    		ev = FinterruptQ.pop();
    	)
        if (ev){
            dispatchEvent(ev,true);
            return true;
        }
    }

    calcTime();
    ev = FtimerQ.first();
    if (ev){
        TsystemTime delTime = ev->deliveryTime();
        if (sysTime() >= delTime){
            FtimerQ.pop();
            dispatchEvent(ev,false);
            return true;
        }
    }

    ev = FprocQ.pop();
    if (ev){
        dispatchEvent(ev,false);
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
		auto start = std::chrono::high_resolution_clock::now();
		Sleep(waitTime);
		auto end = std::chrono::high_resolution_clock::now();
		auto actualWaitTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		FsysTime += actualWaitTime;
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

Tevent::Tevent(Tthread* _owner, multask::Tpriority _priority) : 
    Tevent(_owner)
{
    Fpriority = _priority;
}

void Tevent::signal(){
    taskHandler().signalEvent(this);
}

void Tevent::signalFromIsr(){
    taskHandler().signalEventISR(this);
}

constexpr TsystemTime timeToMilliseconds(const TsystemTime _time){
	return _time*sdds::sysTime::MILLIS;
}

void Tevent::setTimeEvent(const TsystemTime _relTime){
	taskHandler().setTimeEvent(this, timeToMilliseconds(_relTime));
}

void Tevent::setTimeEventTicks(const TsystemTime _relTime){
	taskHandler().setTimeEvent(this,_relTime);
}

void Tevent::reclaim(){
    taskHandler().reclaimEvent(this);
}

void TisrEvent::signal(){
    taskHandler().signalEventISR(this);
}


/************************************************************************************
Tthread
*************************************************************************************/

Tthread::Tthread() : Tevent(this,multask::Tpriority_highest){
    taskHandler().FprocQ.push_first(this);
}

Tthread::Tthread(const char* _name) : Tevent(this, _name){
    getTaskEvent()->setPriority(multask::Tpriority_highest);
    taskHandler().FprocQ.push_first(this);
};

void Tthread::setPriority(multask::Tpriority _priority, bool _transferEvents){
    taskHandler().setTaskPriority(this,_priority,_transferEvents);
}

