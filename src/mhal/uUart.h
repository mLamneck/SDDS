/*
 * uart.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UART_H_
#define MHAL_UART_H_

#include "uPlatform.h"

#if defined(STM32_CUBE)
	#include "STM32/uUart.h"
#elif defined(SDDS_ON_ARDUINO)
	static_assert(false,"not implemented for Arduino");
#else
	namespace mhal{
		template<uintptr_t UART_BASE_ADDR, class RX_PIN, class TX_PIN>
		class Tuart{
			public:
				enum DATAWIDTH {DW7,DW8,DW9};
				enum STOPBITS{SB05,SB1,SB15,SB2};
				enum PARITY{EVEN,ODD,NONE};

				constexpr static void rto_setRxTimeout(dtypes::uint32 _bits) {}
				constexpr static void rto_enableRxTimeout() {}
				constexpr static void rto_disableRxTimeout() {}

				//Receiver Timeout
				constexpr static void isr_rto_enable() { }
				constexpr static void isr_rto_disable(){ }
				constexpr static auto isr_rto_enabled(){ }
				constexpr static auto isr_rto_flagSet(){ return true; }
				constexpr static void isr_rto_clearFlag(){ }

				constexpr static uint32_t uart_hardware_error(){ return 0; }
				constexpr static void uart_hardware_clearErrors(){}

			template<typename... Args>
			constexpr void init(Args&&... args) {
			}
		};
	}

#if (MARKI_DEBUG_PLATFORM == 1)
	#define USART1_BASE 0
	#define USART2_BASE 0
	#define USART3_BASE 0
	#define USART4_BASE 0
	#define USART5_BASE 0
#endif

#endif

#endif /* MHAL_UART_H_ */
