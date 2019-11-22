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

//cpp lua interface Mgr
class CppLuaMgr
{
private:
	lua_State* m_pState = nullptr;
	LOG_FUN  m_info_log_fun = nullptr;
	LOG_FUN  m_error_log_fun = nullptr;

public:
	static CppLuaMgr &Obj() 
	{
		static CppLuaMgr obj;
		return obj;
	}

	lua_State* GetLuaState() {return m_pState;}
	// ����lua�ű�����
	// path  ·��
	// file �����ű��ļ�
	//���� LoadScript("../sample/script/", "../sample/script/main.lua")
	bool LoadScript(const char* path, const char* file);

	// ע��һ��c������lua
	void RegFun2Lua(const char* funName, lua_CFunction fun);

	void AddSearchPath(const char* path);

	bool ExecuteString(const char* str);
	// �ȸ���ָ���ű�
	bool ReloadFile(const char* filePath);

	//��C++ APIϣ���׳�һ��Lua�쳣ʱ����ͨ��lua_error�׳���
	void GenerateLuaError(const char* errorMsg, ...);

	void SetInfoLogFun(LOG_FUN function);
	void SetErrorLogFun(LOG_FUN function);

	static void PrintStack(lua_State *pState);
	void TryLog(const char* msg);

	void TryError(const char* msg);
private:
	CppLuaMgr();
	~CppLuaMgr();


	void DoLogMessage(LOG_FUN function, const char* msg);
};

#define gScriptManager CppLuaMgr::Obj()

