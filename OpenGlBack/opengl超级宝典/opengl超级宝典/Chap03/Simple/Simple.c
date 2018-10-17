// Simple.c
// The Simplest OpenGL program with GLUT
// OpenGL SuperBible, 2nd Edition
// Richard S. Wright Jr.

#include <windows.h>
#include <gl/glut.h>

// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);


	// Flush drawing commands
    glFlush();
	}

// Setup the rendering state
void SetupRC(void)
    {
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    }


// Main program entry point
void main(void)
    {
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutCreateWindow("Simple");
	glutDisplayFunc(RenderScene);

	SetupRC();

	glutMainLoop();
    }

