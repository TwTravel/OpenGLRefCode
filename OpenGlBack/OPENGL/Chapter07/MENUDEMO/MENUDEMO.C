
#include <windows.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "texture.h"
#include "menu.h"


char *className = "OpenGL";
char *windowName = "OpenGL Menu Demo";
int winX = 0, winY = 0;
int winWidth = 400, winHeight = 300;
HDC hDC;
HDC hDCFrontBuffer;
HGLRC hGLRC;
unsigned *teximage;

//  装入纹理
void LoadTexture(char *fn, int t_num)
{
	int texwid, texht;
	int texcomps;

	teximage = read_texture(fn, &texwid, &texht, &texcomps);
	if (!teximage) return;
	glBindTexture(GL_TEXTURE_2D, texture[t_num]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texwid, texht, 0, GL_RGBA, GL_UNSIGNED_BYTE, teximage);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	free(teximage);
} 

void Init()
{
	GLfloat light_ambient[] = {0.7f, 0.0f, 0.7f, 1.0f};
	GLfloat light_diffuse[] = {0.7f, 0.0f, 0.7f, 1.0f};
	GLfloat light_specular[] = {0.7f, 0.0f, 0.7f, 1.0f};
	GLfloat light_position[] = {0.0f, 0.0f, -10.0f, 0.0f};

	glGenTextures(14, texture); 
	// 将所有纹理读入内存
	LoadTexture("textures/cursor.rgb", CURSOR_T);
	LoadTexture("textures/cursor_transp.rgb", CURSOR_TRANSP_T);
	LoadTexture("textures/border1.rgb", BORDER1_T);
	LoadTexture("textures/border2.rgb", BORDER2_T);
	LoadTexture("textures/border_up.rgb", BORDER3_T);
	LoadTexture("textures/pyramid.rgb", PYRAMID_T);
	LoadTexture("textures/cube.rgb", CUBE_T);
	LoadTexture("textures/sphere.rgb", SPHERE_T);
	LoadTexture("textures/cbox.rgb", CBOX_T);
	LoadTexture("textures/spindle.rgb", SPINDLE_T);
	LoadTexture("textures/torus.rgb", TORUS_T);
	LoadTexture("textures/line.rgb", LINE_T);
	LoadTexture("textures/quit.rgb", QUIT_T);
	// 装入模型文件
	LoadMFile("models/cube.m", &models[0]);
	LoadMFile("models/pyramid.m", &models[1]);
	LoadMFile("models/sphere.m", &models[2]);
	LoadMFile("models/spindle.m", &models[3]);
	LoadMFile("models/torus.m", &models[4]);
	LoadMFile("models/chamferbox.m", &models[5]);
	// 初始化菜单
	InitMenu();
	// 使用颜色混合
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	// 设置光照参数
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
}

void setProjection(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-0.6666, 0.6666, -0.5, 0.5, 2, 140);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void resize(void)
{
    glViewport(0, 0, winWidth, winHeight);
    setProjection();
}

void setupPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd = { 
	sizeof(PIXELFORMATDESCRIPTOR),    // pfd结构的大小 
	1,                                // 版本号 
	PFD_DRAW_TO_WINDOW |              // 支持在窗口中绘图 
	PFD_SUPPORT_OPENGL |              // 支持 OpenGL 
	PFD_DOUBLEBUFFER,                 // 双缓存模式 
	PFD_TYPE_RGBA,                    // RGBA 颜色模式 
	24,                               // 24 位颜色深度 
	0, 0, 0, 0, 0, 0,                 // 忽略颜色位 
	0,                                // 没有非透明度缓存 
	0,                                // 忽略移位位 
	0,                                // 无累加缓存 
	0, 0, 0, 0,                       // 忽略累加位 
	32,                               // 32 位深度缓存     
	0,                                // 无模板缓存 
	0,                                // 无辅助缓存 
	PFD_MAIN_PLANE,                   // 主层 
	0,                                // 保留 
	0, 0, 0                           // 忽略层,可见性和损毁掩模 
	}; 	

	int SelectedPixelFormat;
	BOOL retVal;

	pfd.cColorBits = GetDeviceCaps(hDC, BITSPIXEL);

	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags |= PFD_DOUBLEBUFFER;
	pfd.dwFlags |= PFD_DRAW_TO_WINDOW;

	SelectedPixelFormat = ChoosePixelFormat(hDC, &pfd);
	if (SelectedPixelFormat == 0) 
	{
		(void) MessageBox(WindowFromDC(hDC),
		"Failed to find acceptable pixel format.",
		"OpenGL application error",
		MB_ICONERROR | MB_OK);
		exit(1);
    }

	retVal = SetPixelFormat(hDC, SelectedPixelFormat, &pfd);
	if (retVal != TRUE)
	{
		(void) MessageBox(WindowFromDC(hDC),
		"Failed to set pixel format.",
		"OpenGL application error",
		MB_ICONERROR | MB_OK);
		exit(1);
	}
}

LRESULT APIENTRY WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
	static UINT timer;
	static int omx, omy, mx, my;

	switch (message)
	{
		case WM_CREATE:
			timer = SetTimer(hWnd, 1, 40, NULL);
			return 0;
		case WM_DESTROY:
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(hGLRC);
			KillTimer(hWnd, timer);
			PostQuitMessage(0);
			return 0;
		case WM_SIZE:
			if (hGLRC)
			{
				winWidth = (int) LOWORD(lParam);
				winHeight = (int) HIWORD(lParam);
				resize();
				return 0;
			}
		case WM_PAINT:
			if (hGLRC)
			{
				PAINTSTRUCT ps;
				BeginPaint(hWnd, &ps);
				doRedraw(&hDC);
				EndPaint(hWnd, &ps);
				return 0;
			}
			break;
		case WM_TIMER:
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;

		case WM_LBUTTONUP:  // 判断鼠标左键是否按下
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if  (mx & 1 << 15) mx -= (1 << 16);
			if  (my & 1 << 15) my -= (1 << 16);
			check_menu(1, mx, my, winWidth, winHeight, hWnd);
			return 0;

		case WM_MOUSEMOVE: // 判断鼠标是否移动
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if  (mx & 1 << 15) mx -= (1 << 16);
			if  (my & 1 << 15) my -= (1 << 16);
			check_menu(0, mx, my, winWidth, winHeight, hWnd);
			return 0;

		case WM_CHAR:
			switch ((int)wParam) 
			{
				case VK_ESCAPE: // 是否按下ESC键
					DestroyWindow(hWnd);
					return 0;
      			default:
					break;
			}
			break;
      default:
	    break;
   }

  return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY WinMain(
    HINSTANCE hCurrentInst,
    HINSTANCE hPreviousInst,
    LPSTR lpszCmdLine,
    int nCmdShow)
{
	WNDCLASS wndClass;
	HWND hWnd;
	MSG msg;
	HDC hdc;

	// 获取当前的分辨率
	hdc = GetDC(NULL);  

	wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hCurrentInst;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		className,
		windowName,	
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION ,
		winX, winY,
		winWidth, winHeight,
		NULL,
		NULL,
		hCurrentInst,
		NULL);

	ShowCursor(FALSE);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	hDC = GetDC(hWnd);
	hDCFrontBuffer = hDC;
	setupPixelFormat(hDC);
	hGLRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hGLRC);
	Init();
	setProjection();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	ReleaseDC(hWnd, hDC);
	return msg.wParam;
}
