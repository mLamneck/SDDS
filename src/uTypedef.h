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

#ifndef __SDDS_UTYPEDEF_COMPILE_STRCONV
	#define __SDDS_UTYPEDEF_COMPILE_STRCONV 1
#endif

#ifdef SDDS_ON_AVR
    //functional not available in AVR-gcc
#else
	#include <functional>
#endif


/************************************************************************************
macro expansion
*************************************************************************************/

#define __SDDS__TOKENPASTE(x, y) x ## y
#define __SDDS__TOKENPASTE2(x, y) __SDDS__TOKENPASTE(x, y)
#define SDDS_UNIQUE_NAME(_prefix) __SDDS__TOKENPASTE2(_prefix,__COUNTER__)

#ifdef SDDS_ON_AVR
    #define __sdds_storeCallback(_name) *_name = [](void* _self)
#else
    #define __sdds_storeCallback(_name) *_name = [=](void* _self)
#endif

/*
    old version: TcallbackWrapper* _name = new TcallbackWrapper(this);

    variant with static declaration of TcallbackWrapper. The only problem is if an object is 
    instantiated multiple times we have to detect that, and use dynamic memory allocation 
    for the second, third, ... object. This is done in addCbw.
*/
#define __sdds_namedOn(_name,_var) \
    static TcallbackWrapper _name(this);\
    TcallbackWrapper* __SDDS__TOKENPASTE(_name,_local) = _var.callbacks()->addCbw(_name);\
    __sdds_storeCallback(__SDDS__TOKENPASTE(_name,_local))

#define on(_var) __sdds_namedOn(SDDS_UNIQUE_NAME(__on_var),_var)

#define sdds_self(className) auto self = static_cast<className*>(_self)
#define sdds_ref(className) static_cast<className*>(_self)


/************************************************************************************
forward declarations and typedefinitions for this unit
*************************************************************************************/

class TmenuHandle;
class Tdescr;

//callback for ons
#ifdef __AVR__
    //for AVR-gcc std::function is not available
    //we have to store a pure funcion ptr instead
    typedef void (*Tcallback)(void*);
#else
    typedef std::function<void(void* p)> Tcallback;
#endif


namespace sdds{
    typedef uint8_t Toption;
    
    namespace opt{
		typedef dtypes::uint8 Ttype;

        constexpr Toption nothing   = 0;
        constexpr Toption readonly  = 0x01;
        constexpr Toption saveval   = 0x80;


        constexpr Toption mask_show 	= 0x0E;
        constexpr Toption showHex 		= 0x04;
        constexpr Toption showBin 		= 0x06;
        constexpr Toption showString	= 0x08;

        constexpr Toption timeRel   	= 0x02;
        constexpr Toption timeAbs   	= 0x00;
    }

    namespace typeIds{
		typedef dtypes::uint8 Ttype;

        constexpr uint8_t size_mask = 0x0F; 
		constexpr uint8_t first_compose_type = 0x42;
		constexpr uint8_t fARRAY = 0x80;

        constexpr uint8_t time = 0x06; 
    }

    enum class Ttype : dtypes::uint8{
        UINT8   = 0x01,
        UINT16  = 0x02,
        UINT32  = 0x04,

        TIME    = typeIds::time,

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
        Tcallback Fcallback = nullptr;
        void* Fctx = nullptr; 
    public:
        TcallbackWrapper(void* _ctx){
            Fctx = _ctx;
        }

        void* ctx() { return Fctx; }

        inline TcallbackWrapper* operator=(Tcallback _cb){
            Fcallback = _cb;
            return this;
        }

        void emit(){
            if (Fcallback){
                Fcallback(Fctx);
            }
        }
};
typedef TlinkedListIterator<TcallbackWrapper> TcallbackIterator;

class Tcallbacks : public TlinkedList<TcallbackWrapper>{
    public:
        inline void emit(){
            for (auto it = iterator(); it.hasNext();){
                it.next()->emit();
            }
        }

        TcallbackWrapper* addCbw(TcallbackWrapper* _cbw){
            /*
                here we have to check if the given callback wrapper is already in use,
                and if so we have to dynamically allocate one. This happens if a sdds_struct
                is instantiated multiple times
            */
            if (_cbw->linked()){
                _cbw = new TcallbackWrapper(_cbw->ctx());
            }
            push_back(_cbw);
            return _cbw;
        }

        TcallbackWrapper* addCbw(TcallbackWrapper& _cbw){ return addCbw(&_cbw); }
};


/************************************************************************************
Tdescr - abstract class for all types
*************************************************************************************/

class Tdescr : public TlinkedListElement{
	typedef dtypes::TdateTime TdateTime;
    friend class TmenuHandle;

    private:
        TmenuHandle* Fparent = nullptr;
	protected:
        Tcallbacks Fcallbacks;

		constexpr sdds::opt::Ttype __modifyOption(const sdds::opt::Ttype _opt) { return _opt; }

    public:
        #if MARKI_DEBUG_PLATFORM == 1
        int createNummber;
        #endif
        
        Tdescr();

		Tdescr* next() { return static_cast<Tdescr*>(TlinkedListElement::next()); }
		
        Tcallbacks* callbacks(){ return &Fcallbacks; }
        void signalEvents();

        TmenuHandle* findRoot();

        //providing type information
        virtual sdds::Ttype type() = 0;
        virtual sdds::Toption option(){return 0; };
        virtual const char* name(){return ""; };

        dtypes::uint8 typeId(){ return static_cast<dtypes::uint8>(type()); }
        virtual dtypes::uint8 valSize() { return static_cast<uint8_t>(type()) & sdds::typeIds::size_mask; }
		
        virtual void* pValue() { return nullptr; };

        sdds::Toption showOption(){ return (option() & sdds::opt::mask_show); }
        inline bool saveval() { return ((option() & sdds::opt::saveval) > 0); }
        inline bool readonly() { return ((option() & sdds::opt::readonly) > 0); }

        //propably compare default value to current and only save if different
        inline bool shouldBeSaved(){ return saveval(); }

        inline bool isStruct() { return (type()==sdds::Ttype::STRUCT); }
        inline bool isArray() { return (typeId() & sdds::typeIds::fARRAY) > 0; }
		template <typename T>
		T* as() {
			static_assert(!std::is_base_of<TmenuHandle,T>::value,"hkjh"); 
			static_assert(std::is_base_of<Tdescr, T>::value,
						"T must be derived from Tdescr");
			return static_cast<T*>(this);
		}
		template <typename T>
		typename std::enable_if<std::is_base_of<Tdescr, T>::value, T*>::type
		as1() {
			return nullptr;
		}

        inline bool hasChilds() {
            if (!isStruct()) return false;
        }

        //interface for generic handling
        virtual bool setValue(const char* _str) = 0;
        virtual TrawString to_string(){ return ""; }

#if __SDDS_UTYPEDEF_COMPILE_STRCONV

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
            double d = 0;
            if (!Tdescr::_strToDouble(_str,d)) return false;
            _val = d;
            return true;
        }

        //********* convert internal values to string ***************

        template <typename valType>
        TrawString _valToStr(valType _val){return strConv::to_string(_val); }
        TrawString _valToStr(TdateTime _val){
            if (showOption() == sdds::opt::timeRel) return timeToString(_val,"%H:%M:%S");
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
#endif

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
        TdescrTemplate(const TdescrTemplate& _src){
            Fvalue = _src.Fvalue;
        };
        TdescrTemplate(){};

        sdds::Ttype type() override {return _type_id; };
		/**
		 * 27.10.2024 valSize has been changed because it gave wrong values for
		 * arrays i.e. when transfering a string, valSize gives 32. But what we
		 * want to know here is the size of the data to be transmitted which
		 * is 1 for a string. This is what valSize() from Tdescr give us.
		 * 
		 * hopefully this will not break something but I cannot figure out how,
		 * because we are not allowed to overwrite Fvalue for complex types 
		 * like strings or structs anyway.
		 */
        //dtypes::uint8 valSize() override { return sizeof(Fvalue); }
        dtypes::uint8 valSize() override { return (static_cast<uint8_t>(_type_id) & sdds::typeIds::size_mask); }
		void* pValue() override { return &Fvalue; };

        bool setValue(const char* _str) override {
#if __SDDS_UTYPEDEF_COMPILE_STRCONV
            if (_strToValue(_str,Fvalue)){
                signalEvents();
                return true;
            }
#endif
            return false;
        }

#if __SDDS_UTYPEDEF_COMPILE_STRCONV
        TrawString to_string() override { return Tdescr::_valToStr(Fvalue); };
#endif
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

		void __setValue(ValType _value){
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
		virtual uStrings::TstringArrayIterator iterator() = 0;
        virtual int enumCnt() { return 0; };
        virtual const char* getEnum(int _idx) { return ""; };
        virtual int enumBufferSize() { return 0; };
        virtual const char* enumBuffer() { return nullptr; };
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

		uStrings::TstringArrayIterator iterator() override { return Fvalue.iterator(); }
        int enumCnt() override { return ValType::COUNT; };
        const char* getEnum(int _idx) override { return Fvalue.getEnum(_idx); }
        int enumBufferSize() override { return Fvalue.enumBufferSize(); };
        const char* enumBuffer() override { return Fvalue.enumBuffer(); };

        bool setValue(const char* _str) override {
            if (Fvalue.strToVal(_str)){
                signalEvents();
                return true;
            }
            return false;
        }

        static const char* c_str(dtype _enum){ return ValType::c_str(_enum); };
        const char* c_str() { return ValType::c_str(Fvalue); }
        TrawString to_string() override { return Fvalue.c_str(); };

        inline ValType value(){ return Fvalue; }

        // behave like a plain class enum
        operator dtype() const{ return Fvalue.Fvalue; }

		void __setValue(dtype _value){
			Fvalue=_value;
            signalEvents();
		}
        void operator=(dtype _value){
			Fvalue=_value;
            signalEvents();
        }

        // behave like an extended created by macro magic
        operator ValType() const { return Fvalue; }
};

#define __sdds_namedEnum(_name, ...) \
    sdds_enumClass(_name,__VA_ARGS__);\
    typedef TenumTemplate<_name>

//do not use anymore use sdds_enum
#define ENUM(...) __sdds_namedEnum(ENUM_UNIQUE_NAME(),__VA_ARGS__)
#define sdds_enum(...) __sdds_namedEnum(ENUM_UNIQUE_NAME(),__VA_ARGS__)

//do not use anymore... use enum::e
#define ENUMS(_enum) _enum::dtype


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
			_constructorAssign(_class,_name,_value)\
			sdds::Toption option() override { return __modifyOption(_option); }\
			const char* name() override { return #_name; }\
			void operator=(_class::dtype _v){ __setValue(_v); }\
			template<typename T>\
			void operator=(T _val){__setValue(_val); }\
    } _name;

/**
 * macros to change operator= and constructor based on the number of arguments given
 */
#define __sdds_constructorAssignValue(_class,_name,_value)\
_class##_##_name(){\
	Fvalue = _value;\
}
#define __sdds_constructorEmpty(_class,_name,_value)

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
    TobjectEvent* FobjectEvent;
public:
    __TobjectEvent(Tthread* _owner, TobjectEvent* _oe) 
        : Tevent(_owner)
        ,FobjectEvent(_oe)
    {
    }
    TobjectEvent* objectEvent(){ return FobjectEvent; }
};

inline void arrayToDo(){

}

namespace sdds{
	//toDo: adjust TrangeItem according to platform to support maximum value
	//on platforms where 16bit values for TrangeItem are available, use 16 bit here
	//as we are not limited in memory there.
	typedef dtypes::uint8 TrangeItem;
	constexpr static dtypes::uint8 TrangeItem_max = dtypes::high<TrangeItem>();

	struct Trange {
		TrangeItem Ffirst;
		TrangeItem Flast;

		Trange(TrangeItem _first, TrangeItem _last){
			Ffirst = _first;
			Flast = _last;
		}

		Trange() : Trange(0,TrangeItem_max){
		
		}

		bool intersection(const Trange& _r) {
			TrangeItem newFirst = std::max(Ffirst, _r.Ffirst);
			TrangeItem newLast = std::min(Flast, _r.Flast);
			if (newFirst > newLast) return false;

			Ffirst = newFirst;
			Flast = newLast;
			return true;
		}
	};
}

class TobjectEvent : public TlinkedListElement{
	typedef sdds::Trange Trange;
	typedef sdds::TrangeItem TrangeItem;
	 
    friend class __TobjectEvent;
    friend class TobjectEventList;
    private:
        __TobjectEvent Fevent;

		Tdescr* FobservedObj;
		Trange FobservedRange;
		Trange FchangedRange;
    public:
        //TmenuHandle* Fstruct;

        void afterDispatch();

		/**
		 * at the moment only used by plainCommHandler
		 * 
		 *  
		 */
	    TmenuHandle* menuHandle1() { return static_cast<Tstruct*>(FobservedObj)->value(); }
		Tdescr* observedObj() { return FobservedObj; }
		void setObservedObj(Tdescr* _oo){ FobservedObj = _oo; }

	    TrangeItem first() { return FchangedRange.Ffirst; }
        TrangeItem last() { return FchangedRange.Flast; }
        Tevent* event();
        void signal(TrangeItem _first = 0, TrangeItem _last = sdds::TrangeItem_max);

		void cleanup();

		template <class _Tlocator>
		void setObservedRange(_Tlocator& _l){
			arrayToDo();
			FobservedObj = _l.parent();
			FobservedRange.Ffirst = _l.firstItemIdx();
			FobservedRange.Flast = _l.lastItemIdx();
		}

		void setOwner(Tthread* _thread){ Fevent.setOwner(_thread); }
		TobjectEvent(Tthread* _owner) : Fevent(_owner,this){
			afterDispatch();
		}
		TobjectEvent() : TobjectEvent(nullptr) {};

        static TobjectEvent* retrieve(Tevent* _ev){
            auto __oe = static_cast<__TobjectEvent*>(_ev);
            return __oe->objectEvent();
        }
};

class TobjectEventList : public TlinkedList<TobjectEvent>{
	typedef TobjectEvent::TrangeItem TrangeItem;

    public:
        void signal(){
            for (auto it = iterator(); it.hasNext(); ){
                it.next()->event()->signal();
            }
        }
        void signal(TrangeItem _first, int _n = 1, dtypes::uint16 _port = 0);
};


/************************************************************************************
Tstring
*************************************************************************************/

class Tarray{
	public:
	    TobjectEventList FobjectEvents;
};

template <class ValType, sdds::Ttype _type_id> class Tarray1: public TdescrTemplate<ValType,_type_id>{
	public:
	    TobjectEventList FobjectEvents;
};

//class Tstring : public TdescrTemplate<dtypes::string,sdds::Ttype::STRING>, public Tarray{
class Tstring : public Tarray1<dtypes::string,sdds::Ttype::STRING>{
    typedef const char* Tcstr;
	protected:
		constexpr sdds::opt::Ttype __modifyOption(const sdds::opt::Ttype _opt) { return _opt | sdds::opt::showString; }
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

		void setValue(TsubStringRef& _strRef){
			Fvalue.assign(_strRef.c_str(),_strRef.length());
			signalEvents();
		}

		void signalEvents(){
			Tdescr::signalEvents();
			FobjectEvents.signal(0,255);
		}

		template<typename T>
		void __setValue(T _val){
			Fvalue = _val;
			signalEvents();
		}
};


/************************************************************************************
TmenuHandle - base class to be used to declare a structure with descriptive elements
*************************************************************************************/

/**
 */
class TmenuHandle : public Tstruct{
	typedef sdds::TrangeItem TrangeItem;

	friend class TobjectEvent;
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

        //void* pValue() override { return nullptr; };

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
        int find(_TstringRef _name, Tdescr*& _descr){
        		_descr = nullptr;
        		int idx = 0;
        		for (auto it = FmenuItems.iterator(); it.hasNext(); idx++){
        			auto descr = it.next();
        			if (_name == descr->name()){
        				_descr = descr;
        				return idx;
        			};
        		}
        		//throw Exception(_name + " not found");
        		return -1;
        }

        template <class _TstringRef>
        Tdescr* find(_TstringRef _name){
			Tdescr* descr;
			find(_name,descr);
			return descr;
        }

		Tdescr* get(int _idx){
			Tdescr* res = nullptr;
			auto it = iterator();
			while (_idx-- >= 0){
				if (!it.hasNext()) return nullptr;
				res = it.next();
			}
			return res;
		}

		Tdescr* last(){
			Tdescr* res = nullptr;
			auto it = iterator();
			while (it.hasNext()) res = it.next();
			return res;
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

template <class eventType>
class TcallbackEvent: public eventType{
    typedef dtypes::TsystemTime Ttime;
    Tcallbacks Fcallbacks;
    void execute() override{ Fcallbacks.emit(); }
    public:
        TcallbackEvent(): eventType() {}
        Tcallbacks* callbacks(){ return &Fcallbacks; }
};

class Ttimer: public TcallbackEvent<Tevent>{
    typedef dtypes::TsystemTime Ttime;
    public:
        void start(Ttime _waitTime){ setTimeEvent(_waitTime); }
        void stop(){ Tevent::reclaim(); }
        bool running(){ return linked(); }
};

typedef TcallbackEvent<multask::TisrEvent> TisrEvent;

template <uint8_t qsize, typename dtype=dtypes::uint8>
class TisrEventDataQ : public TcallbackEvent<multask::TisrEventDataQ<qsize,dtype>>{};


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
                if ((c == '/') || (c=='-') || (c == '.') || (c=='=') || (c=='?')){
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

/**
 * @brief Used to find elements in a sdds tree for a given binary path of
 * 	of the the form |length|entry0|...|entryN|nItems|  
 * 
 * @note use locate to resolve the given path. If locate returns true
 * 	the path is valid and as a result we get:
 * 		parent: The last struct/array entered.
 * 		if parent is a struct:
 * 			firstItem: the first item in the struct
 * 			lastItem: the last item in the struct
 * 			firstItemIdx: the numeric idx of the first itme
 * 			lastItemIdx: the numeric idx of the last itme
 * 		if parent is an array:
 * 			firstItem, lastItem: the same as parent
 * 			firstItemIdx: index to the first array element
 * 			lastItemIdx: index to the last array element
 * 
 * @tparam t_path_length data type of a path entry (uint8 or uint16)
 * @tparam t_path_entry data type of the path length (uint8 or uint16)
 */
template<typename t_path_length, typename t_path_entry>
class TbinLocator{
    private:
		Tdescr* Fparent = nullptr;

		Tdescr* FfirstItem = nullptr;
		Tdescr* FlastItem = nullptr;
		t_path_entry FfirstItemIdx = 0;
		t_path_entry FlastItemIdx = 0;
	public:
		static int constexpr MAX_ENTRY = dtypes::high<t_path_entry>();

		Tdescr* parent() { return Fparent; }
		TmenuHandle* menu1() {
			if (!Fparent->isStruct()) return nullptr;
			return static_cast<Tstruct*>(Fparent)->value(); 
		}
		Tdescr* firstItem() { return FfirstItem; }
		Tdescr* lastItem() { return FlastItem; }
		t_path_entry firstItemIdx() { return FfirstItemIdx; }
		t_path_entry lastItemIdx() { return FlastItemIdx; }
		
		template <class TmemoryStream>
        bool locate(TmemoryStream& _ms, TmenuHandle* _root){
			t_path_length length = 0;
			t_path_entry entry = 0;
			if (!_ms.readVal(length)) return false;
			if (length < 2) return false;

			Tdescr* parentDescr = _root;
            TmenuHandle* parent = _root;
			while (length-- > 0){
				if (!_ms.readVal(entry)) return false;
				Tdescr* d;
				if (length > 1){
					d = parent->get(entry);
					if (!d) return false;
					if (d->isStruct()){
						parentDescr = d;
						parent = static_cast<Tstruct*>(d)->value();
					}
					else if (d->isArray()){
						if (length > 2) return false;
						Fparent = d;
						FfirstItem = d;
						FlastItem = d;
						if (!_ms.readVal(FfirstItemIdx)) return false;
						if (!_ms.readVal(entry)) return false;
						if (entry < MAX_ENTRY){
							if (dtypes::uint32(entry + FfirstItemIdx - 1) > MAX_ENTRY) return false;
							entry = entry + FfirstItemIdx - 1;
						} 
						FlastItemIdx = entry;
						return true;
					}
					else
						return false;
				}
				else if (length == 1){
					d = parent->get(entry);
					if (!d) return false;
					FfirstItemIdx = entry;
					FfirstItem = d;
				}
				else{
					if (entry < MAX_ENTRY){
						if (dtypes::uint32(entry + FfirstItemIdx - 1) > MAX_ENTRY) return false;
						entry = entry + FfirstItemIdx - 1;
						d = parent->get(entry);
					} 
					else
						d = parent->last();
					if (!d) return false;
					FlastItemIdx = entry;
					FlastItem = d;
				}
			}
			Fparent = parentDescr;
            return true;
        }
};

/*
    declare some global system events here
    toDo: 
        -piorities!?
            what if we need a specific order of execution i.e.
                init Wifi fist
                than init webserver
*/
namespace sdds{
    Ttimer& setup();
}

#endif
