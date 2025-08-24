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

#include <chrono>

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

namespace sdds{
	namespace sysTime{
		dtypes::TtickCount tickCount(){
			return static_cast<dtypes::TtickCount>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
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
		dtypes::TtickCount tickCount(){
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
		dtypes::TtickCount tickCount(){
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
