#include "uTestCase.h"
#include "uStrings.h"

class TtestStrings : public TtestCase{
    using TtestCase::TtestCase;

    dtypes::uint8 val;
    bool succ;

    void testStrings(){

        doTest([this](){
            TstringRef p("0");
            succ = p.parseValue(val);
            if (succ && val == 0 && p.curr()=='\0') return true;
            else {
                debug::log("test failed succ=%d!=true or val=%d!=0 or p.curr()!=%c\\0",succ,val,p.curr());
                return false;
            }
        },"parse 0");

        doTest([this](){
            TstringRef p("123");
            succ = p.parseValue(val);
            if (succ && val == 123 && p.curr()=='\0') return true;
            else {
                debug::log("test failed succ=%d!=true or val=%d!=0 or p.curr()=%c!=\\0",succ,val,p.curr());
                return false;
            }
        },"parse 123");

        doTest([this](){
            TstringRef p("254,");
            succ = p.parseValue(val);
            if (succ && val == 254 && p.curr()==',') return true;
            else {
                debug::log("test failed succ=%d!=true or val=%d!=0 or p.curr()=%c!=,",succ,val,p.curr());
                return false;
            }
        },"parse 254,");
    }

    bool test() override{
        testStrings();
        return true;
    }

} strings_test("strings_test");