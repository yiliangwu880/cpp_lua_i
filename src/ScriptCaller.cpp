
#include "ScriptCaller.h"

#include <string>

static const int WEIRD_POS = 20;

ScriptCaller::ScriptCaller( const char* funName)
	: m_pState(CppLuaMgr::Obj().GetLuaState())
	, m_numArgs(0)
	, m_called(false)
{
	lua_pushcfunction(m_pState, Traceback);
	m_tb = lua_gettop(m_pState);

	if (m_tb >= WEIRD_POS) {
		gScriptManager.TryError("lua 堆栈上caller有异常！！！");
	}

    std::string fun = funName;
	size_t index = fun.find('.');
    if (index != std::string::npos) {
        std::string table = fun.substr(0, index);
        std::string realFun = fun.substr( index + 1, fun.length() - 1);
        lua_getglobal(m_pState, table.c_str());

        if (lua_istable(m_pState, -1)) {
            lua_pushstring(m_pState, realFun.c_str());
            lua_rawget(m_pState, -2);

            lua_remove(m_pState, -2); // pop掉 funName占的栈
        }
        
    } else {
        lua_getglobal(m_pState, funName);
    }
}




ScriptCaller::~ScriptCaller() {
    lua_settop(m_pState, m_tb - 1); 
}

int ScriptCaller::Traceback(lua_State *pState) {
	const char *msg = lua_tostring(pState, -1);

	if (msg) {
        printf("%s", msg); // 显示在控制台提示一下
        gScriptManager.TryError(msg);
	}
    
	CppLuaMgr::Obj().PrintStack(pState);


    
	return 0;
}

ScriptCaller& ScriptCaller::Arg(int value) {
	lua_pushinteger(m_pState, value);
	++m_numArgs;
	return *this;
}
 
ScriptCaller& ScriptCaller::Arg(const char* value) {
	lua_pushstring(m_pState, value);
	++m_numArgs;
	return *this;
}

ScriptCaller& ScriptCaller::Arg(double value) {
	lua_pushnumber(m_pState, value);
	++m_numArgs;
	return *this;
}

ScriptCaller& ScriptCaller::Arg(bool value) {
	lua_pushboolean(m_pState, value);
	++m_numArgs;
	return *this;
}

ScriptCaller& ScriptCaller::Arg(unsigned long value) {
    luabridge::Stack<unsigned long>::push(m_pState, value);
    ++m_numArgs;

    return *this;
}

ScriptCaller& ScriptCaller::Arg(unsigned long long value) {
    luabridge::Stack<unsigned long long>::push(m_pState, value);
    ++m_numArgs;

    return *this;
}


ScriptCaller& ScriptCaller::Arg(unsigned int value)
{
    lua_pushinteger(m_pState, value);
    ++m_numArgs;
    return *this;
}

ScriptCaller& ScriptCaller::Arg(long int value) {
    luabridge::Stack<long int>::push(m_pState, value);
    ++m_numArgs;
    return *this;
}

ScriptCaller& ScriptCaller::Arg(const std::string& value) {
    lua_pushlstring(m_pState, value.c_str(), value.size());
    ++m_numArgs;
    return *this;
}

bool ScriptCaller::EnableCall() {
    return lua_isfunction(m_pState, -1) || lua_iscfunction(m_pState, -1);
}

bool ScriptCaller::Call(LuaResult* result) {
	if (m_called) {
		// CTODO: log日志
		return false;
	}
	else {
		m_called = true;
	}

    if (lua_pcall(m_pState, m_numArgs, LUA_MULTRET, m_tb)) {
		return false;
	}

    if (result) {
        int resultCount = lua_gettop(m_pState) - m_tb;

        result->SetResultCount(resultCount);
        result->SetLuaState(m_pState);
    }

	return true;
}