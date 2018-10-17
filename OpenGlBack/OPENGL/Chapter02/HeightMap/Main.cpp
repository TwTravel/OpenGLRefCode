#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

#include "main.h"	
#include "camera.h"

bool  g_bFullScreen = true;								
HWND  g_hWnd;											
RECT  g_rRect;											
HDC   g_hDC;											
HGLRC g_hRC;											
HINSTANCE g_hInstance;									

CCamera g_Camera;

BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];			// 保存高程数据

bool  g_bRenderMode = true;						// 渲染模式

UINT g_Texture[MAX_TEXTURES] = {0};				
//  各位置的纹理ID
#define BACK_ID		1
#define FRONT_ID	2
#define BOTTOM_ID	3
#define TOP_ID		4
#define LEFT_ID		5
#define RIGHT_ID	6


//  下面的函数的功能是生成天空
void CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	// 使用纹理
	glEnable(GL_TEXTURE_2D);

	// 设置BACK纹理参数
	glBindTexture(GL_TEXTURE_2D, g_Texture[BACK_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 天空的位置
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// 开始绘制
	glBegin(GL_QUADS);		
		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// 设置FRONT部分的纹理参数
	glBindTexture(GL_TEXTURE_2D, g_Texture[FRONT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// 开始绘制
	glBegin(GL_QUADS);	
	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// 设置BOTTOM部分的纹理参数
	glBindTexture(GL_TEXTURE_2D, g_Texture[BOTTOM_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 开始绘制
	glBegin(GL_QUADS);		
	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// 设置TOP部分的纹理参数
	glBindTexture(GL_TEXTURE_2D, g_Texture[TOP_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 开始绘制
	glBegin(GL_QUADS);		
		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// 设置LEFT部分的纹理参数
	glBindTexture(GL_TEXTURE_2D, g_Texture[LEFT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 开始绘制
	glBegin(GL_QUADS);		
		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// 设置RIGHT部分的纹理参数
	glBindTexture(GL_TEXTURE_2D, g_Texture[RIGHT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// 开始绘制
	glBegin(GL_QUADS);		

		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}

//  下面的函数功能是初始化地形数据和各种纹理数据
void Init(HWND hWnd)
{
	g_hWnd = hWnd;	
	GetClientRect(g_hWnd, &g_rRect);
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);

	//  读入"Terrain.raw"文件，作为高程数据
	LoadRawFile("Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);	

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);	
	glEnable(GL_CULL_FACE);	
	//  装入所有的纹理
	CreateTexture(g_Texture[0],			"Terrain.bmp"); 
	CreateTexture(g_Texture[BACK_ID],	"Back.bmp");
	CreateTexture(g_Texture[FRONT_ID],	"Front.bmp");
	CreateTexture(g_Texture[BOTTOM_ID], "Bottom.bmp");
	CreateTexture(g_Texture[TOP_ID],	"Top.bmp");	
	CreateTexture(g_Texture[LEFT_ID],	"Left.bmp");
	CreateTexture(g_Texture[RIGHT_ID],	"Right.bmp");

	// 设置摄像机的位置
	g_Camera.PositionCamera( 280, 35, 225,  281, 35, 225,  0, 1, 0);

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
			g_Camera.Update();	
			RenderScene();	
        } 
	}
	
	DeInit();

	return(msg.wParam);	
}


//  下面的函数功能是绘制场景
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	// 设置摄像机位置和方向
	g_Camera.Look();

	// 渲染地形
	RenderHeightMap(g_HeightMap);						

	// 绘制天空包围盒
	CreateSkyBox(500, 0, 500, 2000, 2000, 2000);

	// 获得当前摄像机的位置
	CVector3 vPos		= g_Camera.Position();
	CVector3 vNewPos    = vPos;

	//  判断摄像机是否低于地平面
	if(vPos.y < Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10)
	{
		// 设置新的位置
		vNewPos.y = (float)Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10;

		// 获得y方向的差
		float temp = vNewPos.y - vPos.y;

		//  获得当前摄像机的方向
		CVector3 vView = g_Camera.View();
		vView.y += temp;

		// 设置新的摄像机位置
		g_Camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
								vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	// 交换缓冲区
	SwapBuffers(g_hDC);									
}


LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

	case WM_LBUTTONDOWN:				// 如果按下鼠标左键
		
		g_bRenderMode = !g_bRenderMode;

		// 改变渲染模式
		if(g_bRenderMode) 				
		{
			// 纹理模式		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
		else 
		{
			// 线框模式
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
		}
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:
				PostQuitMessage(0);	
				break;
		}
		break;
 
    case WM_CLOSE:			
        PostQuitMessage(0);	
        break; 
    } 
 
    return DefWindowProc (hWnd, uMsg, wParam, lParam); 	
}

