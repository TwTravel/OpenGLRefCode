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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /I "lpng106" /I "zlib" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /D "PNG_USE_PNGVCRD" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /Zp1 /MDd /W3 /Gm /GX /ZI /Od /I "lpng106" /I "zlib" /I "..\..\..\include" /D "_DEBUG" /D "PNG_USE_PNGVCRD" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "libpng - Win32 Release"
# Name "libpng - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lpng106\png.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngerror.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngget.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngmem.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngpread.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngread.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngrio.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngrtran.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngrutil.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngset.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngtrans.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngwio.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngwrite.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngwtran.c
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngwutil.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\lpng106\png.h
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngasmrd.h
# End Source File
# Begin Source File

SOURCE=.\lpng106\pngconf.h
# End Source File
# End Group
# End Target
# End Project
