/*
 * uPwm.h
 *
 *  Created on: Nov 14, 2024
 *      Author: mark.lamneck
 */

#ifndef MHAL_UPWM_H_
#define MHAL_UPWM_H_

#include "uPlatform.h"
#include "uMmath.h"

namespace mhal{
	struct TtimerValues{
		static constexpr dtypes::uint32 MAX_ARR  = dtypes::high<dtypes::uint16>();
		static constexpr dtypes::uint32 MAX_PSC  = dtypes::high<dtypes::uint16>();

		dtypes::uint32  cpuFreq   = 0;
		dtypes::uint32  arr       = 0;
		dtypes::uint32  prescaler = 1;
		dtypes::uint8	dithValue = 1;
		bool            valid     = false;

		dtypes::float32 frequency() const {
			if (arr == 0 || prescaler == 0)
				return 0.0f;
			return static_cast<dtypes::float32>(cpuFreq) * dithValue / (static_cast<dtypes::float32>(arr) * prescaler);
		}

		bool useDithering(){ return dithValue > 1; }

		static TtimerValues calcTimerValues(dtypes::uint32 _cpuFreq, dtypes::float32 _freq, dtypes::uint8 _dithValue){
			TtimerValues tv;
			tv.cpuFreq   = _cpuFreq;
			tv.dithValue = _dithValue;
			if (_freq <= 0.0f || _dithValue == 0)
				return tv; //invalid

			dtypes::uint32  maxArr = MAX_ARR / _dithValue;
			dtypes::float32 ticks  = static_cast<dtypes::float32>(_cpuFreq) / _freq;

			dtypes::float32 prescF = mmath::ceil(ticks / static_cast<dtypes::float32>(maxArr));
			dtypes::uint32  presc = static_cast<dtypes::uint32>(prescF);
			if (prescF < 1.0f)
				presc = 1;
			else if (prescF > static_cast<dtypes::float32>(MAX_PSC))
				presc = MAX_PSC;
			tv.prescaler = presc;

			dtypes::float32 arrF = ticks / static_cast<dtypes::float32>(presc) * static_cast<dtypes::float32>(_dithValue) + 0.5f;
			if (arrF > static_cast<dtypes::float32>(MAX_ARR)){
				tv.arr   = MAX_ARR;
				tv.valid = false;
			} else {
				tv.arr   = static_cast<dtypes::uint32>(arrF);
				tv.valid = (tv.arr / _dithValue) >= 1;       // ARR=0 -> invalid
			}
			return tv;
		}

		bool calc(dtypes::uint32 _cpuFreq, dtypes::float32 _freq, dtypes::uint32 _dithValue=16){
			auto tvNoDith   = calcTimerValues(_cpuFreq, _freq, 1);
			auto tvWithDith = calcTimerValues(_cpuFreq, _freq, _dithValue);

			if (tvWithDith.valid && (!tvNoDith.valid || tvWithDith.arr >= tvNoDith.arr))
				*this = tvWithDith;
			else
				*this = tvNoDith;
			return valid;
		}
	};

	template<uintptr_t TIM_BASE_ADDR>
	class TtimerBase{
	public:
		constexpr static void setCounter(uint32_t _value){ };
		static int getArr() {return 8000; };
		static void deinit() {};
	};

	template<class _Ttimer, class _TgpioPin, uint32_t _CH>
	class TpwmChannelBase{
	public:
		using TIMER = _Ttimer;
		typedef _TgpioPin GPIO_PIN;
		static bool Fenabled;
		constexpr static void setCompareChannel(uint32_t _value){ };
		static bool init(dtypes::float32 _freq = 0.0f) {  Fenabled = true; return true; };
		static void deinit() { Fenabled = false; };
		static bool isEnabled() { return Fenabled; }
	};

	template<class _Ttimer, class _TgpioPin, uint32_t _CH>
	bool TpwmChannelBase<_Ttimer,_TgpioPin,_CH>::Fenabled = false;

}


#if defined(STM32_CUBE)
#include "STM32/uPwm.h"

#elif defined(SDDS_ON_ARDUINO)
#include "arduino/uPwm.h"
#else

namespace mhal{
	template<class _Ttimer, class _TgpioPin, uint32_t _CH>
	class TpwmChannel : public TpwmChannelBase<_Ttimer, _TgpioPin, _CH> {};
	
	template<uintptr_t TIM_BASE_ADDR>
	class Ttimer : public TtimerBase<TIM_BASE_ADDR> {};
}

#endif


#if (MARKI_DEBUG_PLATFORM == 1)
#define TIM1_BASE 0
#define TIM2_BASE 0
#define TIM3_BASE 0
#define TIM4_BASE 0
#define TIM5_BASE 0
#define TIM6_BASE 0
#define TIM7_BASE 0
#define TIM8_BASE 0
#define TIM15_BASE 0
#define TIM16_BASE 0
#define TIM17_BASE 0
#define TIM20_BASE 0
#endif

#endif /* MHAL_UPWM_H_ */
