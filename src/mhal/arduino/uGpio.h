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
	}

	constexpr static void high(){  }
	constexpr static void low(){ }
	constexpr static void toggle(){ }

	constexpr static void pulse() { }

	constexpr static void exti_clearFlag(){ }

	constexpr static dtypes::uint32 exti_readFlag(){ return 0; }
};

}

#endif /* MHAL_GPIO_ARDUINO */
