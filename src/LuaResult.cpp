#include "LuaResult.h"

LuaResult::LuaResult() : m_pState(NULL), m_resultCount(0) {

}

LuaResult::~LuaResult() {
}

void LuaResult::SetLuaState(lua_State* pState) {
    m_pState = pState;
}

void LuaResult::SetResultCount(int count) {
    m_resultCount = count;
}


bool LuaResult::Result(bool) {
    return static_cast<bool>(lua_toboolean(m_pState, -m_resultCount--));
}

int LuaResult::Result(int) {
    return luaL_checkinteger(m_pState, -m_resultCount--);
}

double LuaResult::Result(double) {
    return luaL_checknumber(m_pState, -m_resultCount--);
}

const char* LuaResult::Result(const char*) {
    return luaL_checkstring(m_pState, -m_resultCount--);
}

unsigned int LuaResult::Result(unsigned int) {
    return static_cast<unsigned int>(luaL_checkinteger(m_pState, -m_resultCount--) );
}

long LuaResult::Result(long) {
    const char* str = luaL_checkstring(m_pState, -m_resultCount--);

    long result = 0;
    result = strtoul(str, NULL, 0);
    return result;
}

unsigned long LuaResult::Result(unsigned long) {
   const char* str = luaL_checkstring(m_pState, -m_resultCount--);

   unsigned long result = 0;
   result = strtoul(str, NULL, 0);

   return result;
}
long long LuaResult::Result(long long) {
    const char* str = luaL_checkstring(m_pState, -m_resultCount--);

    long long result = 0;
    result = strtoll(str, NULL, 0);
    return result;
}


unsigned long long LuaResult::Result(unsigned long long) {
    const char* str = luaL_checkstring(m_pState, -m_resultCount--);

    unsigned long long result = 0;
    result = strtoull(str, NULL, 0);
    return result;
}

std::string LuaResult::Result(std::string&)
{
    size_t len;
    const char* str = luaL_checklstring(m_pState, -m_resultCount--, &len);
    return std::string(str, len);
}
