#include "uTime.h"

dtypes::string timeToString(timeval& _time, const char* _fmt){
    constexpr int bufsize = 64;
    dtypes::string res;
    res.resize(bufsize);
    char* buf = res.data();
    time_t secs = _time.tv_sec;
    auto t = gmtime(&secs);
    int len = 0;
    if (*_fmt == '\0') len = strftime(buf,bufsize,"%d.%m.%Y %H:%M:%S",t);
    else len = strftime(buf,bufsize,_fmt,t);
    res.resize(len);
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

