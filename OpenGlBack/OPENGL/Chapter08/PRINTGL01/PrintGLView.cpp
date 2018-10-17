// PrintGLView.cpp : implementation of the CPrintGLView class
//

#include "stdafx.h"
#include "PrintGL.h"

#include "PrintGLDoc.h"
#include "PrintGLView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CPrintGLView

IMPLEMENT_DYNCREATE(CPrintGLView, CView)

BEGIN_MESSAGE_MAP(CPrintGLView, CView)
	//{{AFX_MSG_MAP(CPrintGLView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintGLView construction/destruction

CPrintGLView::CPrintGLView()
{
	m_bTimerEnabled = TRUE;

}

CPrintGLView::~CPrintGLView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CPrintGLView printing

BOOL CPrintGLView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	m_bTimerEnabled = FALSE;

	if(!pInfo->m_bPreview)
	{
		CRect rcDIB;
		GetClientRect(&rcDIB);
		OnDraw(GetCDC());
		CapturedImage.Capture(GetCDC(), rcDIB);
	}

	BOOL rc = DoPreparePrinting(pInfo);
	
	if(!rc)
		m_bTimerEnabled = TRUE;

	return rc;
}


void CPrintGLView::OnFilePrintPreview()
{
	CRect rcDIB;
	GetClientRect(&rcDIB);
	OnDraw(GetCDC());
	CapturedImage.Capture(GetCDC(), rcDIB);

	CView::OnFilePrintPreview();
}

void CPrintGLView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CView::OnBeginPrinting(pDC, pInfo);
}

void CPrintGLView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	
	m_bTimerEnabled = TRUE;
	CapturedImage.Release();

	CView::OnEndPrinting(pDC, pInfo);
}


void CPrintGLView::OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView) 
{
	m_bTimerEnabled = TRUE;
	CView::OnEndPrintPreview(pDC, pInfo, point, pView);
}


/////////////////////////////////////////////////////////////////////////////
// CPrintGLView drawing

void CPrintGLView::OnDraw(CDC* pDC)
{
	CPrintGLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if (pDC->IsPrinting()) 
	{
		CRect rcDIB;
		GetClientRect(&rcDIB);
		rcDIB.right = rcDIB.Width();
		rcDIB.bottom = rcDIB.Height();
		// 获得打印机页面尺寸(像素)
		int cxPage = pDC->GetDeviceCaps(HORZRES);
		int cyPage = pDC->GetDeviceCaps(VERTRES);
		// 获得打印机每英寸上的像素个数
		int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
		int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);
		CRect rcDest;
		rcDest.top = rcDest.left = 0;
		rcDest.bottom = (int)(((double)rcDIB.Height() * cxPage * cyInch)
				/ ((double)rcDIB.Width() * cxInch));
		rcDest.right = cxPage;
		CapturedImage.OnDraw(pDC->m_hDC, &rcDest, &rcDIB);
	}
	else 
	{
		wglMakeCurrent( pDC->m_hDC, m_hRC );
		RenderScene();
		wglMakeCurrent( 0, 0 );
	}

}

/////////////////////////////////////////////////////////////////////////////
// CPrintGLView diagnostics

#ifdef _DEBUG
void CPrintGLView::AssertValid() const
{
	CView::AssertValid();
}

void CPrintGLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CPrintGLDoc* CPrintGLView::GetDocument() // non-debug version is inline
{
	return STATIC_DOWNCAST(CPrintGLDoc, m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPrintGLView message handlers


BOOL CPrintGLView::PreCreateWindow(CREATESTRUCT& cs) 
{
    // An OpenGL window must be created with the following flags and must not
    // include CS_PARENTDC for the class style. Refer to SetPixelFormat
    // documentation in the "Comments" section for further information.
    cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	return CView::PreCreateWindow(cs);
}

int CPrintGLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

//////////////////////////////////////////////////////////////////
//初始化OpenGL和设置定时器
	m_pDC = new CClientDC(this);
	SetTimer(1, 20, NULL);
	InitializeOpenGL(m_pDC);
//////////////////////////////////////////////////////////////////

	return 0;
}

void CPrintGLView::OnDestroy() 
{
	CView::OnDestroy();
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

void CPrintGLView::OnSize(UINT nType, int cx, int cy) 
{
	
	CView::OnSize(nType, cx, cy);
	
/////////////////////////////////////////////////////////////////
//添加窗口缩放时的图形变换函数
	glViewport(0,0,cx,cy);
/////////////////////////////////////////////////////////////////
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (cx <= cy) 
		glOrtho (-1.5, 1.5, -1.5*(GLfloat)cy/(GLfloat)cx,1.50*(GLfloat)cy/(GLfloat)cx, -10.0, 10.0);
    else 
		glOrtho (-1.5*(GLfloat)cx/(GLfloat)cy,1.5*(GLfloat)cx/(GLfloat)cy, -1.5, 1.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity ();	

}

void CPrintGLView::OnTimer(UINT nIDEvent) 
{
/////////////////////////////////////////////////////////////////
//添加定时器响应函数和场景更新函数
	Invalidate(FALSE);	
/////////////////////////////////////////////////////////////////
	if(m_bTimerEnabled)
	{
		step = step + 1.0;
		if (step > 360.0)
			step = step - 360.0;
		s+=0.005;
		if(s>1.0)
			s=0.1;
	}
	
	CView::OnTimer(nIDEvent);

}

/////////////////////////////////////////////////////////////////////
//	                  设置逻辑调色板
//////////////////////////////////////////////////////////////////////
void CPrintGLView::SetLogicalPalette(void)
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
BOOL CPrintGLView::InitializeOpenGL(CDC* pDC)
{
	m_pDC = pDC;
	SetupPixelFormat();
	//生成绘制描述表
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	//置当前绘制描述表
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	GLfloat light_ambient[]={0.3,0.2,0.5};
	GLfloat light_diffuse[]={1.0,1.0,1.0};
    GLfloat light_position[] = { 2.0, 2.0, 2.0, 1.0 };

    GLfloat light1_ambient[]={0.3,0.3,0.2};
	GLfloat light1_diffuse[]={1.0,1.0,1.0};
    GLfloat light1_position[] = { -2.0, -2.0, -2.0, 1.0 };

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glColorMaterial(GL_FRONT_AND_BACK,GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
	return TRUE;
}

//////////////////////////////////////////////////////////
//						设置像素格式
//////////////////////////////////////////////////////////
BOOL CPrintGLView::SetupPixelFormat()
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
BOOL CPrintGLView::RenderScene() 
{
	glClearColor(0.1,0.5,0.7,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
  		glScalef(s,s,s);
		glRotatef(step,0.0,1.0,0.0);
		glRotatef(step,0.0,0.0,1.0);
  		glRotatef(step,1.0,0.0,0.0);
		DrawColorBox();		//  绘制立方体
    glPopMatrix();

    glFlush();

	::SwapBuffers(m_pDC->GetSafeHdc());		//交互缓冲区
	return TRUE;
}

void CPrintGLView::DrawColorBox(void)
{
	GLfloat  p1[]={0.5,-0.5,-0.5}, p2[]={0.5,0.5,-0.5},
             p3[]={0.5,0.5,0.5},   p4[]={0.5,-0.5,0.5},
             p5[]={-0.5,-0.5,0.5}, p6[]={-0.5,0.5,0.5},
             p7[]={-0.5,0.5,-0.5}, p8[]={-0.5,-0.5,-0.5};

	GLfloat  m1[]={1.0,0.0,0.0},	m2[]={-1.0,0.0,0.0},
		 	 m3[]={0.0,1.0,0.0},	m4[]={0.0,-1.0,0.0},
			 m5[]={0.0,0.0,1.0},	m6[]={0.0,0.0,-1.0};

   	
	GLfloat  c1[]={0.0,0.0,1.0},  c2[]={0.0,1.0,1.0},
             c3[]={1.0,1.0,1.0},  c4[]={1.0,0.0,1.0},
             c5[]={1.0,0.0,0.0},  c6[]={1.0,1.0,0.0},
             c7[]={0.0,1.0,0.0},  c8[]={1.0,1.0,1.0}; 	
    glBegin (GL_QUADS);	

		glColor3fv(c1);
         glNormal3fv(m1);
        glVertex3fv(p1);
		  glColor3fv(c2);
        glVertex3fv(p2);
		  glColor3fv(c3);
        glVertex3fv(p3);
		  glColor3fv(c4);
        glVertex3fv(p4);

		glColor3fv(c5);
         glNormal3fv(m5);
        glVertex3fv(p5);
		  glColor3fv(c6);
        glVertex3fv(p6);
		  glColor3fv(c7);
        glVertex3fv(p7);
		  glColor3fv(c8);
        glVertex3fv(p8);

	    glColor3fv(c5);
         glNormal3fv(m3);
        glVertex3fv(p5);
		  glColor3fv(c6);
        glVertex3fv(p6);
		  glColor3fv(c3);
        glVertex3fv(p3);
		  glColor3fv(c4);
        glVertex3fv(p4);

		glColor3fv(c1);
         glNormal3fv(m4);
        glVertex3fv(p1);
		  glColor3fv(c2);
        glVertex3fv(p2);
		  glColor3fv(c7);
        glVertex3fv(p7);
		  glColor3fv(c8);
        glVertex3fv(p8);

		glColor3fv(c2);
         glNormal3fv(m5);
        glVertex3fv(p2);
		  glColor3fv(c3);
        glVertex3fv(p3);
		  glColor3fv(c6);
        glVertex3fv(p6);
		  glColor3fv(c7);
        glVertex3fv(p7);

		glColor3fv(c1);
         glNormal3fv(m6);
        glVertex3fv(p1);
		  glColor3fv(c4);
        glVertex3fv(p4);
		  glColor3fv(c5);
        glVertex3fv(p5);
		  glColor3fv(c8);
        glVertex3fv(p8);

	  glEnd();
}



