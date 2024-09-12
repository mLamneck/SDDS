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
	if (_s1.isNil() || !_s2) return false;

    _s1.initIterate();
    while (*_s2 != '\0'){
        if (_s1.next() != *_s2) { 
			return false; 
		}
        _s2++;
    }
	auto res =(!_s1.hasNext()); 
    return res;
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
		bool isNil(){ return !Fstr; }
        inline const char* pCurr(){ return Frun; }
        inline void initIterate(){ Frun = Fstr; }
        inline bool hasNext(){ return (*Frun != '\0') && (*Frun != '\n'); }
        inline char curr() { return *Frun; }
        inline char next() { return hasNext()? *Frun++ : '\0'; }
        inline char get(int _ofs) {
            const char* destAddr = Frun+_ofs;
            if (_ofs > 0){
                //this is not tested!!!
                auto currRun = Frun;
                while (hasNext() && (_ofs-->0)) next();
                char res = curr();
                Frun = currRun;
                return res;
            }
            return (destAddr >= Fstr)? (*destAddr) : '\0';
        }
        void offset(int _ofs){
            if (_ofs < 0){
                Frun = (Frun + _ofs)>=Fstr? (Frun + _ofs) : Fstr;
            }
        }

        /** \brief parse an unsigned integer into _out, a call to curr() retrieves the character at which the parsing was stopped.
         *
         * \param _out an integer receiving the value
         * \return true if an integer could be parsed, false otherwise
         *
         */
        bool parseValue(dtypes::uint32& _out){
            if (!hasNext()) return false;               //initial checks

            dtypes::uint32 val = 0;
            const char* pstart = Frun;
            do{
                dtypes::uint8 c = curr() - 0x30;        //overflow if c is less than 0x30
                if (c > 9) break;                       //break if c is not in [0..9]
                val = val*10 + c;
                next();
            } while (hasNext());

            if (Frun == pstart) return false;           //return false if not at least 1 char was processed
            _out = val;
            return true;
        }

        template <typename T>
        bool parseValue(T& _out){
            dtypes::uint32 val;
            if (!parseValue(val)) return false;
            if ( val > (1ULL << (sizeof(T)*8))-1 ) return false;        //ULL -> Unsigned Long Long to get rid of warnings shift overflow
            _out = val;
            return true;
        }

        inline bool operator==(const char* _str){
            return string_cmp(*this,_str);
        }
};

class TsubStringRef : public TstringRef{
    private:
        const char* Fend;
    public:
		TsubStringRef() : TstringRef(nullptr){
			Fend = nullptr;
		}
        TsubStringRef(const char* _str, const char* _end) : TstringRef(_str){
            Fend = (_end > _str)?_end:_str;
        }
        TsubStringRef(const char* _str, const int _length) : TstringRef(_str){
			//BUGFIX 06.09.2024 FendStr has to be > Fstr for length > 0
			//if _length=0 -> Fend=Fstr
			//if _length=1 -> Fend=Fstr+1 
			//Fend = _str + (_length > 0? _length-1 : 0);
			Fend = _str + (_length > 0? _length : 0);
        }
		void init(const void* _str, const int _length){
			const char* str = static_cast<const char*>(_str);
			Fstr = str;
			//BUGFIX 06.09.2024 see comment above
            //Fend = str + (_length > 0? _length-1 : 0);
			Fend = str + (_length > 0? _length : 0);
		}

		int length() { return (Fend - Fstr); }
		const char* c_str() { return Fstr; }

        inline bool hasNext(){
			//BUGFIX 04.09.2024 Frun <= Fend -> otherwise we cannot read the last char!
            //return (TstringRef::hasNext() && Frun < Fend);
            //return (TstringRef::hasNext() && Frun <= Fend);

			//BUGFIX 06.09.2024 change back... last error was due to wrong impl of init (_str,_length)
            return (TstringRef::hasNext() && Frun < Fend);
        }
        inline char next(){ return hasNext()? *Frun++ : '\0'; }

        void copy(char* _dest);

        inline bool operator==(const char* _str){
            return string_cmp(*this,_str);
        }
};



#endif
