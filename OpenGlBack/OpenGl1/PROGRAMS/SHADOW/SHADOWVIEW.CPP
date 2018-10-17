// ShadowView.cpp : implementation of the CShadowView class
//

#include "stdafx.h"
#include "Shadow.h"

#include "ShadowDoc.h"
#include "ShadowView.h"

#include "math.h"
#include "point.h"
#include "pencil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShadowView

IMPLEMENT_DYNCREATE(CShadowView, CView)

BEGIN_MESSAGE_MAP(CShadowView, CView)
	//{{AFX_MSG_MAP(CShadowView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadowView construction/destruction

CShadowView::CShadowView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_lightPos_x=1.0f;
	m_lightPos_y=-1.0f;
	m_lightPos_z=1.3f;


}

CShadowView::~CShadowView()
{
}

BOOL CShadowView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CShadowView drawing

void CShadowView::OnDraw(CDC* pDC)
{
	CShadowDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CShadowView diagnostics

#ifdef _DEBUG
void CShadowView::AssertValid() const
{
	CView::AssertValid();
}

void CShadowView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CShadowDoc* CShadowView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CShadowDoc)));
	return (CShadowDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShadowView message handlers

void CShadowView::Init()
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

BOOL CShadowView::bSetupPixelFormat()
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

int CShadowView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Init();	
	return 0;
}

void CShadowView::OnDestroy() 
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

void CShadowView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
    GLfloat aspect;

    glViewport( 0, 0, cx, cy);

    aspect = (GLfloat) cx / cy;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(45.0, aspect, 1.0, 10.0 );
}

void CShadowView::DrawScene()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);
	glFrontFace(GL_CCW);

	glTranslatef(0.0f,-1.2,0.0);

	//	将图形坐标系变成笛卡尔坐标系
	glRotatef(-90.0,1.0f,0.0f,0.0f);
	glRotatef(-90.0,0.0f,0.0f,1.0f);
	CreateSun();
	
	Point sun_direction;
	Pencil pencil;

	sun_direction.set(-m_lightPos_x,-m_lightPos_y,-m_lightPos_z);

	pencil.SetPencil(0.08,0.56,2.0);
	pencil.translate_parameter(0.5,0.0,0.0);

	//	请自行作一些变换,例如:
	//	pencil.rotate_angle_x(75.0*3.14159265/180.0);
	//	pencil.translate_parameter(0.5,0.0,0.5);
	
	pencil.draw();

	pencil.draw_shadow(sun_direction);
	
	Ground();
	
	glFinish();
}

void CShadowView::CreateSun()
{
	GLfloat light0Amb[]={0.0f,0.0f,0.0f,1.0f};
	GLfloat light0Dif[]={0.9f,0.9f,0.9f,1.0f};
	GLfloat light0Spe[]={1.0f,1.0f,1.0f,1.0f};

	GLfloat light0Pos[]={m_lightPos_x,m_lightPos_y,m_lightPos_z,0.0f};

	//	第０盏灯为太阳，第１盏灯为聚光灯
	//	为了简化，本程序假定聚光不产生阴影：
	glLightfv(GL_LIGHT0,GL_AMBIENT,light0Amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0Dif);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0Spe);
	glLightfv(GL_LIGHT0,GL_POSITION,light0Pos);

	GLfloat globalAmb[]={0.2f,0.2f,0.2f,1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,globalAmb);

	GLfloat spotAmb[]={0.2f,0.2f,0.2f,0.8f};
	GLfloat spotDif[]={0.7f,0.7f,0.7f,0.8f};
	GLfloat spotSpe[]={0.7f,0.7f,0.7f,0.8f};
	GLfloat spotPos[]={1.0f,0.8f,1.3f,0.8f};
	GLfloat spotDir[]={-1.0f,-0.8f,-.80f,1.0f};
	GLfloat spotCutoff=45.0f;

	glLightfv(GL_LIGHT1,GL_AMBIENT,spotAmb);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,spotDif);
	glLightfv(GL_LIGHT1,GL_SPECULAR,spotSpe);
	glLightfv(GL_LIGHT1,GL_POSITION,spotPos);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,spotDir);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,spotCutoff);
	glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.3);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

}

void CShadowView::Ground()
{
	glNormal3f(0.0f,0.0f,1.0f);
	
	glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glBegin(GL_QUADS);
			glColor4f(0.0f,0.0f,0.0f,1.0f);
			glVertex3f(-5.0f,-2.0f,0.0f);
			glColor4f(0.70f,0.70f,0.70f,1.0f);
			glVertex3f(1.60f,-2.0f,0.0f);
			glColor4f(0.70f,0.70f,0.70f,1.0f);
			glVertex3f(1.60f,2.0f,0.0f);
			glColor4f(0.0f,0.0f,0.0f,1.0f);
			glVertex3f(-5.0f,2.0f,0.0f);
		glEnd();
	glPopAttrib();
	
}

