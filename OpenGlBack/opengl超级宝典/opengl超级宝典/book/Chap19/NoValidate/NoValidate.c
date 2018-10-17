// NoValidate.c
// OpenGL SuperBible, Chapter 19
// Program by Richard S. Wright Jr.
// Demonstrates short circuiting the WM_PAINT handler

#include <windows.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"

HPALETTE hPalette = NULL;

GLUquadricObj *pSphere = NULL;

static LPCTSTR lpszAppName = "Timer";

// Declaration for Window procedure
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);


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


// Reset Viewport and viewing volume transormation
void ChangeSize(GLsizei w, GLsizei h)
	{
	GLfloat fAspect;

	// Prevent a divide by zero
	if(h == 0)
		h = 1;

	// Set Viewport to window dimensions
    glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    fAspect = (float)w/(float)h;
    gluPerspective(17.5, fAspect, 1.0, 200.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -200.0f);
	glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
	}

void SetupRC(void)
	{
	BYTE *pBytes;
	int nWidth, nHeight;

	// Set background clearing color to blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	pSphere = gluNewQuadric();
	gluQuadricDrawStyle(pSphere, GLU_FILL);
	gluQuadricNormals(pSphere, GLU_NONE);
	gluQuadricTexture(pSphere, GLU_TRUE);

	pBytes = gltResourceBMPBits(IDB_EARTH, &nWidth, &nHeight);

    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_DECAL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,nWidth, nHeight, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pBytes);

	}

// Called by AUX library to draw scene
void RenderScene(void)
	{
	static float fRot = 0.0f;
	fRot += 1.0f;
	if(fRot > 360.0f)
		fRot = 1.0f;

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT);
	
	glPushMatrix();
	glRotatef(fRot, 0.0f, 0.0f, 1.0f);
	gluSphere(pSphere, 20.0f, 15, 15);
	glPopMatrix();
	}



// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
	{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange,GreenRange,BlueRange;
								// Range for each color entry (7,7,and 3)


	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
		{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
		}
		

	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC,hRetPal,FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
	}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
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
	}



// Entry point of all Windows programs
int APIENTRY WinMain(	HINSTANCE 	hInstance,
						HINSTANCE 	hPrevInstance,
						LPSTR 		lpCmdLine,
						int			nCmdShow)
	{
	MSG			msg;		// Windows message structure
	WNDCLASS	wc;			// Windows class structure
	HWND		hWnd;		// Storeage for window handle


	// Register Window style
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= (WNDPROC) WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance 		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	
	// No need for background brush for OpenGL window
	wc.hbrBackground	= NULL;		
	
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= lpszAppName;

	// Register the window class
	if(RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
				lpszAppName,
				lpszAppName,
				
				// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
				WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
	
				// Window position and size
				100, 100,
				400, 400,
				NULL,
				NULL,
				hInstance,
				NULL);

	// If window was not created, quit
	if(hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd,SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while( GetMessage(&msg, NULL, 0, 0))
		{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	return msg.wParam;
	}



// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam)
	{
	static HGLRC hRC;		// Permenant Rendering context
	static HDC hDC;			// Private GDI Device context
	
	static LARGE_INTEGER timerFrequency;
	static LARGE_INTEGER startTime;
	static int nFrames = 0;

	switch (message)
	   	{
		// Window creation, setup for OpenGL
		case WM_CREATE:
			// Store the device context
			hDC = GetDC(hWnd);		

			// Select the pixel format
			SetDCPixelFormat(hDC);		

			// Create the rendering context and make it current
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);


			SetupRC();

			// Create the palette
			hPalette = GetOpenGLPalette(hDC);

			QueryPerformanceFrequency(&timerFrequency);
			QueryPerformanceCounter(&startTime);

			break;

		// Window is being destroyed, cleanup
		case WM_DESTROY:

			// Deselect the current rendering context and delete it
			gluDeleteQuadric(pSphere);
			wglMakeCurrent(hDC,NULL);
			wglDeleteContext(hRC);

			// Delete the palette
			if(hPalette != NULL)
				DeleteObject(hPalette);

			// Tell the application to terminate after the window
			// is gone.
			PostQuitMessage(0);
			break;

		// Window is resized.
		case WM_SIZE:
			// Call our function which modifies the clipping
			// volume and viewport
			ChangeSize(LOWORD(lParam), HIWORD(lParam));
			break;

		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
		case WM_PAINT:
			{
			// Call OpenGL drawing code
			RenderScene();

			// Call function to swap the buffers
			SwapBuffers(hDC);

			nFrames++;
			if(nFrames > 100)
				{
				char cOutBuffer[32];
				LARGE_INTEGER currentTime;
				float fps;

				// Get the current Time
				QueryPerformanceCounter(&currentTime);

				fps = (float)nFrames/((float)(currentTime.QuadPart - startTime.QuadPart)/
					(float)timerFrequency.QuadPart);

				// Display the fps
				sprintf(cOutBuffer,"%0.1f",fps);
				SetWindowText(hWnd, cOutBuffer);
				nFrames = 0;

				// Resets the timer
				QueryPerformanceCounter(&startTime);
				}
			}
			break;


		// Windows is telling the application that it may modify
		// the system palette.  This message in essance asks the 
		// application for a new palette.
		case WM_QUERYNEWPALETTE:
			// If the palette was created.
			if(hPalette)
				{
				int nRet;

				// Selects the palette into the current device context
				SelectPalette(hDC, hPalette, FALSE);

				// Map entries from the currently selected palette to
				// the system palette.  The return value is the number 
				// of palette entries modified.
				nRet = RealizePalette(hDC);

				// Repaint, forces remap of palette in current window
				InvalidateRect(hWnd,NULL,FALSE);

				return nRet;
				}
			break;

	
		// This window may set the palette, even though it is not the 
		// currently active window.
		case WM_PALETTECHANGED:
			// Don't do anything if the palette does not exist, or if
			// this is the window that changed the palette.
			if((hPalette != NULL) && ((HWND)wParam != hWnd))
				{
				// Select the palette into the device context
				SelectPalette(hDC,hPalette,FALSE);

				// Map entries to system palette
				RealizePalette(hDC);
				
				// Remap the current colors to the newly realized palette
				UpdateColors(hDC);
				return 0;
				}
			break;


        default:   // Passes it on if unproccessed
            return (DefWindowProc(hWnd, message, wParam, lParam));

        }

    return (0L);
	}


