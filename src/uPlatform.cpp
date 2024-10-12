#include "uPlatform.h"

#if MARKI_DEBUG_PLATFORM == 1

void debug::write(const char* _fmt...)
{
    va_list argptr;
    va_start(argptr, _fmt);
    vfprintf(stderr, _fmt, argptr);
    va_end(argptr);
}

void debug::log(const char* _fmt...)
{
    va_list argptr;
    va_start(argptr, _fmt);
    vfprintf(stderr, _fmt, argptr);
    va_end(argptr);
    printf("\n");
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

#elif defined(SDDS_ON_ARDUINO)

void debug::log(const char* _fmt...){}
void debug::write(const char* _fmt...){}


#else

void debug::log(const char* _fmt...){}
void debug::write(const char* _fmt...){}

#endif
