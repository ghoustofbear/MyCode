@echo off
REM -- ������

echo �Զ������ļ�.lib/.dll��Ŀ��λ��
echo ��ע���޸�auto_copy.bat�ļ��е�Ŀ��·��

copy /Y "c2sdk.h"                  ..\include\
copy /Y "c2sdk.h"                  ..\..\ui\client\include\

rem copy /Y "..\..\ui\client\rc\*.ini" ..\..\ui\client\bin\windows\x32
rem copy /Y "..\..\ui\client\rc\*.ini" ..\..\ui\client\bin\windows\x64

if not exist "x32\Debug\c2sdk_d.lib" goto :Check2
if not exist "x32\Debug\c2sdk_d.dll" goto :Check2

echo ��ʼ����x32\Debug�������ļ�
copy /Y "x32\Debug\c2sdk_d.lib"    ..\lib\windows\x32
copy /Y "x32\Debug\c2sdk_d.lib"    ..\..\ui\client\lib\windows\x32
copy /Y "x32\Debug\c2sdk_d.dll"    ..\..\ui\client\bin\windows\x32
copy /Y "x32\Debug\c2sdk_d.dll"    ..\..\ui\server\bin\windows\x32
echo ��ɸ���x32\Debug�������ļ�

:Check2
if not exist "x32\Release\c2sdk.lib" goto :Check3
if not exist "x32\Release\c2sdk.dll" goto :Check3

echo ��ʼ����x32\Release�������ļ�
copy /Y "x32\Release\c2sdk.lib"    ..\lib\windows\x32
copy /Y "x32\Release\c2sdk.lib"    ..\..\ui\client\lib\windows\x32
copy /Y "x32\Release\c2sdk.dll"    ..\..\ui\client\bin\windows\x32
copy /Y "x32\Release\c2sdk.dll"    ..\..\ui\server\bin\windows\x32
echo ��ɸ����е�x32\Release�������ļ�


:Check3
if not exist "x64\Debug\c2sdk_d.lib" goto :Check4
if not exist "x64\Debug\c2sdk_d.dll" goto :Check4

echo ��ʼ����x64\Debug�������ļ�
copy /Y "x64\Debug\c2sdk_d.lib"    ..\lib\windows\x64
copy /Y "x64\Debug\c2sdk_d.lib"    ..\..\ui\client\lib\windows\x64
copy /Y "x64\Debug\c2sdk_d.dll"    ..\..\ui\client\bin\windows\x64
copy /Y "x64\Debug\c2sdk_d.dll"    ..\..\ui\server\bin\windows\x64
echo ��ɸ���x64\Debug�������ļ�

:Check4
if not exist "x64\Release\c2sdk.lib" goto :EndFile
if not exist "x64\Release\c2sdk.dll" goto :EndFile

echo ��ʼ����x64\Release�������ļ�
copy /Y "x64\Release\c2sdk.lib"    ..\lib\windows\x64
copy /Y "x64\Release\c2sdk.lib"    ..\..\ui\client\lib\windows\x64
copy /Y "x64\Release\c2sdk.dll"    ..\..\ui\client\bin\windows\x64
copy /Y "x64\Release\c2sdk.dll"    ..\..\ui\server\bin\windows\x64
echo ��ɸ����е�x64\Release�������ļ�
 
:EndFile
