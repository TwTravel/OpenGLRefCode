// UtilityView.cpp : implementation of the CUtilityView class
//
#include "WINDOWS.H" 
#include "stdafx.h"
#include "Utility.h"

#include "UtilityDoc.h"
#include "UtilityView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CUtilityView

IMPLEMENT_DYNCREATE(CUtilityView, CView)

BEGIN_MESSAGE_MAP(CUtilityView, CView)
	//{{AFX_MSG_MAP(CUtilityView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_QUADRICS, OnQuadrics)
	ON_UPDATE_COMMAND_UI(IDM_QUADRICS, OnUpdateQuadrics)
	ON_COMMAND(IDM_SCALE_IMAGE, OnScaleImage)
	ON_UPDATE_COMMAND_UI(IDM_SCALE_IMAGE, OnUpdateScaleImage)
	ON_COMMAND(IDM_AUTO_MIPMAPPING, OnAutoMipmapping)
	ON_UPDATE_COMMAND_UI(IDM_AUTO_MIPMAPPING, OnUpdateAutoMipmapping)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUtilityView construction/destruction

CUtilityView::CUtilityView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_typeControl=Quadrics;
	m_iWindowWidth=400;
	m_iWindowHeight=435;
}

CUtilityView::~CUtilityView()
{
}

BOOL CUtilityView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CUtilityView drawing

void CUtilityView::OnDraw(CDC* pDC)
{
	CUtilityDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CUtilityView diagnostics

#ifdef _DEBUG
void CUtilityView::AssertValid() const
{
	CView::AssertValid();
}

void CUtilityView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUtilityDoc* CUtilityView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUtilityDoc)));
	return (CUtilityDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUtilityView message handlers

void CUtilityView::Init()
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

BOOL CUtilityView::bSetupPixelFormat()
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

int CUtilityView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	Init();
	return 0;
}

void CUtilityView::OnDestroy() 
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

void CUtilityView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	m_iWindowWidth=cx;
	m_iWindowHeight=cy;
	
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

void CUtilityView::DrawScene()
{
	BeginWaitCursor();
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);
	
	switch(m_typeControl)
	{
		case Quadrics:		DrawQuadrics();		break;
		case ScaleDib:		ScaleImage();		break;
		case AutoMipmap:	AutoMipmapping();   break;
	}

	glFinish();
	EndWaitCursor();
}

void CUtilityView::DrawQuadrics()
{
	//光照与材质
	GLfloat position[] = { 0.0, 0.0, 3.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	{
	  	GLfloat material[3] = {0.2f, 0.05f, 0.05f};
		
		glMaterialfv (GL_FRONT, GL_AMBIENT, material);
		
		material[0] = 1.0f;
		material[1] = 0.2f;
		material[2] = 0.1f;	
		glMaterialfv (GL_FRONT, GL_DIFFUSE, material);

		material[0] = 0.8f; 
		material[1] = 0.8f; 
		material[2] = 0.8f;
		glMaterialfv (GL_FRONT, GL_SPECULAR, material);
		glMaterialf (GL_FRONT, GL_SHININESS, 60.0);
	}

	GLUquadricObj* quadObj;
 
	//点球
	glNewList(1, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_POINT);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluSphere(quadObj, 0.4,32,32);
	glEndList();

    //圆锥
	glNewList(2, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluCylinder(quadObj,0.0,0.3,0.8,16,16);
	glEndList();
	
	//圆柱
    glNewList(3, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluCylinder(quadObj,0.3,0.3,0.8,16,16);
	glEndList();

	//台锥
    glNewList(4, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluCylinder(quadObj,0.1,0.3,0.6,16,16);
	glEndList();
	
	//圆环
    glNewList(5, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluQuadricOrientation(quadObj,GLU_OUTSIDE);
		gluDisk(quadObj,0.1,0.3,16,16);
	glEndList();

	//圆盘
    glNewList(6, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluDisk(quadObj,0.0,0.3,16,16);
	glEndList();
	
	//扇形
    glNewList(7, GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluPartialDisk(quadObj,0.1,0.3,16,16,0.0,60);
	glEndList();

	glPushMatrix();
		glTranslatef(-1.0f,0.8f,0.0f);
		glCallList(5);
		glTranslatef(0.7f,0.0f,0.0f);
		glCallList(6);
		glTranslatef(0.7f,0.0f,0.0f);
		glCallList(7);	

		glTranslatef(-0.4f,-0.8f,0.0f);
		glRotatef(90.0f,1.0f,0.0f,0.0f);
		glCallList(2);
		glTranslatef(-1.0f,0.0f,0.0f);
		glCallList(1);
		glTranslatef(2.0f,0.0f,0.0f);
		glCallList(3);
		glTranslatef(0.0f,0.0f,-1.0f);
		glCallList(4);
	glPopMatrix();
}

void CUtilityView::OnQuadrics() 
{
	// TODO: Add your command handler code here
	m_typeControl=Quadrics;
	Invalidate();

}

void CUtilityView::OnUpdateQuadrics(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == Quadrics)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CUtilityView::OnScaleImage() 
{
	// TODO: Add your command handler code here
	m_typeControl=ScaleDib;
	Invalidate();
	
}

void CUtilityView::OnUpdateScaleImage(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == ScaleDib)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CUtilityView::OnAutoMipmapping() 
{
	// TODO: Add your command handler code here
	m_typeControl=AutoMipmap;
	Invalidate();

}

void CUtilityView::OnUpdateAutoMipmapping(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == AutoMipmap)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CUtilityView::ScaleImage()
{
	m_pDib= new CDib("Wheat.BMP");
	
	GLubyte* pDibBits=(GLubyte*)m_pDib->GetDibBitsPtr();
	
	GLint width=m_pDib->GetDibWidth();
	GLint height=m_pDib->GetDibHeight();

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
	GLubyte*	image;
	image= new GLubyte[width*height*12];
	
	ASSERT(image != NULL);
	
	gluScaleImage(GL_BGR_EXT,width,height,GL_UNSIGNED_BYTE,pDibBits,
		2*width,2*height,GL_UNSIGNED_BYTE,image);

	glRasterPos3f(-1.5f,-1.5f,0.0f);
	glDrawPixels(2*width,2*height,GL_BGR_EXT,GL_UNSIGNED_BYTE,image);

	delete []image;
}

void CUtilityView::AutoMipmapping()
{
	int i,j;
	GLubyte mipmap32[32][32][4];
	GLubyte mipmap16[16][16][4];
	GLubyte mipmap8[8][8][4];
	GLubyte mipmap4[4][4][4];
	GLubyte mipmap2[2][2][4];
	GLubyte mipmap1[1][1][4];

	//作Mip贴图的图像数据
    for (i = 0; i < 32; i++) {
    for (j = 0; j < 32; j++) {
        mipmap32[i][j][0] = 50;
        mipmap32[i][j][1] = 125;
        mipmap32[i][j][2] = 255;
		mipmap32[i][j][3] = 255;
    }
    }

    for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
        mipmap16[i][j][0] = 200;
        mipmap16[i][j][1] = 200;
        mipmap16[i][j][2] = 0;
		mipmap16[i][j][3] = 255;
    }
    }
    for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
        mipmap8[i][j][0] = 255;
        mipmap8[i][j][1] = 0;
        mipmap8[i][j][2] = 0;
		mipmap8[i][j][3] = 255;
    }
    }

    for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
        mipmap4[i][j][0] = 0;
        mipmap4[i][j][1] = 200;
        mipmap4[i][j][2] = 0;
		mipmap4[i][j][3] = 255;
    }
    }

    for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
        mipmap2[i][j][0] = 0;
        mipmap2[i][j][1] = 0;
        mipmap2[i][j][2] = 200;
		mipmap2[i][j][3] = 255;
    }
    }

    mipmap1[0][0][0] = 255;
    mipmap1[0][0][1] = 255;
    mipmap1[0][0][2] = 255;
	mipmap1[0][0][3] = 255;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_FLAT);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//第零级
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,32,32,GL_RGBA,
		GL_UNSIGNED_BYTE,mipmap32);

	//第一级
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,16,16,GL_RGBA,
		GL_UNSIGNED_BYTE,mipmap16);
	
	//第二级
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,8,8,GL_RGBA,
		GL_UNSIGNED_BYTE,mipmap8);

	//第三级
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,4,4,GL_RGBA,
		GL_UNSIGNED_BYTE,mipmap4);

	//第四级
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,2,2,GL_RGBA,
		GL_UNSIGNED_BYTE,mipmap2);

	//第五级
	gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,1,1,GL_RGBA,
		GL_UNSIGNED_BYTE,mipmap1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap32);
	glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 16, 16, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap16);
    glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, 8, 8, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap8);
    glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, 4, 4, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap4);
    glTexImage2D(GL_TEXTURE_2D, 4, GL_RGBA, 2, 2, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap2);
    glTexImage2D(GL_TEXTURE_2D, 5, GL_RGBA, 1, 1, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap1);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   
	glEnable(GL_TEXTURE_2D);

	//放大视线,体现Mip贴图的作用
    gluPerspective(60.0, 1.0*(GLfloat)m_iWindowWidth/(GLfloat)m_iWindowHeight, 1.0, 30000.0);

	//Mip贴图处理的物体
    glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(-2.0, -1.0, 0.0);
		glTexCoord2f(0.0, 5.0); glVertex3f(-2.0, 1.0, 0.0);
		glTexCoord2f(5.0, 5.0); glVertex3f(1500.0, 1.0, -5000.0);
		glTexCoord2f(5.0, 0.0); glVertex3f(1500.0, -1.0, -5000.0);
    glEnd();

	glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	//视线回位
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0,1.0,-1.0,1.0,3.0,7.0);

}
