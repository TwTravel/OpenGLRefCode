/*
 * 1D textured rainbow demo from Chapter 8.
 *
 * Written by Michael Sweet
 */

/*
 * Include necessary headers.
 */

#include <GL/glut.h>
#include <math.h>
#ifndef M_PI
#  define M_PI 3.141592649
#endif /* !M_PI */


/*
 * Globals...
 */

int Width;   /* Width of window */
int Height;  /* Height of window */


/*
 * Functions...
 */

void Redraw(void);
void Resize(int width, int height);


/*
 * 'main()' - Open a window and display a rainbow.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(792, 573);
    glutCreateWindow("1D Textured Rainbow");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);
    glutMainLoop();
    return (0);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    GLfloat x, y, z, th;
    static GLubyte roygbiv_image[8][3] =
        {
        { 0x3f, 0x00, 0x3f }, /* Dark Violet (for 8 colors...) */
        { 0x7f, 0x00, 0x7f }, /* Violet */
        { 0xbf, 0x00, 0xbf }, /* Indigo */
        { 0x00, 0x00, 0xff }, /* Blue */
        { 0x00, 0xff, 0x00 }, /* Green */
        { 0xff, 0xff, 0x00 }, /* Yellow */
        { 0xff, 0x7f, 0x00 }, /* Orange */
        { 0xff, 0x00, 0x00 }  /* Red */
        };

    /* Clear the window to light blue */
    glClearColor(0.5, 0.5, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Load the texture data */
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage1D(GL_TEXTURE_1D, 0, 3, 8, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 roygbiv_image);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    /* First draw the ground... */
    glDisable(GL_TEXTURE_1D);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glPushMatrix();
    glRotatef(10.0, 0.0, 1.0, 0.0);
    glTranslatef(0.0, -40.0, -100.0);

    glColor3f(0.0, 0.8, 0.0);
    glBegin(GL_POLYGON);
    for (th = 0.0; th < (2.0 * M_PI); th += (0.03125 * M_PI))
        {
        x = cos(th) * 200.0;
	z = sin(th) * 200.0;
	glVertex3f(x, 0.0, z);
        }
    glEnd();

    /* Then a rainbow... */
    glEnable(GL_TEXTURE_1D);
    glBegin(GL_QUAD_STRIP);
    for (th = 0.0; th <= M_PI; th += (0.03125 * M_PI))
        {
        x = cos(th) * 50.0;
	y = sin(th) * 50.0;
	z = -50.0;
	glTexCoord1f(0.0);
	glVertex3f(x, y, z);

        x = cos(th) * 55.0;
	y = sin(th) * 55.0;
	z = -50.0;
	glTexCoord1f(1.0);
	glVertex3f(x, y, z);
        }
    glEnd();

    glPopMatrix();

    glFinish();
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
    gluPerspective(30.0, (float)width / (float)height, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    }
