#ifndef UJSONSERIALIZER_H
#define UJSONSERIALIZER_H

#include "uTypedef.h"
#include "uStream.h"

template<int MAX_NUMBER>
class TnumberMemory {
private:
	typedef unsigned int TnativeInt;

	static constexpr int BITS_PER_NATIVE_INT = sizeof(TnativeInt) * 8;
	static constexpr int NUM_NATIVE_INTS = (MAX_NUMBER + BITS_PER_NATIVE_INT - 1) / BITS_PER_NATIVE_INT;
	TnativeInt Fbits[NUM_NATIVE_INTS] = { 0 };
	TnativeInt Fcount = 0;

public:
	TnativeInt count() { return Fcount; }

	bool remember(unsigned int _num) {
		if (_num >= MAX_NUMBER) 
			return false;
		TnativeInt index = _num / BITS_PER_NATIVE_INT;
		TnativeInt bitPosition = _num % BITS_PER_NATIVE_INT;
		TnativeInt bitMask = 1u << bitPosition;
		Fbits[index] |= bitMask;
		Fcount++;
		return true;
	}

	bool knows(unsigned int _num) const {
		if (_num >= MAX_NUMBER)
			return false;
		TnativeInt index = _num / BITS_PER_NATIVE_INT;
		TnativeInt bitPosition = _num % BITS_PER_NATIVE_INT;
		TnativeInt bitMask = 1u << bitPosition;
		return (Fbits[index] & bitMask) != 0;
	}
};

class TjsonSerializer{
  private:
	typedef int TrangeItem;
	constexpr static int TrangeItem_max = 255;//dtypes::high<TrangeItem>();

	TnumberMemory<64> FenumIds;
	
	TmenuHandle* Froot;
	Tstream* Fstream;
	bool FwithValue;
  public:
	template<typename T>
	void writeKey(T _key){
		Fstream->write('"');
		Fstream->write(_key);
		Fstream->write('"');
		Fstream->write(':');
	}

	TjsonSerializer(TmenuHandle* _root, Tstream* _stream, bool _withValue=true);

	static void encodeString(Tstream* _stream, const char* _str);
	
	static void serializeAsString(Tstream* _stream, const char* _str);

	static void serializeAsString(Tstream* _stream, Tdescr* _d);

	void serializeAsString(Tdescr* _d);

	static void serializeFloat32(Tstream* _stream, Tfloat32* _f);
	
	static void serializeValues(Tstream* _stream, TmenuHandle* _struct, TrangeItem _first=0, TrangeItem _last=TrangeItem_max);

	static void serializeEnums(Tstream* _stream, TenumBase* _enum);

	void serializeTypeDescr(Tdescr* _d);

	bool _serializeEnums(TmenuHandle* _curr);

	void _serialize(TmenuHandle* _curr);

	void serialize(TmenuHandle* _curr);

	void serialize();
};

class TjsonStringSerializer : public TjsonSerializer{
  TstringStream FstringStream;
  public:
	TstringStream stream() { return FstringStream; }
	const char* c_str() { return FstringStream.data(); }
	TjsonStringSerializer(TmenuHandle* _root, bool _withValue=true) 
	  : TjsonSerializer(_root,&FstringStream,_withValue)
	{

	}
};

#endif
