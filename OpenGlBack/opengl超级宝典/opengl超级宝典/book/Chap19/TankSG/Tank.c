// Tank.c
// OpenGL SuperBible 2nd Edition, Chapter 19
// Program by Richard S. Wright Jr.
// Demonstrates real-time reaction and camera movement

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "common.h"


HDC hDC = NULL;			// Private GDI Device context
HWND hMainWindow = NULL;// Main Application Window
LARGE_INTEGER timerFrequency; // High resolution timer frequency
	
static LARGE_INTEGER startTime;	// Time at start of simulation

CRITICAL_SECTION		csThreadSafe;	// Critical section 
struct RENDER_STATE		rsRenderData;	// Communication between threads


static LPCTSTR lpszAppName = "Tank Scene Graph";	// Application game

// Declaration for Window procedure
LRESULT CALLBACK WndProc(	HWND 	hWnd,
							UINT	message,
							WPARAM	wParam,
							LPARAM	lParam);




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
				0, 0,
				640, 480,
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
	
	hMainWindow = hWnd;


	// Process application messages until the application closes
	while(GetMessage(&msg, NULL, 0, 0))
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
	static HANDLE hThreadHandle = NULL;		// Thread Handle
	static int nLastFrames = 0;				// Frame count on last read

	switch (message)
	   	{
		// Window creation, setup for OpenGL
		case WM_CREATE:
			// Store the device context
			hDC = GetDC(hWnd);		

			// Initialize the critical section
			InitializeCriticalSection(&csThreadSafe);

			// Initialize interthread communication
			rsRenderData.uiFrames = 0;
			rsRenderData.bResize = FALSE;
			rsRenderData.bTerminate = FALSE;
			rsRenderData.bFatalError = FALSE;
			rsRenderData.bModifyFlag = TRUE;

			QueryPerformanceFrequency(&timerFrequency);
			QueryPerformanceCounter(&startTime);

			// Create the rendering thread
			hThreadHandle = (HANDLE) _beginthread(RenderingThreadEntryPoint,0,NULL);

			// This timer will be used to recalculate the frame rate every five seconds
			SetTimer(hWnd, 1, 5000, NULL); // Every Five Seconds
			break;

		// Updates frame rate every 5 seconds
		case WM_TIMER:
			{
			int nFrames;
			char cOutBuffer[32];
			LARGE_INTEGER currentTime;
			float fps;

			// Get the frame count
			EnterCriticalSection(&csThreadSafe);
			nFrames = rsRenderData.uiFrames;
			LeaveCriticalSection(&csThreadSafe);

			// Get the current Time
			QueryPerformanceCounter(&currentTime);

			fps = (float)(nFrames - nLastFrames)/((float)(currentTime.QuadPart - startTime.QuadPart)/
				(float)timerFrequency.QuadPart);

			// Display the fps
			sprintf(cOutBuffer,"TankSG %0.1f fps",fps);
			SetWindowText(hWnd, cOutBuffer);
			nLastFrames = nFrames;

			// Resets the timer
			QueryPerformanceCounter(&startTime);
			}
			break;


		case WM_CHAR:
			if(wParam == 27)	// Escape Key
				DestroyWindow(hWnd);

			break;


		// Tell the application to terminate after the window
		// is gone.
		case WM_DESTROY:
			{
			// Tell the rendering loop to terminate itself
			EnterCriticalSection(&csThreadSafe);
			rsRenderData.bTerminate = TRUE;
			rsRenderData.bModifyFlag = TRUE;
			LeaveCriticalSection(&csThreadSafe);

			// Need to wait for rendering thread to terminate so it can clean
			// up. Wait for the thread handle to become signaled. Give it five
			// seconds, then terminate anyway if it hasn't yet (it's probably
			// stuck if it hasn't terminated by now)
			WaitForSingleObject(hThreadHandle, 5000);

			DeleteCriticalSection(&csThreadSafe);

			ReleaseDC(hWnd, hDC);
			PostQuitMessage(0);
			}
			break;

		// Flag the rendering thread that a window change has occured
		case WM_MOVE:
		case WM_SIZE:
			EnterCriticalSection(&csThreadSafe);
			rsRenderData.bResize = TRUE;
			rsRenderData.bModifyFlag = TRUE;
			LeaveCriticalSection(&csThreadSafe);
			break;

		case WM_ERASEBKGND :
			return 1L;			// Return handled to prevent erasing background
			break;

        default:   // Passes it on if unproccessed
            return (DefWindowProc(hWnd, message, wParam, lParam));

        }

    return (0L);
	}



