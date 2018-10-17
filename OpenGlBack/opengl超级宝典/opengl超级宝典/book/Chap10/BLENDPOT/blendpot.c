/*
 * Transparent pots using glBlendFunc() from Chapter 10.
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

int Width;  /* Width of window */
int Height; /* Height of window */


/*
 * Functions...
 */

void Redraw(void);
void Resize(int width, int height);


/*
 * 'main()' - Open a window and display two teapots.
 */

int                /* O - Exit status */
main(int  argc,    /* I - Number of command-line arguments */
     char *argv[]) /* I - Command-line arguments */
    {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(792, 573);
    glutCreateWindow("Transparent Teapots");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);
    glutMainLoop();
    return (0);
    }


/*
 * 'Redraw()' - Clear the window...
 */

void
Redraw(void)
    {
    static GLfloat red_light[4]  = { 1.0, 0.0, 0.0, 1.0 };
    static GLfloat red_pos[4]    = { 1.0, 1.0, 1.0, 0.0 };
    static GLfloat blue_light[4] = { 0.0, 0.0, 1.0, 1.0 };
    static GLfloat blue_pos[4]   = { -1.0, -1.0, -1.0, 0.0 };

    /* Clear the window to black */
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Enable drawing features that we need... */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glShadeModel(GL_SMOOTH);

    /*
     * Draw two teapots in different colors...
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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -15.0);
    glRotatef(-45.0, 0.0, 1.0, 0.0);

    glDisable(GL_BLEND);
    glColor3f(1.0, 1.0, 0.0);
    glutSolidTeapot(1.0);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(45.0, 0.0, 1.0, 0.0);

    glEnable(GL_BLEND);
    glColor4f(1.0, 1.0, 1.0, 0.25);
    glutSolidTeapot(1.0);
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
    gluPerspective(22.5, (float)width / (float)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    }
