#include "uTestCase.h"
#include "uParamSave.h"

class TtestParamSave : public TtestCase{
    using TtestCase::TtestCase;

    class TsubStructNoSave : public TmenuHandle{
        public:
        sdds_struct(
            sdds_var(Tfloat32,subVal,0,0);
        )
    };

    class TsubStructSave : public TmenuHandle{
        public:
        sdds_struct(
            sdds_var(Tfloat32,subValSave,sdds::opt::saveval,0);
        )
    };

    class TstructAllTypes : public TmenuHandle{
        public:
            sdds_struct(
                sdds_var(Tuint8,Fuint8,sdds::opt::saveval,0)
                sdds_var(Tuint16,Fuint16,sdds::opt::saveval,0)
                sdds_var(Tuint32,Fuint32,sdds::opt::saveval,0)

                sdds_var(Tint8,Fint8,sdds::opt::saveval,0)
                sdds_var(Tint16,Fint16,sdds::opt::saveval,0)
                sdds_var(Tint32,Fint32,sdds::opt::saveval,0)

                sdds_var(Tfloat32,Ffloat32,sdds::opt::saveval,0)

                sdds_var(Ttime,Ftime,sdds::opt::saveval,stringToTime("10:00:00"))

                sdds_var(Tstring,Fstr,sdds::opt::saveval)
            )
    };

    class TnestedStruct : public TmenuHandle{
        public:
        sdds_struct(
            sdds_var(Tuint8,Fuint8);
            sdds_var(TsubStructNoSave,subNoSave);
            sdds_var(TsubStructSave,subSave);
        )
    };

    sdds_enum(OFF,ON) TonOffState;
        
    class Tled : public TmenuHandle{
        public:
        sdds_struct(
            sdds_var(TonOffState,ledSwitch,sdds::opt::saveval)
            sdds_var(TonOffState,blinkSwitch,sdds::opt::saveval)
            sdds_var(Tuint16,onTime,sdds::opt::saveval,500)
            sdds_var(Tuint16,offTime,sdds::opt::saveval,500)
        )
    };

    class TuserStruct : public TmenuHandle{
        public:
            sdds_struct(
                sdds_var(Tled,led,sdds::opt::saveval)
                //sdds_var(TparamSaveMenu,params)
            )
    };

    void logStruct(TmenuHandle& s1, dtypes::string _pre = ""){
        for (auto it=s1.iterator(); it.hasCurrent(); it.jumpToNext()){
            auto d = it.current();
            if (d->isStruct()){
                TmenuHandle* mh1 = static_cast<Tstruct*>(d)->value();
                debug::log("%s-> %s",_pre.c_str(),d->name());
                logStruct(*mh1,_pre + "  ");
                debug::log("%s<- %s",_pre.c_str(),d->name());
            } else {
                debug::log("%s%s=%s",_pre.c_str(),d->name(),d->to_string().c_str());
            }
        }
    }

    int _calcSize(TmenuHandle& _s, int _size = 0){
        for (auto it=_s.iterator(); it.hasCurrent(); it.jumpToNext()){
            auto d = it.current();
            if (d->isStruct()){
                TmenuHandle* mh1 = static_cast<Tstruct*>(d)->value();
                _size = _calcSize(*mh1,_size);
                continue;
            } 
             
            if (!d->saveval()) continue;
            if (d->type() == sdds::Ttype::STRING){
                _size += d->to_string().length() + 1;
            } else {
                _size += d->valSize();
            }
        }
        return _size;
    }
    
    int calcSize(TmenuHandle& _s){ return _calcSize(_s) + sizeof(sdds::paramSave::TparamSaveVersion) + sizeof(sdds::paramSave::TparamHeaderV0); }

    bool compareStructs(TmenuHandle& s1, TmenuHandle& s2, dtypes::string _path = ""){
        auto it2 = s2.iterator();
        for (auto it1 = s1.iterator(); it1.hasCurrent(); it1.jumpToNext()){
            auto d1 = it1.current();

            //d1 has no saveval -> this means it is not saved to eeprom
            //we don't have to compare it
            if (!d1->saveval()) continue;

            //var doesn't exist in s2, that's ok, we don't have to check.
            //this is the case when a variable of a struct is deleted and the
            //eeprom save is still old.
            auto d2 = s2.find(d1->name());
            if (!d2){
                debug::log("invalid test condition structures are not equal %s not found in s2",d1->name());
                return false;
            };

            dtypes::string varPath = _path + "." + d1->name();
            if (_path == "") varPath = d1->name();

            if (d1->isStruct()){
                if (d2->isStruct()){
                    //debug::log("%s-> %s",_path.c_str(),d1->name());
                    TmenuHandle* mh1 = static_cast<Tstruct*>(d1)->value();
                    TmenuHandle* mh2 = static_cast<Tstruct*>(d2)->value();
                    bool res = compareStructs(*mh1,*mh2,varPath);
                    //debug::log("%s<- %s",_path.c_str(),d1->name());
                    if (!res) return false;
                }
                else{
                    debug::log("invalid test condition %s is struct %s is not",d1->name(),d2->name());
                    return false;
                }
            }
            
            //debug::log("%s%s",_path.c_str(),d1->name());
            if (d1->to_string() != d2->to_string() ){
                debug::log("%s=%s!=%s",d1->name(),d1->to_string().c_str(),d2->to_string().c_str());
                return false;
            }
            else debug::log("var %s=%s ok",varPath.c_str(),d1->to_string().c_str());
        }
        return true;
    }

    /** \brief save struct s1, load it in s2 and compare if equal
     *
     * \param s1 struct with some test data to be saved
     * \param s1 struct to load the data in
     * \return true if test successfully
     *
     */
    bool test2structs(TmenuHandle& s1, TmenuHandle& s2
        ,TparamError::dtype _expLoadError = TparamError::e::___
    )
    {
        sdds::paramSave::TparamStreamer ps;
        sdds::paramSave::TstringStream s;
        bool res = ps.save(s1,&s);

        int expSize = calcSize(s1); 
        if (expSize != s.high()){
            debug::log("size='%d'!='%d'=expected size",s.high(),expSize);
            return false;
        }

        //debug::log(binToHex(s.str()));
        s.seek(sdds::paramSave::TseekMode::start,0);
        res = ps.load(s2,&s);
        if (_expLoadError != ps.error()){
            debug::log("error='%s'!='%s'=expectedError",ps.error().to_string().c_str(),TparamError::enumClass::c_str(_expLoadError));
            return false;
        }
        if (_expLoadError != TparamError::e::___) return true;
        return compareStructs(s1,s2);
    }

    bool test() override {
        doTest([this](){
            TuserStruct struct1;
            TuserStruct struct2;
            struct1.led.ledSwitch = TonOffState::e::ON;
            struct1.led.blinkSwitch = TonOffState::e::ON;
            return test2structs(struct1,struct2);
        },"Tled");

        doTest([this](){
            TstructAllTypes struct1;
            TnestedStruct struct11;
            bool res = test2structs(struct1,struct11,TparamError::e::crc);
            return res;
        },"invalid crc");

        doTest([this](){
            TstructAllTypes struct1;
            TstructAllTypes struct11;
            struct1.Fuint8 = 11;
            struct1.Fuint16 = 5123;
            struct1.Fuint32 = 5123;
            struct1.Fint8 = 11;
            struct1.Fint16 = 5123;
            struct1.Fint32 = 5123;
            struct1.Ffloat32 = 1.2;
            struct1.Ftime = stringToTime("12:23:05");
            struct1.Fstr = "HalloMarkiDasIstEinStringMitTab\t";
            return test2structs(struct1,struct11);
        },"TstructAllTypes");

        doTest([this](){
            TstructAllTypes struct1;
            TstructAllTypes struct11;
            struct1.Fstr = "";
            return test2structs(struct1,struct11);
        },"Test Menu with empty string");

        doTest([this](){
            TnestedStruct struct1;
            TnestedStruct struct11;
            struct1.Fuint8 = 12;
            struct1.subSave.subValSave = 123;
            return test2structs(struct1,struct11);
        },"TnestedStruct");

        return false;
    };
} paramSave_test("paramSave_test");
