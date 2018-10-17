/*
 * Letter A using the GLU polygon tessellator from Chapter 12.
 *
 * Written by Michael Sweet
 */

#include <GL/glut.h>


/*
 * Globals...
 */

int     Width;          /* Width of window */
int     Height;         /* Height of window */
GLfloat Rotation = 0.0; /* Rotation of letter */


/*
 * Functions...
 */

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
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Letter A Using GLU Polygon Tessellator");

    glutDisplayFunc(Redraw);
    glutIdleFunc(Idle);
    glutReshapeFunc(Resize);

    glutMainLoop();
    return (0);
    }


/*
 * 'Idle()' - Move the viewer and redraw...
 */

void
Idle(void)
    {
    Rotation += 2.0;

    glutPostRedisplay();
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    GLUtesselator   *tess;
    static GLdouble outside[7][3] =
        {
        { 0.0, 1.0, 0.0 },
        { -0.5, -1.0, 0.0 },
        { -0.4, -1.0, 0.0 },
        { -0.2, -0.1, 0.0 },
        { 0.2, -0.1, 0.0 },
        { 0.4, -1.0, 0.0 },
        { 0.5, -1.0, 0.0 }
        };
    static GLdouble inside[3][3] =
        {
        { 0.0, 0.6, 0.0 },
        { -0.1, 0.1, 0.0 },
        { 0.1, 0.1, 0.0 }
        };
    static float    red_light[4] = { 1.0, 0.0, 0.0, 1.0 };
    static float    red_pos[4] = { 1.0, 0.0, 0.0, 0.0 };
    static float    blue_light[4] = { 0.0, 0.0, 1.0, 1.0 };
    static float    blue_pos[4] = { -1.0, 0.0, 0.0, 0.0 };

    /* Enable drawing features that we need... */
    glDisable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glShadeModel(GL_SMOOTH);

    /* Clear the color and depth buffers... */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    /*
     * Draw the Letter A in different colors...
     *
     * We have positioned two lights in this scene.  The first is red and
     * located above, to the right, and behind the viewer.  The second is blue
     * and located below, to the left, and in front of the viewer.
     */

    glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light);
    glLightfv(GL_LIGHT0, GL_POSITION, red_pos);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, blue_light);
    glLightfv(GL_LIGHT1, GL_POSITION, blue_pos);

    glEnable(GL_COLOR_MATERIAL);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -15.0);
    glRotatef(-Rotation, 0.0, 1.0, 0.0);

    glColor3f(0.0, 1.0, 0.0);
    glNormal3f(0.0, 0.0, 1.0);

    tess = gluNewTess();
    gluTessCallback(tess, GLU_BEGIN, glBegin);
    gluTessCallback(tess, GLU_VERTEX, glVertex3dv);
    gluTessCallback(tess, GLU_END, glEnd);

    gluTessBeginPolygon(tess, (GLvoid *)0);
    gluTessBeginContour(tess);
      gluTessVertex(tess, outside[0], outside[0]);
      gluTessVertex(tess, outside[1], outside[1]);
      gluTessVertex(tess, outside[2], outside[2]);
      gluTessVertex(tess, outside[3], outside[3]);
      gluTessVertex(tess, outside[4], outside[4]);
      gluTessVertex(tess, outside[5], outside[5]);
      gluTessVertex(tess, outside[6], outside[6]);
    gluTessEndContour(tess);

    gluTessBeginContour(tess);
        gluTessVertex(tess, inside[0], inside[0]);
        gluTessVertex(tess, inside[1], inside[1]);
        gluTessVertex(tess, inside[2], inside[2]);
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    gluDeleteTess(tess);

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
