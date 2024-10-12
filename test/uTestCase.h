#ifndef UTESTCASE_H
#define UTESTCASE_H

typedef std::function<bool()> TtestProc;
typedef bool (*TtestProcSimple)(Tdescr*);

namespace unitTest{
    const int MAX_TESTS = 100;
    void registerTest(void* _tc);
}

class TtestCase{
	typedef dtypes::uint8 uint8;
    private:
        const char* FtestUnit;
        char FlogBuffer[1024];
    public:
        int Ffailed = 0;
        int Fsucc = 0;

    protected:
        char* binToHex(void* _buf, int size){
            uint8* buf = reinterpret_cast<uint8*>(_buf); 
            int run = 0;
            for (auto i = 0; i<size; i++){
                run += sprintf(&FlogBuffer[run],"0x%.2X ",buf[i]);
            }
			FlogBuffer[run]=0;
            return &FlogBuffer[0];
        }

        char* binToHex(dtypes::string _str){ return binToHex(_str.data(),_str.length()); }

		void logStruct(TmenuHandle& s1, dtypes::string _pre = ""){
			for (auto it=s1.iterator(); it.hasNext();){
				auto d = it.next();
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

        void doTest(TtestProc _tp, const char* _name){
            debug::log("--------------- running test %s",_name);
            try{
                if (_tp()) {
                    debug::log("passed");
                    Fsucc++;
                }
                else {
                    debug::log("failed");
                    Ffailed++;
                }
            }
            //Why the heck is this asshole not catching exceptions here!?!?!?!?
            //Fuck C++!!!!!!!!!!!! probably I have to set some flags in the build options
            catch (...){
                debug::log("exception");
                Ffailed++;
            }
        }
        virtual bool test() = 0;
    public:
        TtestCase(const char* _testUnit){
            FtestUnit = _testUnit;
            unitTest::registerTest(this);
        }
        
        void runTests(){
            debug::log("XXXXXXXXXXXXXXXXXXXX running tests for %s XXXXXXXXXXXXXXXXXXXXXXXXXX",FtestUnit);
            test();
            debug::log("XXXXXXXXXXXXXXXXXXXX test results for %s XXXXXXXXXXXXXXXXXXXXXXXXXX",FtestUnit);
            debug::log("nTests = %d test failed = %d",Fsucc+Ffailed,Ffailed);
            debug::log("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
            debug::log("");
        }
};

namespace unitTest{
	class TtestData: public TmenuHandle{
		public:
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
				sdds_var(TstructAllTypes,allTypes)
			)
		};

		sdds_struct(
			sdds_var(Tuint8,var1)
			sdds_var(TstructAllTypes,allTypes)
			sdds_var(TnestedStruct,sub)
			sdds_var(Tuint8,var2)
		)
	};

    int FnTests;
    static TtestCase* Ftests[MAX_TESTS];
    int Ffailed;
    int FnTestCases;

    void registerTest(void* _tc){
        if (FnTests >= MAX_TESTS) throw("to many tests registert -> increase MAX_TESTS");
        Ftests[FnTests++] = (TtestCase*)_tc;
    }
    
    void run(){
        debug::log("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
        debug::log("start unit tests, test to run %d",FnTests);
        debug::log("");

        Ffailed = 0;
        FnTestCases = 0;
        for (auto i = 0; i<FnTests; i++){
            auto tc = Ftests[i];
            tc->runTests();
            Ffailed = Ffailed + tc->Ffailed;
            FnTestCases += tc->Ffailed+tc->Fsucc;
        }
        
        debug::log("Final result:");
        debug::log("number of testCases=%d, failed = %d",FnTestCases,Ffailed);
        debug::log("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    }
}


#endif