#include "uTestCase.h"
#include "uJsonSerializer.h"

class TtestTypedef : public TtestCase{
    using TtestCase::TtestCase;
    TstringStream Fstream;

    public:
    class TbaseStruct : public TmenuHandle{
        public:
        sdds_struct(
            sdds_var(Tfloat32,val1,0,0);
        )
    };

    class TderivedStruct : public TbaseStruct{
        public:
            sdds_struct(
                sdds_var(Tfloat32,val2,0,0);
                sdds_var(Tfloat32,val3,0,0);
            )
    };

    class TdoubleDerivedStruct : public TderivedStruct{
        public:
            sdds_struct(
                sdds_var(Tfloat32,val4,0,0);
            )
    };

    class TstructAllTypes : public TmenuHandle{
        public:
            sdds_struct(
                sdds_var(Tuint8,Fuint8,sdds::opt::saveval,0);
                sdds_var(Tuint16,Fuint16,sdds::opt::saveval,0);
                sdds_var(Tuint32,Fuint32,sdds::opt::saveval,0);

                sdds_var(Tint8,Fint8,sdds::opt::saveval,0);
                sdds_var(Tint16,Fint16,sdds::opt::saveval,0);
                sdds_var(Tint32,Fint32,sdds::opt::saveval,0);

                sdds_var(Tfloat32,Ffloat32,sdds::opt::saveval,0);

                sdds_var(Ttime,Ftime,sdds::opt::saveval,stringToTime("10:00:00"));
            )
    };

    class TnestedStruct : public TmenuHandle{
        sdds_struct(
            sdds_var(Tuint8,rootVal0);
            sdds_var(TdoubleDerivedStruct,doubleDerived);
            sdds_var(Tuint8,rootValN);
        )
    };    
    
    class TdoubleNestedStruct : public TmenuHandle{
        sdds_struct(
            sdds_var(TnestedStruct,nestedStruct);
            sdds_var(Tuint8,ValN);
        )
    };

    class TderivedDoubleNestedStruct : public TdoubleNestedStruct{
        sdds_struct(
            sdds_var(Tuint8,valEnd);
        )
    };

    const char* serialize(TmenuHandle& _struct){
        Fstream.clear();
        TjsonSerializer s(&_struct,&Fstream,false);
        s.serialize();
        //debug::log(Fstream.data());
        return Fstream.data();
    }

    /** \brief test struct against our own jsonSerializer
     *
     * \param _struct struct to test for
     * \param _jsonRef expected json string 
     * \return true if test passed
     */
    bool testStruct(TmenuHandle& _struct, const char* _jsonRef){
        auto json = serialize(_struct);
        if (!string_cmp(_jsonRef,json)){
            debug::log("expected:");
            debug::log(_jsonRef);
            debug::log("found:");
            debug::log(json);
            return false;
        } 
        else 
            return true;
    }

    /*
    so far tests no test check the default values from the ssds_var(). This might better be done
    in a separate step because probably the to_string method will change it's formatting
    and would break the tests. For example single=1 vs. single=1.0 or so.
    it only check it the rtti is created correct. This assumes that
    our JsonSerializer works. But 2 errors cancel each other out sounds unlikely :-)
    
    code to produce the jsonRef:
    TjsonStringSerializer s(userStruct,false);
    userStruct.log();       //check if output correct by "hand"
    s.serialize();
    debug::log(s.c_str());  //output json string used below. copy from console into here

    */
    bool test() override {
        doTest([this](){
            TstructAllTypes s;
            return testStruct(s,R"([{"type":1,"opt":128,"name":"Fuint8"},{"type":2,"opt":128,"name":"Fuint16"},{"type":4,"opt":128,"name":"Fuint32"},{"type":17,"opt":128,"name":"Fint8"},{"type":18,"opt":128,"name":"Fint16"},{"type":20,"opt":128,"name":"Fint32"},{"type":36,"opt":128,"name":"Ffloat32"},{"type":6,"opt":128,"name":"Ftime"}])");
        },"TstructAllTypes");

        doTest([this](){
            TderivedStruct s;
            return testStruct(s,R"([{"type":36,"opt":0,"name":"val1"},{"type":36,"opt":0,"name":"val2"},{"type":36,"opt":0,"name":"val3"}])");
        },"TderivedStruct struct");

        doTest([this](){
            TdoubleDerivedStruct s;
            return testStruct(s,R"([{"type":36,"opt":0,"name":"val1"},{"type":36,"opt":0,"name":"val2"},{"type":36,"opt":0,"name":"val3"},{"type":36,"opt":0,"name":"val4"}])");
        },"TdoubleDerivedStruct struct");

        doTest([this](){
            TnestedStruct s;
            return testStruct(s,R"([{"type":1,"opt":0,"name":"rootVal0"},{"type":66,"opt":0,"name":"doubleDerived","value":[{"type":36,"opt":0,"name":"val1"},{"type":36,"opt":0,"name":"val2"},{"type":36,"opt":0,"name":"val3"},{"type":36,"opt":0,"name":"val4"}]},{"type":1,"opt":0,"name":"rootValN"}])");
        },"TnestedStruct struct");

        doTest([this](){
            TdoubleNestedStruct s;
            return testStruct(s,R"([{"type":66,"opt":0,"name":"nestedStruct","value":[{"type":1,"opt":0,"name":"rootVal0"},{"type":66,"opt":0,"name":"doubleDerived","value":[{"type":36,"opt":0,"name":"val1"},{"type":36,"opt":0,"name":"val2"},{"type":36,"opt":0,"name":"val3"},{"type":36,"opt":0,"name":"val4"}]},{"type":1,"opt":0,"name":"rootValN"}]},{"type":1,"opt":0,"name":"ValN"}])");
        },"TdoubleNestedStruct struct");

        doTest([this](){
            TderivedDoubleNestedStruct s;
            return testStruct(s,R"([{"type":66,"opt":0,"name":"nestedStruct","value":[{"type":1,"opt":0,"name":"rootVal0"},{"type":66,"opt":0,"name":"doubleDerived","value":[{"type":36,"opt":0,"name":"val1"},{"type":36,"opt":0,"name":"val2"},{"type":36,"opt":0,"name":"val3"},{"type":36,"opt":0,"name":"val4"}]},{"type":1,"opt":0,"name":"rootValN"}]},{"type":1,"opt":0,"name":"ValN"},{"type":1,"opt":0,"name":"valEnd"}])");
        },"TdoubleNestedStruct struct");

        return false;
    };
} typedef_test("typedef_test");