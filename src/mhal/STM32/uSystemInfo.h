/*
 * uSystemInfo.h
 *
 *  Created on: Jun 8, 2025
 *      Author: mark.lamneck
 */

#ifndef SDDS_SRC_MHAL_STM32_USYSTEMINFO_H_
#define SDDS_SRC_MHAL_STM32_USYSTEMINFO_H_

#include "uTypedef.h"

namespace mhal{
	class TsystemInfo{
		public:
			sdds_enum(___,LP,WWD,IWD,SOFTR,POWER,EPIN,BOR) TenBootReason;

			static void init(){
				dwt_init();
			}

			static dtypes::uint32 getCpuFreq(){ return HAL_RCC_GetSysClockFreq(); }
			static dtypes::uint32 getCycleCount() { return DWT->CYCCNT; }
			static TenBootReason::dtype getBootReason(){
				auto res = _getBootReason();
			    __HAL_RCC_CLEAR_RESET_FLAGS();
			    return res;
			}

		private:
			static void dwt_init(){
			    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
			        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
			    }

			    DWT->CYCCNT = 0;                     // set counter to 0
			    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // activate counter
			}

			static TenBootReason::dtype _getBootReason(){
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST))
			        return TenBootReason::e::LP;
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST))
			        return TenBootReason::e::WWD;
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
			        return TenBootReason::e::IWD;
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST))
			        return TenBootReason::e::SOFTR;
			#ifdef RCC_FLAG_PORRST
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST))
			        jlkjlkjlk;
			#endif
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST))
			        return TenBootReason::e::EPIN;
			    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST))
			        return TenBootReason::e::BOR;
			    else
			        return TenBootReason::e::___;
			}
	};

}



#endif /* SDDS_SRC_MHAL_STM32_USYSTEMINFO_H_ */
