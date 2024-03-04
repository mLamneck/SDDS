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
            case(1): return "val1";
            case(2): return "val2";
            case(3): return "val3";
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
        void setLastCreateStruct(TmenuHandle* _mh){
            FlastCreatedStruct = _mh;
        }

        void enterSds(){
            push(FlastCreatedStruct);
        }

        void leaveSds(){
            if (Fcnt > 0) Fcnt--;
        }

        void addDescr(Tdescr* _d){
            TstackItem* si = last();
            if (si){
                SDDS_LOG("add %s to %s",getVariableName(_d->createNummber).c_str(),getVariableName(si->mh->createNummber).c_str());
                si->mh->addDescr(_d);
            }
        }
} structStack;



/************************************************************************************
Tdescr - abstract class for all types
*************************************************************************************/

Tdescr::Tdescr(int id){
    #if MARKI_DEBUG_PLATFORM == 1
    createNummber = __createNummber++;
    #endif
    structStack.addDescr(this);
}

TstartMenuDefinition::TstartMenuDefinition(){
    structStack.enterSds();
}

TfinishMenuDefinition::TfinishMenuDefinition(){
    structStack.leaveSds();
}

TmenuHandle::TmenuHandle(){
    DEBUGCODE(getVariableName(createNummber))
    Fvalue = this;
    structStack.setLastCreateStruct(this);
};

void Tdescr::signalEvents(){
    Fcallbacks.emit(this);
    if (Fparent){
        Fparent->signalEvents(this);
    }
}


/************************************************************************************
TobjectEvent
*************************************************************************************/

void __TobjectEvent::afterDispatch(){
    auto ctx = context();
    if (ctx){
        static_cast<TobjectEvent*>(ctx)->afterDispatch();
    }
}

void TobjectEvent::afterDispatch(){
    Ffirst = TrangeItem_max;
    Flast = 0;
}

Tevent* TobjectEvent::event(){
    return &Fevent;
}

void TobjectEvent::signal(TrangeItem _first, TrangeItem _last){
    if (_first < Ffirst){
        Ffirst = _first;
    }
    if (_last > Flast){
        Flast = _last;
    }
    event()->signal();
}


TobjectEvent::TobjectEvent(Tthread* _owner, const char* _name) : Fevent(_owner){
    afterDispatch();
    Fname = _name;
    Fevent.setContext(this);
}

void TobjectEventList::signal(TrangeItem _first){
    for (auto it = iterator(); it.hasNext(); ){
        auto ev = it.next();
        ev->signal(_first,_first);
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


void handleTimerEvent(Tevent* _timerEv){
    //((Ttimer1*)_timerEv)->onTimerElapsed();
    static_cast<Ttimer*>(_timerEv)->onTimerElapsed();
}

