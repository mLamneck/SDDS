#include "uTime.h"

dtypes::string timeToString(timeval& _time){
    constexpr int bufsize = 64;
    dtypes::string res;
    res.resize(bufsize);
    char* buf = res.data();
    time_t secs = _time.tv_sec;
    auto t = gmtime(&secs);
    int len = strftime(buf,bufsize,"%d.%m.%Y %H:%M:%S",t);
    res.resize(len);
    if (_time.tv_usec > 0){
        res = res + "." + strConv::to_string(_time.tv_usec/1000);
    }
    return res;
}

