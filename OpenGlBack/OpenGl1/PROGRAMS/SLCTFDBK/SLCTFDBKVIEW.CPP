// SlctFdbkView.cpp : implementation of the CSlctFdbkView class
//

#include "stdafx.h"
#include "SlctFdbk.h"

#include "SlctFdbkDoc.h"
#include "SlctFdbkView.h"

#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkView

IMPLEMENT_DYNCREATE(CSlctFdbkView, CView)

BEGIN_MESSAGE_MAP(CSlctFdbkView, CView)
	//{{AFX_MSG_MAP(CSlctFdbkView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(IDM_SELECT, OnSelect)
	ON_UPDATE_COMMAND_UI(IDM_SELECT, OnUpdateSelect)
	ON_COMMAND(IDM_FEEDBACK, OnFeedback)
	ON_UPDATE_COMMAND_UI(IDM_FEEDBACK, OnUpdateFeedback)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkView construction/destruction

CSlctFdbkView::CSlctFdbkView()
{
	m_pDC=NULL;
    
    for (int i = 0; i < 2; i++) 
    for (int j = 0; j < 2; j ++)
        BackColor[i][j] = 0;
	m_modeControl=Select;
//	m_modeControl=Feedback;	
	y=1.3f;

}

CSlctFdbkView::~CSlctFdbkView()
{
}

BOOL CSlctFdbkView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkView drawing

void CSlctFdbkView::OnDraw(CDC* pDC)
{
	CSlctFdbkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkView diagnostics

#ifdef _DEBUG
void CSlctFdbkView::AssertValid() const
{
	CView::AssertValid();
}

void CSlctFdbkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSlctFdbkDoc* CSlctFdbkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSlctFdbkDoc)));
	return (CSlctFdbkDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkView message handlers

void CSlctFdbkView::Init()
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

BOOL CSlctFdbkView::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),  
        1,                              
        PFD_DRAW_TO_WINDOW |PFD_SUPPORT_GDI|            
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

int CSlctFdbkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Init();
	CreateSpheres();
	return 0;
}

void CSlctFdbkView::OnDestroy() 
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

void CSlctFdbkView::OnSize(UINT nType, int cx, int cy) 
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
		glFrustum(-1.0,1.0,-1.0,1.0,3,7.0);
		glViewport(0, 0, cx, cy);
   
    }
}

void CSlctFdbkView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);
	glDepthFunc(GL_LESS);
    glDepthRange (0.0, 1.0);    

	switch(m_modeControl)
	{
		case Select:	DrawNameObjects (GL_RENDER); break;
		case Feedback:	ApplyFeedback();	 break;
	}
	
	if(	m_modeControl==Select)DrawNonSelect();
	glFinish();

}

void CSlctFdbkView::DrawNameObjects(GLenum mode)
{
	int i,j;
	for(i=0;i<2;i++)
	{
		if (mode == GL_SELECT)glLoadName (i);
		for(j=0;j<2;j++)
		{
			if (mode == GL_SELECT)glPushName (j);

			glColor3f ((GLfloat) i/3.0, (GLfloat) j/3.0, 
				(GLfloat) BackColor[i][j]/3.0);

			glRectf ((i-1)*1.5f, (j-1)*1.5f, i*1.5f, j*1.5f);
			
			if((i==0)&&(j==0))
			{
				glEnable(GL_LIGHTING);
				glCallList(9);
				glDisable(GL_LIGHTING);
			}
			if((i==0)&&(j==1))
			{
				glEnable(GL_LIGHTING);
				glCallList(10);
				glDisable(GL_LIGHTING);

			}

			if((i==1)&&(j==0))
			{
				glEnable(GL_LIGHTING);
				glCallList(11);
				glDisable(GL_LIGHTING);
			
			}

			if((i==1)&&(j==1))
			{
				glEnable(GL_LIGHTING);
				glCallList(12);
				glDisable(GL_LIGHTING);
			}

		}

		if (mode == GL_SELECT)glPopName();
		if (mode == GL_SELECT)glPopName();
	}
}

void CSlctFdbkView::WhatHits(GLint hits, GLuint* buffer)
{
	int i, j;
	GLuint ii, jj, names, *ptr;
	
	ptr = buffer;

	for (i = 0; i < hits; i++) 
	{ 
		names = *ptr;
		ptr++;	ptr++;	ptr++;

		for (j = 0; j < names; j++) 
		{  
 			if (j == 0) ii = *ptr;
			else if (j == names-1) jj = *ptr;
			
			ptr++;
		}

		BackColor[ii][jj] = (BackColor[ii][jj] + 1) % 4;
	}
}

void CSlctFdbkView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_modeControl == Feedback)return;
	
	GLuint selectBuf[512];
	int x, y;
	GLint hits;
	GLint viewport[4];

	x = point.x;
	y = point.y;
	glGetIntegerv (GL_VIEWPORT, viewport);

	glSelectBuffer (512, selectBuf);
	glRenderMode (GL_SELECT);

	glInitNames();
	glPushName((unsigned)-1);

	glMatrixMode (GL_PROJECTION);
	glPushMatrix ();
		glLoadIdentity (); 

		gluPickMatrix ((GLdouble) x, (GLdouble) (viewport[3] - y), 
			5.0, 5.0, viewport);

		glFrustum(-1.0,1.0,-1.0,1.0,2.0,8.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glTranslatef(0.0f,0.0f,-3.5f);

		DrawNameObjects (GL_SELECT);

		glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	hits = glRenderMode (GL_RENDER);
	WhatHits (hits, selectBuf);

	Invalidate();
}

void CSlctFdbkView::OnSelect() 
{
 	m_modeControl=Select;
	Invalidate();
}

void CSlctFdbkView::OnUpdateSelect(CCmdUI* pCmdUI) 
{
	if(m_modeControl == Select)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}
void CSlctFdbkView::OnFeedback() 
{
 	m_modeControl=Feedback;
	Invalidate();
	
}

void CSlctFdbkView::OnUpdateFeedback(CCmdUI* pCmdUI) 
{
	if(m_modeControl == Feedback)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CSlctFdbkView::DrawFdbkObjects(GLenum mode)
{
	glPointSize(3.0f);
    glBegin (GL_POINTS);
		glNormal3f (0.0f, 0.0f, 1.0f);
		glVertex3f (0.5f, 0.0f, 0.0f);
    glEnd ();

    if (mode == GL_FEEDBACK) glPassThrough (1.0);
    glBegin (GL_POINTS);
		glVertex3f (10.0f, -10.0f, -10.0f);
    glEnd ();

    if (mode == GL_FEEDBACK) glPassThrough (2.0);
    glBegin (GL_POINTS);
		glNormal3f (0.0f, 0.0f, 1.0f);
		glVertex3f (0.5f, 0.5f, 0.0f);
    glEnd ();

}

void CSlctFdbkView::OutputInfo(GLint size, GLfloat * buffer)
{
    GLint count;
    GLfloat token;

    count = size;
    while (count) 
	{
		token = buffer[size-count]; 
		count--;
		if (token == GL_PASS_THROUGH_TOKEN) count--;
		else if (token == GL_POINT_TOKEN)
			VertexData (size, &count, buffer);
		else if (token == GL_LINE_TOKEN) 
		{
			VertexData (size, &count, buffer);
			VertexData (size, &count, buffer);
		}
		else if (token == GL_LINE_RESET_TOKEN) 
		{
			VertexData (size, &count, buffer);
			VertexData (size, &count, buffer);
		}

		//and so on ... ...
    }

}

void CSlctFdbkView::VertexData(GLint size, 
	    GLint *count, GLfloat *buffer)
{
	int i;
    
    for (i = 0; i < 7; i++) 
	{
		OutputFloatValue(y,buffer[size-(*count)]);
		*count = *count - 1;
		y-=0.12f;
    }
}

void CSlctFdbkView::OutputFloatValue(GLfloat position,GLfloat value)
{

	wglUseFontBitmaps(wglGetCurrentDC(),0,256,1000);
	glListBase(1000);
	glRasterPos3f(-1.2f,position,0.0f);

	int     i,decimal,  sign, precision = 7;   
	char    *buffer; 
	float	source = value;

	buffer = _fcvt( source, precision, &decimal, &sign );
	
	if(sign==0)
		glCallLists(1,GL_UNSIGNED_BYTE," ");
	else
		glCallLists(1,GL_UNSIGNED_BYTE,"-");

	glCallLists(decimal,GL_BYTE,buffer);
	glCallLists(1,GL_UNSIGNED_BYTE,".");
	for(i=0;i<decimal;i++)buffer++;
	glCallLists(precision,GL_BYTE,buffer);

}

void CSlctFdbkView::ApplyFeedback()
{
    GLfloat feedBuffer[1024];
    GLint size;

    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat matDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat matSpe[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif);
	glMaterialfv(GL_FRONT, GL_SPECULAR,matSpe);
	glMaterialf(GL_FRONT, GL_SHININESS, 25.0f);

    DrawFdbkObjects(GL_RENDER);

    glFeedbackBuffer (1024, GL_3D_COLOR, feedBuffer);
    glRenderMode (GL_FEEDBACK);
    DrawFdbkObjects(GL_FEEDBACK);

    size = glRenderMode (GL_RENDER);

	//适用于在视区内只输出两个点的信息之情况
	if(y<(1.3f-14.0f*0.12f))   y=1.3f;	

    OutputInfo (size, feedBuffer);
	glDisable(GL_LIGHTING);
}

void CSlctFdbkView::CreateSpheres()
{
	GLfloat matDif_1[] = { 0.8f, 0.8f, 0.0f, 1.0f };
	GLfloat matSpe_1[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	GLfloat matDif_2[] = { 0.1f, 0.2f, 0.8f, 1.0f };
	GLfloat matSpe_2[] = { 0.1, 1.0, 1.0, 1.0f };

	GLfloat matDif_3[] = { 0.1f, 0.8f, 0.8f, 1.0f };
	GLfloat matSpe_3[] = { 0.1, 1.0, 1.0, 1.0f };

	GLfloat matDif_4[] = { 0.8f, 0.2f, 0.8f, 1.0f };
	GLfloat matSpe_4[] = { 0.1, 1.0, 1.0, 1.0f };

	
	GLfloat lightPosition[] = { 0.0f, 3.0f, 2.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glNewList(5, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif_1);
		glMaterialfv(GL_FRONT, GL_SPECULAR,matSpe_1);
		glMaterialf(GL_FRONT, GL_SHININESS, 25.0f);
	glEndList();

	glNewList(6, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif_2);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe_2);
		glMaterialf(GL_FRONT, GL_SHININESS, 70.0f);
	glEndList();

	glNewList(7, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif_3);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe_3);
		glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
	glEndList();

	glNewList(8, GL_COMPILE);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, matDif_4);
		glMaterialfv(GL_FRONT, GL_SPECULAR, matSpe_4);
		glMaterialf(GL_FRONT, GL_SHININESS, 60.0f);
	glEndList();

	GLUquadricObj* quadObj;
	glNewList(1,GL_COMPILE);
			glCallList(5);
			quadObj=gluNewQuadric();
			gluQuadricDrawStyle(quadObj,GLU_FILL);
			gluQuadricNormals(quadObj,GLU_SMOOTH);
			gluSphere(quadObj,0.2,16,16);
	glEndList();

	glNewList(2,GL_COMPILE);
			glCallList(6);
			quadObj=gluNewQuadric();
			gluQuadricDrawStyle(quadObj,GLU_FILL);
			gluQuadricNormals(quadObj,GLU_SMOOTH);
			gluSphere(quadObj,0.2,16,16);
	glEndList();

	glNewList(3,GL_COMPILE);
			glCallList(7);
			quadObj=gluNewQuadric();
			gluQuadricDrawStyle(quadObj,GLU_FILL);
			gluQuadricNormals(quadObj,GLU_SMOOTH);
			gluSphere(quadObj,0.2,16,16);
	glEndList();

	glNewList(4,GL_COMPILE);
			glCallList(8);
			quadObj=gluNewQuadric();
			gluQuadricDrawStyle(quadObj,GLU_FILL);
			gluQuadricNormals(quadObj,GLU_SMOOTH);
			gluSphere(quadObj,0.2,16,16);
	glEndList();


	glNewList(9,GL_COMPILE);
		glPushMatrix();
			glTranslatef(-0.4f,0.4f,0.0f);
			glCallList(1);
			glTranslatef(-0.6f,0.6f,0.0f);
			glCallList(4);
		glPopMatrix();
	glEndList();

	glNewList(10,GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.4f,0.4f,0.0f);
			glCallList(2);
			glTranslatef(0.6f,0.6f,0.0f);
			glCallList(3);
		glPopMatrix();
	glEndList();

	glNewList(11,GL_COMPILE);
		glPushMatrix();
			glTranslatef(-0.4f,-0.4f,0.0f);
			glCallList(3);
			glTranslatef(-0.6f,-0.6f,0.0f);
			glCallList(2);
		glPopMatrix();
	glEndList();

	glNewList(12,GL_COMPILE);
		glPushMatrix();
			glTranslatef(0.4f,-0.4f,0.0f);
			glCallList(4);
			glTranslatef(0.6f,-0.6f,0.0f);
			glCallList(1);
		glPopMatrix();
	glEndList();

	glDisable(GL_LIGHTING);

}


void CSlctFdbkView::Circle(GLfloat dRadius, GLfloat fLine, GLfloat n)
{
	GLfloat dStep=(2*3.14159265)/(GLfloat)n;
	GLfloat fV[3];
	
	glLineWidth(fLine);

	glBegin(GL_LINE_STRIP);
		
	for(GLdouble i=0.0;i<=2*3.14159265+dStep;i+=dStep)
	{
		fV[0]=dRadius*cos(i);
		fV[1]=dRadius*sin(i);
		fV[2]=0.0f;
		glVertex3fv(fV);
	}
	glEnd();

}

void CSlctFdbkView::DrawNonSelect()
{
	GLfloat lw[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,lw);

	glPushMatrix();
	glTranslatef(0.0f,0.0f,0.01f);
		glColor3f(1.0f,0.0f,0.0f);
		Circle(0.2,lw[1]/8.0,50);
		Circle(0.4,lw[1]/8.0,50);
		Circle(0.6,lw[1]/8.0,50);
		Circle(0.8,lw[1]/8.0,100);
		glColor3f(0.0f,1.0f,0.0f);
		Circle(1.0,lw[1]/8.0,100);
		Circle(1.2,lw[1]/8.0,100);
		Circle(1.4,lw[1]/8.0,100);
		Circle(1.6,lw[1]/8.0,100);
		glColor3f(0.0f,0.0f,1.0f);
		Circle(1.8,lw[1]/8.0,100);
		Circle(2.0,lw[1]/8.0,100);
		Circle(2.2,lw[1]/8.0,100);
		Circle(2.4,lw[1]/8.0,100);
		Circle(2.6,lw[1]/8.0,100);
		Circle(2.8,lw[1]/8.0,100);
	glPopMatrix();

}
