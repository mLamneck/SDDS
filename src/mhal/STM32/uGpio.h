/*
 * gpio.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_GPIO_STM32_H_
#define MHAL_GPIO_STM32_H_


/***********************************************************
 * Includes
 ***********************************************************/

#include "uPlatform.h"


/***********************************************************
 * Defines
 ***********************************************************/

#ifdef __STM32G474xx_H
	#define EXTI0_IRQ EXTI0_IRQn
#else
	#define EXTI0_IRQ EXTI0_1_IRQn
#endif


/***********************************************************
 * Temporary Macros
 ***********************************************************/

#define mhal_mIteratePins(x) x(0) x(1) x(2) x(3) x(4) x(5) x(6) x(7)	x(8) x(9) x(10)	x(11)	x(12)	x(13)	x(14) x(15)

template <int GPIO_PIN>
constexpr static uint32_t __mhal_NUMBER_TO_LL_GPIO_PIN() {
	switch(GPIO_PIN){
		#define X(n) case n: return LL_GPIO_PIN_##n;
		mhal_mIteratePins(X);
		#undef X
	}
}

template <int GPIO_PIN>
constexpr static uint32_t __mhal_NUMBER_TO_LL_EXTI_LINE() {
	switch(GPIO_PIN){
		#define X(n) case n: return LL_EXTI_LINE_##n;
		mhal_mIteratePins(X);
		#undef X
	}
}
#undef mhal_mIteratePins


/***********************************************************
 * Classes
 ***********************************************************/

	/**
	 *
	 */
	template<uintptr_t GPIO_BASE_ADDR, uint32_t _GPIO_PIN>
	class TgpioPin {
	public:
			static_assert(_GPIO_PIN <= 15,"GPIO_PIN must be <= 15");
			constexpr static uint32_t GPIO_PIN = __mhal_NUMBER_TO_LL_GPIO_PIN<_GPIO_PIN>();
			constexpr static uint32_t EXTI_LINE = __mhal_NUMBER_TO_LL_EXTI_LINE<_GPIO_PIN>();
			constexpr static GPIO_TypeDef* _PORT(){ return (GPIO_TypeDef*)GPIO_BASE_ADDR; }

			constexpr static void init(PIN_MODE _mode = PIN_MODE::output){
				/*
					LL_USART_InitTypeDef USART_InitStruct = {0};
					GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
					GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
					GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
					GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
					GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
					GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
					LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
				 */
				/*
				*/
				GPIO_InitTypeDef GPIO_InitStruct = {0};
				GPIO_InitStruct.Pin = GPIO_PIN;
				switch(_mode){
					case PIN_MODE::input:
						break;
					case PIN_MODE::output:
						GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
						break;
					case PIN_MODE::it_rising_falling:
						GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
						HAL_NVIC_SetPriority(EXTI0_IRQ, 2, 0);
						//HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
						break;

				}
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				HAL_GPIO_Init(_PORT(), &GPIO_InitStruct);
			}

			constexpr static void high(){ LL_GPIO_SetOutputPin(_PORT(), GPIO_PIN); }
			constexpr static void low(){ LL_GPIO_ResetOutputPin(_PORT(), GPIO_PIN); }
			constexpr static void toggle(){ LL_GPIO_TogglePin(_PORT(), GPIO_PIN); }

			constexpr static void pulse() {
				high();
				low();
			}

			constexpr static void exti_clearFlag(){
				#ifdef __STM32G474xx_H
					LL_EXTI_ClearFlag_0_31(EXTI_LINE);
				#else
					LL_EXTI_ClearFallingFlag_0_31 (EXTI_LINE);
					LL_EXTI_ClearRisingFlag_0_31(EXTI_LINE);
				#endif
			}

			constexpr static dtypes::uint32 exti_readFlag(){
				#ifdef __STM32G474xx_H
					return LL_EXTI_ReadFlag_0_31 (EXTI_LINE);
				#else
					return (LL_EXTI_ReadFallingFlag_0_31(EXTI_LINE) > 0) || (LL_EXTI_ReadRisingFlag_0_31(EXTI_LINE) > 0);
				#endif
			}
	};


#endif /* MHAL_GPIO_H_ */
