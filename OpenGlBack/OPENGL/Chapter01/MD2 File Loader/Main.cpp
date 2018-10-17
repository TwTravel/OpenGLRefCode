
#include "main.h"										
#include "MD2.h"										

bool  g_bFullScreen = true;								
HWND  g_hWnd;											
RECT  g_rRect;											
HDC   g_hDC;											
HGLRC g_hRC;											
HINSTANCE g_hInstance;	
				
#define FILE_NAME  "tris.md2"							
#define TEXTURE_NAME "rhino.bmp"

UINT g_Texture[MAX_TEXTURES] = {0};						

CLoadMD2 g_LoadMd2;										
t3DModel g_3DModel;									

int   g_ViewMode	  = GL_TRIANGLES;			
bool  g_bLighting     = true;				
float g_RotateX		  = 0.0f;				
float g_RotationSpeed = 0.5f;	


void Init(HWND hWnd)
{
	g_hWnd = hWnd;										
	GetClientRect(g_hWnd, &g_rRect);					
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);

	g_LoadMd2.ImportMD2(&g_3DModel, FILE_NAME, TEXTURE_NAME);		


	// 遍历所有的材质
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// 判断是否是一个文件名
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{
			//  使用纹理文件名称来装入位图
			CreateTexture(g_Texture, g_3DModel.pMaterials[i].strFile, i);			
		}

		// 设置材质的纹理ID
		g_3DModel.pMaterials[i].texureId = i;
	}

	glEnable(GL_LIGHT0);								
	glEnable(GL_LIGHTING);								
	glEnable(GL_COLOR_MATERIAL);					

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

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

	// 遍历场景中所有的对象
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// 删除所有的变量
		delete [] g_3DModel.pObject[i].pFaces;
		delete [] g_3DModel.pObject[i].pNormals;
		delete [] g_3DModel.pObject[i].pVerts;
		delete [] g_3DModel.pObject[i].pTexVerts;
	}

	DeInit();	
	return(msg.wParam);	
}

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glLoadIdentity();	
	gluLookAt(		0, 1.5f, 90,		0, 0.5f, 0,			0, 1, 0);
	
	glRotatef(g_RotateX, 0, 1.0f, 0);
	g_RotateX += g_RotationSpeed;	


	if(g_3DModel.pObject.size() <= 0) return;

	// 获得当前显示的对象
	t3DObject *pObject = &g_3DModel.pObject[0];

	glBegin(g_ViewMode);

		// 遍历对象中所有的面
		for(int j = 0; j < pObject->numOfFaces; j++)
		{
			// 遍历三角形中的每个顶点
			for(int whichVertex = 0; whichVertex < 3; whichVertex++)
			{
				int index = pObject->pFaces[j].vertIndex[whichVertex];

				int index2 = pObject->pFaces[j].coordIndex[whichVertex];
			
				glNormal3f(-pObject->pNormals[ index ].x, -pObject->pNormals[ index ].y, -pObject->pNormals[ index ].z);
					
				// 判断是否有纹理坐标
				if(pObject->pTexVerts) 
				{
					glTexCoord2f(pObject->pTexVerts[ index2 ].x, pObject->pTexVerts[ index2 ].y);
				}
				
				glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
			}
		}

	glEnd();

	SwapBuffers(g_hDC);									// 交换缓冲区
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

	case WM_LBUTTONDOWN:								// 按下鼠标左键，改变绘制模式
		
		if(g_ViewMode == GL_TRIANGLES) {		
			g_ViewMode = GL_LINE_STRIP;		
		} else {
			g_ViewMode = GL_TRIANGLES;	
		}
		break;

	case WM_RBUTTONDOWN:								// 按下鼠标右键，改变光照模式
		
		g_bLighting = !g_bLighting;		

		if(g_bLighting) {					
			glEnable(GL_LIGHTING);			
		} else {
			glDisable(GL_LIGHTING);			
		}
		break;

	case WM_KEYDOWN:									// 键盘响应

		switch(wParam) {								
			case VK_ESCAPE:								// 按下ESC键
				PostQuitMessage(0);					
				break;

			case VK_LEFT:								// 按下向左键
				g_RotationSpeed -= 0.05f;	
				break;

			case VK_RIGHT:								// 按下向右键
				g_RotationSpeed += 0.05f;			
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


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial shows how to load a .Md2 file.  This is a good addition to an engine.
// In the next tutorial we will show you how to handle the key frame animation.
// I think this is the best and easiest 3D file format I have worked with, especially
// for animation.
// 
// * What's An STL (Standard Template Library) Vector? *
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
// Once again I should point out that the coordinate system of OpenGL and .Md2 files are different.
// Since Quake2 Models have the Z-Axis pointing up (strange and ugly I know! :), 
// we need to flip the y values with the z values in our vertices.  That way it
// will be normal, with Y pointing up.  Also, because we swap the Y and Z we need to negate 
// the Z to make it come out correctly.  This is also explained and done in ReadMd2Data().
//
// I would like to thank Daniel E. Schoenblum <dansch@hops.cs.jhu.edu> for help
// with explaining the file format.  You can check it out at:  
// 
// http://www.ugrad.cs.jhu.edu/~dansch/md2/#model_magic
//
// I would also like to thank John at Eurogamer.net for allowing us to use his Rhino model.
//
// Let me know if this helps you out!
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// The Quake2 .Md2 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written conset
// from ID Software.
//
// Quake and Quake2 are trademarks of id Software.
// All trademarks used are properties of their respective owners. 
//
//
