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
        Tthread* Fowner = nullptr;
        TsystemTime FdeliveryTime;
    protected:
        virtual void beforeDispatch(){};
        virtual void execute(){};
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

        Tevent(){};
        Tevent(Tthread* _owner);
        Tevent(Tthread* _owner, const char* _name) : Tevent(_owner){
            #if MULTASK_DEBUG
                Fname = _name;
            #endif
        };
};

namespace multask{

    /**
     * @brief TisrEvent event to be used to notify the application that an interrupt occured
     * 
     * This type of event can be used to send the application a notification from withing an 
     * interrupt. We hide methods derived from Tevent in a private section, because these are
     * not allowed to be called.
     */
    class TisrEvent : public Tevent{
        private:
            using Tevent::signal;
            using Tevent::setTimeEvent;
            using Tevent::reclaim;
        public:
            using Tevent::Tevent;
            void signal();
    };

    /**
     * @brief TisrEventDataQ
     * 
     * this is not tested and fully implemented yet. But the idea is to have a ring buffer
     * That acts like a fifo and can be in one direction, sending data from an interrupt
     * to the application. It's limited to 255 elements of a given type.
     * 
     * @tparam qsize number of elments that can fit into the ringbuffer
     * @tparam dtype the datatype of each element. If not specified this will be byte.
     */
    template <dtypes::uint8 qsize, typename dtype=dtypes::uint8>
    class TisrEventDataQ : TisrEvent{
        dtype Fqueue[qsize];
        dtypes::uint8 Fhead;
        dtypes::uint8 Ftail;
        
        public:
            using TisrEvent::signal;
            
            bool read(dtype& _out){
                if (Fhead == Ftail) return false;
                dtypes::uint8 tail = Ftail++;
                _out = Fqueue[Ftail];
                Ftail = (tail >= qsize) ? 0 : tail;
                return true;
            }

            bool write(dtype _data){
                dtypes::uint8 head = Fhead >= qsize? 0 : Fhead+1;
                if (head == Ftail) return false;
                Fqueue[head] = _data;
                Fhead = head;
                return true;
            }
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
}


/************************************************************************************
TtaskHandler
*************************************************************************************/

class TtaskHandler{
    friend class Tthread;
    friend class Tevent;
    friend class multask::TisrEvent;
    private:
        multask::TeventQ FinterruptQ;
        multask::TeventQ FprocQ;
        multask::TeventQ FtimerQ;
        Tthread* FcurrTask;
        TsystemTime FsysTime = millis();

        inline TsystemTime sysTime(){ return FsysTime; }

        void unlinkTimeEvent(Tevent* _ev){
            if (!_ev->linked()) return;
            if (FtimerQ.remove(_ev)) return;
            if (FprocQ.remove(_ev)) return;
        };

        void signalEvent(Tevent* _ev);
        void signalEventISR(Tevent* _ev);
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
