/*
 * uPwm.h
 *
 *  Created on: Nov 14, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UPWM_H_
#define MHAL_UPWM_H_

namespace mhal{

	template<uintptr_t PWM_BASE_ADDR, uint32_t _CH>
	class TpwmChannel{
		public:
	#if defined(STM32_CUBE)
			constexpr static TIM_TypeDef* TIM(){ return (TIM_TypeDef*)PWM_BASE_ADDR; }
			constexpr static void setCompareChannel(uint32_t _value){
				if (_CH == 1) LL_TIM_OC_SetCompareCH1(TIM(),_value);
				else if (_CH == 2) LL_TIM_OC_SetCompareCH2(TIM(),_value);
				else if (_CH == 3) LL_TIM_OC_SetCompareCH3(TIM(),_value);
				else if (_CH == 4) LL_TIM_OC_SetCompareCH4(TIM(),_value);
				else if (_CH == 5) LL_TIM_OC_SetCompareCH5(TIM(),_value);
				else if (_CH == 6) LL_TIM_OC_SetCompareCH6(TIM(),_value);
			}
	#else
			constexpr static void setCompareChannel(uint32_t _value){ };
	#endif
	};

}

#if (MARKI_DEBUG_PLATFORM == 1)
	#define TIM1_BASE 0
	#define TIM2_BASE 0
	#define TIM3_BASE 0
	#define TIM4_BASE 0
#endif

#endif /* MHAL_UPWM_H_ */
