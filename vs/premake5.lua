--
-- premake5 file to build RecastDemo
-- http://premake.github.io/
--
require "lib"

local action = _ACTION or ""
local outdir = action


WorkSpaceInit  "cpp_lua_i"



Project "cpp_lua_i"
	IncludeFile { 
		"../include/",
	}

	SrcPath { 
		"../src/**",  --**递归所有子目录，指定目录可用 "cc/*.cpp" 或者  "cc/**.cpp"
		"../bin/**", 
		"../include/**", 
	}
	files {
	"../*.txt",
	"../*.lua",
	"../*.sh",
	}
	
    
 
Project "sample"
	IncludeFile { 
		"../include/",
	}

	SrcPath { 
		"../sample/**",  --**递归所有子目录，指定目录可用 "cc/*.cpp" 或者  "cc/**.cpp"
		"../bin/sample/**", 
	}

	    
 
Project "test"
	IncludeFile { 
		"../include/",
	}

	SrcPath { 
		"../test/**",  --**递归所有子目录，指定目录可用 "cc/*.cpp" 或者  "cc/**.cpp"
		"../bin/test/**", 
	}

	




