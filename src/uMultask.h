#ifndef UMULTASK_H
#define UMULTASK_H

#define MULTASK_DEBUG false

#include "uPlatform.h"
#include "uLinkedList.h"

using namespace dtypes;

class Tevent;
class Ttask;
class Tthread;
class TtaskHandler;


/************************************************************************************
Tevent
*************************************************************************************/

class Tevent;
typedef void (*TeventProc)(Tevent*);

class Tevent : public TlinkedListElement{
    friend class Tthread;
    friend class TtaskHandler;
    private:
        Tthread* Fowner;
        TsystemTime FdeliveryTime;
        TeventProc Fcallback = nullptr;
        void* Fcontext;
    protected:
        virtual void beforeDispatch(){};
        virtual void afterDispatch(){};
    public:
        #if MULTASK_DEBUG
        const char* Fname;
        #endif

        Tthread* owner() { return Fowner; }
        inline TsystemTime deliveryTime() { return FdeliveryTime; }

        void signal();
        void setTimeEvent(TsystemTime _relTime);
        void reclaim();

        void setContext(void* _ctx){ Fcontext = _ctx; }
        void* context(){ return Fcontext; }

        Tevent(Tthread* _owner);
        Tevent(TeventProc _cb): Fcallback{_cb} {}
        Tevent(Tthread* _owner, const char* _name) : Tevent(_owner){
            #if MULTASK_DEBUG
                Fname = _name;
            #endif
        };

};


/************************************************************************************
TeventQ - only for debuggin purpose print/remove
*************************************************************************************/

class TeventQ : public TlinkedList<Tevent>{
    public:
        void print(){
            #if MULTASK_DEBUG
            auto it = iterator();
            while (it.hasNext()){
                auto ev = it.next();
                debug::log("name=%s delTime=%d",ev->Fname,ev->deliveryTime());
            }
            #endif
        }
        bool remove(Tevent* _ev){
            #if MULTASK_DEBUG
                debug::log("--- remove event");
                print();
                bool res = TlinkedList<Tevent>::remove(_ev);
                print();
                return res;
            #else
                return TlinkedList<Tevent>::remove(_ev);
            #endif
        }
};


/************************************************************************************
TtaskHandler
*************************************************************************************/

class TtaskHandler{
    friend class Tthread;
    friend class Tevent;
    private:
        TeventQ FprocQ;
        TeventQ FtimerQ;
        Tthread* FcurrTask;
        TsystemTime FsysTime = millis();

        inline TsystemTime sysTime(){ return FsysTime; }

        void unlinkTimeEvent(Tevent* _ev){
            if (!_ev->linked()) return;
            if (FtimerQ.remove(_ev)) return;
            if (FprocQ.remove(_ev)) return;
        };

        void signalEvent(Tevent* _ev);
        void setTimeEvent(Tevent* _ev, TsystemTime _relTime);
        void reclaimEvent(Tevent* _ev);

        void dispatchEvent(Tevent* _ev);
        void calcTime();
        bool _handleEvent();
        void _handleEvents();
    public:
        static void handleEvents();
};


/************************************************************************************
Tthread
*************************************************************************************/

class Tthread : public Tevent{
    using Tevent::Tevent;
    friend class TtaskHandler;
    public:
        Tthread();
        Tthread(const char* _name);
        bool isTaskEvent(Tevent* _ev) { return (this==_ev); };
    protected:
        virtual void execute(Tevent* _ev) = 0;
};



#endif
