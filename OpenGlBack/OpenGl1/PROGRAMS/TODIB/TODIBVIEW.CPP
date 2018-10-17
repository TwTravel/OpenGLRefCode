// ToDibView.cpp : implementation of the CToDibView class
//

#include "stdafx.h"
#include "ToDib.h"

#include "ToDibDoc.h"
#include "ToDibView.h"

#include "math.h"

#include "ToDibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PI	3.14159265358979323846


IMPLEMENT_DYNCREATE(CToDibView, CView)

BEGIN_MESSAGE_MAP(CToDibView, CView)
	//{{AFX_MSG_MAP(CToDibView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_SAVE_TO_DIB, OnSaveToDib)
	ON_COMMAND(IDM_DIB_SET, OnDibSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDibView construction/destruction

CToDibView::CToDibView()
{
	m_pDC=NULL;

	m_dibWidth=64;
	m_dibHeight=64;
	m_dibLeft=0;
	m_dibBottom=0;

}

CToDibView::~CToDibView()
{
}

BOOL CToDibView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CToDibView drawing

void CToDibView::OnDraw(CDC* pDC)
{
	CToDibDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CToDibView diagnostics

#ifdef _DEBUG
void CToDibView::AssertValid() const
{
	CView::AssertValid();
}

void CToDibView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CToDibDoc* CToDibView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CToDibDoc)));
	return (CToDibDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CToDibView message handlers

void CToDibView::Init()
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

BOOL CToDibView::bSetupPixelFormat()
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

int CToDibView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Init();	

	InitObjects();
	return 0;
}

void CToDibView::OnDestroy() 
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

void CToDibView::OnSize(UINT nType, int cx, int cy) 
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
				
		glFrustum(-1.0,1.0,-1.0,1.0,3.0,15.0);
		glViewport(0, 0, cx, cy);
    }
}

void CToDibView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,0.5f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);

	DrawBack();
	
	glTranslatef(0.0f,0.0f,-8.0f);

	EnableLighting();

	Artistic();

	glFinish();
	glDisable(GL_LIGHTING);
}

void CToDibView::Artistic()
{
	glEnable(GL_LINE_SMOOTH);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

	GLbyte BaseArray[3]={0,1,2};
	glCallLists(3,GL_BYTE,BaseArray);

	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
}

void CToDibView::EnableLighting()
{
	GLfloat light0Amb[]={0.0f,0.0f,0.0f,1.0f};
	GLfloat light0Dif[]={0.9f,0.9f,0.9f,1.0f};
	GLfloat light0Pos[]={0.0f,10.0f,15.0f,1.0f};

	glLightfv(GL_LIGHT0,GL_AMBIENT,light0Amb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0Dif);
	glLightfv(GL_LIGHT0,GL_POSITION,light0Pos);

	GLfloat spotAmb[]={0.2f,0.2f,0.2f,0.8f};
	GLfloat spotDif[]={1.0f,1.0f,1.0f,0.8f};
	GLfloat spotSpe[]={0.9f,0.9f,0.7f,0.8f};
	GLfloat spotPos[]={-10.0f,1.0f,10.0f,0.8f};
	GLfloat spotDir[]={1.0f,0.0f,-1.0f,1.0f};
	GLfloat spotCutoff=38.0f;

	glLightfv(GL_LIGHT1,GL_AMBIENT,spotAmb);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,spotDif);
	glLightfv(GL_LIGHT1,GL_SPECULAR,spotSpe);
	glLightfv(GL_LIGHT1,GL_POSITION,spotPos);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,spotDir);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,spotCutoff);
	glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,0.3);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

}
void CToDibView::Circle(GLfloat dRadius,
			GLfloat dThick,GLint n)
{
	GLfloat dStep=(2*PI)/(GLfloat)n;
	GLfloat fV[3],fN[3];
	
	glLineWidth(dThick);

	glBegin(GL_LINE_STRIP);
		
	for(GLdouble i=0.0;i<=2*PI+dStep;i+=dStep)
	{
		fV[0]=dRadius*cos(i);
		fV[1]=dRadius*sin(i);
		fV[2]=0.0f;
		CalcuNormal(fV,fN);
		glNormal3fv(fN);
		glVertex3fv(fV);
	}
	glEnd();

}

void CToDibView::CalcuNormal(GLfloat* fV,GLfloat* fN)
{
	GLfloat fL=(fV[0]*fV[0]+fV[1]*fV[1]+fV[2]*fV[2]);
	fL=sqrt(fL);
	if(fL<0.00001)fL=1.0f;
	fN[0]=fV[0]/fL;
	fN[1]=fV[1]/fL;
	fN[2]=fV[2]/fL;
}

void CToDibView::InitObjects()
{
	GLfloat mat_amb[4] = {0.2f, 0.2f, 0.05f,0.8f};
	GLfloat mat_dif[4]={1.0f,0.6f,0.1f,0.8f};	
	GLfloat mat_spe[4]={0.8f,0.8f,0.8f,0.8f};
	GLfloat mat_emi[4]={0.2f,0.2f,0.2f,0.8};
	GLfloat shininess=60;

	m_iListBase=glGenLists(3);
	glListBase(m_iListBase);

	glNewList(m_iListBase,GL_COMPILE);

		glMaterialfv (GL_FRONT, GL_AMBIENT, mat_amb);
		glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_dif);
		glMaterialfv (GL_FRONT, GL_SPECULAR, mat_spe);
		glMaterialfv(GL_FRONT,GL_EMISSION,mat_emi);
		glMaterialf (GL_FRONT, GL_SHININESS,shininess );

		glPushMatrix();
		glRotatef(-105.0f,1.0f,0.0f,1.0f);

		GLfloat lw[2];
		glGetFloatv(GL_LINE_WIDTH_RANGE,lw);

		Circle(0.2,lw[1]/8.0,50);
		Circle(0.4,lw[1]/8.0,50);
		Circle(0.6,lw[1]/8.0,50);
		Circle(0.8,lw[1]/8.0,100);
		Circle(1.0,lw[1]/7.0,100);
		Circle(1.2,lw[1]/7.0,100);
		Circle(1.4,lw[1]/7.0,100);
		Circle(1.6,lw[1]/7.0,100);
		Circle(1.8,lw[1]/6.0,100);
		Circle(2.0,lw[1]/6.0,100);
		Circle(2.2,lw[1]/6.0,100);
		Circle(2.4,lw[1]/6.0,100);
		Circle(2.6,lw[1]/4.0,100);
		Circle(2.8,lw[1]/4.0,100);
		Circle(3.0,lw[1]/4.0,100);
		Circle(3.2,lw[1]/4.0,100);
		glPopMatrix();
	glEndList();
	
	glNewList(m_iListBase+1,GL_COMPILE);
		glPushMatrix();
			glRotatef(210.0f,0.0f,0.0f,1.0f);
			glCallList(m_iListBase);
		glPopMatrix();	
	glEndList();
	DrawSphere();
}

void CToDibView::DrawBack()
{
	glFrontFace(GL_CW);

	glBegin(GL_QUADS);
		glColor4f(1.0f,1.0f,0.0f,0.5f);
		glVertex3f(-5.0f,5.0f,-14.0f);
		glColor4f(0.0f,1.0f,1.0f,0.5f);
		glVertex3f(-5.0f,-5.0f,-14.0f);
		glColor4f(1.0f,0.0f,0.0f,0.5f);
		glVertex3f(5.0f,-5.0f,-14.0f);
		glColor4f(1.0f,0.0f,1.0f,0.5f);
		glVertex3f(5.0f,5.0f,-14.0f);
	glEnd();

}

void CToDibView::DrawSphere()
{
	GLfloat mat_amb[4] = {0.2f, 0.2f, 0.05f,0.8f};
	GLfloat mat_dif[4]={0.70f,0.7f,0.20f,0.8f};	
	GLfloat mat_spe[4]={0.8f,0.8f,0.8f,0.8f};
	GLfloat mat_emi[4]={0.2f,0.0f,0.1f,0.8};
	GLfloat shininess=50;
	GLfloat lw[2];

	glGetFloatv(GL_LINE_WIDTH_RANGE,lw);

	GLUquadricObj* quadObj;

	glNewList(m_iListBase+2,GL_COMPILE);
		glLineWidth(lw[1]/8.0);

		glMaterialfv (GL_FRONT, GL_AMBIENT, mat_amb);
		glMaterialfv (GL_FRONT, GL_DIFFUSE, mat_dif);
		glMaterialfv (GL_FRONT, GL_SPECULAR, mat_spe);
		glMaterialfv(GL_FRONT,GL_EMISSION,mat_emi);
		glMaterialf (GL_FRONT, GL_SHININESS,shininess );

		glPushMatrix();
			glTranslatef(-2.5f,-2.6f,-1.0f);
			glRotatef(140.0f,1.0f,0.0f,0.0f);
			quadObj=gluNewQuadric();
			gluQuadricDrawStyle(quadObj,GLU_LINE);
			gluQuadricNormals(quadObj,GLU_FLAT);
			gluSphere(quadObj,2.0,24,24);
		glPopMatrix();
	glEndList();

}

void CToDibView::OnSaveToDib() 
{
	BeginWaitCursor();
	
	BITMAPFILEHEADER bmfHdr;
	LPBITMAPINFOHEADER lpBI;

	DWORD dwBitsSize;
	HDIB hDIB;
	LPSTR pDIBits;

	dwBitsSize=sizeof(BITMAPINFOHEADER)
				+m_dibWidth*m_dibHeight*3;

	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize+32);
	if (hDIB == 0)	return ;

	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDIB);

	if (lpBI == NULL)	return;

	pDIBits=(LPSTR)lpBI+sizeof(BITMAPINFOHEADER);

	bmfHdr.bfType=0x4d42;
	bmfHdr.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)
		+m_dibWidth*m_dibHeight*3;
	bmfHdr.bfReserved1=0;
	bmfHdr.bfReserved2=0; 
    bmfHdr.bfOffBits=sizeof(BITMAPFILEHEADER)+
		sizeof(BITMAPINFOHEADER);

	lpBI->biSize=sizeof(BITMAPINFOHEADER);    
	lpBI->biWidth=m_dibWidth;    
	lpBI->biHeight=m_dibHeight;    
	lpBI->biPlanes=1; 
	lpBI->biBitCount=24;    
	lpBI->biCompression=0;    
	lpBI->biSizeImage=0; 
	lpBI->biXPelsPerMeter=0;    
	lpBI->biYPelsPerMeter=0;    
	lpBI->biClrUsed=0; 
	lpBI->biClrImportant=0;	

	glReadPixels(m_dibLeft,m_dibBottom,
		m_dibWidth,m_dibHeight,
		GL_BGR_EXT,GL_UNSIGNED_BYTE,(GLubyte*)pDIBits);

	//	请创建自己的目录或获取当前目录：
	CString FileName="e:\\xsmimag\\DibTest1.bmp";
	
	CFile file(FileName,CFile::modeCreate |
		CFile::modeReadWrite | CFile::shareExclusive);

	file.Write((LPSTR)&bmfHdr,sizeof(BITMAPFILEHEADER));
	file.WriteHuge((LPSTR)lpBI,dwBitsSize);
	file.Close;	
	
	::GlobalUnlock((HGLOBAL) hDIB);

	//在此处，现在可以无效化句柄hDIB：
	::GlobalFree((HGLOBAL) hDIB);

	EndWaitCursor();
}

void CToDibView::OnDibSet() 
{
	CToDibDlg 	dlg;
	GetClientRect(&m_currentRect);

	dlg.m_width = m_dibWidth;
	dlg.m_height = m_dibHeight;
	dlg.m_leftX = m_dibLeft;
	dlg.m_bottomY = m_dibBottom;

	int responeDlg=dlg.DoModal();
	if(responeDlg == IDOK)
	{
		 m_dibWidth=dlg.m_width;
		 m_dibHeight=dlg.m_height;
		 m_dibLeft=dlg.m_leftX;
		 m_dibBottom=dlg.m_bottomY;
	}

	if(m_dibWidth > 4*((m_currentRect.Width()/4)-1)
			-m_dibLeft)
	{
		m_dibWidth=	4*(((m_currentRect.Width()
			-m_dibLeft)/4)-1);
	}

	if(m_dibHeight > 4*((m_currentRect.Height()/4)-1)
			-m_dibBottom)
	{
		m_dibHeight= 4*(((m_currentRect.Height()
			-m_dibBottom)/4)-1);
	}
}
