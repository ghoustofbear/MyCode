# Microsoft Developer Studio Project File - Name="UtNet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=UtNet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UtNet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UtNet.mak" CFG="UtNet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UtNet - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "UtNet - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UtNet - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTNET_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /ZI /I "../../common/" /I "../../include/" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NET_EXPORTS" /D "CONFIG_DLL" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Winmm.lib Mux_AVFile.lib /nologo /dll /machine:I386 /libpath:"../../lib/"
# SUBTRACT LINK32 /map /debug
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=auto_copy.bat
# End Special Build Tool

!ELSEIF  "$(CFG)" == "UtNet - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "UTNET_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../common/" /I "../../include/" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NET_EXPORTS" /D "CONFIG_DLL" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib Winmm.lib Mux_AVFile.lib /nologo /dll /debug /machine:I386 /out:"Debug/UtNetD.dll" /pdbtype:sept /libpath:"../../lib/"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=auto_copy.bat
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "UtNet - Win32 Release"
# Name "UtNet - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\audio_helmet_gx.cpp
# End Source File
# Begin Source File

SOURCE=.\audio_record.cpp
# End Source File
# Begin Source File

SOURCE=.\c2platform_conn.cpp
# End Source File
# Begin Source File

SOURCE=.\c2platform_devTalk.cpp
# End Source File
# Begin Source File

SOURCE=.\c2platform_download.cpp
# End Source File
# Begin Source File

SOURCE=.\c2Platform_DownloadByTime.cpp
# End Source File
# Begin Source File

SOURCE=.\c2Platform_Dualtalk.cpp
# End Source File
# Begin Source File

SOURCE=.\c2Platform_DualtalkEx.cpp
# End Source File
# Begin Source File

SOURCE=.\c2platform_pcTalk.cpp
# End Source File
# Begin Source File

SOURCE=.\c2platform_preview.cpp
# End Source File
# Begin Source File

SOURCE=.\c2platform_record.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\common.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\concourrency.cpp
# End Source File
# Begin Source File

SOURCE=.\connection.cpp
# End Source File
# Begin Source File

SOURCE=.\download.cpp
# End Source File
# Begin Source File

SOURCE=.\download_bytime.cpp
# End Source File
# Begin Source File

SOURCE=.\dualtalk.cpp
# End Source File
# Begin Source File

SOURCE=.\dualtalkEx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\fs_rw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\g711.cpp
# End Source File
# Begin Source File

SOURCE=.\helmet_interface_player.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_alarm.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_conn.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_download.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_download_bytime.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_dualtalk.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_dualtalkEx.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_preview.cpp
# End Source File
# Begin Source File

SOURCE=.\hikplatform_record.cpp
# End Source File
# Begin Source File

SOURCE=.\net_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\pc_talk.cpp
# End Source File
# Begin Source File

SOURCE=.\playback.cpp
# End Source File
# Begin Source File

SOURCE=.\preview.cpp
# End Source File
# Begin Source File

SOURCE=.\socket.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\ut_net.cpp
# End Source File
# Begin Source File

SOURCE=.\ut_netserver.cpp
# End Source File
# Begin Source File

SOURCE=.\UtNet.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\audio_helmet_gx.h
# End Source File
# Begin Source File

SOURCE=.\audio_record.h
# End Source File
# Begin Source File

SOURCE=.\basedownload.h
# End Source File
# Begin Source File

SOURCE=.\basedownloadbytime.h
# End Source File
# Begin Source File

SOURCE=.\basedualtalk.h
# End Source File
# Begin Source File

SOURCE=.\basedualtalkEx.h
# End Source File
# Begin Source File

SOURCE=.\baseplatfrom_conn.h
# End Source File
# Begin Source File

SOURCE=.\basepview.h
# End Source File
# Begin Source File

SOURCE=.\baserecord.h
# End Source File
# Begin Source File

SOURCE=..\..\common\bitoperation.h
# End Source File
# Begin Source File

SOURCE=.\c2platform_conn.h
# End Source File
# Begin Source File

SOURCE=.\c2platform_devTalk.h
# End Source File
# Begin Source File

SOURCE=.\c2platform_download.h
# End Source File
# Begin Source File

SOURCE=.\c2Platform_DownloadByTime.h
# End Source File
# Begin Source File

SOURCE=.\c2Platform_Dualtalk.h
# End Source File
# Begin Source File

SOURCE=.\c2Platform_DualtalkEx.h
# End Source File
# Begin Source File

SOURCE=.\c2platform_pcTalk.h
# End Source File
# Begin Source File

SOURCE=.\c2platform_preview.h
# End Source File
# Begin Source File

SOURCE=.\c2platform_record.h
# End Source File
# Begin Source File

SOURCE=..\..\common\common.h
# End Source File
# Begin Source File

SOURCE=..\..\common\concourrency.h
# End Source File
# Begin Source File

SOURCE=.\connection.h
# End Source File
# Begin Source File

SOURCE=.\download.h
# End Source File
# Begin Source File

SOURCE=.\download_bytime.h
# End Source File
# Begin Source File

SOURCE=.\dualtalk.h
# End Source File
# Begin Source File

SOURCE=.\dualtalkEx.h
# End Source File
# Begin Source File

SOURCE=..\..\common\fs_rw.h
# End Source File
# Begin Source File

SOURCE=..\..\common\g711.h
# End Source File
# Begin Source File

SOURCE=.\helmet_interface_player.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_alarm.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_conn.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_download.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_download_bytime.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_dualtalk.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_preview.h
# End Source File
# Begin Source File

SOURCE=.\hikplatform_record.h
# End Source File
# Begin Source File

SOURCE=.\net_msg.h
# End Source File
# Begin Source File

SOURCE=.\net_param.h
# End Source File
# Begin Source File

SOURCE=.\pc_talk.h
# End Source File
# Begin Source File

SOURCE=.\playback.h
# End Source File
# Begin Source File

SOURCE=.\preview.h
# End Source File
# Begin Source File

SOURCE=.\socket.h
# End Source File
# Begin Source File

SOURCE=..\..\common\stream.h
# End Source File
# Begin Source File

SOURCE=..\..\include\tiandy_play.h
# End Source File
# Begin Source File

SOURCE=.\ut_net.h
# End Source File
# Begin Source File

SOURCE=.\ut_netserver.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
