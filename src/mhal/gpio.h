/*
 * gpio.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_GPIO_H_
#define MHAL_GPIO_H_

template<uintptr_t GPIO_BASE_ADDR, uint32_t GPIO_PIN>
class TgpioPin {
public:
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
				  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 2, 0);  // Priorität des Interrupts setzen
				  //HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);  // Interrupt aktivieren
					break;

			}
		  GPIO_InitStruct.Pull = GPIO_NOPULL;
		  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		  HAL_GPIO_Init(_PORT(), &GPIO_InitStruct);
		}

    static void high(){
    	//LL_GPIO_SetOutputPin(GPIOx, PinMask)
  		_PORT()->BSRR = GPIO_PIN;
      __NOP();
    }

    static void low(){
    	_PORT()->BRR = GPIO_PIN;
      __NOP();
    }

    static void pulse() {
    	high();
      low();
    }

    static void toggle(){
      uint32_t odr = _PORT()->ODR;
      _PORT()->BSRR = ((odr & GPIO_PIN) << (16U)) | (~odr & GPIO_PIN);
    }
};

#endif /* MHAL_GPIO_H_ */
