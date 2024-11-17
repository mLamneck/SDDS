#ifndef UPARAMSAVE_H
#define UPARAMSAVE_H

#define uParamSave_debug 0

#if defined(SDDS_ON_ARDUINO)
    //to be checked
    #if defined(ESP32) || defined(ESP8266)
        #define SDDS_EEPROM_COMMIT 1
        #define SDDS_EEPROM_BEGIN_WITH_SIZE 1
    #else
        #define SDDS_EEPROM_COMMIT 0
        #define SDDS_EEPROM_BEGIN_WITH_SIZE 0
    #endif
#endif

#ifndef SDDS_PS_MAX_SIZE
	#define SDDS_PS_MAX_SIZE 2048
#endif

#include "uTypedef.h"
#include "uCrc8.h"

/*****************************************************************************/
//includes depending on platform
/*****************************************************************************/

#if SDDS_ON_ARDUINO == 1
#include <EEPROM.h>
#elif defined(STM32_CUBE)
#include <mhal/uFlashProg.h>
#elif MARKI_DEBUG_PLATFORM == 1
/*
    on debug plattform we store to filesystem.
 */
#include <iostream>
#include <fstream>
#endif


/*****************************************************************************/
//global definitions
/*****************************************************************************/

sdds_enum(___,crc,invStrLen,outOfMem,invVers) TparamError;
sdds_enum(___,load,save) TenLoadSave;


namespace sdds{
	namespace paramSave{
		enum class TseekMode {start, end, curr};

		typedef int16_t TstreamLength;

		class TstreamBase{
			private:
				TstreamLength Frun = 0;
				TstreamLength FhighWater = 0;

			protected:
				TstreamLength curr() { return Frun; };

				virtual TstreamLength availableForRead() = 0;
				virtual bool doWriteByte(uint8_t _byte) = 0;
				virtual bool doReadByte(uint8_t& _byte) = 0;
			public:
				static void INIT(){ }
				virtual void initRead() { }
				virtual void initWrite() { }

				bool writeByte(uint8_t _byte);

				bool readByte(uint8_t& _byte);

				bool writeBytes(const void* _buf, uint8_t _len);

				bool readBytes(void* _buf, uint8_t _len);

				bool seek(TseekMode _mode, int _pos);

				virtual bool grow(int _size){ return true; }

				virtual void flush(){};

				TstreamLength high(){ return FhighWater; }
		};

#if SDDS_ON_ARDUINO == 1
		class TeepromStream : public TstreamBase{
				bool grow(int _size) override{ return (_size <= SDDS_PS_MAX_SIZE); }

				virtual TstreamLength availableForRead() override { return SDDS_PS_MAX_SIZE - curr(); }

				bool doWriteByte(uint8_t _byte) override{
					EEPROM.put(curr(),_byte);
					return true;
				}

				bool doReadByte(uint8_t& _byte) override {
					EEPROM.get(curr(),_byte);
				}

				void flush() override {
					#if SDDS_EEPROM_COMMIT == 1
					EEPROM.commit();
					#endif
				}
			public:
				static void INIT() {
					#if SDDS_EEPROM_BEGIN_WITH_SIZE == 1
					EEPROM.begin(SDDS_PS_MAX_SIZE);
					#else
					EEPROM.begin();
					#endif
				}
		};
		typedef TeepromStream Tstream;
#else

		class TstringStream : public TstreamBase{
			protected:
				dtypes::string Fbuffer;
				virtual TstreamLength availableForRead() override { return Fbuffer.length() - curr(); }

				bool grow(int _size) override{
					while(Fbuffer.length() < static_cast<dtypes::uint32>(_size)) Fbuffer+='\0';
					return true;
				}

				bool doWriteByte(uint8_t _byte) override{
					Fbuffer[curr()] = _byte;
					return true;
				}

				bool doReadByte(uint8_t& _byte) override {
					_byte = Fbuffer[curr()];
					return true;
				}

			public:
				TstringStream(){ Fbuffer.reserve(256); }
				dtypes::string str() { return Fbuffer; }
		};

#if MARKI_DEBUG_PLATFORM == 1
		class TfileStream : public TstringStream{
				const char* Ffilename;

				void flush() override {
					std::ofstream outfile;
					outfile.open(Ffilename,std::ios::binary);
					outfile.write(Fbuffer.c_str(),Fbuffer.length());
					outfile.close();
				}

				void initRead() override{
					std::ifstream ifs(Ffilename);
					Fbuffer = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
					ifs.close();
				}

			public:
				TfileStream(){
					//toDo: make it relative to project
					Ffilename = "c:\\temp\\params.txt";
				}
		};
		typedef TfileStream Tstream;

#elif defined(STM32_CUBE)
		class TflashStream : public TstringStream{
				constexpr static dtypes::uint32 PS_STARTADDR = 0x0807F000; //toDo: declare in platform file
				virtual TstreamLength availableForRead() override { return SDDS_PS_MAX_SIZE - curr(); }

				bool doReadByte(uint8_t& _byte) override {
					return mhal::TflashProg::readByte(PS_STARTADDR+curr(),_byte);
				}

				void flush() override {
					mhal::TflashProg f;
					f.Write(PS_STARTADDR, Fbuffer.c_str(), Fbuffer.length());
				}
		};
		typedef TflashStream Tstream;
#endif

#endif	// SDDS_ON_ARDUINO == 1

		typedef uint8_t TparamSaveVersion;

		struct TparamHeaderV0{
				crc8::Tcrc crc;
		};

		union TstructCrc{
				struct{
						crc8::Tcrc Ftype;
						crc8::Tcrc Fname;
				} rCrc;
				uint16_t crc;
		};

		class TparamStreamer{
				TparamError Ferror;
				TstreamBase* Fstream;
				TstructCrc Fcrc;

				dtypes::uint16 _calcCrc(TmenuHandle* s, dtypes::uint16 _cnt);

				void calcCrc(TmenuHandle* s){ Fcrc.crc=0; _calcCrc(s,0); }

				bool writeByte(dtypes::uint8 _byte);

				bool saveStruct(TmenuHandle* s);

				bool readByte(dtypes::uint8& _byte);

				bool _loadStruct(TmenuHandle* s);

				bool loadStructV0(TmenuHandle* s);

			public:
				TparamError error() { return Ferror; }

				bool save(TmenuHandle* s, TstreamBase* _stream);

				bool load(TmenuHandle* s, TstreamBase* _stream);
		};
	}

}

class TparamSaveMenu : public TmenuHandle{
	public:
		typedef TenLoadSave::e Taction;
		sdds_struct(
				sdds_var(TenLoadSave,action)
				sdds_var(TparamError,error)
				sdds_var(Tuint16,size)
				sdds_var(Tuint32,time)
		)
		TparamSaveMenu();
};

#endif
