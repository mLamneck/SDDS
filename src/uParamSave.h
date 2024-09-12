#ifndef UPARAMSAVE_H
#define UPARAMSAVE_H

#define uParamSave_debug 0

#include "uTypedef.h"
#include "uCrc8.h"

#if SDDS_ON_ARDUINO == 1
    #include <EEPROM.h>
#endif

#ifndef SDDS_EEPROM_SIZE
	#define SDDS_EEPROM_SIZE 0
#endif

enum class TseekMode {start, end, curr};

typedef int16_t TstreamLength;

class TparamStream{
    private:
        TstreamLength Frun = 0;
        TstreamLength FhighWater = 0;

    protected:
        TstreamLength curr() { return Frun; };

        virtual TstreamLength availableForRead() = 0;
        virtual bool doWriteByte(uint8_t _byte) = 0;
        virtual bool doReadByte(uint8_t& _byte) = 0;
    public:
        inline bool writeByte(uint8_t _byte);

        inline bool readByte(uint8_t& _byte);

        bool writeBytes(const void* _buf, uint8_t _len);

        bool readBytes(void* _buf, uint8_t _len);

        bool seek(TseekMode _mode, int _pos);

        virtual bool grow(int _size){ return true; }

        virtual void flush(){};

        TstreamLength high(){ return FhighWater; }
};


#ifndef SDDS_EEPROM_COMMIT 
    #define SDDS_EEPROM_COMMIT 1
#endif

class TeepromStream : public TparamStream{
    bool grow(int _size) override{ return (_size <= SDDS_EEPROM_SIZE); }

    virtual TstreamLength availableForRead() override { return SDDS_EEPROM_SIZE - curr(); }

    bool doWriteByte(uint8_t _byte) override{
		#if SDDS_ON_ARDUINO == 1
        EEPROM.put(curr(),_byte);
		#endif
        return true;
    }

    bool doReadByte(uint8_t& _byte) override {
		#if SDDS_ON_ARDUINO == 1
        EEPROM.get(curr(),_byte);
		#endif
        return true;
    }

    void flush() override {
		#if SDDS_ON_ARDUINO == 1
		#if SDDS_EEPROM_COMMIT == 1
		EEPROM.commit();
		#endif
		#endif
    }
};

#if SDDS_ON_ARDUINO == 1
#endif

#if MARKI_DEBUG_PLATFORM == 1

    class TparamStringStream : public TparamStream{
        protected:
            dtypes::string Fbuffer;
            virtual TstreamLength availableForRead() override { return Fbuffer.length() - curr(); }

            bool grow(int _size) override{
                while(Fbuffer.length() < _size) Fbuffer+='\0';
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
            dtypes::string str() { return Fbuffer; }
    };

    /*
    on debug plattform we store to filesystem.
    */
    #include <iostream>
    #include <fstream>

    class TparamFileStream : public TparamStringStream{
        const char* Ffilename;

        void flush() override { 
            std::ofstream outfile;
            outfile.open(Ffilename,std::ios::binary);
            outfile.write(Fbuffer.c_str(),Fbuffer.length());
            outfile.close();
        }

        public:
            TparamFileStream(const char* _filename, bool _output){
                Ffilename = _filename;
                if (!_output){
                    std::ifstream ifs(_filename);
                    Fbuffer = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
                    ifs.close();
                }
            }
    };

#endif

typedef uint8_t TparamSaveVersion; 
struct __attribute__ ((packed)) TparamHeaderV0{
    crc8::Tcrc crc;
};

union TstructCrc{
    struct{
        crc8::Tcrc Ftype;
        crc8::Tcrc Fname;
    } rCrc;
    uint16_t crc;
};

sdds_enum(___,crc,invStrLen,outOfMem,invVers) TparamError;

class TparamStreamer{
    TparamError Ferror;
    TparamStream* Fstream;
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

        bool save(TmenuHandle* s, TparamStream* _stream);

        bool load(TmenuHandle* s, TparamStream* _stream);
};


sdds_enum(___,load,save) TenLoadSave;

class TparamSaveMenu : public TmenuHandle{
    public:
        typedef TenLoadSave::e Taction;
        sdds_struct(
            sdds_var(TenLoadSave,action)
            sdds_var(TparamError,error)
            sdds_var(Tuint16,size)
        )
        TparamSaveMenu();
};

#endif
