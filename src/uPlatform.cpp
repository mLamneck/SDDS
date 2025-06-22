#include "uPlatform.h"

/************************************************************************************
 * MARKI_DEBUG_PLATFORM
 *
 * used for development on windows machines
 * compiler flags:
 * 	sdds_noDebugOuput	
 * 	STM32_CUBE
*************************************************************************************/

#if MARKI_DEBUG_PLATFORM == 1

void debug::write(const char* _fmt...)
{
	#if sdds_noDebugOuput == 0
    va_list argptr;
    va_start(argptr, _fmt);
    vfprintf(stderr, _fmt, argptr);
    va_end(argptr);
	#endif
}

void debug::log(const char* _fmt...)
{
	#if sdds_noDebugOuput == 0
    va_list argptr;
    va_start(argptr, _fmt);
    vfprintf(stderr, _fmt, argptr);
    va_end(argptr);
    printf("\n");
	#endif
}

dtypes::TsystemTime _millis(){
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

dtypes::TsystemTime startTime = _millis();

dtypes::TsystemTime millis(){
    return _millis() - startTime;
}

namespace sdds{
	namespace sysTime{
		dtypes::TsystemTime tickCount(){
			return millis();
		}
	};
};


#else //MARKI_DEBUG_PLATFORM


#if defined(SDDS_ON_ARDUINO)

/************************************************************************************
 * Arduino
*************************************************************************************/

namespace sdds{
	namespace sysTime{
		dtypes::TsystemTime tickCount(){
			return millis();
		}
	};
};


#elif defined(STM32_CUBE)


/************************************************************************************
 * STM32 Cube
*************************************************************************************/

volatile dtypes::uint32 myTickCounter = 0;

namespace sdds{
	namespace sysTime{
		dtypes::TsystemTime tickCount(){
			return myTickCounter;
		}
	};
};

extern "C" void HAL_IncTick(void)
{
	myTickCounter++;	//increase our tickCounter 100us tickes

    static dtypes::uint32 modCounter = 0;
    if (++modCounter >= 1000/sdds::sysTime::SYS_TICK_TIMEBASE)
    {
        modCounter = 0;
        uwTick++;
    }
}

extern "C" HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  HAL_StatusTypeDef  status = HAL_OK;

  if ((uint32_t)uwTickFreq != 0UL)
  {
  	//48000000 / (1000 / 1) = 48000	example for 1ms with 48Mhz
  	constexpr int temp = (1000000/sdds::sysTime::SYS_TICK_TIMEBASE);
    if (HAL_SYSTICK_Config(SystemCoreClock / (temp / (uint32_t)uwTickFreq)) == 0U)
    {
      if (TickPriority < (1UL << __NVIC_PRIO_BITS))
      {
        HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority, 0U);
        uwTickPrio = TickPriority;
      }
      else
      {
        status = HAL_ERROR;
      }
    }
    else
    {
      status = HAL_ERROR;
    }
  }
  else
  {
    status = HAL_ERROR;
  }

  /* Return function status */
  return status;
}

#endif

void debug::log(const char* _fmt...){}
void debug::write(const char* _fmt...){}

#endif //STM32_CUBE
