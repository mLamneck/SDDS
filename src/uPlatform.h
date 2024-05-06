//never comment this in in production, as it leats to terrible exceptions especially on esp!!!
//#pragma pack (push, 1)

#ifndef UPLATFORM_H
#define UPLATFORM_H

#ifdef __AVR__
    #define SDDS_ON_AVR
#endif

#if defined(__MINGW64__) || defined(WIN32)      //__MINGW64__ works in VS_Code, WIN32 in codeBlocks
    #include <stdint.h>                         //uint8_t, ...
    #include <sys/time.h>
    
    #define MARKI_DEBUG_PLATFORM 1
    #define CRC_TAB_IN_PROGMEM 0

    #include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
    #include <string>

    //compatibility with arduino
    constexpr int LED_BUILTIN = 1;
    constexpr int OUTPUT = 1;
    void digitalWrite(int pin, int out);
    inline int digitalRead(int pin) { return 1; };
    inline void pinMode(int pin, int mode) {};
    template <typename T>
    inline const char* pcTaskGetName(T taskHandle) { return "debugTask"; }

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

    //to be checked
    #if defined(ESP32) || defined(ESP8266)
        #define CRC_TAB_IN_PROGMEM 0
        #define SDDS_EEPROM_COMMIT 1
        #define SDDS_EEPROM_SIZE 1024
        #define SDDS_EEPROM_BEGIN_WITH_SIZE 1
    #else
        #define CRC_TAB_IN_PROGMEM 1
        #define SDDS_EEPROM_COMMIT 0
        #define SDDS_EEPROM_SIZE 1024
        #define SDDS_EEPROM_BEGIN_WITH_SIZE 0
    #endif

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

void disableISR();
void enableISR();

#endif
