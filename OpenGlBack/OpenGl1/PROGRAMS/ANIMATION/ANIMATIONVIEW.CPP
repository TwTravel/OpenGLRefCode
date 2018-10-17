// AnimationView.cpp : implementation of the CAnimationView class
//

#include "stdafx.h"
#include "Animation.h"

#include "AnimationDoc.h"
#include "AnimationView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationView

IMPLEMENT_DYNCREATE(CAnimationView, CView)

BEGIN_MESSAGE_MAP(CAnimationView, CView)
	//{{AFX_MSG_MAP(CAnimationView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(IDM_PLAY_PAUSE, OnPlayPause)
	ON_UPDATE_COMMAND_UI(IDM_PLAY_PAUSE, OnUpdatePlayPause)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationView construction/destruction

CAnimationView::CAnimationView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	
	m_fAngleY=10.0f;
    m_fAngleX=1.0f;
    m_fAngleZ=5.0f;

	m_play = FALSE;

}

CAnimationView::~CAnimationView()
{
}

BOOL CAnimationView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAnimationView drawing

void CAnimationView::OnDraw(CDC* pDC)
{
	CAnimationDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CAnimationView diagnostics

#ifdef _DEBUG
void CAnimationView::AssertValid() const
{
	CView::AssertValid();
}

void CAnimationView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAnimationDoc* CAnimationView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAnimationDoc)));
	return (CAnimationDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnimationView message handlers

void CAnimationView::Init()
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

BOOL CAnimationView::bSetupPixelFormat()
{
static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
        1,                              // version number
        PFD_DRAW_TO_WINDOW |            // support window
          PFD_SUPPORT_OPENGL|
		  PFD_DOUBLEBUFFER,             // double buffered,			// support OpenGL
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

int CAnimationView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	Init();
	return 0;
}

void CAnimationView::OnDestroy() 
{
	KillTimer(1);

	HGLRC	hrc;
	
	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pDC)
        delete m_pDC;

	CView::OnDestroy();
}

void CAnimationView::OnSize(UINT nType, int cx, int cy) 
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

void CAnimationView::DrawScene()
{
    static BOOL     bBusy = FALSE;

    if(bBusy) 	return;
    bBusy = TRUE;

	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

    glPushMatrix();

        glRotatef(m_fAngleX, 1.0f, 0.0f, 0.0f);
        glRotatef(m_fAngleY, 0.0f, 1.0f, 0.0f);
        glRotatef(m_fAngleZ, 0.0f, 0.0f, 1.0f);

		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f, 0.5f);

			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

            glColor3f(1.0f, 1.0f, 0.0f);
            glVertex3f(0.5f, -0.5f, 0.5f);

			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

            glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);
		
			glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.5f, 0.5f, -0.5f);

            glColor3f(0.0f, 0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f,  -0.5f);

			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f, 0.5f);

		glEnd();

        glBegin(GL_QUADS);
			glColor3f(1.0f, 0.0f, 1.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);

			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(0.5f, 0.5f, 0.5f);

			glColor3f(0.0f, 1.0f, 1.0f);
			glVertex3f(0.5f, 0.5f, -0.5f);

			glColor3f(0.0f, 0.0f, 1.0f);
            glVertex3f(-0.5f, 0.5f, -0.5f);
        glEnd();

        glBegin(GL_QUADS);
            glColor3f(1.0f, 0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f, 0.5f);

			glColor3f(1.0f, 1.0f, 0.0f);
            glVertex3f(0.5f, -0.5f, 0.5f);

			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.5f, -0.5f, -0.5f);

			glColor3f(0.0f, 0.0f, 0.0f);
            glVertex3f(-0.5f, -0.5f,  -0.5f);
        glEnd();

	glPopMatrix();

	glFinish();
	SwapBuffers(wglGetCurrentDC());

	bBusy = FALSE;

}

void CAnimationView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_fAngleY+=10.0f;
    m_fAngleX+=1.0f;
    m_fAngleZ+=5.0f;
	
	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}

void CAnimationView::OnPlayPause() 
{
	// TODO: Add your command handler code here
	m_play = m_play ? FALSE : TRUE;
	if (m_play)
		SetTimer(1, 45, NULL);
	else
		KillTimer(1);

	
}

void CAnimationView::OnUpdatePlayPause(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}
