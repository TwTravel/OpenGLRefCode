// ModelView.cpp : implementation of the CModelView class
//

#include "stdafx.h"
#include "Model.h"

#include "ModelDoc.h"
#include "ModelView.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelView

IMPLEMENT_DYNCREATE(CModelView, CView)

BEGIN_MESSAGE_MAP(CModelView, CView)
	//{{AFX_MSG_MAP(CModelView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelView construction/destruction

CModelView::CModelView()
{
	// TODO: add construction code here
	m_pDC=NULL;

	m_fDisplayList = TRUE;

	m_iCurrentMode = 0;

	m_dwTotalMillis = 0;
	m_iTotalTriangles = 0;

	m_play=TRUE;
	m_controlMode=TRUE;
	m_selectMode=GL_FILL;

}

CModelView::~CModelView()
{
}

BOOL CModelView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CModelView drawing

void CModelView::OnDraw(CDC* pDC)
{
	CModelDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	DrawScene();

}

/////////////////////////////////////////////////////////////////////////////
// CModelView diagnostics

#ifdef _DEBUG
void CModelView::AssertValid() const
{
	CView::AssertValid();
}

void CModelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CModelDoc* CModelView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CModelDoc)));
	return (CModelDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelView message handlers

void CModelView::Init()
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

BOOL CModelView::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR), 
        1,                             
        PFD_DRAW_TO_WINDOW |           
          PFD_SUPPORT_OPENGL|
		  PFD_DOUBLEBUFFER,		
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

int CModelView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
 	Init();
	
	Init_Sphere_Data();

	SetTimer(1, 30, NULL);

	return 0;
}

void CModelView::OnDestroy() 
{
	HGLRC	hrc;

	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
	if(m_play)KillTimer(1);
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pDC)
        delete m_pDC;
	CView::OnDestroy();	
}

void CModelView::OnSize(UINT nType, int cx, int cy) 
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

void CModelView::DrawScene()
{
	glClearColor(0.2f,0.5f,0.8f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	glPolygonMode(GL_FRONT_AND_BACK,m_selectMode);
	PlayBall();
	glFinish();
	SwapBuffers(wglGetCurrentDC());
}

void CModelView::OnTimer(UINT nIDEvent) 
{
	Invalidate(FALSE);
	CView::OnTimer(nIDEvent);
}

void CModelView::DrawVertexArray()
{
    glDrawElements(GL_TRIANGLES, STRIANGLES*3, GL_UNSIGNED_INT, m_triSphere);
}

void CModelView::DrawTriangles()
{
    int iVert, *pidx;
    glBegin(GL_TRIANGLES);
    pidx = (int *)&m_triSphere[0];
    for (iVert = 0; iVert < STRIANGLES*3; iVert++)
    {
		Vertex(*pidx++);
    }
    glEnd();
}

void CModelView::DrawStrips()
{
    int iIdxBase;
    int iRing, iSection;

    //	绘制两极：用三角形扇
    glBegin(GL_TRIANGLE_FAN);
    
    Vertex(0);
    iIdxBase = 1;
    for (iSection = 0; iSection <= SECTIONS; iSection++)
    {
        Vertex(iIdxBase+(iSection % SECTIONS));
    }
    
    glEnd();

    glBegin(GL_TRIANGLE_FAN);

    Vertex(SVERTICES-1);
    iIdxBase = SVERTICES-SECTIONS-1;
    for (iSection = SECTIONS; iSection >= 0 ; iSection--)
    {
        Vertex(iIdxBase+(iSection % SECTIONS));
    }

    glEnd();

    //	回归线内：用三角形片
    iIdxBase = 1;
    for (iRing = 0; iRing < RINGS; iRing++)
    {
        glBegin(GL_TRIANGLE_STRIP);
        
        for (iSection = 0; iSection <= SECTIONS; iSection++)
        {
            Vertex(iIdxBase+(iSection % SECTIONS));
            Vertex(iIdxBase+(iSection % SECTIONS)+SECTIONS);
        }

        glEnd();

        iIdxBase += SECTIONS;
    }

}

void CModelView::GenerateSphere(float fRadius, int iRings, int iSections,
                    VERTEX *pvtx, TRIANGLE *ptri)
{
    float fTheta, fPhi; 
    float fDTheta, fDPhi;
    float x, y, z, fV, fRSinTheta;
    int   i, j, n, m;               
    int   iVert, iTri;

    iTri = COMPUTE_SPHERE_TRIANGLES(iRings, iSections);
    iVert = COMPUTE_SPHERE_VERTICES(iRings, iSections);

    //	获取顶部和底部顶点信息：
    pvtx[0].x = 0.0f;
    pvtx[0].y = fRadius;
    pvtx[0].z = 0.0f;
    pvtx[0].fNx = 0.0f;
    pvtx[0].fNy = 1.0f;
    pvtx[0].fNz = 0.0f;
    pvtx[0].dwColor = RGB_COLOR(0, 0, 255);
    pvtx[iVert - 1].x = 0.0f;
    pvtx[iVert - 1].y = -fRadius;
    pvtx[iVert - 1].z = 0.0f;
    pvtx[iVert - 1].fNx = 0.0f;
    pvtx[iVert - 1].fNy = -1.0f;
    pvtx[iVert - 1].fNz = 0.0f;
    pvtx[iVert - 1].dwColor = RGB_COLOR(0, 255, 0);

    //产生顶点信息：
    fDTheta = PI / (float) (iRings + 2);
    fDPhi = 2.0f * PI / (float) iSections;
    
	n = 1;

    fTheta = fDTheta;

	//	球坐标生成法：

    for (i = 0; i <= iRings; i++)
    {
        y = (float)(fRadius * cos(fTheta)); 
        fV = fTheta / PI; 
        fRSinTheta = (float)(fRadius * sin(fTheta));
        fPhi = 0.0f;
	
        for (j = 0; j < iSections; j++)
        {
            x = (float)(fRSinTheta * sin(fPhi));
            z = (float)(fRSinTheta * cos(fPhi));
            pvtx[n].x = x;
            pvtx[n].z = z;
            pvtx[n].y = y;
            pvtx[n].fNx = x / fRadius;
            pvtx[n].fNy = y / fRadius;
            pvtx[n].fNz = z / fRadius;
            if (n & 1)
            {
                pvtx[n].dwColor = RGB_COLOR(0, 0, 255);
            }
            else
            {
                pvtx[n].dwColor = RGB_COLOR(0, 255, 0);
            }
            fPhi += fDPhi;
            n++;
        }
	
        fTheta += fDTheta;
    }

    //生成三角形信息（两极）：
    for (i = 0; i < iSections; i++)
    {
        ptri[i].iV1 = 0;
        ptri[i].iV2 = i + 1;
        ptri[i].iV3 = 1 + ((i + 1) % iSections);

        ptri[iTri - iSections + i].iV1 = iVert - 1;
        ptri[iTri - iSections + i].iV2 = iVert - 2 - i;
        ptri[iTri - iSections + i].iV3 = iVert - 2 - ((1 + i) % iSections);
    }

	//生成三角形信息（极回归线内）	
    m = 1; 
    n = iSections; 
	
    for (i = 0; i < iRings; i++)
    {
        for (j = 0; j < iSections; j++)
        {
            ptri[n].iV1 = m + j;
            ptri[n].iV2 = m + iSections + j;
            ptri[n].iV3 = m + iSections + ((j + 1) % iSections);
            ptri[n + 1].iV1 = ptri[n].iV1;
            ptri[n + 1].iV2 = ptri[n].iV3;
            ptri[n + 1].iV3 = m + ((j + 1) % iSections);
            n += 2;
        }
        m += iSections;
    }
}

void CModelView::InitSphere()
{
    GenerateSphere(1.0f, RINGS, SECTIONS, m_vtxSphere, m_triSphere);
    
    glVertexPointer(3, GL_FLOAT, sizeof(VERTEX), &m_vtxSphere[0].x);
    glNormalPointer(GL_FLOAT, sizeof(VERTEX), &m_vtxSphere[0].fNx);
    glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(VERTEX), &m_vtxSphere[0].dwColor);
    
    //	随机种子：（用于旋转）	
	srand(time(NULL));

    m_sphere.xr = 0.0f;
    m_sphere.yr = 0.0f;
    m_sphere.zr = 0.0f;
    m_sphere.fDXr = DROT - FRANDOM(2 * DROT);
    m_sphere.fDYr = DROT - FRANDOM(2 * DROT);
    m_sphere.fDZr = DROT - FRANDOM(2 * DROT);

}

void CModelView::UpdateSphere()
{
    m_sphere.xr += m_sphere.fDXr;
    m_sphere.yr += m_sphere.fDYr;
    m_sphere.zr += m_sphere.fDZr;
}

void CModelView::Vertex(int iVert)
{
    VERTEX *pvtx;

    pvtx = &m_vtxSphere[iVert];
    glColor3ubv((GLubyte *)&pvtx->dwColor);
    glNormal3fv(&pvtx->fNx);
    glVertex3fv(&pvtx->x);
}

void CModelView::Init_Sphere_Data()
{
    float fVector[4];

    int i;
    GLint dlBase;
    
    InitSphere();
    
    fVector[0] = 0.05f;
    fVector[1] = 0.05f;
    fVector[2] = 0.05f;
    fVector[3] = 1.0f;
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, fVector);
    
    fVector[0] = 0.0f;
    fVector[1] = 1.0f;
    fVector[2] = 1.0f;
    fVector[3] = 0.0f;
    glLightfv(GL_LIGHT0, GL_POSITION, fVector);
    fVector[0] = 0.9f;
    fVector[1] = 0.9f;
    fVector[2] = 0.9f;
    fVector[3] = 1.0f;
    glLightfv(GL_LIGHT0, GL_DIFFUSE, fVector);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
    fVector[0] = 0.7f;
    fVector[1] = 0.7f;
    fVector[2] = 0.7f;
    fVector[3] = 1.0f;
    glMaterialfv(GL_FRONT, GL_SPECULAR, fVector);
    
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
    
    glEnable(GL_CULL_FACE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    dlBase = glGenLists(NLISTS);
    for (i = 0; i < NLISTS; i++)
    {
        m_iLists[i] = dlBase+i;
        glNewList(m_iLists[i], GL_COMPILE);

		if(i==0)DrawVertexArray();
		if(i==1)DrawTriangles();
		if(i==2)DrawStrips();
        
		glEndList();
    }
}

void CModelView::PlayBall()
{
    DWORD dwMillis;
    
    glClear(GL_COLOR_BUFFER_BIT);

	glRotatef(m_sphere.xr, 1.0f, 0.0f, 0.0f);
	glRotatef(m_sphere.yr, 0.0f, 1.0f, 0.0f);
	glRotatef(m_sphere.zr, 0.0f, 0.0f, 1.0f);

    if (m_fDisplayList)
    {
        glCallList(m_iLists[m_iCurrentMode]);
    }
    else
    {
		if(m_iCurrentMode==0)DrawVertexArray();
		if(m_iCurrentMode==1)DrawTriangles();
		if(m_iCurrentMode==2)DrawStrips();
    }
    
    UpdateSphere();

    dwMillis = GetTickCount()-dwMillis;

    m_dwTotalMillis += dwMillis;
    m_iTotalTriangles += STRIANGLES;
}

void CModelView::ResetTotals()
{
    m_dwTotalMillis = 0;
    m_iTotalTriangles = 0;
}

void CModelView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		case VK_SPACE:		Key_Space();	break;
		case 'S':			Key_S();		break;
	}	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CModelView::Key_Space()
{
	m_play=!m_play;
	if(m_play)
		SetTimer(1,30,NULL);
	else
		KillTimer(1);
}

void CModelView::Key_S()
{
	m_controlMode=!m_controlMode;
	if(m_controlMode)
		m_selectMode=GL_FILL;
	else
		m_selectMode=GL_LINE;
}
