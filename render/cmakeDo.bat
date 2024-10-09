@echo off
​
set buildDir=build
​
@REM build的文件夹不存在则创建
if not exist %buildDir% md %buildDir%
​
if exist %buildDir% cd %buildDir%
echo "***************:Execute cmake ***************"
cmake ./
@cmake -G"Unix Makefiles" ../
echo "***************:Execute make ***************"
make 
​
pause