// TetrisView.cpp : implementation of the CTetrisView class
//

#include "stdafx.h"
#include "Tetris.h"
#include "time.h"

#include "TetrisDoc.h"
#include "TetrisView.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTetrisView

IMPLEMENT_DYNCREATE(CTetrisView, CView)

BEGIN_MESSAGE_MAP(CTetrisView, CView)
	//{{AFX_MSG_MAP(CTetrisView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_TIMER()
	ON_COMMAND(ID_GAME_NEW, OnGameNew)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTetrisView construction/destruction

CTetrisView::CTetrisView()
{
	// TODO: add construction code here
	gameintro  = true;
	gamerun    = false;
	gamewin    = false;
	gamefinish = false;	
	onetime    = true;
	test       = false;
	running    = true;

	xpos = 155;
	ypos = 295;

	level = 1;
	speed = 1000;

	keys[VK_RETURN]=false;
	keys[VK_LEFT]=false;
	keys[VK_RIGHT]=false;
	keys[VK_UP]=false;
	keys[VK_DOWN]=false;
	keys[VK_SPACE]=false;

	int i,j;
	for(i=0;i<=10;i++)
		for(j=0;j<=20;j++)
			map[i][j]=0;
	for(i=0;i<=4;i++)
		for(j=0;j<=5;j++)
			nextblockmap[i][j]=0;

}

CTetrisView::~CTetrisView()
{
}

BOOL CTetrisView::PreCreateWindow(CREATESTRUCT& cs)
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
// CTetrisView drawing

void CTetrisView::OnDraw(CDC* pDC)
{
	CTetrisDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
//////////////////////////////////////////////////////////////////
	RenderScene();	//渲染场景
//////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////
// CTetrisView printing

BOOL CTetrisView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTetrisView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTetrisView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTetrisView diagnostics

#ifdef _DEBUG
void CTetrisView::AssertValid() const
{
	CView::AssertValid();
}

void CTetrisView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTetrisDoc* CTetrisView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTetrisDoc)));
	return (CTetrisDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTetrisView message handlers

int CTetrisView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
//////////////////////////////////////////////////////////////////
//初始化OpenGL和设置定时器
	m_pDC = new CClientDC(this);
	SetTimer(1, 10, NULL);
	InitializeOpenGL(m_pDC);
//////////////////////////////////////////////////////////////////
	return 0;
}

void CTetrisView::OnDestroy() 
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
	m_font.KillFont();	
}

void CTetrisView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (cy==0)										// Prevent A Divide By Zero By
	{
		cy=1;										// Making Height Equal One
	}
	glViewport(0,0,cx,cy);						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0f,640.0f,480.0f,0.0f,-1.0f,1.0f);				// Create Ortho 640x480 View (0,0 At Top Left)
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	
}

void CTetrisView::OnTimer(UINT nIDEvent) 
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
void CTetrisView::SetLogicalPalette(void)
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
BOOL CTetrisView::InitializeOpenGL(CDC* pDC)
{
	m_pDC = pDC;
	SetupPixelFormat();
	//生成绘制描述表
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	//置当前绘制描述表
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	CreateTexture(texture, "Data/GameRun.bmp",0);	    
	CreateTexture(texture, "Data/Block.bmp",1);
	CreateTexture(texture, "Data/GameIntro.bmp",2);
	CreateTexture(texture, "Data/GameOver.bmp",3);
	CreateTexture(texture, "Data/GameWin.bmp",4);
	CreateTexture(texture, "Data/LevelSelect.bmp",5);
	CreateTexture(texture, "Data/Mask.bmp",6);

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	HDC hDC = wglGetCurrentDC();
	m_font.BuildFont(hDC);	
	srand(timeGetTime() );

	return TRUE;
}

//////////////////////////////////////////////////////////
//						设置像素格式
//////////////////////////////////////////////////////////
BOOL CTetrisView::SetupPixelFormat()
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
BOOL CTetrisView::RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear Screen And Depth Buffer
	glLoadIdentity();											// Reset The Current Modelview Matrix
				
	if(gameintro)
	{
		DrawGameIntro();		
	}
	if(gamerun)
	{
		DrawGameRun();																
	}
			
	if(gamefinish)
	{
		DrawGameRun();
		DrawGameFinish();	
	}
	if(gamewin)
	{
		DrawGameRun();
		DrawGameWin();	
	}

	::SwapBuffers(m_pDC->GetSafeHdc());		//交互缓冲区
	return TRUE;
}



void CTetrisView::DrawGameIntro()
{
	//  游戏封面的绘制
	glBindTexture(GL_TEXTURE_2D,  texture[2]);	
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);	glVertex2i(0,0);
		glTexCoord2i(0,0);	glVertex2i(0,480);
		glTexCoord2i(1,0);	glVertex2i(640,480);
		glTexCoord2i(1,1);	glVertex2i(640,0);
	glEnd();
	//  被选择难度等级数的绘制
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	glEnable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D,  texture[6]);	
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);	glVertex2i(xpos,ypos);
		glTexCoord2i(0,0);	glVertex2i(xpos,ypos+64);
		glTexCoord2i(1,0);	glVertex2i(xpos+64,ypos+64);
		glTexCoord2i(1,1);	glVertex2i(xpos+64,ypos);
	glEnd();
	glBlendFunc(GL_ONE,GL_ONE);
	glBindTexture(GL_TEXTURE_2D,  texture[5]);	
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);	glVertex2i(xpos,ypos);
		glTexCoord2i(0,0);	glVertex2i(xpos,ypos+64);
		glTexCoord2i(1,0);	glVertex2i(xpos+64,ypos+64);
		glTexCoord2i(1,1);	glVertex2i(xpos+64,ypos);
		glEnd();
	glDisable(GL_BLEND);

	if (keys[VK_LEFT])
	{
		keys[VK_LEFT] = false;
		if(level--==1)
			level=10;
		if (xpos <= 155 && ypos == 295)
		{
			xpos = 419;
			ypos = 295+66;
		}else if (xpos <= 155 && ypos == 295+66)
		{
			xpos = 419;
			ypos = 295;
		}else
			xpos-=66;
	}
	if (keys[VK_RIGHT])
	{
		keys[VK_RIGHT] = false;
		if(level++==10)
			level=1;
		if (xpos >= 419 && ypos == 295)
		{
			xpos = 155;
			ypos = 295+66;
		}else if (xpos >= 419 && ypos == 295+66)
		{
			xpos = 155;
			ypos = 295;
		}else
			xpos+=66;
	}

	if (keys[VK_RETURN])
	{
		speed=int(speed/(level+1));
		SetTimer(1, speed, NULL);
		gamefinish = false;
		gamewin    = false;
		gameintro  = false;
		gamerun    = true;
	}

}
 
int CTetrisView::DrawGameRun(GLvoid)
{	
	if (onetime)
	{
		InitGame();
		onetime = false;
	}
	else if (running)
	{
		if (!MoveDown())
		{
			CheckWin();
		}
	}

	// 绘制背景
	glBindTexture(GL_TEXTURE_2D,  texture[0]);	
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);	glVertex2i(0,0);
		glTexCoord2i(0,0);	glVertex2i(0,480);
		glTexCoord2i(1,0);	glVertex2i(640,480);
		glTexCoord2i(1,1);	glVertex2i(640,0);
	glEnd();		

	//绘制主窗口的方块图	
	for (int x = 0;x <= 9;x++)
	{
		for (int y = 0;y <= 19;y++)
		{
			int xp = x* 25+50;
			int yp = y* 20+40;
			if (map[x][y] == 2 || map [x][y] == 1)
			{
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glBegin(GL_QUADS);
					glTexCoord2i(0,1); glVertex2i(xp,yp);
					glTexCoord2i(1,1); glVertex2i(xp + 25,yp);   
					glTexCoord2i(1,0); glVertex2i(xp + 25,yp + 20);      
					glTexCoord2i(0,0); glVertex2i(xp,yp + 20);           
				glEnd();
			}
		}
	}
    // 绘制下一个将要下落的随机小方块
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			int ip = i* 25+410;
			int jp = j* 20+320;
			if (nextblockmap[i][j] != 0 && type != 0)
			{	
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glBegin(GL_QUADS);
					glTexCoord2i(0,1); glVertex2i(ip,jp);
					glTexCoord2i(1,1); glVertex2i(ip + 25,jp);   
					glTexCoord2i(1,0); glVertex2i(ip + 25,jp + 20);      
					glTexCoord2i(0,0); glVertex2i(ip,jp + 20);           
				glEnd();
			}
			if (nextblockmap[i][j] != 0 && type == 0)
			{
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glBegin(GL_QUADS);
					glTexCoord2i(0,1); glVertex2i(ip+12,jp);
					glTexCoord2i(1,1); glVertex2i(ip+12 + 25,jp);   
					glTexCoord2i(1,0); glVertex2i(ip+12 + 25,jp + 20);      
					glTexCoord2i(0,0); glVertex2i(ip+12,jp + 20);           
				glEnd();
			}

		}
	}    				
	// 绘制文本
	glDisable(GL_TEXTURE_2D);
    glColor3d(128,128,128);
	glRasterPos2d(370, 65);
	m_font.glPrint("L E V E L");
	glRasterPos2d(485, 65);
	m_font.glPrint("%d",level-1);
	glRasterPos2d(370,68+73+13);
	m_font.glPrint("L I N E S");
	glRasterPos2d(485,68+73+13);
	m_font.glPrint("%d",lines);
	glRasterPos2d(370, 239);
	m_font.glPrint("S C O R E");
	glRasterPos2d(485, 239);
	m_font.glPrint("%d",score);
	glRasterPos2d(433,320);
	m_font.glPrint("N E X T");
	glEnable(GL_TEXTURE_2D);

	if (keys[VK_LEFT])
	{
		keys[VK_LEFT] = false;
		MoveLeft();
	}
	if (keys[VK_RIGHT])
	{
		keys[VK_RIGHT] = false;
		MoveRight();
	}
	if (keys[VK_UP])
	{
		keys[VK_UP] = false;
		Rotate();
	}
	if (keys[VK_DOWN])
	{
		keys[VK_DOWN] = false;
		MoveDown();
	}
	if (keys[VK_SPACE])
	{
		keys[VK_SPACE] = false;
		do{} while (MoveDown());
	}

	return true;
}

void CTetrisView::DrawGameWin()
{
	glBindTexture(GL_TEXTURE_2D,  texture[4]);	
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);	glVertex2i(640/2-150,480/2-75);
		glTexCoord2i(0,0);	glVertex2i(640/2-150,480/2+150-75);
		glTexCoord2i(1,0);	glVertex2i(640/2+300-150,480/2+150-75);
		glTexCoord2i(1,1);	glVertex2i(640/2+300-150,480/2-75);
	glEnd();
}

void CTetrisView::DrawGameFinish()
{	
	glBindTexture(GL_TEXTURE_2D,  texture[3]);	
	glBegin(GL_QUADS);
		glTexCoord2i(0,1);	glVertex2i(640/2-150,480/2-75);
		glTexCoord2i(0,0);	glVertex2i(640/2-150,480/2+150-75);
		glTexCoord2i(1,0);	glVertex2i(640/2+300-150,480/2+150-75);
		glTexCoord2i(1,1);	glVertex2i(640/2+300-150,480/2-75);
	glEnd();
}


void CTetrisView::InitGame()
{
	score = 0;
	state = 0;
	lines = 0;
	block = nextblock;
	state = 0;
	NewBlock();
	CheckWin();
}


void CTetrisView::NewBlock()
{
	int i;

	type = rand()%7;
	switch(type)
	{
		case 0:
			for ( i = 0;i <= 3;i++)
			{	
				block[0].x[i] = 4;
				block[1].x[i] = 5;
				block[2].x[i] = 4;
				block[3].x[i] = 5;

				block[0].y[i] = 0;
				block[1].y[i] = 0;
				block[2].y[i] = 1;
				block[3].y[i] = 1;
			}
		break;
		case 1:
			for ( i = 0;i <= 3;i++)
			{
				block[i].y[0] = i;
				block[i].y[2] = i;
				block[i].x[0] = 5;
				block[i].x[2] = 5;

				block[i].x[1] = i + 4;
				block[i].x[3] = i + 4;
				block[i].y[1] = 0;
				block[i].y[3] = 0;
			}
		break;
		case 2:
			for ( i = 0;i <= 2;i += 2)
			{
				block[0].x[i] = 4;
				block[1].x[i] = 5;
				block[2].x[i] = 5;
				block[3].x[i] = 6;

				block[0].y[i] = 0;
				block[1].y[i] = 0;
				block[2].y[i] = 1;
				block[3].y[i] = 1;

				block[0].x[i+1] = 5;
				block[1].x[i+1] = 5;
				block[2].x[i+1] = 4;
				block[3].x[i+1] = 4;

				block[0].y[i+1] = 0;
				block[1].y[i+1] = 1;
				block[2].y[i+1] = 1;
				block[3].y[i+1] = 2;
			}
		break;	
		case 3:
			for ( i = 0;i <= 2;i +=2)
			{
				block[0].x[i] = 6;
				block[1].x[i] = 5;
				block[2].x[i] = 5;
				block[3].x[i] = 4;

				block[0].y[i] = 0;
				block[1].y[i] = 0;
				block[2].y[i] = 1;
				block[3].y[i] = 1;

				block[0].x[i+1] = 5;
				block[1].x[i+1] = 5;
				block[2].x[i+1] = 4;
				block[3].x[i+1] = 4;

				block[0].y[i+1] = 2;
				block[1].y[i+1] = 1;
				block[2].y[i+1] = 1;
				block[3].y[i+1] = 0;
			}
		break;
		case 4:
			block[0].x[0] = 4; // 状态 0
			block[1].x[0] = 4;
			block[2].x[0] = 5;
			block[3].x[0] = 6;
		
			block[0].y[0] = 1;
			block[1].y[0] = 0;
			block[2].y[0] = 0;
			block[3].y[0] = 0;

			block[0].x[1] = 5; // 状态 1
			block[1].x[1] = 6;
			block[2].x[1] = 6;
			block[3].x[1] = 6;
		
			block[0].y[1] = 0;
			block[1].y[1] = 0;
			block[2].y[1] = 1;
			block[3].y[1] = 2;

			block[0].x[2] = 4; // 状态 2
			block[1].x[2] = 5;
			block[2].x[2] = 6;
			block[3].x[2] = 6;
		
			block[0].y[2] = 1;
			block[1].y[2] = 1;
			block[2].y[2] = 1;
			block[3].y[2] = 0;

			block[0].x[3] = 5; // 状态 3
			block[1].x[3] = 5;
			block[2].x[3] = 5;
			block[3].x[3] = 6;
		
			block[0].y[3] = 0;
			block[1].y[3] = 1;
			block[2].y[3] = 2;
			block[3].y[3] = 2;
		break;
		case 5:
			block[0].x[0] = 4; // 状态 0
			block[1].x[0] = 5;
			block[2].x[0] = 6;
			block[3].x[0] = 6;
		
			block[0].y[0] = 0;
			block[1].y[0] = 0;
			block[2].y[0] = 0;
			block[3].y[0] = 1;

			block[0].x[1] = 6; // 状态 1
			block[1].x[1] = 6;
			block[2].x[1] = 6;
			block[3].x[1] = 5;
		
			block[0].y[1] = 0;
			block[1].y[1] = 1;
			block[2].y[1] = 2;
			block[3].y[1] = 2;

			block[0].x[2] = 4; // 状态 2
			block[1].x[2] = 4;
			block[2].x[2] = 5;
			block[3].x[2] = 6;
		
			block[0].y[2] = 0;
			block[1].y[2] = 1;
			block[2].y[2] = 1;
			block[3].y[2] = 1;

			block[0].x[3] = 6; // 状态 3
			block[1].x[3] = 5;
			block[2].x[3] = 5;
			block[3].x[3] = 5;
		
			block[0].y[3] = 0;
			block[1].y[3] = 0;
			block[2].y[3] = 1;
			block[3].y[3] = 2;
		break;
		case 6:
			block[0].x[3] = 5;  // 状态 0
			block[1].x[3] = 5;
			block[2].x[3] = 5;
			block[3].x[3] = 4;

			block[0].y[3] = 0;
			block[1].y[3] = 1;
			block[2].y[3] = 2;
			block[3].y[3] = 1;

			block[0].x[2] = 4; // 状态 1
			block[1].x[2] = 5;
			block[2].x[2] = 6;
			block[3].x[2] = 5;

			block[0].y[2] = 1;
			block[1].y[2] = 2;
			block[2].y[2] = 1;
			block[3].y[2] = 1;

			block[0].x[0] = 4;  // 状态 2
			block[1].x[0] = 5;
			block[2].x[0] = 6;
			block[3].x[0] = 5;
	
			block[0].y[0] = 1;
			block[1].y[0] = 1;
			block[2].y[0] = 1;
			block[3].y[0] = 0;

			block[0].x[1] = 5;  // 状态 3
			block[1].x[1] = 5;
			block[2].x[1] = 5;
			block[3].x[1] = 6;

			block[0].y[1] = 0;
			block[1].y[1] = 1;
			block[2].y[1] = 2;
			block[3].y[1] = 1;
		break;
	}
}

void CTetrisView::CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID)
{
	AUX_RGBImageRec *pBitmap = NULL;
	//  如果文件不存在，在返回
	if(!strFileName)								
		return;
	//  读入位图文件中的数据		
	pBitmap = auxDIBImageLoad(strFileName);				
	if(pBitmap == NULL)									
		exit(0);
	//  生成纹理
	glGenTextures(1, &textureArray[textureID]);
	//  捆绑纹理
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
	//  设置纹理参数
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pBitmap->sizeX, pBitmap->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pBitmap->data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	//  释放资源
	if (pBitmap)									
	{
		if (pBitmap->data)								
		{
			free(pBitmap->data);					
		}
		free(pBitmap);									
	}
}

bool CTetrisView::CheckWin()
{
	linemultiplier = 0;
	linesformed = 0;
	for (int i = 0;i <=3;i++)
		map[block[i].x[state]][block[i].y[state]] = 1;

	for ( i = 0;i<20;i++)
	{
		lineformed = TRUE;
		for(int j=0;j<10;j++)
		{
			if (map[j][i] == 0)
				lineformed = FALSE;
		}
		
		if (lineformed)
		{
		    
			linesformed++;
			for (int k=i-1;k>=0;k--)
			{
				for (int l=0;l<10;l++)
				{
					map[l][k + 1] = map[l][k];
				}
			}
		}
	}
	switch (linesformed)
	{
	case 1:
		linemultiplier = 10; 
		break;
	case 2:
		linemultiplier = 25; 
		break;
	case 3:
		linemultiplier = 75; 
		break;
	case 4:
		linemultiplier = 300; 
		break;
	}
	//  游戏分数递增
	lines += linesformed;
	score += (level + 1) * 4 * linemultiplier;
	
	for (i=0;i<4;i++) 
	currentblock[i] = nextblock[i];
	block = nextblock;
	state = 0;
	NewBlock();

	for (int y=0;y<4;y++)
	{
		for (int x=0;x<5;x++)
		{
			nextblockmap[y][x] = 0;
		}
	}

	for (i = 0;i <=3;i++)
		nextblockmap[block[i].x[state]-3][block[i].y[state]+1] = 1;

	block = currentblock;

	for (i = 0;i <=3;i++)
	{ 
			if (map[block[i].x[state]][block[i].y[state]] == 1 && !onetime)
			{
				gamerun    = false;
				gameintro  = false;
				gamewin    = false;
				gamefinish = true;
				running	   = false;				
			}	
	
	}
	for (i = 0;i <=3;i++)
		map[block[i].x[state]][block[i].y[state]] = 2;

	if (lines >= (level) * 10)
	{
		//  难度等级递增
		level++;
		speed=int(speed/(level+1));
		SetTimer(1, speed, NULL);
		//  如果游戏难度等级大于10，则游戏成功
		if (level >10)
		{
			gamerun    = false;
			gameintro  = false;
			gamefinish = false;
			gamewin    = true;
			running    = false;
		}
		return true;
	}

	return false;
}

bool CTetrisView::MoveDown()
{
	for (int i = 0;i <=3;i++)
	{
		// 如果小方块到达底部
		if ((map[block[i].x[state]][block[i].y[state]+1] == 1) || 
			(block[i].y[state] >= 19))	return false;
	} 

	for (i = 0;i <=3;i++)
	{
		// 删除块
		map[block[i].x[state]][block[i].y[state]] = 0;
		// 向下移动
		for (int j = 0;j <= 3;j++)
		{
			block[i].y[j] = block[i].y[j]++;
		}
	}
	for (i = 0;i <=3;i++)
		map[block[i].x[state]][block[i].y[state]] = 2;

	return true;
}

bool CTetrisView::MoveLeft()
{
	for (int i = 0;i <=3;i++)
	{
		if (map[block[i].x[state]-1][block[i].y[state]] == 1 ||
			block[i].x[state] <= 0)
			return false;
	} 
	for (i = 0;i <=3;i++)
	{
		map[block[i].x[state]][block[i].y[state]] = 0;
		for (int j = 0;j <= 3;j++)
		{
			block[i].x[j] = block[i].x[j]--;
		}
	}
	for (i = 0;i <=3;i++)
		map[block[i].x[state]][block[i].y[state]] = 2;

	return true;
}

bool CTetrisView::MoveRight()
{
	for (int i = 0;i <=3;i++)
	{
		if (map[block[i].x[state]+1][block[i].y[state]] == 1 ||
			block[i].x[state] >= 9)
			return false;
	} 
	for (i = 0;i <=3;i++)
	{
		map[block[i].x[state]][block[i].y[state]] = 0;
		for (int j = 0;j <= 3;j++)
		{
			block[i].x[j] = block[i].x[j]++;
		}
	}
	for (i = 0;i <=3;i++)
		map[block[i].x[state]][block[i].y[state]] = 2;

	return true;
}

bool CTetrisView::Rotate()
{
	newstate = state+1;
	if (newstate==4)
		newstate=0;
	for (int i = 0;i <=3;i++)
	{
		if (map[block[i].x[newstate]][block[i].y[newstate]] == 1 ||
			block[i].x[newstate] >= 10 || block[i].x[newstate] <= -1 ||
			block[i].y[newstate] >= 19 || block[i].y[newstate] <= 0)
			{
				return FALSE;
			}
	}
	for (i = 0;i <=3;i++)
	{
		map[block[i].x[state]][block[i].y[state]] = 0;
	}
	for (i = 0;i <=3;i++)
		map[block[i].x[newstate]][block[i].y[newstate]] = 2;
	
	state = newstate;
	return true;
}

void CTetrisView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	keys[nChar]=true;
	if(nChar==VK_ESCAPE) exit(0);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CTetrisView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	keys[nChar]=false;
	
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}



void CTetrisView::OnGameNew() 
{
	// TODO: Add your command handler code here
	int i,j;
	for(i=0;i<=10;i++)
		for(j=0;j<=20;j++)
			map[i][j]=0;
	for(i=0;i<=4;i++)
		for(j=0;j<=5;j++)
			nextblockmap[i][j]=0;

	gameintro  = true;
	gamerun    = false;
	gamewin    = false;
	gamefinish = false;
	onetime    = true;
	test       = false;
	running    = true;
}
