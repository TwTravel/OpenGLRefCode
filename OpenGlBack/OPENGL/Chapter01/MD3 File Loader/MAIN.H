#ifndef _MAIN_H
#define _MAIN_H

#pragma warning(disable: 4786)

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <fstream>
#include <vector>
#include <gl\gl.h>	
#include <gl\glu.h>	
#include <gl\glaux.h>
#include <crtdbg.h>
using namespace std;

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 300
#define SCREEN_DEPTH 16	

#define MAX_TEXTURES 100								// 最大的纹理数目


// 定义3D点的类，用于保存模型中的顶点
class CVector3 
{
public:
	float x, y, z;
};

// 定义2D点类，用于保存模型的UV纹理坐标
class CVector2 
{
public:
	float x, y;
};

//  面的结构定义
struct tFace
{
	int vertIndex[3];			// 顶点索引
	int coordIndex[3];			// 纹理坐标索引
};

//  材质信息结构体
struct tMaterialInfo
{
	char  strName[255];			// 纹理名称
	char  strFile[255];			// 如果存在纹理映射，则表示纹理文件名称
	BYTE  color[3];				// 对象的RGB颜色

	int   texureId;				// 纹理ID
	float uTile;				// u 重复
	float vTile;				// v 重复
	float uOffset;			    // u 纹理偏移
	float vOffset;				// v 纹理偏移
} ;

//  对象信息结构体
struct t3DObject 
{
	int  numOfVerts;			// 模型中顶点的数目
	int  numOfFaces;			// 模型中面的数目
	int  numTexVertex;			// 模型中纹理坐标的数目
	int  materialID;			// 纹理ID
	bool bHasTexture;			// 是否具有纹理映射
	char strName[255];			// 对象的名称
	CVector3  *pVerts;			// 对象的顶点
	CVector3  *pNormals;		// 对象的法向量
	CVector2  *pTexVerts;		// 纹理UV坐标
	tFace *pFaces;				// 对象的面信息
};

//  模型信息结构体
struct t3DModel 
{
	int numOfObjects;					// 模型中对象的数目
	int numOfMaterials;					// 模型中材质的数目
	vector<tMaterialInfo> pMaterials;	// 材质链表信息
	vector<t3DObject> pObject;			// 模型中对象链表信息
	
	int numOfTags;						// 模型中标签的数目
	t3DModel	**pLinks;
	struct tMd3Tag		*pTags;
};


extern bool  g_bFullScreen;									
extern HWND  g_hWnd;										
extern RECT  g_rRect;										
extern HDC   g_hDC;											
extern HGLRC g_hRC;											
extern HINSTANCE g_hInstance;								

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow);
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
WPARAM MainLoop();

// 生成纹理，并将其保存在纹理队列中
void CreateTexture(UINT &texture, LPSTR strFileName);

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


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file stores all the structures needed to just load a Quake3 character.
// To animate the character, we will need to add a bit more structures and such.
// These structures should all be the same pretty much as the other loader tutorials.
//
// I would like to note that in our t3DModel structure, we put a "struct" in front
// of the tMD3Tag so that we don't get an error when compiling because it doesn't
// know what a tMD3Tag is yet.  You can think of this as a "forward declaration", where
// it allows you to use it, even if it's hasn't been defined yet.  If you have problems
// with this in your compiler, just rip the structure tMD3Tag out of md3.h and stick
// it right above the t3DModel structure.
//
// * What's An STL (Standard Template Library) Vector? *
//
// Let me quickly explain the STL vector for those of you who are not familiar with them.
// To use a vector you must include <vector> and use the std namespace: using namespace std;
// A vector is an array based link list.  It allows you to dynamically add and remove nodes.
// This is a template class so it can be a list of ANY type.  To create a vector of type
// "int" you would say:  vector<int> myIntList;
// Now you can add a integer to the dynamic array by saying: myIntList.push_back(10);
// or you can say:  myIntList.push_back(num);.  The more you push back, the larger
// your array gets.  You can index the vector like an array.  myIntList[0] = 0;
// To get rid of a node you use the pop_back() function.  To clear the vector use clear().
// It frees itself so you don't need to worry about it, except if you have data
// structures that need information freed from inside them, like our objects.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// The Quake3 .Md3 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// Lara Croft is a trademark of Eidos and should not be used for any commercial gain.
// All trademarks used are properties of their respective owners. 
//
//
