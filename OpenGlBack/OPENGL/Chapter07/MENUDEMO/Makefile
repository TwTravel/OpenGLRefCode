##!nmake

!include <win32.mak>

OPENGL = glu32.lib opengl32.lib

CFILES  = menudemo2.c
TARGETS	= $(CFILES:.c=.exe)
LCFLAGS	= $(cflags) -DWIN32
LLDLIBS	= $(lflags) $(OPENGL) $(guilibs)

default	: $(TARGETS)

$(TARGETS): $*.obj
        $(link) -out:$@ $** $(LLDLIBS)

.c.obj	: 
	$(CC) $(LCFLAGS) $<

# dependencies (must come AFTER inference rules)
menudemo2.exe	: menu.obj texture.obj mload.obj

menu.obj: menu.c
	$(CC) $(LCFLAGS) menu.c
texture.obj	: texture.c
	$(CC) $(LCFLAGS) texture.c
mload.obj: mload.c
	$(CC) $(LCFLAGS) mload.c
