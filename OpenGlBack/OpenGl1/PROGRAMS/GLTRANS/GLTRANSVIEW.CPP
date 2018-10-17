// GLTransView.cpp : implementation of the CGLTransView class
//

#include "stdafx.h"
#include "GLTrans.h"
#include "math.h"

#include "GLTransDoc.h"
#include "GLTransView.h"

#define ABS(x) ((x)<0 ? -(x) : (x)>0 ? (x) : 0)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLTransView

IMPLEMENT_DYNCREATE(CGLTransView, CView)

BEGIN_MESSAGE_MAP(CGLTransView, CView)
	//{{AFX_MSG_MAP(CGLTransView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_CUBE, OnCube)
	ON_UPDATE_COMMAND_UI(IDM_CUBE, OnUpdateCube)
	ON_COMMAND(IDM_TRANSLATE, OnTranslate)
	ON_UPDATE_COMMAND_UI(IDM_TRANSLATE, OnUpdateTranslate)
	ON_COMMAND(IDM_ROTATE, OnRotate)
	ON_UPDATE_COMMAND_UI(IDM_ROTATE, OnUpdateRotate)
	ON_COMMAND(IDM_SCALE, OnScale)
	ON_UPDATE_COMMAND_UI(IDM_SCALE, OnUpdateScale)
	ON_COMMAND(IDM_CLIP, OnClip)
	ON_UPDATE_COMMAND_UI(IDM_CLIP, OnUpdateClip)
	ON_COMMAND(IDM_ASSEMBLE, OnAssemble)
	ON_UPDATE_COMMAND_UI(IDM_ASSEMBLE, OnUpdateAssemble)
	ON_COMMAND(IDM_MATRIX_STACK, OnMatrixStack)
	ON_UPDATE_COMMAND_UI(IDM_MATRIX_STACK, OnUpdateMatrixStack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLTransView construction/destruction

CGLTransView::CGLTransView()
{
	// TODO: add construction code here
	//	m_controlTrans=Cube;

	m_pDC=NULL;

}

CGLTransView::~CGLTransView()
{
}

BOOL CGLTransView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CGLTransView drawing

void CGLTransView::OnDraw(CDC* pDC)
{
	CGLTransDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CGLTransView diagnostics

#ifdef _DEBUG
void CGLTransView::AssertValid() const
{
	CView::AssertValid();
}

void CGLTransView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CGLTransDoc* CGLTransView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CGLTransDoc)));
	return (CGLTransDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGLTransView message handlers

void CGLTransView::Init()
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

BOOL CGLTransView::bSetupPixelFormat()
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

int CGLTransView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	Init();

	return 0;
}

void CGLTransView::OnDestroy() 
{
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

void CGLTransView::OnSize(UINT nType, int cx, int cy) 
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

void CGLTransView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	switch(m_controlTrans)
	{
		case Cube:			DrawCube(0.5);break;
		case Translate:		PfmTranslate();break;
		case Rotate:		PfmRotate();break;
		case Scale:			PfmScale();break;
		case Clip:			PfmClip();break;
		case Assemble:		PfmAssemble();break;
		case Truck:			PfmMatrixStack();break;
	}

	glFinish();

}

void CGLTransView::DrawCube(GLdouble dLength)
{
	GLdouble dAbsLength;
	dAbsLength=ABS(dLength);

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.5f);
	
	glColor3f(0.2f,0.5f,1.0f);

	glBegin(GL_QUAD_STRIP);
		glVertex3d(-dAbsLength, dAbsLength, dAbsLength);
        glVertex3d(-dAbsLength, -dAbsLength, dAbsLength);

		glVertex3d(dAbsLength, dAbsLength, dAbsLength);
        glVertex3d(dAbsLength, -dAbsLength, dAbsLength);

		glVertex3d(dAbsLength, dAbsLength, -dAbsLength);
		glVertex3d(dAbsLength, -dAbsLength, -dAbsLength);
		
	    glVertex3d(-dAbsLength, dAbsLength, -dAbsLength);
        glVertex3d(-dAbsLength, -dAbsLength,  -dAbsLength);

		glVertex3d(-dAbsLength, dAbsLength, dAbsLength);
        glVertex3d(-dAbsLength, -dAbsLength, dAbsLength);

	glEnd();

    glBegin(GL_QUADS);
		glVertex3d(-dAbsLength, dAbsLength, dAbsLength);
		glVertex3d(dAbsLength, dAbsLength, dAbsLength);
		glVertex3d(dAbsLength, dAbsLength, -dAbsLength);
        glVertex3d(-dAbsLength, dAbsLength, -dAbsLength);
    glEnd();

    glBegin(GL_QUADS);
	    glVertex3d(-dAbsLength, -dAbsLength, dAbsLength);
	    glVertex3d(dAbsLength, -dAbsLength, dAbsLength);
		glVertex3d(dAbsLength, -dAbsLength, -dAbsLength);
        glVertex3d(-dAbsLength, -dAbsLength,  -dAbsLength);
    glEnd();

}

void CGLTransView::OnCube() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Cube;
	Invalidate();
	
}

void CGLTransView::OnUpdateCube(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Cube)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CGLTransView::OnTranslate() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Translate;
	Invalidate();
	
}

void CGLTransView::OnUpdateTranslate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Translate)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}


void CGLTransView::PfmTranslate()
{
	glPushMatrix();
		glTranslatef(0.2f,0.3f,1.0f);
		DrawCube(0.5);
	glPopMatrix();
}

void CGLTransView::OnRotate() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Rotate;
	Invalidate();
	
}

void CGLTransView::OnUpdateRotate(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Rotate)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CGLTransView::PfmRotate()
{
	glPushMatrix();
		glRotatef(60.0f,0.0f,1.0f,0.0f);
		glRotated(10.0,1.0,0.0,0.0);
		glRotatef(15.0f,0.0f,0.0f,1.0f);
		DrawCube(0.5);
	glPopMatrix();

}

void CGLTransView::OnScale() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Scale;
	Invalidate();
	
}

void CGLTransView::OnUpdateScale(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Scale)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CGLTransView::PfmScale()
{
	glPushMatrix();
		glScalef(1.5f,1.0f,1.0f);
		DrawCube(0.5);
	glPopMatrix();

}

void CGLTransView::OnClip() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Clip;
	Invalidate();
	
}

void CGLTransView::OnUpdateClip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Clip)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CGLTransView::PfmClip()
{
	GLdouble dPlaneEquation[4]={1.0,1.0,1.0,-0.1};
	
	glPushMatrix();

		glClipPlane(GL_CLIP_PLANE0,dPlaneEquation);
		glEnable(GL_CLIP_PLANE0);

		glRotatef(60.0f,0.0f,1.0f,0.0f);
		glRotated(10.0,1.0,0.0,0.0);
		glRotatef(15.0f,0.0f,0.0f,1.0f);
		DrawCube(0.5);

		glDisable(GL_CLIP_PLANE0);

	glPopMatrix();

}

void CGLTransView::OnAssemble() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Assemble;
	Invalidate();
	
}

void CGLTransView::OnUpdateAssemble(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Assemble)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CGLTransView::PfmAssemble()
{
	glPushMatrix();
		DrawCube(0.3);
		glTranslatef(0.8f,0.0f,0.0f);
		DrawCube(0.3);
		glTranslatef(-1.6f,0.0f,0.0f);
		DrawCube(0.3);
		glTranslatef(0.8f,0.8f,0.0f);
		DrawCube(0.3);
	glPopMatrix();

	glPushMatrix();
		//向下平移0.8个单位.
		glTranslatef(0.0f,-0.8f,0.0f);
		DrawCube(0.3);
		glTranslatef(0.8f,0.0f,0.0f);
		glRotatef(90.0f,0.0f,1.0f,0.0f);
		DrawCube(0.3);
		glTranslatef(0.0f,0.0f,-1.6f);
		DrawCube(0.3);
		glTranslatef(0.0f,0.8f,0.8f);
		DrawCube(0.3);
	glPopMatrix();

}

void CGLTransView::OnMatrixStack() 
{
	// TODO: Add your command handler code here
	m_controlTrans=Truck;
	Invalidate();
	
}

void CGLTransView::OnUpdateMatrixStack(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_controlTrans== Truck)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CGLTransView::PfmMatrixStack()
{	
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.5f);
	
	glColor3f(0.2f,0.5f,1.0f);

	glPushMatrix();
		glTranslatef(0.4f,0.0f,0.0f);

		//主货箱
		DrawQuadrangle(0.4,0.8);

		//连接主货箱与拖箱
		glBegin(GL_LINES);
			glVertex2f(-0.4f,0.0f);
			glVertex2f(-0.7f,0.0f);
		glEnd();

		//主货箱的两个轮子
		glPushMatrix();
			glTranslatef(0.0f,-0.3f,0.0f);
			glTranslatef(-0.3f,0.0f,0.0f);
			DrawWheel(0.1);
			glTranslatef(0.6f,0.0f,0.0f);
			DrawWheel(0.1);
		glPopMatrix();

		glPushMatrix();
			glTranslatef(-1.f,0.0f,0.0f);

			//拖箱
			DrawQuadrangle(0.4,0.6);
			
			//拖箱的两个轮子
			glPushMatrix();
				glTranslatef(0.0f,-0.3f,0.0f);
				glTranslatef(-0.2f,0.0f,0.0f);
				DrawWheel(0.1);
				glTranslatef(0.4f,0.0f,0.0f);
				DrawWheel(0.1);
			glPopMatrix();

		glPopMatrix();

	glPopMatrix();

	
}

void CGLTransView::DrawQuadrangle(GLdouble dQuadWidth,GLdouble dQuadHeight)
{
	GLdouble dWidth,dHeight,dTemp;
	
	dWidth=ABS(dQuadWidth);
	dHeight=ABS(dQuadHeight);

	if(dWidth<dHeight)
	{
		dTemp=dWidth;
		dWidth=dHeight;
		dHeight=dTemp;
	}

	if(dHeight==0.0)
	{
	   MessageBox("Error:	the Length of Quadrangle is 0.0");
        return;
	}

	dWidth=dWidth/2.0;
	dHeight=dHeight/2.0;

	glBegin(GL_QUADS);
		glVertex2d(-dWidth,dHeight);
		glVertex2d(-dWidth,-dHeight);
		glVertex2d(dWidth,-dHeight);
		glVertex2d(dWidth,dHeight);
	glEnd();
}

void CGLTransView::DrawWheel(GLdouble dWheelRadius)
{
	GLdouble dRadius=ABS(dWheelRadius);
	GLdouble dStep=0.1;
	GLdouble dTwoPie=2.0*3.14159265+dStep;

	glBegin(GL_LINE_STRIP);
		glVertex2d(dRadius,0.0f);
	for(GLdouble dIndex=dStep;dIndex<=dTwoPie;dIndex+=dStep)
		glVertex2d(dRadius*cos(dIndex),dRadius*sin(dIndex));
	glEnd();
}
