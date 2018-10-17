// ccube.c
// OpenGL SuperBible, Chapter 6
// Demonstrates primative RGB Color Cube
// Program by Richard S. Wright Jr.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>



// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;



// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    glPushMatrix();

   	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);


	// Front face
	glBegin(GL_POLYGON);
		// White
		glColor3ub((GLubyte) 255, (GLubyte)255, (GLubyte)255);
		glVertex3f(50.0f,50.0f,50.0f);

		// Yellow
		glColor3ub((GLubyte) 255, (GLubyte)255, (GLubyte)0);
		glVertex3f(50.0f,-50.0f,50.0f);

		// Red
		glColor3ub((GLubyte) 255, (GLubyte)0, (GLubyte)0);
		glVertex3f(-50.0f,-50.0f,50.0f);

		// Magenta
		glColor3ub((GLubyte) 255, (GLubyte)0, (GLubyte)255);
		glVertex3f(-50.0f,50.0f,50.0f);
	glEnd();

	
	// Back Face
	glBegin(GL_POLYGON);
		// Cyan
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(50.0f,50.0f,-50.0f);

		// Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(50.0f,-50.0f,-50.0f);
		
		// Black
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-50.0f,-50.0f,-50.0f);

		// Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-50.0f,50.0f,-50.0f);
	glEnd();


	// Top Face
	glBegin(GL_POLYGON);
		// Cyan
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(50.0f,50.0f,-50.0f);

		// White
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(50.0f,50.0f,50.0f);

		// Magenta
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-50.0f,50.0f,50.0f);

		// Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-50.0f,50.0f,-50.0f);
	glEnd();


	// Bottom Face
	glBegin(GL_POLYGON);
		// Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(50.0f,-50.0f,-50.0f);

		// Yellow
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(50.0f,-50.0f,50.0f);

		// Red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-50.0f,-50.0f,50.0f);

		// Black
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-50.0f,-50.0f,-50.0f);
	glEnd();


	// Left face
	glBegin(GL_POLYGON);
		// White
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(50.0f,50.0f,50.0f);

		// Cyan
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(50.0f,50.0f,-50.0f);

		// Green
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(50.0f,-50.0f,-50.0f);

		// Yellow
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(50.0f,-50.0f,50.0f);
	glEnd();


	// Right face
	glBegin(GL_POLYGON);
		// Magenta
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-50.0f,50.0f,50.0f);

		// Blue
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-50.0f,50.0f,-50.0f);

		// Black
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-50.0f,-50.0f,-50.0f);

		// Red
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-50.0f,-50.0f,50.0f);
	glEnd();

    glPopMatrix();

	// Show the graphics
	glutSwapBuffers();
	}

// This function does any needed initialization on the rendering
// context. 
void SetupRC()
	{
	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	glEnable(GL_DEPTH_TEST);	
//	glEnable(GL_DITHER);
	glShadeModel(GL_SMOOTH);
    }

void SpecialKeys(int key, int x, int y)
	{
	if(key == GLUT_KEY_UP)
		xRot-= 5.0f;

	if(key == GLUT_KEY_DOWN)
		xRot += 5.0f;

	if(key == GLUT_KEY_LEFT)
		yRot -= 5.0f;

	if(key == GLUT_KEY_RIGHT)
		yRot += 5.0f;

	if(key > 356.0f)
		xRot = 0.0f;

	if(key < -1.0f)
		xRot = 355.0f;

	if(key > 356.0f)
		yRot = 0.0f;

	if(key < -1.0f)
		yRot = 355.0f;

	// Refresh the Window
	glutPostRedisplay();
	}


void ChangeSize(int w, int h)
	{
	GLfloat nRange = 100.0f;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
    if (w <= h) 
		glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
    else 
		glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}

int main(int argc, char* argv[])
	{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("RGB Cube");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();

	return 0;
	}
