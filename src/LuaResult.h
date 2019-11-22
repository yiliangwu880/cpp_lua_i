#pragma once
#include "cpp_lua_mgr.h"


template<class T>
struct LuaArg {
};

template<>
struct LuaArg<unsigned long> {
	typedef unsigned  long type;
};

template<>
struct LuaArg<unsigned long long> {
	typedef unsigned long long type;
};

template<>
struct LuaArg<int> {
	typedef	int type;
};

template<>
struct LuaArg<double> {
	typedef	double type;
};

template<>
struct LuaArg<void> {
	typedef	void type;
};

template<>
struct LuaArg<char*> {
	typedef	const char* type;
};

template<>
struct LuaArg<const char*> {
	typedef	const char* type;
};

template<>
struct LuaArg<bool> {
	typedef	bool type;
};

template<>
struct LuaArg<float> {
	typedef	float type;
};

// lua返回值
class LuaArgInterface;
struct lua_State;
class ScriptCaller;

class LuaResult {
    friend ScriptCaller;
public:
    LuaResult();
    ~LuaResult();

public:

    template<typename T>
    T GetResult() {
        typedef typename LuaArg<T>::type ValueType;
        ValueType value = ValueType();

        return Result(value);
    }

    int GetResultCount() {
        return m_resultCount;
    }

    template<typename T>
    T GetObjectResult() {
        int index = lua_gettop(m_pState) - m_resultCount + 1;
        --m_resultCount;
        return luabridge::Stack<T>::get(m_pState, index);
    }
    
private:
    void SetLuaState(lua_State* pState);

    void SetResultCount(int count);

    bool Result(bool);
    int Result(int);
    unsigned int Result(unsigned int);
    double Result(double);
    const char* Result(const char*);
    std::string Result(std::string&);
    long Result(long);
    unsigned long Result(unsigned long);
    long long Result(long long);
    unsigned long long Result(unsigned long long);

private:
    lua_State* m_pState;
    int m_resultCount;

};