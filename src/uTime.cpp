#include "uTime.h"

using namespace dtypes;

dtypes::string timeToString(dtypes::TdateTime& _time, const char* _fmt){
#if __SDDS_UTIME_CAN_PARSE_TEXT == 1
    //10.10.2024 10:33:33.999       //length 23
    constexpr int bufsize = 24;
    char buffer[bufsize];
    time_t secs = _time.tv_sec;
    auto t = gmtime(&secs);
    int len = 0;
    if (*_fmt == '\0') len = strftime(buffer,bufsize-1,"%d.%m.%Y %H:%M:%S",t);
    else len = strftime(buffer,bufsize-1,_fmt,t);
    buffer[len] = '\0';

    dtypes::string res(buffer);
    if (_time.tv_usec > 0){
        res = res + "." + strConv::to_string(_time.tv_usec/1000);
    }
    return res;
#else
    return "";
#endif
}

dtypes::TdateTime stringToTime(const char* _timeStr){
    TdateTimeParser p(_timeStr);
    p.parse();
    return p.Fresult;
}

