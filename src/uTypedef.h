/************************************************************************************
uTypeDef.h
toDo:
    - find an alternative for pValue() method to provide access to Fvalue for 
      streaming access;
*************************************************************************************/

#ifndef UTYPEDEF_H
#define UTYPEDEF_H

#include "uPlatform.h"
#include "uMultask.h"
#include "uLinkedList.h"
#include "uStrings.h"
#include "uEnumMacros.h"
#include "uTime.h"

#ifdef __AVR__
    //functional not available in AVR-gcc
#else
	#include <functional>
#endif


/************************************************************************************
macro expansion
*************************************************************************************/

#ifdef __AVR__
    //AVR-gcc seems to have some issues with __VAR_ARGS__ and __VA_OPT__
    #define descr(_var, _option, _value) addDescr(&_var, #_var, _option, _value)
#else
    #define on(_var) _var.Fcallbacks = [=](Tdescr* sender)
//#define descr(_var, ...) addDescr(&_var, #_var __VA_OPT__(,) __VA_ARGS__)
    #define descr(_var, ...) addDescr(&_var, #_var, ##__VA_ARGS__)
#endif

//__VA_OPT__ expands to ,VA_ARGS if VA_ARGS is not empty and to "" otherwise
//#define log(_fmt,...) debug::log(_fmt __VA_OPT__(,) __VA_ARGS__)


/************************************************************************************
forward declarations and typedefinitions for this unit
*************************************************************************************/

class TmenuHandle;
class Tdescr;

//callback for ons
#ifdef __AVR__
    //for AVR-gcc std::function is not available
    //we have to store a pure funcion ptr instead
    typedef void (*Tcallback)(Tdescr*);
#else
    typedef std::function<void(Tdescr* p)> Tcallback;
#endif


namespace sdds{
    typedef uint8_t Toption;
    
    namespace opt{
        constexpr Toption nothing   = 0;
        constexpr Toption readonly  = 0x01;
        constexpr Toption saveval   = 0x80;

        constexpr Toption timerel   = 0x02;

        constexpr Toption mask_show = 0x0E;
    }

    namespace typeIds{
        constexpr uint8_t size_mask = 0x0F; 
    }

    enum class Ttype : dtypes::uint8{
        UINT8   = 0x01,
        UINT16  = 0x02,
        UINT32  = 0x04,

        TIME    = 0x06,

        INT8    = 0x11,
        INT16   = 0x12,
        INT32   = 0x14,
        
        FLOAT32 = 0x24,
        
        ENUM    = 0x31,

        STRUCT  = 0x42,

        STRING  = 0x81
    };
}



/************************************************************************************
//callbacks for SCDSs
*************************************************************************************/

class TcallbackWrapper : public TlinkedListElement{
    private:
        Tcallback Fcallback;
    public:
        TcallbackWrapper(Tcallback _cb){
            Fcallback = _cb;
        }
        void emit(Tdescr* _descr){
            if (Fcallback){
                Fcallback(_descr);
            }
        }
};
typedef TlinkedListIterator<TcallbackWrapper> TcallbackIterator;

class Tcallbacks : public TlinkedList<TcallbackWrapper>{
    public:
        inline void emit(Tdescr* _descr){
            for (auto it = iterator(); it.hasNext();){
                it.next()->emit(_descr);
            }
        }

        inline TcallbackWrapper* add(Tcallback _cb){
            TcallbackWrapper* res = new TcallbackWrapper(_cb);
            push_back(res);
            return res;
        }
        inline TcallbackWrapper* operator=(Tcallback _cb){
            return add(_cb);
        }
};


/************************************************************************************
Tdescr - abstract class for all types
*************************************************************************************/

class Tdescr : public TlinkedListElement{
    private:
        TmenuHandle* Fparent = nullptr;

    public:
        #if MARKI_DEBUG_PLATFORM == 1
        int createNummber;
        #endif

        void signalEvents();
        Tdescr();
        Tcallbacks Fcallbacks;
        friend class TmenuHandle;

        TmenuHandle* findRoot();

        //providing type information
        virtual sdds::Ttype type() = 0;
        virtual dtypes::uint8 typeId(){ return static_cast<dtypes::uint8>(type()); }

        virtual const char* name(){return ""; };
        virtual sdds::Toption option(){return 0; };
        virtual dtypes::uint8 valSize() { return static_cast<uint8_t>(type()) & sdds::typeIds::size_mask; }
        virtual void* pValue() { return nullptr; };

        sdds::Toption showOption(){ return (option() & sdds::opt::mask_show); }
        inline bool saveval() { return ((option() & sdds::opt::saveval) > 0); }

        //propably compare default value to current and only save if different
        inline bool shouldBeSaved(){ return saveval(); }

        inline bool isStruct() { return (type()==sdds::Ttype::STRUCT); }

        inline bool hasChilds() {
            if (!isStruct()) return false;
        }

        //interface for generic handling
        virtual bool setValue(const char* _str) = 0;
        virtual TrawString to_string(){ return ""; }

        /************************************************************************************
        functions for type conversion used by derived types
        *************************************************************************************/

        //helper funtions
        static bool _strToDouble(const char* _str, double& _res){
            char * pEnd = NULL;
            _res = strtod(_str, &pEnd);
            if (*pEnd){
                return isspace(*pEnd);
            }
            return !(*pEnd);
        }
        template<typename ValType> static bool _strToNumber(const char* _str, ValType& _val){
            double d;
            bool result = Tdescr::_strToDouble(_str,d);
            _val = d;
            return result;
        }

        //********* convert internal values to string ***************

        template <typename valType>
        TrawString _valToStr(valType _val){return strConv::to_string(_val); }
        TrawString _valToStr(TdateTime _val){
            if (showOption() == sdds::opt::timerel) return timeToString(_val,"%H:%M:%S");
            return timeToString(_val);
        }
        TrawString _valToStr(TrawString _val){return _val; }
        TrawString _valToStr(TmenuHandle* _val){ return (_val) ? "1" : "0"; };


        //********* covert string to internal values ***************

        //unsigned int
        static bool _strToValue(const char* _str, dtypes::uint8& _value){return Tdescr::_strToNumber<dtypes::uint8>(_str,_value);}
        static bool _strToValue(const char* _str, dtypes::uint16& _value){return Tdescr::_strToNumber<dtypes::uint16>(_str,_value);}
        static bool _strToValue(const char* _str, dtypes::uint32& _value){return Tdescr::_strToNumber<dtypes::uint32>(_str,_value);}

        //signed int
        static bool _strToValue(const char* _str, dtypes::int8& _value){return Tdescr::_strToNumber<dtypes::int8>(_str,_value);}
        static bool _strToValue(const char* _str, dtypes::int16& _value){return Tdescr::_strToNumber<dtypes::int16>(_str,_value);}
        static bool _strToValue(const char* _str, dtypes::int32& _value){return Tdescr::_strToNumber<dtypes::int32>(_str,_value);}

        //floating point
        static bool _strToValue(const char* _str, dtypes::float32& _value){return Tdescr::_strToNumber<dtypes::float32>(_str,_value);}

        static bool _strToValue(const char* _str, dtypes::string& _value){ _value = _str; return true; }

        //DateTime
        static bool _strToValue(const char* _str, dtypes::TdateTime& _value){
            TdateTimeParser p(_str);
            bool res = p.parse();
            if (res) _value=p.Fresult;
            return res;
        ;}

        static bool _strToValue(const char* _str, TmenuHandle*& _value){return true;}

};


/************************************************************************************
template for the actual descriptive types
*************************************************************************************/

template <class ValType, sdds::Ttype _type_id> class TdescrTemplate: public Tdescr{
    private:
    protected:
        ValType Fvalue;
    public:
        friend class TmenuHandle;
        typedef ValType dtype;

        /* override copy constructor to avoid a deep copy of the object
         * including callbacks and events
        */
        TdescrTemplate(const TdescrTemplate& _src) : TdescrTemplate(){
            Fvalue = _src.Fvalue;
        };
        TdescrTemplate(){};

        sdds::Ttype type() override {return _type_id; };
        dtypes::uint8 valSize() override { return sizeof(Fvalue); }
        void* pValue() override { return &Fvalue; };

        bool setValue(const char* _str) override {
            if (_strToValue(_str,Fvalue)){
                signalEvents();
                return true;
            }
            return false;
        }

        TrawString to_string() override { return Tdescr::_valToStr(Fvalue); };

        inline ValType value(){return Fvalue; }

        operator ValType() const
        {
            return Fvalue;
        }

        /*
        ValType operator+(TdescrTemplate<ValType,_type_id>& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return Fvalue;
            return Fvalue + (*pVal);
        }
        */

        template <typename T>
        void operator++(T) { Fvalue++; signalEvents(); };

        template <typename T>
        void operator--(T) { Fvalue--; signalEvents(); };

        void operator+=(TdescrTemplate<ValType,_type_id>& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return;
            Fvalue += (*pVal);
            signalEvents();
        }

        template <typename T>
        void operator+=(T _inp){ Fvalue += _inp; signalEvents(); }

        void operator-=(TdescrTemplate<ValType,_type_id>& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return;
            Fvalue -= (*pVal);
            signalEvents();
        }

        template <typename T>
        void operator-=(T _inp){ Fvalue -= _inp; signalEvents(); }

        void operator*=(TdescrTemplate<ValType,_type_id>& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return;
            Fvalue *= (*pVal);
            signalEvents();
        }
        template <typename T>
        void operator*=(T _inp){ Fvalue *= _inp; signalEvents(); }

        template <typename T>
        void operator<<=(T _inp){ Fvalue <<= _inp; signalEvents(); }

        template <typename T>
        void operator>>=(T _inp){ Fvalue >>= _inp; signalEvents(); }

        bool operator==(TdescrTemplate<ValType,_type_id>& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return false;
            return (Fvalue==(*pVal));
        }

        //= operator will not be inherited!!!
        void operator=(ValType _value){
            Fvalue=_value;
            signalEvents();
        }
};


/************************************************************************************
Enums
*************************************************************************************/

class TenumBase : public Tdescr{
    public:
        TenumBase(){}
        virtual int enumCnt() { return 0; };
        virtual const char* getEnum(int _idx) { return ""; };
};

template <typename ValType, sdds::Ttype _type_id=sdds::Ttype::ENUM> class TenumTemplate: public TenumBase{
    private:
    protected:
        ValType Fvalue;
    public:
        friend class TmenuHandle;

        typedef ValType enumClass;
        typedef typename ValType::e dtype;
        typedef typename ValType::e e;

        /* override copy constructor to avoid a deep copy of the object
         * including callbacks and events
        */
        TenumTemplate(const TenumTemplate& _src) : TenumTemplate(){
            Fvalue = _src.Fvalue;
        };
        TenumTemplate(){}

        sdds::Ttype type() override {return _type_id; };
        dtypes::uint8 valSize() override { return sizeof(dtype); }
        void* pValue() override { return &Fvalue.Fvalue; };

        int enumCnt() override { return ValType::COUNT; };
        const char* getEnum(int _idx) override { return Fvalue.getEnum(_idx); }

        bool setValue(const char* _str) override {
            if (Fvalue.strToVal(_str)){
                signalEvents();
                return true;
            }
            return false;
        }

        TrawString to_string() override { return Fvalue.c_str(); };

        inline ValType value(){ return Fvalue; }

        // behave like a plain class enum
        operator dtype() const{ return Fvalue.Fvalue; }

        void operator=(dtype _value){
            Fvalue=_value;
            signalEvents();
        }

        // behave like an extended created by macro magic
        operator ValType() const { return Fvalue; }
};

#define __sdds_namedEnum(_name, ...) \
    ENUM_CLASS(_name,__VA_ARGS__);\
    typedef TenumTemplate<_name>

//do not use anymore use sdds_enum
#define ENUM(...) __sdds_namedEnum(ENUM_UNIQUE_NAME(),__VA_ARGS__)
#define sdds_enum(...) __sdds_namedEnum(ENUM_UNIQUE_NAME(),__VA_ARGS__)

//do not use anymore... use enum::e
#define ENUMS(_enum) _enum::dtype


/************************************************************************************
Tstring
*************************************************************************************/

class Tstring : public TdescrTemplate<dtypes::string,sdds::Ttype::STRING>{
    typedef const char* Tcstr;
    public:
        typedef dtypes::string ValType;

        dtypes::int32 length() { return Fvalue.length(); }

        bool operator==(Tstring& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return false;
            return (Fvalue==(*pVal));
        }
        bool operator==(const char* _inp){ return (Fvalue==_inp); }

        bool operator!=(Tstring& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return false;
            return (Fvalue!=(*pVal));
        }
        bool operator!=(const char* _inp){ return (Fvalue!=_inp); }
        
        void operator+=(Tstring& _inp){
            ValType* pVal = static_cast<ValType*>(_inp.pValue());
            if (!pVal) return;
            Fvalue += (*pVal);
            signalEvents();
        }

        template <typename T>
        void operator+=(T _inp){ Fvalue += _inp; signalEvents(); }

        const char* c_str() { return Fvalue.c_str(); }
        
        operator ValType() { return Fvalue; }
        operator Tcstr(){ return c_str(); }
};


/************************************************************************************
finally instantiate real datatypes to be used outside of this unit
*************************************************************************************/

//unsigned integers
typedef TdescrTemplate<dtypes::uint8,sdds::Ttype::UINT8> Tuint8;
typedef TdescrTemplate<dtypes::uint16,sdds::Ttype::UINT16> Tuint16;
typedef TdescrTemplate<dtypes::uint32,sdds::Ttype::UINT32> Tuint32;

//signed integers
typedef TdescrTemplate<dtypes::int8,sdds::Ttype::INT8> Tint8;
typedef TdescrTemplate<dtypes::int16,sdds::Ttype::INT16> Tint16;
typedef TdescrTemplate<dtypes::int32,sdds::Ttype::INT32> Tint32;

//floating point
typedef TdescrTemplate<dtypes::float32,sdds::Ttype::FLOAT32> Tfloat32;

typedef TdescrTemplate<dtypes::TdateTime,sdds::Ttype::TIME> Ttime;
//typedef TdescrTemplate<dtypes::string,sdds::Ttype::STRING> Tstring;

//composed types
typedef TdescrTemplate<TmenuHandle*,sdds::Ttype::STRUCT> Tstruct;


/************************************************************************************
macros for menu definition... Example below

class TuserStruct : public TmenuHandle{
  public:
    sdds_struct(
      sdds_var(Tuint8, val1, sdds_joinOpt(sdds::opt::readonly), 0);       //with option and default value
      sdds_var(Tuint8, val2, sdds_joinOpt(sdds::opt::saveval);            //with option
      sdds_var(Tuint8, val3);
    )
} userStruct;

*************************************************************************************/

class TstartMenuDefinition{
    public:
    TstartMenuDefinition();
};

class TfinishMenuDefinition{
    public:
    TfinishMenuDefinition();
};

#define __sdds_expandOption(_o)|_o

/**
 * The purpose of the typedef in line 1 is only to get rid of the confusing
 * error messages when using declare with a none existing class. With this
 * typedef, it shows the correct line and says _class does not name a type and
 * usually suggests the right one... this is what we want!
*/
#define __sdds_declareField(_class, _name, _option, _value, _constructorAssign) \
    typedef _class _class##_##_name##_type;\
    class _class##_##_name : public _class{\
        public:\
            using Tdescr::signalEvents;\
            _class##_##_name(){\
                _constructorAssign(_value)\
            }\
            sdds::Toption option() override { return _option; }\
            const char* name() override { return #_name; }\
            void operator=(_class::dtype _v){\
                Fvalue = _v;\
                signalEvents();\
            }\
    } _name;

/**
 * macros to change operator= and constructor based on the number of arguments given
 */
#define __sdds_constructorAssignValue(_value) Fvalue = _value;
#define __sdds_constructorEmpty(_value)

/**
 * implement variable number of arguments for sdds_var
 */
#define __sdds_declareField_param1(_class) static_assert(false,"Need At Least 2 Parameters" );     //throw error for 1 and 0 parameters to sdds_var
#define __sdds_declareField_param2(_class,_var) __sdds_declareField(_class,_var,0,0,__sdds_constructorEmpty);
#define __sdds_declareField_param3(_class,_var,_opt) __sdds_declareField(_class,_var,_opt,0,__sdds_constructorEmpty)
#define __sdds_declareField_param4(_class,_var,_opt,_value) __sdds_declareField(_class,_var,_opt,_value,__sdds_constructorAssignValue);
#define __sdds_get5thArg(_1,_2,_3,_4,_N,...) _N
#define __sdds_getMacro(...) __sdds_get5thArg(__VA_ARGS__,__sdds_declareField_param4,__sdds_declareField_param3,__sdds_declareField_param2,__sdds_declareField_param1)

/**
 * to be used outside of this unit
 */
#define sdds_struct(...) TstartMenuDefinition __firstVar; __VA_ARGS__ TfinishMenuDefinition __lastVar;
#define sdds_joinOpt(...) 0 SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(__sdds_expandOption,__VA_ARGS__)
#define sdds_var(...) __sdds_getMacro(__VA_ARGS__)(__VA_ARGS__)


/************************************************************************************
TobjectEvent
*************************************************************************************/

class TobjectEvent;
class TobjectEventList;

class __TobjectEvent : public Tevent{
    void afterDispatch() override;
public:
    using Tevent::Tevent;
};

typedef dtypes::uint8 TrangeItem;
constexpr dtypes::uint8 TrangeItem_max = 255;

class TobjectEvent : public TlinkedListElement{
    friend class __TobjectEvent;
    friend class TobjectEventList;
    private:
        __TobjectEvent Fevent;
        const char* Fname;
        TrangeItem Ffirst;
        TrangeItem Flast;
    public:
        TmenuHandle* Fstruct;
        dtypes::uint8 Ftag;

        void afterDispatch();

        TmenuHandle* menuHandle() { return Fstruct; }
        TrangeItem first() { return Ffirst; }
        TrangeItem last() { return Flast; }
        Tevent* event();
        void signal(TrangeItem _first = 0, TrangeItem _last = TrangeItem_max);

        TobjectEvent(Tthread* _owner, const char* _name);
};



class TobjectEventList : public TlinkedList<TobjectEvent>{
    public:
        void signal(){
            for (auto it = iterator(); it.hasNext(); ){
                it.next()->event()->signal();
            }
        }
        void signal(TrangeItem _first);
};


/************************************************************************************
TmenuHandle - base class to be used to declare a structure with descriptive elements
*************************************************************************************/

/**
 */
class TmenuHandle : public Tstruct{
    public:
        typedef TlinkedListIterator<Tdescr> Titerator;
    private:
        TlinkedList<Tdescr> FmenuItems;
        TobjectEventList FobjectEvents;
        void push_back(Tdescr* d){FmenuItems.push_back(d);}
    public:
        TmenuHandle();

        TlinkedListIterator<Tdescr> iterator() { return FmenuItems.iterator(); }
        TlinkedListIterator<Tdescr> iterator(TrangeItem _first) { return FmenuItems.iterator(_first); }

        TobjectEventList* events() { return &FobjectEvents; }

        void* pValue() override { return nullptr; };

        void signalEvents(Tdescr* _sender){
            if (events()->hasElements()){
                int idx = FmenuItems.indexOf(_sender);
                if (idx > -1){
                    events()->signal(idx);
                }
            }
        }

        //called by all overloaded constructors through addDescr(_descr,_name,_opt) anyway
        void addDescr(Tdescr* _descr){
            _descr->Fparent = this;
            push_back(_descr);
        }

        void print();

        Tdescr* find(const char* _name);

        template <class _TstringRef>
        Tdescr* find(_TstringRef _name){
            for (auto it = FmenuItems.iterator(); it.hasNext();){
                auto descr = it.next();
                if (_name == descr->name()){
                    return descr;
                };
            }
            //throw Exception(_name + " not found");
            return nullptr;
        }

        #if (MARKI_DEBUG_PLATFORM == 1)
        void log(dtypes::string _pre = ""){
            for (auto it=iterator(); it.hasNext();){
                auto d = it.next();
                if (d->isStruct()){
                    TmenuHandle* mh1 = static_cast<Tstruct*>(d)->value();
                    debug::log("%s-> %s",_pre.c_str(),d->name());
                    mh1->log(_pre + "  ");
                    debug::log("%s<- %s",_pre.c_str(),d->name());
                } else {
                    debug::log("%s%s=%s",_pre.c_str(),d->name(),d->to_string().c_str());
                }
            }
        }
        void _logCreateAssoc(){
            for (auto it=iterator(); it.hasNext();){
                auto d = it.next();
                debug::log("  case(%d): return \"%s\";",d->createNummber,d->name());
                if (d->isStruct()){
                    TmenuHandle* mh1 = static_cast<Tstruct*>(d)->value();
                    mh1->_logCreateAssoc();
                }
            }
        }
        void logCreateAssoc(){
            debug::log("switch(createNumber){");
            debug::log("  case(0): return \"root\";");
            _logCreateAssoc();
            debug::log("}");
        }
        #endif
        
};


/************************************************************************************
Ttimer
*************************************************************************************/

void handleTimerEvent(Tevent* _timerEv);

class Ttimer : public Tevent{
    typedef dtypes::TsystemTime Ttime;
    public:
        Tcallbacks Fcallbacks;
        Ttimer() : Tevent(&handleTimerEvent) {};

        void start(Ttime _waitTime){ setTimeEvent(_waitTime); }
        void stop(){ Tevent::reclaim(); }
        bool running(){ return linked(); }
        void onTimerElapsed(){ Fcallbacks.emit(nullptr); }
};


/************************************************************************************
Should be moved to another unit later on
*************************************************************************************/

template <class TstringRef>
class Tokenizer{
    private:
        TstringRef& Fstr;
        char Fcurr = '\0';
    public:
        Tokenizer(TstringRef& _str):Fstr(_str){}

        inline bool hasNext(){ return (Fstr.hasNext() && (Fcurr != '=')); }
        inline bool isLast(){ return !hasNext(); }
        inline char curr() { return Fcurr; }

        TsubStringRef next(){
            const char* pStart = Fstr.pCurr();
            while (Fstr.hasNext()){
                char c = Fstr.next();
                Fcurr = c;
                if ((c == '/') || (c == '.') || (c=='=') || (c=='?')){
                    return TsubStringRef(pStart,Fstr.pCurr()-1);
                }
            }
            return TsubStringRef(pStart,Fstr.pCurr());
        }
};

class Tlocator{
    private:
        TmenuHandle* Froot = nullptr;
        Tdescr* Fresult = nullptr;
    public:
        Tlocator(TmenuHandle* _root){
            Froot = _root;
        }
        Tlocator(TmenuHandle& _root){
            Froot = &_root;
        }

        inline Tdescr* result(){return Fresult; };

        bool locate(const char* _path){
            TstringRef path(_path);
            return locate(path);
        }

        template <class _TstringRef>
        bool locate(_TstringRef& _path){
            TmenuHandle* parent = Froot;
            for (auto t = Tokenizer<_TstringRef>(_path); t.hasNext();)
            {
                auto token = t.next();
                Tdescr* d = parent->find(token);
                if (!d) {return false;};
                Fresult = d;
                if (d->isStruct()){
                    parent = static_cast<Tstruct*>(d)->value();
                    if (t.isLast()) {return true; }
                }
                else{
                    if (!t.isLast()){return false;}
                    return true;
                }
            }
            return false;
        }
};

class TsetupTimer : public Ttimer{
    public: TsetupTimer() { start(0); }
};

namespace sdds{
    static Ttimer& setup(){
        static Ttimer __setup;
        __setup.start(0);
        return __setup;
    }

    static TsetupTimer init;
}

#endif
