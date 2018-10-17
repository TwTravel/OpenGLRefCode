/* gasket3d.c  */

/* E. Angel, Interactive Computer Graphics */
/* A Top-Down Approach with OpenGL, Third Edition */
/* Addison-Wesley Longman, 2003 */


/* Three-Dimensional Sierpinski Gasket GL program */

/* The Sierpinski gasket is defined recursively as follows:
Start with three vertices in the place, defining a triangle.
Pick a  random point inside the triangle. 
Select any of the vertices of the triangle.
Plot the point halfway between the random point and the
randomly chosen vertex. 
Continue with this point as the new starting point. */

/* This program illustrates simple use of mouse with OpenGL
to start and stop program execution */

#include <stdlib.h>
#include <GL/glut.h>

/* define a point data type */

typedef struct { float x,y,z;} point;

point vertices[4]={{0,0,0},{250,500,100},{500,250,250},{250,100,250}}; /* A tetrahedron */

int j;
point new, old={250,100,250}; 

void clear(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void display(void)

/* computes and plots a single new point */

{
	int rand();
	j=rand()%4; /* pick a vertex at random */


/* Compute point halfway between vertex and old point */

	new.x = (old.x+vertices[j].x)/2; 
	new.y = (old.y+vertices[j].y)/2;
	new.z = (old.z+vertices[j].z)/2;
	
/* plot point */

	glBegin(GL_POINTS);
		glColor3f(1.0-new.z/250.,new.z/250.,0.);
		glVertex3f(new.x, new.y,new.z);
	glEnd();


/* replace old point by new */

	old.x=new.x;
	old.y=new.y;
	old.z=new.z;
	

	glFlush();
}


void mouse(int btn, int state, int x, int y)
{
if(btn==GLUT_LEFT_BUTTON&state==GLUT_DOWN)  glutIdleFunc(display);
if(btn==GLUT_MIDDLE_BUTTON&state==GLUT_DOWN)   glutIdleFunc(NULL);
if(btn==GLUT_RIGHT_BUTTON&state==GLUT_DOWN)   exit(0);
}

int main(int argc, char** argv)
{


	glutInit(&argc,argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Sierpinski Gasket");

	glutIdleFunc (display);
	glutMouseFunc (mouse);  
	glClearColor(1.0, 1.0, 1.0, 0.0); /* white background */
	glColor3f(1.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 500.0, 0.0, 500.0, -500.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glutDisplayFunc(clear);


	glutMainLoop();

}

