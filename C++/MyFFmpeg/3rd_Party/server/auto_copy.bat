@echo off
REM -- ������

echo �Զ������ļ�.lib/.dll��Ŀ��λ��
echo ��ע���޸�auto_copy.bat�ļ��е�Ŀ��·��

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

echo ��ʼ����x32\Debug�������ļ�
copy /Y "x32\Debug\c2msv_d.lib"     ..\lib\windows\x32
copy /Y "x32\Debug\c2msv_d.lib"     ..\..\ui\server\lib\windows\x32
copy /Y "x32\Debug\c2msv_d.dll"     ..\..\ui\server\bin\windows\x32
echo ��ɸ���x32\Debug�������ļ�

:Check2
if not exist "x32\Release\c2msv.lib" goto :Check3
if not exist "x32\Release\c2msv.dll" goto :Check3

echo ��ʼ����x32\Release�������ļ�
copy /Y "x32\Release\c2msv.lib"     ..\lib\windows\x32
copy /Y "x32\Release\c2msv.lib"     ..\..\ui\server\lib\windows\x32
copy /Y "x32\Release\c2msv.dll"     ..\..\ui\server\bin\windows\x32
echo ��ɸ����е�x32\Release�������ļ�


:Check3
if not exist "x64\Debug\c2msv_d.lib" goto :Check4
if not exist "x64\Debug\c2msv_d.dll" goto :Check4

echo ��ʼ����x64\Debug�������ļ�
copy /Y "x64\Debug\c2msv_d.lib"  ..\lib\windows\x64
copy /Y "x64\Debug\c2msv_d.lib"  ..\..\ui\server\lib\windows\x64
copy /Y "x64\Debug\c2msv_d.dll"  ..\..\ui\server\bin\windows\x64
echo ��ɸ���x64\Debug�������ļ�

:Check4
if not exist "x64\Release\c2msv.lib" goto :EndFile
if not exist "x64\Release\c2msv.dll" goto :EndFile

echo ��ʼ����x64\Release�������ļ�
copy /Y "x64\Release\c2msv.lib"     ..\lib\windows\x64
copy /Y "x64\Release\c2msv.lib"     ..\..\ui\server\lib\windows\x64
copy /Y "x64\Release\c2msv.dll"     ..\..\ui\server\bin\windows\x64
echo ��ɸ����е�x64\Release�������ļ�

:EndFile
