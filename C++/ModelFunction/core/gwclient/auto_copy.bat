@echo off
REM -- 批处理

echo 自动复制文件.lib/.dll到目标位置
echo 请注意修改auto_copy.bat文件中的目标路径

copy /Y "gwsdk.h"                  ..\include\
copy /Y "gwsdk.h"                  ..\..\ui\client\include\

if not exist "x32\Debug\gwsdk_d.lib" goto :Check2
if not exist "x32\Debug\gwsdk_d.dll" goto :Check2

echo 开始复制x32\Debug中所需文件
copy /Y "x32\Debug\gwsdk_d.lib"    ..\lib\windows\x32
copy /Y "x32\Debug\gwsdk_d.lib"    ..\..\ui\client\lib\windows\x32
copy /Y "x32\Debug\gwsdk_d.dll"    ..\..\ui\client\bin\windows\x32
copy /Y "x32\Debug\gwsdk_d.dll"    ..\..\ui\server\bin\windows\x32
echo 完成复制x32\Debug中所需文件

:Check2
if not exist "x32\Release\gwsdk.lib" goto :Check3
if not exist "x32\Release\gwsdk.dll" goto :Check3

echo 开始复制x32\Release中所需文件
copy /Y "x32\Release\gwsdk.lib"    ..\lib\windows\x32
copy /Y "x32\Release\gwsdk.lib"    ..\..\ui\client\lib\windows\x32
copy /Y "x32\Release\gwsdk.dll"    ..\..\ui\client\bin\windows\x32
copy /Y "x32\Release\gwsdk.dll"    ..\..\ui\server\bin\windows\x32
echo 完成复制中的x32\Release中所需文件


:Check3
if not exist "x64\Debug\gwsdk_d.lib" goto :Check4
if not exist "x64\Debug\gwsdk_d.dll" goto :Check4

echo 开始复制x64\Debug中所需文件
copy /Y "x64\Debug\gwsdk_d.lib"    ..\lib\windows\x64
copy /Y "x64\Debug\gwsdk_d.lib"    ..\..\ui\client\lib\windows\x64
copy /Y "x64\Debug\gwsdk_d.dll"    ..\..\ui\client\bin\windows\x64
copy /Y "x64\Debug\gwsdk_d.dll"    ..\..\ui\server\bin\windows\x64
echo 完成复制x64\Debug中所需文件

:Check4
if not exist "x64\Release\gwsdk.lib" goto :EndFile
if not exist "x64\Release\gwsdk.dll" goto :EndFile

echo 开始复制x64\Release中所需文件
copy /Y "x64\Release\gwsdk.lib"    ..\lib\windows\x64
copy /Y "x64\Release\gwsdk.lib"    ..\..\ui\client\lib\windows\x64
copy /Y "x64\Release\gwsdk.dll"    ..\..\ui\client\bin\windows\x64
copy /Y "x64\Release\gwsdk.dll"    ..\..\ui\server\bin\windows\x64
echo 完成复制中的x64\Release中所需文件
 
:EndFile
