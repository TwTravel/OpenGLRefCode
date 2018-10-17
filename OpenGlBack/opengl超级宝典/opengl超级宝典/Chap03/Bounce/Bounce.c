// Bounce.c
// Demonstrates a simple animated rectangle program with GLUT
// OpenGL SuperBible, 2nd Edition
// Richard S. Wright Jr.

#include <windows.h>
#include <gl/glut.h>

// Initial square position and size
GLfloat x1 = 100.0f;
GLfloat y1 = 150.0f;
GLsizei rsize = 50;

// Step size in x and y directions
// (number of pixels to move each time)
GLfloat xstep = 1.0f;
GLfloat ystep = 1.0f;

// Keep track of windows changing width and height
GLfloat windowWidth;
GLfloat windowHeight;


// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

   	// Set current drawing color to red
	//		   R	 G	   B
	glColor3f(1.0f, 0.0f, 0.0f);

	// Draw a filled rectangle with current color
	glRectf(x1, y1, x1+rsize, y1+rsize);

	// Flush drawing commands
    glutSwapBuffers();
	}

// Called by GLUT library when idle (window not being
// resized or moved)
void TimerFunction(int value)
	{
	// Reverse direction when you reach left or right edge
	if(x1 > windowWidth-rsize || x1 < 0)
		xstep = -xstep;

	// Reverse direction when you reach top or bottom edge
	if(y1 > windowHeight-rsize || y1 < 0)
		ystep = -ystep;


	// Check bounds.  This is incase the window is made
	// smaller and the rectangle is outside the new
	// clipping volume
	if(x1 > windowWidth-rsize)
		x1 = windowWidth-rsize-1;

	if(y1 > windowHeight-rsize)
		y1 = windowHeight-rsize-1;

	// Actually move the square
	x1 += xstep;
	y1 += ystep;

	// Redraw the scene with new coordinates
    glutPostRedisplay();
	glutTimerFunc(33,TimerFunction, 1);
	}

// Setup the rendering state
void SetupRC(void)
    {
    // Set clear color to blue
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    }



// Called by GLUT library when the window has chanaged size
void ChangeSize(GLsizei w, GLsizei h)
	{
	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Keep the square square, this time, save calculated
	// width and height for later use
	if (w <= h) 
		{
		windowHeight = 250.0f*h/w;
		windowWidth = 250.0f;
		}
    else 
		{
		windowWidth = 250.0f*w/h;
		windowHeight = 250.0f;
		}

	// Set the clipping volume
	glOrtho(0.0f, windowWidth, 0.0f, windowHeight, 1.0f, -1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}



// Main program entry point
void main(void)
    {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Bounce");
	glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);
    glutTimerFunc(33, TimerFunction, 1);

	SetupRC();

	glutMainLoop();
    }

