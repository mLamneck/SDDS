# STM32 Alternate Function Code Generator (extras/)

This folder contains a small utility to generate C++ code for STM32 **Alternate Function (AF) mappings** directly from an official datasheet.

## What it does
- Parses the STM32 datasheet table for **Alternate Functions** (AF).
- Extracts the mapping between **Port / Pin / Peripheral Function** and the corresponding **AF number**.
- Generates C++ code with a `constexpr` lookup function so that the AF can be retrieved at compile time.  

## Result:

```C++
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
//...
```