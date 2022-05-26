@echo off
REM -- 批处理

echo 自动复制文件.lib/.dll到目标位置
echo 请注意修改auto_copy.bat文件中的目标路径

copy /Y "exports.h"                   ..\..\ui\server\include\
copy /Y "onuiupdate.h"                ..\..\ui\server\include\
copy /Y "servmgr.h"                   ..\..\ui\server\include\
copy /Y "log.h"                       ..\..\ui\server\include\
copy /Y "config\cfg*.h"               ..\..\ui\server\include\
copy /Y "config\view*.h"              ..\..\ui\server\include\
copy /Y "config\observable.h"         ..\..\ui\server\include\
copy /Y "config\observer.h"           ..\..\ui\server\include\

copy /Y "..\..\ui\server\rc\log.ini"  ..\..\ui\server\bin\windows\x32
copy /Y "..\..\ui\server\rc\log.ini"  ..\..\ui\server\bin\windows\x64
rem copy /Y "..\..\ui\server\rc\c2msv.db" ..\..\ui\server\bin\windows\x32
rem copy /Y "..\..\ui\server\rc\c2msv.db" ..\..\ui\server\bin\windows\x64

if not exist "x32\Debug\c2msv_d.lib" goto :Check2
if not exist "x32\Debug\c2msv_d.dll" goto :Check2

echo 开始复制x32\Debug中所需文件
copy /Y "x32\Debug\c2msv_d.lib"     ..\lib\windows\x32
copy /Y "x32\Debug\c2msv_d.lib"     ..\..\ui\server\lib\windows\x32
copy /Y "x32\Debug\c2msv_d.dll"     ..\..\ui\server\bin\windows\x32
echo 完成复制x32\Debug中所需文件

:Check2
if not exist "x32\Release\c2msv.lib" goto :Check3
if not exist "x32\Release\c2msv.dll" goto :Check3

echo 开始复制x32\Release中所需文件
copy /Y "x32\Release\c2msv.lib"     ..\lib\windows\x32
copy /Y "x32\Release\c2msv.lib"     ..\..\ui\server\lib\windows\x32
copy /Y "x32\Release\c2msv.dll"     ..\..\ui\server\bin\windows\x32
echo 完成复制中的x32\Release中所需文件


:Check3
if not exist "x64\Debug\c2msv_d.lib" goto :Check4
if not exist "x64\Debug\c2msv_d.dll" goto :Check4

echo 开始复制x64\Debug中所需文件
copy /Y "x64\Debug\c2msv_d.lib"  ..\lib\windows\x64
copy /Y "x64\Debug\c2msv_d.lib"  ..\..\ui\server\lib\windows\x64
copy /Y "x64\Debug\c2msv_d.dll"  ..\..\ui\server\bin\windows\x64
echo 完成复制x64\Debug中所需文件

:Check4
if not exist "x64\Release\c2msv.lib" goto :EndFile
if not exist "x64\Release\c2msv.dll" goto :EndFile

echo 开始复制x64\Release中所需文件
copy /Y "x64\Release\c2msv.lib"     ..\lib\windows\x64
copy /Y "x64\Release\c2msv.lib"     ..\..\ui\server\lib\windows\x64
copy /Y "x64\Release\c2msv.dll"     ..\..\ui\server\bin\windows\x64
echo 完成复制中的x64\Release中所需文件

:EndFile
