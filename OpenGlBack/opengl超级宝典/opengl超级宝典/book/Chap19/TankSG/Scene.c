// Scene.c
// OpenGL SuperBible 2nd Edition, Chapter 19
// Program by Richard S. Wright Jr.
// Part of the Tank example program. This code renders the 3D scene

///////////////////////////////////////////////////////////////////////////////
// Include Files
#include <windows.h>
#include <process.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "common.h"
#include "resource.h"

///////////////////////////////////////////////////////////////////////////////
// Variables shared between files (initialized in Tankx.c)
extern HDC hDC;								// Private GDI Device context
extern HWND hMainWindow;					// Main Application window
extern CRITICAL_SECTION		csThreadSafe;	// Critical section 
extern struct RENDER_STATE	rsRenderData;	// Communication between threads
extern LARGE_INTEGER timerFrequency;		// Timer Frequency

///////////////////////////////////////////////////////////////////////////////
// Locally global variables
static HGLRC hRC = NULL;		// Permenant Rendering context
static GLUquadricObj *pQuad;	// Quadric object
static LARGE_INTEGER lastTime;	// Time of last movement

struct CAMERA
	{
	float position[3];
	float orientation[3];
	} cameraData;

// Light values and coordinates
GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };

// Position of the sunlight...
GLfloat	 lightPos[] = { -0.5f, 1.0f, 0.3f, 0.0f };


// Four texture objects (ground, balls, pyramids, slabs, columns)
#define NUM_TEXTURES	4
#define TO_EARTH		0
#define TO_SPHERE		1
#define TO_COLUMN		2
#define TO_PYRAMID		3
GLuint tobjects[NUM_TEXTURES];

#define MODEL_COUNT   100
struct MODEL {
	float position[3];
	int nObjectType;
	} *pModelList;


// Convert degrees to radians
#define DEGTORAD(x) ((x)*0.017453292519943296)


///////////////////////////////////////////////////////////////////////////////
// Utility Functions
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// Update the position of the camera.This function can vary wildly depending
// on how motion is acheived. For a flight sim, motion will always be along the
// viewer's Z axis. For a first person walking around, motion will always be in
// the XY plane. Even if the camera tilts down (to look at the ground), or up
// (to look at the sky), the player still only moves along the XY plane.
// This function must be called continuously, or the timed motion will not work.
// If this function is called only when a key press is made, then the time between
// calls will be huge and the camera will 'rocket' to the new location with each key
// press
void UpdatePosition(struct CAMERA *pCamera)
	{
	float fLinearVelocity = 0.0f;
	float fAngularVelocityY = 0.0f;
	float fAngularVelocityX = 0.0f;
	float fAngularVelocityZ = 0.0f;
	float fTime,fAngle,fXDelta,fZDelta;
	JOYINFO ji;
	LARGE_INTEGER currentTime;

	// Check Joystick if present
	if(joyGetPos(JOYSTICKID1, &ji) == JOYERR_NOERROR)
		{
		// Button 1 moves forward, button 2 moves backwards
		if(ji.wButtons & JOY_BUTTON1)
			fLinearVelocity = 0.8f;

		if(ji.wButtons & JOY_BUTTON2)
			fLinearVelocity = -0.8f;

		// Check for spin left/right (Y Axis rotation)
		// Move quicker if joystick is moved hard
		if(ji.wXpos < 20000)
			fAngularVelocityY = -15.0f;
		if(ji.wXpos < 3000)
			fAngularVelocityY = -22.0f;

		if(ji.wXpos > 45536)
			fAngularVelocityY = 15.0f;
		if(ji.wXpos > 62536)
			fAngularVelocityY = 22.0f;

		// Check for looking up and down
		if(ji.wYpos < 20000)
			fAngularVelocityX = 14.0f;
		if(ji.wYpos < 3000)
			fAngularVelocityX = 21.0f;
		
		if(ji.wYpos > 45536)
			fAngularVelocityX = -14.0f;
		if(ji.wYpos > 63536)
			fAngularVelocityX = - 21.0f;
		}


	// Keyboard Input
	// Check for forward or backwards Motion
	if(GetAsyncKeyState(VK_UP))
		fLinearVelocity = 0.8f;

	if(GetAsyncKeyState(VK_DOWN))
		fLinearVelocity = -0.8f;

	// Check for spin left/right (Y Axis rotation)
	if(GetAsyncKeyState(VK_LEFT))
		fAngularVelocityY = -18.0f;

	if(GetAsyncKeyState(VK_RIGHT))
		fAngularVelocityY = 18.0f;

	// Check for looking up and down
	if(GetAsyncKeyState(0x41))		// A key
		fAngularVelocityX = 14.0f;
	
	if(GetAsyncKeyState(0x5a))		// Z key
		fAngularVelocityX = -14.0f;


	// Adjust position and orientation. Get the time since the last
	// check. If the velocity = 0 (no keypress or mouse movement)
	// then the motion will be nil...
	// D = vt
	QueryPerformanceCounter(&currentTime);
	fTime = (float)(currentTime.QuadPart - lastTime.QuadPart)/
				(float)timerFrequency.QuadPart;
	lastTime = currentTime;

	// Update Rotation angles (clamp the X rotation)
	fAngle = fTime * fAngularVelocityX;
	fAngle += pCamera->orientation[0];

	if((fAngle < 90.0f) && (fAngle > -90.0f))
		pCamera->orientation[0] = fAngle;

	pCamera->orientation[1] += fTime * fAngularVelocityY;
	
	// Update linear position
	fTime = fTime * fLinearVelocity;
	fXDelta = fTime * (float)(sin(DEGTORAD(pCamera->orientation[1])));
	fXDelta += pCamera->position[0];
	fZDelta = fTime * (float)(cos(DEGTORAD(pCamera->orientation[1])));
	fZDelta += pCamera->position[2];


	pCamera->position[0] = fXDelta;
	pCamera->position[2] = fZDelta;

	// Halt the camera at the boundaries of the Virtual World.
	// The maximum range for the person is +/- 95 from the center
	// The boundaries of the world are +/- 100 in all directions
	if(pCamera->position[0] > 95.0f)
		pCamera->position[0] = 95.0f;

	if(pCamera->position[0] < -95.0f)
		pCamera->position[0] = -95.0f;

	if(pCamera->position[2] > 95.0f)
		pCamera->position[2] = 95.0f;

	if(pCamera->position[2] < -95.0f)
		pCamera->position[2] = -95.0f;
	}


///////////////////////////////////////////////////////////////////////////////
// Load a bitmap from program resources (must be 24-bit color bitmap)
BYTE* gltResourceBMPBits(UINT nResource, int *nWidth, int *nHeight)
	{
	HINSTANCE hInstance;	// Instance Handle
	HANDLE hBitmap;			// Handle to bitmap resource
	BITMAPINFO bmInfo;
	BYTE *pData;

	// Find the bitmap resource
	hInstance = GetModuleHandle(NULL);
	hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(nResource));

	if(hBitmap == NULL)
		return NULL;

	GetObject(hBitmap,sizeof(BITMAPINFO),&bmInfo);
	DeleteObject(hBitmap);

	hBitmap = LoadResource(hInstance,
		 FindResource(hInstance,MAKEINTRESOURCE(nResource), RT_BITMAP));

	if(hBitmap == NULL)
		return NULL;

	pData = (BYTE *)LockResource(hBitmap);
	pData += sizeof(BITMAPINFO)-1;

	*nWidth = bmInfo.bmiHeader.biWidth; //bm.bmWidth;
	*nHeight = bmInfo.bmiHeader.biHeight;//bm.bmHeight;

	return pData;
	}


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


///////////////////////////////////////////////////////////////////////////////
// Resets the viewport. This function is called when the window is resized
void ResetViewport()
	{
	float fAspect;

	// Get the Window dimensions
	RECT rect;
	GetClientRect(hMainWindow, &rect);

	// Set the OpenGL viewport
	glViewport(0,0,rect.right, rect.bottom);

	if(rect.bottom == 0)
		fAspect = 1.0f;
	else
		fAspect = (float)rect.right/(float)rect.bottom;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(35.0f, fAspect, 0.5f, 50.0f);

	glMatrixMode(GL_MODELVIEW);
	}



///////////////////////////////////////////////////////////////////////////////
// Check rendering states and make appropriate adjustements. Returns true on
// termination flag.
BOOL CheckStates()
	{
	BOOL bRet = FALSE;

	// Is it time to leave or anything else
	EnterCriticalSection(&csThreadSafe);

	if(	rsRenderData.bModifyFlag)
		{
		// Flag set to terminate
		if(rsRenderData.bTerminate)
			bRet = TRUE;

		// Window resized
		if(rsRenderData.bResize)
			ResetViewport();

		// Reset the modify flag and exit
		rsRenderData.bModifyFlag = FALSE;
		}

	// Increment frame count
	rsRenderData.uiFrames++;

	LeaveCriticalSection(&csThreadSafe);

	return bRet;
	}

///////////////////////////////////////////////////////////////////////////////
// Pick a random value between -100.0 and 100.0
float RandomPos(void)
	{
	double dRet = 0.0f;

	dRet = (double)rand();			// between 0 and RAND_MAX
	dRet = dRet/(double)RAND_MAX;	// between 0 and 1.0
	dRet *= 200.0f;				// between 0 and 2000
	dRet -= 100.0f;				// between -1000 and 1000

	return (float)dRet;
	}


///////////////////////////////////////////////////////////////////////////////
// Initialize rendering context, etc.
void SetupRC(void)
	{
	int nPixelFormat;
	BYTE *pBytes;
	int nWidth, nHeight;
	int nIndex;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		32,								// Want 32 bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		16,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		0,	            				// Not used to select mode
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	// Set background clearing color to light blue
	glClearColor(0.1f, 0.1f, 0.9f, 1.0f);

	pQuad = gluNewQuadric();
	gluQuadricDrawStyle(pQuad, GLU_FILL);
	gluQuadricNormals(pQuad, GLU_TRUE);
	gluQuadricTexture(pQuad, GLU_TRUE);

	// Enable lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Setup and enable light 0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
	glEnable(GL_LIGHT0);

	// Enable color tracking
	glEnable(GL_COLOR_MATERIAL);
	
	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);


	// Set drawing color to white
	glColor3ub(255,255,255);


	// Camera initial position and orientation
	// Initially at Origin, slightly above the ground
	cameraData.position[0] = 0.0f;	
	cameraData.position[1] = -0.2f;
	cameraData.position[2] = -3.0f;

	// Looking down negative Z Axis (North). Positive X is to the right
	cameraData.orientation[0] = 0.0f;
	cameraData.orientation[1] = 0.0f;
	cameraData.orientation[2] = 0.0f;

   
	// Load Texture Objects
	glEnable(GL_TEXTURE_2D);
	glGenTextures(NUM_TEXTURES, tobjects);

	// Load the Ground
	pBytes = gltResourceBMPBits(IDB_GROUND, &nWidth, &nHeight);

	glBindTexture(GL_TEXTURE_2D, tobjects[TO_EARTH]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

	// Load the Sphere texture
	pBytes = gltResourceBMPBits(IDB_SPHERE, &nWidth, &nHeight);

	glBindTexture(GL_TEXTURE_2D, tobjects[TO_SPHERE]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);


	// Load the Column texture
	pBytes = gltResourceBMPBits(IDB_COLUMN, &nWidth, &nHeight);

	glBindTexture(GL_TEXTURE_2D, tobjects[TO_COLUMN]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

	// Load the Pyramid texture
	pBytes = gltResourceBMPBits(IDB_PYRAMID, &nWidth, &nHeight);

	glBindTexture(GL_TEXTURE_2D, tobjects[TO_PYRAMID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);


	// Populate the world
	pModelList = (struct MODEL *)malloc(sizeof(struct MODEL) * 3 * MODEL_COUNT);
	
	// Populate Spheres
	for(nIndex = 0; nIndex < MODEL_COUNT; nIndex++)
		{
		pModelList[nIndex].nObjectType = TO_SPHERE;
		pModelList[nIndex].position[0] = RandomPos();
		pModelList[nIndex].position[1] = 0.25;
		pModelList[nIndex].position[2] = RandomPos();
		}


	for(nIndex = MODEL_COUNT; nIndex < (MODEL_COUNT*2); nIndex++)
		{
		pModelList[nIndex].nObjectType = TO_COLUMN;
		pModelList[nIndex].position[0] = RandomPos();
		pModelList[nIndex].position[1] = 0.0;
		pModelList[nIndex].position[2] = RandomPos();
		}

	for(nIndex = MODEL_COUNT*2; nIndex < (MODEL_COUNT*3); nIndex++)
		{
		pModelList[nIndex].nObjectType = TO_PYRAMID;
		pModelList[nIndex].position[0] = RandomPos();
		pModelList[nIndex].position[1] = 0.0;
		pModelList[nIndex].position[2] = RandomPos();
		}
	}

///////////////////////////////////////////////////////////////////////////////
// Close down the rendering context
void ShutdownRC()
	{
	gluDeleteQuadric(pQuad);
	glDeleteTextures(NUM_TEXTURES, tobjects);
	free(pModelList);
	}

///////////////////////////////////////////////////////////////////////////////
// Draw a pyramid
void DrawPyramid(void)
	{
	GLfloat vNormal[3];
	GLfloat vvVertices[3][3];

	GLfloat fWidth = 0.5f;
	GLfloat fHeight = 0.75f;


	// Simple, build with four triangles.
	glBegin(GL_TRIANGLES);
		vvVertices[0][0] = fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = fWidth;

		vvVertices[1][0] = fWidth;
		vvVertices[1][1] = 0.0f;
		vvVertices[1][2] = -fWidth;

		vvVertices[0][0] = 0.0f;
		vvVertices[0][1] = fHeight;
		vvVertices[0][2] = 0.0f;
		calcNormal(vvVertices, vNormal);
		
		glNormal3fv(vNormal);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(fWidth, 0.0f, fWidth);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(fWidth, 0.0f, -fWidth);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(0.0f, fHeight, 0.0f);

		vvVertices[0][0] = fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = fWidth;

		vvVertices[1][0] = 0.0f;
		vvVertices[1][1] = fHeight;
		vvVertices[1][2] = 0.0f;

		vvVertices[0][0] = -fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = fWidth;
		calcNormal(vvVertices, vNormal);

		glNormal3fv(vNormal);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(fWidth, 0.0f, fWidth);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(0.0f, fHeight, 0.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(-fWidth, 0.0f, fWidth);

		vvVertices[0][0] = -fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = fWidth;

		vvVertices[1][0] = 0.0f;
		vvVertices[1][1] = fHeight;
		vvVertices[1][2] = 0.0f;

		vvVertices[0][0] = -fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = -fWidth;
		calcNormal(vvVertices, vNormal);

		glNormal3fv(vNormal);
		glTexCoord2f(1.0f,0.0f);		
		glVertex3f(-fWidth, 0.0f, fWidth);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(0.0f, fHeight, 0.0f);
		glTexCoord2f(0.0f,1.0f);		
		glVertex3f(-fWidth, 0.0f, -fWidth);


		vvVertices[0][0] = -fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = -fWidth;

		vvVertices[1][0] = 0.0f;
		vvVertices[1][1] = fHeight;
		vvVertices[1][2] = 0.0f;

		vvVertices[0][0] = fWidth;
		vvVertices[0][1] = 0.0f;
		vvVertices[0][2] = -fWidth;
		calcNormal(vvVertices, vNormal);

		glNormal3fv(vNormal);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(-fWidth, 0.0f, -fWidth);
		glTexCoord2f(1.0f,1.0f);		
		glVertex3f(0.0f, fHeight, 0.0f);
		glTexCoord2f(0.0f,1.0f);		
		glVertex3f(fWidth, 0.0f, -fWidth);
	glEnd();
	}


///////////////////////////////////////////////////////////////////////////////
// Called by rendering thread to draw scene
void RenderScene(void)
	{
	int nModel;

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the Ground
	// One big quad with a tiled texture
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, tobjects[TO_EARTH]);
	glBegin(GL_QUADS);
		glNormal3i(0, 1, 0);
		glTexCoord2f(10.0f, 10.0f);
		glVertex3i(100, 0, -100);
		glTexCoord2f(10.0f, 0.0f);
		glVertex3i(-100,0,-100);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3i(-100,0,100);
		glTexCoord2f(0.0f, 10.0f);
		glVertex3i(100, 0, 100);
	glEnd();

	// All other Geometry is lit,modulated
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_MODULATE);

	for(nModel = 0; nModel < (MODEL_COUNT * 3); nModel++)
		{
		glPushMatrix();
		glTranslatef(pModelList[nModel].position[0],
			pModelList[nModel].position[1],	pModelList[nModel].position[2]);

		switch(pModelList[nModel].nObjectType)
			{
			case TO_SPHERE:
				glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
				glBindTexture(GL_TEXTURE_2D, tobjects[TO_SPHERE]);
				gluSphere(pQuad, 0.25f, 20, 20);
				break;

			case TO_COLUMN:
				glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
				glBindTexture(GL_TEXTURE_2D, tobjects[TO_COLUMN]);
				gluCylinder(pQuad, 0.1f, 0.1f, 0.75, 15, 15);
				break;

			case TO_PYRAMID:
				glBindTexture(GL_TEXTURE_2D, tobjects[TO_PYRAMID]);
				DrawPyramid();
				break;

			}
		glPopMatrix();
		}
	}

///////////////////////////////////////////////////////////////////////////////
// Rendering Thread Entry Point
void RenderingThreadEntryPoint(void *pVoid)
	{
	// Setup the rendering context
	SetupRC();

	QueryPerformanceCounter(&lastTime);

	// Main loop for rendering
	while(1)
		{
		// Check rendering state info, terminate if flagged
		if(CheckStates())
			break;	

		// Get the new camera position and update the viewing transformation
		// accordingly
		UpdatePosition(&cameraData);	// Based on joystick/keyboard

		// Position lights, and camera location
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();	// Z-X-Y (Boom specific)

		// Place the light before placing the camera
		glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

		glRotatef(cameraData.orientation[2], 0.0f, 0.0f, 1.0f);
		glRotatef(cameraData.orientation[0], 1.0f, 0.0f, 0.0f);
		glRotatef(cameraData.orientation[1], 0.0f, 1.0f, 0.0f);
		glTranslatef(-cameraData.position[0],cameraData.position[1],cameraData.position[2]);

		RenderScene();

		// Swap buffers
		SwapBuffers(hDC);
		}

	// Cleanup/delete memory, etc.
	ShutdownRC();

	// Finally, shut down OpenGL Rendering context
	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(hRC);

	_endthread();	// Terminate the thread nicely
	}
