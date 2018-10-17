# Microsoft Developer Studio Generated NMAKE File, Format Version 40001
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=matlight - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to matlight - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "matlight - Win32 Release" && "$(CFG)" !=\
 "matlight - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "matlight.mak" CFG="matlight - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "matlight - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "matlight - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "matlight - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "matlight - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\matlight.exe"

CLEAN : 
	-@erase ".\Release\matlight.exe"
	-@erase ".\Release\LightEditor.obj"
	-@erase ".\Release\matlight.pch"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\matlightDoc.obj"
	-@erase ".\Release\matlight.obj"
	-@erase ".\Release\ShineEdit.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\matlightView.obj"
	-@erase ".\Release\MaterialPage.obj"
	-@erase ".\Release\Editor.obj"
	-@erase ".\Release\LightPage.obj"
	-@erase ".\Release\MatEditor.obj"
	-@erase ".\Release\matlight.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/matlight.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/matlight.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/matlight.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/matlight.pdb" /machine:I386 /out:"$(OUTDIR)/matlight.exe" 
LINK32_OBJS= \
	"$(INTDIR)/LightEditor.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/matlightDoc.obj" \
	"$(INTDIR)/matlight.obj" \
	"$(INTDIR)/ShineEdit.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/matlightView.obj" \
	"$(INTDIR)/MaterialPage.obj" \
	"$(INTDIR)/Editor.obj" \
	"$(INTDIR)/LightPage.obj" \
	"$(INTDIR)/MatEditor.obj" \
	"$(INTDIR)/matlight.res"

"$(OUTDIR)\matlight.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "matlight - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\matlight.exe" "$(OUTDIR)\matlight.pch"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\matlight.pch"
	-@erase ".\Debug\matlight.exe"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\LightPage.obj"
	-@erase ".\Debug\Editor.obj"
	-@erase ".\Debug\LightEditor.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\ShineEdit.obj"
	-@erase ".\Debug\matlightDoc.obj"
	-@erase ".\Debug\MatEditor.obj"
	-@erase ".\Debug\matlightView.obj"
	-@erase ".\Debug\matlight.obj"
	-@erase ".\Debug\MaterialPage.obj"
	-@erase ".\Debug\matlight.res"
	-@erase ".\Debug\matlight.ilk"
	-@erase ".\Debug\matlight.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /c
# SUBTRACT CPP /Gy
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/matlight.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/matlight.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/matlight.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 opengl32.lib glu32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=opengl32.lib glu32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/matlight.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/matlight.exe" 
LINK32_OBJS= \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/LightPage.obj" \
	"$(INTDIR)/Editor.obj" \
	"$(INTDIR)/LightEditor.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/ShineEdit.obj" \
	"$(INTDIR)/matlightDoc.obj" \
	"$(INTDIR)/MatEditor.obj" \
	"$(INTDIR)/matlightView.obj" \
	"$(INTDIR)/matlight.obj" \
	"$(INTDIR)/MaterialPage.obj" \
	"$(INTDIR)/matlight.res"

"$(OUTDIR)\matlight.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "matlight - Win32 Release"
# Name "matlight - Win32 Debug"

!IF  "$(CFG)" == "matlight - Win32 Release"

!ELSEIF  "$(CFG)" == "matlight - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "matlight - Win32 Release"

!ELSEIF  "$(CFG)" == "matlight - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\matlight.cpp
DEP_CPP_MATLI=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\MainFrm.h"\
	".\matlightDoc.h"\
	".\matlightView.h"\
	

"$(INTDIR)\matlight.obj" : $(SOURCE) $(DEP_CPP_MATLI) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "matlight - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/matlight.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\matlight.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "matlight - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)/matlight.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\matlight.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\MainFrm.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\matlightDoc.cpp
DEP_CPP_MATLIG=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\matlightDoc.h"\
	".\matlightView.h"\
	".\LightEditor.h"\
	".\MatEditor.h"\
	".\MainFrm.h"\
	".\Editor.h"\
	".\ShineEdit.h"\
	

"$(INTDIR)\matlightDoc.obj" : $(SOURCE) $(DEP_CPP_MATLIG) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\matlightView.cpp

!IF  "$(CFG)" == "matlight - Win32 Release"

DEP_CPP_MATLIGH=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\matlightDoc.h"\
	".\matlightView.h"\
	
NODEP_CPP_MATLIGH=\
	".\m_GLPalette"\
	

"$(INTDIR)\matlightView.obj" : $(SOURCE) $(DEP_CPP_MATLIGH) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


!ELSEIF  "$(CFG)" == "matlight - Win32 Debug"

DEP_CPP_MATLIGH=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\matlightDoc.h"\
	".\matlightView.h"\
	

"$(INTDIR)\matlightView.obj" : $(SOURCE) $(DEP_CPP_MATLIGH) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\matlight.rc
DEP_RSC_MATLIGHT=\
	".\res\matlight.ico"\
	".\res\matlightDoc.ico"\
	".\res\Toolbar.bmp"\
	".\res\matlight.rc2"\
	

"$(INTDIR)\matlight.res" : $(SOURCE) $(DEP_RSC_MATLIGHT) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\LightPage.cpp
DEP_CPP_LIGHT=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\LightPage.h"\
	

"$(INTDIR)\LightPage.obj" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MaterialPage.cpp
DEP_CPP_MATER=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\MaterialPage.h"\
	

"$(INTDIR)\MaterialPage.obj" : $(SOURCE) $(DEP_CPP_MATER) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Editor.cpp
DEP_CPP_EDITO=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\Editor.h"\
	

"$(INTDIR)\Editor.obj" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\MatEditor.cpp
DEP_CPP_MATED=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\MatEditor.h"\
	".\ShineEdit.h"\
	".\matlightDoc.h"\
	".\Editor.h"\
	

"$(INTDIR)\MatEditor.obj" : $(SOURCE) $(DEP_CPP_MATED) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\LightEditor.cpp
DEP_CPP_LIGHTE=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\MainFrm.h"\
	".\LightEditor.h"\
	".\matlightDoc.h"\
	".\Editor.h"\
	

"$(INTDIR)\LightEditor.obj" : $(SOURCE) $(DEP_CPP_LIGHTE) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ShineEdit.cpp
DEP_CPP_SHINE=\
	".\StdAfx.h"\
	".\matlight.h"\
	".\ShineEdit.h"\
	

"$(INTDIR)\ShineEdit.obj" : $(SOURCE) $(DEP_CPP_SHINE) "$(INTDIR)"\
 "$(INTDIR)\matlight.pch"


# End Source File
# End Target
# End Project
################################################################################
