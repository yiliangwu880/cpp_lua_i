#include "unit_test.h"
#include "cpp_lua_mgr.h"
#include "ScriptCaller.h"

using namespace std;

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
	int a = lua_tointeger(pState, -1);
	UNIT_INFO("a=%d", a);
	return 0;
}

int LuaCallFun_exception(lua_State *pState)
{
	CppLuaMgr::Obj().MakeLuaException("LuaCallFun_exception make exception");
	return 0;
}

void RunException()
{

	UNIT_INFO("run exception start");
	CppLuaMgr::Obj().DoFile("../sample/script/exception_main.lua");

	UNIT_INFO("-------------------");
	{
		ScriptCaller caller("callLuaException");
		if (caller.IsOk())
		{
			caller.Call();
		}
	}

}

int main(int argc, char* argv[])
{
	CppLuaMgr::Obj().SetInfoLogFun(MyLogFun);
	CppLuaMgr::Obj().SetErrorLogFun(MyErrorLogFun);
	CppLuaMgr::Obj().SetExcLogFun(MyExcLogFun);

	REG_C_FUN(LuaCallFun1);
	REG_C_FUN(LuaCallFun_exception);

	CppLuaMgr::Obj().DoFile("../sample/script/main.lua");
	CppLuaMgr::Obj().DoString("local a  ='do string ok'; print(a)");

	{
		ScriptCaller caller("call1");
		if (caller.IsOk())
		{
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
	}
	{
		ScriptCaller caller("call1");
		caller << 13 << "a";
		if (caller.IsOk())
		{
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
	}

	RunException();
	return 0;
}

