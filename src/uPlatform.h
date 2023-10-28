#pragma pack (push, 1)

#ifndef UPLATFORM_H
#define UPLATFORM_H

#if defined(WIN32)      //#if defined(__MINGW64__)
    #define MARKI_DEBUG_PLATFORM

    #include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
    #include <string>
    #include <sys/time.h>

    namespace dtypes{
        typedef std::string string;
        typedef int64_t TsystemTime;
    }

    namespace strConv{
        template<typename valType>
        inline dtypes::string to_string(valType _val) { return std::to_string(_val); }
    }

    dtypes::TsystemTime millis();

#else
	#include <Arduino.h>

    namespace dtypes{
        typedef String string;                      //for Arduino this seems to be the best option for dynamic strings
        typedef long int TsystemTime;               //millis on Arduino returns... what is this exactly? unsigned long int?
    }
    namespace strConv{
        template<typename valType>
        inline dtypes::string to_string(valType _val) { return String(_val); }
    };
#endif

//available on all compilers?
namespace dtypes{
    //signed integers
    typedef uint8_t uint8;
    typedef uint16_t uint16;
    typedef uint32_t uint32;
    typedef uint64_t uint64;

    //unsigned integers
    typedef int8_t int8;
    typedef int16_t int16;
    typedef int32_t int32;
    typedef int64_t int64;

    //toDo!!! to be checked on each individual compiler and platfom!!!
    typedef float float32;
    typedef double float64;
}

namespace debug{
    void log(const char* _fmt...);
}

#endif