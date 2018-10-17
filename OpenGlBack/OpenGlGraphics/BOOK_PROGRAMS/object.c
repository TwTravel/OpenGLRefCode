/*   object.c          */

/* E. Angel, Interactive Computer Graphics */
/* A Top-Down Approach with OpenGL, Third Edition */
/* Addison-Wesley Longman, 2003 */

/* displays various glu objects */

#include <stdlib.h>
#include <GL/glut.h>



GLUquadricObj *obj;
static GLfloat theta[] = {0.0,0.0,0.0};
static GLint axis = 2;

void display()
{
/* display callback, clear frame buffer and z buffer,
   rotate object and draw, swap buffers */

 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);

/* glutWireIcosahedron();*/
/*	glutWireDodecahedron();*/
 /*gluSphere(obj, 1.0, 12, 12);*/
/*	gluCylinder(obj, 1.0, 0.5, 1.0, 12, 12); */
/*	gluDisk(obj, 0.5, 1.0, 10, 10);*/
/*	gluPartialDisk( obj, 0.5, 1.0, 10, 10, 0.0, 45.0);*/
	glutWireTeapot(1.0);
/*	glutWireTorus(0.5, 1.0, 10, 10);*/
/*	glutWireCone(1.0, 1.0, 10, 10);*/

	glutSwapBuffers();
}

void spinObject()
{

/* Idle callback, spin cube 2 degrees about selected axis */

	theta[axis] += 2.0;
	if( theta[axis] > 360.0 ) theta[axis] -= 360.0;
	glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y)
{

/* mouse callback, selects an axis about which to rotate */

	if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN) axis = 0;
	if(btn==GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) axis = 1;
	if(btn==GLUT_RIGHT_BUTTON && state == GLUT_DOWN) axis = 2;
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho(-2.0, 2.0, -2.0 * (GLfloat) h / (GLfloat) w,
            2.0 * (GLfloat) h / (GLfloat) w, -10.0, 10.0);
    else
        glOrtho(-2.0 * (GLfloat) w / (GLfloat) h,
            2.0 * (GLfloat) w / (GLfloat) h, -2.0, 2.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

void key(unsigned char key, int x, int y)
{
	if(key=='1') glutIdleFunc(NULL);
	if(key=='2') glutIdleFunc(spinObject);
}

void
main(int argc, char **argv)
{
   
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("object");

/* need both double buffering and z buffer */

    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
   glutIdleFunc(NULL);
   glutMouseFunc(mouse);
   glutKeyboardFunc(key);
   glClearColor(1.0, 1.0, 1.0, 1.0);
   glColor3f(0.0, 0.0, 0.0);
   obj = gluNewQuadric();
   gluQuadricDrawStyle(obj, GLU_LINE);
 
    glutMainLoop();
}
