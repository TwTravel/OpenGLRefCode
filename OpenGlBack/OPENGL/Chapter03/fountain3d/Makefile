##!nmake

!include <win32.mak>

OPENGL = glut32.lib glu32.lib opengl32.lib

CFILES  = fountain3d.c
TARGETS	= $(CFILES:.c=.exe)
LCFLAGS	= $(cflags) -DWIN32
LLDLIBS	= $(lflags) $(OPENGL) $(guilibs)

default	: $(TARGETS)

$(TARGETS): $*.obj
        $(link) -out:$@ $** $(LLDLIBS)

.c.obj	: 
	$(CC) $(LCFLAGS) $<

# dependencies (must come AFTER inference rules)
fountain3d.exe	: texture.obj

texture.obj	: texture.c
	$(CC) $(LCFLAGS) texture.c
