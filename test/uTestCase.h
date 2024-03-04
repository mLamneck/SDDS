#ifndef UTESTCASE_H
#define UTESTCASE_H

typedef std::function<bool()> TtestProc;
typedef bool (*TtestProcSimple)(Tdescr*);

class TtestCase{
    private:
        const char* FtestUnit;
        int Fsucc = 0;
        int Ffailed = 0;
        char FlogBuffer[1024];

    protected:
        char* binToHex(void* _buf, int size){
            uint8* buf = reinterpret_cast<uint8*>(_buf); 
            int run = 0;
            for (auto i = 0; i<size; i++){
                run += sprintf(&FlogBuffer[run],"0x%.2X ",buf[i]);
            }
            return &FlogBuffer[0];
        }

        char* binToHex(dtypes::string _str){ return binToHex(_str.data(),_str.length()); }

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
        }

        void runTests(){
            debug::log("XXXXXXXXXXXXXXXXXXXX running tests for %s XXXXXXXXXXXXXXXXXXXXXXXXXX",FtestUnit);
            test();
            debug::log("XXXXXXXXXXXXXXXXXXXX test results for %s XXXXXXXXXXXXXXXXXXXXXXXXXX",FtestUnit);
            debug::log("nTests = %d test failed = %d",Fsucc+Ffailed,Ffailed);
            debug::log("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
        }
};

#endif