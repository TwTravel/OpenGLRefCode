// FractalView.cpp : implementation of the CFractalView class
//

#include "stdafx.h"
#include "Fractal.h"

#include "FractalDoc.h"
#include "FractalView.h"

#include <math.h>

#define PI ((float)3.14159265358979323846)
#define RADIANS(fAngle) ((float)(fAngle)*PI/180.0)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFractalView

IMPLEMENT_DYNCREATE(CFractalView, CView)

BEGIN_MESSAGE_MAP(CFractalView, CView)
	//{{AFX_MSG_MAP(CFractalView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_RABBIT, OnRabbit)
	ON_UPDATE_COMMAND_UI(IDM_RABBIT, OnUpdateRabbit)
	ON_COMMAND(IDM_FRACTAL, OnFractal)
	ON_UPDATE_COMMAND_UI(IDM_FRACTAL, OnUpdateFractal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFractalView construction/destruction

CFractalView::CFractalView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_controlType=RabtToTos;
}

CFractalView::~CFractalView()
{
}

BOOL CFractalView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CFractalView drawing

void CFractalView::OnDraw(CDC* pDC)
{
	CFractalDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CFractalView diagnostics

#ifdef _DEBUG
void CFractalView::AssertValid() const
{
	CView::AssertValid();
}

void CFractalView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFractalDoc* CFractalView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFractalDoc)));
	return (CFractalDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFractalView message handlers

void CFractalView::Init()
{
	PIXELFORMATDESCRIPTOR pfd;
    int         n;
	HGLRC		hrc;

    m_pDC = new CClientDC(this);

    ASSERT(m_pDC != NULL);

    if (!bSetupPixelFormat())
        return;

    n =::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    hrc = wglCreateContext(m_pDC->GetSafeHdc());
    wglMakeCurrent(m_pDC->GetSafeHdc(), hrc);

    GetClientRect(&m_oldRect);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

BOOL CFractalView::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR), 
        1,                             
        PFD_DRAW_TO_WINDOW |           
          PFD_SUPPORT_OPENGL,		
        PFD_TYPE_RGBA,              
        24,                         
        0, 0, 0, 0, 0, 0,           
        0,                          
        0,                          
        0,                          
        0, 0, 0, 0,                 
        32,                         
        0,                          
        0,                          
        PFD_MAIN_PLANE,             
        0,                          
        0, 0, 0                     
    };
    int pixelformat;

    if ( (pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0 )
    {
        MessageBox("ChoosePixelFormat failed");
        return FALSE;
    }

    if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
    {
        MessageBox("SetPixelFormat failed");
        return FALSE;
    }

    return TRUE;
}

int CFractalView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	Init();
	
	return 0;
}

void CFractalView::OnDestroy() 
{
	HGLRC	hrc;

	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pDC)
        delete m_pDC;

	CView::OnDestroy();
}

void CFractalView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	if(cy > 0)
    { 
		if((m_oldRect.right > cx) || (m_oldRect.bottom > cy))
            RedrawWindow();

        m_oldRect.right = cx;
        m_oldRect.bottom = cy;

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
		glFrustum(-1.0,1.0,-1.0,1.0,3.0,7.0);
		glViewport(0, 0, cx, cy);
    }
}

void CFractalView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	glColor3f(1.0f,0.0f,0.0f);

	switch(m_controlType)
	{
		case RabtToTos:		Chase();		break;
		case Fractal:		myFractal();	break;
	}
	
	glFinish();
}

void CFractalView::MoveThem(GLfloat* fa, GLfloat fb, GLfloat ft)
{
	*fa=(1-ft)*(*fa)+ft*fb;
}

void CFractalView::Chase()
{
	GLfloat	x1,y1;
	GLfloat	x2,y2;
	GLfloat	x3,y3;
	GLfloat	x4,y4;
	GLfloat t=0.05;
	x1=-1.3f;	
	y1=-1.3f;
	x2=1.3f;
	y2=-1.3f;
	x3=1.3f;
	y3=1.3f;
	x4=-1.3f;
	y4=1.3f;

	int iColor=0;

	do
	{
		glColor3f((GLfloat)iColor/256.0-0.1f,
				0.3f,1.0f-(GLfloat)iColor/256.0);

		glBegin(GL_LINE_LOOP);
			glVertex2f(x1,y1);
			glVertex2f(x2,y2);
			glVertex2f(x3,y3);
			glVertex2f(x4,y4);
			glVertex2f(x1,y1);
		glEnd();

		MoveThem(&x1,x2,t);
		MoveThem(&y1,y2,t);
		MoveThem(&x2,x3,t);
		MoveThem(&y2,y3,t);
		MoveThem(&x3,x4,t);
		MoveThem(&y3,y4,t);
		MoveThem(&x4,x1,t);
		MoveThem(&y4,y1,t);

		if(iColor<192)
			iColor += 16;
		else
			iColor +=2;

	}while(Distance(x1,y1,x2,y2)>0.06);

}

GLdouble CFractalView::Distance(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	return sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1));
}

void CFractalView::OnRabbit() 
{
	m_controlType=RabtToTos;
	Invalidate();
}

void CFractalView::OnUpdateRabbit(CCmdUI* pCmdUI) 
{
	if( m_controlType==RabtToTos)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CFractalView::OnFractal() 
{
	m_controlType=Fractal;
	Invalidate();
}

void CFractalView::OnUpdateFractal(CCmdUI* pCmdUI) 
{
	if( m_controlType==Fractal)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

GLfloat CFractalView::MoveX(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	GLfloat xMove;
	GLfloat fAngle;
	GLfloat fDist;
	GLfloat fRadius;
	GLfloat xShift;
	GLfloat yShift;

	fAngle=RADIANS(45.0);

	fDist=Distance(x1,y1,x2,y2);
	fRadius=(sqrt(2.0)/2.0)*fDist;

	xShift=cos(fAngle)*(x2-x1);
	yShift=sin(fAngle)*(y2-y1);
	
	xMove=fRadius/fDist*(xShift-yShift);
	return xMove;
}

GLfloat CFractalView::MoveY(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	GLfloat yMove;
	GLfloat fAngle;
	GLfloat fDist;
	GLfloat fRadius;
	GLfloat xShift;
	GLfloat yShift;

	fAngle=RADIANS(45.0);

	
	fDist=Distance(x1,y1,x2,y2);
	fRadius=(sqrt(2.0)/2.0)*fDist;

	xShift=cos(fAngle)*(x2-x1);
	yShift=sin(fAngle)*(y2-y1);
	
	yMove=fRadius/fDist*(xShift+yShift);
	return yMove;
}

void CFractalView::myFractal()
{
	GLfloat	x1,y1,x2,y2;

	x1=-0.8f;
	y1=-0.6f;
	x2=0.8f;
	y2=-0.6f;

	CreateFractals(x1,y1,x2,y2);
}

void CFractalView::CreateFractals(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
	GLfloat fDistPt;	

	GLfloat xSpike,ySpike;
	
	fDistPt=Distance(x1,y1,x2,y2);
	
	if(fDistPt<=0.001)return;
	
	xSpike=x1+MoveX(x1,y1,x2,y2);
	ySpike=y1+MoveY(x1,y1,x2,y2);
	
	glBegin(GL_LINES);

		glVertex2f(x1,y1);
		glVertex2f(x2,y2);
	
		glVertex2f(x1,y1);
		glVertex2f(xSpike,ySpike);
		
		glVertex2f(xSpike,ySpike);
		glVertex2f(x2,y2);

	glEnd();
	
	CreateFractals(x1,y1,xSpike,ySpike);
	CreateFractals(xSpike,ySpike,x2,y2);
}

