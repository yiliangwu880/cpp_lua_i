#include "cpp_lua_mgr.h"
using namespace std;


namespace{
static const char * INIT_SCRIPT = R"(
	local path,script = ...
	path = string.match(path,[[(.*)/[^/]*$]])
	package.path = package.path .. [[;]] .. path .. [[/?.lua;]] .. path .. [[/?/init.lua]]
	local f = assert(loadfile(script))
	f(script)
)";

static int Panic(lua_State* pState) {
	const char *msg = lua_tostring(pState, -1);
	gScriptManager.TryError(msg);
	return 0;
}

//static int adb(lua_State *pState)
//{
//	gScriptManager.GetDebugger()->Debug();
//	return 0;
//}

//copy from lua5.2 ltablib.c 
static int table_pack(lua_State *L) {
	int n = lua_gettop(L);  /* number of elements to pack */
	lua_createtable(L, n, 1);  /* create result table */
	lua_pushinteger(L, n);
	lua_setfield(L, -2, "n");  /* t.n = number of elements */
	if (n > 0) {  /* at least one element? */
		int i;
		lua_pushvalue(L, 1);
		lua_rawseti(L, -2, 1);  /* insert first element */
		lua_replace(L, 1);  /* move table into index 1 */
		for (i = n; i >= 2; i--)  /* assign other elements */
			lua_rawseti(L, 1, i);
	}
	return 1;  /* return table */
}


static int table_unpack(lua_State *L) {
	int i, e;
	unsigned int n;
	luaL_checktype(L, 1, LUA_TTABLE);
	i = luaL_optint(L, 2, 1);
	e = luaL_opt(L, luaL_checkint, 3, luaL_getn(L, 1));
	if (i > e) return 0;  /* empty range */
	n = (unsigned int)e - (unsigned int)i;  /* number of elements minus 1 */
	if (n > (INT_MAX - 10) || !lua_checkstack(L, ++n))
		return luaL_error(L, "too many results to unpack");
	lua_rawgeti(L, 1, i);  /* push arg[i] (avoiding overflow problems) */
	while (i++ < e)  /* push arg[i + 1...e] */
		lua_rawgeti(L, 1, i);
	return n;
}



static int log_error(lua_State* L) 
{
	const char *msg = lua_tostring(L, -1);
	gScriptManager.TryError(msg);
	return 0;
}

static int log_print(lua_State *L) 
{
	const char *msg = lua_tostring(L, -1);
	gScriptManager.TryLog(msg);
	return 0;
}

int Traceback(lua_State *pState) {
	const char *msg = lua_tostring(pState, -1);

	if (msg) {
		printf("%s", msg); // 显示在控制台提示一下
		gScriptManager.TryError(msg);
	}

	CppLuaMgr::Obj().PrintStack(pState);

	//if (!gScriptManager.IsProtectModel()) {
	//	gScriptManager.GetDebugger()->Debug();
	//}

	return 0;
}


}
CppLuaMgr::CppLuaMgr()
{
	m_pState = luaL_newstate();
	lua_atpanic(m_pState, Panic);
	luaL_openlibs(m_pState);

#define RegFun(fun) RegFun2Lua(#fun, fun)
	RegFun(log_print);
	RegFun(log_error);
	RegFun(table_pack);
	RegFun(table_unpack);
#undef RegFun
}

CppLuaMgr::~CppLuaMgr()
{
	if (m_pState) {
		lua_close(m_pState);
		m_pState = nullptr;
	}
}


void CppLuaMgr::AddSearchPath(const char* path) {

	char strPath[1024] = { 0 };
	sprintf(strPath, "local path = string.match([[%s]],[[(.*)/[^/]*$]])\n package.path = package.path .. [[;]] .. path .. [[/?.lua;]] .. path .. [[/?/init.lua]]\n", path);
	ExecuteString(strPath);
}

bool CppLuaMgr::LoadScript(const char* path, const char* file) {
	lua_pushcfunction(m_pState, Traceback);//用途未明

	int tb = lua_gettop(m_pState);
	luaL_loadstring(m_pState, INIT_SCRIPT);

	lua_pushstring(m_pState, path);
	lua_pushstring(m_pState, file);

	return lua_pcall(m_pState, 2, 0, tb) == 0;


}

void CppLuaMgr::RegFun2Lua(const char* funName, lua_CFunction fun) {
	lua_pushcfunction(m_pState, fun);
	lua_setglobal(m_pState, funName);
}

bool CppLuaMgr::ExecuteString(const char* str) {
	if (luaL_dostring(m_pState, str)) {
		Traceback(m_pState);

		return false;
	}
	return true;
}

void CppLuaMgr::GenerateLuaError(const char* errorMsg, ...) {
	va_list args;
	char msg[256];
	va_start(args, errorMsg);
	vsnprintf(msg, sizeof(msg) / sizeof(*msg), errorMsg, args);

	va_end(args);

	lua_pushstring(m_pState, msg);

	lua_error(m_pState);
}


void CppLuaMgr::SetInfoLogFun(LOG_FUN function) {
	m_info_log_fun = function;
}

void CppLuaMgr::SetErrorLogFun(LOG_FUN function) {
	m_error_log_fun = function;
}

bool CppLuaMgr::ReloadFile(const char* filePath)
{
	bool failed = luaL_dofile(m_pState, filePath);

	if (failed)
	{
		TryError(lua_tostring(m_pState, -1));
	}

	return !failed;
}

void CppLuaMgr::TryLog(const char* msg)
{
	if (!m_info_log_fun) {
		printf("%s\n", msg);
		return;
	}

	DoLogMessage(m_info_log_fun, msg);
}

void CppLuaMgr::TryError(const char* msg) {
	if (!m_error_log_fun) {
		printf("%s\n", msg); // 显示在控制台提示一下
		return;
	}

	DoLogMessage(m_error_log_fun, msg);
}



void CppLuaMgr::DoLogMessage(LOG_FUN function, const char* msg) {
	lua_Debug info;
	int depth = 0;
	while (lua_getstack(m_pState, depth, &info)) {

		lua_getinfo(m_pState, "S", &info);
		lua_getinfo(m_pState, "n", &info);
		lua_getinfo(m_pState, "l", &info);

		if (info.source[0] == '@') {
			function(&info.source[1], info.currentline, msg);
			return;
		}

		++depth;
	}
	if (depth == 0) {
		// 说明没有堆栈
		function("trunk", 0, msg);
	}
}


void CppLuaMgr::PrintStack(lua_State *pState) {
	std::stringstream stackInfo;

	lua_Debug info;
	int depth = 0;
	while (lua_getstack(pState, depth, &info)) {
		stackInfo << depth << " ";

		lua_getinfo(pState, "S", &info);
		lua_getinfo(pState, "n", &info);
		lua_getinfo(pState, "l", &info);

		if (info.name) {
			stackInfo << info.name;
		}
		else {
			stackInfo << "(trunk)";
		}

		stackInfo << " ";

		stackInfo << info.source;

		if (info.source[0] == '@') {
			stackInfo << " :" << info.currentline;
		}

		stackInfo << "\n";
		++depth;
	}


	gScriptManager.TryError(stackInfo.str().c_str());
}






