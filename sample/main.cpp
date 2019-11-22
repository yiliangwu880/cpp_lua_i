#include "unit_test.h"
#include "cpp_lua_mgr.h"

using namespace std;

void MyLogFun(const char* fileName, uint32_t lineNum, const char* msg)
{
	UNIT_INFO("%s [filename=%s line=%d] ", msg, fileName, lineNum );
}
void MyErrorLogFun(const char* fileName, uint32_t lineNum, const char* msg)
{
	UNIT_ERROR("%s [filename=%s line=%d] ", msg, fileName, lineNum);
}

int main(int argc, char* argv[])
{
	CppLuaMgr::Obj().SetInfoLogFun(MyLogFun);
	CppLuaMgr::Obj().SetErrorLogFun(MyErrorLogFun);

	CppLuaMgr::Obj().LoadScript("../sample/script/", "../sample/script/main.lua");
	return 0;
}

