
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
test();
