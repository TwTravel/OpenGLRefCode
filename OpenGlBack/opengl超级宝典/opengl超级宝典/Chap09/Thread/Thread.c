// Thread.c
// OpenGL SuperBible, Chapter 9
// Demonstrates rendering a single piece of a model (Bolt Threads)
// Program by Richard S. Wright Jr.

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <math.h>


// Define a constant for the value of PI
#define GL_PI 3.1415f

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
	{
	float length;
	
	// Calculate the length of the vector		
	length = (float)sqrt((vector[0]*vector[0]) + 
						(vector[1]*vector[1]) +
						(vector[2]*vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if(length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
	}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
	{
	float v1[3],v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y]*v2[z] - v1[z]*v2[y];
	out[y] = v1[z]*v2[x] - v1[x]*v2[z];
	out[z] = v1[x]*v2[y] - v1[y]*v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
	}


// Creates the thread that spirals along the body of the bolt
void RenderThread(void)
	{
	float x,y,z,angle;				// Calculate coordinates and step angle
	float height = 75.0f;			// Height of the threading
	float diameter = 20.0f;			// Diameter of the threading
	float normal[3],corners[4][3];	// Storeage for calculate normal and corners
	float step = (3.1415f/32.0f);	// one revolution
	float revolutions = 7.0f;		// How many time around the shaft
	float threadWidth = 2.0f;		// How wide is the thread
	float threadThick = 3.0f;		// How thick is the thread
	float zstep = .125f;			// How much does the thread move up
									// the Z axis each time a new segment
									// is drawn.

	// 360 degrees in radians
	#define PI2 (2.0f*3.1415f)

	// Set material color for head of screw
	glColor3f(0.0f, 0.0f, 0.4f);	
		

	z = -height/2.0f+2;	// Starting spot almost to the end

	// Go around and draw the sides until finished spinning up
	for(angle = 0.0f; angle < PI2*revolutions; angle += step)
		{
		// Calculate x and y position of the next vertex
		x = diameter*(float)sin(angle);
		y = diameter*(float)cos(angle);
	
		// Store the next vertex next to the shaft
		corners[0][0] = x;
		corners[0][1] = y;
		corners[0][2] = z;

		// Calculate the position away from the shaft
		x = (diameter+threadWidth)*(float)sin(angle);
		y = (diameter+threadWidth)*(float)cos(angle);

		corners[1][0] = x;
		corners[1][1] = y;
		corners[1][2] = z;

		// Calculate the next position away from the shaft
		x = (diameter+threadWidth)*(float)sin(angle+step);
		y = (diameter+threadWidth)*(float)cos(angle+step);

		corners[2][0] = x;
		corners[2][1] = y;
		corners[2][2] = z + zstep;

		// Calculate the next position along the shaft
		x = (diameter)*(float)sin(angle+step);
		y = (diameter)*(float)cos(angle+step);

		corners[3][0] = x;
		corners[3][1] = y;
		corners[3][2] = z+ zstep;
	

		// We'll be using triangels, so make 
		// counter clock-wise polygons face out
		glFrontFace(GL_CCW);		
		glBegin(GL_TRIANGLES);	// Start the top section of thread

			// Calculate the normal for this segment
			calcNormal(corners, normal);
			glNormal3fv(normal);

			// Draw two triangles to cover area
			glVertex3fv(corners[0]);
			glVertex3fv(corners[1]);
			glVertex3fv(corners[2]);

			glVertex3fv(corners[2]);
			glVertex3fv(corners[3]);
			glVertex3fv(corners[0]);

		glEnd();


		// Move the edge along the shaft slightly up the z axis
		// to represent the bottom of the thread
		corners[0][2] += threadThick;
		corners[3][2] += threadThick;

		// Recalculate the normal since points have changed, this
		// time it points in the opposite direction, so reverse it
		calcNormal(corners, normal);
		normal[0] = -normal[0];
		normal[1] = -normal[1];
		normal[2] = -normal[2];
				
		// Switch to clock-wise facing out for underside of the
		// thread.
		glFrontFace(GL_CW);

		// Draw the two triangles
		glBegin(GL_TRIANGLES);
			glNormal3fv(normal);

			glVertex3fv(corners[0]);
			glVertex3fv(corners[1]);
			glVertex3fv(corners[2]);

			glVertex3fv(corners[2]);
			glVertex3fv(corners[3]);
			glVertex3fv(corners[0]);

		glEnd();

		// Creep up the Z axis
		z += zstep;
		}
	}


// Called to draw scene
void RenderScene(void)
	{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	// Rotate about x and y axes
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 0.0f, 1.0f);

	// Render just the Thread of the nut
	RenderThread();

	glPopMatrix();

	// Swap buffers
	glutSwapBuffers();
	}


// This function does any needed initialization on the rendering
// context. 
void SetupRC()
	{
	// Light values and coordinates
	GLfloat  ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat  diffuseLight[] = {0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat	 lightPos[] = { -50.0f, 200.0f, 200.0f, 1.0f };
	GLfloat  specref[] =  { 0.6f, 0.6f, 0.6f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glEnable(GL_CULL_FACE);		// Do not calculate inside of solid object
	glFrontFace(GL_CCW);
	
	// Enable lighting
	glEnable(GL_LIGHTING);

	// Setup light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular);

	// Position and turn on the light
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// All materials hereafter have full specular reflectivity
	// with a moderate shine
	glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
	glMateriali(GL_FRONT,GL_SHININESS,64);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	}

///////////////////////////////////////////////////////////////////////////////
// Process arrow keys
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
		glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange*2.0f, nRange*2.0f);
    else 
		glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange*2.0f, nRange*2.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	}

int main(int argc, char* argv[])
	{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Bolt Thread");
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);
	SetupRC();
	glutMainLoop();

	return 0;
	}
