#ifndef USTRINGS_H
#define USTRINGS_H

#include "uPlatform.h"
#include "string.h"

typedef dtypes::string TrawString;

void string_sub(const char* _src, char* _dest, int _start, int _stop);
void string_sub(const char* _src, char* _dest, const char* _pSrcStart, const char* _pSrcEnd);
const char* string_find(const char* _str, const char _c);
inline bool string_cmp(const char* _s1, const char* _s2){ return (strcmp(_s1,_s2)==0); }

template <class _TstringRef>
bool string_cmp(_TstringRef& _s1, const char* _s2){
    _s1.initIterate();
    while (*_s2 != '\0'){
        if (_s1.next() != *_s2) { return false; }
        _s2++;
    }
    return (!_s1.hasNext());
};

class TstringRef{
    protected:
        const char* Fstr = nullptr;
        const char* Frun = nullptr;
    public:

        TstringRef(const char* _str){
            Fstr = _str;
            initIterate();
        }
    public:
        inline const char* pCurr(){ return Frun; }
        inline void initIterate(){ Frun = Fstr; }
        inline bool hasNext(){ return (*Frun != '\0'); }
        inline char curr() { return *Frun; }
        inline char next() { return hasNext()? *Frun++ : '\0'; }
        void offset(int _ofs){ 
            if (_ofs < 0){
                Frun = (Frun + _ofs)>=Fstr? (Frun + _ofs) : Fstr;
            }
        }
        inline bool operator==(const char* _str){
            return string_cmp(*this,_str);
        }
};

class TsubStringRef : public TstringRef{
    private:
        const char* Fend;
    public:
        TsubStringRef(const char* _str, const char* _end) : TstringRef(_str){
            Fend = (_end > _str)?_end:_str;
        }
        inline bool hasNext(){ 
            return (TstringRef::hasNext() && Frun < Fend);
        }
        inline char next(){ return hasNext()? *Frun++ : '\0'; }
        
        void copy(char* _dest);

        inline bool operator==(const char* _str){
            return string_cmp(*this,_str);
        }
};



#endif