// CuvSufcView.cpp : implementation of the CCuvSufcView class
//

#include "stdafx.h"
#include "CuvSufc.h"

#include "CuvSufcDoc.h"
#include "CuvSufcView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcView

IMPLEMENT_DYNCREATE(CCuvSufcView, CView)

BEGIN_MESSAGE_MAP(CCuvSufcView, CView)
	//{{AFX_MSG_MAP(CCuvSufcView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_BEZIER_CURVE, OnBezierCurve)
	ON_UPDATE_COMMAND_UI(IDM_BEZIER_CURVE, OnUpdateBezierCurve)
	ON_COMMAND(IDM_EVEN_BEZIER_CUV, OnEvenBezierCuv)
	ON_UPDATE_COMMAND_UI(IDM_EVEN_BEZIER_CUV, OnUpdateEvenBezierCuv)
	ON_COMMAND(IDM_NURBS_CURVE, OnNurbsCurve)
	ON_UPDATE_COMMAND_UI(IDM_NURBS_CURVE, OnUpdateNurbsCurve)
	ON_COMMAND(IDM_BZRSUFAC_MESH, OnBzrsufacMesh)
	ON_UPDATE_COMMAND_UI(IDM_BZRSUFAC_MESH, OnUpdateBzrsufacMesh)
	ON_COMMAND(IDM_BZRSUFAC_FILL, OnBzrsufacFill)
	ON_UPDATE_COMMAND_UI(IDM_BZRSUFAC_FILL, OnUpdateBzrsufacFill)
	ON_COMMAND(IDM_BZRSUFAC_TEXTURE, OnBzrsufacTexture)
	ON_UPDATE_COMMAND_UI(IDM_BZRSUFAC_TEXTURE, OnUpdateBzrsufacTexture)
	ON_COMMAND(IDM_NURBSSUFAC_MESH, OnNurbssufacMesh)
	ON_UPDATE_COMMAND_UI(IDM_NURBSSUFAC_MESH, OnUpdateNurbssufacMesh)
	ON_COMMAND(IDM_NURBSSUFAC_FILL, OnNurbssufacFill)
	ON_UPDATE_COMMAND_UI(IDM_NURBSSUFAC_FILL, OnUpdateNurbssufacFill)
	ON_COMMAND(IDM_NURBSSUFAC_PACH, OnNurbssufacPach)
	ON_UPDATE_COMMAND_UI(IDM_NURBSSUFAC_PACH, OnUpdateNurbssufacPach)
	ON_COMMAND(IDM_NURBSSUFAC_TRIM, OnNurbssufacTrim)
	ON_UPDATE_COMMAND_UI(IDM_NURBSSUFAC_TRIM, OnUpdateNurbssufacTrim)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcView construction/destruction

CCuvSufcView::CCuvSufcView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	
	m_typeControl=BezierCurve;
	m_nurbsSurfaceMode=GLU_OUTLINE_POLYGON;

}

CCuvSufcView::~CCuvSufcView()
{
}

BOOL CCuvSufcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcView drawing

void CCuvSufcView::OnDraw(CDC* pDC)
{
	CCuvSufcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcView diagnostics

#ifdef _DEBUG
void CCuvSufcView::AssertValid() const
{
	CView::AssertValid();
}

void CCuvSufcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCuvSufcDoc* CCuvSufcView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCuvSufcDoc)));
	return (CCuvSufcDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcView message handlers

void CCuvSufcView::Init()
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

BOOL CCuvSufcView::bSetupPixelFormat()
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

int CCuvSufcView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	Init();
	return 0;
}

void CCuvSufcView::OnDestroy() 
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

void CCuvSufcView::OnSize(UINT nType, int cx, int cy) 
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

void CCuvSufcView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	switch(m_typeControl)
	{
		case BezierCurve:		DrawBezierCurve(); break;
		case EvenBezier:		EvenBezierCurve(); break;
		case NurbsCurve:		DrawNurbsCurve();  break;
		case BezierSurfaceMesh:	BzrSufacMesh();	   break;
		case BezierSurfaceFill: BzrSufacFill();	   break;
		case BezierSurfaceTex:  BzrSufacTex();	   break;
		case NurbsSurfaceMesh:	NurbsSurface();	   break;
		case NurbsSurfaceFill:	NurbsSurface();	   break;
		case NurbsSurfacePach:	NurbsSurface();	   break;
		case NurbsSurfaceTrim:	NurbsSufacTrim();  break;

	}
	glFinish();

}

void CCuvSufcView::DrawBezierCurve()
{
	glColor3f(1.0f, 0.0f, 0.0f);

	//控制点
	GLfloat controlPoints[9][3] = {
		{ -1.0f, 0.0f, 0.0f}, { -0.75f, 0.707f, 0.0f}, 
		{ -0.5f, 1.0f, 0.0f}, { -0.25f, 0.707f, 0.0f},
		{0.0f,0.0f,0.0f},	  { 0.25f, -0.707f, 0.0f}, 
		{ 0.5f, -1.0f, 0.0f}, { 0.75f, -0.707f, 0.0f},
		{1.0f, 0.0f, 0.0f}
	};

	//一维评价器
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 9,(const float*)controlPoints);
 
	//激活
	glEnable(GL_MAP1_VERTEX_3);

	//计算曲线坐标并连接
    glBegin(GL_LINE_STRIP);
		for (int i = 0; i <= 50; i++) 
			glEvalCoord1f((GLfloat) i/(GLfloat)50.0);
    glEnd();

	//挂起
	glDisable(GL_MAP1_VERTEX_3);
}

void CCuvSufcView::EvenBezierCurve()
{
	glColor3f(0.0f, 0.0f, 1.0f);

	//控制点
	GLfloat controlPoints[9][3] = {
		{ -1.0f, 0.0f, 0.0f}, { -0.75f, 0.707f, 0.0f}, 
		{ -0.5f, 1.0f, 0.0f}, { -0.25f, 0.707f, 0.0f},
		{0.0f,0.0f,0.0f},	  { 0.25f, -0.707f, 0.0f}, 
		{ 0.5f, -1.0f, 0.0f}, { 0.75f, -0.707f, 0.0f},
		{1.0f, 0.0f, 0.0f}
	};

    //一维评价器
	glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 9,(const float*)controlPoints);
 
	//激活
	glEnable(GL_MAP1_VERTEX_3);

	//自动等分区间
	glMapGrid1f(50,0.0f,1.0f);

	//绘制曲线
	glEvalMesh1(GL_LINE,0,50);
	
	//挂起
	glDisable(GL_MAP1_VERTEX_3);
	
}

void CCuvSufcView::DrawNurbsCurve()
{
	//控制点
	GLfloat controlPoints[7][3] = {
		{-1.2f, 0.8f, 0.0f}, {-0.8f, -2.0f, 0.0f},
		{-0.4f, 0.4f, 0.0f}, {0.0f, 2.0f, 0.0f},
		{0.4f, 0.4f, 0.0f},  {0.8f, -2.0f, 0.0f},
		{1.2f, 0.8f, 0.0f}
	};

	//节点
	GLfloat knots[14] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
	}; 

	glColor3f (0.0f, 0.0f, 1.0f);

	//创建NURBS对象
	GLUnurbsObj *theNurb;
    theNurb = gluNewNurbsRenderer();

	//属性
	gluNurbsProperty (theNurb, GLU_SAMPLING_TOLERANCE, 10.0);

	//绘制
	glNewList(1, GL_COMPILE);
		gluBeginCurve(theNurb);
			gluNurbsCurve(theNurb,14, knots,3,
				(float*)controlPoints, 
				7,
				GL_MAP1_VERTEX_3);
		gluEndCurve(theNurb);
	glEndList();

	glCallList(1);

}

void CCuvSufcView::OnBezierCurve() 
{
	// TODO: Add your command handler code here
	m_typeControl=BezierCurve;
	Invalidate();
}

void CCuvSufcView::OnUpdateBezierCurve(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl ==BezierCurve)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnEvenBezierCuv() 
{
	// TODO: Add your command handler code here
	m_typeControl=EvenBezier;
	Invalidate();
	
}

void CCuvSufcView::OnUpdateEvenBezierCuv(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == EvenBezier)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnNurbsCurve() 
{
	// TODO: Add your command handler code here
	m_typeControl=NurbsCurve;
	Invalidate();
	
}

void CCuvSufcView::OnUpdateNurbsCurve(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == NurbsCurve)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::BzrSufacMesh()
{
	int i, j;

	//控制点
	GLfloat controlPoints[4][4][3] = {
		{{-0.8f, -0.6f, 0.8f}, {-0.2f, -0.6f, 1.6f}, 
		{0.2f, -0.6f, -0.4f},  {0.6f, -0.6f, 0.8f}}, 
		{{-0.6f, -0.2f, 0.8f}, {-0.2f, -0.2f, 1.6f}, 
		{0.2f, -0.2f, -0.4f},  {0.6f, -0.2f, 0.8f}},
		{{-0.6f, 0.2f, 0.8f},  {-0.2f, 0.2f, 0.4f}, 
		{0.2f, 0.2f, 0.0f},    {0.3f, 0.2f, -0.4f}}, 
		{{-0.6f, 0.6f, 0.8f},  {-0.2f, 0.6f, 0.4f}, 
		{-0.8f, 0.6f, 0.0f},    {0.8f, 0.6f, -0.4f}}
	};

	//二维评价器
    glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4,
	    0.0, 1.0, 12, 4, 
		(const float *)controlPoints);

	//激活
    glEnable(GL_MAP2_VERTEX_3);

    glColor3f(0.0, 0.0, 1.0);

	//绘制
    glPushMatrix ();
		glRotatef(45.0, 1.0, 1.0, 1.0);
		for (j = 0; j <= 8; j++) 
		{
			glBegin(GL_LINE_STRIP);
				for (i = 0; i <= 30; i++)
				glEvalCoord2f((GLfloat)i/30.0, (GLfloat)j/8.0);
			glEnd();
			
			glBegin(GL_LINE_STRIP);
				for (i = 0; i <= 30; i++)
				glEvalCoord2f((GLfloat)j/8.0, (GLfloat)i/30.0);
			glEnd();
  
		}
	
    glPopMatrix ();
    
	//挂起
	glDisable(GL_MAP2_VERTEX_3);

}

void CCuvSufcView::OnBzrsufacMesh() 
{
	// TODO: Add your command handler code here
	m_typeControl=BezierSurfaceMesh;
	Invalidate();
	
}

void CCuvSufcView::OnUpdateBzrsufacMesh(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == BezierSurfaceMesh)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnBzrsufacFill() 
{
	// TODO: Add your command handler code here
	m_typeControl=BezierSurfaceFill;
	Invalidate();
	
}

void CCuvSufcView::OnUpdateBzrsufacFill(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == BezierSurfaceFill)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnBzrsufacTexture() 
{
	// TODO: Add your command handler code here
	m_typeControl=BezierSurfaceTex;
	Invalidate();
	
}

void CCuvSufcView::OnUpdateBzrsufacTexture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == BezierSurfaceTex)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::BzrSufacFill()
{
	//控制点
	GLfloat controlPoints[4][4][3] = {
		{{-0.8f, -0.6f, 0.8f}, {-0.2f, -0.6f, 1.6f}, 
		{0.2f, -0.6f, -0.4f},  {0.6f, -0.6f, 0.8f}}, 
		{{-0.6f, -0.2f, 0.8f}, {-0.2f, -0.2f, 1.6f}, 
		{0.2f, -0.2f, -0.4f},  {0.6f, -0.2f, 0.8f}},
		{{-0.6f, 0.2f, 0.8f},  {-0.2f, 0.2f, 0.4f}, 
		{0.2f, 0.2f, 0.0f},    {0.3f, 0.2f, -0.4f}}, 
		{{-0.6f, 0.6f, 0.8f},  {-0.2f, 0.6f, 0.4f}, 
		{-0.8f, 0.6f, 0.0f},   {0.8f, 0.6f, -0.4f}}
	};

	//光照
	Lighting();

	glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

	//二维评价器
    glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 4,
	    0.0, 1.0, 12, 4, 
		(const float *)controlPoints);

	//自动网格
    glMapGrid2f(30, 0.0, 1.0, 30, 0.0, 1.0);

	//激活
    glEnable(GL_MAP2_VERTEX_3);

	//绘制
    glPushMatrix ();
		glRotatef(45.0, 1.0, 1.0, 1.0);
		glEvalMesh2(GL_FILL, 0, 30, 0, 30);
    glPopMatrix ();

	//挂起
	glDisable(GL_MAP2_VERTEX_3);

	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	 
}

void CCuvSufcView::Lighting()
{
    GLfloat lightAmb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightPos[] = { 1.0f, 1.0f, 2.0f, 1.0f };
    GLfloat matDiffuse[] = { 0.0f, 0.7f, 0.7f, 1.0f };
    GLfloat matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat matShininess[] = { 60.0f };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

}

void CCuvSufcView::BzrSufacTex()
{
	//几何控制点
	GLfloat controlPoints[4][4][3] = {
		{{-0.8f, -0.6f, 0.8f}, {-0.2f, -0.6f, 1.6f}, 
		{0.2f, -0.6f, -0.4f},  {0.6f, -0.6f, 0.8f}}, 
		{{-0.6f, -0.2f, 0.8f}, {-0.2f, -0.2f, 1.6f}, 
		{0.2f, -0.2f, -0.4f},  {0.6f, -0.2f, 0.8f}},
		{{-0.6f, 0.2f, 0.8f},  {-0.2f, 0.2f, 0.4f}, 
		{0.2f, 0.2f, 0.0f},    {0.3f, 0.2f, -0.4f}}, 
		{{-0.6f, 0.6f, 0.8f},  {-0.2f, 0.6f, 0.4f}, 
		{-0.8f, 0.6f, 0.0f},   {0.8f, 0.6f, -0.4f}}
	};

	//纹理控制点
	GLfloat textureCntrlPnt[2][2][2]={
		{{0.0f,0.0f},{0.0f,0.1f}},
		{{1.0f,0.0f},{1.0f,1.0f}}
	};

	//构造纹理
	m_pDib= new CDib("studio.BMP");

	GLubyte* pTextureBits=(GLubyte*)m_pDib->GetDibBitsPtr();
	
	GLint width=m_pDib->GetDibWidth();
	GLint height=m_pDib->GetDibHeight();

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glTexImage2D(GL_TEXTURE_2D,0,3,width,height,
		0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pTextureBits);
	
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

	//二维评价器:	生成几何坐标和纹理坐标
    glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
        0, 1, 12, 4, (const float*)controlPoints);
    glMap2f(GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 
        0, 1, 4, 2, (const float*)textureCntrlPnt);

	//光照
	Lighting();
	
	glEnable(GL_TEXTURE_2D);
    glEnable(GL_MAP2_TEXTURE_COORD_2);
    glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

	//绘制
    glMapGrid2f(30, 0.0, 1.0, 30, 0.0, 1.0);

    glPushMatrix ();
		glScalef(1.2f,1.2f,1.2f);
		glRotatef(45.0f,0.0f,0.0f,1.0f);
		glRotatef(45.0f, 1.0f, 1.0f, 1.0f);
		glEvalMesh2(GL_FILL, 0, 30, 0, 30);
    glPopMatrix ();

	//挂起
	glDisable(GL_MAP2_VERTEX_3);
    glDisable(GL_MAP2_TEXTURE_COORD_2);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	
}

void CCuvSufcView::NurbsSurface()
{
	//控制点
	GLfloat controlPoints[4][4][3] = {
		{{-0.8f, -0.6f, 0.8f}, {-0.2f, -0.6f, 1.6f}, 
		{0.2f, -0.6f, -0.4f},  {0.6f, -0.6f, 0.8f}}, 
		{{-0.6f, -0.2f, 0.8f}, {-0.2f, -0.2f, 1.6f}, 
		{0.2f, -0.2f, -0.4f},  {0.6f, -0.2f, 0.8f}},
		{{-0.6f, 0.2f, 0.8f},  {-0.2f, 0.2f, 0.4f}, 
		{0.2f, 0.2f, 0.0f},    {0.3f, 0.2f, -0.4f}}, 
		{{-0.6f, 0.6f, 0.8f},  {-0.2f, 0.6f, 0.4f}, 
		{-0.8f, 0.6f, 0.0f},   {0.8f, 0.6f, -0.4f}}
	};

	//节点
	GLfloat sknots[8] = {
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

	GLfloat tknots[8] = {
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

	//光照
	Lighting();
	
	glDepthFunc(GL_LESS);
	glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

	//NURBS对象及属性
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
    
	gluNurbsProperty (theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, m_nurbsSurfaceMode);    

	//绘制
	glNewList (1, GL_COMPILE);
		gluBeginSurface(theNurb);
			gluNurbsSurface(theNurb, 
				8, sknots,
				8, tknots,
				3,
				12,
				(float*)controlPoints, 
				4, 4,
				GL_MAP2_VERTEX_3);
		gluEndSurface(theNurb);
    glEndList ();

	glPushMatrix();
		glScalef(1.2f,1.2f,1.2f);
		glRotatef(45.0f,0.0f,0.0f,1.0f);
		glRotatef(45.0f, 1.0f, 1.0f, 1.0f);

		glCallList (1);
	glPopMatrix();

	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);

}

void CCuvSufcView::NurbsSufacTrim()
{
	//控制点
	GLfloat controlPoints[4][4][3] = {
		{{-0.8f, -0.6f, 0.8f}, {-0.2f, -0.6f, 1.6f}, 
		{0.2f, -0.6f, -0.4f},  {0.6f, -0.6f, 0.8f}}, 
		{{-0.6f, -0.2f, 0.8f}, {-0.2f, -0.2f, 1.6f}, 
		{0.2f, -0.2f, -0.4f},  {0.6f, -0.2f, 0.8f}},
		{{-0.6f, 0.2f, 0.8f},  {-0.2f, 0.2f, 0.4f}, 
		{0.2f, 0.2f, 0.0f},    {0.3f, 0.2f, -0.4f}}, 
		{{-0.6f, 0.6f, 0.8f},  {-0.2f, 0.6f, 0.4f}, 
		{-0.8f, 0.6f, 0.0f},   {0.8f, 0.6f, -0.4f}}
	};

	//节点
	GLfloat sknots[8] = {
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

	GLfloat tknots[8] = {
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};

	//外边界:逆时针
	GLfloat edgePoint[5][2] ={
		{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, 
		{0.0f, 1.0f}, {0.0f, 0.0f}
	};

	//内边界:顺时针
	GLfloat pwlPoint[5][2] ={
		{0.75f, 0.75f}, {0.75f, 0.25f}, {0.25f, 0.25f},
		{0.25f,0.75f},{0.75f, 0.75f}
	};
		
	//光照
	Lighting();
	
	glDepthFunc(GL_LESS);
	glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);

	//NURBS对象及属性
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
    
	gluNurbsProperty (theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);    

	//绘制
	glNewList (1, GL_COMPILE);
		gluBeginSurface(theNurb);
			gluNurbsSurface(theNurb, 
				8, sknots,
				8, tknots,
				3,
				12,
				(float*)controlPoints, 
				4, 4,
				GL_MAP2_VERTEX_3);

			gluBeginTrim (theNurb);
				gluPwlCurve (theNurb,  5, 
					(float*)edgePoint, 2, 
					GLU_MAP1_TRIM_2);
			gluEndTrim (theNurb);

			gluBeginTrim (theNurb);
				gluPwlCurve (theNurb, 5, 
					(float*)pwlPoint, 2, 
					GLU_MAP1_TRIM_2);
			gluEndTrim (theNurb);

		gluEndSurface(theNurb);
    glEndList ();

	glPushMatrix();
		glScalef(1.2f,1.2f,1.2f);
		glRotatef(45.0f,0.0f,0.0f,1.0f);
		glRotatef(45.0f, 1.0f, 1.0f, 1.0f);
		glCallList (1);
	glPopMatrix();

	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);

}


void CCuvSufcView::OnNurbssufacMesh() 
{
	m_typeControl=NurbsSurfaceMesh;
	m_nurbsSurfaceMode=GLU_OUTLINE_POLYGON;
	Invalidate();
}

void CCuvSufcView::OnUpdateNurbssufacMesh(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == NurbsSurfaceMesh)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnNurbssufacFill() 
{
	m_typeControl=NurbsSurfaceFill;
	m_nurbsSurfaceMode=GLU_FILL;
	Invalidate();
}

void CCuvSufcView::OnUpdateNurbssufacFill(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == NurbsSurfaceFill)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnNurbssufacPach() 
{
	m_typeControl=NurbsSurfacePach;
	m_nurbsSurfaceMode=GLU_OUTLINE_PATCH;
	Invalidate();
}

void CCuvSufcView::OnUpdateNurbssufacPach(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == NurbsSurfacePach)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CCuvSufcView::OnNurbssufacTrim() 
{
	m_typeControl=NurbsSurfaceTrim;
	Invalidate();
}

void CCuvSufcView::OnUpdateNurbssufacTrim(CCmdUI* pCmdUI) 
{
	if(m_typeControl == NurbsSurfaceTrim)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}
 
