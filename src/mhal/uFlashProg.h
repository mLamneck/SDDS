#ifndef MHAL_UFLASHPROG_H_
#define MHAL_UFLASHPROG_H_


#if defined(STM32_CUBE)
	#include "STM32/uFlashProg.h"
#else
	#error "uProgFlash not implemented yet for this platform"
#endif


#endif /* MHAL_UFLASHPROG_H_ */
