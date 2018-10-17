/*
 * OpenGL raster font demo from Chapter 7.
 *
 * Written by Michael Sweet.
 */

/*
 * Include necessary headers.
 */

#include "font.h"
#include <GL/glut.h>


/*
 * Globals...
 */

int    Width;  /* Width of window */
int    Height; /* Height of window */
GLFONT *Font;  /* Font data */


/*
 * Functions...
 */

void Redraw(void);
void Resize(int width, int height);


/*
 * 'main()' - Open a window and display some text.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Bitmap Font Example");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);
    Font = FontCreate(wglGetCurrentDC(), "Times", 32, 0, 1);
    glutMainLoop();
    return (0);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    /* Clear the window to black */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw a vertical line down the center of the window */
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    glVertex2i(Width / 2, 0);
    glVertex2i(Width / 2, Height);
    glEnd();

    /* Draw text left justified... */
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2i(Width / 2, 3 * Height / 4);
    FontPrintf(Font, 1, "Left Justified Text");

    /* Draw text centered... */
    glColor3f(0.0, 1.0, 0.0);
    glRasterPos2i(Width / 2, 2 * Height / 4);
    FontPrintf(Font, 0, "Centered Text");

    /* Draw right-justified text... */
    glColor3f(0.0, 0.1, 1.0);
    glRasterPos2i(Width / 2, 1 * Height / 4);
    FontPrintf(Font, -1, "Right Justified Text");

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
    glOrtho(0.0, (GLfloat)width, 0.0, (GLfloat)height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    }
