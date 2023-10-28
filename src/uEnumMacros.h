#ifndef UENUMMACROS_h
#define UENUMMACROS_h

#include "uSpookyMacros.h"

#define ENUM__TOKENPASTE(x, y) x ## y
#define ENUM__TOKENPASTE2(x, y) ENUM__TOKENPASTE(x, y)
#define ENUM_UNIQUE_NAME() ENUM__TOKENPASTE2(__Tenum__,__LINE__)

#define ENUM_CASE(_enumStr) case e::_enumStr : return #_enumStr;
#define ENUM_TYPE_NAME e
#define ENUM_STR_CASE(_e) else if (strcmp(_str,#_e)==0){ Fvalue = ENUM_TYPE_NAME::_e; return true; }
#define ENUM_STR(_e) #_e ,

#define ENUM_CLASS(_name,...)\
class _name{                \
    public: \
        constexpr static const uint8_t COUNT = SP_COUNT_VARARGS(__VA_ARGS__);                                          \
        constexpr static const char* ENUM_STRS[] = {SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(ENUM_STR,__VA_ARGS__)};    \
        typedef uint8_t dtype;\
        enum class ENUM_TYPE_NAME : dtype {__VA_ARGS__};                                                             \
        ENUM_TYPE_NAME Fvalue;                                                                                         \
        operator ENUM_TYPE_NAME() const{return Fvalue;}                                                                \
        void operator=(ENUM_TYPE_NAME _value){Fvalue = _value;}                                                        \
        static const char* to_string(ENUM_TYPE_NAME _v){\
            dtype idx = static_cast<dtype>(_v);\
            if (idx < _name::COUNT) return _name::ENUM_STRS[idx];\
            return "unknown";\
        }\
        bool strToVal(const char* _str){  \
            for (int i=0; i<_name::COUNT; i++){ if (strcmp(_name::ENUM_STRS[i],_str) == 0){ Fvalue = static_cast<ENUM_TYPE_NAME>(i); return true; }}\
            return false;\
        }\
        const char* to_string(){ return _name::to_string(Fvalue); }                                       \
}

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
