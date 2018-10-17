// TwoDShapeView.cpp : implementation of the CTwoDShapeView class
//

#include "stdafx.h"
#include "TwoDShape.h"

#include "TwoDShapeDoc.h"
#include "TwoDShapeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeView

IMPLEMENT_DYNCREATE(CTwoDShapeView, CView)

BEGIN_MESSAGE_MAP(CTwoDShapeView, CView)
	//{{AFX_MSG_MAP(CTwoDShapeView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_CUBE, OnCube)
	ON_UPDATE_COMMAND_UI(IDM_CUBE, OnUpdateCube)
	ON_COMMAND(IDM_LINE_LOOP, OnLineLoop)
	ON_UPDATE_COMMAND_UI(IDM_LINE_LOOP, OnUpdateLineLoop)
	ON_COMMAND(IDM_LINE_STIPPLE, OnLineStipple)
	ON_UPDATE_COMMAND_UI(IDM_LINE_STIPPLE, OnUpdateLineStipple)
	ON_COMMAND(IDM_LINE_STRIP, OnLineStrip)
	ON_UPDATE_COMMAND_UI(IDM_LINE_STRIP, OnUpdateLineStrip)
	ON_COMMAND(IDM_LINES, OnLines)
	ON_UPDATE_COMMAND_UI(IDM_LINES, OnUpdateLines)
	ON_COMMAND(IDM_NONCONVEX, OnNonconvex)
	ON_UPDATE_COMMAND_UI(IDM_NONCONVEX, OnUpdateNonconvex)
	ON_COMMAND(IDM_POINTS, OnPoints)
	ON_UPDATE_COMMAND_UI(IDM_POINTS, OnUpdatePoints)
	ON_COMMAND(IDM_POLYGON, OnPolygon)
	ON_UPDATE_COMMAND_UI(IDM_POLYGON, OnUpdatePolygon)
	ON_COMMAND(IDM_POLYGON_STIPPLE, OnPolygonStipple)
	ON_UPDATE_COMMAND_UI(IDM_POLYGON_STIPPLE, OnUpdatePolygonStipple)
	ON_COMMAND(IDM_QUAD_STRIP, OnQuadStrip)
	ON_UPDATE_COMMAND_UI(IDM_QUAD_STRIP, OnUpdateQuadStrip)
	ON_COMMAND(IDM_QUADRILATERALS, OnQuadrilaterals)
	ON_UPDATE_COMMAND_UI(IDM_QUADRILATERALS, OnUpdateQuadrilaterals)
	ON_COMMAND(IDM_TRIANGLE_FAN, OnTriangleFan)
	ON_UPDATE_COMMAND_UI(IDM_TRIANGLE_FAN, OnUpdateTriangleFan)
	ON_COMMAND(IDM_TRIANGLE_STRIP, OnTriangleStrip)
	ON_UPDATE_COMMAND_UI(IDM_TRIANGLE_STRIP, OnUpdateTriangleStrip)
	ON_COMMAND(IDM_TRIANGLES, OnTriangles)
	ON_UPDATE_COMMAND_UI(IDM_TRIANGLES, OnUpdateTriangles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeView construction/destruction

CTwoDShapeView::CTwoDShapeView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	m_twoDShape=Points;
}

CTwoDShapeView::~CTwoDShapeView()
{
}

BOOL CTwoDShapeView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeView drawing

void CTwoDShapeView::OnDraw(CDC* pDC)
{
	CTwoDShapeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();

}

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeView diagnostics

#ifdef _DEBUG
void CTwoDShapeView::AssertValid() const
{
	CView::AssertValid();
}

void CTwoDShapeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTwoDShapeDoc* CTwoDShapeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTwoDShapeDoc)));
	return (CTwoDShapeDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeView message handlers

int CTwoDShapeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	Init();

	return 0;
}

void CTwoDShapeView::OnDestroy() 
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

void CTwoDShapeView::OnSize(UINT nType, int cx, int cy) 
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
		glFrustum(-1.0,1.0,-1.0,1.0,0.0,7.0);
		glViewport(0, 0, cx, cy);
   
    }
	
}

void CTwoDShapeView::Init()
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

BOOL CTwoDShapeView::bSetupPixelFormat()
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

void CTwoDShapeView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	glShadeModel(GL_FLAT);

	switch(m_twoDShape)
	{
		case Points:			DrawPoints();break;
		case Lines:				DrawLines();break;
		case LineStipple:		DrawLineStipple();break;
		case LineStrip:			DrawLineStrip();break;
		case LineLoop:			DrawLineLoop();break;
		case Polygons:			DrawPolygons();break;
		case PolygonStipple:	DrawPolygonStipple();break;
		case Triangles:			DrawTriangles();break;
		case TriangleStrip:		DrawTriangleStrip();break;
		case TriangleFan:		DrawTriangleFan();break;
		case Quadrilaterals:	DrawQuadrilaterals();break;
		case QuadStrip:			DrawQuadStrip();break;
		case NonConvex:			DrawNonConvex();break;
		case Cube:				DrawCube();break;

	}

	glFinish();

}

void CTwoDShapeView::OnCube() 
{
	// TODO: Add your command handler code here
	m_twoDShape=Cube;
	Invalidate();
}

void CTwoDShapeView::OnUpdateCube(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == Cube)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CTwoDShapeView::OnLineLoop() 
{
	// TODO: Add your command handler code here

	m_twoDShape=LineLoop;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateLineLoop(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == LineLoop)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnLineStipple() 
{
	// TODO: Add your command handler code here

	m_twoDShape=LineStipple;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateLineStipple(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
	if(m_twoDShape == LineStipple)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);


}

void CTwoDShapeView::OnLineStrip() 
{
	// TODO: Add your command handler code here

	m_twoDShape=LineStrip;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateLineStrip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == LineStrip)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnLines() 
{
	// TODO: Add your command handler code here
	m_twoDShape=Lines;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateLines(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == Lines)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CTwoDShapeView::OnNonconvex() 
{
	// TODO: Add your command handler code here

	m_twoDShape=NonConvex;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateNonconvex(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_twoDShape == NonConvex)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);


}

void CTwoDShapeView::OnPoints() 
{
	// TODO: Add your command handler code here

	m_twoDShape=Points;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdatePoints(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == Points)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CTwoDShapeView::OnPolygon() 
{
	// TODO: Add your command handler code here

		m_twoDShape=Polygons;
		Invalidate();
	
}

void CTwoDShapeView::OnUpdatePolygon(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == Polygons)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnPolygonStipple() 
{
	// TODO: Add your command handler code here

	m_twoDShape=PolygonStipple;
	Invalidate();
		
}

void CTwoDShapeView::OnUpdatePolygonStipple(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == PolygonStipple)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnQuadStrip() 
{
	// TODO: Add your command handler code here

	m_twoDShape=QuadStrip;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateQuadStrip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == QuadStrip)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnQuadrilaterals() 
{
	// TODO: Add your command handler code here

	m_twoDShape=Quadrilaterals;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateQuadrilaterals(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == Quadrilaterals)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnTriangleFan() 
{
	// TODO: Add your command handler code here
	m_twoDShape=TriangleFan;
	Invalidate();
		
}

void CTwoDShapeView::OnUpdateTriangleFan(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == TriangleFan)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTwoDShapeView::OnTriangleStrip() 
{
	// TODO: Add your command handler code here

	m_twoDShape=TriangleStrip;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateTriangleStrip(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == TriangleStrip)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CTwoDShapeView::OnTriangles() 
{
	// TODO: Add your command handler code here

	m_twoDShape=Triangles;
	Invalidate();
	
}

void CTwoDShapeView::OnUpdateTriangles(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here

	if(m_twoDShape == Triangles)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}


void CTwoDShapeView::DrawPoints()
{
	GLfloat fPointSize[2];
	glGetFloatv(GL_POINT_SIZE_RANGE,fPointSize);
	glPointSize(fPointSize[1]/2.0f);

	glBegin(GL_POINTS);
		glVertex2f(0.0f,0.0f);
		glVertex2f(0.5f,0.5f);
		glVertex2f(-0.5f,0.5f);
		glVertex2f(0.5f,-0.5f);
		glVertex2f(-0.5f,-0.5f);
	glEnd();

}

void CTwoDShapeView::DrawLines()
{
	GLfloat fLineWidth[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,fLineWidth);
	glLineWidth(fLineWidth[0]);

	glBegin(GL_LINES);
		
		glVertex2f(0.5f,0.5f);
		glVertex2f(-0.50f,0.0f);

	glEnd();
	
}

void CTwoDShapeView::DrawLineStipple()
{
	GLfloat fLineWidth[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,fLineWidth);
	glLineWidth(fLineWidth[1]/6.0f);

	glBegin(GL_LINES);
		glVertex2f(-0.8f,0.8f);
	   	glVertex2f(0.80f,0.8f);
	glEnd();

	glEnable(GL_LINE_STIPPLE);

	glLineStipple(1,0x00FF);
	glBegin(GL_LINES);
		glVertex2f(-0.8f,0.4f);
	   	glVertex2f(0.80f,0.4f);
	glEnd();

	glLineStipple(1,0x0F0F);
	glBegin(GL_LINES);
		glVertex2f(-0.8f,0.0f);
	   	glVertex2f(0.80f,0.0f);
	glEnd();

	glLineStipple(2,0x0101);
	glBegin(GL_LINES);
		glVertex2f(-0.8f,-0.4f);
	   	glVertex2f(0.80f,-0.4f);
	glEnd();

	glLineStipple(1,0x1C47);
	glBegin(GL_LINES);
		glVertex2f(-0.8f,-0.8f);
	   	glVertex2f(0.80f,-0.8f);
	glEnd();


	glDisable(GL_LINE_STIPPLE);
	

}

void CTwoDShapeView::DrawLineStrip()
{
	GLfloat fLineWidth[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,fLineWidth);
	glLineWidth(fLineWidth[1]/6.0f);

	glBegin(GL_LINE_STRIP);
		glVertex2f(-0.8f,0.6f);
		glVertex2f(0.8f,0.6f);
		
		glVertex2f(-0.8f,0.2f);
		glVertex2f(0.8f,0.2f);

		glVertex2f(-0.8f,-0.2f);
		glVertex2f(0.8f,-0.2f);

		glVertex2f(-0.8f,-0.6f);
		glVertex2f(0.8f,-0.6f);

	glEnd();


}

void CTwoDShapeView::DrawLineLoop()
{
	GLfloat fLineWidth[2];
	glGetFloatv(GL_LINE_WIDTH_RANGE,fLineWidth);
	glLineWidth(fLineWidth[1]/6.0f);

	glBegin(GL_LINE_LOOP);
		glVertex2f(-0.8f,0.6f);
		glVertex2f(0.8f,0.6f);

		glVertex2f(-0.8f,0.2f);
		glVertex2f(0.8f,0.2f);

		glVertex2f(-0.8f,-0.2f);
		glVertex2f(0.8f,-0.2f);

		glVertex2f(-0.8f,-0.6f);
		glVertex2f(0.8f,-0.6f);

	glEnd();

}

void CTwoDShapeView::DrawPolygons()
{
	glLineWidth(2.0f);

	glPolygonMode(GL_FRONT,GL_LINE);
	glPolygonMode(GL_BACK,GL_FILL);

	glFrontFace(GL_CCW);

	glBegin(GL_POLYGON);
		glVertex2f(-0.3f,0.3f);
		glVertex2f(-0.7f,-0.20f);
		glVertex2f(-0.3f,-0.40f);
		glVertex2f(0.4f,-0.40f);
		glVertex2f(0.8f,-0.2f);
		glVertex2f(0.4f,0.3f);
	glEnd();

}

void CTwoDShapeView::DrawPolygonStipple()
{
   GLubyte polygonFillPattern[] = {
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x03, 0x80, 0x01, 0xC0, 0x06, 0xC0, 0x03, 0x60, 
      0x04, 0x60, 0x06, 0x20, 0x04, 0x30, 0x0C, 0x20, 
      0x04, 0x18, 0x18, 0x20, 0x04, 0x0C, 0x30, 0x20,
      0x04, 0x06, 0x60, 0x20, 0x44, 0x03, 0xC0, 0x22, 
      0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22, 
      0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22,
      0x44, 0x01, 0x80, 0x22, 0x44, 0x01, 0x80, 0x22, 
      0x66, 0x01, 0x80, 0x66, 0x33, 0x01, 0x80, 0xCC, 
      0x19, 0x81, 0x81, 0x98, 0x0C, 0xC1, 0x83, 0x30,
      0x07, 0xe1, 0x87, 0xe0, 0x03, 0x3f, 0xfc, 0xc0, 
      0x03, 0x31, 0x8c, 0xc0, 0x03, 0x33, 0xcc, 0xc0, 
      0x06, 0x64, 0x26, 0x60, 0x0c, 0xcc, 0x33, 0x30,
      0x18, 0xcc, 0x33, 0x18, 0x10, 0xc4, 0x23, 0x08, 
      0x10, 0x63, 0xC6, 0x08, 0x10, 0x30, 0x0c, 0x08, 
      0x10, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x08};

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glPolygonStipple(polygonFillPattern);
	glEnable(GL_POLYGON_STIPPLE);

	glBegin(GL_POLYGON);
		glVertex2f(-0.8f,0.8f);
		glVertex2f(-0.8f,-0.80f);
		glVertex2f(0.8f,-0.80f);
		glVertex2f(0.8f,0.80f);
	glEnd();

	glDisable(GL_POLYGON_STIPPLE);

}

void CTwoDShapeView::DrawTriangles()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.0f);

	glBegin(GL_TRIANGLES);
	 	glVertex2f(-0.5f,0.4f);
		glVertex2f(-0.6f,-0.5f);
		glVertex2f(0.5f,-0.5f);
	
		glVertex2f(-0.5f,0.8f);
		glVertex2f(-0.5f,0.6f);
		glVertex2f(0.5f,0.5f);
	glEnd();

}

void CTwoDShapeView::DrawTriangleStrip()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.0f);

	glBegin(GL_TRIANGLE_STRIP);
		
		glVertex2f(-0.8f,0.3f);
		glVertex2f(-0.8f,-0.3f);	
		glVertex2f(-0.3f,0.3f);

		glVertex2f(0.0f,-0.3f);

		glVertex2f(0.3f,0.5f);

		glVertex2f(0.6f,0.4f);

		glVertex2f(0.8f,0.8f);

	glEnd();

}

void CTwoDShapeView::DrawTriangleFan()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.0f);

	glBegin(GL_TRIANGLE_FAN);
		glVertex2f(-0.5f,-0.3f);
		glVertex2f(0.3f,-0.0f);
		glVertex2f(0.3f,0.3f);
		
		glVertex2f(0.4f,0.6f);

		glVertex2f(-0.0f,0.6f);

		glVertex2f(-0.3f,0.6f);

	glEnd();

}

void CTwoDShapeView::DrawQuadrilaterals()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.0f);

	glBegin(GL_QUADS);
		glVertex2f(-0.6f,0.8f);
		glVertex2f(-0.6f,-0.6f);
		glVertex2f(0.5f,0.0f);
		glVertex2f(0.6f,0.5f);
	glEnd();

}

void CTwoDShapeView::DrawQuadStrip()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.0f);

	glBegin(GL_QUAD_STRIP);
		glVertex2f(-0.8f,0.3f);
		glVertex2f(-0.7f,0.6f);
		glVertex2f(-0.6f,0.1f);
		glVertex2f(-0.3f,0.6f);
		

		glVertex2f(-0.1f,-0.2f);
		glVertex2f(0.0f,0.55f);

		glVertex2f(0.3f,-0.4f);
		glVertex2f(0.5f,0.6f);

		glVertex2f(0.8f,-0.4f);
		glVertex2f(0.9f,0.5f);

	glEnd();

}

void CTwoDShapeView::DrawNonConvex()
{
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.0f);

	glBegin(GL_POLYGON);
		glEdgeFlag(TRUE);
		glVertex2f(-0.3f,0.3f);
		glEdgeFlag(FALSE);
		glVertex2f(-0.1f,-0.1f);
		glEdgeFlag(TRUE);
		glVertex2f(0.0f,-0.1f);
		glVertex2f(0.0f,0.3f);
	glEnd();

	glBegin(GL_POLYGON);
		glEdgeFlag(TRUE);
		glVertex2f(-0.1f,-0.1f);
		glVertex2f(-0.2f,-0.4f);
		glEdgeFlag(FALSE);
		glVertex2f(0.0f,-0.4f);
		glVertex2f(0.0f,-0.1f);
	glEnd();

	glBegin(GL_POLYGON);
		glEdgeFlag(FALSE);
		glVertex2f(0.0f,-0.1f);
		glEdgeFlag(TRUE);
		glVertex2f(0.0f,-0.4f);
		glVertex2f(0.4f,-0.4f);
		glVertex2f(0.45f,0.1f);
	glEnd();

}

void CTwoDShapeView::DrawCube()
{

	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	glLineWidth(1.5f);
	
	glColor3f(0.2f,0.5f,1.0f);

	glPushMatrix();
		
		//将盒子绕Y轴70度
		glRotatef(70.0f,0.0f,1.0f,0.0f);

		//将盒子绕Z轴55度
		glRotatef(55.0f,0.0f,0.0f,1.0f);

		//前面
		glBegin(GL_POLYGON);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,0.5f,0.5f);
		glEnd();
	
		//后面
		glBegin(GL_POLYGON);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
		glEnd();
	
		//左面
		glBegin(GL_POLYGON);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
		glEnd();

		//右面
		glBegin(GL_POLYGON);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,0.5f);
		glEnd();

		//底面
		glBegin(GL_POLYGON);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
		glEnd();

		//顶面
		glBegin(GL_POLYGON);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
		glEnd();

	glPopMatrix();

}
