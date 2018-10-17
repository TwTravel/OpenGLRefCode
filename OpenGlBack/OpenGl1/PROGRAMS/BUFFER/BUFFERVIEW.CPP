// BufferView.cpp : implementation of the CBufferView class
//

#include "stdafx.h"
#include "Buffer.h"

#include "BufferDoc.h"
#include "BufferView.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBufferView

IMPLEMENT_DYNCREATE(CBufferView, CView)

BEGIN_MESSAGE_MAP(CBufferView, CView)
	//{{AFX_MSG_MAP(CBufferView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_STENCIL_MASK, OnStencilMask)
	ON_UPDATE_COMMAND_UI(IDM_STENCIL_MASK, OnUpdateStencilMask)
	ON_COMMAND(IDM_STENCIL_CLIP, OnStencilClip)
	ON_UPDATE_COMMAND_UI(IDM_STENCIL_CLIP, OnUpdateStencilClip)
	ON_COMMAND(IDM_ANTISCENE, OnAntiscene)
	ON_UPDATE_COMMAND_UI(IDM_ANTISCENE, OnUpdateAntiscene)
	ON_COMMAND(IDM_NON_ANTIACCUM, OnNonAntiaccum)
	ON_UPDATE_COMMAND_UI(IDM_NON_ANTIACCUM, OnUpdateNonAntiaccum)
	ON_COMMAND(IDM_FOCUS_DEPTH, OnFocusDepth)
	ON_UPDATE_COMMAND_UI(IDM_FOCUS_DEPTH, OnUpdateFocusDepth)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBufferView construction/destruction

CBufferView::CBufferView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_typeControl=StencilMask;

}

CBufferView::~CBufferView()
{
}

BOOL CBufferView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CBufferView drawing

void CBufferView::OnDraw(CDC* pDC)
{
	CBufferDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CBufferView diagnostics

#ifdef _DEBUG
void CBufferView::AssertValid() const
{
	CView::AssertValid();
}

void CBufferView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBufferDoc* CBufferView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBufferDoc)));
	return (CBufferDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBufferView message handlers

void CBufferView::Init()
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

BOOL CBufferView::bSetupPixelFormat()
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

int CBufferView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	Init();
	RenderObjects();
	
	return 0;
}

void CBufferView::OnDestroy() 
{
	CView::OnDestroy();
	
	// TODO: Add your message handler code here
	HGLRC	hrc;

	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pDC)
        delete m_pDC;
	
	CView::OnDestroy();
	
}

void CBufferView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
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

void CBufferView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearStencil(0x0);
	glClear(GL_STENCIL_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_STENCIL_TEST);

	switch(m_typeControl)
	{
		case StencilMask:	StencilAppMask();	break;	
		case StencilClip:	StencilAppClip();	break;	
		case AntiAccum:		AccumlateScene();	break;	
		case NonAntiAccum:	NonAccumuScene();	break;
		case Focus:			FocusDepth();		break;
	}
	
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_LIGHTING);
	glFinish();
}

void CBufferView::DrawSphere(GLdouble dRadius)
{
	GLUquadricObj* quadObj;
	quadObj=gluNewQuadric();
	gluQuadricDrawStyle(quadObj,GLU_FILL);
	gluQuadricNormals(quadObj,GLU_SMOOTH);
	gluSphere(quadObj,dRadius,32,32);
	gluDeleteQuadric(quadObj);
}

void CBufferView::StencilAppMask()
{
	GLfloat matDiffuse[] = { 0.8f, 0.8f, 0.0f, 1.0f };
	GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat maskDiffuse[] = { 0.1f, 0.2f, 0.8f, 1.0f };
	GLfloat maskSpecular[] = { 0.1, 1.0, 1.0, 1.0f };

	GLfloat lightPosition[] = { 0.0f, 3.0f, 2.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	GLint iMatList=glGenLists(2);
	glNewList(iMatList, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, maskDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR,maskSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.0f);
	glEndList();

	glNewList(iMatList+1, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
		glMaterialf(GL_FRONT, GL_SHININESS, 70.0f);
	glEndList();

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glDepthFunc(GL_LESS);

	//创建一个正方形模板区域
	glStencilFunc (GL_ALWAYS, 0x1, 0x1);
	glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glBegin(GL_QUADS);
		glVertex3f (-0.7f, 0.0f, 0.0f);
		glVertex3f (0.0f, 0.7f, 0.0f);
		glVertex3f (0.7f, 0.0f, 0.0f);
		glVertex3f (0.0f, -0.7f, 0.0f);
	glEnd();

	//	在模板值为1的地方绘制一个蓝色球
	glStencilFunc (GL_EQUAL, 0x1, 0x1);
	glCallList (iMatList);
	DrawSphere(0.5);

	//	在模板值非1的地方绘制一个黄色球 
	glStencilFunc (GL_NOTEQUAL, 0x1, 0x1);
	glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
		glCallList (iMatList+1);
		DrawSphere(0.9);
	glPopMatrix();
}

void CBufferView::StencilAppClip()
{
	glDepthFunc(GL_LEQUAL);

	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

	glStencilFunc (GL_ALWAYS, 0x1, 0x3);
	glStencilOp (GL_REPLACE, GL_REPLACE, GL_REPLACE);
	
	glPushMatrix();
		glTranslatef(-0.4f,0.0f,0.0f);
		DrawSphere(0.8);
	glPopMatrix();

	glStencilFunc (GL_EQUAL, 0x1, 0x3);
	glStencilOp (GL_KEEP, GL_INCR, GL_INCR);

	glPushMatrix();
		glTranslatef(0.4f,0.0f,0.0f);
		DrawSphere(0.8);
	glPopMatrix();

	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	
	glStencilFunc (GL_EQUAL, 0x2, 0x3);
	glStencilOp (GL_KEEP, GL_KEEP, GL_KEEP);

	glPolygonMode(GL_FRONT,GL_FILL);
	glDisable(GL_LIGHTING);

	glPushMatrix();//
		glTranslatef(0.0f,0.0f,1.0f);
		glBegin(GL_QUADS);
			glColor3f(1.0f,1.0f,1.0f);
			glVertex3f (-0.8f, 0.8f, 0.0f);
			glVertex3f (0.8f, 0.8f, 0.0f);
			glColor3f(0.0f,0.0f,0.0f);
			glVertex3f (0.8f, -0.8f, 0.0f);
			glVertex3f (-0.8f, -0.8f, 0.0f);
		glEnd();
	glPopMatrix();	
}

void CBufferView::OnStencilMask() 
{
	m_typeControl=StencilMask;
	Invalidate();
}

void CBufferView::OnUpdateStencilMask(CCmdUI* pCmdUI) 
{
	if(m_typeControl == StencilMask)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CBufferView::OnStencilClip() 
{
	m_typeControl=StencilClip;
	Invalidate();
}

void CBufferView::OnUpdateStencilClip(CCmdUI* pCmdUI) 
{
	if(m_typeControl == StencilClip)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CBufferView::RenderObjects()
{
	GLfloat matDif[4] = { 0.6f, 0.05f, 0.05f, 1.0f };
	GLfloat matSpe[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glNewList(2,GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe);
		glMaterialf(GL_FRONT, GL_SHININESS, 75.0f);
	glEndList();

	matDif[0]=0.75f; matDif[1]=0.7f; matDif[2]=0.2f;
	glNewList(3,GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe);
		glMaterialf(GL_FRONT, GL_SHININESS, 60.0f);
	glEndList();

	matDif[0]=0.65f; matDif[1]=0.65f; matDif[2]=0.65f;
	glNewList(4,GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe);
		glMaterialf(GL_FRONT, GL_SHININESS, 60.0f);
	glEndList();

	matDif[0]=0.08f; matDif[1]=0.75f; matDif[2]=0.08f;
	glNewList(5,GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe);
		glMaterialf(GL_FRONT, GL_SHININESS, 30.0f);
	glEndList();

	matDif[0]=0.05f; matDif[1]=0.65f; matDif[2]=0.65f;
	glNewList(6,GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	glEndList();

	GLUquadricObj* quadObj;
	quadObj=gluNewQuadric();
	gluQuadricDrawStyle(quadObj,GLU_FILL);
	gluQuadricNormals(quadObj,GLU_SMOOTH);

	glNewList((ACCUMLIST),GL_COMPILE);
		glPushMatrix();
			glTranslatef(-0.8f,-0.3f,1.0f);
			glCallList(2);
			gluSphere(quadObj,0.4,16,16);

			glTranslatef(0.6f,0.0f,-0.7f);
			glCallList(3);
			gluSphere(quadObj,0.4,16,16);

			glTranslatef(0.6f,0.0f,-0.7f);
			glCallList(4);
			gluSphere(quadObj,0.4,16,16);

			glTranslatef(0.6f,0.0f,-0.7f);
			glCallList(5);
			gluSphere(quadObj,0.4,16,16);

			glTranslatef(0.6f,0.0f,-0.7f);
			glCallList(6);
			gluSphere(quadObj,0.4,16,16);
		glPopMatrix();
	glEndList();

}

void CBufferView::AccumlateScene()
{
	GLdouble dOffsetX[16], dOffsetY[16];

	GLdouble dTransRef[16][2] = {
		{0.4375, 0.4375}, {0.1875, 0.5625}, 
		{0.9375, 1.1875}, {0.4375, -0.065}, 
		{0.6875, 0.5625}, {0.1875, 0.0625}, 
		{0.6875, 0.3125}, {0.1875, 0.3125}, 
		{0.4375, 0.1875}, {-0.065, 0.4375}, 
		{0.6875, 0.8125}, {0.4375, 0.6875}, 
		{0.6875, 0.0625}, {0.9375, 0.9375}, 
		{1.1875, 0.8125}, {0.9375, 0.6875}, 
	};

	GLint i, viewport[4];
	GLfloat lightPosition[] = { 0.0f, 3.0f, 2.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glGetIntegerv (GL_VIEWPORT, viewport);

	for (i = 0; i < 16; i++) 
	{
		dOffsetX[i] = dTransRef[i][0]*10/viewport[2];
		dOffsetY[i] = dTransRef[i][1]*10/viewport[3];
	}

	glShadeModel(GL_FLAT);
	glClearAccum(0.0, 0.0, 0.0, 0.0);

	glClear(GL_ACCUM_BUFFER_BIT);

	for (i = 0; i < 16; i++) 
	{
		glPushMatrix();
			glTranslated(dOffsetX[i], dOffsetY[i], 0.0);
			glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			glCallList((ACCUMLIST));
		glPopMatrix();
		glAccum(GL_ACCUM, 1.0/16.0);
		glFlush();
		SwapBuffers(wglGetCurrentDC());
	}

	glAccum(GL_RETURN, 1.0);

	glShadeModel(GL_SMOOTH);

}


void CBufferView::NonAccumuScene()
{
	glShadeModel(GL_FLAT);
	glCallList((ACCUMLIST));
	glShadeModel(GL_SMOOTH);	
}

void CBufferView::OnAntiscene() 
{
	m_typeControl=AntiAccum;
	Invalidate();
}

void CBufferView::OnUpdateAntiscene(CCmdUI* pCmdUI) 
{
	if(m_typeControl == AntiAccum)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CBufferView::OnNonAntiaccum() 
{
	m_typeControl=NonAntiAccum;
	Invalidate();
}

void CBufferView::OnUpdateNonAntiaccum(CCmdUI* pCmdUI) 
{
	if(m_typeControl == NonAntiAccum)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CBufferView::OnFocusDepth() 
{
	m_typeControl=Focus;
	Invalidate();
}

void CBufferView::OnUpdateFocusDepth(CCmdUI* pCmdUI) 
{
	if(m_typeControl == Focus)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CBufferView::myFrustum(GLdouble left, 
		GLdouble right, GLdouble bottom, 
		GLdouble top,   GLdouble znear, 
		GLdouble zfar,  GLdouble eyedx, 
		GLdouble eyedy, GLdouble focus)
{
    GLdouble xwsize, ywsize; 
    GLdouble dx, dy;
    
    xwsize = right - left;
    ywsize = top - bottom;
    
    dx = -eyedx*znear/focus;
    dy = -eyedy*znear/focus;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(left + dx, right + dx, bottom + dy, top + dy, znear, zfar);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef (-eyedx, -eyedy, 0.0);

}

void CBufferView::myPerspective(GLdouble fovy, 
		GLdouble aspect,	GLdouble znear, 
		GLdouble zfar,		GLdouble eyedx, 
		GLdouble eyedy,		GLdouble focus)
{
    GLdouble fov,left,right,bottom,top;

    fov = ((fovy*3.1415926535898) / 180.0) / 2.0;

    top = znear / (cos(fov) / sin(fov));
    bottom = -top;

    right = top * aspect;
    left = -right;

    myFrustum (left, right, bottom, top, znear, zfar,
		eyedx, eyedy, focus);
}

void CBufferView::FocusDepth()
{
	typedef struct 
	{
		GLfloat x, y;
	} OFFSET;

	 OFFSET		eyeOffset[] =
	{
		{-0.33,  0.44},	{ 0.28, -0.39},
		{ 0.46,  0.14},	{-0.41, -0.19},
		{-0.18,  0.08},	{-0.08, -0.32},
		{ 0.10,  0.30},	{ 0.16, -0.05}
	};
    
	int iTimes;
	GLint viewport[4];

	glGetIntegerv (GL_VIEWPORT, viewport);
	if (!viewport[3]) return;

	glShadeModel(GL_FLAT);
	glClearAccum(0.0, 0.0, 0.0, 0.0);
	glClear(GL_ACCUM_BUFFER_BIT);

	for (iTimes = 0; iTimes < 8; iTimes++) 
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		myPerspective (38.0, 
			(GLdouble) viewport[2]/(GLdouble) viewport[3], 
			3, 7.0, 
			0.04*eyeOffset[iTimes].x, 0.04*eyeOffset[iTimes].y,
			2.7);
		glPushMatrix();
			glTranslatef(0.0f,0.0f,-4.5f);
			glCallList((ACCUMLIST));
		glPopMatrix();
		glAccum (GL_ACCUM, 0.125);
		glFlush();
		SwapBuffers(wglGetCurrentDC());
	}

	glAccum (GL_RETURN, 1.0);
	glFlush();

	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glFrustum(-1.0,1.0,-1.0,1.0,3.0,7.0);
}

