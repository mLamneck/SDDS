#include "uStrings.h"
#include "uTime.h"

class TtestCase{
    public:
        virtual void test() = 0;
};

void testStrings(){
    dtypes::uint8 val;
    bool succ;

    TstringRef p("0");
    succ = p.parseValue(val);
    if (succ && val == 0 && p.curr()=='\0') debug::log("test successful");
    else debug::log("test failed succ=%d!=true or val=%d!=0 or p.curr()!=%c\\0",succ,val,p.curr());

    p = TstringRef("123");
    succ = p.parseValue(val);
    if (succ && val == 123 && p.curr()=='\0') debug::log("test successful");
    else debug::log("test failed succ=%d!=true or val=%d!=0 or p.curr()=%c!=\\0",succ,val,p.curr());

    p = TstringRef("254,");
    succ = p.parseValue(val);
    if (succ && val == 254 && p.curr()==',') debug::log("test successful");
    else debug::log("test failed succ=%d!=true or val=%d!=0 or p.curr()=%c!=,",succ,val,p.curr());
}

void testConvTimeCase(const char* _input, const char* _expectedOutput, bool _expectedSucc = true){
    TdateTimeParser dtp(_input);
    bool succ = dtp.parse();
    dtypes::string timeStr = timeToString(dtp.Fresult);
    if (succ != _expectedSucc){
        debug::log("testcase '%s' failed _succ=%s!=%s",_input,succ==true?"true":"false",_expectedSucc==true?"true":"false");
        return;
    }
    if (succ){
        if (strcmp(_expectedOutput,timeStr.c_str()) != 0){
            debug::log("testcase '%s' failed '%s' != '%s'",_input,_expectedOutput,timeStr.c_str());
            return;
        }
    }
    debug::log("testcase '%s' successful",_input);
}

void testConvTimeCase(const char* _input, bool _expectedSucc = true){
    testConvTimeCase(_input,_input,_expectedSucc);
}

void testTimeConv(){
    testConvTimeCase("28.11.2023 19:12:30");
    testConvTimeCase("28.11.2023 19:12:30.765");
    testConvTimeCase("19:12:30","01.01.1970 19:12:30");
    testConvTimeCase("01.01.1970T19:12:30","01.01.1970 19:12:30");
    testConvTimeCase("05/30/2020 19:12:30","30.05.2020 19:12:30");

    testConvTimeCase("10.10.2023 00:00:00","10.10.2023 00:00:00");
    testConvTimeCase("10.10.2023","10.10.2023 00:00:00");

    testConvTimeCase("31/30/2020 19:12:30",false);
    int i = 0;
    if (i > 0){

    }
}
