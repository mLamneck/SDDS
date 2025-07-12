#ifndef UPLATFORM_H
#define UPLATFORM_H

/************************************************************************************
 * UPLATFORM
 *
 * compiler flags:
 * 	if sdds_noDebugOuput == 1		//prevent debug::write/log to print to the console
 * 	#ifdef STM32_CUBE				//use stm32
 *  #ifdef SDDS_ON_AVR
 * 	#ifdef ESP32
 * 	#ifdef ESP8266
 *  #if defined(SDDS_ON_ARDUINO)
 * 	? __SDDS_UTYPEDEF_COMPILE_STRCONV 0
 *	? __SDDS_UTIME_CAN_PARSE_TEXT 0

*************************************************************************************/

#ifdef __AVR__
    #define SDDS_ON_AVR
#endif

#ifndef STM32_CUBE
	#if defined(STM32C031xx) || defined(STM32G474xx)
		#define STM32_CUBE 1
	#endif
#endif


/************************************************************************************
 * stuff available on all platforms and compilers
*************************************************************************************/

#include <stdint.h>                         //uint8_t, ...

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

	//floating point
	typedef float float32;
	typedef double float64;

	/**
	 * @brief this is the type of our timebase. It is the type of the value that is stored in 
	 * timeevents and compared to our sysTime. Never change this to an unsigned value. Otherwise
	 * time calculation will fail...
	 */
	typedef int32_t TtickCount;

	//numeric limits
	template <typename T> constexpr T high();
	template <typename T> constexpr T low();
	template <> constexpr uint8 high<uint8>() { return 255; }
	template <> constexpr uint8 low<uint8>() { return 0; }
	template <> constexpr uint16 high<uint16>() { return 65535; }
	template <> constexpr uint16 low<uint16>() { return 0; }
	template <> constexpr uint32 high<uint32>() { return 4294967295U; }
	template <> constexpr uint32 low<uint32>() { return 0; }
	template <> constexpr uint64 high<uint64>() { return 18446744073709551615ULL; }
	template <> constexpr uint64 low<uint64>() { return 0; }
	template <> constexpr int8 high<int8>() { return 127; }
	template <> constexpr int8 low<int8>() { return -128; }
	template <> constexpr int16 high<int16>() { return 32767; }
	template <> constexpr int16 low<int16>() { return -32768; }
	template <> constexpr int32 high<int32>() { return 2147483647; }
	template <> constexpr int32 low<int32>() { return -2147483648; }
	template <> constexpr int64 high<int64>() { return 9223372036854775807LL; }
	template <> constexpr int64 low<int64>() { return -9223372036854775807LL - 1; }
	template <> constexpr float32 high<float32>() { return 3.402823e+38F; }
	template <> constexpr float32 low<float32>() { return -3.402823e+38F; }
	template <> constexpr float64 high<float64>() { return 1.7976931348623157e+308; }
	template <> constexpr float64 low<float64>() { return -1.7976931348623157e+308; }
}

namespace sdds{
	class TsysFuncs{
		public:
			static dtypes::uint32 getCpuFreq(){ return 0; }
	};
}


/************************************************************************************
 * ESP specific stuff
*************************************************************************************/

#if defined(ESP32)
	#include "freertos/FreeRTOS.h"
	#include "freertos/semphr.h"

	static portMUX_TYPE __sdds_mux = portMUX_INITIALIZER_UNLOCKED;
	#define __sdds_isr_critical(_code)\
		portENTER_CRITICAL(&__sdds_mux);\
		_code\
		portEXIT_CRITICAL(&__sdds_mux);
#elif defined(ESP8266)
#endif


/************************************************************************************
 * MARKI_DEBUG_PLATFORM
 *
 * used for development on windows machines
*************************************************************************************/

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

		class SysFuncs : public TsysFuncs{
			public:
		};

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

    inline void __sdds_systemReset(){}
    inline void __sdds_isr_disable(){}
    inline void __sdds_isr_enable(){}
	#define __sdds_isr_critical(_code)

    inline void digitalWrite(int pin, int out) { sdds::simul::pins[pin].digitalValue = out; };
    inline int  digitalRead(int pin) { return sdds::simul::pins[pin].digitalValue; };
    inline int  analogRead(int pin) { return sdds::simul::pins[pin].analogValue; };
    inline void pinMode(int pin, int mode) { sdds::simul::pins[pin].mode = mode; };

    template <typename T>
    inline const char* pcTaskGetName(T taskHandle) { return "debugTask"; }

    namespace dtypes{
        typedef std::string string;
    }

    namespace strConv{
        template<typename valType>
        inline dtypes::string to_string(valType _val) { return std::to_string(_val); }
    }


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

inline void __sdds_systemReset(){ NVIC_SystemReset(); }

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
}

namespace strConv {
	template<typename valType>
	inline dtypes::string to_string(valType _val) {
		return std::to_string(_val);
	}
}

namespace sdds{
	class SysFuncs : public TsysFuncs{
		public:
			static dtypes::uint32 getCpuFreq(){ return HAL_RCC_GetSysClockFreq(); }
	};

	namespace sysTime{
		constexpr int SYS_TICK_TIMEBASE = 100; //time in us for timeoverflow
	}
}

#else


/************************************************************************************
 * Arduino
*************************************************************************************/

#define SDDS_ON_ARDUINO 1

	#include <Arduino.h>

	constexpr auto PINMODE_OUTPUT = OUTPUT;
	constexpr auto PINMODE_INPUT = INPUT;

	namespace sdds{
		namespace sysTime{
			constexpr int SYS_TICK_TIMEBASE = 1000; //time in us for timeoverflow
		}

		class SysFuncs : public TsysFuncs{
		};
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
    }
    namespace strConv{
        template<typename valType>
        inline dtypes::string to_string(valType _val) { return String(_val); }
    };
#endif

#if !defined(SDDS_ON_ARDUINO)
	#define PROGMEM
#endif

namespace sdds{
	namespace sysTime{
		constexpr int MILLIS = 1000/SYS_TICK_TIMEBASE;
		dtypes::TtickCount tickCount();
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
