#ifndef UTYPEDEF_H
#define UTYPEDEF_H

#include "uPlatform.h"
#include "uMultask.h"
#include "uLinkedList.h"
#include "uStrings.h"
#include "uObjectEvents.h"
#include "uEnumMacros.h"

#ifdef __AVR__
    //functional not available in AVR-gcc
#else
	#include <functional>
#endif


/************************************************************************************
consts for Tdescr type and option
*************************************************************************************/

#define OPT_NO          0x00
#define OPT_fREADONLY   0x01
#define OPT_fSAVEVAL    0x80

#define TYPE_INT        0x01
#define TYPE_FLOAT      0x02
#define TYPE_STR        0x03
#define TYPE_STRUCT     0x42


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

//types used in Tdescr
typedef uint8_t Ttype_id;
typedef uint8_t Toption;

//callback for ons
#ifdef __AVR__
    //for AVR-gcc std::function is not available
    //we have to store a pure funcion ptr instead
    typedef void (*Tcallback)(Tdescr*);
#else
    typedef std::function<void(Tdescr* p)> Tcallback;
#endif


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
        Toption Foption = 0;
        TmenuHandle* Fparent = nullptr;

    protected:
        const char* Fname = nullptr;
        void signalEvents();
    public:
        constexpr static bool is_struct = false;
        Tdescr(int id);
        Tcallbacks Fcallbacks;
        friend class TmenuHandle;

        //providing type information
        virtual Ttype_id typeId() = 0;
        virtual const char* name(){return Fname; };
        inline Toption option(){return Foption; };

        virtual bool isEnum() { return false; };
        inline bool isStruct() {return (typeId()==TYPE_STRUCT); };

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
            return !(*pEnd);
        }
        template<typename ValType> static bool _strToNumber(const char* _str, ValType& _val){
            double d;
            bool result = Tdescr::_strToDouble(_str,d);
            _val = d;
            return result;
        }

        //convert internal values to string
        template <typename valType>
        static TrawString _valToStr(valType _val){return strConv::to_string(_val); }
        static TrawString _valToStr(TrawString _val){return _val; }
        static TrawString _valToStr(TmenuHandle* _val){ return ""; };

        //covert string to internal values
        static bool _strToValue(const char* _str, int& _value){return Tdescr::_strToNumber<int>(_str,_value);}
        static bool _strToValue(const char* _str, double& _value){return Tdescr::_strToNumber<double>(_str,_value);}
        static bool _strToValue(const char* _str, dtypes::float32& _value){return Tdescr::_strToNumber<dtypes::float32>(_str,_value);}
        static bool _strToValue(const char* _str, dtypes::uint8& _value){return Tdescr::_strToNumber<dtypes::uint8>(_str,_value);}
        static bool _strToValue(const char* _str, TmenuHandle*& _value){return true;}
        template <typename T> static bool _strToValue(const char* _str, T& _value){return true;}
};


/************************************************************************************
template for the actual descriptive types
*************************************************************************************/

template <class ValType, Ttype_id _type_id> class TdescrTemplate: public Tdescr{
    private:
    protected:
        ValType Fvalue;
    public:
        constexpr static bool is_struct = (_type_id==0x42);
        friend class TmenuHandle;
        typedef ValType dtype;

        /* override copy constructor to avoid a deep copy of the object
         * including callbacks and events
        */
        TdescrTemplate(const TdescrTemplate& _src) : TdescrTemplate(){
            Fvalue = _src.Fvalue;
        };
        TdescrTemplate() : Tdescr(_type_id){};

        Ttype_id typeId() override {return _type_id; };

        bool setValue(const char* _str) override {
            if constexpr (is_struct){
                return true;
            }else{
                if (_strToValue(_str,Fvalue)){
                    signalEvents();
                    return true;
                }
                return false;
            }
        }

        TrawString to_string() override { return Tdescr::_valToStr(Fvalue); };

        inline ValType value(){return Fvalue; }

        operator ValType() const
        {
            return Fvalue;
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
    bool isEnum() override{ return true; }
    public:
        TenumBase() : Tdescr(77) {}
        virtual int enumCnt() { return 0; };
        virtual const char* getEnum(int _idx) { return ""; };
};

template <typename ValType, Ttype_id _type_id=0x01> class TenumTemplate: public TenumBase{
    private:
    protected:
        ValType Fvalue;
    public:
        friend class TmenuHandle;
        typedef ValType dtype;
        typedef typename ValType::e e;

        /* override copy constructor to avoid a deep copy of the object
         * including callbacks and events
        */
        TenumTemplate(const TenumTemplate& _src) : TenumTemplate(){
            Fvalue = _src.Fvalue;
        };
        TenumTemplate(){}

        Ttype_id typeId() override {return _type_id; };
        int enumCnt() override { return ValType::COUNT; };
        const char* getEnum(int _idx) override { return ValType::ENUM_STRS[_idx]; };

        void setValue(ValType _val){ Fvalue = _val; }

        bool setValue(const char* _str) override {
            if (Fvalue.strToVal(_str)){
                signalEvents();
                return true;
            }
            return false;
        }

        TrawString to_string() override { return Fvalue.to_string(); };

        inline ValType value(){ return Fvalue; }

        // behave like a plain class enum
        operator e() const{ return Fvalue.Fvalue; }
        void operator=(e _value){
            Fvalue=_value;
            signalEvents();
        }

        // behave like an extended created by macro magic
        operator ValType() const { return Fvalue; }
};

#define ENUM(...) \
    ENUM_CLASS(ENUM_UNIQUE_NAME(),__VA_ARGS__);\
    typedef TenumTemplate<ENUM_UNIQUE_NAME()>

#define ENUMS(_enum) _enum::dtype::e


/************************************************************************************
finally instantiate real datatypes to be used outside of this unit
*************************************************************************************/

//unsigned integers
typedef TdescrTemplate<dtypes::uint8,0x01> Tuint8;
typedef TdescrTemplate<dtypes::uint16,0x02> Tuint16;
typedef TdescrTemplate<dtypes::uint32,0x04> Tuint32;

//signed integers
typedef TdescrTemplate<dtypes::int8,0x11> Tint8;
typedef TdescrTemplate<dtypes::int16,0x12> Tint16;
typedef TdescrTemplate<dtypes::int32,0x14> Tint32;

//floating point
typedef TdescrTemplate<dtypes::float32,0x24> Tfloat32;

//composed types
typedef TdescrTemplate<TmenuHandle*,TYPE_STRUCT> Tstruct;

#define sdds_expandOption(_o)|_o
#define sdds_opt(...) 0 SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(sdds_expandOption,__VA_ARGS__)

/**
 * It's no problem to redefine __firstVar/__lastVar in a derived class
 */
#define sdds_struct(...) TstartMenuDefinition __firstVar; __VA_ARGS__ TfinishMenuDefinition __lastVar;

/**
 * The purpose of the typedef in line 1 is only to get rid of the confusing
 * error messages when using declare with a none existing class. With this
 * typedef, it shows the correct line and says _class does not name a type and
 * usually suggests the right one... this is what we want!
*/
#define declare(_class, _name, _option, _value) \
    typedef _class _class##_##_name##_type;\
    class _class##_##_name : public _class{\
        public:\
            _class##_##_name(){\
                if constexpr (!_class##_##_name::is_struct){\
                    Fvalue = _value;\
                }\
            }\
            Toption option() { return _option; }\
            const char* name() override { return #_name; }\
            void operator=(_class::dtype _v){\
                if constexpr (!_class##_##_name::is_struct){\
                    Fvalue = _v;\
                    signalEvents();\
                }\
            }\
    } _name;

#define declare_var(...) declare(__VA_ARGS__)


/************************************************************************************
TmenuHandle - base class to be used to declare a structure with descriptive elements
*************************************************************************************/

#define __expandPublish(_p) _publish(_p);
#define publish(...) SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(__expandPublish,__VA_ARGS__)

class TstartMenuDefinition{
    public:
    TstartMenuDefinition();
};

class TfinishMenuDefinition{
    public:
    TfinishMenuDefinition();
};

/**
 */
class TmenuHandle : public Tstruct{
    private:
        TlinkedList<Tdescr> FmenuItems;
        TobjectEventList FobjectEvents;
        void push_back(Tdescr* d){FmenuItems.push_back(d);}
    public:
        TmenuHandle();

        TlinkedListIterator<Tdescr> iterator() { return FmenuItems.iterator(); }

        TobjectEventList* events() { return &FobjectEvents; }

        //class T can be either a instance of Tdescr or a child of TmenuHandle
        template <class T>
        void _publish(T& _var){
            if constexpr (T::is_struct){
                Tstruct* s = &_var.s;
                addDescr(s);
                s->Fvalue = &_var;
            }
            else{
                addDescr(&_var);
            };
        }

        void addDescr(TmenuHandle* _s, const char* _name){
            Tstruct* s = new Tstruct();
            addDescr(s,_name,OPT_NO,_s);
        }

        //descr(val,"alias",OPT_fREADONLY)
        void addDescr(Tdescr* _descr, const char* _name, const char* _alias, Toption _opt){
            addDescr(_descr,_alias,_opt);
        }

        //descr(val,"alias",OPT_fREADONLY,1.9)
        template<class descrType, class valType>
        void addDescr(descrType* _descr, const char* _name, const char* _alias, Toption _opt, valType _value){
            addDescr(_descr,_name,_alias,_opt);
            _descr->Fvalue = _value;
        }

        //descr(val,OPT_fREADONLY,1.9)
        template<class descrType, class valType>
        void addDescr(descrType* _descr, const char* _name, Toption _opt, valType _value){
            addDescr(_descr,_name,_opt);
            _descr->Fvalue = _value;
        }

        //descr(val)
        void addDescr(Tdescr* _descr, const char* _name){
            addDescr(_descr, _name, OPT_NO);
        }

        //descr(val,OPT_fSAVEVAL)
        //called by all overloaded constructors
        void addDescr(Tdescr* _descr, const char* _name, Toption _opt){
            _descr->Fname = _name;
            _descr->Foption = _opt;
            addDescr(_descr);
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
};

class TmenuHandleAuto : public TmenuHandle{
    public:
    TmenuHandleAuto();
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
        void start(Ttime _waitTime){
            setTimeEvent(_waitTime);
        }
        void onTimerElapsed(){
            Fcallbacks.emit(nullptr);
        }
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
                if ((c == '/') || (c == '.') || (c=='=')){
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
            char tokenBuf[16];
            char* pToken = &tokenBuf[0];
            for (auto t = Tokenizer<_TstringRef>(_path); t.hasNext();)
            {
                auto token = t.next();
                token.copy(pToken);
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

#endif
