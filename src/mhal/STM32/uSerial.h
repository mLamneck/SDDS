/*
 * uSerial.h
 *
 *  Created on: Aug 30, 2025
 *      Author: mark.lamneck
 */

#ifndef SDDS_SRC_MHAL_STM32_USERIAL_H_
#define SDDS_SRC_MHAL_STM32_USERIAL_H_

#include <mhal/uSerial.h>

namespace mhal{
	template <class __UART>
	class Tserial : public mhal::TserialBase, public __UART{
			static Tserial<__UART>* Finstance;
		public:
			using TserialBase::write;

			Tserial<__UART>(){
				Finstance = this;
			}

			size_t write(const dtypes::uint8* _buffer, int _len) override{
				int len = _len;
				if (__UART::tdr_empty()){
					__UART::transmit(*_buffer++);
					len--;
				}
				if (len > 0){
					while (len-- > 0)
						FtxBuffer.write(*_buffer++);
					__UART::isr_txfe_enable();
				}
				return len-_len;
			}


			void isr(){
				board::FLASH_CONN::UART_EN::high();
				if (__UART::uart_hardware_error())
					__UART::uart_hardware_clearErrors();

				while (__UART::ddr_notEmpty()) {
					isr_readByte(__UART::ddr_read());
				}

				if(__UART::isr_txfe_enabled()){
					while (__UART::tdr_empty()){
						if (FtxBuffer.readable() <= 0){
							__UART::isr_txfe_disable();
							if (FtxIdle)
								FtxIdle->signal();
							break;
						}
						__UART::transmit(FtxBuffer.read());
					}
				}

				if (__UART:: isr_rto_enabled() && __UART::isr_rto_flagSet() ) {
					__UART::isr_rto_clearFlag();
					if (FrxEvent)
						FrxEvent->signal();
				}
				//board::FLASH_CONN::UART_EN::low();
			}

			static void irq_handler(){ Finstance->isr(); }

			void begin(dtypes::uint32 _baudrate,
				uart::DATAWIDTH _dw = uart::DW8,
				uart::PARITY _par = uart::NONE,
				uart::STOPBITS _sb = uart::SB1
			){
#if !MARKI_DEBUG_PLATFORM
				__UART::init(_baudrate,_dw,_par,_sb);
				__UART::isr_rxne_enable();
#endif
			}
	};
	template<class __UART>
	Tserial<__UART>* Tserial<__UART>::Finstance = nullptr;

} //namespace mhal


#endif /* SDDS_SRC_MHAL_STM32_USERIAL_H_ */
