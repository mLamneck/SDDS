/*
 * uBootReason.h
 *
 *  Created on: Jun 5, 2025
 *      Author: mark.lamneck
 */

#ifndef SDDS_SRC_MHAL_UBOOTREASON_H_
#define SDDS_SRC_MHAL_UBOOTREASON_H_

#if defined(STM32_CUBE)
	#include "STM32/uBootReason.h"
#elif MARKI_DEBUG_PLATFORM

	namespace mhal{
		class TbootReason{
			public:
				sdds_enum(___) TenBootReason;
				static TenBootReason::dtype getBootReason(){
					return TenBootReason::e::___;
				}
			};
	}


#else
	#error "uBootReason not implemented yet for this platform"
#endif





#endif /* SDDS_SRC_MHAL_UBOOTREASON_H_ */
