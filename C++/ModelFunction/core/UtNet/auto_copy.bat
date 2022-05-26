@echo off
REM -- 批处理

echo 自动复制文件.lib/.dll到目标位置
echo 请注意修改auto_copy.bat文件中的目标路径

if not exist "Debug\UtNetD.lib" goto :Check2
if not exist "Debug\UtNetD.dll" goto :Check2

echo 开始复制Debug中所需文件
copy /Y "ut_net.h"          ..\..\include
copy /Y "ut_net.h"          ..\..\..\ui\client\include
copy /Y "Debug\UtNetD.lib"  ..\..\lib
copy /Y "Debug\UtNetD.lib"  ..\..\..\ui\client\win32\lib
copy /Y "Debug\UtNetD.dll"  ..\..\..\ui\client\win32\bin
copy /Y "Debug\UtNetD.dll"  ..\..\..\ui\server\win32\bin
echo 完成复制Debug中所需文件

:Check2
if not exist "Release\UtNet.lib" goto :EndFile
if not exist "Release\UtNet.dll" goto :EndFile

echo 开始复制Release中所需文件
copy /Y "ut_net.h"           ..\..\include
copy /Y "ut_net.h"           ..\..\..\ui\client\include
copy /Y "Release\UtNet.lib"  ..\..\lib
copy /Y "Release\UtNet.lib"  ..\..\..\ui\client\win32\lib
copy /Y "Release\UtNet.dll"  ..\..\..\ui\client\win32\bin
copy /Y "Release\UtNet.dll"  ..\..\..\ui\server\win32\bin
echo 完成复制中的Release中所需文件

:EndFile


