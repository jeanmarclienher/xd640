# Microsoft Developer Studio Project File - Name="libpng" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=libpng - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libpng.mak" CFG="libpng - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libpng - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "libpng - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libpng - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /GX /Ot /Op /Ob2 /I "." /I ".." /I "..\zlib" /D "NDEBUG" /D "FL_STATIC" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpng.lib"

!ELSEIF  "$(CFG)" == "libpng - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /GX /Z7 /Od /I "." /I ".." /I "..\zlib" /D "_DEBUG" /D "FL_STATIC" /D "WIN32" /D "_WINDOWS" /D "WIN32_LEAN_AND_MEAN" /D "VC_EXTRA_LEAN" /D "WIN32_EXTRA_LEAN" /FR /YX /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\libpngd.lib"

!ENDIF 

# Begin Target

# Name "libpng - Win32 Release"
# Name "libpng - Win32 Debug"
# Begin Source File

SOURCE=..\png\png.c
# End Source File
# Begin Source File

SOURCE=..\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\png\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=..\png\pngget.c
# End Source File
# Begin Source File

SOURCE=..\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\png\pngread.c
# End Source File
# Begin Source File

SOURCE=..\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\png\pngset.c
# End Source File
# Begin Source File

SOURCE=..\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\png\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwutil.c
# End Source File
# End Target
# End Project
