
--require "common/class"


LogInfo("load and exucte main.lua")
function UNIT_ASSERT(cond)
	if (false == cond) then
		LogError("cond")
	end
end

function FNoParaNoReturn()

	LogInfo("FNoParaNoReturn")
end

function f1(p1,p2)

	LogInfo("f1")
	return p1,p2,3, "12345", 67111111111
end

function GetMyClass(obj)
	local l_obj = test.MyClass ()
	local l_obj=obj
	LogInfo("GetMyClass a=".. obj.a)
	UNIT_ASSERT(obj.a == 44)
	return l_obj
end

function Start_lua_call_cpp()
	local obj = MyClass2()
	obj:fun(55)
end