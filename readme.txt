简介：
	使用lua5.1 luabridge为基础，封装简化一些cpp lua交互接口。
	快速地给cpp项目接入lua

编译方法：
	整个文件夹放到linux目录，安装cmake gcc git等。

	主目录执行：sh clearBuild.sh 完成编译

vs浏览代码：
	执行.\vs\run.bat,生成sln文件

	
使用方法： 代码的方式提供
		 把src代码加入到你的项目
		 同时要include lib文件夹里面的库和头文件

目录结构：
	src == 交互接口源码。