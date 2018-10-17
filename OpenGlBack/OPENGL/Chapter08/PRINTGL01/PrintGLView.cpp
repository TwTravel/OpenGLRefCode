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
		// ��ô�ӡ��ҳ��ߴ�(����)
		int cxPage = pDC->GetDeviceCaps(HORZRES);
		int cyPage = pDC->GetDeviceCaps(VERTRES);
		// ��ô�ӡ��ÿӢ���ϵ����ظ���
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
//��ʼ��OpenGL�����ö�ʱ��
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
//ɾ����ɫ�����Ⱦ�����ġ���ʱ��
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
//��Ӵ�������ʱ��ͼ�α任����
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
//��Ӷ�ʱ����Ӧ�����ͳ������º���
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
//	                  �����߼���ɫ��
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
//						��ʼ��openGL����
//////////////////////////////////////////////////////////
BOOL CPrintGLView::InitializeOpenGL(CDC* pDC)
{
	m_pDC = pDC;
	SetupPixelFormat();
	//���ɻ���������
	m_hRC = ::wglCreateContext(m_pDC->GetSafeHdc());
	//�õ�ǰ����������
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
//						�������ظ�ʽ
//////////////////////////////////////////////////////////
BOOL CPrintGLView::SetupPixelFormat()
{
	PIXELFORMATDESCRIPTOR pfd = { 
	    sizeof(PIXELFORMATDESCRIPTOR),    // pfd�ṹ�Ĵ�С 
	    1,                                // �汾�� 
	    PFD_DRAW_TO_WINDOW |              // ֧���ڴ����л�ͼ 
	    PFD_SUPPORT_OPENGL |              // ֧�� OpenGL 
	    PFD_DOUBLEBUFFER,                 // ˫����ģʽ 
	    PFD_TYPE_RGBA,                    // RGBA ��ɫģʽ 
	    24,                               // 24 λ��ɫ��� 
	    0, 0, 0, 0, 0, 0,                 // ������ɫλ 
	    0,                                // û�з�͸���Ȼ��� 
	    0,                                // ������λλ 
	    0,                                // ���ۼӻ��� 
	    0, 0, 0, 0,                       // �����ۼ�λ 
	    32,                               // 32 λ��Ȼ���     
	    0,                                // ��ģ�建�� 
	    0,                                // �޸������� 
	    PFD_MAIN_PLANE,                   // ���� 
	    0,                                // ���� 
	    0, 0, 0                           // ���Բ�,�ɼ��Ժ������ģ 
	}; 	
	int pixelformat;
	pixelformat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);//ѡ�����ظ�ʽ
	::SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd);	//�������ظ�ʽ
	if(pfd.dwFlags & PFD_NEED_PALETTE)
		SetLogicalPalette();	//�����߼���ɫ��
	return TRUE;
}

//////////////////////////////////////////////////////////
//						������������Ⱦ
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
		DrawColorBox();		//  ����������
    glPopMatrix();

    glFlush();

	::SwapBuffers(m_pDC->GetSafeHdc());		//����������
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



