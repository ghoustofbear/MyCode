@echo off
copy /Y "c2fplay.h"                  ..\..\include\
copy /Y "c2fplay.h"                  ..\..\..\ui\client\include\
copy /Y "playerlistener.h"                  ..\..\include\
copy /Y "playerlistener.h"                  ..\..\..\ui\client\include\


if not exist "x32\Debug\c2fplay_d.lib" goto :Check2
if not exist "x32\Debug\c2fplay_d.dll" goto :Check2

copy /Y "x32\Debug\c2fplay_d.lib"    ..\..\lib\windows\x32
copy /Y "x32\Debug\c2fplay_d.lib"    ..\..\..\ui\client\lib\windows\x32
copy /Y "x32\Debug\c2fplay_d.dll"    ..\..\..\ui\client\bin\windows\x32
copy /Y "x32\Debug\c2fplay_d.dll"    ..\..\..\ui\server\bin\windows\x32

:Check2
if not exist "x32\Release\c2fplay.lib" goto :Check3
if not exist "x32\Release\c2fplay.dll" goto :Check3

copy /Y "x32\Release\c2fplay.lib"    ..\..\lib\windows\x32
copy /Y "x32\Release\c2fplay.lib"    ..\..\..\ui\client\lib\windows\x32
copy /Y "x32\Release\c2fplay.dll"    ..\..\..\ui\client\bin\windows\x32
copy /Y "x32\Release\c2fplay.dll"    ..\..\..\ui\server\bin\windows\x32


:Check3
if not exist "x64\Debug\c2fplay_d.lib" goto :Check4
if not exist "x64\Debug\c2fplay_d.dll" goto :Check4

copy /Y "x64\Debug\c2fplay_d.lib"    ..\..\lib\windows\x64
copy /Y "x64\Debug\c2fplay_d.lib"    ..\..\..\ui\client\lib\windows\x64
copy /Y "x64\Debug\c2fplay_d.dll"    ..\..\..\ui\client\bin\windows\x64
copy /Y "x64\Debug\c2fplay_d.dll"    ..\..\..\ui\server\bin\windows\x64

:Check4
if not exist "x64\Release\c2fplay.lib" goto :EndFile
if not exist "x64\Release\c2fplay.dll" goto :EndFile

copy /Y "x64\Release\c2fplay.lib"    ..\..\lib\windows\x64
copy /Y "x64\Release\c2fplay.lib"    ..\..\..\ui\client\lib\windows\x64
copy /Y "x64\Release\c2fplay.dll"    ..\..\..\ui\client\bin\windows\x64
copy /Y "x64\Release\c2fplay.dll"    ..\..\..\ui\server\bin\windows\x64
 
:EndFile
