/*
 * Flight simulator example from Chapter 16.
 *
 * Written by Michael Sweet
 */

#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#ifndef WIN32
#  include <sys/time.h>
#endif /* !WIN32 */
#ifndef M_PI
#  define M_PI	(double)3.14159265358979323846
#endif /* !M_PI */
#include "texture.h"
#include "font.h"


/*
 * Constants:
 */

#define TERRAIN_COUNT   41     /* Number of postings (must be odd) */
#define TERRAIN_SIZE    4000.0 /* Size of terrain database */
#define TERRAIN_VIEW    1000.0 /* Viewable distance in terrain database */
#define TERRAIN_SPACING 100.0  /* Spacing between postings */


/*
 * Terrain posting structure...
 */

typedef struct
    {
    GLfloat v[3]; /* Position */
    GLfloat n[3]; /* Lighting normal */
    } TP;


/*
 * Globals...
 */

int        RedrawAll;            /* 1 = redraw everything, 0 = just terrain */
int        RedrawAirspeed;       /* 1 = redraw the airspeed indicator */
int        RedrawAltimeter;      /* 1 = redraw the altimeter */
int        RedrawCompass;        /* 1 = redraw the compass */
int        RedrawHorizon;        /* 1 = redraw the artificial horizon */

int        Width;                /* Width of window */
int        Height;               /* Height of window */
double     LastTime;             /* Last update time */
int	   MouseStartX;          /* Initial mouse X position */
int        MouseStartY;          /* Initial mouse Y position */
int	   MouseX;               /* Mouse X position */
int        MouseY;               /* Mouse Y position */
int        ViewAngle = 0;        /* Viewing angle */
GLenum     PolyMode = GL_FILL;   /* Polygon drawing mode */
int        UseTexturing = 1;     /* Use texturing? */
int        ShowLighting = 1;     /* Show lighting? */
int        ShowSky = 1;          /* Show sky? */
int        ShowTerrain = 1;      /* Show 3D terrain? */
int        ShowWater = 1;        /* Show water? */
GLfloat    Velocity = 10.0;      /* Flying speed */
GLfloat	   Position[3] =         /* Position of viewer */
    {
    0.0, 10.0, 0.0
    };
GLfloat	   Orientation[3] =      /* Orientation of viewer */
    {
    0.0, 0.0, 0.0
    };
TP         Terrain[TERRAIN_COUNT][TERRAIN_COUNT];
                                 /* Terrain postings */
GLuint     LandTexture;          /* Land texture object */
GLuint     SkyTexture;           /* Sky texture object */

GLFONT     *TextFont;            /* Font to use for text */

int        FPS = 0,              /* Frames per second value */
           FPSCount = 0;         /* Frames per second count */
double     FPSTime = 0.0;        /* Frames per second time */
GLfloat    Airspeed = 10.0;      /* Airspeed indicator */
GLfloat    Altimeter = 0.0;      /* Altimeter reading */
GLfloat    Horizon[2] =          /* Artificial horizon position */
    {
    0.0, 0.0
    };
GLfloat    Compass = 0.0;        /* Compass position */

int        UseSwapHint;          /* Use swap hint extension? */
PROC       glAddSwapHintRectWIN; /* Swap hint extension function */

/*
 * Functions...
 */

void   BuildTerrain(void);
void   DivideTerrain(int left, int right, int bottom, int top);
double GetClock(void);
void   Idle(void);
void   Joystick(unsigned state, int x, int y, int z);
void   Keyboard(unsigned char key, int x, int y);
void   Motion(int x, int y);
void   Mouse(int button, int state, int x, int y);
void   Redraw(void);
void   Resize(int width, int height);
void   Special(int key, int x, int y);


/*
 * 'main()' - Open a window and display a sphere and cube.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Flight Simulator");

    glutDisplayFunc(Redraw);
    if (glutDeviceGet(GLUT_HAS_JOYSTICK))
        glutJoystickFunc(Joystick, 200);
    glutKeyboardFunc(Keyboard);
    glutMotionFunc(Motion);
    glutMouseFunc(Mouse);
    glutReshapeFunc(Resize);
    glutSpecialFunc(Special);

    TextFont = FontCreate(wglGetCurrentDC(), "Arial", 16, 0, 0);

    LandTexture = TextureLoad("land.bmp", GL_FALSE, GL_LINEAR_MIPMAP_LINEAR,
                              GL_LINEAR, GL_REPEAT);
    SkyTexture  = TextureLoad("sky.bmp", GL_FALSE, GL_LINEAR, GL_LINEAR,
                              GL_CLAMP);

    BuildTerrain();

    puts("QUICK HELP:");
    puts("");
    puts("ESC - Quit");
    puts("',' - Slow down, '<' - Slowest");
    puts("'.' - Speed up, '>' - Fastest");
    puts("'3' - Toggle terrain");
    puts("'l' - Toggle lighting");
    puts("'s' - Toggle sky/clouds");
    puts("'t' - Toggle texturing");
    puts("'w' - Toggle water");
    puts("'W' - Toggle wireframe");

    printf("GL_EXTENSIONS = %s\n", glGetString(GL_EXTENSIONS));

    if (strstr(glGetString(GL_EXTENSIONS), "WIN_swap_hint") != NULL)
        {
        UseSwapHint = 1;
	glAddSwapHintRectWIN = wglGetProcAddress("glAddSwapHintRectWIN");
	}
    else
        UseSwapHint = 0;

    RedrawAll = 1;

    glutMainLoop();
    return (0);
    }


/*
 * 'BuildTerrain()' - Build a fractal landscape...
 */

void
BuildTerrain(void)
    {
    int     i, j;           /* Looping vars */
    TP      *tp;            /* Terrain posting */
    GLfloat nx, ny, nz, nw; /* Normal components */

    for (i = 0, tp = Terrain[0]; i < TERRAIN_COUNT; i ++)
        for (j = 0; j < TERRAIN_COUNT; j ++, tp ++)
	    {
	    tp->v[0] = TERRAIN_SPACING * i - 0.5 * TERRAIN_SIZE;
	    tp->v[1] = 2.0;
	    tp->v[2] = 0.5 * TERRAIN_SIZE - TERRAIN_SPACING * j;
	    }

    DivideTerrain(0, TERRAIN_COUNT - 1, 0, TERRAIN_COUNT - 1);

    /*
     * Loop through the terrain arrays and regenerate the
     * lighting normals based on the terrain height.
     */
    for (i = 0, tp = Terrain[0]; i < (TERRAIN_COUNT - 1); i ++, tp ++)
        for (j = 0; j < (TERRAIN_COUNT - 1); j ++, tp ++)
            {
            /*
             * Compute the cross product of the vectors above and to
             * the right (simplified for this special case).
             */

            nx = tp[0].v[1] - tp[1].v[1];
            ny = -1.0;
            nz = tp[0].v[1] - tp[TERRAIN_COUNT].v[1];
            nw = -1.0 / sqrt(nx * nx + ny * ny + nz * nz);

            /* Normalize the normal vector and store it... */
            tp->n[0] = nx * nw;
            tp->n[1] = ny * nw;
            tp->n[2] = nz * nw;
            }

    /*
     * Set the top and right normals to be the same as the
     * second-to-last normals.
     */

    for (i = 0, tp = Terrain[TERRAIN_COUNT - 2];
         i < TERRAIN_COUNT;
	 i ++, tp ++)
        {
	  tp[TERRAIN_COUNT].n[0] = tp[0].n[0];
	  tp[TERRAIN_COUNT].n[1] = tp[0].n[1];
	  tp[TERRAIN_COUNT].n[2] = tp[0].n[2];
	}

    for (i = 0, tp = Terrain[0] + TERRAIN_COUNT - 2;
         i < (TERRAIN_COUNT - 1);
	 i ++, tp += TERRAIN_COUNT)
        {
	  tp[1].n[0] = tp[0].n[0];
	  tp[1].n[1] = tp[0].n[1];
	  tp[1].n[2] = tp[0].n[2];
	}

    /* Position the viewer just above the terrain */
    Position[1] = Terrain[TERRAIN_COUNT / 2][TERRAIN_COUNT / 2].v[1] + 10.0;
    }


/*
 * 'DivideTerrain()' - Subdivide terrain until all of the landscape is done.
 */

void
DivideTerrain(int left,   /* I - Left posting */
              int right,  /* I - Right posting */
	      int bottom, /* I - Bottom posting */
	      int top)    /* I - Top posting */
    {
    int x, y;             /* Middle column and row */
    int halfel, maxel;    /* Maximum amount of change for middle */

    x      = (left + right) / 2;
    y      = (bottom + top) / 2;
    halfel = (rand() % 50) + 50;
    maxel  = 2 * halfel + 1;

    Terrain[x][bottom].v[1] = 0.5 * (Terrain[left][bottom].v[1] +
                                     Terrain[right][bottom].v[1]) +
                              ((rand() % maxel) - halfel) * 0.2;

    Terrain[x][top].v[1]    = 0.5 * (Terrain[left][top].v[1] +
                                     Terrain[right][top].v[1]) +
                              ((rand() % maxel) - halfel) * 0.2;

    Terrain[left][y].v[1]   = 0.5 * (Terrain[left][bottom].v[1] +
                                     Terrain[left][top].v[1]) +
                              ((rand() % maxel) - halfel) * 0.2;

    Terrain[right][y].v[1]  = 0.5 * (Terrain[right][bottom].v[1] +
                                     Terrain[right][top].v[1]) +
                              ((rand() % maxel) - halfel) * 0.2;

    if (x == (TERRAIN_COUNT / 2) && y == (TERRAIN_COUNT / 2))
        Terrain[x][y].v[1] = 0.25 * (Terrain[left][bottom].v[1] +
                                     Terrain[right][bottom].v[1] +
	    			     Terrain[left][top].v[1] +
                                     Terrain[right][top].v[1]) +
                             ((rand() % maxel) - halfel) * 0.5;
    else
        Terrain[x][y].v[1] = 0.25 * (Terrain[left][bottom].v[1] +
                                     Terrain[right][bottom].v[1] +
	    			     Terrain[left][top].v[1] +
                                     Terrain[right][top].v[1]) +
                             ((rand() % maxel) - halfel) * 0.2;

    if ((x - left) > 1)
        {
	DivideTerrain(left, x, bottom, y);
	DivideTerrain(left, x, y, top);
	DivideTerrain(x, right, bottom, y);
	DivideTerrain(x, right, y, top);
	}
    }


/*
 * 'GetClock()' - Return an increasing clock time in seconds...
 */

double /* O - Time in seconds */
GetClock(void)
    {
#ifdef WIN32
    SYSTEMTIME t;      /* Current time of day */

    GetSystemTime(&t);
    return (((t.wHour * 60.0) + t.wMinute) * 60 + t.wSecond +
            t.wMilliseconds * 0.001);
#else /* UNIX */
    struct timeval t; /* Current time of day */

    gettimeofday(&t, NULL);
    return ((double)t.tv_sec + 0.000001 * (double)t.tv_usec);
#endif /* WIN32 */
    }


/*
 * 'Idle()' - Move the viewer and redraw...
 */

void
Idle(void)
    {
    int     i, j;         /* Column and row in terrain */
    GLfloat movex, movey; /* Scaled mouse movement */
    double  curtime;      /* Current time in milliseconds */
    GLfloat distance;     /* Distance to move */
    GLfloat cheading;     /* Cosine of heading */
    GLfloat sheading;     /* Sine of heading */
    GLfloat cpitch;       /* Cosine of pitch */
    GLfloat spitch;       /* Sine of pitch */

    /* Get the current system time to figure out how far to move. */
    curtime  = GetClock();
    distance = curtime - LastTime;
    LastTime = curtime;

    /*
     * See how far the mouse pointer is from the 'center' (click)
     * position.
     */

    movex = 0.01 * sqrt(Velocity) * (MouseX - MouseStartX);
    movey = 0.01 * sqrt(Velocity) * (MouseY - MouseStartY);

    /*
     * Adjust roll, pitch, and heading according to the current
     * mouse inputs and orientation.
     */

    Orientation[0] += distance * movey * cos(Orientation[2] * M_PI / 180.0);
    Orientation[1] += distance * movey * sin(Orientation[2] * M_PI / 180.0);
    Orientation[2] += distance * movex;

    /* Move based upon the current orientation... */
    cheading = cos(Orientation[1] * M_PI / 180.0);
    sheading = sin(Orientation[1] * M_PI / 180.0);
    cpitch   = cos(Orientation[0] * M_PI / 180.0);
    spitch   = sin(Orientation[0] * M_PI / 180.0);

    Position[0] += Velocity * distance * sheading * cpitch;
    Position[1] += Velocity * distance * spitch;
    Position[2] -= Velocity * distance * cheading * cpitch;

    /* Limit the viewer to the terrain... */
    if (Position[0] < (-0.45 * TERRAIN_SIZE))
        {
	Position[0]    = -0.45 * TERRAIN_SIZE;
	Orientation[1] += 10.0 * distance;
	}

    if (Position[0] > (0.45 * TERRAIN_SIZE))
        {
	Position[0]    = 0.45 * TERRAIN_SIZE;
	Orientation[1] += 10.0 * distance;
	}

    if (Position[2] < (-0.45 * TERRAIN_SIZE))
        {
	Position[2]    = -0.45 * TERRAIN_SIZE;
	Orientation[1] += 10.0 * distance;
	}

    if (Position[2] > (0.45 * TERRAIN_SIZE))
        {
	Position[2]    = 0.45 * TERRAIN_SIZE;
	Orientation[1] += 10.0 * distance;
	}

    i = (Position[0] + 0.5 * TERRAIN_SIZE) / TERRAIN_SPACING + 0.5;
    j = (0.5 * TERRAIN_SIZE - Position[2]) / TERRAIN_SPACING + 0.5;

    if (Position[1] < (5.0 + Terrain[i][j].v[1]))
        {
	Position[1]    = 5.0 + Terrain[i][j].v[1];
	Orientation[0] += 5.0 * distance;
	}

    if (Position[1] > (0.2 * TERRAIN_SIZE))
        {
	Position[1]    = 0.2 * TERRAIN_SIZE;
	Orientation[0] -= 5.0 * distance;
	}

    if (Orientation[0] < -90.0)
        {
        Orientation[0] = -180.0 - Orientation[0];
	Orientation[2] += 180.0;
	}
    else if (Orientation[0] > 90.0)
        {
        Orientation[0] = 180.0 - Orientation[0];
	Orientation[2] -= 180.0;
	}

    if (Orientation[1] < 0.0)
        Orientation[1] += 360.0;
    else if (Orientation[1] > 360.0)
        Orientation[1] -= 360.0;

    if (Orientation[2] < -180.0)
        Orientation[2] += 360.0;
    else if (Orientation[2] > 180.0)
        Orientation[2] -= 360.0;

    /* Update instruments as needed... */
    if (fabs(Orientation[0] - Horizon[0]) > 1.0 ||
        fabs(Orientation[2] - Horizon[1]) > 1.0)
        RedrawHorizon = 1;

    if (fabs(Orientation[1] - Compass) > 1.0 &&
        fabs(Orientation[1] - Compass) < 359.0)
	RedrawCompass = 1;

    if (Position[1] != Altimeter)
        RedrawAltimeter = 1;

    /* Update the frames-per-second value */
    FPSTime += distance;
    FPSCount ++;
    if (FPSCount >= 20)
        {
	FPS       = FPSCount / FPSTime + 0.5;
	FPSTime   = 0.0;
	FPSCount  = 0;
	}

    RedrawAll = -1;
    Redraw();
    }


/*
 * 'Joystick()' - Handle joystick movement.
 */

void
Joystick(unsigned state,       /* I - Button state */
         int      x,           /* I - X position (-1000 to 1000) */
 	 int      y,           /* I - Y position (-1000 to 1000) */
	 int      z)           /* I - Z position (-1000 to 1000) */
    {
    float      new_velocity;   /* New velocity value */
    static int last_state = 0; /* Last button state */

    if (last_state != state)
        {
	/* Button changed state; see what the new state is... */
	if (state && !last_state)
            {
    	    /* Start flying */
            MouseStartX = MouseX = x / 2;
	    MouseStartY = MouseY = y / 2;
	    LastTime    = GetClock();
            glutIdleFunc(Idle);
	    }
            else if (!state && last_state)
            {
	    /* Stop flying */
            glutIdleFunc((void (*)(void))0);
	    }

	last_state = state;
	}

    /* Update the joystick/mouse position */
    if (state)
        {
	MouseX = x / 2;
        MouseY = y / 2;
	}

    if (z > -999)
        Velocity = (999 - z) * 0.045 + 10.0;

    if (fabs(Velocity - Airspeed) >= 1.0)
	RedrawAirspeed = 1;
    }

/*
 * 'Keyboard()' - Handle key presses...
 */

void
Keyboard(unsigned char key, /* I - Key that was pressed */
         int           x,   /* I - Mouse X position */
	 int           y)   /* I - Mouse Y position */
    {
    switch (key)
        {
	case 0x1b :
	    puts("");
	    exit(0);
	    break;
	case ',' :
	    if (Velocity > 5.0)
	        {
		RedrawAirspeed = 1;
	        Velocity       -= 5.0;
		}
	    break;
	case '.' :
	    if (Velocity < 100.0)
	        {
		RedrawAirspeed = 1;
	        Velocity       += 5.0;
		}
	    break;
	case '<' :
	    RedrawAirspeed = 1;
	    Velocity       = 10.0;
	    break;
	case '>' :
	    RedrawAirspeed = 1;
	    Velocity       = 100.0;
	    break;
	case '3' :
	    ShowTerrain = !ShowTerrain;
	    break;
	case 'l' :
	    ShowLighting = !ShowLighting;
	    break;
	case 's' :
	    ShowSky = !ShowSky;
	    break;
	case 't' :
	    UseTexturing = !UseTexturing;
	    break;
	case 'w' :
	    ShowWater = !ShowWater;
	    break;
	case 'W' :
	    if (PolyMode == GL_FILL)
	        PolyMode = GL_LINE;
	    else
	        PolyMode = GL_FILL;
	    break;
	}

    glutPostRedisplay();
    }


/*
 * 'Motion()' - Handle mouse pointer motion.
 */

void
Motion(int x, /* I - Current mouse X position */
       int y) /* I - Current mouse Y position */
    {
    MouseX = x;
    MouseY = y;
    }


/*
 * 'Mouse()' - Handle mouse button events.
 */

void
Mouse(int button, /* I - Button that changed */
      int state,  /* I - Current button states */
      int x,      /* I - Current mouse X position */
      int y)      /* I - Current mouse Y position */
    {
    if (state == GLUT_DOWN)
        {
	/* Start flying */
	MouseStartX = MouseX = x;
	MouseStartY = MouseY = y;
	LastTime    = GetClock();
        glutIdleFunc(Idle);
	}
    else
        {
	/* Stop flying */
        glutIdleFunc((void (*)(void))0);
	}
    }


/*
 * 'draw_instrument()' - Draws an instrument frame.
 */

void
draw_instrument(int ix, int iy, int isize)
    {
    float	theta; /* Angle for circle */

    glColor3f(0.5, 0.5, 0.525);
    glBegin(GL_QUADS);
    glVertex2i(ix, iy);
    glVertex2i(ix + isize - 1, iy);
    glVertex2i(ix + isize - 1, iy + isize - 1);
    glVertex2i(ix, iy + isize - 1);
    glEnd();

    glColor3f(0.75, 0.75, 0.788);
    glBegin(GL_LINE_STRIP);
    glVertex2i(ix, iy);
    glVertex2i(ix, iy + isize - 1);
    glVertex2i(ix + isize - 1, iy + isize - 1);
    glEnd();

    glColor3f(0.25, 0.25, 0.262);
    glBegin(GL_LINE_STRIP);
    glVertex2i(ix, iy);
    glVertex2i(ix + isize - 1, iy);
    glVertex2i(ix + isize - 1, iy + isize - 1);
    glEnd();

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ix + isize * 0.5f, iy + isize * 0.5f);

    for (theta = 0.0f; theta < (2.0f * M_PI + M_PI / 18.0f); theta += M_PI / 18.0f)
	glVertex2f(ix + isize * 0.5f + cos(theta) * isize * 0.4f,
	           iy + isize * 0.5f + sin(theta) * isize * 0.4f);
    glEnd();
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    int   i, j;
    int   ix, iy, isize;
    TP    *tp;
    float theta, st, ct;
    static GLfloat sunpos[4] = { -0.7071, -0.7071, 0.0, 0.0 };
    static GLfloat suncolor[4] = { 1.0, 1.0, 0.8, 1.0 };
    static GLfloat sunambient[4] = { 0.25, 0.25, 0.2, 1.0 };
    static GLfloat s_vector[4] = { 50.0 / TERRAIN_SIZE, 0.0, 0.0, 0.0 };
    static GLfloat t_vector[4] = { 0.0, 0.0, 50.0 / TERRAIN_SIZE, 0.0 };

    /* Force a full redraw for normal situations; Idle sets it to -1 */
    RedrawAll ++;

    /* Reset the viewport... */
    glViewport(0, 3 * Height / 8, Width, Height - 3 * Height / 8);
    glScissor(0, 3 * Height / 8, Width, Height - 3 * Height / 8);
    glEnable(GL_SCISSOR_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(22.5, 2.0f * (float)Width / (float)Height, 4.0,
                   TERRAIN_VIEW);
    glMatrixMode(GL_MODELVIEW);

    /* Clear the window to light blue... */
    if (ShowSky && UseTexturing && PolyMode == GL_FILL)
        glClear(GL_DEPTH_BUFFER_BIT);
    else
        {
        glClearColor(0.75, 0.75, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

    /* Setup viewing transformations for the current orientation... */
    glPushMatrix();
    glRotatef((float)ViewAngle, 0.0, 1.0, 0.0);
    glRotatef(Orientation[2], 0.0, 0.0, 1.0);
    glRotatef(Orientation[0], -1.0, 0.0, 0.0);
    glRotatef(Orientation[1], 0.0, 1.0, 0.0);
    glTranslatef(0.0, -Position[1], 0.0);

    glPolygonMode(GL_FRONT_AND_BACK, PolyMode);

    /* Draw the sky */
    if (UseTexturing && SkyTexture && ShowSky && PolyMode == GL_FILL)
        {
        glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, SkyTexture);

        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5, 0.5);
        glVertex3f(0.0, TERRAIN_VIEW, 0.0);
        for (theta = 0.0; theta < (2.1 * M_PI); theta += M_PI / 8)
            {
            ct = cos(theta);
            st = sin(theta);
            glTexCoord2f(0.5 + 0.3 * ct, 0.5 + 0.3 * st);
            glVertex3f(ct * TERRAIN_VIEW * 0.7071,
	               TERRAIN_VIEW * 0.7071,
	               st * TERRAIN_VIEW * 0.7071);
            }
        glEnd();

        glBegin(GL_TRIANGLE_STRIP);
        for (theta = 0.0; theta < (2.1 * M_PI); theta += M_PI / 8)
            {
            ct = cos(theta);
            st = sin(theta);
            glTexCoord2f(0.5 + 0.3 * ct, 0.5 + 0.3 * st);
            glVertex3f(ct * TERRAIN_VIEW * 0.7071,
	               TERRAIN_VIEW * 0.7071,
	               st * TERRAIN_VIEW * 0.7071);
            glTexCoord2f(0.5 + 0.5 * ct, 0.5 + 0.5 * st);
            glVertex3f(ct * TERRAIN_VIEW, -100.0,
	               st * TERRAIN_VIEW);
            }
        glEnd();
        }

    /* Setup lighting if needed... */
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sunambient);
    glEnable(GL_COLOR_MATERIAL);

    if (ShowLighting)
        {
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, sunpos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, suncolor);
        glLightfv(GL_LIGHT0, GL_AMBIENT, sunambient);
        }
    else
        glDisable(GL_LIGHT0);

    /* Then draw the terrain... */
    if (UseTexturing && LandTexture && PolyMode == GL_FILL)
        {
        glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, LandTexture);
        }

    glTranslatef(-Position[0], 0.0, -Position[2]);

    glColor3f(0.3, 0.8, 0.2);

    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, s_vector);

    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, t_vector);

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);

    if (ShowTerrain)
        {
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        for (i = 0, tp = Terrain[0]; i < (TERRAIN_COUNT - 1); i ++)
            {
            glBegin(GL_TRIANGLE_STRIP);
            for (j = 0; j < TERRAIN_COUNT; j ++, tp ++)
                {
                glNormal3fv(tp[0].n);
	        glVertex3fv(tp[0].v);

                glNormal3fv(tp[TERRAIN_COUNT].n);
	        glVertex3fv(tp[TERRAIN_COUNT].v);
	        }
            glEnd();
	    }
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        }
     else
        {
        glBegin(GL_QUADS);
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(-0.5 * TERRAIN_SIZE, 0.0, -0.5 * TERRAIN_SIZE);
        glVertex3f( 0.5 * TERRAIN_SIZE, 0.0, -0.5 * TERRAIN_SIZE);
        glVertex3f( 0.5 * TERRAIN_SIZE, 0.0,  0.5 * TERRAIN_SIZE);
        glVertex3f(-0.5 * TERRAIN_SIZE, 0.0,  0.5 * TERRAIN_SIZE);
        glEnd();
	}

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    if (ShowWater && ShowTerrain)
        {
        glEnable(GL_DEPTH_TEST);

        glColor4f(0.0, 0.0, 0.25, 0.75);
        for (i = 0; i < (TERRAIN_COUNT - 1); i ++)
	    {
	    glBegin(GL_TRIANGLE_STRIP);
            glNormal3f(0.0, 1.0, 0.0);
            for (j = 0; j < TERRAIN_COUNT; j ++)
	        {
		glTexCoord2i(i, j);
		glVertex3f(i * TERRAIN_SPACING - 0.5 * TERRAIN_SIZE, 0.0,
		           0.5 * TERRAIN_SIZE - j * TERRAIN_SPACING);

		glTexCoord2i(i + 1, j);
		glVertex3f((i + 1) * TERRAIN_SPACING - 0.5 * TERRAIN_SIZE, 0.0,
		           0.5 * TERRAIN_SIZE - j * TERRAIN_SPACING);
		}
	    glEnd();
	    }

        glDisable(GL_DEPTH_TEST);
        }

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    /* Figure out the sizes of the controls... */
    isize = Height / 4;
    if ((19 * isize / 4) > Width)
	isize = Width / 5;

    ix = (Width - 19 * isize / 4) / 2;
    iy = (Height / 2 - 3 * isize / 2) / 2;

    /* Redraw the FPS and instruments... */
    glViewport(0, 0, Width, Height);
    glDisable(GL_SCISSOR_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, (float)Width, 0.0f, (float)Height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    /* Show the cockpit outline */
    glColor3f(0.2f, 0.2f, 0.21f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2i(Width / 2, 3 * Height / 8);
    for (theta = 0.0; theta < (M_PI + M_PI / 20.0); theta += M_PI / 20.0)
        glVertex2f(Width / 2 - Width / 2 * cos(theta),
	           3 * Height / 8 + Height / 8 * sin(theta));
    glEnd();

    /* Show the frames-per-second in the upper window... */
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(Width / 2, 7 * Height / 16);
    FontPrintf(TextFont, 0, "FPS = %d", FPS);

    if (RedrawAll)
        {
        glColor3f(0.2f, 0.2f, 0.21f);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(Width, 0);
	glVertex2i(Width, 3 * Height / 8);
	glVertex2i(0, 3 * Height / 8);
	glEnd();
        }

    if (RedrawAirspeed || RedrawAll)
        {
	/*
	 * Redraw the airspeed indicator...
	 */

        Airspeed = Velocity;

        draw_instrument(ix, iy, isize);

        glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2i(ix + isize * 0.5f, iy + isize * 0.3f);
	FontPrintf(TextFont, 0, "KNOTS");

	glBegin(GL_LINES);
        for (i = 0; i < 200; i += 10)
	    {
	    theta = i * M_PI * 2.0f / 200.0f;
	    st    = sin(theta);
	    ct    = cos(theta);

	    glVertex2f(ix + isize * 0.5 + st * 0.3f * isize,
	               iy + isize * 0.5f + ct * 0.3f * isize);
	    glVertex2f(ix + isize * 0.5 + st * 0.4f * isize,
	               iy + isize * 0.5f + ct * 0.4f * isize);
	    }
	glEnd();

        glRasterPos2f(ix + isize * 0.5f, iy + isize * 0.2f + 2);
	FontPrintf(TextFont, 0, "100");

        glRasterPos2f(ix + isize * 0.2f, iy + isize * 0.5f - 4);
	FontPrintf(TextFont, 1, "150");

        glRasterPos2f(ix + isize * 0.5f, iy + isize * 0.8f - 10);
	FontPrintf(TextFont, 0, "0");

        glRasterPos2f(ix + isize * 0.8f, iy + isize * 0.5f - 4);
	FontPrintf(TextFont, -1, "50");

        glPushMatrix();
	    glTranslatef(ix + isize * 0.5f, iy + isize * 0.5f, 0.0);
	    glRotatef((Airspeed * 3600.0f / 1852.0f) * 360.0f / 200.0f,
	              0.0f, 0.0f, -1.0f);

            glColor3f(1.0f, 0.0f, 0.0f);
	    glBegin(GL_TRIANGLES);
	    glVertex2f(-0.05f * isize, 0.0f);
	    glVertex2f( 0.05f * isize, 0.0f);
	    glVertex2f( 0.0f, 0.35f * isize);
	    glEnd();
	glPopMatrix();
	}

    ix += 5 * isize / 4;
    iy += isize / 4;

    if (RedrawAltimeter || RedrawAll)
        {
	/*
	 * Redraw the altimeter...
	 */

        Altimeter = Position[1];

        draw_instrument(ix, iy, isize);

        glColor3f(0.0f, 0.0f, 0.0f);
	glRasterPos2i(ix + isize * 0.5f, iy + isize * 0.5f);
	glBitmap(0, 0, 0, 0, 0, -5, NULL);
	FontPrintf(TextFont, 0, "%05d FT", (int)(Altimeter * 3.28));

	glBegin(GL_LINES);
        for (i = 0; i < 100; i += 10)
	    {
	    theta = i * M_PI * 2.0f / 100.0f;
	    st    = sin(theta);
	    ct    = cos(theta);

	    glVertex2f(ix + isize * 0.5 + st * 0.3f * isize,
	               iy + isize * 0.5f + ct * 0.3f * isize);
	    glVertex2f(ix + isize * 0.5 + st * 0.4f * isize,
	               iy + isize * 0.5f + ct * 0.4f * isize);
	    }
	glEnd();

        glRasterPos2f(ix + isize * 0.5f, iy + isize * 0.8f - 10);
	FontPrintf(TextFont, 0, "0");

        glPushMatrix();
	    glTranslatef(ix + isize * 0.5f, iy + isize * 0.5f, 0.0);
	    glRotatef(fmod(Altimeter * 0.0328, 1.0) * 360.0f,
	              0.0f, 0.0f, -1.0f);

            glColor3f(1.0f, 0.0f, 0.0f);
	    glBegin(GL_TRIANGLES);
	    glVertex2f(-0.05f * isize, 0.0f);
	    glVertex2f( 0.05f * isize, 0.0f);
	    glVertex2f( 0.0f, 0.35f * isize);
	    glEnd();
	glPopMatrix();
	}

    ix += 5 * isize / 4;

    if (RedrawHorizon || RedrawAll)
        {
	/*
	 * Redraw the artificial horizon; we draw a 2D horizon which isn't
	 * perfect but illustrates the point...
	 */

	float start, end; /* Start and end angles of horizon */

        Horizon[0] = Orientation[0];
	Horizon[1] = Orientation[2];

        draw_instrument(ix, iy, isize);

        start = M_PI * (90.0f + Horizon[0] - Horizon[1]) / 180.0f;
	end   = M_PI * (270.0f - Horizon[0] - Horizon[1]) / 180.0f;

        glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_POLYGON);

	for (theta = start; theta < end; theta += M_PI / 18)
	    glVertex2f(ix + isize * 0.5f + sin(theta) * isize * 0.4f,
	               iy + isize * 0.5f + cos(theta) * isize * 0.4f);

	glVertex2f(ix + isize * 0.5f + sin(end) * isize * 0.4f,
	           iy + isize * 0.5f + cos(end) * isize * 0.4f);
	glEnd();

        glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_LINES);
	glVertex2f(ix + 0.2f * isize, iy + 0.5f * isize);
	glVertex2f(ix + 0.4f * isize, iy + 0.5f * isize);
	glVertex2f(ix + 0.6f * isize, iy + 0.5f * isize);
	glVertex2f(ix + 0.8f * isize, iy + 0.5f * isize);
	glEnd();
	}

    ix += 5 * isize / 4;
    iy -= isize / 4;

    if (RedrawCompass || RedrawAll)
        {
	/*
	 * Redraw the compass...
	 */

        Compass = Orientation[1];

        draw_instrument(ix, iy, isize);

        glPushMatrix();
	    glTranslatef(ix + isize * 0.5f, iy + isize * 0.5f, 0.0);
	    glRotatef(Compass, 0.0f, 0.0f, 1.0f);

            glColor3f(0.0f, 0.0f, 0.0f);

            glRasterPos2f(0.0f, isize * 0.3f);
	    glBitmap(0, 0, 0, 0, 0, -5, NULL);
	    FontPrintf(TextFont, 0, "N");

            glRasterPos2f(0.0f, -isize * 0.3f);
	    glBitmap(0, 0, 0, 0, 0, -5, NULL);
	    FontPrintf(TextFont, 0, "S");

            glRasterPos2f(isize * 0.3f, 0.0f);
	    glBitmap(0, 0, 0, 0, 0, -5, NULL);
	    FontPrintf(TextFont, 0, "E");

            glRasterPos2f(-isize * 0.3f, 0.0f);
	    glBitmap(0, 0, 0, 0, 0, -5, NULL);
	    FontPrintf(TextFont, 0, "W");

            glBegin(GL_LINES);
            for (i = 0; i < 360; i += 10)
	        if (i % 90)
		    {
		    theta = i * M_PI / 180.0f;
	            st    = sin(theta);
	            ct    = cos(theta);

	            glVertex2f(st * 0.3f * isize, ct * 0.3f * isize);
	            glVertex2f(st * 0.4f * isize, ct * 0.4f * isize);
		    }
	    glEnd();

	    glRotatef(Compass, 0.0f, 0.0f, -1.0f);

            glColor3f(1.0f, 0.0f, 0.0f);
	    glBegin(GL_TRIANGLES);
	    glVertex2f(0.0f, 0.3f * isize);
	    glVertex2f(-0.05f * isize, 0.1f * isize);
	    glVertex2f(0.05f * isize, 0.1f * isize);
	    glEnd();

            glRasterPos2i(0, 0);
	    FontPrintf(TextFont, 0, "%03d", (int)Compass);

	glPopMatrix();
	}

    ix -= 15 * isize / 4;

    if (!RedrawAll && UseSwapHint)
        {
	/* Tell windows just to swap the top */
	(*glAddSwapHintRectWIN)(0, 3 * Height / 8, Width,
	                        Height - 3 * Height / 8);

        /* and any instruments we've changed... */
	if (RedrawAirspeed)
	    (*glAddSwapHintRectWIN)(ix, iy, isize, isize);
	if (RedrawAltimeter)
	    (*glAddSwapHintRectWIN)(ix + 5 * isize / 4, iy + isize / 4, isize, isize);
	if (RedrawHorizon)
	    (*glAddSwapHintRectWIN)(ix + 10 * isize / 4, iy + isize / 4, isize, isize);
	if (RedrawCompass)
	    (*glAddSwapHintRectWIN)(ix + 15 * isize / 4, iy, isize, isize);
	}

    /* Finish up */
    glutSwapBuffers();

    RedrawAll       = 0;
    RedrawAirspeed  = 0;
    RedrawAltimeter = 0;
    RedrawCompass   = 0;
    RedrawHorizon   = 0;
    }


/*
 * 'Resize()' - Resize the window...
 */

void
Resize(int width,  /* I - Width of window */
       int height) /* I - Height of window */
    {
    /* Save the new width and height */
    Width  = width;
    Height = height;

    /* Force the whole window to be redrawn... */
    RedrawAll = 1;
    }


/*
 * 'Special()' - Handle special keys like left, right, up, and down.
 */

void
Special(int key, /* I - Key that was pressed */
        int x,   /* I - X location of the mouse pointer */
	int y)   /* I - Y location of the mouse pointer */
    {
    switch (key)
        {
	case GLUT_KEY_UP :
	    ViewAngle = 0;
	    glutPostRedisplay();
	    break;
	case GLUT_KEY_DOWN :
	    ViewAngle = 180;
	    glutPostRedisplay();
	    break;
	case GLUT_KEY_LEFT :
	    ViewAngle = (ViewAngle + 315) % 360;
	    glutPostRedisplay();
	    break;
	case GLUT_KEY_RIGHT :
	    ViewAngle = (ViewAngle + 45) % 360;
	    glutPostRedisplay();
	    break;
	}
    }
