#ifndef USTRINGS_H
#define USTRINGS_H

#include "uPlatform.h"
#include "string.h"

typedef dtypes::string TrawString;

void string_sub(const char* _src, char* _dest, int _start, int _stop);
void string_sub(const char* _src, char* _dest, const char* _pSrcStart, const char* _pSrcEnd);
const char* string_find(const char* _str, const char _c);
inline bool string_cmp(const char* _s1, const char* _s2){ return (strcmp(_s1,_s2)==0); }

namespace uStrings{
	inline void assign(dtypes::string& _target, const char* _buf, int _nBytes){
		#if SDDS_ON_ARDUINO == 1
			_target = "";
			//_target.concat(reinterpret_cast<const char*>(_buf),_nBytes);
			while (_nBytes-- > 0) _target += *_buf++;
		#else
			_target.assign(_buf, _nBytes);
		#endif
	}

	inline void append(dtypes::string& _target, const char* _buf, int _nBytes){
		#if SDDS_ON_ARDUINO == 1
			while (_nBytes-- > 0) _target += *_buf++;
			//_target.concat(reinterpret_cast<const char*>(_buf),_nBytes);
		#else
			_target.assign(reinterpret_cast<const char*>(_buf), _nBytes);
		#endif
	}

	/**
	 * @brief Get the String N object from an array of char in the 
	 * form "str1\0str2\0...stringN\0\0"
	 * 
	 * @param _inp 
	 * @param _n nth string to return. 0 returns _inp
	 * @return if (_n<strings in input) return the nTh string in fromArray, nullptr otherwise.
	 */
	inline const char* getStringN(const char* _inp, int _n){
		while (_n-- > 0){
			while (*_inp != '\0') _inp++;
			if (*(++_inp) == '\0') return nullptr;  
		}
		return _inp;
	}

	struct TstringArrayIterator{
		const char* FcurrStr;
		TstringArrayIterator(const char* _s){
			FcurrStr = _s;
		}
		bool hasNext(){ return FcurrStr; }
		const char* next(){
			auto temp = FcurrStr;
			FcurrStr = getStringN(FcurrStr,1);
			return temp;
		}
	};
}

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
        virtual bool hasNext(){ return (*Frun != '\0') && (*Frun != '\n'); }
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

        bool parseHexValue(dtypes::uint32& _out) {
            if (!hasNext()) return false;

            dtypes::uint32 val = 0;
            const char* pstart = Frun;
            do {
                char ch = curr();
                dtypes::uint8 c;

                if (ch >= '0' && ch <= '9') {
                    c = ch - '0';
                } else if (ch >= 'a' && ch <= 'f') {
                    c = 10 + (ch - 'a');
                } else if (ch >= 'A' && ch <= 'F') {
                    c = 10 + (ch - 'A');
                } else {
                    break;
                }

                val = (val << 4) | c;
                next();
            } while (hasNext());

            if (Frun == pstart) return false;
            _out = val;
            return true;
        }

        template <typename T>
        bool parseHexValue(T& _out) {
            dtypes::uint32 val;
            if (!parseHexValue(val)) return false;
            if ( val > (1ULL << (sizeof(T)*8)) - 1 ) return false; // Overflow check
            _out = static_cast<T>(val);
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
			initIterate();
        }
		void init(const void* _str, const int _length){
			const char* str = static_cast<const char*>(_str);
			Fstr = str;
			//BUGFIX 06.09.2024 see comment above
            //Fend = str + (_length > 0? _length-1 : 0);
			Fend = str + (_length > 0? _length : 0);
			initIterate();
		}

		int length() { return (Fend - Fstr); }
		const char* c_str() { return Fstr; }

        bool hasNext() override{
			//BUGFIX 04.09.2024 Frun <= Fend -> otherwise we cannot read the last char!
            //return (TstringRef::hasNext() && Frun < Fend);
            //return (TstringRef::hasNext() && Frun <= Fend);

			//BUGFIX 06.09.2024 change back... last error was due to wrong impl of init (_str,_length)
            return (TstringRef::hasNext() && Frun < Fend);
        }

        void copy(char* _dest);

        inline bool operator==(const char* _str){
            return string_cmp(*this,_str);
        }
};



#endif
