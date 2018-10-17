// EffectView.cpp : implementation of the CEffectView class
//

#include "stdafx.h"
#include "Effect.h"

#include "EffectDoc.h"
#include "EffectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectView

IMPLEMENT_DYNCREATE(CEffectView, CView)

BEGIN_MESSAGE_MAP(CEffectView, CView)
	//{{AFX_MSG_MAP(CEffectView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_ALPHA_2D, OnAlpha2d)
	ON_UPDATE_COMMAND_UI(IDM_ALPHA_2D, OnUpdateAlpha2d)
	ON_COMMAND(IDM_ALPHA_3D, OnAlpha3d)
	ON_UPDATE_COMMAND_UI(IDM_ALPHA_3D, OnUpdateAlpha3d)
	ON_COMMAND(IDM_ANTI_LINE, OnAntiLine)
	ON_UPDATE_COMMAND_UI(IDM_ANTI_LINE, OnUpdateAntiLine)
	ON_COMMAND(IDM_ANTI_PLGN, OnAntiPlgn)
	ON_UPDATE_COMMAND_UI(IDM_ANTI_PLGN, OnUpdateAntiPlgn)
	ON_COMMAND(IDM_ANTI_CUBE, OnAntiCube)
	ON_UPDATE_COMMAND_UI(IDM_ANTI_CUBE, OnUpdateAntiCube)

	ON_COMMAND(IDM_FOG, OnFog)
	ON_UPDATE_COMMAND_UI(IDM_FOG, OnUpdateFog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectView construction/destruction

CEffectView::CEffectView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_typeControl=Alpha2D;
}

CEffectView::~CEffectView()
{
}

BOOL CEffectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectView drawing

void CEffectView::OnDraw(CDC* pDC)
{
	CEffectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CEffectView diagnostics

#ifdef _DEBUG
void CEffectView::AssertValid() const
{
	CView::AssertValid();
}

void CEffectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEffectDoc* CEffectView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEffectDoc)));
	return (CEffectDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffectView message handlers

void CEffectView::Init()
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

BOOL CEffectView::bSetupPixelFormat()
{
static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
        1,                              // version number
        PFD_DRAW_TO_WINDOW |            // support window
          PFD_SUPPORT_OPENGL,			// support OpenGL
        PFD_TYPE_RGBA,                  // RGBA type
        24,                             // 24-bit color depth
        0, 0, 0, 0, 0, 0,               // color bits ignored
        0,                              // no alpha buffer
        0,                              // shift bit ignored
        0,                              // no accumulation buffer
        0, 0, 0, 0,                     // accum bits ignored
        32,                             // 32-bit z-buffer
        0,                              // no stencil buffer
        0,                              // no auxiliary buffer
        PFD_MAIN_PLANE,                 // main layer
        0,                              // reserved
        0, 0, 0                         // layer masks ignored
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

int CEffectView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	Init();
	return 0;
}

void CEffectView::OnDestroy() 
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

void CEffectView::OnSize(UINT nType, int cx, int cy) 
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

void CEffectView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);
	
	switch(m_typeControl)
	{
		case Alpha2D:		BlendQuads();			break;	
		case Alpha3D:		BlendObjects();			break;	
		case AntialiasLine:	AntialiasLineSphere();  break;
		case AntialiasPlgn:	AntialiasTriangle();	break;
		case AntialiasCube:	AntiCube();				break;
		case Fog:			FogScene();				break;
	}

	glFlush();

}

void CEffectView::BlendQuads()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);


	glColor4f(0.0f,1.0f,0.0f,0.75f);
	glRectf(-0.7f,0.3f,0.3f,-0.7f);

	glColor4f(0.0f,0.0f,1.0f,0.5f);
	glPushMatrix();
		glTranslatef(0.0f,0.0f,0.05f);
		glRectf(-0.1f,1.0f,1.0f,-0.1f);
	glPopMatrix();

	glDisable(GL_BLEND)	;

}

void CEffectView::OnAlpha2d() 
{
	// TODO: Add your command handler code here
	m_typeControl=Alpha2D;
	Invalidate();

}

void CEffectView::OnUpdateAlpha2d(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == Alpha2D)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CEffectView::OnAlpha3d() 
{
	// TODO: Add your command handler code here
	m_typeControl=Alpha3D;
	Invalidate();
	
}

void CEffectView::OnUpdateAlpha3d(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == Alpha3D)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CEffectView::OnAntiLine() 
{
	// TODO: Add your command handler code here
	m_typeControl=AntialiasLine;
	Invalidate();
	
}

void CEffectView::OnUpdateAntiLine(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == AntialiasLine)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}
void CEffectView::OnAntiPlgn() 
{
	// TODO: Add your command handler code here
	m_typeControl=AntialiasPlgn;
	Invalidate();
	
}

void CEffectView::OnUpdateAntiPlgn(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == AntialiasPlgn)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CEffectView::OnAntiCube() 
{
	// TODO: Add your command handler code here
	m_typeControl=AntialiasCube;
	Invalidate();

}

void CEffectView::OnUpdateAntiCube(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == AntialiasCube)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}


void CEffectView::OnFog() 
{
	// TODO: Add your command handler code here
	m_typeControl=Fog;
	Invalidate();
	
}

void CEffectView::OnUpdateFog(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == Fog)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CEffectView::BlendObjects()
{
    //光的位置
	GLfloat lightPosition[] = { 0.6f, 0.5f, 1.0f, 0.0f };
    
	//不透明物体材质
	GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 0.15f };
    GLfloat matShininess[] = { 100.0f };
	GLfloat matSolid[] = { 0.75f, 0.75f, 0.0f, 1.0f };
	GLfloat matZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	//透明物体材质
	GLfloat matTransparent[] = { 0.0f, 0.8f, 0.8f, 0.6f };
	GLfloat matEmission[] = { 0.0f, 0.3f, 0.3f, 0.6f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//球
    GLint	sphereList=Sphere(0.7);

	//玻璃
	GLint	cubeList = glGenLists(1);
	glNewList(cubeList, GL_COMPILE);
		DrawCube(2.0f,0.01f,1.6f);
	glEndList();

	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	//第一步:启动深度测试
	glEnable(GL_DEPTH_TEST);

	//第二步:绘制不透明体
	glPushMatrix ();
		glTranslatef (0.0f, 0.0f, 0.05f);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
		glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
		glMaterialfv(GL_FRONT, GL_EMISSION, matZero);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matSolid);
		glCallList(sphereList);
	glPopMatrix();
	
	//第三步:设置深度缓存为只读模式,并启动融合
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	//第四步:绘制透明体
	glPushMatrix();
		glTranslatef (-0.8f, -0.8f, 1.0f);
		glRotatef(5.0f, 1.0f, 1.0f, 0.0f);
		glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
		glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matTransparent);
		glCallList(cubeList);
	glPopMatrix();

	//第五步:恢复深度缓存的可读可写属性
	glDepthMask(GL_TRUE);
	
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

}

GLint CEffectView::Sphere(GLdouble radius)
{
	//球
    GLint	sphereList = glGenLists(1);
	GLUquadricObj* quadObj;
    glNewList(sphereList, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluSphere(quadObj,radius,32,32);
	glEndList();

	return sphereList;
}

void CEffectView::DrawCube(GLdouble dLength, GLdouble dWidth, GLdouble dHeight)
{
	glBegin(GL_QUAD_STRIP);
		glVertex3d(0.0, dHeight, dWidth);
        glVertex3d(0.0, 0.0, dWidth);

		glVertex3d(dLength, dHeight, dWidth);
        glVertex3d(dLength, 0.0, dWidth);

		glVertex3d(dLength, dHeight, 0.0);
		glVertex3d(dLength, 0.0, 0.0);
		
	    glVertex3d(0.0, dHeight, 0.0);
        glVertex3d(0.0, 0.0,0.0);

		glVertex3d(0.0, dHeight, dWidth);
        glVertex3d(0.0, 0.0, dWidth);

	glEnd();

    glBegin(GL_QUADS);
		glVertex3d(0.0, dHeight, 0.0);
		glVertex3d(0.0, dHeight, dWidth);
		glVertex3d(dLength, dHeight, dWidth);
        glVertex3d(dLength, dHeight, 0.0);
    glEnd();

    glBegin(GL_QUADS);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(dLength, 0.0, 0.0);
		glVertex3d(dLength, 0.0, dWidth);
		glVertex3d(0.0, 0.0, dWidth);
    glEnd();
}

void CEffectView::AntialiasLineSphere()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//光
	GLfloat lightPosition[] = { 0.5f, 0.5f, 1.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);	

	//材质
	GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat matShininess[] = { 100.0f };
	GLfloat matSolid[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat matZero[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, matZero);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matSolid);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//启动线反走样功能
	glEnable(GL_LINE_SMOOTH);

	//利用融合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	//行为控制
	glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	
	glLineWidth(1.0f);

    //线型球
	GLint	sphereList = glGenLists(1);
	GLUquadricObj* quadObj;
    glNewList(sphereList, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_LINE);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluSphere(quadObj,0.9,16,16);
	glEndList();
	
	glCallList(sphereList);

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);

}

void CEffectView::AntialiasTriangle()
{
	//启动多边形反走样功能
	glEnable(GL_POLYGON_SMOOTH);

	//利用融合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	//行为控制
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

	glPushMatrix();
		glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_TRIANGLES);
			glColor4f(0.2f,0.5f,1.0f,0.8f);
			glVertex2d(-0.8, 0.8);
			glVertex2d(-0.8, -0.8);
			glVertex2d(0.8, -0.8);
		glEnd();
	glPopMatrix();

	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_BLEND);

}

void CEffectView::FogScene()
{
	glClearColor(0.5, 0.5, 0.5, 1.0);

	//光照与材质
	GLfloat position[] = { 0.5, 0.5, 3.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	{
	  	GLfloat material[3] = {0.2f, 0.05f, 0.05f};
		
		glMaterialfv (GL_FRONT, GL_AMBIENT, material);
		
		material[0] = 0.8f;
		material[1] = 0.1f;
		material[2] = 0.05f;	
		glMaterialfv (GL_FRONT, GL_DIFFUSE, material);

		material[0] = 0.8f; 
		material[1] = 0.8f; 
		material[2] = 0.8f;
		glMaterialfv (GL_FRONT, GL_SPECULAR, material);
		glMaterialf (GL_FRONT, GL_SHININESS, 60.0);
	}

	//雾化
	glEnable(GL_FOG);
	{
		GLfloat fogColor[4] = {1.0, 1.0, 1.0, 1.0};
		glFogi (GL_FOG_MODE, GL_LINEAR);
		glFogfv (GL_FOG_COLOR, fogColor);
		glFogf (GL_FOG_DENSITY, 0.35);
		glFogf (GL_FOG_START, 1.0);
		glFogf (GL_FOG_END, 7.0);
		glHint (GL_FOG_HINT, GL_DONT_CARE);
	}
	
	//场景
	GLint sphere1=Sphere(0.6);
	GLint sphere2=Sphere(0.4);
	GLint sphere3=Sphere(0.3);
	GLint sphere4=Sphere(0.2);
	
	glPushMatrix();
		
		glTranslatef(0.0f,-0.5f,0.0f);
		glCallList(sphere1);

		glTranslatef(-1.0f,0.0f,-0.0f);
		glCallList(sphere3);

		glTranslatef(2.0f,0.0f,0.0f);
		glCallList(sphere3);

		glTranslatef(0.5f,0.5f,-2.0f);
		glCallList(sphere2);

		glTranslatef(-1.0f,0.0f,0.0f);
		glCallList(sphere4);

		glTranslatef(-1.6f,0.0f,0.0f);
		glRotatef(35.0f, 1.0f, 1.0f, 0.0f);
		glRotatef(10.0f, 0.0f, 1.0f, 0.0f);
		DrawCube(1.0f,1.0f,1.f);

		glTranslatef(1.6f,0.0f,0.0f);
		DrawCube(0.5f,1.0f,1.f);

	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_FOG);
}

void CEffectView::AntiCube()
{
	//启动多边形反走样功能
	glEnable(GL_POLYGON_SMOOTH);

	//利用融合
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	//行为控制
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

	glPushMatrix();
		glTranslatef(-0.3f,-0.5f,0.0f);
		glRotatef(35.0f, 0.0f, 0.0f, 1.0f);
		glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(20.0f, 0.0f, 1.0f, 0.0f);

		DrawCube(1.0f,1.0f,1.0f);
	glPopMatrix();

	glDisable(GL_POLYGON_SMOOTH);
	glDisable(GL_BLEND);

}
