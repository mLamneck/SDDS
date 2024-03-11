#ifndef UPARAMSAVE_H
#define UPARAMSAVE_H

#define uParamSave_debug 0

#include "uTypedef.h"
#include "uCrc8.h"
#if MARKI_DEBUG_PLATFORM == 0
    #define USE_EEPROM 1
#endif

#if USE_EEPROM == 1
    #define EEPROM_STREAM_SIZE 1024
    #include <EEPROM.h>
#endif

enum class TseekMode {start, end, curr};

typedef int32_t TsreamLength;

class TparamStream{
    private:
        TsreamLength Frun = 0;
        TsreamLength Fsize = 0;

    protected:
        TsreamLength curr(bool _inc = false);

    public:
        dtypes::uint16 size(){ return Fsize; }

        virtual bool writeByte(uint8_t _byte) = 0;

        virtual bool readByte(uint8_t& _byte) = 0;

        bool writeBytes(const void* _buf, uint8_t _len);

        bool readBytes(void* _buf, uint8_t _len);

        virtual bool grow(int _size){ return true; }

        virtual bool seek(TseekMode _mode, int _pos);

        virtual void flush(){};
};

#if USE_EEPROM == 1

class TeepromStream : public TparamStream{
    bool grow(int _size) override{ return (_size <= EEPROM_STREAM_SIZE); }

    bool writeByte(uint8_t _byte) override{
        EEPROM.put(curr(true),_byte);
        return true;
    }

    bool readByte(uint8_t& _byte) override {
        if (curr() >= EEPROM_STREAM_SIZE) return false;
        EEPROM.get(curr(true),_byte);
        return true;
    }

    void flush() override {
        EEPROM.commit();
    }
};

#endif

#if MARKI_DEBUG_PLATFORM == 1

    class TparamStringStream : public TparamStream{
        protected:
            dtypes::string Fbuffer;
            virtual int availableForRead(){ return Fbuffer.length() - curr(); }

            bool grow(int _size) override{
                while(Fbuffer.length() < _size) Fbuffer+='\0';
                return true;
            }

            bool writeByte(uint8_t _byte) override{
                grow(curr()+1);
                Fbuffer[curr(true)] = _byte;
                return true;
            }

            bool readByte(uint8_t& _byte) override {
                if (availableForRead() <= 0) return false;
                _byte = Fbuffer[curr(true)];
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
    Ttimer timer;
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