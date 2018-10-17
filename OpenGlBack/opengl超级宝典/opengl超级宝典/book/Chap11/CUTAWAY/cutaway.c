/*
 * Example program demonstrating the use of glDepthFunc and glDrawBuffer
 * to implement a cutting plane for Chapter 11.
 *
 * Written by Michael Sweet.
 *
 * Press the 'd' key to toggle between GL_LESS and GL_GREATER depth
 * tests.  Press the 'ESC' key to quit.
 */

#include <GL/glut.h>


/*
 * Globals...
 */

int     Width;                /* Width of window */
int     Height;               /* Height of window */
GLenum  DepthFunc = GL_LESS;  /* Depth comparison function */
GLfloat	CuttingPlane = -22.0, /* Cutting plane distance */
	CuttingDir   = 0.25;   /* Cutting plane direction */


/*
 * Functions...
 */

void Timer(int val);
void Keyboard(unsigned char key, int x, int y);
void Redraw(void);
void Resize(int width, int height);


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
    glutCreateWindow("Cutting Example Using glDepthTest and glDrawBuffer");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);
    glutTimerFunc(250, Timer, 0);
    glutKeyboardFunc(Keyboard);
    glutMainLoop();
    return (0);
    }


/*
 * 'Timer()' - Move the cutting plane while we are idle...
 */

void
Timer(int val)
    {
    CuttingPlane += CuttingDir;

    /* Reverse directions as needed... */
    if (CuttingPlane <= -22.0 ||
        CuttingPlane >= -18.0)
        CuttingDir = -CuttingDir;

    Redraw();
    glutTimerFunc(250, Timer, 0);
    }


/*
 * 'Keyboard()' - Handle key presses...
 */

void
Keyboard(unsigned char key, /* I - Key that was pressed */
         int           x,   /* I - Mouse X position */
	 int           y)   /* I - Mouse Y position */
    {
    if (key == 'd' || key == 'D')
        {
	/* Toggle depth test */
        if (DepthFunc == GL_LESS)
            DepthFunc = GL_GREATER;
        else
            DepthFunc = GL_LESS;

        Redraw();
        }
    else if (key == 0x1b)
        exit(0);
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    static float red_light[4]  = {  1.0,  0.0,  0.0,  1.0 };
    static float red_pos[4]    = {  1.0,  1.0,  1.0,  0.0 };
    static float blue_light[4] = {  0.0,  0.0,  1.0,  1.0 };
    static float blue_pos[4]   = { -1.0, -1.0, -1.0,  0.0 };


    /* Enable drawing features that we need... */
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glShadeModel(GL_SMOOTH);
    glDepthFunc(DepthFunc); 

    /* Clear the color and depth buffers... */

    if (DepthFunc == GL_LESS)
        glClearDepth(1.0);
    else
        glClearDepth(0.0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
     * Draw the cutting plane.  Note that we disable drawing into
     * the normal color buffer while we do this...
     */

    glDrawBuffer(GL_NONE);

    glColor3i(0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex3f(-100.0, 100.0, CuttingPlane);
    glVertex3f(100.0, 100.0, CuttingPlane);
    glVertex3f(100.0, -100.0, CuttingPlane);
    glVertex3f(-100.0, -100.0, CuttingPlane);
    glEnd();

    glDrawBuffer(GL_BACK);

    /*
     * Draw the cube and sphere in different colors...
     *
     * We have positioned two lights in this scene.  The first is red
     * and located above, to the right, and behind the viewer.  The
     * second is blue and located below, to the left, and in front of
     * the viewer.
     */

    glLightfv(GL_LIGHT0, GL_DIFFUSE, red_light);
    glLightfv(GL_LIGHT0, GL_POSITION, red_pos);

    glLightfv(GL_LIGHT1, GL_DIFFUSE, blue_light);
    glLightfv(GL_LIGHT1, GL_POSITION, blue_pos);

    glPushMatrix();
    glTranslatef(-1.0, 0.0, -20.0);
    glutSolidSphere(1.0, 18, 9);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.0, 0.0, -20.0);
    glRotatef(15.0, 0.0, 1.0, 0.0);
    glRotatef(15.0, 0.0, 0.0, 1.0);
    glutSolidCube(2.0);
    glPopMatrix();

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
    gluPerspective(22.5, (float)width / (float)height, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    }
