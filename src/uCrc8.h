#ifndef UCRC8_H
#define UCRC8_H

#include "uPlatform.h"

namespace crc8{
	typedef uint8_t Tcrc;
	extern Tcrc tab[];

	inline void calc(Tcrc& _crc, uint8_t _input){
		_crc = tab[_crc ^ _input];
	}

	inline void calc(Tcrc& _crc, const void* _in, uint8_t _inLen){
		const uint8_t* p = static_cast<const uint8_t*>(_in);
		while (_inLen-- > 0) calc(_crc,*p);
	}
}

#endif