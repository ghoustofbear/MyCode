@echo off

copy /Y "c2cap.h"                ..\include\
copy /Y "c2cap.h"                ..\..\ui\client\include\

if not exist "x32\Debug\c2cap_d.lib" goto :Check2
if not exist "x32\Debug\c2cap_d.dll" goto :Check2

copy /Y "x32\Debug\c2cap_d.lib"  ..\lib\windows\x32
copy /Y "x32\Debug\c2cap_d.lib"  ..\..\ui\client\lib\windows\x32
copy /Y "x32\Debug\c2cap_d.dll"  ..\..\ui\client\bin\windows\x32
copy /Y "x32\Debug\c2cap_d.dll"  ..\..\ui\server\bin\windows\x32


:Check2
if not exist "x32\Release\c2cap.lib" goto :Check3
if not exist "x32\Release\c2cap.dll" goto :Check3

copy /Y "x32\Release\c2cap.lib"  ..\lib\windows\x32
copy /Y "x32\Release\c2cap.lib"  ..\..\ui\client\lib\windows\x32
copy /Y "x32\Release\c2cap.dll"  ..\..\ui\client\bin\windows\x32
copy /Y "x32\Release\c2cap.dll"  ..\..\ui\server\bin\windows\x32


:Check3
if not exist "x64\Debug\c2cap_d.lib" goto :Check4
if not exist "x64\Debug\c2cap_d.dll" goto :Check4

copy /Y "x64\Debug\c2cap_d.lib"  ..\lib\windows\x64
copy /Y "x64\Debug\c2cap_d.lib"  ..\..\ui\client\lib\windows\x64
copy /Y "x64\Debug\c2cap_d.dll"  ..\..\ui\client\bin\windows\x64
copy /Y "x64\Debug\c2cap_d.dll"  ..\..\ui\server\bin\windows\x64

:Check4
if not exist "x64\Release\c2cap.lib" goto :EndFile
if not exist "x64\Release\c2cap.dll" goto :EndFile

copy /Y "x64\Release\c2cap.lib"  ..\lib\windows\x64
copy /Y "x64\Release\c2cap.lib"  ..\..\ui\client\lib\windows\x64
copy /Y "x64\Release\c2cap.dll"  ..\..\ui\client\bin\windows\x64
copy /Y "x64\Release\c2cap.dll"  ..\..\ui\server\bin\windows\x64

:EndFile
