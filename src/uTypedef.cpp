#include "uStrings.h"
#include "uTypedef.h"


/************************************************************************************
TstructStack
*************************************************************************************/


#if MARKI_DEBUG_PLATFORM == 1
    //#define DEBUGCODE(_code) _code ;
    #define DEBUGCODE(_code)
    #define SDDS_LOG(...) //debug::log(__VA_ARGS__)

    int __createNummber = 0;
    dtypes::string var_name = "";
    dtypes::string lastCreatedStruct = "";
    dtypes::string FlastCreatedStruct_ = "";
    dtypes::string structOnStack = "";

    /*
    create this list with struct.logCreateAssoc of a given structure and copy it here.

    userStruct.logCreateAssoc();
    */
    const char* _getVariableName(int createNumber){
        switch(createNumber){
            case(0): return "root";
            case(1): return "nestedStruct";
            case(2): return "rootVal0";
            case(3): return "doubleDerived";
            case(4): return "val1";
            case(5): return "val2";
            case(6): return "val3";
            case(7): return "val4";
            case(10): return "params";
            case(8): return "rootValN";
            case(9): return "ValN";
        }
        return "unknown";
    }

    dtypes::string getVariableName(int createNumber){
        var_name = _getVariableName(createNumber);
        return var_name;
    }
#else
    #define SDDS_LOG(...)
    #define DEBUGCODE(_code)
#endif


class __attribute__ ((packed)) TstructStack{
    struct __attribute__ ((packed)) TstackItem{
        TmenuHandle* mh;
        bool inSds;
    };

    private:
        constexpr static int STACKSIZE = 10;
        TstackItem Fstack[STACKSIZE];
        int Fcnt = 0;
        TmenuHandle* FlastCreatedStruct = nullptr;
        bool inSds = false;

        void push(TmenuHandle* _mh){
            if (Fcnt < STACKSIZE) {
                Fstack[Fcnt].inSds = inSds;
                Fstack[Fcnt++].mh = _mh;
                DEBUGCODE(structOnStack=getVariableName(_mh->createNummber));
            }
        }
        
        TstackItem* last(){
            if (Fcnt > 0) return &Fstack[Fcnt-1];
            return nullptr;
        }

    public:
        TmenuHandle* lastCreatedStruct(){ return FlastCreatedStruct; }

        void setLastCreateStruct(TmenuHandle* _mh){
            FlastCreatedStruct = _mh;
        }

        void enterSds(){
            push(FlastCreatedStruct);
            DEBUGCODE( structOnStack = getVariableName(last()->mh->createNummber).c_str(); )
        }

        void leaveSds(){
            if (Fcnt > 0) {
                FlastCreatedStruct = last()->mh;
                Fcnt--;
                DEBUGCODE(if (Fcnt > 0){ structOnStack = getVariableName(last()->mh->createNummber).c_str(); })
            }
        }

        void addDescr(Tdescr* _d){
            TstackItem* si = last();
            if (si){
                SDDS_LOG("add %s to %s",getVariableName(_d->createNummber).c_str(),getVariableName(si->mh->createNummber).c_str());
                si->mh->addDescr(_d);
            }
        }
};

TstructStack* structStack(){ 
    static TstructStack s;
    return &s;
}

TstartMenuDefinition::TstartMenuDefinition(){
    structStack()->enterSds();
}

TfinishMenuDefinition::TfinishMenuDefinition(){
    structStack()->leaveSds();
}

TmenuHandle::TmenuHandle(){
    DEBUGCODE(getVariableName(createNummber))
    Fvalue = this;
    structStack()->setLastCreateStruct(this);
};


/************************************************************************************
Tdescr - abstract class for all types
*************************************************************************************/

Tdescr::Tdescr(){
    #if MARKI_DEBUG_PLATFORM == 1
    createNummber = __createNummber++;
    #endif
    structStack()->addDescr(this);
}

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

void __TobjectEvent::afterDispatch(){
    FobjectEvent->afterDispatch();
}

void TobjectEvent::afterDispatch(){
    FchangedRange.Ffirst = sdds::TrangeItem_max;
    FchangedRange.Flast = 0;
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
	if (!Fstruct) return;
	Fstruct->events()->remove(this);
	Fevent.reclaim();
}

void TobjectEventList::signal(TrangeItem _first, int _n, dtypes::uint16 _port){
	if (_n <= 0) return;

	for (auto it = iterator(); it.hasNext(); ){
		auto ev = it.next();
		if ((_port != 0) && (_port == ev->Fevent.args.word1))
			continue;
		ev->signal(_first,_first+_n-1);
	}
}





/************************************************************************************
TmenuHandle - base class to be used to declare a structure with descriptive elements
*************************************************************************************/

void TmenuHandle::print(){
    auto it = iterator();
    while (it.hasNext()){
        Tdescr* descr = it.next();
        if (descr->isStruct()){

            Tstruct* s = static_cast<Tstruct*>(descr);
            SDDS_LOG("-> %s",descr->name());
            TmenuHandle* _mh = s->value();
            if (_mh){
                _mh->print();
            }
            SDDS_LOG("<- %s",descr->name());
        }
        else{
            SDDS_LOG(descr->name());
        }
    }
}

Tdescr* TmenuHandle::find(const char* _name){
    auto it = iterator();
    while (it.hasNext()){
        Tdescr* descr = it.next();
        if (string_cmp(descr->name(),_name)){
            return descr;
        }
    }
    return nullptr;
}

namespace sdds{
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