/*
 * gpio.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_GPIO_ARDUINO_H_
#define MHAL_GPIO_ARDUINO

#include "uPlatform.h"

namespace mhal{

template<uint32_t _GPIO_PIN>
class TgpioPin {
public:
	constexpr static void init(PIN_MODE _mode = PIN_MODE::output){
		if (_mode == PIN_MODE::output) pinMode(_GPIO_PIN,OUTPUT);
		else pinMode(_GPIO_PIN,INPUT);
	}

	constexpr static void analogWrite(int _val){ ::analogWrite(_GPIO_PIN, _val); }
	constexpr static void digitalWrite(int _val){ ::digitalWrite(_GPIO_PIN, _val); }
	constexpr static int analogRead(){ return ::analogRead(_GPIO_PIN); }
	constexpr static int digitalRead(){ return ::digitalRead(_GPIO_PIN); }
	
	constexpr static void high(){ digitalWrite(HIGH);  }
	constexpr static void low(){ digitalWrite(LOW); }
	constexpr static void toggle(){ digitalWrite(!digitalRead()); }
	constexpr static void pulse() { high(); low(); }

	constexpr static void exti_clearFlag(){ }

	constexpr static dtypes::uint32 exti_readFlag(){ return 0; }
};

}

#endif /* MHAL_GPIO_ARDUINO */
