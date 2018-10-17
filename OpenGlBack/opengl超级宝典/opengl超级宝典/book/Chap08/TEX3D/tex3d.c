/*
 * 3D texturing demo from Chapter 8.
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

int     Width;                  /* Width of window */
int     Height;                 /* Height of window */
GLubyte Texture[16][16][16][3]; /* Texture image */


/*
 * Functions...
 */

void MakeTexture(void);
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
    glutCreateWindow("3D Textured Cut-Away Cube");
    glutReshapeFunc(Resize);
    glutDisplayFunc(Redraw);

    MakeTexture();

    glutMainLoop();
    return (0);
    }


/*
 * 'MakeTexture()' - Make the 3D texture image...
 */

void
MakeTexture(void)
    {
    int i, j, k; /* Looping vars */
    GLubyte colors[8][3] =
        {
	{ 255, 255, 255 }, /* White */
	{ 255, 0,   0 },   /* Red */
	{ 255, 63,  0 },   /* Orange */
	{ 255, 255, 0 },   /* Yellow */
	{ 0,   255, 0 },   /* Green */
	{ 0,   255, 255 }, /* Blue */
	{ 0,   0,   255 }, /* Indigo */
	{ 255, 0,   255 }  /* Violet */
	};

    /* Set the color layers */
    for (k = 0; k < 8; k ++)
        for (i = 1; i < 15; i ++)
            for (j = 1; j < 15; j ++)
	        {
	        /* Top */
	        Texture[16 - k][i][j][0] = colors[k][0];
	        Texture[16 - k][i][j][1] = colors[k][1];
	        Texture[16 - k][i][j][2] = colors[k][2];
	        /* Bottom */
	        Texture[k][i][j][0] = colors[k][0];
	        Texture[k][i][j][1] = colors[k][1];
	        Texture[k][i][j][2] = colors[k][2];
	        /* Left */
	        Texture[j][i][k][0] = colors[k][0];
	        Texture[j][i][k][1] = colors[k][1];
	        Texture[j][i][k][2] = colors[k][2];
	        /* Right */
	        Texture[j][i][16 - k][0] = colors[k][0];
	        Texture[j][i][16 - k][1] = colors[k][1];
	        Texture[j][i][16 - k][2] = colors[k][2];
	        /* Front */
	        Texture[i][k][j][0] = colors[k][0];
	        Texture[i][k][j][1] = colors[k][1];
	        Texture[i][k][j][2] = colors[k][2];
	        /* Back */
	        Texture[i][16 - k][j][0] = colors[k][0];
	        Texture[i][16 - k][j][1] = colors[k][1];
	        Texture[i][16 - k][j][2] = colors[k][2];
	        }
    }


/*
 * 'Redraw()' - Redraw the window...
 */

void
Redraw(void)
    {
    /* Define the 3D texture image. */
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, 3, 16, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 Texture);
    glEnable(GL_TEXTURE_3D);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(0.0, 0.0, -50.0);
    glRotatef(45.0, 0.0, 1.0, 0.0);

    glColor3f(1.0, 1.0, 1.0);

    /* Draw a 3D cube with a quadrant cut away... */
    glBegin(GL_TRIANGLE_STRIP);

    glTexCoord3f(0.0, 0.0, 0.0);
    glVertex3f(-10.0, -10.0, 10.0);
    glTexCoord3f(0.0, 1.0, 0.0);
    glVertex3f(-10.0, 10.0, 10.0);

    glTexCoord3f(0.5, 0.0, 0.0);
    glVertex3f(0.0, -10.0, 10.0);
    glTexCoord3f(0.5, 1.0, 0.0);
    glVertex3f(0.0, 10.0, 10.0);

    glTexCoord3f(0.5, 0.0, 0.5);
    glVertex3f(0.0, -10.0, 0.0);
    glTexCoord3f(0.5, 1.0, 0.5);
    glVertex3f(0.0, 10.0, 0.0);

    glTexCoord3f(1.0, 0.0, 0.5);
    glVertex3f(10.0, -10.0, 0.0);
    glTexCoord3f(1.0, 1.0, 0.5);
    glVertex3f(10.0, 10.0, 0.0);

    glTexCoord3f(1.0, 0.0, 1.0);
    glVertex3f(10.0, -10.0, -10.0);
    glTexCoord3f(1.0, 1.0, 1.0);
    glVertex3f(10.0, 10.0, -10.0);

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
