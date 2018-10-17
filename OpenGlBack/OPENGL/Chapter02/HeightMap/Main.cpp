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

BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];			// ����߳�����

bool  g_bRenderMode = true;						// ��Ⱦģʽ

UINT g_Texture[MAX_TEXTURES] = {0};				
//  ��λ�õ�����ID
#define BACK_ID		1
#define FRONT_ID	2
#define BOTTOM_ID	3
#define TOP_ID		4
#define LEFT_ID		5
#define RIGHT_ID	6


//  ����ĺ����Ĺ������������
void CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	// ʹ������
	glEnable(GL_TEXTURE_2D);

	// ����BACK�������
	glBindTexture(GL_TEXTURE_2D, g_Texture[BACK_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ��յ�λ��
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// ��ʼ����
	glBegin(GL_QUADS);		
		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// ����FRONT���ֵ��������
	glBindTexture(GL_TEXTURE_2D, g_Texture[FRONT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	// ��ʼ����
	glBegin(GL_QUADS);	
	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// ����BOTTOM���ֵ��������
	glBindTexture(GL_TEXTURE_2D, g_Texture[BOTTOM_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ��ʼ����
	glBegin(GL_QUADS);		
	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// ����TOP���ֵ��������
	glBindTexture(GL_TEXTURE_2D, g_Texture[TOP_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ��ʼ����
	glBegin(GL_QUADS);		
		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// ����LEFT���ֵ��������
	glBindTexture(GL_TEXTURE_2D, g_Texture[LEFT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ��ʼ����
	glBegin(GL_QUADS);		
		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// ����RIGHT���ֵ��������
	glBindTexture(GL_TEXTURE_2D, g_Texture[RIGHT_ID]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// ��ʼ����
	glBegin(GL_QUADS);		

		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}

//  ����ĺ��������ǳ�ʼ���������ݺ͸�����������
void Init(HWND hWnd)
{
	g_hWnd = hWnd;	
	GetClientRect(g_hWnd, &g_rRect);
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);

	//  ����"Terrain.raw"�ļ�����Ϊ�߳�����
	LoadRawFile("Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);	

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);	
	glEnable(GL_CULL_FACE);	
	//  װ�����е�����
	CreateTexture(g_Texture[0],			"Terrain.bmp"); 
	CreateTexture(g_Texture[BACK_ID],	"Back.bmp");
	CreateTexture(g_Texture[FRONT_ID],	"Front.bmp");
	CreateTexture(g_Texture[BOTTOM_ID], "Bottom.bmp");
	CreateTexture(g_Texture[TOP_ID],	"Top.bmp");	
	CreateTexture(g_Texture[LEFT_ID],	"Left.bmp");
	CreateTexture(g_Texture[RIGHT_ID],	"Right.bmp");

	// �����������λ��
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


//  ����ĺ��������ǻ��Ƴ���
void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();
	// ���������λ�úͷ���
	g_Camera.Look();

	// ��Ⱦ����
	RenderHeightMap(g_HeightMap);						

	// ������հ�Χ��
	CreateSkyBox(500, 0, 500, 2000, 2000, 2000);

	// ��õ�ǰ�������λ��
	CVector3 vPos		= g_Camera.Position();
	CVector3 vNewPos    = vPos;

	//  �ж�������Ƿ���ڵ�ƽ��
	if(vPos.y < Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10)
	{
		// �����µ�λ��
		vNewPos.y = (float)Height(g_HeightMap, (int)vPos.x, (int)vPos.z ) + 10;

		// ���y����Ĳ�
		float temp = vNewPos.y - vPos.y;

		//  ��õ�ǰ������ķ���
		CVector3 vView = g_Camera.View();
		vView.y += temp;

		// �����µ������λ��
		g_Camera.PositionCamera(vNewPos.x,  vNewPos.y,  vNewPos.z,
								vView.x,	vView.y,	vView.z,	0, 1, 0);								
	}

	// ����������
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

	case WM_LBUTTONDOWN:				// �������������
		
		g_bRenderMode = !g_bRenderMode;

		// �ı���Ⱦģʽ
		if(g_bRenderMode) 				
		{
			// ����ģʽ		
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
		}
		else 
		{
			// �߿�ģʽ
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

