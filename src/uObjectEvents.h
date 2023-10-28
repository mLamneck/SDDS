#ifndef UOBJECTEVENTS_H_INCLUDED
#define UOBJECTEVENTS_H_INCLUDED

#include "uMultask.h"
#include "uTypedef.h"

class TmenuHandle;

class TobjectEvent : public TlinkedListElement{
    private:
        Tevent Fevent;
        const char* Fname;
    public:
        TmenuHandle* Fstruct;
        char Ftag;

        Tevent* event(){ return &Fevent; }
        TobjectEvent(Tthread* _owner, const char* _name) : Fevent(_owner){
            Fname = _name;
            Fevent.setContext(this);
        }
};

class TobjectEventList : public TlinkedList<TobjectEvent>{
    public:
        void signal(){
            for (auto it = iterator(); it.hasNext(); ){
                it.next()->event()->signal();
            }
        }
};

#endif // UOBJECTEVENTS_H_INCLUDED
