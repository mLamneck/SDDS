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

    protected:
        TsreamLength Frun = 0;

    public:
        dtypes::uint16 size(){ return Frun+1; }

        virtual bool writeByte(uint8_t _byte){
            //debug::log("0x%02X",_byte);
            return true;
        }

        virtual bool readByte(uint8_t& _byte){
            return false;
        }

        bool writeBytes(const void* _buf, uint8_t _len){
            auto buf = static_cast<const uint8_t*>(_buf);
            while (_len-- > 0){ if (!writeByte(*buf++)) return false; }
            return true;
        }

        bool readBytes(void* _buf, uint8_t _len){
            auto buf = static_cast<uint8_t*>(_buf);
            uint8_t byte;
            while (_len-- > 0){ 
                if (!readByte(byte)) return false;
                *buf++ = byte;  
            }
            return true;
        }

        virtual bool seek(TseekMode _mode, int _pos) {
            switch(_mode){
                case (TseekMode::start):
                    if (_pos < 0) return false;
                    Frun = _pos;
                    break;
                case (TseekMode::curr):
                    Frun += _pos;
                    if (Frun < 0) Frun = 0;
                    break;
                case (TseekMode::end):
                    if (_pos > 0) return false;
                    if (Frun < 0) Frun = 0;
                    break;
            }
            return true; 
        }

        virtual void flush(){}
};

#if USE_EEPROM == 1

class TeepromStream : public TparamStream{
    bool writeByte(uint8_t _byte) override{
        EEPROM.put(Frun++,_byte);
        return true;
    }

    bool readByte(uint8_t& _byte) override {
        EEPROM.get(Frun++,_byte);
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
            virtual int availableForRead(){ return Fbuffer.length() - Frun; }

            void resize(int _size){ while(Fbuffer.length() < _size) Fbuffer+='\0'; }
            
            bool writeByte(uint8_t _byte) override{
                resize(Frun+1);
                Fbuffer[Frun] = _byte;
                Frun++;
                return true;
            }

            bool readByte(uint8_t& _byte) override {
                if (availableForRead() <= 0) return false;
                _byte = Fbuffer[Frun++];
                return true;
            }

        public:
            dtypes::string str() { return Fbuffer; }

            bool seek(TseekMode _mode, int _pos) override {
                switch(_mode){
                    case (TseekMode::start):
                        if (_pos < 0) return false;
                        Frun = _pos;
                        break;
                    case (TseekMode::curr):
                        Frun += _pos;
                        if (Frun < 0) Frun = 0;
                        break;
                    case (TseekMode::end):
                        if (_pos > 0) return false;
                        Frun = Fbuffer.length()-1+_pos;
                        if (Frun < 0) Frun = 0;
                        break;
                }
                return true; 
            }
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

sdds_enum(___,crc,stream) TparamError;

class TparamStreamer{
    TparamError Ferror;
    TparamStream* Fstream;
    TstructCrc Fcrc;

    void _calcCrc(TmenuHandle* s){
        for (auto it = s->iterator(); it.hasNext();){
            auto descr = it.next();
            if (!descr->shouldBeSaved()) continue;

            crc8::calc(Fcrc.rCrc.Ftype,descr->typeId());
            //crc8::calc(Fcrc.rCrc.Fname,descr->name(),strlen(descr->name()));
            if (descr->isStruct()){
                TmenuHandle* mh = static_cast<Tstruct*>(descr)->value();
                if (mh) _calcCrc(mh);
            }
        }
    }
    void calcCrc(TmenuHandle* s){ Fcrc.crc=0; _calcCrc(s); }

    bool saveStruct(TmenuHandle* s){
        for (auto it = s->iterator(); it.hasNext();){
            auto descr = it.next();
            if (!descr->shouldBeSaved()) continue;

            crc8::calc(Fcrc.rCrc.Ftype,descr->typeId());
            //crc8::calc(Fcrc.rCrc.Fname,descr->name(),strlen(descr->name()));
            if (descr->isStruct()){
                TmenuHandle* mh = static_cast<Tstruct*>(descr)->value();
                if (!mh) continue;

                if (!saveStruct(mh)) return false;
                continue;
            }

        #if uParamSave_debug == 1
            uint8_t tempBuf[16] = {};
            int size = descr->valSize();
            bool res = Fstream->writeBytes(descr->pValue(),descr->valSize());
            Fstream->seek(TseekMode::curr,-size);
            Fstream->readBytes(&tempBuf,size);
            if (!res){
        #else
            if (!Fstream->writeBytes(descr->pValue(),descr->valSize())){
        #endif
                Ferror = TparamError::e::stream;
                return false;
            };

        }
        return true;
    }

    bool _loadStruct(TmenuHandle* s){
        for (auto it = s->iterator(); it.hasNext();){
            auto descr = it.next();
            if (!descr->shouldBeSaved()) continue;

            if (descr->isStruct()){
                TmenuHandle* mh = static_cast<Tstruct*>(descr)->value();
                if (!mh) continue;

                if (!_loadStruct(mh)) return false;
                continue;
            }
            
        #if uParamSave_debug == 1
            uint8_t tempBuf[16] = {};
            int size = descr->valSize();
            if (!Fstream->readBytes(&tempBuf,size)){
        #else
            if (!Fstream->readBytes(descr->pValue(),descr->valSize())){
        #endif
                Ferror = TparamError::e::stream;
                return false;
            }else{
        #if uParamSave_debug == 1
                memccpy(descr->pValue(),&tempBuf,size,size);
        #endif
                descr->signalEvents();
            };
        }
        return true;
    }

    bool loadStructV0(TmenuHandle* s){
        TparamHeaderV0 header;
        if (!Fstream->readBytes(&header,sizeof(header))){
            Ferror = TparamError::e::stream;
            return false;
        } 
        calcCrc(s);
        if (header.crc != Fcrc.rCrc.Ftype){
            Ferror = TparamError::e::crc;
            return false;
        }
        return _loadStruct(s);
    }

    public:
        TparamError error() { return Ferror; }

        bool save(TmenuHandle* s, TparamStream* _stream){
            Fcrc.crc = 0;
            Ferror = TparamError::e::___;
            Fstream = _stream;
            Fstream->seek(TseekMode::start,sizeof(TparamSaveVersion)+sizeof(TparamHeaderV0));
            bool res = saveStruct(s); 
            if (res){
                Fstream->seek(TseekMode::start,0);
                TparamSaveVersion version = 0;
                TparamHeaderV0 header = { .crc = Fcrc.rCrc.Ftype };
                if (
                    (!Fstream->writeBytes(&version,sizeof(TparamSaveVersion)))
                    || (!Fstream->writeBytes(&header,sizeof(TparamHeaderV0)))
                )
                {
                    Ferror = TparamError::e::stream;
                    return false;
                }
            }
            else{
                Fstream->seek(TseekMode::start,0);
            }
            Fstream->flush();
            return res;
        }

        bool load(TmenuHandle* s, TparamStream* _stream){
            Ferror = TparamError::e::___;
            Fstream = _stream;
            TparamSaveVersion version = 0;
            if (!Fstream->readBytes(&version,sizeof(TparamSaveVersion))) return false;
  
            switch(version){
                case(0): return loadStructV0(s);
            }
            return false;
        }

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
        TparamSaveMenu(){
            on(action){
                if (action != TenLoadSave::e::___){
                    TmenuHandle* root = findRoot();
                    TparamStreamer ps;
                    #if MARKI_DEBUG_PLATFORM == 1
                    TparamFileStream s("c:\\temp\\params.txt",action==TenLoadSave::e::save);
                    #elif USE_EEPROM == 1
                    //to be done
                    TeepromStream s;
                    #endif
                    if (action==TenLoadSave::e::load){
                        ps.load(root,&s);
                    }else if(action==TenLoadSave::e::save){
                        ps.save(root,&s);
                    }
                    error = ps.error();
                    size = s.size();
                    action = TenLoadSave::e::___;
                }
            };

            on(sdds::init){
                #if USE_EEPROM == 1
                    EEPROM.begin(EEPROM_STREAM_SIZE);
                #endif
                action = Taction::load;
            };
        }
};

#endif