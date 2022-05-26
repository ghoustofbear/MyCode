@echo off
REM -- 批处理


echo 自动复制文件.lib/.dll到目标位置
echo 请注意修改auto_copy.bat文件中的目标路径

if not exist "x32\Debug\c2com_d.lib" goto :Check2
if not exist "x32\Debug\c2com_d.dll" goto :Check2

echo 开始复制x32\Debug中所需文件
copy /Y "x32\Debug\c2com_d.lib"  ..\lib\windows\x32
copy /Y "x32\Debug\c2com_d.lib"  ..\..\ui\client\lib\windows\x32
copy /Y "x32\Debug\c2com_d.dll"  ..\..\ui\client\bin\windows\x32
copy /Y "x32\Debug\c2com_d.lib"  ..\..\ui\server\lib\windows\x32
copy /Y "x32\Debug\c2com_d.dll"  ..\..\ui\server\bin\windows\x32
echo 完成复制x32\Debug中所需文件

:Check2
if not exist "x32\Release\c2com.lib" goto :Check3
if not exist "x32\Release\c2com.dll" goto :Check3

echo 开始复制x32\Release中所需文件
copy /Y "x32\Release\c2com.lib"  ..\lib\windows\x32
copy /Y "x32\Release\c2com.lib"  ..\..\ui\client\lib\windows\x32
copy /Y "x32\Release\c2com.dll"  ..\..\ui\client\bin\windows\x32
copy /Y "x32\Release\c2com.lib"  ..\..\ui\server\lib\windows\x32
copy /Y "x32\Release\c2com.dll"  ..\..\ui\server\bin\windows\x32
echo 完成复制中的x32\Release中所需文件

:Check3
if not exist "x64\Debug\c2com_d.lib" goto :Check4
if not exist "x64\Debug\c2com_d.dll" goto :Check4

echo 开始复制x64\Debug中所需文件
copy /Y "x64\Debug\c2com_d.lib"  ..\lib\windows\x64
copy /Y "x64\Debug\c2com_d.lib"  ..\..\ui\client\lib\windows\x64
copy /Y "x64\Debug\c2com_d.dll"  ..\..\ui\client\bin\windows\x64
copy /Y "x64\Debug\c2com_d.lib"  ..\..\ui\server\lib\windows\x64
copy /Y "x64\Debug\c2com_d.dll"  ..\..\ui\server\bin\windows\x64
echo 完成复制x64\Debug中所需文件

:Check4
if not exist "x64\Release\c2com.lib" goto :EndFile
if not exist "x64\Release\c2com.dll" goto :EndFile

echo 开始复制x64\Release中所需文件
copy /Y "x64\Release\c2com.lib"  ..\lib\windows\x64
copy /Y "x64\Release\c2com.lib"  ..\..\ui\client\lib\windows\x64
copy /Y "x64\Release\c2com.dll"  ..\..\ui\client\bin\windows\x64
copy /Y "x64\Release\c2com.lib"  ..\..\ui\server\lib\windows\x64
copy /Y "x64\Release\c2com.dll"  ..\..\ui\server\bin\windows\x64
echo 完成复制中的x64\Release中所需文件

:EndFile
