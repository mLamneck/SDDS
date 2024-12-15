#include "uStrings.h"
#include "uTypedef.h"


/************************************************************************************
Tdescr - abstract class for all types
*************************************************************************************/

TmenuHandle* Tdescr::findRoot(){
    if (!Fparent) return nullptr;

    TmenuHandle* root = Fparent;
    while (root->Fparent) { root = root->Fparent; }
    return root;
}

void Tdescr::signalEvents(){
    Fcallbacks.emit();
    if (Fparent){
        Fparent->signalEvents(this);
    }
}


/************************************************************************************
TobjectEvent
*************************************************************************************/

void TobjectEvent::TproxyEvent::afterDispatch(){
    FobjectEvent->afterDispatch();
}

void TobjectEvent::TproxyEvent::execute(){
    FobjectEvent->execute();
}

void TobjectEvent::afterDispatch(){
    FchangedRange.Ffirst = sdds::TrangeItem_max;
    FchangedRange.Flast = 0;
}

void TobjectEvent::execute(){
   
}

Tevent* TobjectEvent::event(){
    return &Fevent;
}

void TobjectEvent::signal(TrangeItem _first, TrangeItem _last){
	Trange newRange(_first,_last);
	if (!newRange.intersection(FobservedRange)) 
		return;

    if (newRange.Ffirst < FchangedRange.Ffirst){
        FchangedRange.Ffirst = newRange.Ffirst;
    }
    if (newRange.Flast > FchangedRange.Flast){
        FchangedRange.Flast = newRange.Flast;
    }
    event()->signal();
}

void TobjectEvent::cleanup(){
	FobservedObj = nullptr;
	setOwner(nullptr);
	Fevent.reclaim();
}

void TobjectEventList::signal(TrangeItem _first, int _n, dtypes::uint16 _port){
	if (_n <= 0) return;

	for (auto it = iterator(); it.hasCurrent(); it.jumpToNext() ){
		auto ev = it.current();
		if ((_port != 0) && (_port == ev->Fevent.args.word1))
			continue;
		ev->signal(_first,_first+_n-1);
	}
}


/************************************************************************************
TmenuHandle - base class to be used to declare a structure with descriptive elements
*************************************************************************************/

TmenuHandle::TmenuHandle(){
    Fvalue = this;
};

Tdescr* TmenuHandle::find(const char* _name){
    auto it = iterator();
    while (it.hasCurrent()){
        Tdescr* descr = it.current();
		it.jumpToNext();
        if (string_cmp(descr->name(),_name)){
            return descr;
        }
    }
    return nullptr;
}

namespace sdds{
	constexpr dtypes::uint16 TlinkTime::TIMES[];

    Ttimer& setup(){
        /*
        For some fucking reason, on TEENSY 3.2 and probably on other ARM CPU's
        this static object creation does not work. The constructor is not called!!!
        Fuck C++!!! This is the worst programming language I know. Even the most primitive
        thing like global variables does not work with this freaking bullshit!!!! Fuck C++!!!!!!
        */        
        //static TsetupTimer __setup;
        static Ttimer* __setup = nullptr;
        if (!__setup){
            __setup = new Ttimer;
            __setup->start(0);
        }
        return *__setup;
    }
}
