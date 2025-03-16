#ifndef UMEMORYUTILS_H
#define UMEMORYUTILS_H

#include <type_traits>

namespace sdds{
	namespace memUtils{

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
		};


		/**
		 * @file uSingletonContainer.h
		 * 
		 * @brief This header file defines the `TsingletonContainer` template class.
		 * 
		 * The `TsingletonContainer` class is a container for managing a single instance of 
		 * derived classes from a base class `TbaseClass`. It ensures that only one instance 
		 * of a specific type exists in the container at any time, and that type must be selected 
		 * from a predefined list of types (`Ts...`). The class also provides mechanisms to ensure 
		 * that only valid types from this list can be instantiated.
		 * 
		 * @tparam TbaseClass The base type from which all allowed types must derive.
		 * @tparam Ts... A variadic list of types that defines the allowed derived types.
		 * 
		 * @example
		 * 	class TeditorContainer: public TsingletonContainer<TeditorBase
		 *		,TenumEditor
		*		,TintEditor<Tuint8>
		*		,TintEditor<Tuint16>
		*		,TintEditor<Tuint32>
		*		,TintEditor<Tint8>
		*		,TintEditor<Tint16>
		*		,TintEditor<Tint32>
		*		,TintEditor<Tfloat32>
		*		> 
		*	{
		*		public:
		*			TeditorBase* create(Tdescr* _d, const int _displayWidth){
		*				switch(_d->type()){
		*					case sdds::Ttype::INT8:
		*						return TsingletonContainer::create<TintEditor<Tint8>>(_d,_displayWidth);
		*					case sdds::Ttype::INT16:
		*						return TsingletonContainer::create<TintEditor<Tint16>>(_d,_displayWidth);
		*					case sdds::Ttype::INT32:
		*						return TsingletonContainer::create<TintEditor<Tint32>>(_d,_displayWidth);
		*					case sdds::Ttype::UINT8:
		*						return TsingletonContainer::create<TintEditor<Tuint8>>(_d,_displayWidth);
		*					case sdds::Ttype::UINT16:
		*						return TsingletonContainer::create<TintEditor<Tuint16>>(_d,_displayWidth);
		*					case sdds::Ttype::UINT32:
		*						return TsingletonContainer::create<TintEditor<Tuint32>>(_d,_displayWidth);
		*					case sdds::Ttype::ENUM:
		*						return TsingletonContainer::create<TenumEditor>();
		*					case sdds::Ttype::FLOAT32:
		*						return TsingletonContainer::create<TintEditor<Tfloat32>>(_d,_displayWidth);
		*				}
		*				return nullptr;
		*			}
		*
		*	};
		*/

		template <class TbaseClass, typename... Ts>
		class TsingletonContainer {

			/***************************************
			 * retrieve size an alignment for all types
			****************************************/

			template <typename T, typename... Rest>
			struct MaxSizeAlign {
				static constexpr size_t maxSize = 
					sizeof(T) > MaxSizeAlign<Rest...>::maxSize
					? sizeof(T)
					: MaxSizeAlign<Rest...>::maxSize;

				static constexpr size_t maxAlign = 
					alignof(T) > MaxSizeAlign<Rest...>::maxAlign
					? alignof(T)
					: MaxSizeAlign<Rest...>::maxAlign;
			};

			template <typename T>
			struct MaxSizeAlign<T> {
				static constexpr size_t maxSize = sizeof(T);
				static constexpr size_t maxAlign = alignof(T);
			};


			/***************************************
			 * check item is in List
			 ***************************************/

			template <typename X, typename Y, typename... Rest>
			struct ContainsItem{
				static constexpr bool value = std::is_same<X,Y>::value||ContainsItem<X,Rest...>::value;
			};

			template <typename X, typename Y>
			struct ContainsItem<X,Y>{
				static constexpr bool value = std::is_same<X,Y>::value;
			};

			/***************************************/

			static constexpr size_t MAX_SIZE = MaxSizeAlign<Ts...>::maxSize;
			static constexpr size_t MAX_ALIGN = MaxSizeAlign<Ts...>::maxAlign;
			using TcontainerType = typename std::aligned_storage<MAX_SIZE, MAX_ALIGN>::type;

		private:
			TcontainerType Fcontainer;
			TbaseClass* Finstance = nullptr;
			
		public:				
			template <typename T, typename... Args>
			TbaseClass* create(Args&&... args) {
				static_assert(std::is_base_of<TbaseClass, T>::value, "T must derive from Base");
				static_assert(ContainsItem<T,Ts...>::value, "attemp to create an item that is not specified in the list of possible classes for this Singleton");

				if (Finstance) {
					Finstance->~TbaseClass();
				}

				Finstance = new (&Fcontainer) T(std::forward<Args>(args)...);
				return Finstance;
			}

			void destroy(){
				if (Finstance) {
					Finstance->~TbaseClass();
					Finstance = nullptr;
				}
			}

			TbaseClass* getInstance() const {
				return Finstance;
			}

			~TsingletonContainer() {
				destroy();
			}
		};

			}
		};

#endif //UMEMORYUTILS_H