#ifndef UMULTASK_H
#define UMULTASK_H

#define MULTASK_DEBUG false

#include "uPlatform.h"
#include "uLinkedList.h"

class Tevent;
class Tthread;
class TtaskHandler;


/************************************************************************************
Tevent
*************************************************************************************/

class Tevent;
typedef void (*TeventProc)(Tevent*);

namespace multask{
    typedef dtypes::uint8 Tpriority;
    constexpr int Tpriority_highest = 255;
}

class Tevent : public TlinkedListElement{
	typedef dtypes::TsystemTime TsystemTime;

    friend class Tthread;
    friend class TtaskHandler;
    private:
        Tthread* Fowner = nullptr;
        TsystemTime FdeliveryTime;
        multask::Tpriority Fpriority = 0;
    protected:
        virtual void beforeDispatch(){};
        virtual void execute(){};
        virtual void afterDispatch(){};
    public:
		union{
			struct{
				dtypes::uint8 byte0;
				dtypes::uint8 byte1;
				dtypes::uint8 byte2;
				dtypes::uint8 byte3;
			};
			struct{
				dtypes::uint16 word0;
				dtypes::uint16 word1;
			};
		} args;
        #if MULTASK_DEBUG
        const char* Fname;
        #endif

        Tthread* owner() { return Fowner; }
        inline TsystemTime deliveryTime() { return FdeliveryTime; }

        void signal();
        void signalFromIsr();
        void setTimeEvent(const TsystemTime _relTime);
        void setTimeEventTicks(const TsystemTime _relTime);
        void reclaim();

        multask::Tpriority priority() { return Fpriority; }
        void setPriority(multask::Tpriority _p){ Fpriority = _p; }
        void setOwner(Tthread* _owner) { Fowner = _owner; }

        Tevent(){};
        Tevent(Tthread* _owner);
        Tevent(Tthread* _owner, multask::Tpriority _priority);
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

namespace multask{
	typedef dtypes::TsystemTime TsystemTime;
}

class TtaskHandler{
	typedef multask::TsystemTime TsystemTime;

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

        void setTaskPriority(Tthread* _thread, multask::Tpriority _priority, bool _transferEvents = true);

        void unlinkTimeEvent(Tevent* _ev){
            if (!_ev->linked()) return;
            if (FtimerQ.remove(_ev)) return;
            if (FprocQ.remove(_ev)) return;
        };

        void signalEvent(Tevent* _ev);
        void signalEventISR(Tevent* _ev);
        void setTimeEvent(Tevent* _ev, const TsystemTime _relTime);
        void reclaimEvent(Tevent* _ev);

        void dispatchEvent(Tevent* _ev, bool _eventFromIsr);
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

    multask::Tpriority Fpriority = 0;
    protected:
        multask::TeventQ FtaskQ;
    public:
        Tthread();
        Tthread(const char* _name);
        bool isTaskEvent(Tevent* _ev) { return (this==_ev); };
        Tevent* getTaskEvent() { return this; }
        void setPriority(multask::Tpriority _priority, bool _transferEvents = true);
        multask::Tpriority priority() { return Fpriority; }
    protected:
        virtual void execute(Tevent* _ev) = 0;
};



#endif
