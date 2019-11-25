

#include <string>
#include "unit_test.h"
#include "cpp_lua_mgr.h"
#include "LuaCaller.h"

using namespace std;
using namespace luabridge;

void MyLogFun(const char* fileName, uint32_t lineNum, const char* msg)
{
	UNIT_INFO("%s [filename=%s line=%d] ", msg, fileName, lineNum);
}
void MyErrorLogFun(const char* fileName, uint32_t lineNum, const char* msg)
{
	UNIT_ERROR("%s [filename=%s line=%d] ", msg, fileName, lineNum);
}

void MyExcLogFun(const char* msg)
{
	UNIT_ERROR("%s ", msg);
}


using namespace std;


class MyClass
{
public:
	int a = 3;

	void fun()
	{
		UNIT_INFO("run My class fun");
	}
};

class MyClass2
{
public:
	static int m_is_ok;

	void fun(int p1)
	{
		UNIT_ASSERT(p1 == 55);
		m_is_ok = true;
	}
};

int MyClass2::m_is_ok = false;

namespace
{


	void Init()
	{
		CppLuaMgr::Obj().SetInfoLogFun(MyLogFun);
		CppLuaMgr::Obj().SetErrorLogFun(MyErrorLogFun);
		CppLuaMgr::Obj().SetExcLogFun(MyExcLogFun);
		CppLuaMgr::Obj().DoFile("./script/main.lua");

	}

	void RegClass()
	{
		getGlobalNamespace(CppLuaMgr::Obj().GetLuaState())
			.beginNamespace("test")
			.beginClass <MyClass>("MyClass")
			.addConstructor<void (*)(void)>()
			.addProperty("a", &MyClass::a)
			.addFunction("fun", &MyClass::fun)
			.endClass()
			.endNamespace();

		getGlobalNamespace(CppLuaMgr::Obj().GetLuaState())
			.beginClass <MyClass2>("MyClass2")
			.addConstructor<void(*)(void)>()
			.addFunction("fun", &MyClass2::fun)
			.endClass();

	}
	
	void cpp_call_lua()
	{
		{
			LuaRef same = getGlobal(CppLuaMgr::Obj().GetLuaState(), "f1");
			same(12, "a");
		}

		{
			LuaCaller call("f1");
			call(5, "ba");
			LuaResult r;
			call.Result(r);
			UNIT_ASSERT(r.GetResult<int>() == 5);
			UNIT_ASSERT(r.GetResult<const char *>() == string("ba"));
		}
		{
			LuaCaller call("FNoParaNoReturn");
			call(5, "ba");
			LuaResult r;
			call.Result(r);
			UNIT_ASSERT(r.GetResultCount() == 0);
		}
		{//转递cpp对象参数
			MyClass obj;
			obj.a = 44;
			LuaCaller call("GetMyClass");
			call(obj);
			LuaResult r;
			call.Result(r);
			UNIT_ASSERT(r.GetResultCount() == 1);
			MyClass ret = r.GetObjectResult<MyClass>();
			UNIT_ASSERT(ret.a == 44);
		}
	}

	void lua_call_cpp()
	{
		UNIT_INFO("lua_call_cpp");
		{
			LuaCaller call("Start_lua_call_cpp");
			call();
		}

		UNIT_ASSERT(MyClass2::m_is_ok = true);
	}
}

UNITTEST(test)
{
	Init();
	RegClass();
	cpp_call_lua();
	lua_call_cpp();
}