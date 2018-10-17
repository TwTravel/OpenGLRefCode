// Select.c
// OpenGL SuperBible, Chapter 14
// Demonstrates OpenGL Picking and Selection with feedback
// Program by Richard S. Wright Jr.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>



#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)

#define CUBE	1
#define SPHERE	2
 
RECT bRect;			// Bounding rectangle
GLuint nWho = 0;		// Who is selected
float fAspect;

// Application name and instance storeage
static LPCTSTR lpszAppName = "Select an Object";
static HINSTANCE hInstance;

// Lighting values
GLfloat  whiteLight[] = { 0.35f, 0.35f, 0.35f, 1.0f };
GLfloat  sourceLight[] = { 0.65f, 0.65f, 0.65f, 1.0f };
GLfloat	 lightPos[] = { -500.0f, 500.0f, 600.0f, 1.0f };



// Render the cube and sphere
void DrawObjects(void)
	{
	// Save the matrix state and do the rotations
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Translate the whole scene out and into view	
	glTranslatef(-80.0f, 0.0f, -300.0f);	

	// Initialize the names stack
	glInitNames();
	glPushName(0);

	// Set material color, Yellow
	// Cube
	glRGB(255, 255, 0);
	glLoadName(CUBE);
	glPassThrough((GLfloat)CUBE);
	glutSolidCube(75.0f);


	// Draw Sphere
	glRGB(128,0,0);
	glTranslatef(130.0f, 0.0f, 0.0f);
	glLoadName(SPHERE);
	glPassThrough((GLfloat)SPHERE);	
	glutSolidSphere(50.0f, 15, 15);

	// Restore the matrix state
	glPopMatrix();	// Modelview matrix
	}
	

// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Draw the objects in the scene
	DrawObjects();

	// If something is selected, draw a bounding box around it
	if(nWho != 0)
		{
		int viewport[4];
		
		// Get the viewport
		glGetIntegerv(GL_VIEWPORT, viewport);

		// Remap the viewing volume to match window coordinates (approximately)
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		
		// Establish clipping volume (left, right, bottom, top, near, far)
		glOrtho(viewport[0], viewport[2], viewport[3], viewport[1], -1, 1);
		glMatrixMode(GL_MODELVIEW);

		glDisable(GL_LIGHTING);
		glColor3ub(255,0,0);		
		glBegin(GL_LINE_LOOP);
			glVertex2i(bRect.left, bRect.top);
			glVertex2i(bRect.left, bRect.bottom);
			glVertex2i(bRect.right, bRect.bottom);
			glVertex2i(bRect.right, bRect.top);
		glEnd();
		glEnable(GL_LIGHTING);
		}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers();
	}




// Go into feedback mode and draw a rectangle around the object
#define FEED_BUFF_SIZE	4096
void MakeSelection(int nChoice)
	{
	// Space for the feedback buffer
	GLfloat feedBackBuff[FEED_BUFF_SIZE];

	// Storeage for counters, etc.
	int size,i,j,count;

	// Initial minimum and maximum values
	bRect.right = bRect.bottom = -999999.0f;
	bRect.left = bRect.top =  999999.0f;

	// Set the feedback buffer
	glFeedbackBuffer(FEED_BUFF_SIZE,GL_2D, feedBackBuff);

	// Enter feedback mode
	glRenderMode(GL_FEEDBACK);

	// Redraw the scene
	DrawObjects();

	// Leave feedback mode
	size = glRenderMode(GL_RENDER);

	// Parse the feedback buffer and get the
	// min and max X and Y window coordinates
	i = 0;
	while(i < size)
		{
		// Search for appropriate token
		if(feedBackBuff[i] == GL_PASS_THROUGH_TOKEN)
			if(feedBackBuff[i+1] == (GLfloat)nChoice)
			{
			i+= 2;
			// Loop until next token is reached
			while(i < size && feedBackBuff[i] != GL_PASS_THROUGH_TOKEN)
				{
				// Just get the polygons
				if(feedBackBuff[i] == GL_POLYGON_TOKEN)
					{
					// Get all the values for this polygon
					count = (int)feedBackBuff[++i]; // How many vertices
					i++;

					for(j = 0; j < count; j++)	// Loop for each vertex
						{
						// Min and Max X
						if(feedBackBuff[i] > bRect.right)
							bRect.right = feedBackBuff[i];

						if(feedBackBuff[i] < bRect.left)
							bRect.left = feedBackBuff[i];
						i++;

						// Min and Max Y
						if(feedBackBuff[i] > bRect.bottom)
							bRect.bottom = feedBackBuff[i];

						if(feedBackBuff[i] < bRect.top)
							bRect.top = feedBackBuff[i];
						i++;
						}
					}
				else
					i++;	// Get next index and keep looking
				}
			break;
			}
		i++;
		}
	}



// Process the selection, which is triggered by a right mouse
// click at (xPos, yPos).
#define BUFFER_LENGTH 64
void ProcessSelection(int xPos, int yPos)
	{
	// Space for selection buffer
	GLuint selectBuff[BUFFER_LENGTH];

	// Hit counter and viewport storeage
	GLint hits, viewport[4];

	// Setup selection buffer
	glSelectBuffer(BUFFER_LENGTH, selectBuff);
	
	// Get the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Switch to projection and save the matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Change render mode
	glRenderMode(GL_SELECT);

	// Establish new clipping volume to be unit cube around
	// mouse cursor point (xPos, yPos) and extending two pixels
	// in the vertical and horzontal direction
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, 2,2, viewport);

	// Apply perspective matrix 
	gluPerspective(60.0f, fAspect, 1.0, 425.0);

	// Draw the scene
	DrawObjects();

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);

	// If a single hit occured, display the info.
	if(hits == 1)
		{
		MakeSelection(selectBuff[3]);
		if(nWho == selectBuff[3])
			nWho = 0;
		else
			nWho = selectBuff[3];
		}

	glutPostRedisplay();
	}




// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
	{
	// Light values and coordinates
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	//glEnable(GL_CULL_FACE);		// Do not calculate insides

	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,whiteLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,sourceLight);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black blue background
	glClearColor(0.60f, 0.60f, 0.60f, 1.0f );
	glLineWidth(2.0f);
	}


void ChangeSize(int w, int h)
	{
	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Calculate aspect ratio of the window
	fAspect = (GLfloat)w/(GLfloat)h;

	// Set the perspective coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Field of view of 45 degrees, near and far planes 1.0 and 425
	gluPerspective(60.0f, fAspect, 1.0, 425.0);

	// Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}

// Process the mouse click
void MouseCallback(int button, int state, int x, int y)
	{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		ProcessSelection(x, y);
	}

int main(int argc, char* argv[])
	{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Select an Object");
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(MouseCallback);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();

	return 0;
	}
