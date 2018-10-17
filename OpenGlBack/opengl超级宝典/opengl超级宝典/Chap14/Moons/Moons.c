// Moons.c
// OpenGL SuperBible, Chapter 14
// Demonstrates OpenGL Hierarchical Selection/Picking
// Program by Richard S. Wright Jr.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>


#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)


#define EARTH	1
#define MARS	2
#define MOON1	3
#define MOON2	4

GLfloat fAspect;

// Lighting values
GLfloat  whiteLight[] = { 0.35f, 0.35f, 0.35f, 1.0f };
GLfloat  sourceLight[] = { 0.65f, 0.65f, 0.65f, 1.0f };
GLfloat	 lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };




// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	// Translate the whole scene out and into view	
	glTranslatef(0.0f, 0.0f, -300.0f);	

	// Initialize the names stack
	glInitNames();
	glPushName(0);

	
	// Draw the Earth
	glPushMatrix();
	glRGB(0,0,255);
	glTranslatef(-100.0f,0.0f,0.0f);
	glLoadName(EARTH);
	glutSolidSphere(30.0f, 15, 15);
	
	// Draw the Moon
	glTranslatef(45.0f, 0.0f, 0.0f);
	glRGB(220,220,220);
	glPushName(MOON1);
	glutSolidSphere(5.0f, 15, 15);glPopName();
	glPopMatrix();

	// Draw Mars
	glRGB(255,0,0);
	glPushMatrix();
	glTranslatef(100.0f, 0.0f, 0.0f);
	glLoadName(MARS);
	glutSolidSphere(20.0f, 15, 15);

	// Draw Moon1
	glTranslatef(-40.0f, 40.0f, 0.0f);
	glRGB(220,220,220);
	glPushName(MOON1);
	glutSolidSphere(5.0f, 15, 15);
	glPopName();

	// Draw Moon2
	glTranslatef(0.0f, -80.0f, 0.0f);
	glPushName(MOON2);
	glutSolidSphere(5.0f, 15, 15);	glPopName();
	glPopMatrix();

	// Restore the matrix state
	glPopMatrix();	// Modelview matrix

	glutSwapBuffers();
	}


// Parse the selection buffer to see which planet/moon was selected
void ProcessPlanet(GLuint *pSelectBuff)
	{
	int id,count;
	char cMessage[64];

	// How many names on the name stack
	count = pSelectBuff[0];

	// Bottom of the name stack
	id = pSelectBuff[3];

	// Select on earth or mars, whichever was picked
	switch(id)
		{
		case EARTH:
			strcpy(cMessage,"You clicked Earth.");

			// If there is another name on the name stack,
			// then it must be the moon that was selected
			// This is what was actually clicked on
			if(count == 2)
				strcat(cMessage,"\nSpecifically the moon.");

			break;

		case MARS:
			strcpy(cMessage,"You clicked Mars.");

			// We know the name stack is only two deep. The precise
			// moon that was selected will be here.
			if(count == 2)
				{
				if(pSelectBuff[4] == MOON1)
					strcat(cMessage,"\nSpecifically Moon #1.");
				else
					strcat(cMessage,"\nSpecifically Moon #2.");
				}
			break;
		
		// If nothing was clicked we shouldn't be here!
		default:
			strcpy(cMessage,"Error - Nothing was clicked on!");
			break;
		}

	// Display the message about planet and moon selection
	MessageBox(NULL,cMessage,"Selection Message",MB_OK);
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
	// in the vertical and horzontal direction. Remember OpenGL specifies the
	// y coordinate from the bottom, Windows from the top. So windows position
	// (as measured from the top) subtract the height and you get it in terms 
	// OpenGL Likes.
	glLoadIdentity();
	gluPickMatrix(xPos, viewport[3] - yPos, 2,2, viewport);

	// Apply perspective matrix 
	gluPerspective(45.0f, fAspect, 1.0, 425.0);

	// Draw the scene
	RenderScene();

	// Collect the hits
	hits = glRenderMode(GL_RENDER);

	// If a single hit occured, display the info.
	if(hits == 1)
		ProcessPlanet(selectBuff);

	// Restore the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Go back to modelview for normal rendering
	glMatrixMode(GL_MODELVIEW);
	}




// Process the mouse click
void MouseCallback(int button, int state, int x, int y)
	{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		ProcessSelection(x, y);
	}


// This function does any needed initialization on the rendering
// context. 
void SetupRC()
	{
	// Light values and coordinates
	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate insides

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
	gluPerspective(45.0f, fAspect, 1.0, 425.0);

	// Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}

int main(int argc, char* argv[])
	{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600,300);
	glutCreateWindow("Pick a Planet or Moon");
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(MouseCallback);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();

	return 0;
	}
