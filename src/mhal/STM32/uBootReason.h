/*
 * uBootReason.h
 *
 *  Created on: Jun 5, 2025
 *      Author: mark.lamneck
 */

#ifndef SDDS_SRC_MHAL_STM32_UBOOTREASON_H_
#define SDDS_SRC_MHAL_STM32_UBOOTREASON_H_

#include "uTypedef.h"

namespace mhal{
	class TbootReason{
		public:
			sdds_enum(___,LP,WWD,IWD,SOFTR,POWER,EPIN,BOR) TenBootReason;
		private:
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
		public:
			static TenBootReason::dtype getBootReason(){
				auto res = _getBootReason();
			    __HAL_RCC_CLEAR_RESET_FLAGS();
			    return res;
			}
	};
}



#endif /* SDDS_SRC_MHAL_STM32_UBOOTREASON_H_ */
