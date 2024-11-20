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

template <int GPIO_PIN>
constexpr static IRQn_Type __mhal_PIN_TO_EXTI_IRQ(){
	switch(GPIO_PIN){
		#ifdef __STM32G474xx_H
			case 0: return EXTI0_IRQn;
			case 1: return EXTI1_IRQn;
			case 2: return EXTI2_IRQn;
			case 3: return EXTI3_IRQn;
			case 4: return EXTI4_IRQn;
			default:
				if (GPIO_PIN <= 9) return EXTI9_5_IRQn;
				else return EXTI15_10_IRQn;
		#else
			//toDo! return real values depending on pin
			return EXTI0_IRQ EXTI0_1_IRQn
		#endif
	}
}

/***********************************************************
 * Classes
 ***********************************************************/

	class TgpioClockManager {
	public:
			constexpr static void enableClock(uintptr_t GPIO_BASE_ADDR) {
					switch (GPIO_BASE_ADDR) {
							case GPIOA_BASE:
									if (!isClockEnabled(GPIOA_BASE)) {
											LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
									}
									break;
							case GPIOB_BASE:
									if (!isClockEnabled(GPIOB_BASE)) {
											LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
									}
									break;
							case GPIOC_BASE:
									if (!isClockEnabled(GPIOC_BASE)) {
											LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
									}
									break;
							case GPIOF_BASE:
									if (!isClockEnabled(GPIOF_BASE)) {
											LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF);
									}
									break;
					}
			}

	private:
			static bool isClockEnabled(uintptr_t GPIO_BASE_ADDR) {
					switch (GPIO_BASE_ADDR) {
							case GPIOA_BASE:
									return RCC->AHB2ENR & RCC_AHB2ENR_GPIOAEN;
							case GPIOB_BASE:
									return RCC->AHB2ENR & RCC_AHB2ENR_GPIOBEN;
							case GPIOC_BASE:
									return RCC->AHB2ENR & RCC_AHB2ENR_GPIOCEN;
							case GPIOF_BASE:
									return RCC->AHB2ENR & RCC_AHB2ENR_GPIOFEN;
							default:
									return false;
					}
			}
	};

	/**
	 * toDo: implement methods to enable interrupts. Implement all modes
	 *	- switch from HAL to LL to get rid of this stupid magic in HAL_GPIO_INIT
	 */
	template<uintptr_t GPIO_BASE_ADDR, uint32_t _GPIO_PIN>
	class TgpioPin {
	public:
			static_assert(_GPIO_PIN <= 15,"GPIO_PIN must be <= 15");
			constexpr static uint32_t GPIO_PIN = __mhal_NUMBER_TO_LL_GPIO_PIN<_GPIO_PIN>();
			constexpr static uint32_t EXTI_LINE = __mhal_NUMBER_TO_LL_EXTI_LINE<_GPIO_PIN>();
			constexpr static IRQn_Type EXTI_IRQ = __mhal_PIN_TO_EXTI_IRQ<_GPIO_PIN>();
			constexpr static GPIO_TypeDef* PORT(){ return (GPIO_TypeDef*)GPIO_BASE_ADDR; }

			constexpr static void enableClock(){
				TgpioClockManager::enableClock(GPIO_BASE_ADDR);
			}

			constexpr static void init(PIN_MODE _mode = PIN_MODE::output){
				enableClock();
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
						//there is no need to set the priority here.
						//implement a method to setup the interrupt
						HAL_NVIC_SetPriority(EXTI_IRQ, 2, 0);
						break;
				}
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
				HAL_GPIO_Init(PORT(), &GPIO_InitStruct);
			}

			constexpr static void high(){ LL_GPIO_SetOutputPin(PORT(), GPIO_PIN); }
			constexpr static void low(){ LL_GPIO_ResetOutputPin(PORT(), GPIO_PIN); }
			constexpr static void toggle(){ LL_GPIO_TogglePin(PORT(), GPIO_PIN); }

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
