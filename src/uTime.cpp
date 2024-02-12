#include "uTime.h"

dtypes::string timeToString(timeval& _time, const char* _fmt){
    constexpr int bufsize = 64;
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
}

dtypes::TdateTime stringToTime(const char* _timeStr){
    TdateTimeParser p(_timeStr);
    p.parse();
    return p.Fresult;
}
