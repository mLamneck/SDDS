/*
 * uSerial.h
 *
 *  Created on: Aug 30, 2025
 *      Author: mark.lamneck
 */

#ifndef SDDS_SRC_MHAL_USERIAL_H_
#define SDDS_SRC_MHAL_USERIAL_H_

#include <uTypedef.h>
#include <uMultask.h>
#include <uMemoryUtils.h>

namespace mhal{

	class TserialBase{
		public:
			template <typename T, size_t S>
			using TringBuffer = sdds::memUtils::TringBuffer<T,S>;
			typedef multask::TisrEvent TrxEvent;
			typedef multask::TisrEvent TtxIdleEvent;
		protected:
			TrxEvent* FrxEvent = nullptr;
			TtxIdleEvent* FtxIdle = nullptr;
			char FendSeq;
			bool FcheckForEndSeq = false;
			bool FautomaticDriverEnable = false;
			TringBuffer<uint8_t,64> FrxBuffer;
			TringBuffer<uint8_t,64> FtxBuffer;

		public:
			void setAutomaticDriverEnable(bool _val){
				FautomaticDriverEnable = _val;
			}

			void installRxEvent(TrxEvent* _ev){
				FrxEvent = _ev;
			}

			void installTxEvent(TtxIdleEvent* _ev){
				FtxIdle = _ev;
			}

			void setEndChar(char _end){
				FendSeq = _end;
				FcheckForEndSeq = true;
			}

			void resetEndChar(){
				FcheckForEndSeq = false;
			}

			virtual size_t write(const dtypes::uint8* _buffer, int _len) {
				return _len;
			}

			size_t write(const char* _buffer){
				int len = strlen(_buffer);
				return write(reinterpret_cast<const dtypes::uint8*>(_buffer),len);
			}

			size_t write(dtypes::string _buffer){
				return write(reinterpret_cast<const dtypes::uint8*>(_buffer.c_str()),_buffer.length());
			}

			size_t available(){
				return FrxBuffer.readable();
			}

			size_t availableForWrite(){
				return FtxBuffer.writeable();
			}

			dtypes::uint8 read(){
				return FrxBuffer.read();
			}

			void isr_readByte(dtypes::uint8 _b){
				FrxBuffer.write(_b);
				if (FrxEvent){
					if (FcheckForEndSeq){
						if (FendSeq == _b)
							FrxEvent->signal();
					};
				}
			}

			TserialBase(){

			}
	}; // class TserialBase

} // namespace mhal


/**
 * platform specific includes:
 */

#if MARKI_DEBUG_PLATFORM
namespace mhal{
	template <class __UART, class _EN_PIN=TgpioPinInactive<0,0>>
	class Tserial : public mhal::TserialBase, public __UART{
		static Tserial<__UART,_EN_PIN>* Finstance;
		public:
			static void irq_handler(){  }
			template<typename... Args>
			void begin(Args&&... args){}
	};
	template<class __UART, class _EN_PIN>
	Tserial<__UART,_EN_PIN>* Tserial<__UART,_EN_PIN>::Finstance = nullptr;
}
#endif

#if defined(STM32_CUBE)
	#include "STM32/uSerial.h"

#elif defined(SDDS_ON_ARDUINO)
	//toDOo
#endif

#endif /* SDDS_SRC_MHAL_USERIAL_H_ */
