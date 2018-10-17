#ifndef _MAIN_H
#define _MAIN_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>	
#include <gl\glu.h>	
#include <gl\glaux.h>
#include <math.h>


#include "Terrain.h"

#define MAX_TEXTURES 1000								// 最大的纹理数目

extern UINT g_Texture[MAX_TEXTURES];					// 保存纹理ID

#define GL_CLAMP_TO_EDGE	0x812F						// 保存天空纹理


#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300	
#define SCREEN_DEPTH 16	

extern bool  g_bFullScreen;									
extern HWND  g_hWnd;										
extern RECT  g_rRect;										
extern HDC   g_hDC;											
extern HGLRC g_hRC;											
extern HINSTANCE g_hInstance;								


// 下面的类是一个3D矢量类
class CVector3
{
public:
	
	// 缺省构造函数
	CVector3() {}

	// 用户构造函数
	CVector3(float X, float Y, float Z) 
	{ 
		x = X; y = Y; z = Z;
	}

	// 定义矢量之间的'+'法 
	CVector3 operator+(CVector3 vVector)
	{
		// 返回结果
		return CVector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	// 定义矢量之间的'-'法 
	CVector3 operator-(CVector3 vVector)
	{
		// 返回矢量相减的结果
		return CVector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	// 定义矢量与数的'*'法
	CVector3 operator*(float num)
	{
		// 返回结果
		return CVector3(x * num, y * num, z * num);
	}

	// 定义矢量与数的'/'法
	CVector3 operator/(float num)
	{
		// 返回结果
		return CVector3(x / num, y / num, z / num);
	}

	float x, y, z;						
};




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM MainLoop();

bool CreateTexture(UINT &texture, LPSTR strFileName);

// 改变显示为全屏模式
void ChangeToFullScreen();

// 生成用户窗口
HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance);

// 设置像素格式
bool bSetupPixelFormat(HDC hdc);

// 初始化投影变换
void SizeOpenGLScreen(int width, int height);

// 初始化OpenGL
void InitializeOpenGL(int width, int height);

// 初始化整个程序
void Init(HWND hWnd);

// 渲染场景
void RenderScene();

// 释放程序占用的内存空间
void DeInit();



#endif 



