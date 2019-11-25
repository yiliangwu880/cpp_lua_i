
--require "common/class"






function callLuaException2(p1,p2)
	DDerror("al12") --exception
end

function callLuaException()
	callLuaException2();
end

function test()
local df=fuud()
end

function LuaCallFun_exception1()
	LogInfo("run  LuaCallFun_exception1 ")
	LuaCallFun_exception();
end

test();
