#pragma once

extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
};

#include "LuaBridge/LuaBridge.h"
#include <functional>
#include <string>
#include <sstream>

using LOG_FUN = std::function<void(const char* fileName, uint32_t lineNum, const char* msg)>;
using EXC_LOG_FUN = std::function<void(const char* msg)>;//exception log fun

//cpp lua interface Mgr
class LuaCaller;
class CppLuaMgr
{
	friend class LuaCaller;
private:
	lua_State*   m_pState        = nullptr;
	LOG_FUN      m_info_log_fun  = nullptr;
	LOG_FUN      m_error_log_fun = nullptr;
	EXC_LOG_FUN  m_exc_log_fun   = nullptr;

public:
	static CppLuaMgr &Obj() 
	{
		static CppLuaMgr obj;
		return obj;
	}

	lua_State* GetLuaState() {return m_pState;}

	void SetInfoLogFun(LOG_FUN function);
	void SetErrorLogFun(LOG_FUN function);
	void SetExcLogFun(EXC_LOG_FUN function);
	// 注册一个c函数到lua
	void RegFun2Lua(const char* funName, lua_CFunction fun);
	//加载并运行脚本，或者热更新
	bool DoFile(const char* filePath);
	bool DoString(const char* str);

	//抛出一个Lua异常,中断后续的lua执行
	void MakeLuaException(const char* errorMsg, ...);
private:
	CppLuaMgr();
	~CppLuaMgr();

	void TryInfo(const char* msg);
	void TryError(const char* msg);
	void TryException(const char* msg);

	static int LogInfo(lua_State *L);
	static int LogError(lua_State* L);
	static int PrintStack(lua_State *pState);
	static int Panic(lua_State* pState);
	static int ExceptionHandle(lua_State *pState);

	void DoLogMessage(LOG_FUN function, const char* msg);
};


#define REG_C_FUN(fun) CppLuaMgr::Obj().RegFun2Lua(#fun, fun)
