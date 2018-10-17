// BilliardView.cpp : implementation of the CBilliardView class
//

#include "stdafx.h"
#include "Billiard.h"

#include "BilliardDoc.h"
#include "BilliardView.h"
#include "model.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBilliardView

IMPLEMENT_DYNCREATE(CBilliardView, CView)

BEGIN_MESSAGE_MAP(CBilliardView, CView)
	//{{AFX_MSG_MAP(CBilliardView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBilliardView construction/destruction

CBilliardView::CBilliardView()
{
	// TODO: add construction code here
	g_Dragging = FALSE;					//  鼠标的初始状态不是拖动状态
	g_DrawingStick = FALSE;				//  球杆在初始时不随场景一起绘制
	g_LastTime	=	0;
	g_SimRunning =	TRUE;				//  初始时即进入游戏
	g_TimeIterations = 10;
	g_UseFixedTimeStep = FALSE;			//  不使用固定的时间步长
	g_MaxTimeStep = 0.01f;
	g_CollisionRootFinding = FALSE;	
	g_UseDamping = TRUE;				// 初始时应用阻尼
	g_UseFriction = TRUE;				// 初始时应用摩擦
	g_CueHitBall = FALSE;				// 初始时球杆没有击中球
	g_BallInPlay = FALSE;				// 初始时没有球在运动

}

CBilliardView::~CBilliardView()
{
}

BOOL CBilliardView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
////////////////////////////////////////////////////////////////
//设置窗口类型
	cs.style |=WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
////////////////////////////////////////////////////////////////
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBilliardView drawing

void CBilliardView::OnDraw(CDC* pDC)
{
	CBilliardDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
//////////////////////////////////////////////////////////////////
	RenderWorld();	//渲染场景
//////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////
// CBilliardView printing

BOOL CBilliardView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBilliardView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBilliardView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CBilliardView diagnostics

#ifdef _DEBUG
void CBilliardView::AssertValid() const
{
	CView::AssertValid();
}

void CBilliardView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBilliardDoc* CBilliardView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBilliardDoc)));
	return (CBilliardDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBilliardView message handlers

int CBilliardView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
//////////////////////////////////////////////////////////////////
//初始化OpenGL和设置定时器
	m_pDC = new CClientDC(this);
	SetTimer(1, 20, NULL);
	InitializeOpenGL(m_pDC);
//////////////////////////////////////////////////////////////////
	SetupViewRC();
	g_LastTime = GetTime();	
	InitRender();
	if(!InitGame())
		return -1;
	return 0;
}

void CBilliardView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
/////////////////////////////////////////////////////////////////
//删除调色板和渲染上下文、定时器
	::wglMakeCurrent(0,0);
	::wglDeleteContext( m_hRC);
	if (m_hPalette)
	    DeleteObject(m_hPalette);
	if ( m_pDC )
	{
		delete m_pDC;
	}
	KillTimer(1);		
/////////////////////////////////////////////////////////////////
	
}

void CBilliardView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if(cy == 0)		  // 避免被0除
		cy = 1;
	double dAspect = (double)cx/(double)cy;
	// 设置视口
    glViewport(0, 0, cx, cy);
	// 设置投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 设置投影变换
	gluPerspective(60.0, dAspect,0.2f, 2000);
	//  设置模型矩阵
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void CBilliardView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
/////////////////////////////////////////////////////////////////
//添加定时器响应函数和场景更新函数
	Invalidate(FALSE);	
/////////////////////////////////////////////////////////////////
	
	CView::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////
//	                  设置逻辑调色板
//////////////////////////////////////////////////////////////////////
void CBilliardView::SetLogicalPalette(void)
{
    struct
    {
        WORD Version;
        WORD NumberOfEntries;
        PALETTEENTRY aEntries[256];
    } logicalPalette = { 0x300, 256 };

	BYTE reds[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE greens[] = {0, 36, 72, 109, 145, 182, 218, 255};
	BYTE blues[] = {0, 85, 170, 255};

    for (int colorNum=0; colorNum<256; ++colorNum)
    {
        logicalPalette.aEntries[colorNum].peRed =
            reds[colorNum & 0x07];
        logicalPalette.aEntries[colorNum].peGreen =
            greens[(colorNum >> 0x03) & 0x07];
        logicalPalette.aEntries[colorNum].peBlue =
            blues[(colorNum >> 0x06) & 0x03];
        logicalPalette.aEntries[colorNum].peFlags = 0;
    }

    m_hPalette = CreatePalette ((LOGPALETTE*)&logicalPalette);
}


//////////////////////////////////////////////////////////
//						初始化openGL场景
//////////////////////////////////////////////////////////
BOOL CBilliardView::InitializeOpenGL(CDC* pDC)
{
	m_pDC = pDC;
	SetupPixelFormat();
	//生成绘制描述表
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	//置当前绘制描述表
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	return TRUE;
}

//////////////////////////////////////////////////////////
//						设置像素格式
//////////////////////////////////////////////////////////
BOOL CBilliardView::SetupPixelFormat()
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
	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);//选择像素格式
	::SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd);	//设置像素格式
	if(pfd.dwFlags & PFD_NEED_PALETTE)
		SetLogicalPalette();	//设置逻辑调色板
	return TRUE;
}



//////////////////////////////////////////////////////////
//						场景绘制与渲染
//////////////////////////////////////////////////////////
BOOL CBilliardView::RenderWorld() 
{
	float Time;
	float DeltaTime;

    if (g_UseFixedTimeStep)		//  如果使用固定步长
		Time = g_LastTime + (g_MaxTimeStep * g_TimeIterations);
	else
		Time = GetTime();

	if (g_SimRunning)
	{
		while(g_LastTime < Time)
		{
			DeltaTime = Time - g_LastTime;
			if(DeltaTime > g_MaxTimeStep)
			{
				DeltaTime = g_MaxTimeStep;
			}

	 		Simulate(DeltaTime,g_SimRunning);
			g_LastTime += DeltaTime;
		}
		g_LastTime = Time;
	}
	else
	{
		DeltaTime = 0;
		Simulate(DeltaTime,g_SimRunning);
	}

////////////////////////////////////////////////////////////////
	//  清除颜色缓存和深度缓存
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	g_CueStick.yaw = -g_POV.rot.y;		// 设置球杆始终位于视场中心
	//  设置摄像机平移
	glTranslatef(-g_POV.trans.x, -g_POV.trans.y, -g_POV.trans.z);
	//  设置摄像机旋转
	glRotatef(g_POV.rot.z, 0.0f, 0.0f, 1.0f);
	glRotatef(g_POV.rot.x, 1.0f, 0.0f, 0.0f); 
	glRotatef(g_POV.rot.y, 0.0f, 1.0f, 0.0f);
	glTranslatef(-g_CueStick.pos.x, -g_CueStick.pos.y, -g_CueStick.pos.z);
	RenderScene();				//  绘制静止的场景
	RenderCueAndBalls();		//  绘制球杆和球
	glPopMatrix();
	::SwapBuffers(m_pDC->GetSafeHdc());		//交互缓冲区
	return TRUE;
}

void CBilliardView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	g_Dragging = TRUE;				//  按下鼠标左键，将鼠标设置为拖动状态
	g_LastYaw = g_POV.rot.y;		//  保存摄像机绕Y轴旋转的位置
	g_LastPitch = g_POV.rot.x;		//  保存摄像机绕X轴旋转的位置
	g_MouseHitX = point.x;			//  鼠标单击的X坐标 
	g_MouseHitY = point.y;			//  鼠标单击的Y坐标 
	// 设置球杆的位置
	g_CueStick.pos.x = g_CurrentSys->pos.x;
	g_CueStick.pos.z = g_CurrentSys->pos.z;
	SetCapture();	
	CView::OnLButtonDown(nFlags, point);
}

void CBilliardView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	g_Dragging = FALSE;		//  释放鼠标左键，解除鼠标的拖动状态
	ReleaseCapture();
	
	CView::OnLButtonUp(nFlags, point);
}

void CBilliardView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	tx = point.x;		//  记下当前鼠标的X坐标 
	ty = point.y;		//  记下当前鼠标的Y坐标 
	if (g_Dragging)		//  当鼠标处于拖动状态
	{
		if (tx != g_MouseHitX)
		{
			//  摄像机绕Y轴旋转
			g_POV.rot.y = g_LastYaw + (float)(tx - g_MouseHitX);
			Invalidate(FALSE);;
		}
		if (ty != g_MouseHitY)
		{
			//  摄像机绕X轴旋转
			g_POV.rot.x = g_LastPitch + (float)(ty - g_MouseHitY);
			if (g_POV.rot.x < 0.0f) 
				g_POV.rot.x = 0.0f;			
			if (g_POV.rot.x > 90.0f) 
				g_POV.rot.x = 90.0f;			
			Invalidate(FALSE);;
		}

	}
	// 如果球杆处于绘制状态(即拖动是鼠标右键),并且球的运动没有结束
	if (g_DrawingStick && !g_BallInPlay)
	{
		if (ty != g_MouseHitY)
		{
			g_CueStick.draw = g_LastDraw + ((float)(ty - g_MouseHitY) * 0.1f);
			// 向后移动球杆
			if (g_LastDraw < g_CueStick.draw)
			{
				g_CueStick.old_draw = g_CueStick.draw;
				g_CueStick.drawtime = GetTime();
			}
			// 如果球杆接触到球，则击中球
			else if (g_CueStick.draw < 0.0f)	
			{

				g_CueHitBall = TRUE;				// 设置球击中标志为TRUE
				magnitude = -CUE_STICK_FORCE * ((g_CueStick.old_draw - g_CueStick.draw) / (GetTime() - g_CueStick.drawtime));
				g_CueForce.x = magnitude * sin(DEGTORAD(g_CueStick.yaw));							
				g_CueForce.z = magnitude * cos(DEGTORAD(g_CueStick.yaw));							
				g_CueStick.draw = 0.2f;			
				g_BallInPlay = TRUE;			//  击中球后，球开始运动
				g_DrawingStick = FALSE;			//  不绘制球杆
			}
			Invalidate(FALSE);;
		}
	}	
	CView::OnMouseMove(nFlags, point);
}

void CBilliardView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	g_DrawingStick = TRUE;					//  绘制球杆
	g_LastDraw = g_CueStick.draw;			//  保存球杆与球之间的距离
	g_CueStick.old_draw = g_CueStick.draw;	//  保存球杆与球之间的距离
	g_MouseHitX = point.x;					//  鼠标单击的X坐标 
	g_MouseHitY = point.y;					//  鼠标单击的Y坐标 
	// 重新设置球杆的位置
	g_CueStick.pos.x = g_CurrentSys->pos.x;
	g_CueStick.pos.z = g_CurrentSys->pos.z;
	SetCapture();	
	CView::OnRButtonDown(nFlags, point);
}

void CBilliardView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	g_DrawingStick = FALSE;		//  不绘制球杆
	ReleaseCapture();
	
	CView::OnRButtonUp(nFlags, point);
}

void CBilliardView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
		case VK_UP:
		{
			if (g_POV.rot.x > 0.0f) 
				g_POV.rot.x -= 1.0;		//  绕X轴负方向旋转		
			Invalidate(FALSE);;
			break;
		}
		case VK_DOWN: 
		{
			if (g_POV.rot.x < 90.0f) 
				g_POV.rot.x += 1.0;		//  绕X轴正方向旋转
			Invalidate(FALSE);;
			break;
		}
		case VK_LEFT:		
		{
			g_POV.rot.y += 1.0;			//  绕Y轴正方向旋转		
			Invalidate(FALSE);;
			break;
		}
		
		case VK_RIGHT:	
		{
			g_POV.rot.y -= 1.0;			//  绕Y轴负方向旋转		
			Invalidate(FALSE);;
			break;
		}
		case VK_PRIOR:		// 场景放大
		{
			if (g_POV.trans.z > 1.0f)
				//  摄像机向z轴的负方向平移
				g_POV.trans.z -= 1.0;			
			Invalidate(FALSE);;
			break;
		}
		case VK_NEXT:		// 场景缩小
		{
			if (g_POV.trans.z < 9.0f) 
				//  摄像机向z轴方向平移
				g_POV.trans.z += 1.0;			
			Invalidate(FALSE);;
			break;
		}
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

//  下面的函数的功能是对游戏进行初始化
BOOL CBilliardView::InitGame(void)
{
	g_CueStick.draw = g_CueStick.old_draw = 0.2f;
	g_CueStick.yaw = 0.0f;
	g_CueStick.pos.x = -4.0f;
	g_CueStick.pos.y = TABLE_POSITION;
	g_CueStick.pos.z = 0.0f;
	g_Kd	= 0.02f;		// 阻尼因子
	g_Kr_Bumper	= 0.8f;		// 球与边界之间碰撞的恢复系数
	g_Kr_Ball	= 0.2f;		// 球与球之间碰撞的恢复系数
	g_Csf	= 0.2f;			// 缺省的静摩擦
	g_Ckf	= 0.1f;			// 缺省的动摩擦
	MAKEVECTOR(g_Gravity, 0.0f, -32.0f, 0.0f)
	g_IntegratorType = EULER_INTEGRATOR;
	g_CollisionRootFinding = FALSE;		// ONLY SET WHEN FINDING A COLLISION
	g_ContactCnt = 0;
	SetupBalls();
	g_CurrentSys = g_GameSys[0];
	g_TargetSys = g_GameSys[1];
	g_CollisionPlaneCnt = 4;
	// 左边的碰撞平面
	MAKEVECTOR(g_CollisionPlane[0].normal,1.0f, 0.0f, 0.0f)
	g_CollisionPlane[0].d = LEFT_BUMPER;
	// 右边的碰撞平面
	MAKEVECTOR(g_CollisionPlane[1].normal,-1.0f, 0.0f, 0.0f)
	g_CollisionPlane[1].d = RIGHT_BUMPER;
	// 上边的碰撞平面
	MAKEVECTOR(g_CollisionPlane[2].normal, 0.0f, 0.0f, -1.0f)
	g_CollisionPlane[2].d = TOP_BUMPER;
	// 下边的碰撞平面
	MAKEVECTOR(g_CollisionPlane[3].normal, 0.0f, 0.0f, 1.0f)
	g_CollisionPlane[3].d = BOTTOM_BUMPER;
	return TRUE;
}


void CBilliardView::InitRender(void)
{
	int loop;
	// 初始化摄像机的位置
	g_POV.trans.x = 0.0f;
	g_POV.trans.y = 0.0f;
	g_POV.trans.z = 8.0f;
	g_POV.rot.x = 20.0f;
	g_POV.rot.y = 90.0f;
	g_POV.rot.z = 0.0f;
	//  定义白球的显示列表
	glNewList(OGL_WBALL_DLIST,GL_COMPILE);
		// 定义数据序列
		glInterleavedArrays(WBALLFORMAT,0,(GLvoid *)&WBALLMODEL);
		glBegin(GL_TRIANGLES);
		for (loop = 0; loop < WBALLPOLYCNT * 3; loop++)
		{
			glArrayElement(loop);
		}
		glEnd();
	glEndList();
	//  定义黄球的显示列表
	glNewList(OGL_YBALL_DLIST,GL_COMPILE);
		// 定义数据序列
		glInterleavedArrays(YBALLFORMAT,0,(GLvoid *)&YBALLMODEL);
		glBegin(GL_TRIANGLES);
		for (loop = 0; loop < YBALLPOLYCNT * 3; loop++)
		{
			glArrayElement(loop);
		}
		glEnd();
	glEndList();
	//  定义球杆的显示列表
	glNewList(OGL_CUE_DLIST,GL_COMPILE);
		// 定义数据序列
		glInterleavedArrays(CUEFORMAT,0,(GLvoid *)&CUEMODEL);
		glBegin(GL_TRIANGLES);
		for (loop = 0; loop < CUEPOLYCNT * 3; loop++)
		{
			glArrayElement(loop);
		}
		glEnd();
	glEndList();

	LoadTextures();

	LoadSceneFile("Pool.ros");			// 装入场景数据
}

//  下面的函数的功能是将所有的TGA图像数据读入到纹理中
void CBilliardView::LoadTextures()
{
	GLubyte	*rgb;
	char texName[80];
	tTGAHeader_s tgaHeader;
	HANDLE specHandle;
	WIN32_FIND_DATA	fileData;
	int rv;

	g_TextureCnt = 0;
	sprintf(texName,"%s*.tga",ART_PATH);
	if ((specHandle=FindFirstFile(texName,&fileData))!= INVALID_HANDLE_VALUE)
	{
		do
		{
			sprintf(g_TexPool[g_TextureCnt].map,"%s%s",ART_PATH,fileData.cFileName);
			glGenTextures(1,&g_TexPool[g_TextureCnt].glTex);

			rgb = m_LoadTGA.LoadTGAFile( g_TexPool[g_TextureCnt].map,&tgaHeader);
			if (rgb == NULL)
			{
				MessageBox("Unable to Open File...",g_TexPool[g_TextureCnt].map,MB_OK);
				g_TexPool[g_TextureCnt].glTex = 0;
				return;
			}

			glBindTexture(GL_TEXTURE_2D, g_TexPool[g_TextureCnt].glTex);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			//  定义2D纹理像素格式
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);	/* Force 4-byte alignment */
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
			glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

			if (tgaHeader.d_pixel_size == 32)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, 4, tgaHeader.d_width, tgaHeader.d_height, 0,
						 GL_RGBA , GL_UNSIGNED_BYTE, rgb);
				rv = gluBuild2DMipmaps( GL_TEXTURE_2D, 4, tgaHeader.d_width, tgaHeader.d_height, 
					GL_RGBA, GL_UNSIGNED_BYTE, rgb );
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, 3, tgaHeader.d_width, tgaHeader.d_height, 0,
						 GL_RGB, GL_UNSIGNED_BYTE, rgb);
				rv = gluBuild2DMipmaps( GL_TEXTURE_2D, 3, tgaHeader.d_width, tgaHeader.d_height, GL_RGB, 
					GL_UNSIGNED_BYTE, rgb );
			}
			//  释放位图和RGB图像数据
			free(rgb);

			g_TextureCnt++;
		}while (FindNextFile(specHandle,&fileData));
		FindClose(specHandle);
	}
}

void CBilliardView::LoadSceneFile(char *filename)
{
	FILE *fp;
	char tempstr[80];
	t_Visual	*visual;

	fp = fopen(filename,"rb");
	if (fp != NULL)
	{
		fread(tempstr,1,4,fp);
		if (strncmp(tempstr,"ROSC",4)!= 0)
		{
			MessageBox("Not a Valid Data File","Load File", MB_OK|MB_ICONEXCLAMATION);
			return;
		}

		visual = &g_Scene;
		fread(&visual->vertexCnt,sizeof(long),1,fp);
		visual->vertex = (tVector *)malloc(sizeof(tVector) * visual->vertexCnt);
		fread(visual->vertex,sizeof(tVector),visual->vertexCnt,fp);
		fread(&visual->triCnt,sizeof(long),1,fp);
		visual->tri = (tPrimPoly *)malloc(sizeof(tPrimPoly) * (visual->triCnt));
		fread(visual->tri,sizeof(tPrimPoly),visual->triCnt,fp);
		fread(&visual->quadCnt,sizeof(long),1,fp);
		visual->quad = (tPrimPoly *)malloc(sizeof(tPrimPoly) * (visual->quadCnt));
		fread(visual->quad,sizeof(tPrimPoly),visual->quadCnt,fp);
	
		fclose(fp);
	}
}

//  设置OpenGL绘制环境
void CBilliardView::SetupViewRC(void)
{
	// 设置黑色背景
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	// 使用深度测试
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	// 无光照
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glPolygonMode(GL_FRONT,GL_FILL);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}


void CBilliardView::RenderCueAndBalls()
{
	// 绘制白球
	glPushMatrix();
		glTranslatef(g_CurrentSys[0].pos.x,g_CurrentSys[0].pos.y,g_CurrentSys[0].pos.z);
		glCallList(OGL_WBALL_DLIST);
	glPopMatrix();

	// 绘制黄球
	glPushMatrix();
		glTranslatef(g_CurrentSys[1].pos.x,g_CurrentSys[1].pos.y,g_CurrentSys[1].pos.z);
		glCallList(OGL_YBALL_DLIST);
	glPopMatrix();

	// 绘制球杆
	glPushMatrix();
		//  球杆的位置
		glTranslatef(g_CueStick.pos.x,g_CueStick.pos.y,g_CueStick.pos.z);
		//  绕Y轴旋转
		glRotatef(g_CueStick.yaw, 0.0f, 1.0f, 0.0f); 
		//  绕X轴旋转一个固定角度，即略向上抬
		glRotatef(-5.0f, 1.0f, 0.0f, 0.0f);
		//  击球时向后移动球杆
		glTranslatef(0,0,g_CueStick.draw);
		glCallList(OGL_CUE_DLIST);
	glPopMatrix();
}

void CBilliardView::RenderScene()
{
    int loop,loop2;
	t_Visual *visual;
	tPrimPoly *poly;

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	visual = &g_Scene;
	for (loop2 = 0; loop2 < g_TextureCnt; loop2++)
	{
		poly = visual->quad;
		glBindTexture(GL_TEXTURE_2D, g_TexPool[loop2].glTex);
		
		for (loop = 0; loop < visual->quadCnt; loop++)
		{
			if ((poly->type & POLY_TEXTURED) > 0 && poly->TexNdx1 == (uint)loop2)
			{		
				glBegin(GL_QUADS);
				glTexCoord2fv((float *)&poly->t1[0]);
				glColor3ubv((unsigned char *)&poly->color[0]);
				glVertex3fv((float *)&visual->vertex[poly->index[0]]);
				glTexCoord2fv((float *)&poly->t1[1]);
				glColor3ubv((unsigned char *)&poly->color[1]);
				glVertex3fv((float *)&visual->vertex[poly->index[1]]);
				glTexCoord2fv((float *)&poly->t1[2]);
				glColor3ubv((unsigned char *)&poly->color[2]);
				glVertex3fv((float *)&visual->vertex[poly->index[2]]);
				glTexCoord2fv((float *)&poly->t1[3]);
				glColor3ubv((unsigned char *)&poly->color[3]);
				glVertex3fv((float *)&visual->vertex[poly->index[3]]);
				glEnd();
			}
			poly++;
		}

		poly = visual->tri;
		for (loop = 0; loop < visual->triCnt; loop++)
		{
			if ((poly->type & POLY_TEXTURED) > 0 && poly->TexNdx1 == (uint)loop2)
			{		
				glBegin(GL_TRIANGLES);
				glTexCoord2fv((float *)&poly->t1[0]);
				glColor3ubv((unsigned char *)&poly->color[0]);
				glVertex3fv((float *)&visual->vertex[poly->index[0]]);
				glTexCoord2fv((float *)&poly->t1[1]);
				glColor3ubv((unsigned char *)&poly->color[1]);
				glVertex3fv((float *)&visual->vertex[poly->index[1]]);
				glTexCoord2fv((float *)&poly->t1[2]);
				glColor3ubv((unsigned char *)&poly->color[2]);
				glVertex3fv((float *)&visual->vertex[poly->index[2]]);
				glEnd();
			}
			poly++;
		}
	}
	glDisable(GL_TEXTURE_2D);
}

void CBilliardView::SetupBalls()
{
	int loop, loop2;
	for (loop = 0; loop < SYSTEM_COUNT; loop++)
	{
		for (loop2 = 0; loop2 < BALL_COUNT; loop2++)
		{	
			switch (loop2)
			{
			// 球1是球杆需要击中的球
			case 0:
				MAKEVECTOR(g_GameSys[loop][loop2].pos,-4.0f, TABLE_POSITION, 0.0f)
				break;
			case 1:
				MAKEVECTOR(g_GameSys[loop][loop2].pos,4.0f, TABLE_POSITION, 0.0f)
				break;
			}
			MAKEVECTOR(g_GameSys[loop][loop2].v,0.0f, 0.0f, 0.0f)
			MAKEVECTOR(g_GameSys[loop][loop2].f,0.0f, 0.0f, 0.0f)
			g_GameSys[loop][loop2].oneOverM = 1.0f / 0.34f;
			MAKEVECTOR(g_GameSys[loop][loop2].angMom,0.0f, 0.0f, 0.0f)
			MAKEVECTOR(g_GameSys[loop][loop2].torque,0.0f, 0.0f, 0.0f)

			// 四元数清零
			MAKEVECTOR(g_GameSys[loop][loop2].orientation,0.0f, 0.0f, 0.0f)
			g_GameSys[loop][loop2].orientation.w = 1.0f;

			g_GameSys[loop][loop2].flags = 0;
			g_GameSys[loop][loop2].flags = 0;
		}
	}
}


#define STATIC_THRESHOLD	0.03f	//  速度的阈值，用于判断使用何种摩擦			

void CBilliardView::ComputeForces( t_Ball	*system)
{
	int loop;
	t_Ball		*curBall;
	tVector		contactN;
	float		FdotN,VdotN,Vmag;		
	tVector		Vn,Vt;	

	curBall = system;
	for (loop = 0; loop < BALL_COUNT; loop++)
	{
		MAKEVECTOR(curBall->f,0.0f,0.0f,0.0f)			// 清除力
		MAKEVECTOR(curBall->torque,0.0f, 0.0f, 0.0f)	// 清除力矩

		if (g_UseDamping)	//  如果用户定义阻尼
		{
			curBall->f.x += (-g_Kd * curBall->v.x);
			curBall->f.y += (-g_Kd * curBall->v.y);
			curBall->f.z += (-g_Kd * curBall->v.z);
		}
		else	//  否则使用缺省阻尼
		{
			curBall->f.x += (-DEFAULT_DAMPING * curBall->v.x);
			curBall->f.y += (-DEFAULT_DAMPING * curBall->v.y);
			curBall->f.z += (-DEFAULT_DAMPING * curBall->v.z);
		}

		//  下面处理球与桌面之间的摩擦问题
		if (g_UseFriction)
		{
			// 计算正压力，即重力
			FdotN = g_Gravity.y / curBall->oneOverM;
			// 计算垂直于法向平面的速度
			MAKEVECTOR(contactN,0.0f, 1.0f, 0.0f)
			VdotN = m_MathDefs.DotProduct(&contactN,&curBall->v);
			m_MathDefs.ScaleVector(&contactN, VdotN, &Vn);
			m_MathDefs.VectorDifference(&curBall->v, &Vn, &Vt);
			Vmag = m_MathDefs.VectorSquaredLength(&Vt);
			// 判断速度是否大于阈值
			if (Vmag > STATIC_THRESHOLD)		// 大于阈值，使用动摩擦模型
			{
				m_MathDefs.NormalizeVector(&Vt);
				m_MathDefs.ScaleVector(&Vt, (FdotN * g_Ckf), &Vt);
				m_MathDefs.VectorSum(&curBall->f,&Vt,&curBall->f);
			}
			else	// 小于阈值，使用静摩擦模型
			{
				Vmag = Vmag / STATIC_THRESHOLD;
				m_MathDefs.NormalizeVector(&Vt);
				m_MathDefs.ScaleVector(&Vt, (FdotN * g_Csf * Vmag), &Vt);  // Scale Friction by Velocity
				m_MathDefs.VectorSum(&curBall->f,&Vt,&curBall->f);
				if (loop == 0)								
				{
					g_BallInPlay = FALSE;
				}
			}
		}

		curBall++;
	}

	//  用户是否定义了球杆撞击球的力
	if (g_CueHitBall)
	{
		m_MathDefs.VectorSum(&system[0].f,&g_CueForce,&system[0].f);
	}
}   


void CBilliardView::IntegrateSysOverTime(t_Ball *initial,t_Ball *source, t_Ball *target, float deltaTime)
{
	int loop;
	float deltaTimeMass;

	for (loop = 0; loop < BALL_COUNT; loop++)
	{
		deltaTimeMass = deltaTime * initial->oneOverM;
		// 获得新的速度
		target->v.x = initial->v.x + (source->f.x * deltaTimeMass);
		target->v.y = initial->v.y + (source->f.y * deltaTimeMass);
		target->v.z = initial->v.z + (source->f.z * deltaTimeMass);

		target->oneOverM = initial->oneOverM;

		// 获得新的位置坐标
		target->pos.x = initial->pos.x + (deltaTime * source->v.x);
		target->pos.y = initial->pos.y + (deltaTime * source->v.y);
		target->pos.z = initial->pos.z + (deltaTime * source->v.z);

		initial++;
		source++;
		target++;
	}
}

//  欧拉积分
void CBilliardView::EulerIntegrate( float DeltaTime)
{
	IntegrateSysOverTime(g_CurrentSys,g_CurrentSys, g_TargetSys,DeltaTime);
}

//  中点方法
void CBilliardView::MidPointIntegrate( float DeltaTime)
{
	float		halfDeltaT;

	halfDeltaT = DeltaTime / 2.0f;
	//  计算半步长
	IntegrateSysOverTime(g_CurrentSys,g_CurrentSys,&g_GameSys[2][0],halfDeltaT);
	//  使用新的位置和速度计算力
	ComputeForces(&g_GameSys[2][0]);
	//  计算全步长
	IntegrateSysOverTime(g_CurrentSys,&g_GameSys[2][0],g_TargetSys,DeltaTime);
}

int CBilliardView::CheckForCollisions( t_Ball	*system )
{
	int collisionState = NOT_COLLIDING;
    float const depthEpsilon = 0.001f;
    float const ballDepthEpsilon = 0.0001f;
	int loop,loop2,planeIndex;
	t_Ball *curBall,*ball2;
	t_CollisionPlane *plane;
	float axbyczd,dist,relativeVelocity;
	tVector	distVect;
	g_ContactCnt = 0;		// 目前的碰撞次数为0
	curBall = system;
	for (loop = 0; 
		(loop < BALL_COUNT) && (collisionState != PENETRATING); 
		loop++,curBall++)
	{
		//  判断球与边界面之间的碰撞
        for(planeIndex = 0;
			(planeIndex < g_CollisionPlaneCnt) && (collisionState != PENETRATING);
			planeIndex++)
        {
			plane = &g_CollisionPlane[planeIndex];

            axbyczd = m_MathDefs.DotProduct(&curBall->pos,&plane->normal) + plane->d;

            if(axbyczd < -depthEpsilon)
            {
				// 任何球穿过边界面，退出循环
				collisionState = PENETRATING;
            }
            else
            if(axbyczd < depthEpsilon)
            {
                relativeVelocity = m_MathDefs.DotProduct(&plane->normal,&curBall->v);
				if(relativeVelocity < 0.0f)
                {
                    collisionState = COLLIDING_WITH_WALL;
					g_Contact[g_ContactCnt].type = COLLIDING_WITH_WALL;
					g_Contact[g_ContactCnt].ball = loop; 
					g_Contact[g_ContactCnt].Kr = g_Kr_Bumper;		// Ball to bumper Kr
					memcpy(&g_Contact[g_ContactCnt].normal,&plane->normal,sizeof(tVector));
					g_ContactCnt++;
                }
            }
        }
		// 现在判断球与球之间的碰撞  
		ball2 = system;
		for (loop2 = 0; (loop2 < BALL_COUNT) && (collisionState != PENETRATING); 
				loop2++,ball2++)
		{
			if (loop2 == loop) continue;		// 不与自身比较
			if (curBall->flags) continue;		// 如果当前球是碰撞球

			m_MathDefs.VectorDifference(&curBall->pos, &ball2->pos, &distVect);
			//  获得两球心之间的距离
			dist = m_MathDefs.VectorSquaredLength(&distVect);

			dist = dist - (BALL_DIAMETER * BALL_DIAMETER);	 
			//  判断两球之间是否穿过
			if(dist < -ballDepthEpsilon)
			{
				// 如果穿过，则退出
				collisionState = PENETRATING;
			}
			else
			if(dist < ballDepthEpsilon)
			{
				m_MathDefs.NormalizeVector(&distVect);
				//  获得相对速度
				relativeVelocity = m_MathDefs.DotProduct(&distVect,&curBall->v);
				//  如果相对速度小于0
				if(relativeVelocity < 0.0f)		
				{
					collisionState = COLLIDING_WITH_BALL;
					g_Contact[g_ContactCnt].type = COLLIDING_WITH_BALL;
					g_Contact[g_ContactCnt].ball = loop; 
					g_Contact[g_ContactCnt].ball2 = loop2; 
					g_Contact[g_ContactCnt].Kr = g_Kr_Ball;	
					memcpy(&g_Contact[g_ContactCnt].normal,&distVect,sizeof(tVector));
					ball2->flags = 1;
					g_ContactCnt++;
				}
			}
		}
	}
    return collisionState;
}

void CBilliardView::ResolveCollisions( t_Ball  *system )
{
	t_Contact	*contact;
	t_Ball		*ball,*ball2;
	float		VdotN;		
	tVector		Vn,Vt,Vn1;
	int			loop;

	contact = g_Contact;
	for (loop = 0; loop < g_ContactCnt; loop++,contact++)
	{
		ball = &system[contact->ball];
		// 计算 Vn
		VdotN = m_MathDefs.DotProduct(&contact->normal,&ball->v);
		m_MathDefs.ScaleVector(&contact->normal, VdotN, &Vn);
		// 计算 Vt
		m_MathDefs.VectorDifference(&ball->v, &Vn, &Vt);
		// 判断碰撞类型
		if (contact->type == COLLIDING_WITH_WALL)	//  如果球与边界碰撞
		{
			//  Vn乘以恢复系数
			m_MathDefs.ScaleVector(&Vn, contact->Kr, &Vn);
			//  设置新的速度
			m_MathDefs.VectorDifference(&Vt, &Vn, &ball->v);
		}
		else	// 球与球之间碰撞
		{
			// Vn乘以恢复系数
			m_MathDefs.ScaleVector(&Vn, contact->Kr, &Vn1);
			// 设置新的速度
			m_MathDefs.VectorDifference(&Vt, &Vn1, &ball->v);

			ball2 = &system[contact->ball2];
			// 球2的Vn乘以恢复系数
			m_MathDefs.ScaleVector(&Vn, 1.0f - contact->Kr, &Vn1);
			// 设置球2的速度
			m_MathDefs.VectorSum(&Vn1,&ball2->v,&ball2->v);

			ball->flags = 0;
			ball2->flags = 0;
		}
	}
}

void CBilliardView::Simulate(float DeltaTime, BOOL running)
{
    float		CurrentTime = 0.0f;
    float		TargetTime = DeltaTime;
	t_Ball		*tempSys;
	int			collisionState;

    while(CurrentTime < DeltaTime)
    {
		if (running)
		{
			//  定义各球的作用力
			ComputeForces(g_CurrentSys);

			//  如果出现碰撞穿透的现象，强制使用欧拉方法，否则提供选择
			if (g_CollisionRootFinding)
			{
				EulerIntegrate(TargetTime-CurrentTime);
			}
			else
			{
				switch (g_IntegratorType)
				{
				case EULER_INTEGRATOR:
					EulerIntegrate(TargetTime-CurrentTime);
					break;
				case MIDPOINT_INTEGRATOR:
					MidPointIntegrate(TargetTime-CurrentTime);
					break;
				}
			}
		}

		collisionState = CheckForCollisions(g_TargetSys);

        if(collisionState == PENETRATING)
        {
			// 将使用欧拉方法
			g_CollisionRootFinding = TRUE;
            // 将时间步长缩短
            TargetTime = (CurrentTime + TargetTime) / 2.0f;

            assert(fabs(TargetTime - CurrentTime) > EPSILON);
        }
        else
        {
            if(collisionState == COLLIDING_WITH_WALL || collisionState == COLLIDING_WITH_BALL)
            {
                int Counter = 0;
                do
                {
                    ResolveCollisions(g_TargetSys);
                    Counter++;
                } while((CheckForCollisions(g_TargetSys) >= COLLIDING_WITH_WALL) && (Counter < 500));

                assert(Counter < 500);
				g_CollisionRootFinding = FALSE;	
            }

 			//  进入下一步
			CurrentTime = TargetTime;
			TargetTime = DeltaTime;

			MAKEVECTOR(g_CueForce,0.0f,0.0f,0.0f);	// 清除球杆的力

			// SWAP MY TWO SYSTEM BUFFERS SO I CAN DO IT AGAIN
			tempSys = g_CurrentSys;
			g_CurrentSys = g_TargetSys;
			g_TargetSys = tempSys;
        }
    }
}

float CBilliardView::GetTime( void )
{
    static long StartMilliseconds;
	long CurrentMilliseconds;
    if(!StartMilliseconds)
    {
        StartMilliseconds = timeGetTime();
    }

    CurrentMilliseconds = timeGetTime();
    return (float)(CurrentMilliseconds - StartMilliseconds) / 1000.0f;
}





