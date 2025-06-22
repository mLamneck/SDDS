#include "uLinkedList.h"
#include "uTestCase.h"

class TtestLinkedList : public TtestCase{
    using TtestCase::TtestCase;

	class TmyListItem : public TlinkedListElement{
		static int CNT(){
			static int cnt = 0;
			return cnt++;
		}
		public:
			int Fcnt;
			TmyListItem(){
				Fcnt = CNT();
			}

	};

	typedef TlinkedList<TmyListItem> TmyList;
	TmyList FmyList;
	TmyListItem Fa1;
	TmyListItem Fa2;
	TmyListItem Fa3;

    bool testConvTimeCase(const char* _input, const char* _expectedOutput, bool _expectedSucc = true){
/*
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
*/
		return true;
    }

    bool testConvTimeCase(const char* _input, bool _expectedSucc = true){
       // return testConvTimeCase(_input,_input,_expectedSucc);
	   return true;
    }

    void testTimeConv(){
		/*
        doTest([this](){ return testConvTimeCase("28.11.2023 19:12:30"); },"case 1");
        doTest([this](){ return testConvTimeCase("28.11.2023 19:12:30.765"); },"case 2");
        doTest([this](){ return testConvTimeCase("19:12:30","01.01.1970 19:12:30"); },"case 3");
        doTest([this](){ return testConvTimeCase("01.01.1970T19:12:30","01.01.1970 19:12:30"); },"case 4");
        doTest([this](){ return testConvTimeCase("05/30/2020 19:12:30","30.05.2020 19:12:30"); },"case 5");
        doTest([this](){ return testConvTimeCase("10.10.2023 00:00:00","10.10.2023 00:00:00"); },"case 6");
        doTest([this](){ return testConvTimeCase("10.10.2023","10.10.2023 00:00:00"); },"case 7");
        doTest([this](){ return testConvTimeCase("31/30/2020 19:12:30",false); },"case 8");
		*/
    }

	dtypes::string listToString(TmyList* _list){
		std::string res = "";
		for (auto it = _list->iterator(); it.hasCurrent();){
			res = res + strConv::to_string(it.current()->Fcnt);
			if (it.jumpToNext())
				res = res + ",";
		}
		return res;
	}

	bool testResult(TmyList* _list, dtypes::string _expRes){
		dtypes::string res = listToString(_list);
		if (res != _expRes){
			debug::log("testcase failed res='%s' != '%s'=expRes",res.c_str(),_expRes.c_str());
			return false;
		}
		return true;
	}

	void fillList(){
		FmyList.push_back(&Fa1);
		FmyList.push_back(&Fa2);
		FmyList.push_back(&Fa3);
	}

	void clearList(){
		while (FmyList.pop()) { }
	}

	bool test_pushBack(){
		fillList();
		return testResult(&FmyList,"0,1,2");
	}

	bool test_pop(){
		fillList();

		FmyList.pop();
		if (Fa1.linked()){
			debug::log("testcase failed '%d' shouldn't be linked but it is",Fa1.Fcnt);
			return false;
		}
		if (!testResult(&FmyList,"1,2")) return false;

		FmyList.pop();
		if (Fa2.linked()){
			debug::log("testcase failed '%d' shouldn't be linked but it is",Fa2.Fcnt);
			return false;
		}
		if (!testResult(&FmyList,"2")) return false;

		FmyList.pop();
		if (Fa1.linked()){
			debug::log("testcase failed '%d' shouldn't be linked but it is",Fa1.Fcnt);
			return false;
		}
		if (!testResult(&FmyList,"")) return false;

		//pop with empty list
		FmyList.pop();

		clearList();
		return true;
	}

	bool test_popLast(){
		fillList();

		FmyList.pop_last();
		if (Fa3.linked()){
			debug::log("testcase failed '%d' shouldn't be linked but it is",Fa3.Fcnt);
			return false;
		}
		if (!testResult(&FmyList,"0,1")) return false;

		FmyList.pop_last();
		if (Fa2.linked()){
			debug::log("testcase failed '%d' shouldn't be linked but it is",Fa2.Fcnt);
			return false;
		}
		if (!testResult(&FmyList,"0")) return false;

		FmyList.pop_last();
		if (Fa1.linked()){
			debug::log("testcase failed '%d' shouldn't be linked but it is",Fa1.Fcnt);
			return false;
		}
		if (!testResult(&FmyList,"")) return false;
		
		//pop with empty list
		FmyList.pop_last();
		
		clearList();
		return true;
	}

    bool test() override{
		doTest([this](){ return test_pushBack(); }, "pushBack");
		doTest([this](){ return test_pop(); }, "test_pop");
		doTest([this](){ return test_popLast(); }, "test_popLast");
		return true;
    }

} linkedList_test("linkedList_test");

