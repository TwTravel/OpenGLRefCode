/* bit.c                     */

/* E. Angel, Interactive Computer Graphics */
/* A Top-Down Approach with OpenGL, Third Edition */
/* Addison-Wesley Longman, 2003 */

/* Puts a checkboard bit pattern on the display */
/* Left mouse button uses XOR to erase and then */
/* redraw in a new location */

#include <GL/glut.h>

GLubyte wb[2]={0x00,0xff};
GLubyte check[512];
int ww;

void init()
{
   glClearColor(1.0, 1.0, 0.0, 1.0);
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 0.0, 0.0);
	glRasterPos2f(0.0, 0.0);
	glBitmap(64, 64, 0.0, 0.0, 0.0, 0.0, check);
	glFlush();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0.0,  (GLfloat) w, 0.0, (GLfloat) h);
	ww=w;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int btn, int state, int x, int y)
{

	if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
		{
		glBitmap(64, 64, 0.0, 0.0, 0.0, 0.0, check);
		glRasterPos2i(x, ww-y);
		glBitmap(64, 64, 0.0, 0.0, 0.0, 0.0, check);
		glFlush();
		}
}


int main(int argc, char** argv)
{

/* Initialize mode and open a window in upper left corner of screen */
/* Window title is name of program (arg[0]) */

	int i,j;
	for(i=0;i<64;i++) for(j=0;j<8;j++) check[i*8+j] = wb[(i/8+j)%2];
	glutInit(&argc,argv); 
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(500,500);
	glutInitWindowPosition(0,0); 
	glutCreateWindow("bitmap");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	init();
	glEnable(GL_COLOR_LOGIC_OP); 
	glLogicOp(GL_XOR);
	glutMainLoop();

}

