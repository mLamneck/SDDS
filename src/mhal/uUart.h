/*
 * uart.h
 *
 *  Created on: Aug 22, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UART_H_
#define MHAL_UART_H_

#include "uPlatform.h"

#ifdef __STM32G474xx_H
	#define RCC_USART1CLKSOURCE RCC_USART1CLKSOURCE_PCLK2
	#define __mhal_UART_AF LL_GPIO_AF_7
#else
	#define RCC_USART1CLKSOURCE RCC_USART1CLKSOURCE_PCLK1
	#define __mhal_UART_AF LL_GPIO_AF_0
#endif

namespace mhal{

	template<uintptr_t UART_BASE_ADDR, class RX_PIN, class TX_PIN>
	class Tuart{
			constexpr static USART_TypeDef* pUart(){ return (USART_TypeDef*)UART_BASE_ADDR; }
			constexpr static IRQn_Type uart_irq_type(){
				static_assert(UART_BASE_ADDR==USART1_BASE || UART_BASE_ADDR==USART3_BASE, "uart not implemented yet");
				if (UART_BASE_ADDR == USART1_BASE)
					return USART1_IRQn;
				else if (UART_BASE_ADDR == USART3_BASE)
					return USART3_IRQn;
			}
			constexpr static uint32_t ERR_MASK = (USART_ISR_ORE | USART_ISR_FE | USART_ISR_NE | USART_ISR_PE);

		public:
			constexpr static void init(int _baud = 115200*8){
				LL_USART_InitTypeDef USART_InitStruct = {0};
				LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
				RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

				/** Initializes the peripherals clocks */
				if (UART_BASE_ADDR == USART1_BASE){
					PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
					PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
					LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
				}
				else if (UART_BASE_ADDR == USART3_BASE){
					  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3;
					  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
					  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
				}
				if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
				{
					Error_Handler();
				}

				RX_PIN::enableClock();
				GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
				GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
				GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
				GPIO_InitStruct.Pin = TX_PIN::GPIO_PIN;
				GPIO_InitStruct.Alternate = __mhal_UART_AF;
				LL_GPIO_Init(TX_PIN::PORT(), &GPIO_InitStruct);

				TX_PIN::enableClock();
				GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
				GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
				GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
				GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
				GPIO_InitStruct.Pin = RX_PIN::GPIO_PIN;
				GPIO_InitStruct.Alternate = __mhal_UART_AF;
				LL_GPIO_Init(RX_PIN::PORT(), &GPIO_InitStruct);

				NVIC_SetPriority(uart_irq_type(), 0);
				isr_enable();

				USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
				USART_InitStruct.BaudRate = _baud;
				USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
				USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
				USART_InitStruct.Parity = LL_USART_PARITY_NONE;
				USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
				USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
				USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
				LL_USART_Init(pUart(), &USART_InitStruct);
				LL_USART_SetTXFIFOThreshold(pUart(), LL_USART_FIFOTHRESHOLD_8_8);
				LL_USART_SetRXFIFOThreshold(pUart(), LL_USART_FIFOTHRESHOLD_1_8);
				LL_USART_EnableFIFO(pUart());
				LL_USART_ConfigAsyncMode(pUart());

				//toDo: remove this block!!! this is for debugging
				/*
				rto_setRxTimeout(20);
				rto_enableRxTimeout();
				*/
				LL_USART_Enable(pUart());
				while((!(LL_USART_IsActiveFlag_TEACK(pUart()))) || (!(LL_USART_IsActiveFlag_REACK(pUart()))))
				{
				}
			}

			constexpr static void isr_enable(){ NVIC_EnableIRQ(uart_irq_type()); };
			constexpr static void isr_disable(){ NVIC_DisableIRQ(uart_irq_type()); };

			constexpr static void transmit(uint8_t _byte){ LL_USART_TransmitData8(pUart(), _byte); }

			//USART Read Data Register or USART RX FIFO Not Empty
			constexpr static uint32_t ddr_notEmpty(){ return LL_USART_IsActiveFlag_RXNE_RXFNE(pUart()); }
			constexpr static uint32_t ddr_empty(){ return !LL_USART_IsActiveFlag_RXNE_RXFNE(pUart()); }

			constexpr static uint32_t tdr_notEmpty(){ return LL_USART_IsActiveFlag_TXE_TXFNF(pUart()); }

			//read data register
			constexpr static uint8_t ddr_read(){ return LL_USART_ReceiveData8(pUart()); }


			//discard content of Rx Fifo without reading it
			constexpr static void flushRx(){ pUart()->RQR = USART_RQR_RXFRQ; }

			//Enable/Disable TX FIFO Empty Interrupt
			constexpr static void isr_txfe_enable(){ LL_USART_EnableIT_TXFE(pUart()); }
			constexpr static void isr_txfe_disable(){ LL_USART_DisableIT_TXFE(pUart()); }
			constexpr static auto isr_txfe_enabled(){ return LL_USART_IsEnabledIT_TXFE(pUart()); }
			constexpr static auto isr_txfe_flagSet(){ return LL_USART_IsActiveFlag_TXFE(pUart()); }

			//Enable/Disable TX Complete Interrupt
			constexpr static void isr_tc_enable(){ LL_USART_EnableIT_TC(pUart()); }
			constexpr static void isr_tc_disable(){ LL_USART_DisableIT_TC(pUart()); }
			constexpr static auto isr_tc_enabled(){ return LL_USART_IsEnabledIT_TC(pUart()); }
			constexpr static auto isr_tc_flagSet(){ return LL_USART_IsActiveFlag_TC(pUart()); }

			//Enable/Disable RX Not Empty and RX FIFO Not Empty Interrupt
			constexpr static void isr_rxne_enable() { LL_USART_EnableIT_RXNE_RXFNE(pUart()); }
			constexpr static void isr_rxne_disable(){ LL_USART_DisableIT_RXNE_RXFNE(pUart()); }
			constexpr static auto isr_rxne_enabled(){ return LL_USART_IsEnabledIT_RXNE(pUart()); }

			constexpr static void rto_setRxTimeout(dtypes::uint32 _bits) { LL_USART_SetRxTimeout(pUart(),_bits); }
			constexpr static void rto_enableRxTimeout() { LL_USART_EnableRxTimeout(pUart()); }
			constexpr static void rto_disableRxTimeout() { LL_USART_DisableRxTimeout(pUart()); }

			//Receiver Timeout
			constexpr static void isr_rto_enable() { LL_USART_EnableIT_RTO(pUart()); }
			constexpr static void isr_rto_disable(){ LL_USART_DisableIT_RTO(pUart()); }
			constexpr static auto isr_rto_enabled(){ return LL_USART_IsEnabledIT_RTO(pUart()); }
			constexpr static auto isr_rto_flagSet(){ return LL_USART_IsActiveFlag_RTO(pUart()); }
			constexpr static void isr_rto_clearFlag(){ LL_USART_ClearFlag_RTO(pUart()); }

			constexpr static uint32_t uart_hardware_error(){ return (USART1->ISR & ERR_MASK); }
			constexpr static void uart_hardware_clearErrors(){
				// Handle Overrun Error
				if (pUart()->ISR & USART_ISR_ORE) {
					pUart()->ICR = USART_ICR_ORECF;
					flushRx();
				}

				// Handle Framing Error
				if (pUart()->ISR & USART_ISR_FE) {
					pUart()->ICR = USART_ICR_FECF;
				}

				// Handle Noise Error
				if (pUart()->ISR & USART_ISR_NE) {
					pUart()->ICR = USART_ICR_NECF;
				}

				// Handle Parity Error
				if (pUart()->ISR & USART_ISR_PE) {
					pUart()->ICR = USART_ICR_PECF;
				}
			}

	};

}
#endif /* MHAL_UART_H_ */
