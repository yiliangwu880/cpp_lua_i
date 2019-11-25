#include "unit_test.h"
#include "cpp_lua_mgr.h"
#include "LuaCaller.h"

using namespace std;
using namespace luabridge;

void MyLogFun(const char* fileName, uint32_t lineNum, const char* msg)
{
	UNIT_INFO("%s [filename=%s line=%d] ", msg, fileName, lineNum );
}
void MyErrorLogFun(const char* fileName, uint32_t lineNum, const char* msg)
{
	UNIT_ERROR("%s [filename=%s line=%d] ", msg, fileName, lineNum);
}

void MyExcLogFun(const char* msg)
{
	UNIT_ERROR("%s ", msg);
}


int LuaCallFun1(lua_State *pState)
{
	UNIT_INFO("LuaCallFun1");
	int a = lua_tointeger(pState, -1);
	UNIT_INFO("a=%d", a);
	return 0;
}

int LuaCallFun_exception(lua_State *pState)
{
	UNIT_INFO("LuaCallFun_exception");
	CppLuaMgr::Obj().MakeLuaException("LuaCallFun_exception make exception");
	return 0;
}

void RunException()
{
	UNIT_INFO("run exception start");
	CppLuaMgr::Obj().DoFile("../sample/script/exception_main.lua");

	UNIT_INFO("-------------------");
	if(0)
	{
		LuaCaller caller("callLuaException");
		caller.Call();
	}
	else
	{
		LuaCaller caller("LuaCallFun_exception1");

		caller << "a";
		caller << 12 << "a";
		UNIT_INFO("--------LuaCallFun_exception1-----------");
		caller.Call();
		
	}

}

int main(int argc, char* argv[])
{
	//init log implement
	CppLuaMgr::Obj().SetInfoLogFun(MyLogFun);
	CppLuaMgr::Obj().SetErrorLogFun(MyErrorLogFun);
	CppLuaMgr::Obj().SetExcLogFun(MyExcLogFun);

	//reg c function to lua
	REG_C_FUN(LuaCallFun1);
	REG_C_FUN(LuaCallFun_exception);

	//load lua and excute
	CppLuaMgr::Obj().DoFile("../sample/script/main.lua");
	CppLuaMgr::Obj().DoString("local a  ='do string ok'; print(a)");

	//call lua function
	{
		LuaCaller caller("call1");
		caller << 12 << "a";
		LuaResult result;
		caller.Call(&result);

		if (result.GetResultCount() == 2)
		{
			UNIT_INFO("result %d ", result.GetResult<int>());
		}
		else
		{
			UNIT_ERROR("caller fail");
		}
	}
	{
		LuaRef same = getGlobal(CppLuaMgr::Obj().GetLuaState(), "call1");

		// These all evaluate to true
		same(12, "a");
	
	}
	{
		LuaCaller caller("call1");
		caller << 13 << "a";
		LuaResult result;
		caller.Call(&result);

		if (result.GetResultCount() == 2)
		{
			UNIT_INFO("result %d ", result.GetResult<int>());
		}
		else
		{
			UNIT_ERROR("caller fail");
		}
	}

	RunException();
	return 0;
}

