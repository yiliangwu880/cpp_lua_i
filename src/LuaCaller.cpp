
#include "LuaCaller.h"

#include <string>

static const int WEIRD_POS = 20;

LuaCaller::LuaCaller( const char* funName)
	: m_pState(CppLuaMgr::Obj().GetLuaState())
	, m_numArgs(0)
	, m_called(false)
{
	lua_pushcfunction(m_pState, CppLuaMgr::ExceptionHandle);
	m_tb = lua_gettop(m_pState);

	if (m_tb >= WEIRD_POS) {
		CppLuaMgr::Obj().TryError("lua 堆栈上caller有异常！！！");
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

	m_is_exist_fun = lua_isfunction(m_pState, -1) || lua_iscfunction(m_pState, -1);
}




LuaCaller::~LuaCaller() {
    lua_settop(m_pState, m_tb - 1); 
}


LuaCaller& LuaCaller::Arg(int value) {
	lua_pushinteger(m_pState, value);
	++m_numArgs;
	return *this;
}
 
LuaCaller& LuaCaller::Arg(const char* value) {
	lua_pushstring(m_pState, value);
	++m_numArgs;
	return *this;
}

LuaCaller& LuaCaller::Arg(double value) {
	lua_pushnumber(m_pState, value);
	++m_numArgs;
	return *this;
}

LuaCaller& LuaCaller::Arg(bool value) {
	lua_pushboolean(m_pState, value);
	++m_numArgs;
	return *this;
}

LuaCaller& LuaCaller::Arg(unsigned long value) {
    luabridge::Stack<unsigned long>::push(m_pState, value);
    ++m_numArgs;

    return *this;
}

LuaCaller& LuaCaller::Arg(unsigned long long value) {
    luabridge::Stack<unsigned long long>::push(m_pState, value);
    ++m_numArgs;

    return *this;
}


LuaCaller& LuaCaller::Arg(unsigned int value)
{
    lua_pushinteger(m_pState, value);
    ++m_numArgs;
    return *this;
}

LuaCaller& LuaCaller::Arg(long int value) {
    luabridge::Stack<long int>::push(m_pState, value);
    ++m_numArgs;
    return *this;
}

LuaCaller& LuaCaller::Arg(const std::string& value) {
    lua_pushlstring(m_pState, value.c_str(), value.size());
    ++m_numArgs;
    return *this;
}

bool LuaCaller::IsExistFun() {
    return m_is_exist_fun;
}

bool LuaCaller::ExCall()
{
	if (!m_is_exist_fun)
	{
		return false;
	}
	if (m_called) {
		// CTODO: log日志
		return false;
	}
	else {
		m_called = true;
	}
	//@m_tb errFunc 错误处理函数，0表示无，表示错误处理函数在栈中的索引
    if (lua_pcall(m_pState, m_numArgs, LUA_MULTRET, m_tb)) {
		return false;
	}
	return true;
}


bool LuaCaller::Call(LuaResult* result) {

	if (!ExCall())
	{
		return false;
	}

    if (result) {
        int resultCount = lua_gettop(m_pState) - m_tb;

        result->SetResultCount(resultCount);
        result->SetLuaState(m_pState);
    }

	return true;
}

bool LuaCaller::Result(LuaResult &result)
{
	if (!m_called) 
	{
		return false;
	}
	int resultCount = lua_gettop(m_pState) - m_tb;

	result.SetResultCount(resultCount);
	result.SetLuaState(m_pState);
	return true;
}
