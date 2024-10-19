#ifndef UENUMMACROS_h
#define UENUMMACROS_h

#include "uSpookyMacros.h"

#define ENUM__TOKENPASTE(x, y) x ## y
#define ENUM__TOKENPASTE2(x, y) ENUM__TOKENPASTE(x, y)
#define ENUM_UNIQUE_NAME() ENUM__TOKENPASTE2(__Tenum__,__COUNTER__)

//#define ENUM_CASE(_enumStr) case e::_enumStr : return #_enumStr;
//#define ENUM_STR_CASE(_e) else if (strcmp(_str,#_e)==0){ Fvalue = ENUM_TYPE_NAME::_e; return true; }
//#define ENUM_STR(_e) #_e ,
#define ENUM_TYPE_NAME e
#define ENUM_STRS_NAME(_name) _name##ENUM_STRS
#define __ENUM_RAWTYPE_NAME(_name) _name##RAWTYPE
#define __SDDS_SM_ENUM_STR(_str) #_str "\0"

namespace sdds{
	namespace metaTypes{
		/**
		 * @brief Baseclass used in sdds_enumClass to reduce code produce by macro magic
		 * 
		 * @tparam t_enum a C++ enum class
		 * @tparam _COUNT number of elements in the enum class
		 */
		template <typename t_enum, int _COUNT>
		class TenumClaseBase{
			public:
				typedef uint8_t dtype;
				typedef t_enum ENUM_TYPE_NAME;                                                        
				constexpr static const dtype COUNT = _COUNT;                                          

				t_enum Fvalue;  

				operator ENUM_TYPE_NAME() const{return Fvalue;}                                       
				
				static const dtype ord(ENUM_TYPE_NAME _v){return static_cast<dtype>(_v);}

			protected:
				static const char* _c_str(ENUM_TYPE_NAME _v, const char* _enumArray){
					dtype idx = static_cast<dtype>(_v);
					if (idx < COUNT) return uStrings::getStringN(_enumArray,idx) ;
					return "unknown";
				}

				bool _strToVal(const char* _strArray, const char* _str){
					uStrings::TstringArrayIterator it(_strArray);
					for (int i=0; it.hasNext(); i++){
						if (strcmp(it.next(),_str) == 0){
							Fvalue = static_cast<t_enum>(i);
							return true;
						};
					}

					int val; TstringRef s(_str);
					if (!s.parseValue(val) || (s.hasNext())) return false;
					Fvalue = static_cast<t_enum>(val);
					return true;
				}
		};
	}
}

/**
 * @brief 
 * notes:
 * 	ENUM_STRS_NAME(_name): is a string containing all enums in form en1\0en2\0...enM\0\0 with an additional \0 at the
 * 		very end. That's why we return sizeof(ENUM_BUFFER)-1 in enumBufferSize
 */
#define ENUM_CLASS(_name,...)\
constexpr static const char ENUM_STRS_NAME(_name)[] = SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(__SDDS_SM_ENUM_STR,__VA_ARGS__);   \
enum class __ENUM_RAWTYPE_NAME(_name) : uint8_t {__VA_ARGS__};\
class _name : public sdds::metaTypes::TenumClaseBase<__ENUM_RAWTYPE_NAME(_name),SP_COUNT_VARARGS(__VA_ARGS__)>{                \
    public: \
		constexpr static dtype ENUM_BUFFER_SIZE = sizeof(ENUM_STRS_NAME(_name))-1; \
        void operator=(ENUM_TYPE_NAME _value){Fvalue = _value;}                                                        \
        static const char* c_str(ENUM_TYPE_NAME _v){ return _c_str(_v,ENUM_STRS_NAME(_name)); }\
        bool strToVal(const char* _str){ return _strToVal(ENUM_STRS_NAME(_name),_str); }\
		uStrings::TstringArrayIterator iterator() { return uStrings::TstringArrayIterator(ENUM_STRS_NAME(_name)); }\
		constexpr const char* enumBuffer(){ return ENUM_STRS_NAME(_name); }\
		constexpr int enumBufferSize(){ return ENUM_BUFFER_SIZE; }\
        const char* getEnum(int _idx) { return uStrings::getStringN(ENUM_STRS_NAME(_name),_idx); } \
        const char* c_str(){ return _name::c_str(Fvalue); }                                       \
};
#define sdds_enumClass ENUM_CLASS

/*
#define ENUM_CLASS2(_name,...)\
constexpr static const char* ENUM_STRS_NAME(_name)[] = {SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(ENUM_STR,__VA_ARGS__)};    \
class _name{                \
    typedef uint8_t dtype;\
    public: \
        constexpr static const dtype COUNT = SP_COUNT_VARARGS(__VA_ARGS__);                                          \
		constexpr static const char ENUM_BUFFER[] = SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(SDDS_SM_ENUM_STR,__VA_ARGS__);   \
		constexpr static dtype ENUM_BUFFER_SIZE = sizeof(ENUM_BUFFER); \
        enum class ENUM_TYPE_NAME : dtype {__VA_ARGS__};                                                             \
        ENUM_TYPE_NAME Fvalue;                                                                                         \
        operator ENUM_TYPE_NAME() const{return Fvalue;}                                                                \
        void operator=(ENUM_TYPE_NAME _value){Fvalue = _value;}                                                        \
        static const dtype ord(ENUM_TYPE_NAME _v){return static_cast<dtype>(_v);}\
        static const char* c_str(ENUM_TYPE_NAME _v){\
            dtype idx = static_cast<dtype>(_v);\
            if (idx < _name::COUNT) return ENUM_STRS_NAME(_name)[idx];\
            return "unknown";\
        }\
        bool tryParseNumber(const char* _str){\
            dtype val; TstringRef s(_str);\
            if (!s.parseValue(val) || (s.hasNext())) return false;\
            if (val>=_name::COUNT) return false;\
            Fvalue = static_cast<ENUM_TYPE_NAME>(val);  \
            return true;\
        }\
        bool strToVal(const char* _str){  \
            for (int i=0; i<_name::COUNT; i++){ if (strcmp(ENUM_STRS_NAME(_name)[i],_str) == 0){ Fvalue = static_cast<ENUM_TYPE_NAME>(i); return true; }}\
            return tryParseNumber(_str);\
        }\
		constexpr const char* enumBuffer(){ return &ENUM_BUFFER[0]; }\
		constexpr int enumBufferSize(){ return sizeof(ENUM_BUFFER)-1; }\
        const char* getEnum(int _idx) { return ENUM_STRS_NAME(_name)[_idx]; } \
        const char* c_str(){ return _name::c_str(Fvalue); }                                       \
};
*/
/* versions with case
    static const char* to_string(ENUM_TYPE_NAME _v){                                                               \
        switch(_v){                                                                                                \
        SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(ENUM_CASE,__VA_ARGS__)                                             \
        default : return "unknown";                                                                   \
        }                                                                                             \
    }                                                                                                 \
    bool strToVal(const char* _str){                                                                  \
        if (1==2) {return false;}                                                                     \
        SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(ENUM_STR_CASE,__VA_ARGS__)                            \
        return false;                                                                                 \
    }                                                                                                 \
*/

#endif // UENUMMACROS_h