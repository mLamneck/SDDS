/*
 * uSystemInfo.h
 *
 *  Created on: Jun 8, 2025
 *      Author: mark.lamneck
 */

#ifndef SDDS_SRC_MHAL_USYSTEMINFO_H_
#define SDDS_SRC_MHAL_USYSTEMINFO_H_

#include "uTypedef.h"

namespace mhal{
	class TsystemInfoTemplate{
		public:
			sdds_enum(___) TenBootReason;

			static void init(){}

			static TenBootReason::dtype getBootReason(){ return TenBootReason::e::___; }

			static dtypes::uint32 getCpuFreq(){ return 0; }
			static dtypes::uint32 getCycleCount() { return 0; }
	};
}

#if defined(STM32_CUBE)
	#include "STM32/uSystemInfo.h"
#else
	namespace mhal{
		class TsystemInfo : public TsystemInfoTemplate{
		};
	}
#endif


#endif /* SDDS_SRC_MHAL_USYSTEMINFO_H_ */
