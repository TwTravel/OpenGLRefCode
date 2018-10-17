/*
 * Opengl Pencil program from Chapter 12.
 *
 * Written by Michael Sweet
 */

#include <GL/glut.h>
#include "texture.h"


/*
 * Globals...
 */

int           Width;            /* Width of window */
int           Height;           /* Height of window */
double        LastTime;         /* Last update time */
GLuint        PencilTexture,    /* Pencil texture image */
              LeadTexture;      /* Lead... */
GLfloat       PencilRoll = 0.0,	/* Pencil orientation */
              PencilPitch = 90.0,
              PencilHeading = 0.0;
GLUquadricObj *PencilObj;


/*
 * Functions...
 */

double GetClock(void);
void   Idle(void);
void   Redraw(void);
void   Resize(int width, int height);


/*
 * 'main()' - Open a window and display a pencil.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(792, 573);
    glutCreateWindow("OpenGL Pencil Using Quadrics");

    glutDisplayFunc(Redraw);
    glutIdleFunc(Idle);
    glutReshapeFunc(Resize);

    LastTime = GetClock();

    PencilObj = gluNewQuadric();
    gluQuadricTexture(PencilObj, GL_TRUE);

    PencilTexture = TextureLoad("pencil.bmp", GL_FALSE, GL_LINEAR, GL_LINEAR,
                                GL_REPEAT);
    LeadTexture   = TextureLoad("lead.bmp", GL_FALSE, GL_LINEAR, GL_LINEAR,
                                GL_REPEAT);

    glutMainLoop();
    return (0);
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
    return ((double)t.tv_sec + 0.001 * (double)t.tv_usec);
#endif /* WIN32 */
    }


/*
 * 'Idle()' - Move the viewer and redraw...
 */

void
Idle(void)
    {
    double  curtime;  /* Current time */
    GLfloat distance; /* Distance to move */

    /* Get the current system time to figure out how far to move. */
    curtime  = GetClock();
    distance = curtime - LastTime;
    LastTime = curtime;

    PencilHeading += distance * 30.0;
    PencilPitch   += distance * 10.0;
    PencilRoll    += distance * 20.0;

    glutPostRedisplay();
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    static GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0 };

    /* Clear the window to light blue */
    glClearColor(0.7, 0.7, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
     * Setup viewing transformations for the current position and
     * orientation...
     */

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(0.0, 0.0, -80.0);
    glRotatef(PencilHeading, 0.0, -1.0, 0.0);
    glRotatef(PencilPitch, 1.0, 0.0, 0.0);
    glRotatef(PencilRoll, 0.0, 0.0, -1.0);

    /* First the pencil body - this uses a 6-sided cylinder... */
    gluQuadricNormals(PencilObj, GLU_FLAT);
    glBindTexture(GL_TEXTURE_2D, PencilTexture);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -20.0);

    gluCylinder(PencilObj, 5.0, 5.0, 40.0, 6, 2);
    glPopMatrix();

    /* Then the ends - a cone at the tip and a flat cone at the base... */
    gluQuadricNormals(PencilObj, GLU_SMOOTH);
    glBindTexture(GL_TEXTURE_2D, LeadTexture);

    glPushMatrix();
    glTranslatef(0.0, 0.0, 20.0);

    gluCylinder(PencilObj, 5.0, 0.0, 7.5, 6, 2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, -20.0);

    /*
     * Normally we might use a disk shape for this, but unfortunately the
     * texture coordinates don't match up...
     */
    gluCylinder(PencilObj, 5.0, 0.0, 0.0, 6, 2);
    glPopMatrix();
    glPopMatrix();

    /* Finish up */
    glutSwapBuffers();
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

    /* Reset the viewport... */
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)width / (float)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    }
