/*
 * gpio.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_GPIO_H_
#define MHAL_GPIO_H_

#include "uPlatform.h"

#ifdef __STM32G474xx_H
	#define EXTI0_IRQ EXTI0_IRQn
#else
	#define EXTI0_IRQ EXTI0_1_IRQn
#endif

template<uintptr_t GPIO_BASE_ADDR, uint32_t GPIO_PIN>
class TgpioPin {
public:
		static_assert(GPIO_PIN <= LL_GPIO_PIN_15,"asjdfklasd");

  	enum class PIN_MODE {input,output,it_rising_falling};

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
				  HAL_NVIC_SetPriority(EXTI0_IRQ, 2, 0);  // Priorität des Interrupts setzen
				  //HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);  // Interrupt aktivieren
					break;

			}
		  GPIO_InitStruct.Pull = GPIO_NOPULL;
		  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		  HAL_GPIO_Init(_PORT(), &GPIO_InitStruct);
		}

    constexpr static void high(){
    	LL_GPIO_SetOutputPin(_PORT(), GPIO_PIN);
    	/*
    	_PORT()->BSRR = GPIO_PIN;
      __NOP();
     	*/
    }

    constexpr static void low(){
    	LL_GPIO_ResetOutputPin(_PORT(), GPIO_PIN);
    	/*
    	_PORT()->BRR = GPIO_PIN;
      __NOP();
     	 */
    }

    constexpr static void pulse() {
    	high();
      low();
    }

    constexpr static void toggle(){ HAL_GPIO_TogglePin(_PORT(), GPIO_PIN); }

    constexpr static void exti_clearFlag(const dtypes::uint32 _line){
#ifdef __STM32G474xx_H
			LL_EXTI_ClearFlag_0_31 (_line);
#else
			LL_EXTI_ClearFallingFlag_0_31 (_line);
			LL_EXTI_ClearRisingFlag_0_31(_line);
#endif
    }

    constexpr static dtypes::uint32 exti_readFlag(const dtypes::uint32 _line){
#ifdef __STM32G474xx_H
			return LL_EXTI_ReadFlag_0_31 (_line);
#else
			return (LL_EXTI_ReadFallingFlag_0_31(_line) > 0)
				|| (LL_EXTI_ReadRisingFlag_0_31(_line) > 0);
#endif
    }

    constexpr static void exti_clearFlag(){
    	switch(GPIO_PIN){
				case LL_GPIO_PIN_0: return exti_clearFlag(LL_EXTI_LINE_0);
				case LL_GPIO_PIN_1: return exti_clearFlag(LL_EXTI_LINE_1);
				case LL_GPIO_PIN_2: return exti_clearFlag(LL_EXTI_LINE_2);
				case LL_GPIO_PIN_3: return exti_clearFlag(LL_EXTI_LINE_3);
				case LL_GPIO_PIN_4: return exti_clearFlag(LL_EXTI_LINE_4);
				case LL_GPIO_PIN_5: return exti_clearFlag(LL_EXTI_LINE_5);
				case LL_GPIO_PIN_6: return exti_clearFlag(LL_EXTI_LINE_6);
				case LL_GPIO_PIN_7: return exti_clearFlag(LL_EXTI_LINE_7);
				case LL_GPIO_PIN_8: return exti_clearFlag(LL_EXTI_LINE_8);
				case LL_GPIO_PIN_9: return exti_clearFlag(LL_EXTI_LINE_9);
				case LL_GPIO_PIN_10: return exti_clearFlag(LL_EXTI_LINE_10);
				case LL_GPIO_PIN_11: return exti_clearFlag(LL_EXTI_LINE_11);
				case LL_GPIO_PIN_12: return exti_clearFlag(LL_EXTI_LINE_12);
				case LL_GPIO_PIN_13: return exti_clearFlag(LL_EXTI_LINE_13);
				case LL_GPIO_PIN_14: return exti_clearFlag(LL_EXTI_LINE_14);
				case LL_GPIO_PIN_15: return exti_clearFlag(LL_EXTI_LINE_15);
    	}
    }

    constexpr static dtypes::uint32 exti_readFlag(){
    	switch(GPIO_PIN){
				case LL_GPIO_PIN_0 : return exti_readFlag(LL_EXTI_LINE_0);
				case LL_GPIO_PIN_1 : return exti_readFlag(LL_EXTI_LINE_1);
				case LL_GPIO_PIN_2 : return exti_readFlag(LL_EXTI_LINE_2);
				case LL_GPIO_PIN_3 : return exti_readFlag(LL_EXTI_LINE_3);
				case LL_GPIO_PIN_4 : return exti_readFlag(LL_EXTI_LINE_4);
				case LL_GPIO_PIN_5 : return exti_readFlag(LL_EXTI_LINE_5);
				case LL_GPIO_PIN_6 : return exti_readFlag(LL_EXTI_LINE_6);
				case LL_GPIO_PIN_7 : return exti_readFlag(LL_EXTI_LINE_7);
				case LL_GPIO_PIN_8 : return exti_readFlag(LL_EXTI_LINE_8);
				case LL_GPIO_PIN_9 : return exti_readFlag(LL_EXTI_LINE_9);
				case LL_GPIO_PIN_10: return exti_readFlag(LL_EXTI_LINE_10);
				case LL_GPIO_PIN_11: return exti_readFlag(LL_EXTI_LINE_11);
				case LL_GPIO_PIN_12: return exti_readFlag(LL_EXTI_LINE_12);
				case LL_GPIO_PIN_13: return exti_readFlag(LL_EXTI_LINE_13);
				case LL_GPIO_PIN_14: return exti_readFlag(LL_EXTI_LINE_14);
				case LL_GPIO_PIN_15: return exti_readFlag(LL_EXTI_LINE_15);
    	}
    	return 0;
    }
};

#endif /* MHAL_GPIO_H_ */
