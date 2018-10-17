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
		fAngularVelocityY = -14.0f;

	if(GetAsyncKeyState(VK_RIGHT))
		fAngularVelocityY = 14.0f;

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
	//gluPerspective(45.0f, fAspect, 0.5f, 200.0f);
	gluPerspective(35.0f, fAspect, 0.5f, 200.0f);

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
// Initialize rendering context, etc.
void SetupRC(void)
	{
	int nPixelFormat;

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

	// Set background clearing color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	pQuad = gluNewQuadric();
	gluQuadricDrawStyle(pQuad, GLU_LINE);
	gluQuadricNormals(pQuad, GLU_NONE);

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

	}

///////////////////////////////////////////////////////////////////////////////
// Close down the rendering context
void ShutdownRC()
	{
	gluDeleteQuadric(pQuad);
	}


///////////////////////////////////////////////////////////////////////////////
// Called by rendering thread to draw scene
void RenderScene(void)
	{
	int r,c;

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the Ground
		// Just draw a bunch of horizontal and vertical lines
	glBegin(GL_LINES);

	for(r = -100; r  <= 100; r += 1)
		{
		glVertex3i(r, 0, -100);
		glVertex3i(r, 0, 100);
		}

	for(c = -100; c <= 100; c += 1)
		{
		glVertex3i(100, 0, c);
		glVertex3i(-100, 0, c);
		}
	glEnd();

	glPushMatrix();
	glTranslatef(0.0f, 0.5f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	gluSphere(pQuad, 0.5f, 15, 15);
	glPopMatrix();
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
