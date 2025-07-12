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

/***********************************************************
 * Temporary Macros
 ***********************************************************/

#define mhal_mIterateChannels(x) x(1) x(2) x(3) x(4) x(5) x(6)

template <int CH_NUM>
constexpr static uint32_t __mhal_LL_TIM_CHANNEL() {
	switch(CH_NUM){
#define X(n) case n: return LL_TIM_CHANNEL_CH##n;
		mhal_mIterateChannels(X);
#undef X
	}
}


/***********************************************************
 * Classes
 ***********************************************************/

	class TtimerClockManager {
	public:
		constexpr static void enableClock(uintptr_t TIMER_BASE_ADDR) {
			switch (TIMER_BASE_ADDR) {
			case TIM1_BASE:
				if (!isClockEnabled(TIM1_BASE)) {
					LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
				}
				break;
			case TIM2_BASE:
				if (!isClockEnabled(TIM2_BASE)) {
					LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
				}
				break;
			case TIM3_BASE:
				if (!isClockEnabled(TIM3_BASE)) {
					LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
				}
				break;
			case TIM4_BASE:
				if (!isClockEnabled(TIM4_BASE)) {
					LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
				}
				break;
			case TIM6_BASE:
				if (!isClockEnabled(TIM6_BASE)) {
					LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
				}
				break;
			case TIM7_BASE:
				if (!isClockEnabled(TIM7_BASE)) {
					LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);
				}
				break;
			case TIM15_BASE:
				if (!isClockEnabled(TIM15_BASE)) {
					LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
				}
				break;
			case TIM16_BASE:
				if (!isClockEnabled(TIM16_BASE)) {
					LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
				}
				break;
			case TIM17_BASE:
				if (!isClockEnabled(TIM17_BASE)) {
					LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
				}
				break;
			default:
				break;
			}
		}

		constexpr static bool isClockEnabled(uintptr_t TIMER_BASE_ADDR) {
			switch (TIMER_BASE_ADDR) {
			case TIM1_BASE:
				return LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM1);
			case TIM2_BASE:
				return LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM2);
			case TIM3_BASE:
				return LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM3);
			case TIM4_BASE:
				return LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM4);
			case TIM6_BASE:
				return LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM6);
			case TIM7_BASE:
				return LL_APB1_GRP1_IsEnabledClock(LL_APB1_GRP1_PERIPH_TIM7);
			case TIM15_BASE:
				return LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM15);
			case TIM16_BASE:
				return LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM16);
			case TIM17_BASE:
				return LL_APB2_GRP1_IsEnabledClock(LL_APB2_GRP1_PERIPH_TIM17);
			default:
				return false;
			}
		}
	};


template<uintptr_t TIM_BASE_ADDR, uint32_t FREQ=115200, bool USE_DITHERING=true>
class Ttimer : public TtimerBase<TIM_BASE_ADDR>{
	public:
		static uint32_t ARR;
	public:
		constexpr static TIM_TypeDef* TIM(){ return (TIM_TypeDef*)TIM_BASE_ADDR; }

		constexpr static void setCounter(uint32_t _value){ LL_TIM_SetCounter(TIM(),_value); };
		constexpr static void enableClock(){ TtimerClockManager::enableClock(TIM_BASE_ADDR); }
		constexpr static bool isClockEnabled(){ return TtimerClockManager::isClockEnabled(TIM_BASE_ADDR); }
		constexpr static void enableCounter() { LL_TIM_EnableCounter(TIM()); }

		static uint32_t getArr(){
			return ARR;
			uint32_t arr = HAL_RCC_GetSysClockFreq()*1.0f/FREQ+0.5f;
			if (USE_DITHERING) arr = arr*16;
			return arr;
		}

		constexpr static void init(){
			if (isClockEnabled()) return;

			enableClock();

			LL_TIM_InitTypeDef TIM_InitStruct = {0};
			if (USE_DITHERING)
				LL_TIM_EnableDithering(TIM());
			TIM_InitStruct.Prescaler = 0;
			TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
			TIM_InitStruct.Autoreload = getArr();
			TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
			LL_TIM_Init(TIM(), &TIM_InitStruct);
			LL_TIM_EnableARRPreload(TIM());
			LL_TIM_SetClockSource(TIM(), LL_TIM_CLOCKSOURCE_INTERNAL);
		}
};

template<uintptr_t TIM_BASE_ADDR, uint32_t FREQ, bool USE_DITHERING>
uint32_t Ttimer<TIM_BASE_ADDR,FREQ,USE_DITHERING>::ARR = 0;

template<class _Ttimer, class _GPIO_PIN, uint32_t _CH>
class TpwmChannel : public TpwmChannelBase<_Ttimer,_GPIO_PIN,_CH>{
public:
	constexpr static TIM_TypeDef* TIM(){ return _Ttimer::TIM(); }
//	using TIMER=_Ttimer;

	constexpr static void setCompareChannel(uint32_t _value){
		if (_CH == 1) LL_TIM_OC_SetCompareCH1(TIM(),_value);
		else if (_CH == 2) LL_TIM_OC_SetCompareCH2(TIM(),_value);
		else if (_CH == 3) LL_TIM_OC_SetCompareCH3(TIM(),_value);
		else if (_CH == 4) LL_TIM_OC_SetCompareCH4(TIM(),_value);
		else if (_CH == 5) LL_TIM_OC_SetCompareCH5(TIM(),_value);
		else if (_CH == 6) LL_TIM_OC_SetCompareCH6(TIM(),_value);
	}

	constexpr static void enableChannel(){
		LL_TIM_CC_EnableChannel(TIM(),__mhal_LL_TIM_CHANNEL<_CH>());
	}

	constexpr static void init(){
		_Ttimer::init();

		/*****************************************
		 * configure compare channel and pwm
		*****************************************/

		LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
		LL_TIM_OC_EnablePreload(TIM(), __mhal_LL_TIM_CHANNEL<_CH>());
		TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
		TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
		TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
		TIM_OC_InitStruct.CompareValue = 0;
		TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
		LL_TIM_OC_Init(TIM(), __mhal_LL_TIM_CHANNEL<_CH>(), &TIM_OC_InitStruct);
		LL_TIM_OC_DisableFast(TIM(), __mhal_LL_TIM_CHANNEL<_CH>());
		LL_TIM_SetTriggerOutput(TIM(), LL_TIM_TRGO_RESET);
		LL_TIM_DisableMasterSlaveMode(TIM());


		/*****************************************
		 * configure gpio
		*****************************************/

		LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
		_GPIO_PIN::enableClock();
		GPIO_InitStruct.Pin = _GPIO_PIN::GPIO_PIN;
		GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
		GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
		GPIO_InitStruct.Alternate = LL_GPIO_AF_10;
		LL_GPIO_Init(_GPIO_PIN::PORT(), &GPIO_InitStruct);


		/*****************************************
		 * finally enable counter and channel
		*****************************************/

		enableChannel();
		_Ttimer::enableCounter();
	}

};

}


#endif /* MHAL_UPWM_STM32_H_ */
