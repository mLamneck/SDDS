#ifndef USDDSTOSTRING_H
#define USDDSTOSTRING_H

/*******************************************************************************
 * uSddsToString
 * 
 * This module provides functions for converting SDDS (Self-Describing-Data-Structure) 
 * variables into strings. Previously, this conversion was handled by overriding 
 * a `to_string` method in derived classes of `Tdescr`. However, this approach 
 * caused issues with compiler optimizations: if no `to_string` method was used, 
 * the compiler could not eliminate the associated code, leading to unnecessary 
 * code bloat and increased flash memory usage—especially on small devices where 
 * string conversion was not needed.
 * 
 * To address this, the string conversion logic has been refactored into this 
 * separate module, allowing more efficient code generation and reducing flash 
 * memory usage for cases where string translation is not required.
 ******************************************************************************/

#include <stdio.h>
#include "uTypedef.h"

namespace sdds{

	template <typename T>
	void to_string_fmt(dtypes::string& _out, T _val, const char* _fmt){
		char buffer[64];
		auto idx = snprintf(buffer, sizeof(buffer), _fmt, _val);
		if (idx < 0 || idx > static_cast<int>(sizeof(buffer)-1)){
			_out = "";
			return;
		}
		uStrings::assign(_out, buffer, idx);
	}

	/****************** */
	// integers

	void to_string(dtypes::string& _out, dtypes::uint8 _val) 	{ to_string_fmt(_out,_val,"%u"); }
	void to_string(dtypes::string& _out, dtypes::uint16 _val)	{ to_string_fmt(_out,_val,"%u"); }
	void to_string(dtypes::string& _out, dtypes::uint32 _val)	{ to_string_fmt(_out,_val,"%u"); }
	void to_string(dtypes::string& _out, dtypes::int8 _val)		{ to_string_fmt(_out,_val,"%d"); }
	void to_string(dtypes::string& _out, dtypes::int16 _val)	{ to_string_fmt(_out,_val,"%d"); }
	void to_string(dtypes::string& _out, dtypes::int32 _val)	{ to_string_fmt(_out,_val,"%d"); }

	/****************** */
	// float

	inline void to_string(dtypes::string& _out, dtypes::float32 _val){
		to_string_fmt(_out,_val,"%.4g");
	}

	/****************** */
	// hex/binary formatting

	inline void to_string_hex(dtypes::string& _out, const void* _pVal, int valSize){
		_out = "0x";
		char hexDigit[3];
		hexDigit[2] = '\0';
		auto pVal = static_cast<const dtypes::uint8*>(_pVal)+valSize-1;
		while (valSize-->0){
			sprintf(&hexDigit[0], "%02X", static_cast<unsigned char>(*pVal));
			_out = _out + hexDigit;
			pVal--;
		}
	}

	inline dtypes::string to_string_hex(const void* _pVal, int _valSize){
		dtypes::string res;
		to_string_hex(res,_pVal,_valSize);
		return res;
	}

	inline dtypes::string to_string_hex(Tdescr* _d){
		return to_string_hex(static_cast<dtypes::uint8*>(_d->pValue()),_d->valSize());
	}

	inline void to_string_bin(dtypes::string& _out, const void* _pVal, int valSize){
		_out = "0b";
		auto pVal = static_cast<const dtypes::uint8*>(_pVal) + valSize - 1;
		while (valSize-->0){
			dtypes::uint8 b = *pVal;
			for (int i = 7; i>=0; i--){
				if (b & (1<<i))
					_out = _out + "1";
				else
					_out = _out + "0";
			}
			pVal--;
		}
	}

	inline dtypes::string to_string_bin(const void* _pVal, int _valSize){
		dtypes::string res;
		to_string_bin(res,_pVal,_valSize);
		return res;
	}

	/****************** */
	// enum

    inline void to_string(dtypes::string& _out, TenumBase* _d, dtypes::uint8 _ord){
		auto it = static_cast<TenumBase*>(_d)->enumInfo().iterator;
		const char* enStr = "";
		for (dtypes::uint32 i=0;i<=_ord;i++)
			enStr = it.next();
		_out = enStr;
    }

	/****************** */
	// time

	inline void to_string(dtypes::string& _out, Ttime& _time, Ttime::dtype& _value){
		if (_time.showOption() == sdds::opt::timeRel) _out = timeToString(_value,"%H:%M:%S");
        else _out = timeToString(_value);
	}


	/****************** */
	// by descr

	//integers
	void to_string(dtypes::string& _out, Tuint8& _val) 	{ to_string(_out,_val.value()); }
	void to_string(dtypes::string& _out, Tuint16& _val) { to_string(_out,_val.value()); }
	void to_string(dtypes::string& _out, Tuint32& _val) { to_string(_out,_val.value()); }
	void to_string(dtypes::string& _out, Tint8& _val) 	{ to_string(_out,_val.value()); }
	void to_string(dtypes::string& _out, Tint16& _val) 	{ to_string(_out,_val.value()); }
	void to_string(dtypes::string& _out, Tint32& _val) 	{ to_string(_out,_val.value()); }
	
	//time
	inline void to_string(dtypes::string& _out, Ttime& _time){
		auto dt = static_cast<dtypes::TdateTime*>(_time.pValue());
		if (_time.showOption() == sdds::opt::timeRel) _out = timeToString(*dt,"%H:%M:%S");
        else _out = timeToString(*dt);
	}
	
	//enums
	void to_string(dtypes::string& _out, TenumBase& _val) { return to_string(_out,&_val,*static_cast<dtypes::uint8*>(_val.pValue())); }

	//string
	void to_string(dtypes::string& _out, Tstring& _val) { _out = _val.Fvalue; }	
	
	//struct
	void to_string(dtypes::string& _out, Tstruct& _val) { _out = _val.value() ? ">" : "NIL"; }
	
	/****************** */
	// by Tdescr

	inline void to_string(dtypes::string& _out, Tdescr* _d){
		auto opt = _d->showOption();
		if (opt == opt::showHex) _out = to_string_hex(_d);
		else if (opt == opt::showBin) _out = to_string_bin(_d->pValue(),_d->valSize());
		else{
			switch(_d->type()){
				case sdds::Ttype::FLOAT32 : return to_string(_out,*static_cast<Tfloat32*>(_d));
				case sdds::Ttype::INT8 : 	return to_string(_out,*static_cast<Tint8*>(_d));
				case sdds::Ttype::INT16: 	return to_string(_out,*static_cast<Tint16*>(_d));
				case sdds::Ttype::INT32: 	return to_string(_out,*static_cast<Tint32*>(_d));
				case sdds::Ttype::UINT8: 	return to_string(_out,*static_cast<Tuint8*>(_d));
				case sdds::Ttype::UINT16: 	return to_string(_out,*static_cast<Tuint16*>(_d));
				case sdds::Ttype::UINT32: 	return to_string(_out,*static_cast<Tuint32*>(_d));
				case sdds::Ttype::ENUM: 	return to_string(_out,static_cast<TenumBase*>(_d),*static_cast<dtypes::uint8*>(_d->pValue()));
				case sdds::Ttype::STRING:	return to_string(_out,*static_cast<Tstring*>(_d));	
				case sdds::Ttype::TIME: 	return to_string(_out,*static_cast<Ttime*>(_d));
				case sdds::Ttype::STRUCT: 	return to_string(_out,*static_cast<Tstruct*>(_d));
				default:
					_out = "";
			}
		}
	}

	inline dtypes::string to_string(Tdescr* _d){
		dtypes::string res;
		to_string(res,_d);
		return res;
	}

	template <typename T>
	dtypes::string to_string(T _val) {
		dtypes::string res;
		to_string(res,_val);
		return res;
	}    

}

#endif // USDDSTOSTRING_H
