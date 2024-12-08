#ifndef UENUMMACROS_h
#define UENUMMACROS_h

#include "uSpookyMacros.h"
#include "uStrings.h"

namespace sdds{

	namespace metaTypes{

		/**
		 * @brief Baseclass used in sdds_enumClass to reduce code produce by macro magic
		 * 
		 * @tparam t_enum a C++ enum class
		 * @tparam _COUNT number of elements in the enum class
		 */
		template <class Tinfo>
		class TenumClaseBase{
			public:
				typedef uint8_t enumOrdType;

				typedef typename Tinfo::enum_type e;
				constexpr static int ENUM_BUFFER_SIZE = Tinfo::ENUM_BUFFER_SIZE;
				constexpr static const int COUNT = Tinfo::COUNT;

				typename Tinfo::enum_type Fvalue;  

				operator e() const{ return Fvalue; }
				
				static const enumOrdType ord(e _v){return static_cast<enumOrdType>(_v);}

				bool strToVal(const char* _str){
					uStrings::TstringArrayIterator it(Tinfo::enum_str());
					for (int i=0; it.hasNext(); i++){
						if (strcmp(it.next(),_str) == 0){
							Fvalue = static_cast<e>(i);
							return true;
						};
					}

					int val; TstringRef s(_str);
					if (!s.parseValue(val) || (s.hasNext())) return false;
					Fvalue = static_cast<e>(val);
					return true;
				}
		
				static const char* c_str(e _v){
					enumOrdType idx = static_cast<enumOrdType>(_v);
					if (idx < COUNT)
						return uStrings::getStringN(Tinfo::enum_str()+1,idx);
					return "unknown";
				}
		        const char* c_str(){ return TenumClaseBase<Tinfo>::c_str(Fvalue); }
		
				uStrings::TstringArrayIterator iterator() { return uStrings::TstringArrayIterator(Tinfo::enum_str()+1); }
		
				constexpr const char* enumBuffer(){ return Tinfo::enum_str(); }

				const char* getEnum(int _idx) { return uStrings::getStringN(Tinfo::enum_str()+1,_idx); }
		};
	}
}

/**
 * @brief 
 * notes:
 * 	enum_str(): returns a string containing all enums in form \0en1\0en2\0...enM\0\0 with an additional \0 at the
 * 		very end. That's why we set ENUM_BUFFER_SIZE=sizeof(enum_str())-1
 */

#define __SDDS_ENUM_STR_ENTRY(_str) #_str "\0"

#define sdds_enumClass(_name,...)\
	class _name##_meta{\
		public:\
			enum class enum_type : uint8_t {__VA_ARGS__};\
			constexpr static int COUNT = sdds_SM_COUNT_VARARGS(__VA_ARGS__);\
			constexpr static int ENUM_BUFFER_SIZE = sizeof("\0" sdds_SM_ITERATE(__SDDS_ENUM_STR_ENTRY,__VA_ARGS__))-1;\
			constexpr static const char* enum_str(){ return "\0" sdds_SM_ITERATE(__SDDS_ENUM_STR_ENTRY,__VA_ARGS__); }\
	\
	};\
	class _name : public sdds::metaTypes::TenumClaseBase<_name##_meta>{\
		public:\
			void operator=(e _value){Fvalue = _value;}\
	};

#endif // UENUMMACROS_h
