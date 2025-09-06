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
	template <class __UART, class _EN_PIN=TgpioPinInactive<0,0>>
	class Tserial : public mhal::TserialBase, public __UART{
			static Tserial<__UART,_EN_PIN>* Finstance;
		public:
			using TserialBase::write;

			Tserial<__UART,_EN_PIN>(){
				Finstance = this;
			}

			size_t write(const dtypes::uint8* _buffer, int _len) override{
				if (_len <= 0) return 0;

				int written = 0;

			    if (__UART::tdr_empty()) {
			        __UART::transmit(*_buffer++);
			        written++;
			    }

			    if (written < _len){
				    for (int i = written; i < _len; i++) {
				        if (!FtxBuffer.write(*_buffer++)) {
				            break;
				        }
				        written++;
				    }
					__UART::isr_txfe_enable();
			    }

			    return written;
			}


			void isr(){
				if (__UART::uart_hardware_error())
					__UART::uart_hardware_clearErrors();

				while (__UART::ddr_notEmpty()) {
					isr_readByte(__UART::ddr_read());
				}

				if(__UART::isr_txfe_enabled()){
					while (__UART::tdr_empty()){
						if (FtxBuffer.readable() <= 0){
							__UART::isr_txfe_disable();
							__UART::isr_tc_enable();
							break;
						}
						__UART::transmit(FtxBuffer.read());
					}
				}

				else if (__UART::isr_tc_enabled() && __UART::isr_tc_flagSet()){
					__UART::isr_tc_clearFlag();
					__UART::isr_tc_disable();
					if (FautomaticDriverEnable)
						_EN_PIN::low();
					if (FtxIdle)
						FtxIdle->signal();
				}

				if (__UART:: isr_rto_enabled() && __UART::isr_rto_flagSet() ) {
					__UART::isr_rto_clearFlag();
					if (FrxEvent)
						FrxEvent->signal();
				}
			}

			static void irq_handler(){ Finstance->isr(); }

			void begin(dtypes::uint32 _baudrate,
				uart::DATAWIDTH _dw = uart::DW8,
				uart::PARITY _par = uart::NONE,
				uart::STOPBITS _sb = uart::SB1
			){
				_EN_PIN::init();
#if !MARKI_DEBUG_PLATFORM
				__UART::init(_baudrate,_dw,_par,_sb);
				__UART::isr_rxne_enable();
#endif
			}
	};
	template<class __UART, class _EN_PIN>
	Tserial<__UART, _EN_PIN>* Tserial<__UART, _EN_PIN>::Finstance = nullptr;

} //namespace mhal


#endif /* SDDS_SRC_MHAL_STM32_USERIAL_H_ */
