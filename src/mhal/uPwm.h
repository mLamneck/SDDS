/*
 * uPwm.h
 *
 *  Created on: Nov 14, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UPWM_H_
#define MHAL_UPWM_H_

namespace mhal{

	#if defined(STM32_CUBE)
		#include "STM32/uPwm.h"

	#elif defined(SDDS_ON_ARDUINO)
		#include "arduino/uPwm.h"
	#else

		template<uintptr_t PWM_BASE_ADDR, uint32_t _CH>
		class TpwmChannel{
			public:
				constexpr static void setCompareChannel(uint32_t _value){ };
		};
	#endif

}

#if (MARKI_DEBUG_PLATFORM == 1)
	#define TIM1_BASE 0
	#define TIM2_BASE 0
	#define TIM3_BASE 0
	#define TIM4_BASE 0
#endif

#endif /* MHAL_UPWM_H_ */
