/*
 * OpenGL paint program from Chapter 10.
 *
 * Written by Michael Sweet.
 */

/*
 * Include necessary headers.
 */

#include <GL/glut.h>


/*
 * Globals...
 */

int     Width;         /* Width of window */
int     Height;        /* Height of window */
GLubyte Brush[7][16] = /* Luminance-alpha brush image (7x7) */
{
  { 0xff,0x00,0xff,0x0f,0xff,0x1f,0xff,0x3f,0xff,0x1f,0xff,0x0f,0xff,0x00 },
  { 0xff,0x0f,0xff,0x2f,0xff,0x7f,0xff,0xbf,0xff,0x7f,0xff,0x2f,0xff,0x0f },
  { 0xff,0x1f,0xff,0x7f,0xff,0xbf,0xff,0xff,0xff,0xbf,0xff,0x7f,0xff,0x1f },
  { 0xff,0x3f,0xff,0xbf,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xbf,0xff,0x3f },
  { 0xff,0x1f,0xff,0x7f,0xff,0xbf,0xff,0xff,0xff,0xbf,0xff,0x7f,0xff,0x1f },
  { 0xff,0x0f,0xff,0x2f,0xff,0x7f,0xff,0xbf,0xff,0x7f,0xff,0x2f,0xff,0x0f },
  { 0xff,0x00,0xff,0x0f,0xff,0x1f,0xff,0x3f,0xff,0x1f,0xff,0x0f,0xff,0x00 },
};


/*
 * Functions...
 */

void	Motion(int x, int y);
void    Redraw(void);
void    Resize(int width, int height);


/*
 * 'main()' - Open a window and let the user paint stuff.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    int i;         /* Looping var */

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Paint");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);
    glutMotionFunc(Motion);
    glutMainLoop();
    return (0);
    }


/*
 * 'Motion()' - Draw where the user is pointing the mouse.
 */

void
Motion(int x, /* I - Mouse X position */
       int y) /* I - Mouse Y position */
    {
    /* Position the brush and center it */
    glRasterPos2i(x, y);
    glBitmap(0, 0, 0, 0, -3, -3, (GLubyte *)0);

    /* Draw the luminance-alpha image */
    glDrawPixels(7, 7, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, Brush[0]);
    glFinish();
    }


/*
 * 'Redraw()' - Clear the window...
 */

void
Redraw(void)
    {
    /* Clear the window to white */
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Setup the pixel mapping */
    glPixelTransferf(GL_RED_SCALE, 1.0);
    glPixelTransferf(GL_GREEN_SCALE, 0.0);
    glPixelTransferf(GL_BLUE_SCALE, 0.0);

    /* Setup the blending functions... */
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

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
    glOrtho(0.0, (GLfloat)width, (GLfloat)height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    }
