#include "uParamSave.h"

TsreamLength TparamStream::curr(bool _inc){ 
    auto curr = Frun;
    if (Frun >= Fsize) Fsize++;
    if (_inc) Frun++;
    return curr; 
}

bool TparamStream::writeBytes(const void* _buf, uint8_t _len){
    auto buf = static_cast<const uint8_t*>(_buf);
    while (_len-- > 0){ if (!writeByte(*buf++)) return false; }
    return true;
}

bool TparamStream::readBytes(void* _buf, uint8_t _len){
    auto buf = static_cast<uint8_t*>(_buf);
    uint8_t byte;
    while (_len-- > 0){ 
        if (!readByte(byte)) return false;
        *buf++ = byte;  
    }
    return true;
}

bool TparamStream::seek(TseekMode _mode, int _pos) {
    switch(_mode){
        case (TseekMode::start):
            if (_pos < 0) return false;
            if (!grow(_pos+1)) return false;
            Frun = _pos;
            break;
        case (TseekMode::curr):
            if (_pos > 0){
                if (!grow(Frun + _pos + 1)) return false;
            } else{
                Frun += _pos;
                if (Frun < 0) Frun = 0;
            }
            break;
        case (TseekMode::end):
            if (_pos > 0) return false;
            if (Frun < 0) Frun = 0;
            break;
    }
    return true; 
}


void TparamStreamer::_calcCrc(TmenuHandle* s){
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

bool TparamStreamer::writeByte(dtypes::uint8 _byte){
    bool res = Fstream->writeByte(_byte);
    if (!res)
        Ferror = TparamError::e::outOfMem;
    return res;
}

bool TparamStreamer::saveStruct(TmenuHandle* s){
    for (auto it = s->iterator(); it.hasNext();){
        auto descr = it.next();
        if (!descr->shouldBeSaved()) continue;

        crc8::calc(Fcrc.rCrc.Ftype,descr->typeId());
        //crc8::calc(Fcrc.rCrc.Fname,descr->name(),strlen(descr->name()));
        if(descr->type() == sdds::Ttype::STRUCT){
            TmenuHandle* mh = static_cast<Tstruct*>(descr)->value();
            if (!mh) continue;

            if (!saveStruct(mh)) return false;
            continue;
        }
        
        if (descr->type() == sdds::Ttype::STRING){
            dtypes::string* str = static_cast<dtypes::string*>(descr->pValue());
            if (str->length() > 255){
                Ferror = TparamError::e::invStrLen;
                return false;
            }
            if (!writeByte(str->length())) return false;
            if (!Fstream->writeBytes(str->c_str(),str->length())){
                Ferror = TparamError::e::outOfMem;
                return false;
            };
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
            Ferror = TparamError::e::outOfMem;
            return false;
        };
        if (descr->type() != sdds::Ttype::STRING)
            continue;
    }
    return true;
}

bool TparamStreamer::readByte(dtypes::uint8& _byte){
    bool res = Fstream->readByte(_byte);
    if (!res)
        Ferror = TparamError::e::outOfMem;
    return res;
}

bool TparamStreamer::_loadStruct(TmenuHandle* s){
    for (auto it = s->iterator(); it.hasNext();){
        auto descr = it.next();
        if (!descr->shouldBeSaved()) continue;

        if (descr->isStruct()){
            TmenuHandle* mh = static_cast<Tstruct*>(descr)->value();
            if (!mh) continue;

            if (!_loadStruct(mh)) return false;
            continue;
        }
        
        if (descr->type() == sdds::Ttype::STRING){
            dtypes::uint8 strSize;
            if (!readByte(strSize)) return false;
            dtypes::string* str = static_cast<dtypes::string*>(descr->pValue());
            str->reserve(strSize);  //don't grow on each iteration
            uint8_t c;
            while(strSize-- > 0){
                if (!readByte(c)) return false;
                *str += c;
            }
            continue;
        }

    #if uParamSave_debug == 1
        uint8_t tempBuf[16] = {};
        int size = descr->valSize();
        if (!Fstream->readBytes(&tempBuf,size)){
    #else
        if (!Fstream->readBytes(descr->pValue(),descr->valSize())){
    #endif
            Ferror = TparamError::e::outOfMem;
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

bool TparamStreamer::loadStructV0(TmenuHandle* s){
    TparamHeaderV0 header;
    if (!Fstream->readBytes(&header,sizeof(header))){
        Ferror = TparamError::e::outOfMem;
        return false;
    } 
    calcCrc(s);
    if (header.crc != Fcrc.rCrc.Ftype){
        Ferror = TparamError::e::crc;
        return false;
    }
    return _loadStruct(s);
}

bool TparamStreamer::save(TmenuHandle* s, TparamStream* _stream){
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
            Ferror = TparamError::e::outOfMem;
            return false;
        }
    }
    else{
        Fstream->seek(TseekMode::start,0);
    }
    Fstream->flush();
    return res;
}

bool TparamStreamer::load(TmenuHandle* s, TparamStream* _stream){
    Ferror = TparamError::e::___;
    Fstream = _stream;
    TparamSaveVersion version = 0;
    if (!Fstream->readBytes(&version,sizeof(TparamSaveVersion))){
        Ferror = TparamError::e::outOfMem;
        return false;
    }

    switch(version){
        case(0): return loadStructV0(s);
    }
    Ferror = TparamError::e::invVers;
    return false;
}

TparamSaveMenu::TparamSaveMenu(){
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