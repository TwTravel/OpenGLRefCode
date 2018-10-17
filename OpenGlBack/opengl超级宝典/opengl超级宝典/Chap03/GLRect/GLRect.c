// GLRect.c
// The Drawing a simple 3D rectangle program with GLUT
// OpenGL SuperBible, 2nd Edition
// Richard S. Wright Jr.

#include <windows.h>
#include <gl/glut.h>

// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);

   	// Set current drawing color to red
	//		   R	 G	   B
	glColor3f(1.0f, 0.0f, 0.0f);

	// Draw a filled rectangle with current color
	glRectf(100.0f, 150.0f, 150.0f, 100.0f);

	// Flush drawing commands
    glFlush();
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

	// Establish clipping volume (left, right, bottom, top, near, far)
    if (w <= h) 
		glOrtho (0.0f, 250.0f, 0.0f, 250.0f*h/w, 1.0, -1.0);
    else 
		glOrtho (0.0f, 250.0f*w/h, 0.0f, 250.0f, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}

// Main program entry point
void main(void)
    {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("GLRect");
	glutDisplayFunc(RenderScene);
    glutReshapeFunc(ChangeSize);

	SetupRC();

	glutMainLoop();
    }

