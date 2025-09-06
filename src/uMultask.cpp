#include "uMultask.h"
#include "uPlatform.h"
#include "mhal/uSystemInfo.h"

#if defined(__MINGW64__)
    #include <windows.h>
	#include <chrono>
#endif
using namespace multask;

dtypes::TtickCount_signed diffTime(TtickCount _t1, TtickCount _t2){
	return static_cast<dtypes::TtickCount_signed>(_t1-_t2);
}

/************************************************************************************
TtaskHandler
*************************************************************************************/

dtypes::uint32 TtaskHandler::FbusyTicks = 0;

TtaskHandler& taskHandler(){
    static TtaskHandler taskhandler;
    return taskhandler;
}

void TtaskHandler::setTaskPriority(Tthread* _thread, multask::Tpriority _priority, bool _transferEvents){
    if (_transferEvents){
		if (_priority < _thread->Fpriority){
			while (_thread->FtaskQ.hasElements()){
				auto ev = _thread->FtaskQ.pop();
				/**
				 * Background:
				 * So far, we use `signalEvent(ev)` to enqueue an event in the process queue —
				 * regardless of whether it is a time event or a regular process event.
				 * Since an expired time event can be delivered immediately, this behavior
				 * initially seems reasonable.
				 *
				 * Problem:
				 * In a scenario with only one resource and multiple competing events, we usually
				 * rely on task priority to filter out asynchronous events. However, if those
				 * events are time events, the following issue occurs:
				 * - The first time event acquires the resource, reschedules itself for the next
				 *   access, and raises the task priority.
				 * - Other events are pushed into the task’s private queue.
				 * - Once the resource is released, the task lowers its priority and queued events
				 *   are transferred to the process queue.
				 * - Since time events are handled before process events, the first event can run
				 *   again immediately, starving other events.
				 *
				 * Solution:
				 * Instead of moving events from the task queue into the process queue, we transfer
				 * them into the timer queue and set their delivery time to "now".
				 * This creates a round-robin effect:
				 * - The lower-priority event is last in the task queue.
				 * - After transfer to the timer queue, it becomes the next one delivered.
				 * - Events are thus handled in the correct order.
				 *
				 * Benefit:
				 * This approach is more efficient because transferring events back requires no
				 * priority check — only a simple `push_first` operation.
				 *
				 * Limitation:
				 * There may be cases where events are not delivered in strict priority order:
				 * - An event coming from the task queue is reinserted without re-checking priority.
				 * - After the task lowers its priority, process events with lower priority may be
				 *   executed before higher-priority ones.
				 *
				 * Possible fix:
				 * When transferring an event to the task queue, store its origin so that a priority
				 * check can be performed when moving it back.
				 *
				 * signalEvent(ev);
				 */
				ev->FdeliveryTime = sysTime();
				FtimerQ.push_first(ev);
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

void TtaskHandler::setTimeEvent(Tevent* _ev, const TtickCount _relTime){
	unlinkTimeEvent(_ev);
	TtickCount now = sysTime();
	TtickCount delTime = now +_relTime;
	auto it = FtimerQ.iterator();

	while (it.hasCurrent()){
		auto ev = it.current();
		if (diffTime(delTime,ev->deliveryTime()) < 0){ break; }
		it.jumpToNext();
	}
	_ev->FdeliveryTime = delTime;
	#if MULTASK_DEBUG
	debug::log("insert event %s, delT=%d before event %s, delT=%d",_ev->Fname,delTime,it.prev()==&FtimerQ?"null":static_cast<Tevent*>(it.prev())->Fname,it.prev()==&FtimerQ?-1:static_cast<Tevent*>(it.prev())->deliveryTime());
	#endif
	it.insert(_ev);
}

void TtaskHandler::unlinkTimeEvent(Tevent* _ev){
	if (!_ev->linked()) return;
	if (FtimerQ.remove(_ev)) return;
	if (FprocQ.remove(_ev)) return;
	if (_ev->Fowner)
		_ev->Fowner->FtaskQ.remove(_ev);
};

void TtaskHandler::reclaimEvent(Tevent* _ev){
	if (FprocQ.remove(_ev)) return;
	if (FtimerQ.remove(_ev)) return;
	if (_ev->Fowner)
		_ev->Fowner->FtaskQ.remove(_ev);
}

void TtaskHandler::dispatchEvent(Tevent* _ev, bool _eventFromIsr){
	if (!FscheduleState){
		FscheduleState = 1;
		FswBusy.start();
	}

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
        FsysTime = sdds::sysTime::tickCount();
    #endif
}

bool TtaskHandler::_handleEvent(){
	Tevent* ev = nullptr;

	int eventHandled = 0;
	
	int loopCnt = 0;
	while (FinterruptQ.hasElements()){
		__sdds_isr_critical(
			ev = FinterruptQ.pop();
		)
		if (!ev) break;

		dispatchEvent(ev,true);
		eventHandled = 1;
		if (++loopCnt >= 5){
			//store this event for debugging
			break;
		}
	}

	loopCnt = 0;
	ev = FtimerQ.first();
	while (ev){
		calcTime();

		TtickCount delTime = ev->deliveryTime();
		if (diffTime(sysTime(),delTime) < 0) break;

		FtimerQ.pop();
		dispatchEvent(ev,false);
		eventHandled = 1;
		if (++loopCnt >= 5){
			//store this event for debugging
			break;
		}
		ev = FtimerQ.first();
	}

	ev = FprocQ.pop();
	if (ev){
		dispatchEvent(ev,false);
		eventHandled = 1;
	}

	if (!eventHandled){
		if (FscheduleState > 0){
			FbusyTicks += FswBusy.getTicks();
			FscheduleState = 0;
		}
	}

	return eventHandled;
};

void TtaskHandler::_handleEvents(){
	while ( _handleEvent()){};

	#if MARKI_DEBUG_PLATFORM == 1
	while (_handleEvent()){};
	auto ev = FtimerQ.first();
	if (ev){
		calcTime();
		TtickCount now = sysTime();
		TtickCount waitTime = ev->deliveryTime() - now;
		if (waitTime > 0){
			auto start = std::chrono::high_resolution_clock::now();
			Sleep(waitTime);
			auto end = std::chrono::high_resolution_clock::now();
			auto actualWaitTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			FsysTime += actualWaitTime;
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

constexpr Tmilliseconds ticksToMillis(const TtickCount _time){
	return _time/sdds::sysTime::MILLIS;
}

constexpr TtickCount millisToTicks(const Tmilliseconds _time){
	return _time*sdds::sysTime::MILLIS;
}

void Tevent::setTimeEvent(const Tmilliseconds _relTime){
	taskHandler().setTimeEvent(this, millisToTicks(_relTime));
}

void Tevent::setTimeEventTicks(const TtickCount _relTime){
	taskHandler().setTimeEvent(this,_relTime);
}

void Tevent::reclaim(){
    taskHandler().reclaimEvent(this);
}

void Tevent::setOwner(Tthread* _owner) { 
	if (Fowner)
		Fowner->FtaskQ.remove(this);	//remove event from taskQ if owner is about to be switched or set to null
	Fowner = _owner; 
}

void multask::TisrEvent::signal(){
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


/************************************************************************************
TstopWatch
*************************************************************************************/

namespace multask{
	void TstopWatch::start(){
		FlastTicks = mhal::TsystemInfo::getCycleCount();
	}

	TtickCount TstopWatch::getTicks(){
		return mhal::TsystemInfo::getCycleCount()-FlastTicks;
	}
}
