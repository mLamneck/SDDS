#ifndef MHAL_AF_STM32_H_
#define MHAL_AF_STM32_H_

#include "stm32g4xx_ll_gpio.h"

namespace mhal{
	namespace AF{
		namespace TIM{
			constexpr static auto CH1 = 1;
			constexpr static auto CH2 = 2;
			constexpr static auto CH3 = 3;
			constexpr static auto CH4 = 4;
			constexpr static auto BKIN = 10;
			constexpr static auto BKIN2 = 15;
		}
		namespace USART{
			constexpr static auto TX = 11;
			constexpr static auto RX = 12;
		}
		namespace I2C{
			constexpr static auto SCL = 13;
			constexpr static auto SDA = 14;
		}

		constexpr static int32_t getAlternateFunction(uintptr_t _port, uint32_t _pin, uintptr_t _per, uint32_t _af){
			if (_port == GPIOA_BASE){
				if (_pin == 0){
					if (_per == TIM2_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM5_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_9;
				}
				if (_pin == 1){
					if (_per == TIM2_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_1;
					if (_per == TIM5_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 2){
					if (_per == TIM2_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_1;
					if (_per == TIM5_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
					if (_per == USART2_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
					if (_per == TIM15_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_9;
				}
				if (_pin == 3){
					if (_per == TIM2_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_1;
					if (_per == TIM5_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == USART2_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == TIM15_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_9;
				}
				if (_pin == 4){
					if (_per == TIM3_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 5){
					if (_per == TIM2_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
				}
				if (_pin == 6){
					if (_per == TIM16_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM3_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_4;
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_6;
				}
				if (_pin == 7){
					if (_per == TIM17_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM3_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 8){
					if (_per == I2C3_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_2;
					if (_per == I2C2_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
					if (_per == TIM1_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_6;
				}
				if (_pin == 9){
					if (_per == I2C2_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
					if (_per == TIM1_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_6;
					if (_per == USART1_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
					if (_per == TIM15_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_9;
					if (_per == TIM2_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_10;
				}
				if (_pin == 10){
					if (_per == TIM17_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_1;
					if (_per == TIM1_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_6;
					if (_per == USART1_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == TIM2_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_10;
					if (_per == TIM8_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_11;
				}
				if (_pin == 11){
					if (_per == TIM4_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_10;
					if (_per == TIM1_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_11;
					if (_per == TIM1_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_12;
				}
				if (_pin == 12){
					if (_per == TIM16_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM4_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_10;
				}
				if (_pin == 13){
					if (_per == I2C4_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_3;
					if (_per == I2C1_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
					if (_per == TIM4_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_10;
				}
				if (_pin == 14){
					if (_per == I2C1_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
					if (_per == TIM8_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_5;
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_6;
					if (_per == USART2_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 15){
					if (_per == TIM2_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM8_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == I2C1_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
					if (_per == USART2_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_9;
				}
			}
			if (_port == GPIOB_BASE){
				if (_pin == 0){
					if (_per == TIM3_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
				}
				if (_pin == 1){
					if (_per == TIM3_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
				}
				if (_pin == 2){
					if (_per == TIM5_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == TIM20_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_3;
				}
				if (_pin == 3){
					if (_per == TIM2_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_1;
					if (_per == USART2_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 4){
					if (_per == TIM16_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM3_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == USART2_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == TIM17_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_10;
				}
				if (_pin == 5){
					if (_per == TIM16_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_1;
					if (_per == TIM3_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
					if (_per == I2C3_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_8;
					if (_per == TIM17_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_10;
				}
				if (_pin == 6){
					if (_per == TIM4_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_5;
					if (_per == USART1_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
					if (_per == TIM8_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_10;
				}
				if (_pin == 7){
					if (_per == TIM4_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
					if (_per == I2C4_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_3;
					if (_per == I2C1_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
					if (_per == TIM8_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_5;
					if (_per == USART1_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == TIM3_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_10;
				}
				if (_pin == 8){
					if (_per == TIM16_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM4_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
					if (_per == I2C1_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
					if (_per == USART3_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == TIM8_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_10;
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_12;
				}
				if (_pin == 9){
					if (_per == TIM17_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
					if (_per == TIM4_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == I2C1_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
					if (_per == USART3_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
					if (_per == TIM8_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_10;
				}
				if (_pin == 10){
					if (_per == TIM2_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_1;
					if (_per == USART3_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_12;
				}
				if (_pin == 11){
					if (_per == TIM2_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_1;
					if (_per == USART3_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
				}
				if (_pin == 12){
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_6;
				}
				if (_pin == 14){
					if (_per == TIM15_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_1;
				}
				if (_pin == 15){
					if (_per == TIM15_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_1;
				}
			}
			if (_port == GPIOC_BASE){
				if (_pin == 0){
					if (_per == TIM1_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
				}
				if (_pin == 1){
					if (_per == TIM1_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 2){
					if (_per == TIM1_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
					if (_per == TIM20_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_6;
				}
				if (_pin == 3){
					if (_per == TIM1_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == TIM1_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_6;
				}
				if (_pin == 4){
					if (_per == I2C2_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
					if (_per == USART1_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 5){
					if (_per == TIM15_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
					if (_per == USART1_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
				}
				if (_pin == 6){
					if (_per == TIM3_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_4;
					if (_per == I2C4_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_8;
				}
				if (_pin == 7){
					if (_per == TIM3_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_4;
					if (_per == I2C4_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_8;
				}
				if (_pin == 8){
					if (_per == TIM3_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_4;
					if (_per == TIM20_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_6;
					if (_per == I2C3_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_8;
				}
				if (_pin == 9){
					if (_per == TIM3_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == TIM8_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_4;
					if (_per == TIM8_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_6;
					if (_per == I2C3_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_8;
				}
				if (_pin == 10){
					if (_per == USART3_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 11){
					if (_per == USART3_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
					if (_per == I2C3_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_8;
				}
				if (_pin == 12){
					if (_per == TIM5_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_1;
				}
				if (_pin == 13){
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
				}
			}
			if (_port == GPIOD_BASE){
				if (_pin == 1){
					if (_per == TIM8_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_4;
					if (_per == TIM8_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_6;
				}
				if (_pin == 2){
					if (_per == TIM8_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_4;
				}
				if (_pin == 3){
					if (_per == TIM2_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
				}
				if (_pin == 4){
					if (_per == TIM2_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 5){
					if (_per == USART2_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 6){
					if (_per == TIM2_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == USART2_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
				}
				if (_pin == 7){
					if (_per == TIM2_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
				}
				if (_pin == 8){
					if (_per == USART3_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 9){
					if (_per == USART3_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
				}
				if (_pin == 12){
					if (_per == TIM4_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
				}
				if (_pin == 13){
					if (_per == TIM4_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 14){
					if (_per == TIM4_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
				}
				if (_pin == 15){
					if (_per == TIM4_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
				}
			}
			if (_port == GPIOE_BASE){
				if (_pin == 0){
					if (_per == TIM16_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_4;
					if (_per == USART1_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
				if (_pin == 1){
					if (_per == TIM17_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_4;
					if (_per == TIM20_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_6;
					if (_per == USART1_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
				}
				if (_pin == 2){
					if (_per == TIM3_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
					if (_per == TIM20_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_6;
				}
				if (_pin == 3){
					if (_per == TIM3_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
					if (_per == TIM20_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_6;
				}
				if (_pin == 4){
					if (_per == TIM3_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
				}
				if (_pin == 5){
					if (_per == TIM3_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
				}
				if (_pin == 8){
					if (_per == TIM5_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_1;
				}
				if (_pin == 9){
					if (_per == TIM5_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_1;
					if (_per == TIM1_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
				}
				if (_pin == 11){
					if (_per == TIM1_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 13){
					if (_per == TIM1_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
				}
				if (_pin == 14){
					if (_per == TIM1_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == TIM1_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_6;
				}
				if (_pin == 15){
					if (_per == TIM1_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
					if (_per == USART3_BASE && (_af==USART::RX)) return LL_GPIO_AF_7;
				}
			}
			if (_port == GPIOF_BASE){
				if (_pin == 0){
					if (_per == I2C2_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
				}
				if (_pin == 2){
					if (_per == TIM20_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
				}
				if (_pin == 3){
					if (_per == TIM20_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == I2C3_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
				}
				if (_pin == 4){
					if (_per == I2C3_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
				}
				if (_pin == 6){
					if (_per == TIM4_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == I2C2_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
					if (_per == TIM5_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_6;
				}
				if (_pin == 7){
					if (_per == TIM20_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
					if (_per == TIM5_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_6;
				}
				if (_pin == 8){
					if (_per == TIM20_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_2;
					if (_per == TIM5_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_6;
				}
				if (_pin == 9){
					if (_per == TIM20_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
					if (_per == TIM15_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_3;
					if (_per == TIM5_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_6;
				}
				if (_pin == 10){
					if (_per == TIM20_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_2;
					if (_per == TIM15_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_3;
				}
				if (_pin == 12){
					if (_per == TIM20_BASE && (_af==TIM::CH1)) return LL_GPIO_AF_2;
				}
				if (_pin == 13){
					if (_per == TIM20_BASE && (_af==TIM::CH2)) return LL_GPIO_AF_2;
				}
				if (_pin == 14){
					if (_per == TIM20_BASE && (_af==TIM::CH3)) return LL_GPIO_AF_2;
					if (_per == I2C4_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
				}
				if (_pin == 15){
					if (_per == TIM20_BASE && (_af==TIM::CH4)) return LL_GPIO_AF_2;
					if (_per == I2C4_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
				}
			}
			if (_port == GPIOG_BASE){
				if (_pin == 3){
					if (_per == TIM20_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
					if (_per == I2C4_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
				}
				if (_pin == 4){
					if (_per == TIM20_BASE && (_af==TIM::BKIN2)) return LL_GPIO_AF_2;
					if (_per == I2C4_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
				}
				if (_pin == 6){
					if (_per == TIM20_BASE && (_af==TIM::BKIN)) return LL_GPIO_AF_2;
				}
				if (_pin == 7){
					if (_per == I2C3_BASE && (_af==I2C::SCL)) return LL_GPIO_AF_4;
				}
				if (_pin == 8){
					if (_per == I2C3_BASE && (_af==I2C::SDA)) return LL_GPIO_AF_4;
				}
				if (_pin == 9){
					if (_per == USART1_BASE && (_af==USART::TX)) return LL_GPIO_AF_7;
				}
			}
		}
	}
}

#endif //MHAL_AF_STM32_H_
