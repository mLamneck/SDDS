#ifndef UPLATFORM_H
#define UPLATFORM_H

#ifdef __AVR__
    #define SDDS_ON_AVR
#endif

#ifndef STM32_CUBE
	#if defined(STM32C031xx) || defined(STM32G474xx)
		#define STM32_CUBE 1
	#endif
#endif

/************************************************************************************
 * MARKI_DEBUG_PLATFORM
 *
 * used for development on windows machines
*************************************************************************************/

#ifdef configUSE_PREEMPTION
	#define SDDS__WITH_RTOS 1
	#define __sdds_isr_critical(_code) do{\
		portENTER_CRITICAL()
		_code\
		portEXIT_CRITICAL()
	} while(0);
#endif

#if defined(ESP32) || defined(ESP8266)
	//#define __sdds_isr_disable() __disable_irq()
	//#define __sdds_isr_enable() __enable_irq()
	#define __sdds_isr_critical(_code)\
		auto __isrStatus = xPortSetInterruptMaskFromISR();\
		_code\
		vPortClearInterruptMaskFromISR(__isrStatus);

	#define ISR_CRITICAL(_code) do {                                 \
		auto __isrStatus = xPortSetInterruptMaskFromISR();           \
		_code                                                        \
		vPortClearInterruptMaskFromISR(__isrStatus);                 \
	} while (0)

	#define TASK_CRITICAL(_code) do {                                \
		taskENTER_CRITICAL();                                        \
		_code                                                        \
		taskEXIT_CRITICAL();                                         \
	} while (0)

//8266
#define XT_CRITICAL_SECTION(_code) do {        \
    uint32_t state = xt_rsil(15);             /* Alle Interrupts maskieren */ \
    _code                                     \
    xt_wsr_ps(state);                        /* Zustand wiederherstellen */   \
} while (0)

#endif

#if defined(__MINGW64__) || defined(WIN32)      //__MINGW64__ works in VS_Code, WIN32 in codeBlocks
    #include <stdint.h>                         //uint8_t, ...
    #include <sys/time.h>
    
    #define MARKI_DEBUG_PLATFORM 1

    #include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
    #include <string>

    //compatibility with arduino
    constexpr int LED_BUILTIN = 1;
    constexpr int PINMODE_OUTPUT = 0;
    constexpr int PINMODE_INPUT = 0;

    namespace sdds{
		namespace sysTime{
			constexpr int SYS_TICK_TIMEBASE = 1000;	//time in us for timeoverflow
		}
        namespace simul{
            typedef void(*Tisr)();
            constexpr int PIN_COUNT = 64;
            struct Tpin{
                uint8_t mode = 0;
                int analogValue = 0;
                uint8_t digitalValue = 0;
                Tisr pIsr = nullptr;
            };
            extern Tpin pins[PIN_COUNT];
        }
    }

    inline void __sdds_isr_disable(){}
    inline void __sdds_isr_enable(){}

    inline void digitalWrite(int pin, int out) { sdds::simul::pins[pin].digitalValue = out; };
    inline int  digitalRead(int pin) { return sdds::simul::pins[pin].digitalValue; };
    inline int  analogRead(int pin) { return sdds::simul::pins[pin].analogValue; };
    inline void pinMode(int pin, int mode) { sdds::simul::pins[pin].mode = mode; };

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


#elif defined(STM32_CUBE)

/************************************************************************************
 * STM32 Cube
*************************************************************************************/

#define __SDDS_UTYPEDEF_COMPILE_STRCONV 0
#define __SDDS_UTIME_CAN_PARSE_TEXT 0

#include <string>
//this file has to be created in the project and include the project related hal file i.e stm32c0xx_hal.h
#ifdef STM32C031xx
    #include "stm32c0xx_hal.h"
#else
	#include "stm32g4xx_hal.h"
#endif

#define __sdds_isr_disable() __disable_irq()
#define __sdds_isr_enable() __enable_irq()
#ifndef __sdds_isr_critical
	#define __sdds_isr_critical(_code) do{\
		auto __isrStatus = __get_PRIMASK();\
		__sdds_isr_disable();\
		_code\
		if (!__isrStatus) __sdds_isr_enable();\
	} while(0);
#endif

namespace dtypes {
	typedef std::string string;
	typedef uint32_t TsystemTime;
}

namespace strConv {
	template<typename valType>
	inline dtypes::string to_string(valType _val) {
		return std::to_string(_val);
	}
}

namespace sdds{
	namespace sysTime{
		constexpr int SYS_TICK_TIMEBASE = 100; //time in us for timeoverflow
	}
}
extern volatile uint32_t uwTick;
inline dtypes::TsystemTime millis() {
	return uwTick;
}

#else


/************************************************************************************
 * Arduino
*************************************************************************************/

#define SDDS_ON_ARDUINO 1

	#include <Arduino.h>

	namespace sdds{
		namespace sysTime{
			constexpr int SYS_TICK_TIMEBASE = 1000; //time in us for timeoverflow
		}
	}

	#define __sdds_isr_disable() noInterrupts()
	#define __sdds_isr_enable() interrupts()
	#ifndef __sdds_isr_critical
		#define __sdds_isr_critical(_code)\
			__sdds_isr_disable();\
			_code\
			__sdds_isr_enable();
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

#if !defined(SDDS_ON_ARDUINO)
	#define PROGMEM
#endif

//available on all compilers?
namespace dtypes {
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

	template <typename T> constexpr T high();
	template <> constexpr uint8 high<uint8>() { return 255; } 
	template <> constexpr uint16 high<uint16>() { return 65535; } 
	template <> constexpr uint32 high<uint32>() { return 4294967295U; } 
	template <> constexpr uint64 high<uint64>() { return 18446744073709551615ULL; } 
	template <> constexpr int8 high<int8>() { return 127; } 
	template <> constexpr int16 high<int16>() { return 32767; } 
	template <> constexpr int32 high<int32>() { return 2147483647; } 
	template <> constexpr int64 high<int64>() { return 9223372036854775807LL; } 
	template <> constexpr float32 high<float32>() { return 3.402823e+38F; } 
	template <> constexpr float64 high<float64>() { return 1.7976931348623157e+308; }
}

namespace sdds{
	namespace sysTime{
		constexpr int MILLIS = 1000/SYS_TICK_TIMEBASE;
	};
};


namespace debug {
	void write(const char *_fmt ...);
	void log(const char *_fmt ...);
}

#ifndef __SDDS_UTYPEDEF_COMPILE_STRCONV
	#define __SDDS_UTYPEDEF_COMPILE_STRCONV 1
#endif

#endif
