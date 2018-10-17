/*
 * 2D texturing demo from Chapter 8.
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
#include "bitmap.h"


/*
 * Globals...
 */

int        Width;    /* Width of window */
int        Height;   /* Height of window */
BITMAPINFO *TexInfo; /* Texture bitmap information */
GLubyte    *TexBits; /* Texture bitmap pixel bits */


/*
 * Functions...
 */

void Redraw(void);
void Resize(int width, int height);


/*
 * 'main()' - Open a window and display a textured sky.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Textured Tea Pot with Specular Highlights");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);

    TexBits = LoadDIBitmap("pot.bmp", &TexInfo);

    glutMainLoop();
    return (0);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    static GLfloat dark[4] = { 0.0, 0.0, 0.0, 1.0 };
    static GLfloat normal[4] = { 0.5, 0.5, 0.5, 1.0 };
    static GLfloat bright[4] = { 1.0, 1.0, 1.0, 1.0 };
    static GLfloat pos[4] = { -0.2588, 0.0, 0.9659, 0.0 };

    /* Clear the window */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Setup for drawing... */
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -50.0);

    /* First draw the pot without texturing and with specular highlights */
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, dark);

    glLightfv(GL_LIGHT0, GL_DIFFUSE, dark);
    glLightfv(GL_LIGHT0, GL_SPECULAR, bright);
    glLightfv(GL_LIGHT0, GL_POSITION, pos);

    glMaterialfv(GL_FRONT, GL_AMBIENT, dark);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, dark);
    glMaterialfv(GL_FRONT, GL_SPECULAR, bright);
    glMateriali(GL_FRONT, GL_SHININESS, 128);

    glColor3f(1.0, 1.0, 1.0);
    glutSolidTeapot(10.0);

    /* Define the 2D texture image. */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, TexInfo->bmiHeader.biWidth,
                 TexInfo->bmiHeader.biHeight, 0, GL_BGR_EXT,
		 GL_UNSIGNED_BYTE, TexBits);

    /* Draw the teapot with texturing, adding the textured colors... */
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, normal);

    glLightfv(GL_LIGHT0, GL_AMBIENT, normal);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, normal);
    glLightfv(GL_LIGHT0, GL_SPECULAR, dark);

    glMaterialfv(GL_FRONT, GL_AMBIENT, normal);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, normal);
    glMaterialfv(GL_FRONT, GL_SPECULAR, dark);
    glMateriali(GL_FRONT, GL_SHININESS, 0);

    glutSolidTeapot(10.0);

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
