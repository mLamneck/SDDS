#include "utime.h"
#include "uTestCase.h"

class TtestTime : public TtestCase{
    using TtestCase::TtestCase;

    bool testConvTimeCase(const char* _input, const char* _expectedOutput, bool _expectedSucc = true){
        TdateTimeParser dtp(_input);
        bool succ = dtp.parse();
        dtypes::string timeStr = timeToString(dtp.Fresult);
        if (succ != _expectedSucc){
            debug::log("testcase '%s' failed _succ=%s!=%s",_input,succ==true?"true":"false",_expectedSucc==true?"true":"false");
            return false;
        }
        if (succ){
            if (strcmp(_expectedOutput,timeStr.c_str()) != 0){
                debug::log("testcase '%s' failed '%s' != '%s'",_input,_expectedOutput,timeStr.c_str());
                return false;
            }
        }
        return true;
    }

    bool testConvTimeCase(const char* _input, bool _expectedSucc = true){
        return testConvTimeCase(_input,_input,_expectedSucc);
    }

    void testTimeConv(){
        doTest([this](){ return testConvTimeCase("28.11.2023 19:12:30"); },"case 1");
        doTest([this](){ return testConvTimeCase("28.11.2023 19:12:30.765"); },"case 2");
        doTest([this](){ return testConvTimeCase("19:12:30","01.01.1970 19:12:30"); },"case 3");
        doTest([this](){ return testConvTimeCase("01.01.1970T19:12:30","01.01.1970 19:12:30"); },"case 4");
        doTest([this](){ return testConvTimeCase("05/30/2020 19:12:30","30.05.2020 19:12:30"); },"case 5");
        doTest([this](){ return testConvTimeCase("10.10.2023 00:00:00","10.10.2023 00:00:00"); },"case 6");
        doTest([this](){ return testConvTimeCase("10.10.2023","10.10.2023 00:00:00"); },"case 7");
        doTest([this](){ return testConvTimeCase("31/30/2020 19:12:30",false); },"case 8");
    }

    bool test() override{
        testTimeConv();
        return true;
    }

} time_test("time_test");

