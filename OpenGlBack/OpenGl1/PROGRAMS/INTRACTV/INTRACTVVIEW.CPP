// IntractvView.cpp : implementation of the CIntractvView class
//

#include "stdafx.h"
#include "Intractv.h"

#include "IntractvDoc.h"
#include "IntractvView.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

#define PI 3.14159265358979323846

GLubyte contourTexture1[] = {
    255, 255, 255, 255,
    255, 255, 255, 255,
    255, 255, 255, 255,
    127, 127, 127, 127,
};

GLubyte contourTexture2[] = {
    255, 255, 255, 255,
    255, 127, 127, 127,
    255, 127, 127, 127,
    255, 127, 127, 127,
};


#define GETm_coord(frame, x, y) (&(m_waveMesh.coords[frame*m_waveMesh.numCoords+(x)+(y)*(m_waveMesh.m_widthX+1)]))
#define GETm_facet(frame, x, y) (&(m_waveMesh.facets[frame*m_waveMesh.numFacets+(x)+(y)*m_waveMesh.m_widthX]))


///////////////////////////////////////////////////////
// CIntractvView

IMPLEMENT_DYNCREATE(CIntractvView, CView)

BEGIN_MESSAGE_MAP(CIntractvView, CView)
	//{{AFX_MSG_MAP(CIntractvView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntractvView construction/destruction

CIntractvView::CIntractvView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_framesNum=10;
    m_widthX = 10;
    m_widthY = 10;
    m_checkerSize = 2;
    m_height = 0.2;
	m_smooth = GL_TRUE;
	m_lighting = GL_TRUE;
	m_stepControl = GL_FALSE;
	m_rotate = GL_FALSE;
	m_clearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
	m_depth = GL_TRUE;
	m_play=TRUE;

	m_contouring = 0;
	m_curFrame = 0;
	m_nextFrame = 0;
	m_rotateAngle=0.0f;
}

CIntractvView::~CIntractvView()
{
}

BOOL CIntractvView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CIntractvView drawing

void CIntractvView::OnDraw(CDC* pDC)
{
	CIntractvDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CIntractvView diagnostics

#ifdef _DEBUG
void CIntractvView::AssertValid() const
{
	CView::AssertValid();
}

void CIntractvView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CIntractvDoc* CIntractvView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CIntractvDoc)));
	return (CIntractvDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIntractvView message handlers

void CIntractvView::Init()
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

BOOL CIntractvView::bSetupPixelFormat()
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

int CIntractvView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	Init();
	
	glFrontFace(GL_CW);
    InitMaterials();
    InitTexture();
    InitMesh();

	SetTimer(1, 30, NULL);
	return 0;
}

void CIntractvView::OnDestroy() 
{
	HGLRC	hrc;

	if(m_play)KillTimer(1);

	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pDC)
        delete m_pDC;
	
	CView::OnDestroy();
}

void CIntractvView::OnSize(UINT nType, int cx, int cy) 
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

void CIntractvView::DrawScene()
{
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear(m_clearMask);
	glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.1f,-4.5f);

	glScalef(2.0, 2.0, 2.0);
	glRotatef(-35.0, 1.0, 0.0, 0.0);
	glRotatef(35.0, 0.0, 0.0, 1.0);

	Wave();
	
	glFinish();
	SwapBuffers(wglGetCurrentDC());
}

void CIntractvView::OnTimer(UINT nIDEvent) 
{
	Invalidate(FALSE);

	CView::OnTimer(nIDEvent);
}

void CIntractvView::InitMesh()
{
    struct m_coord *m_coord;
    struct m_facet *m_facet;
    float dp1[3], dp2[3];
    float *pt1, *pt2, *pt3;
    float angle, d, x, y;
    GLint numFacets, numCoords, frameNum;
	int		i, j;

    m_waveMesh.m_widthX = m_widthX;
    m_waveMesh.m_widthY = m_widthY;

    m_waveMesh.numFrames = m_framesNum;

    numFacets = m_widthX * m_widthY;
    numCoords = (m_widthX + 1) * (m_widthY + 1);

    m_waveMesh.numCoords = numCoords;
    m_waveMesh.numFacets = numFacets;

    m_waveMesh.coords = (struct m_coord *)malloc(
				m_framesNum*numCoords*sizeof(struct m_coord));
    
	m_waveMesh.facets = (struct m_facet *)malloc(
				m_framesNum*numFacets*sizeof(struct m_facet));

    if (m_waveMesh.coords == NULL || m_waveMesh.facets == NULL) 
	{
		MessageBox("Out of Memory!");
		return;
    }

    for (frameNum = 0; frameNum < m_framesNum; frameNum++) 
	{
		for (i = 0; i <= m_widthX; i++) 
		{
			x = i / (float)m_widthX;
			for (j = 0; j <= m_widthY; j++) 
			{
				y = j / (float)m_widthY;

				d = sqrt(x*x+y*y);
				if (d == 0.0)	d = 0.0001;

        		angle = 2 * PI * d + (2 * PI / m_framesNum * frameNum);

				m_coord = GETm_coord(frameNum, i, j);

				m_coord->vertex[0] = x - 0.5;
				m_coord->vertex[1] = y - 0.5;
				m_coord->vertex[2] = (m_height - m_height * d) * cos(angle);

				m_coord->normal[0] = -(m_height / d) * x * ((1 - d) * 2 * PI *
                   sin(angle) + cos(angle));
				m_coord->normal[1] = -(m_height / d) * y * ((1 - d) * 2 * PI *
                   sin(angle) + cos(angle));
				m_coord->normal[2] = -1;

				d = 1.0 / sqrt(m_coord->normal[0]*m_coord->normal[0]+
                   m_coord->normal[1]*m_coord->normal[1]+1);
				m_coord->normal[0] *= d;
				m_coord->normal[1] *= d;
				m_coord->normal[2] *= d;
				}
		}
		for (i = 0; i < m_widthX; i++) 
		{
			for (j = 0; j < m_widthY; j++) 
			{
				m_facet = GETm_facet(frameNum, i, j);
				
				if (((i/m_checkerSize)%2)^(j/m_checkerSize)%2) 
				{
						m_facet->color[0] = 1.0;
						m_facet->color[1] = 0.1;
						m_facet->color[2] = 0.1;
				} 
				else 
				{
					m_facet->color[0] = 0.2;
					m_facet->color[1] = 1.0;
					m_facet->color[2] = 0.2;
				}

				pt1 = GETm_coord(frameNum, i, j)->vertex;
				pt2 = GETm_coord(frameNum, i, j+1)->vertex;
				pt3 = GETm_coord(frameNum, i+1, j+1)->vertex;

				dp1[0] = pt2[0] - pt1[0];
				dp1[1] = pt2[1] - pt1[1];
				dp1[2] = pt2[2] - pt1[2];

				dp2[0] = pt3[0] - pt2[0];
				dp2[1] = pt3[1] - pt2[1];
				dp2[2] = pt3[2] - pt2[2];

				m_facet->normal[0] = dp1[1] * dp2[2] - dp1[2] * dp2[1];
				m_facet->normal[1] = dp1[2] * dp2[0] - dp1[0] * dp2[2];
				m_facet->normal[2] = dp1[0] * dp2[1] - dp1[1] * dp2[0];

				d = 1.0 / sqrt(m_facet->normal[0]*m_facet->normal[0]+
                   m_facet->normal[1]*m_facet->normal[1]+
                   m_facet->normal[2]*m_facet->normal[2]);

				m_facet->normal[0] *= d;
				m_facet->normal[1] *= d;
				m_facet->normal[2] *= d;
			}
		}
	}
}

void CIntractvView::InitMaterials()
{
	float ambient[] = {0.1, 0.1, 0.1, 1.0};
    float diffuse[] = {0.5, 1.0, 1.0, 1.0};
    float position[] = {4.0, 6.0, 10.0, 0.0};
    float front_mat_shininess[] = {60.0};
    float front_mat_specular[] = {0.2, 0.2, 0.2, 1.0};
    float front_mat_diffuse[] = {0.5, 0.28, 0.38, 1.0};
    float back_mat_shininess[] = {60.0};
    float back_mat_specular[] = {0.5, 0.5, 0.2, 1.0};
    float back_mat_diffuse[] = {1.0, 1.0, 0.2, 1.0};
    float model_ambient[] = {1.0, 1.0, 1.0, 1.0};
    float model_twoside[] = {GL_TRUE};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);
    glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, model_twoside);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    glMaterialfv(GL_FRONT, GL_SHININESS, front_mat_shininess);
    glMaterialfv(GL_FRONT, GL_SPECULAR, front_mat_specular);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, front_mat_diffuse);
    glMaterialfv(GL_BACK, GL_SHININESS, back_mat_shininess);
    glMaterialfv(GL_BACK, GL_SPECULAR, back_mat_specular);
    glMaterialfv(GL_BACK, GL_DIFFUSE, back_mat_diffuse);

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
}

void CIntractvView::InitTexture()
{
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void CIntractvView::Wave()
{
    struct m_coord*		m_coord;
    struct m_facet*		m_facet;
    float*	lastColor;
    float*	thisColor;
    GLint i, j;

    if (m_nextFrame || !m_stepControl) 
	{
		m_curFrame++;	//动画控制参数
    }
    if (m_curFrame >= m_waveMesh.numFrames) 
	{
		m_curFrame = 0;
    }

    if ((m_nextFrame || !m_stepControl) && m_rotate) 
	{	
		m_rotateAngle+=5.0f;
		if(m_rotateAngle>360.0f)m_rotateAngle=0.0f;
		glRotatef(m_rotateAngle, 0.0f, 0.0f, 1.0f);
   }
    m_nextFrame = 0;

    for (i = 0; i < m_waveMesh.m_widthX; i++) 
	{
		glBegin(GL_QUAD_STRIP);
		lastColor = NULL;
		for (j = 0; j < m_waveMesh.m_widthY; j++) 
		{
			m_facet = GETm_facet(m_curFrame, i, j);
			if (!m_smooth && m_lighting) 
			{
				glNormal3fv(m_facet->normal);
			}
			if (m_lighting) 
			{

				thisColor = m_facet->color;
				glColor3fv(m_facet->color);

			} 

			if (!lastColor || (thisColor[0] != lastColor[0] && m_smooth)) 
			{
				if (lastColor) 
				{
					glEnd();
					glBegin(GL_QUAD_STRIP);
				}
				m_coord = GETm_coord(m_curFrame, i, j);
				if (m_smooth && m_lighting) 
				{
					glNormal3fv(m_coord->normal);
				}
				glVertex3fv(m_coord->vertex);

				m_coord = GETm_coord(m_curFrame, i+1, j);
				if (m_smooth && m_lighting) 
				{
					glNormal3fv(m_coord->normal);
				}
				glVertex3fv(m_coord->vertex);
			}

			m_coord = GETm_coord(m_curFrame, i, j+1);
			if (m_smooth && m_lighting) 
			{
				glNormal3fv(m_coord->normal);
			}
			glVertex3fv(m_coord->vertex);

			m_coord = GETm_coord(m_curFrame, i+1, j+1);
			if (m_smooth && m_lighting) 
			{
				glNormal3fv(m_coord->normal);
        
			}
			glVertex3fv(m_coord->vertex);

			lastColor = thisColor;
		}
		glEnd();
	}
}

void CIntractvView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		case 'T':		KeyDown_T();	break;
		case 'S':		KeyDown_S();	break;
		case 'L':		KeyDown_L();	break;
		case 'D':		KeyDown_D();	break;
		case 'K':		KeyDown_K();	break;
		case 'R':		KeyDown_R();	break;
		case 'F':		KeyDown_F();	break;
	}
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CIntractvView::KeyDown_T()
{
    m_contouring++;
    if (m_contouring == 1) 
	{
        static GLfloat map[4] = {0, 0, 20, 0};

        glTexImage2D(GL_TEXTURE_2D, 0, 3, 4, 4, 0, GL_LUMINANCE,
             GL_UNSIGNED_BYTE, (GLvoid *)contourTexture1);
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
        glTexGenfv(GL_S, GL_OBJECT_PLANE, map);
        glTexGenfv(GL_T, GL_OBJECT_PLANE, map);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);
    } 
	else if (m_contouring == 2) 
	{
        static GLfloat map[4] = {0, 0, 20, 0};

        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
        glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

        glPushMatrix();
	        glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTexGenfv(GL_S, GL_EYE_PLANE, map);
			glTexGenfv(GL_T, GL_EYE_PLANE, map);
        glPopMatrix();
    } 
	else 
	{
        m_contouring = 0;
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
        glDisable(GL_TEXTURE_2D);
    }
}

void CIntractvView::KeyDown_S()
{
    m_smooth = !m_smooth;
    if (m_smooth) {
        glShadeModel(GL_SMOOTH);
    } else {
        glShadeModel(GL_FLAT);
    }
}

void CIntractvView::KeyDown_L()
{
    m_lighting = !m_lighting;
    if (m_lighting) {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glEnable(GL_COLOR_MATERIAL);
    }
	else 
	{
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_COLOR_MATERIAL);
    }
}

void CIntractvView::KeyDown_D()
{
    m_depth = !m_depth;
    if (m_depth) 
	{
        glEnable(GL_DEPTH_TEST);
        m_clearMask |= GL_DEPTH_BUFFER_BIT;
    } 
	else 
	{
        glDisable(GL_DEPTH_TEST);
        m_clearMask &= ~GL_DEPTH_BUFFER_BIT;
    }
}

void CIntractvView::KeyDown_K()
{
    m_stepControl = !m_stepControl;
	m_play=!m_play;

	if (m_play)
		SetTimer(1, 30, NULL);
	else
		KillTimer(1);
}

void CIntractvView::KeyDown_F()
{
    if (m_stepControl) m_nextFrame = 1;
}

void CIntractvView::KeyDown_R()
{
	m_rotate = !m_rotate;
}
