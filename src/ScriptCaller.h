//cpp 调用lua函数用，简化接口，增加可读性
#pragma once
#include "LuaResult.h"


class ScriptCaller
{
private:
	lua_State* m_pState;
	int m_numArgs;
	int m_tb;
	bool m_called;

public:
	ScriptCaller(const char* fun_name);
	~ScriptCaller();

    int GetArgsNum() {return m_numArgs;}
    void SetArgsNum(int args) { m_numArgs = args; }

    // 是否有这个函数
    bool IsOk();


	//附加参数
	template<typename T>
	ScriptCaller& operator+=(T t) {
		return this->Arg(t);
	}

	template<typename T>
	ScriptCaller& operator<<(T t) {
		return this->Arg(t);
	}
    
    template<typename T>
    ScriptCaller& PushObject(T t) {
        luabridge::push(m_pState, t);
        ++m_numArgs;
        return *this;
    }

    bool Call(LuaResult* result = NULL);

private:

	ScriptCaller& Arg(int value);
	ScriptCaller& Arg(const char* value);
	ScriptCaller& Arg(double value);
	ScriptCaller& Arg(bool value);
    ScriptCaller& Arg(unsigned int value);
    ScriptCaller& Arg(unsigned long value);
    ScriptCaller& Arg(unsigned long long value);
    ScriptCaller& Arg(const std::string& value);
    ScriptCaller& Arg(long int value);
};