
--require "common/class"

print("start\n")
log_print("abc")
--log_error("abc")
--local aa = [[adbd]]
--log_print(aa)
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

function call1()
	local a = 1
	call2()
end

function test()
	local a = 0
	print("test 123455")
	
	DDerror("al12")
	return 1,2,3, "12345", 67111111111
end
test()