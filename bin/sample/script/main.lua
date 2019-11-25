
--require "common/class"




print("start\n")
LogInfo("abc")
--LogError("abc")
--local aa = [[adbd]]
--LogInfo(aa)
function call3()
	local a  =3
	local b = 123

	for i,v in ipairs({123}) do
		print(i,v)
	end

	print(12)
	print(1)
	print(3)
	debug()
end

function call2()
	local a = 2
	debug()
	call3()
end

function call1(p1,p2)
	local a = 1
	--call2()
	return p1,p2
end
function callLuaException2(p1,p2)
	DDerror("al12") --exception
end
function callLuaException()
	callLuaException2();
end
function test()
	local a = 0
	print("test 123455")
	
	--DDerror("al12")
	return 1,2,3, "12345", 67111111111
end

LuaCallFun1(55);
--LuaCallFun_exception();
test()
