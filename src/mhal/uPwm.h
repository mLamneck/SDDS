/*
 * uPwm.h
 *
 *  Created on: Nov 14, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UPWM_H_
#define MHAL_UPWM_H_

#include "uPlatform.h"

namespace mhal{

	template<uintptr_t TIM_BASE_ADDR>
	class TtimerBase{
	public:
		constexpr static void setCounter(uint32_t _value){ };
	};

	template<class _Ttimer, class _TgpioPin, uint32_t _CH>
	class TpwmChannelBase{
	public:
		typedef _TgpioPin GPIO_PIN;
		constexpr static void setCompareChannel(uint32_t _value){ };
	};

}


#if defined(STM32_CUBE)
#include "STM32/uPwm.h"

#elif defined(SDDS_ON_ARDUINO)
#include "arduino/uPwm.h"
#else

namespace mhal{
	template<class _Ttimer, class _TgpioPin, uint32_t _CH>
	class TpwmChannel : public TpwmChannelBase<_Ttimer, _TgpioPin, _CH> {};
	
	template<uintptr_t TIM_BASE_ADDR>
	class Ttimer : public TtimerBase<TIM_BASE_ADDR> {};
}

#endif


#if (MARKI_DEBUG_PLATFORM == 1)
#define TIM1_BASE 0
#define TIM2_BASE 0
#define TIM3_BASE 0
#define TIM4_BASE 0
#endif

#endif /* MHAL_UPWM_H_ */
