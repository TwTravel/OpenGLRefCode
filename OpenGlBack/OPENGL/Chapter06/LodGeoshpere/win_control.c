#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "win_control.h"

#define DEF_SCREEN_WIDTH 400
#define DEF_SCREEN_HEIGHT 300

HWND hWnd;
HDC hDC;
HGLRC hRC;
char TheKey=0;
int TheSysKey=0;
BOOL b_done = 0;
float glAspect;
float fTime=0.f, fDeltaTime=0.f;
float fOldTime;
LARGE_INTEGER TimerFreq;	// ��ʱ��Ƶ��
LARGE_INTEGER TimeStart;	// ��ʼʱ��
LARGE_INTEGER TimeCur;		// ��ǰʱ��
int pixelwidth;
int pixelheight;

int SetUpOpenGL(HWND hWnd)
{
	int nMyPixelFormatID;
	static PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // pfd�ṹ�Ĵ�С 
	    1,                                // �汾�� 
	    PFD_DRAW_TO_WINDOW |              // ֧���ڴ����л�ͼ 
	    PFD_SUPPORT_OPENGL |              // ֧�� OpenGL 
	    PFD_DOUBLEBUFFER,                 // ˫����ģʽ 
	    PFD_TYPE_RGBA,                    // RGBA ��ɫģʽ 
	    24,                               // 24 λ��ɫ��� 
	    0, 0, 0, 0, 0, 0,                 // ������ɫλ 
	    0,                                // û�з�͸���Ȼ��� 
	    0,                                // ������λλ 
	    0,                                // ���ۼӻ��� 
	    0, 0, 0, 0,                       // �����ۼ�λ 
	    32,                               // 32 λ��Ȼ���     
	    0,                                // ��ģ�建�� 
	    0,                                // �޸������� 
	    PFD_MAIN_PLANE,                   // ���� 
	    0,                                // ���� 
	    0, 0, 0                           // ���Բ�,�ɼ��Ժ������ģ 
	}; 	

	hDC = GetDC(hWnd);

	nMyPixelFormatID = ChoosePixelFormat(hDC,&pfd);
	if(nMyPixelFormatID == 0)
	{
	    MessageBox(NULL, "ChoosePixelFormat() Failed" "Cannot Setup OpenGL", "Error", MB_OK);
	    return 0;
	}
	memset(&pfd,0,sizeof(PIXELFORMATDESCRIPTOR));
	DescribePixelFormat(hDC,nMyPixelFormatID,sizeof(PIXELFORMATDESCRIPTOR),&pfd);
	SetPixelFormat(hDC, nMyPixelFormatID,&pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC,hRC);

	return 1;
}

long FAR PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
		case WM_CREATE:
			if(!SetUpOpenGL(hWnd))
				PostQuitMessage(0);
			break;

		case WM_CHAR:
			TheKey = (TCHAR) wparam;
			break;
		case WM_KEYDOWN:
			TheSysKey = (int) wparam;
			break;
 		case WM_SIZE:
			pixelwidth = (GLsizei)LOWORD(lparam);
			pixelheight = (GLsizei)HIWORD(lparam);
			glAspect = (GLfloat)pixelwidth/(GLfloat)pixelheight;
			glViewport(0,0,pixelwidth,pixelheight);
			break;

		case WM_DESTROY:

			wglMakeCurrent(NULL,NULL);
			ReleaseDC(hWnd,hDC);
			wglDeleteContext(hRC);
			b_done  = TRUE;
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, message, wparam, lparam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	char MenuTitle[256];
	float title_bar_fps=0.0f;
	MSG msg;

	if(sscanf(lpCmdLine,"%d %d",&pixelwidth,&pixelheight)<2)
	{
		pixelwidth = DEF_SCREEN_WIDTH;
		pixelheight = DEF_SCREEN_HEIGHT;
	}

	hWnd = CreateWin(&msg,(WNDPROC)WindowProc);

	if(!hWnd)
		return 0;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	SetWindowText(hWnd,"Lod Geosphere");

	initialize();

	QueryPerformanceFrequency(&TimerFreq);
	QueryPerformanceCounter(&TimeStart);

	while(!b_done)
	{
		if(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		fOldTime = fTime;
		QueryPerformanceCounter(&TimeCur);
		fTime = (float)((double)(TimeCur.QuadPart-TimeStart.QuadPart)/
										(double)TimerFreq.QuadPart);
		fDeltaTime = fTime - fOldTime;

		if(title_bar_fps > 2.0f)
		{
			sprintf(MenuTitle,"LOD Geosphere(%.3f fps)",1.0f/fDeltaTime);
			title_bar_fps = 0.0f;
			SetWindowText(hWnd,MenuTitle);
		}
		title_bar_fps += fDeltaTime;


		key_check();
		draw_screen();
		SwapBuffers(hDC);
		TheKey = 0;
		TheSysKey = 0;
	}

	destroy();
	DestroyWindow(hWnd);
	PostQuitMessage(0);
	return msg.wParam;
}

HWND CreateWin(MSG *msg,WNDPROC WindowProc)
{
	WNDCLASS wc;
	HWND hWnd;
	HINSTANCE hInstance;

	hInstance = GetModuleHandle(NULL);
	wc.style =  CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, NULL);
	wc.hCursor       = LoadCursor(NULL,NULL);
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "lodgeo";

	if(!RegisterClass(&wc)) 
	{
		DWORD TheError;
		char buffer[256];
		TheError = GetLastError();
		sprintf(buffer,"Error : %d",TheError);
		MessageBox(NULL, "RegisterClass() failed:""Cannot register window class.", buffer, MB_OK);
		return NULL;
	}

	{
		int h; 
		int w; 
		h = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXDLGFRAME)*2 + pixelheight; 

		w = GetSystemMetrics(SM_CXDLGFRAME)*2 + pixelwidth;
		hWnd = CreateWindow("lodgeo","lodgeo",
		                    WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		                    50, 50, w,h,
                        NULL,NULL,hInstance,NULL);
	}
	if(hWnd == NULL)
	{
		DWORD TheError;
		char buffer[256];
		TheError = GetLastError();
		sprintf(buffer,"Error : %d",TheError);

		MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.", buffer, MB_OK);
		return NULL;
	}
	return hWnd; 
}
