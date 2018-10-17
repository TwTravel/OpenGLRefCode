#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

#include "main.h"
#include "Md3.h"

bool  g_bFullScreen = true;								
HWND  g_hWnd;											
RECT  g_rRect;											
HDC   g_hDC;											
HGLRC g_hRC;											
HINSTANCE g_hInstance;									

UINT g_Texture[MAX_TEXTURES] = {0};			


#define MODEL_PATH  "lara"
#define MODEL_NAME	"lara"
#define GUN_NAME    "Railgun"

CModelMD3 g_Model;

float g_RotateX		  =    0.0f;
float g_RotationSpeed =    0.1f;
float g_TranslationZ  = -120.0f;
bool  g_RenderMode    =    true;


void Init(HWND hWnd)
{
	g_hWnd = hWnd;										
	GetClientRect(g_hWnd, &g_rRect);					
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);
	
	bool bResult = g_Model.LoadModel(MODEL_PATH, MODEL_NAME);

	bResult = g_Model.LoadWeapon(MODEL_PATH, GUN_NAME);


	glEnable(GL_CULL_FACE);	
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

}


WPARAM MainLoop()
{
	MSG msg;

	while(1)											
	{													
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					
				break;
            TranslateMessage(&msg);						
            DispatchMessage(&msg);					
        }
		else									
		{ 
			RenderScene();						
        } 
	}

	DeInit();	
	return(msg.wParam);	
}



void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();	

	gluLookAt(	0, 5.5f, g_TranslationZ,	0, 5.5f, 0,			0, 1, 0);

	glRotatef(g_RotateX, 0, 1.0f, 0);
	g_RotateX += g_RotationSpeed;


	g_Model.DrawModel();


	SwapBuffers(g_hDC);		
}

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:							
		if(!g_bFullScreen)				
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hWnd, &g_rRect);				
		}
        break; 
 
	case WM_PAINT:									
		BeginPaint(hWnd, &ps);						
		EndPaint(hWnd, &ps);					
		break;

	case WM_KEYDOWN:

		switch(wParam) {	
			case VK_ESCAPE:						// ESC键
				PostQuitMessage(0);	
				break;

			case VK_LEFT:						// LEFT箭头键
				g_RotationSpeed -= 0.05f;
				break;

			case VK_RIGHT:						// RIGHT箭头键
				g_RotationSpeed += 0.05f;
				break;

			case VK_UP:							// UP箭头键
				g_TranslationZ += 2;
				break;

			case VK_DOWN:						// DOWN箭头键
				g_TranslationZ -= 2;
				break;

			case 'W':

				g_RenderMode = !g_RenderMode;	// 改变渲染模式

				if(g_RenderMode) 				
				{
	
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
				}
				else 
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
				}
				break;

		}
		break;

    case WM_CLOSE:
        PostQuitMessage(0);	
        break; 
     
    default:
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;	
}

