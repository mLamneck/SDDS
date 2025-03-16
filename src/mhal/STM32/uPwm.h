/*
 * uPwm.h
 *
 *  Created on: Nov 14, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UPWM_STM32_H_
#define MHAL_UPWM_STM32_H_

#include "stm32g4xx_ll_tim.h"

namespace mhal{

template<uintptr_t TIM_BASE_ADDR>
class Ttimer : public TtimerBase<TIM_BASE_ADDR>{
	public:
		constexpr static TIM_TypeDef* TIM(){ return (TIM_TypeDef*)TIM_BASE_ADDR; }

		constexpr static void setCounter(uint32_t _value){ LL_TIM_SetCounter(TIM(),_value); };
};

template<class _Ttimer, class _GPIO_PIN, uint32_t _CH>
class TpwmChannel : public TpwmChannelBase<_Ttimer,_GPIO_PIN,_CH>{
public:
	constexpr static TIM_TypeDef* TIM(){ return _Ttimer::TIM(); }

	constexpr static void setCompareChannel(uint32_t _value){
		if (_CH == 1) LL_TIM_OC_SetCompareCH1(TIM(),_value);
		else if (_CH == 2) LL_TIM_OC_SetCompareCH2(TIM(),_value);
		else if (_CH == 3) LL_TIM_OC_SetCompareCH3(TIM(),_value);
		else if (_CH == 4) LL_TIM_OC_SetCompareCH4(TIM(),_value);
		else if (_CH == 5) LL_TIM_OC_SetCompareCH5(TIM(),_value);
		else if (_CH == 6) LL_TIM_OC_SetCompareCH6(TIM(),_value);
	}
};

}


#endif /* MHAL_UPWM_STM32_H_ */
