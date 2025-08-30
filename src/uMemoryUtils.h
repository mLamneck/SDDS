#ifndef UMEMORYUTILS_H
#define UMEMORYUTILS_H

#include "uPlatform.h"

namespace sdds{
	namespace memUtils{

		void swap_endianness(void* _buf, int _nBytes){
			uint8_t* p = reinterpret_cast<uint8_t*>(_buf);
			for (int i=0; i<_nBytes/2; i++){
				uint8_t temp = p[_nBytes-i-1];
				p[_nBytes-i-1] = p[i];
				p[i] = temp;
			}
		}

		template <typename T>
		void swap_endianness(T& value){
			swap_endianness(&value,sizeof(T));
		}


		/**
		 * @class TbufferStream
		 * @brief A simple buffer-based stream for reading and writing data.
		 *
		 * This class provides methods for managing a memory buffer that can be used for
		 * sequential reading and writing operations. It supports basic operations such
		 * as writing and reading primitive data types, checking available space, and moving data.
		 */
		class TbufferStream{
			typedef dtypes::uint8 uint8;
			protected:
				dtypes::uint8* Fbuffer = nullptr;
				int FbufferSize = 0;
				int FbytesAvailableForRead = 0;
				int FreadPos = 0;
				int FwritePos = 0;
			public:
				constexpr dtypes::uint8* buffer() { return Fbuffer; }

				int writePos() { return FwritePos; }
				void setWritePos(int _val) { FwritePos = _val; }
				int readPos() { return FreadPos; }
				void setReadPos(int _val) { FreadPos = _val; }
				void setAvailableForRead(int _available){ FbytesAvailableForRead = _available; }

				void init(dtypes::uint8* _buffer, int _size, int _availableForRead = 0){
					Fbuffer = static_cast<uint8*>(_buffer);
					FbufferSize = _size;
					FbytesAvailableForRead = _availableForRead;
					FreadPos = 0;
					FwritePos = 0;
				}

				void move(int _nBytes){
					memcpy(&Fbuffer[FwritePos], &Fbuffer[FreadPos], _nBytes);
					FreadPos+=_nBytes;
					FwritePos+=_nBytes;
				}

				int bytesAvailableForRead() { return FbytesAvailableForRead - FreadPos; }
				
				bool hasSpaceFor(const uint8 _size){
					return ((FwritePos + _size) <= FbufferSize);
				}

				int spaceAvailableForWrite() { return FbufferSize - FwritePos; }
				
				bool writeByte(const uint8 _byte){ return writeVal(_byte); }
				bool writeWord(const dtypes::uint16 _val){ return writeVal(_val); }
				
				void writeBytesNoCheck(const void* _ptr, int _n){
					memcpy(&Fbuffer[FwritePos], _ptr, _n);
					FwritePos += _n;
				}

				int writeBytes(const void* _ptr, int _n, bool _skipIfdoesntFit = false){
					auto spaceAvailable = spaceAvailableForWrite();
					if (_n > spaceAvailable){
						if (_skipIfdoesntFit) return 0;
						_n = spaceAvailable;
					} 
					memcpy(&Fbuffer[FwritePos], _ptr, _n);
					FwritePos += _n;
					return _n;
				}

				int readBytes(void* _dst, int _n){
					if (bytesAvailableForRead() < _n) return 0;
					memcpy(_dst, &Fbuffer[FreadPos], _n);
					FreadPos += _n;
					return _n;
				}

				template <typename T>
				bool writeVal(const T _value){
					if (!hasSpaceFor(sizeof(T))) return false;
					memcpy(&Fbuffer[FwritePos], &_value, sizeof(T));
					FwritePos += sizeof(T);
					return true;
				}

				template <typename T>
				bool readVal(T& _value){
					if (bytesAvailableForRead() < (int)sizeof(T)) return false;
					memcpy(&_value,&Fbuffer[FreadPos], sizeof(T));
					FreadPos += sizeof(T);
					return true;
				}

				template<typename T>
				bool writeVal_be(T _value){
					swap_endianness(&_value,sizeof(T));
					return writeVal(_value);
				}

				template<typename T>
				bool readVal_be(T& _value) {
					if (!readVal(_value)) return false;
					swap_endianness(_value);
					return true;
				}

				/* this can be used to load data from complex objects. This will be usefull if we use the protocoll
					on udp where Taddr contains IP and Port. It might be event useful now to declare Taddr as class
					and have methods like isBroadcast on it. But I have no time to test this now
				template <typename T>
				typename std::enable_if<std::is_trivially_copyable<T>::value && !std::is_class<T>::value && sizeof(T) == 1, bool>::type
				readVal(T& _value){
					if (bytesAvailableForRead() < (int)sizeof(T)) return false;
					_value = Fbuffer[FreadPos];
					FreadPos += sizeof(T);
					return true;
				}
				
				template <typename T>
				typename std::enable_if<std::is_trivially_copyable<T>::value && !std::is_class<T>::value && sizeof(T) != 1, bool>::type
				readVal(T& _value){
					if (bytesAvailableForRead() < (int)sizeof(T)) return false;
					memcpy(&_value,&Fbuffer[FreadPos], sizeof(T));
					FreadPos += sizeof(T);
					return true;
				}

				template <typename T>
				typename std::enable_if<std::is_class<T>::value, bool>::type
				readVal(T& _value) {
					//toBe done ....
					//if (!_value.deserialize(buffer + readPos, size - readPos)) return false;
					//readPos += sizeof(T);
					//static_assert(false,"not implemented");
					static_assert(!std::is_class<T>::value,"not implemented");
					return false;
				}

				*/

				bool readOfs(dtypes::uint8 _ofs){
					if (_ofs >= bytesAvailableForRead()) return false;
					FreadPos += _ofs;
					return true;
				}

				template <typename T, int OFS>
				void writeValToOfs(const T _val){
					memcpy(&Fbuffer[OFS], &_val, sizeof(T));
				};

				template <int OFS>
				constexpr void writeValToOfs(const dtypes::uint8 _val){
					Fbuffer[OFS]=_val;
				}

				template <typename T>		
				void writeValToOfs(int _ofs, const T _val){
					memcpy(&Fbuffer[_ofs], &_val, sizeof(T));
				}

				void writeValToOfs(int _ofs, const dtypes::uint8 _val){
					Fbuffer[_ofs] =  _val;
				}

				void writeByteToOfs(int _ofs, const dtypes::uint8 _val){
					Fbuffer[_ofs] =  _val;
				}

				template <typename T, int OFS>
				T readValFromOfs() {
					T value;
					memcpy(&value, &Fbuffer[OFS], sizeof(T));
					return value;
				}

				template <int OFS>
				constexpr dtypes::uint8 readValFromOfs() {
					return Fbuffer[OFS];
				}
		}; //TbufferStream

	} //namespace memUtils
} //namespace sdds

#endif //UMEMORYUTILS_H
