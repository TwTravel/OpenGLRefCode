# Microsoft Developer Studio Project File - Name="iseecompress" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=iseecompress - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iseecompress.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iseecompress.mak" CFG="iseecompress - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iseecompress - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "iseecompress - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iseecompress - Win32 Release"

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
# ADD CPP /nologo /Zp1 /MD /W3 /GX /O2 /Ob1 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\iseecompress.lib"

!ELSEIF  "$(CFG)" == "iseecompress - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /GB /Gd /Zp1 /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ  /c
# SUBTRACT CPP /WX /Fr /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\iseecompress.lib"

!ENDIF 

# Begin Target

# Name "iseecompress - Win32 Release"
# Name "iseecompress - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\alloc.c
# End Source File
# Begin Source File

SOURCE=.\io.c
# End Source File
# Begin Source File

SOURCE=.\iseecompress.c
# End Source File
# Begin Source File

SOURCE=.\n2b_99.c
# End Source File
# Begin Source File

SOURCE=.\n2b_d.c
# End Source File
# Begin Source File

SOURCE=.\n2b_ds.c
# End Source File
# Begin Source File

SOURCE=.\n2b_to.c
# End Source File
# Begin Source File

SOURCE=.\n2d_99.c
# End Source File
# Begin Source File

SOURCE=.\n2d_d.c
# End Source File
# Begin Source File

SOURCE=.\n2d_ds.c
# End Source File
# Begin Source File

SOURCE=.\n2d_to.c
# End Source File
# Begin Source File

SOURCE=.\ucl_crc.c
# End Source File
# Begin Source File

SOURCE=.\ucl_dll.c
# End Source File
# Begin Source File

SOURCE=.\ucl_init.c
# End Source File
# Begin Source File

SOURCE=.\ucl_ptr.c
# End Source File
# Begin Source File

SOURCE=.\ucl_str.c
# End Source File
# Begin Source File

SOURCE=.\ucl_util.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\getbit.h
# End Source File
# Begin Source File

SOURCE=.\iseecompress.h
# End Source File
# Begin Source File

SOURCE=.\ucl.h
# End Source File
# Begin Source File

SOURCE=.\ucl_conf.h
# End Source File
# Begin Source File

SOURCE=.\ucl_ptr.h
# End Source File
# Begin Source File

SOURCE=.\ucl_util.h
# End Source File
# Begin Source File

SOURCE=.\uclconf.h
# End Source File
# Begin Source File

SOURCE=.\uclutil.h
# End Source File
# End Group
# End Target
# End Project
