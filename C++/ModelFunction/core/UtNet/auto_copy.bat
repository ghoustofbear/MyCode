@echo off
REM -- ������

echo �Զ������ļ�.lib/.dll��Ŀ��λ��
echo ��ע���޸�auto_copy.bat�ļ��е�Ŀ��·��

if not exist "Debug\UtNetD.lib" goto :Check2
if not exist "Debug\UtNetD.dll" goto :Check2

echo ��ʼ����Debug�������ļ�
copy /Y "ut_net.h"          ..\..\include
copy /Y "ut_net.h"          ..\..\..\ui\client\include
copy /Y "Debug\UtNetD.lib"  ..\..\lib
copy /Y "Debug\UtNetD.lib"  ..\..\..\ui\client\win32\lib
copy /Y "Debug\UtNetD.dll"  ..\..\..\ui\client\win32\bin
copy /Y "Debug\UtNetD.dll"  ..\..\..\ui\server\win32\bin
echo ��ɸ���Debug�������ļ�

:Check2
if not exist "Release\UtNet.lib" goto :EndFile
if not exist "Release\UtNet.dll" goto :EndFile

echo ��ʼ����Release�������ļ�
copy /Y "ut_net.h"           ..\..\include
copy /Y "ut_net.h"           ..\..\..\ui\client\include
copy /Y "Release\UtNet.lib"  ..\..\lib
copy /Y "Release\UtNet.lib"  ..\..\..\ui\client\win32\lib
copy /Y "Release\UtNet.dll"  ..\..\..\ui\client\win32\bin
copy /Y "Release\UtNet.dll"  ..\..\..\ui\server\win32\bin
echo ��ɸ����е�Release�������ļ�

:EndFile


