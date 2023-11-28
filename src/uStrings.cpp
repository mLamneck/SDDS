#include "uStrings.h"

void string_sub(const char* _src, char* _dest, int _start, int _stop){
    char* p = &_dest[0];
    while (_start < _stop){
        *p++ = _src[_start];
        //*p++;
        _start++;
    }
    *p = '\0';
}

void string_sub(const char* _src, char* _dest, const char* _pSrcStart, const char* _pSrcEnd){
    int len = _pSrcEnd-_pSrcStart;
    while (len-- > 0){
        *_dest++ = *_pSrcStart++;
    }
    *_dest = '\0';
}

const char* string_find(const char* _str, const char _c){
    const char* p = _str;
    while (*p != '\0'){
        if (*p == _c){
           return p;
        }
        p++;
    }
    return nullptr;
}

void TsubStringRef::copy(char* _dest){
    initIterate();
    while (hasNext()){
        *_dest++ = *Frun++;
    }
    *_dest = '\0';
}
