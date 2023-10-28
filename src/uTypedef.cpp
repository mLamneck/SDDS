#include "uStrings.h"
#include "uTypedef.h"

/************************************************************************************
TstructStack
*************************************************************************************/

class TstructStack{
    struct TstackItem{
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
            }
        }
        TstackItem* first(){
            if (Fcnt > 0) return &Fstack[Fcnt-1];
            return nullptr;
        }

    public:
        void setLastCreateStruct(TmenuHandle* _mh){
            debug::log("creating TmenuHandleAuto... %d",_mh);
            if (!FlastCreatedStruct){
                FlastCreatedStruct = _mh;
                inSds = false;
            }
            else if (inSds) FlastCreatedStruct = _mh;
            //else throw "cannot create structure outside of sds";
        }
        void enterSds(){
            debug::log("-> enter sds");
            inSds = true;
            push(FlastCreatedStruct);
        }
        void leaveSds(){
            debug::log("<- leave sds");
            inSds = false;
            if (Fcnt > 0) Fcnt--;
        }
        void addDescr(Tdescr* _d){
            TstackItem* si = first();
            debug::log("create descr -> stackItem %d",si);
            if (si){
                if (si->inSds && si->mh){
                    si->mh->addDescr(_d);
                }
                else{
                    debug::log("not in sds, thats why we skip descr %d",_d);
                }
            }
        }
} structStack;


/************************************************************************************
Tdescr - abstract class for all types
*************************************************************************************/


Tdescr::Tdescr(int id){
    structStack.addDescr(this);
}

TstartMenuDefinition::TstartMenuDefinition(){
    structStack.enterSds();
}

TfinishMenuDefinition::TfinishMenuDefinition(){
    structStack.leaveSds();
}

TmenuHandleAuto::TmenuHandleAuto(){
    Fvalue = this;
    structStack.setLastCreateStruct(this);
}






TmenuHandle::TmenuHandle(){
    debug::log("creating TmenuHandle...");
    //Fvalue = this;
    structStack.setLastCreateStruct(nullptr);
    //__lastCreatedMenu = nullptr;
};

void Tdescr::signalEvents(){
    Fcallbacks.emit(this);
    if (Fparent){
        Fparent->events()->signal();
    }
}


/************************************************************************************
TmenuHandle - base class to be used to declare a structure with descriptive elements
*************************************************************************************/

void TmenuHandle::print(){
    auto it = iterator();
    while (it.hasNext()){
        Tdescr* descr = it.next();
        if (descr->typeId() == TYPE_STRUCT){

            Tstruct* s = static_cast<Tstruct*>(descr);
            debug::log("-> %s",descr->name());
            TmenuHandle* _mh = s->value();
            if (_mh){
                _mh->print();
            }
            debug::log("<- %s",descr->name());
        }
        else{
            debug::log(descr->name());
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
