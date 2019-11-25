#include "cpp_lua_mgr.h"
using namespace std;


namespace{





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




}
CppLuaMgr::CppLuaMgr()
{
	m_pState = luaL_newstate();
	lua_atpanic(m_pState, Panic);
	luaL_openlibs(m_pState);

#define REG_FUN(fun) RegFun2Lua(#fun, fun)
	REG_FUN(LogInfo);
	REG_FUN(LogError);
	REG_FUN(table_pack);
	REG_FUN(table_unpack);
#undef REG_FUN
}

CppLuaMgr::~CppLuaMgr()
{
	if (m_pState) {
		lua_close(m_pState);
		m_pState = nullptr;
	}
}


int  CppLuaMgr::Panic(lua_State* pState) {
	const char *msg = lua_tostring(pState, -1);
	CppLuaMgr::Obj().TryError(msg);
	return 0;
}

int CppLuaMgr::ExceptionHandle(lua_State *pState) 
{
	const char *msg = lua_tostring(pState, -1);
	if (msg) {
		CppLuaMgr::Obj().TryException(msg);
	}

	CppLuaMgr::Obj().PrintStack(pState);


	return 0;
}


int CppLuaMgr::LogError(lua_State* L)
{
	const char *msg = lua_tostring(L, -1);
	CppLuaMgr::Obj().TryError(msg);
	return 0;
}

int CppLuaMgr::LogInfo(lua_State *L)
{
	const char *msg = lua_tostring(L, -1);

	if (!CppLuaMgr::Obj().m_info_log_fun) {
		printf("%s\n", msg);
		return 0;
	}

	CppLuaMgr::Obj().DoLogMessage(CppLuaMgr::Obj().m_info_log_fun, msg);
	return 0;
}

void CppLuaMgr::RegFun2Lua(const char* funName, lua_CFunction fun) {
	lua_pushcfunction(m_pState, fun);
	lua_setglobal(m_pState, funName);
}

bool CppLuaMgr::DoString(const char* str) {
	if (luaL_dostring(m_pState, str)) {
		ExceptionHandle(m_pState);

		return false;
	}
	return true;
}

void CppLuaMgr::MakeLuaException(const char* errorMsg, ...) {
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


void CppLuaMgr::SetExcLogFun(EXC_LOG_FUN function)
{
	m_exc_log_fun = function;
}



bool CppLuaMgr::DoFile(const char* filePath)
{
	lua_pushcfunction(m_pState, ExceptionHandle);
	int tb = lua_gettop(m_pState);
	bool failed = (luaL_loadfile(m_pState, filePath) || lua_pcall(m_pState, 0, LUA_MULTRET, tb));

	return !failed;
}

void CppLuaMgr::TryInfo(const char* msg)
{
	if (!m_info_log_fun) {
		printf("%s\n", msg); 
		return;
	}

	DoLogMessage(m_info_log_fun, msg);
}

void CppLuaMgr::TryError(const char* msg) {
	if (!m_error_log_fun) {// 显示在控制台提示一下
		return;
	}

	DoLogMessage(m_error_log_fun, msg);
}


void CppLuaMgr::TryException(const char* msg) {
	
	if (!m_exc_log_fun) {
		printf("%s\n", msg); // 显示在控制台提示一下
		return;
	}
	m_exc_log_fun(msg);
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


int CppLuaMgr::PrintStack(lua_State *pState) {
	std::stringstream stackInfo;

	lua_Debug info;
	int depth = 0;
	stackInfo << "\n";
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
	if (!CppLuaMgr::Obj().m_exc_log_fun) {
		printf("%s\n", stackInfo.str().c_str());
		return 0;
	}

	CppLuaMgr::Obj().m_exc_log_fun(stackInfo.str().c_str());
	return 0;
}






