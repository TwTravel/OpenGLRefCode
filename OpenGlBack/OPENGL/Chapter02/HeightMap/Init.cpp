
#include "main.h"


bool CreateTexture(UINT &texture, LPSTR strFileName)
{
	AUX_RGBImageRec *pImage = NULL;
	FILE *pFile = NULL;

	if(!strFileName) 
		return false;

	// 以只读模式打开文件 
	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		// 如果文件无法打开，则显示错误信息
		MessageBox(g_hWnd, "Unable to load BMP File!", "Error", MB_OK);
		return NULL;
	}

	// 装入位图
	pImage = auxDIBImageLoad(strFileName);				

	// 确保位图数据已经装入
	if(pImage == NULL)								
		return false;

	// 生成纹理
	glGenTextures(1, &texture);

	// 设置像素格式
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// 捆绑纹理
	glBindTexture(GL_TEXTURE_2D, texture);
	
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, 
					  pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//  释放位图数据占据的内存资源
	if (pImage)	
	{
		if (pImage->data)	
		{
			free(pImage->data);
		}

		free(pImage);	
	}

	// 返回true
	return true;
}


void ChangeToFullScreen()
{
	DEVMODE dmSettings;								

	memset(&dmSettings,0,sizeof(dmSettings));		

	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;		
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;	
	
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	if(result != DISP_CHANGE_SUCCESSFUL)
	{

		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));			
	wndclass.style = CS_HREDRAW | CS_VREDRAW;		
	wndclass.lpfnWndProc = WinProc;				
	wndclass.hInstance = hInstance;				
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);	
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);
	wndclass.lpszClassName = "GameTutorials";		

	RegisterClass(&wndclass);						
	
	if(bFullScreen && !dwStyle) 					
	{												
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();					
		ShowCursor(FALSE);						
	}
	else if(!dwStyle)							
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;						

	RECT rWindow;
	rWindow.left	= 0;				
	rWindow.right	= width;			
	rWindow.top	    = 0;					
	rWindow.bottom	= height;					
	AdjustWindowRect( &rWindow, dwStyle, false);	

												
	hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, 0, 0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;					

	ShowWindow(hWnd, SW_SHOWNORMAL);	
	UpdateWindow(hWnd);					

	SetFocus(hWnd);					

	return hWnd;
}

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			
    pfd.nVersion = 1;								
													
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;			
    pfd.iPixelType = PFD_TYPE_RGBA;				
    pfd.cColorBits = SCREEN_DEPTH;				
    pfd.cDepthBits = SCREEN_DEPTH;				
    pfd.cAccumBits = 0;						
    pfd.cStencilBits = 0;				
 
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;							
}

void SizeOpenGLScreen(int width, int height)		
{
	if (height==0)										
	{
		height=1;										
	}

	glViewport(0,0,width,height);						

	glMatrixMode(GL_PROJECTION);		
	glLoadIdentity();						
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 4 ,4000.0f);

	glMatrixMode(GL_MODELVIEW);						
	glLoadIdentity();									
}

void InitializeOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);							
											
    if (!bSetupPixelFormat(g_hDC))			
        PostQuitMessage (0);					

    g_hRC = wglCreateContext(g_hDC);			
    wglMakeCurrent(g_hDC, g_hRC);			

	glEnable(GL_TEXTURE_2D);				
	glEnable(GL_DEPTH_TEST);				

	SizeOpenGLScreen(width, height);	
}

void DeInit()
{
	if (g_hRC)											
	{
		wglMakeCurrent(NULL, NULL);						
		wglDeleteContext(g_hRC);						
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);						
		
	if(g_bFullScreen)									
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);							
	}

	UnregisterClass("GameTutorials", g_hInstance);	

	PostQuitMessage (0);							
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;

	// 判断用户是否需要全屏显示
	if(MessageBox(NULL, "Click Yes to go to full screen (Recommended)", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
		g_bFullScreen = false;
	
	hWnd = CreateMyWindow(" 地形模拟 ", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	if(hWnd == NULL) return true;

	// 初始化OpenGL
	Init(hWnd);													
	return MainLoop();						
}



