#ifndef UCRC8_H
#define UCRC8_H

#include "uPlatform.h"

//ToDo: check if CRC_TAB_IN_PROGMEM 1 works for STM32 and 485 uart
#if defined(SDDS_ON_ARDUINO)
    #if defined(ESP32) || defined(ESP8266)
        #define CRC_TAB_IN_PROGMEM 0
    #else
        #define CRC_TAB_IN_PROGMEM 1
    #endif
#elif MARKI_DEBUG_PLATFORM == 1
    #define CRC_TAB_IN_PROGMEM 1
#endif


namespace crc8{
	typedef uint8_t Tcrc;
    #if CRC_TAB_IN_PROGMEM == 1
		extern const Tcrc tab[] PROGMEM;
	#else 
		extern Tcrc tab[];
	#endif

	inline void calc(Tcrc& _crc, uint8_t _input){
		_crc = tab[_crc ^ _input];
	}

	inline void calc(Tcrc& _crc, const void* _in, uint8_t _inLen){
		const uint8_t* p = static_cast<const uint8_t*>(_in);
		while (_inLen-- > 0) calc(_crc,*p++);
	}

	/* not used yet
	inline Tcrc calc(const void* _in, uint8_t _inLen){
		Tcrc c = 0;
		calc(c,_in,_inLen);
		return c;
	}
	*/
}

#endif
