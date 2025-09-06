/*
 * gpio.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_GPIO_H_
#define MHAL_GPIO_H_

#include "uPlatform.h"

namespace mhal{

	enum class PIN_MODE {input,output,it_rising_falling};
	template<uintptr_t GPIO_BASE_ADDR, uint32_t _GPIO_PIN>
	class TgpioPinInactive {
		public:
			constexpr static void init(PIN_MODE _mode = PIN_MODE::output){ }

			constexpr static void high(){  }
			constexpr static void low(){ }
			constexpr static void toggle(){ }

			constexpr static void pulse() { }

			constexpr static void exti_clearFlag(){ }

			constexpr static dtypes::uint32 exti_readFlag(){ return 0; }
	};

	#if (MARKI_DEBUG_PLATFORM == 1)
		#define GPIOA_BASE 0
		#define GPIOB_BASE 0
		#define GPIOC_BASE 0
		#define GPIOD_BASE 0
	#endif
}

#if defined(STM32_CUBE)
	#include "STM32/uGpio.h"

#elif defined(SDDS_ON_ARDUINO)
	#include "arduino/uGpio.h"
#else
	namespace mhal{

	template<uintptr_t GPIO_BASE_ADDR, uint32_t _GPIO_PIN>
		class TgpioPin {
			public:
				constexpr static void init(PIN_MODE _mode = PIN_MODE::output){ }

				constexpr static void high(){  }
				constexpr static void low(){ }
				constexpr static void toggle(){ }

				constexpr static void pulse() { }

				constexpr static void exti_clearFlag(){ }

				constexpr static dtypes::uint32 exti_readFlag(){ return 0; }
		};
	}
#endif


#endif /* MHAL_GPIO_H_ */
