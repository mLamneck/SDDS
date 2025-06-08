#include "uJsonSerializer.h"

#include "uTypedef.h"
#include "uStream.h"

using namespace sdds;

TjsonSerializer::TjsonSerializer(TmenuHandle* _root, Tstream* _stream, bool _withValue){
	Froot = _root;
	Fstream = _stream;
	FwithValue = _withValue;
}

void TjsonSerializer::serializeAsString(Tstream* _stream, const char* _str){
	_stream->write('"');
	TjsonSerializer::encodeString(_stream,_str);
	_stream->write('"');
}

void TjsonSerializer::serializeAsString(Tstream* _stream, Tdescr* _d){
	TjsonSerializer::serializeAsString(_stream,_d->to_string().c_str());
}

void TjsonSerializer::serializeAsString(Tdescr* _d){
	TjsonSerializer::serializeAsString(Fstream,_d);
}

void TjsonSerializer::encodeString(Tstream* _stream, const char* _str){
	while (*_str != '\0'){
	switch(*_str){
		case '"': case '\\': case '/':
		case '\b': case '\f': case '\n':
		case '\r': case '\t':
		_stream->write('\\');
		break;
	}
	_stream->write(*_str++);
	}
}

void TjsonSerializer::serializeValues(Tstream* _stream, TmenuHandle* _struct, TrangeItem _first, TrangeItem _last){
	_stream->write("[");
	auto it = _struct->iterator(_first);
	if (it.hasCurrent()){
		do{
		Tdescr* d = it.current();
		switch(d->type()){
			case(Ttype::STRING): case(Ttype::TIME): case(Ttype::ENUM):
				TjsonSerializer::serializeAsString(_stream,d);
				break;
			case (Ttype::FLOAT32):
				if (static_cast<Tfloat32*>(d)->isNan())
					TjsonSerializer::serializeAsString(_stream,"nan");
				else	
					_stream->write(d->to_string().c_str());
				break;
			default:
				_stream->write(d->to_string().c_str());
				break;
		}
		if (!it.jumpToNext() || (_first++ >= _last)) break;
		_stream->write(',');
		} while(true);
	}
	_stream->write("]");
}

void TjsonSerializer::serializeEnums(Tstream* _stream, TenumBase* _enum){
	_stream->write('[');
	TenumBase* en = static_cast<TenumBase*>(_enum);
	for (auto it=en->enumInfo().iterator;;){
		TjsonSerializer::serializeAsString(_stream,it.next());
		if (!it.hasNext()) break;
		_stream->write(',');
	}
	_stream->write(']');
}

void TjsonSerializer::serializeTypeDescr(Tdescr* _d){
	writeKey(FjsonTypeKey);
	Fstream->write(_d->typeId());
	Fstream->write(',');
	if (!skipDefaultOption || _d->option() != 0)
	{
		writeKey(FjsonOptKey);
		Fstream->write(_d->option());
		Fstream->write(',');
	}
	writeKey(FjsonNameKey);
	Fstream->write('"');
	Fstream->write(_d->name());
	Fstream->write('"');

	//for enums, write list of enums
	if (_d->type() == sdds::Ttype::ENUM){
		auto en = static_cast<TenumBase*>(_d);
		Fstream->write(',');
		writeKey(FjsonEnumKey);
		if (FreferEnums && FenumIds.knows(en->enumInfo().id))
			Fstream->write(en->enumInfo().id);
		else
			serializeEnums(Fstream,en);
	}
}

void TjsonSerializer::_serialize(TmenuHandle* _curr){
	Fstream->write('[');
	auto it = _curr->iterator();
	while (it.hasCurrent()){
		Tdescr* d = it.current();

		Fstream->write('{');
		serializeTypeDescr(d);
		if (d->isStruct()){
			Fstream->write(',');
			writeKey(FjsonValueKey);
			TmenuHandle* mh = static_cast<Tstruct*>(d)->value();
			if (mh) _serialize(mh);
			else Fstream->write("null");
		}

		else if (FwithValue){
			Fstream->write(',');
			writeKey(FjsonValueKey);
			switch(d->type()){
			case sdds::Ttype::ENUM: case sdds::Ttype::STRING: case sdds::Ttype::TIME:
				serializeAsString(d);
				break;
			default:
				Fstream->write(d->to_string().c_str());
			}
		}
		Fstream->write('}');
		if (it.jumpToNext()) Fstream->write(',');
	}
	Fstream->write(']');
}

bool TjsonSerializer::_serializeEnums(TmenuHandle* _curr){
	for (auto it = _curr->iterator(); it.hasCurrent(); it.jumpToNext()){
		Tdescr* d = it.current();

		if (d->isStruct()){
			TmenuHandle* mh = static_cast<Tstruct*>(d)->value();
			if (mh) 
				if (!_serializeEnums(mh)) return false;
		}
		if (d->type() == sdds::Ttype::ENUM){
			auto en = static_cast<TenumBase*>(d);
			auto uid = en->enumInfo().id;
			if (!FenumIds.knows(uid)){
				if (!FenumIds.remember(uid)) return false;
				if (FenumIds.count() > 1)
					Fstream->write(',');
				writeKey(uid);
				serializeEnums(Fstream,en);
			}
		}
	}
	return true;
}

void TjsonSerializer::serialize(TmenuHandle* _curr){
	if (!FreferEnums) return _serialize(_curr);

	Fstream->write('{');
	
	writeKey('e');
	Fstream->write('{');
	_serializeEnums(_curr);
	Fstream->write('}');

	Fstream->write(',');
	writeKey('d');
	_serialize(_curr);

	Fstream->write('}');
}

void TjsonSerializer::serialize(){ 
	serialize(Froot); 
}
