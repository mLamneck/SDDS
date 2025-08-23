#include <uTypedef.h>
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

    sdds_enum(on,off,___) TonOff;

    class TstructAllTypes : public TmenuHandle{
        public:
            sdds_struct(
                sdds_var(TonOff,onOff);
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

	template <typename T, typename V>
	int sameValues(T _sddsVar, V _rawValue, const char* _msg=""){
		if (_sddsVar != _rawValue){
			 debug::log("(%s)value is not equal",_msg);
			 return 1;
		}
		return 0;
	}

	template<class T, typename V, typename W>
	bool testOperators(T& _val, V _testValue=10, W _f=5){
		typename T::dtype rawValue = _testValue;
		int err = 0; 
		
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val,rawValue,"val=");

		/** Operator++ */
		rawValue=_testValue; _val=rawValue;
		_val++; rawValue++;
		err += sameValues(_val,rawValue,"val++");

		/** Operator++ test post increment*/
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val++,rawValue++,"val++ postincrement");

		/** ++Operator */
		rawValue=_testValue; _val=rawValue;
		++_val; ++rawValue;
		err += sameValues(_val,rawValue,"++val");

		/** ++Operator test pre increment*/
		rawValue=_testValue; _val=rawValue;
		err += sameValues(++_val,++rawValue,"++val predincrement");


		/** Operator-- */
		rawValue=_testValue; _val=rawValue;
		_val--; rawValue--;
		err += sameValues(_val,rawValue,"val--");

		/** Operator-- test post decrement*/
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val--,rawValue--,"val-- postdecrement)");

		/** --Operator */
		rawValue=_testValue; _val=rawValue;
		--_val; --rawValue;
		err += sameValues(_val,rawValue,"--val");

		/** --Operator test pre decrement*/
		rawValue=_testValue; _val=rawValue;
		err += sameValues(--_val,--rawValue,"--val predecrement");


		/** Operator+= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val+=_f,rawValue+=_f,"+=");

		/** Operator-= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val-=_f,rawValue-=_f,"-=");

		/** Operator*= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val*=_f,rawValue*=_f,"*=");

		/** Operator/= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val/=_f,rawValue/=_f,"/=");

		/** Operator<<= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val<<=2,rawValue<<=2,"<<=");

		/** Operator>>= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val>>=2,rawValue>>=2,">>=");

		/** Operator|= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val|=2,rawValue|=2,"|=");

		/** Operator&= */
		rawValue=_testValue; _val=rawValue;
		err += sameValues(_val&=2,rawValue&=2,"&=");

		_val = _testValue; rawValue = _testValue;
		err += sameValues(_val == rawValue, true, "val == rawValue");
		err += sameValues(_val != (rawValue+1), true, "val != rawValue+1");
		err += sameValues(_val < (rawValue + 1), true, "val < rawValue+1");
		err += sameValues(_val <= rawValue, true, "val <= rawValue");
		err += sameValues(_val > (rawValue - 1), true, "val > rawValue-1");
		err += sameValues(_val >= rawValue, true, "val >= rawValue");

		return (err==0);
	}

	void testOperators(){
        doTest([this](){
	        Tuint8 val;
            return testOperators(val,10,5);
        },"Operators for Tuint8");
/*
        doTest([this](){
	        Tuint16 val;
            return testOperators(val,10,5);
        },"Operators for Tuint16");
        doTest([this](){
	        Tuint32 val;
            return testOperators(val,10,5);
        },"Operators for Tuint32");

        doTest([this](){
	        Tint8 val;
            return testOperators(val,10,5);
        },"Operators for Tint8");
        doTest([this](){
	        Tint16 val;
            return testOperators(val,10,5);
        },"Operators for Tint16");
        doTest([this](){
	        Tint32 val;
            return testOperators(val,10,5);
        },"Operators for Tint32");
		*/

	};

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
            TbaseStruct s;
            return testStruct(s,R"({"e":{},"d":[[36,0,"val1"]]})");
        },"TbaseStruct");

        doTest([this](){
            TstructAllTypes s;
            return testStruct(s,R"({"e":{"0":["on","off","___"]},"d":[[49,0,"onOff",0],[1,128,"Fuint8"],[2,128,"Fuint16"],[4,128,"Fuint32"],[17,128,"Fint8"],[18,128,"Fint16"],[20,128,"Fint32"],[36,128,"Ffloat32"],[6,128,"Ftime"]]})");
        },"TstructAllTypes");

        doTest([this](){
            TderivedStruct s;
            return testStruct(s,R"({"e":{},"d":[[36,0,"val1"],[36,0,"val2"],[36,0,"val3"]]})");
        },"TderivedStruct struct");

        doTest([this](){
            TdoubleDerivedStruct s;
            return testStruct(s,R"({"e":{},"d":[[36,0,"val1"],[36,0,"val2"],[36,0,"val3"],[36,0,"val4"]]})");
        },"TdoubleDerivedStruct struct");

        doTest([this](){
            TnestedStruct s;
            return testStruct(s,R"({"e":{},"d":[[1,0,"rootVal0"],[66,0,"doubleDerived",[[36,0,"val1"],[36,0,"val2"],[36,0,"val3"],[36,0,"val4"]]],[1,0,"rootValN"]]})");
        },"TnestedStruct struct");

        doTest([this](){
            TdoubleNestedStruct s;
            return testStruct(s,R"({"e":{},"d":[[66,0,"nestedStruct",[[1,0,"rootVal0"],[66,0,"doubleDerived",[[36,0,"val1"],[36,0,"val2"],[36,0,"val3"],[36,0,"val4"]]],[1,0,"rootValN"]]],[1,0,"ValN"]]})");
        },"TdoubleNestedStruct struct");

        doTest([this](){
            TderivedDoubleNestedStruct s;
            return testStruct(s,R"({"e":{},"d":[[66,0,"nestedStruct",[[1,0,"rootVal0"],[66,0,"doubleDerived",[[36,0,"val1"],[36,0,"val2"],[36,0,"val3"],[36,0,"val4"]]],[1,0,"rootValN"]]],[1,0,"ValN"],[1,0,"valEnd"]]})");
        },"TdoubleNestedStruct struct");

		testOperators();
        return false;
    };
} typedef_test("typedef_test");