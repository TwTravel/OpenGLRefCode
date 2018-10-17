// LightView.cpp : implementation of the CLightView class
//

#include "stdafx.h"
#include "Light.h"

#include "LightDoc.h"
#include "LightView.h"
#include "math.h"

#include "LightMaterial.h"
#include "Lighting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned char threeto8[8] = 
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

unsigned char twoto8[4] = 
{
	0, 0x55, 0xaa, 0xff
};

unsigned char oneto8[2] = 
{
	0, 255
};

static int defaultOverride[13] = 
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

static PALETTEENTRY defaultPalEntry[20] = 
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};


/////////////////////////////////////////////////////////////////////////////
// CLightView

IMPLEMENT_DYNCREATE(CLightView, CView)

BEGIN_MESSAGE_MAP(CLightView, CView)
	//{{AFX_MSG_MAP(CLightView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_CUBE, OnCube)
	ON_UPDATE_COMMAND_UI(IDM_CUBE, OnUpdateCube)
	ON_COMMAND(IDM_SPHERE, OnSphere)
	ON_UPDATE_COMMAND_UI(IDM_SPHERE, OnUpdateSphere)
	ON_COMMAND(IDM_COLOR, OnColorMaterial)
	ON_UPDATE_COMMAND_UI(IDM_COLOR, OnUpdateColorMaterial)
	ON_COMMAND(IDM_LIGHT, OnLight)
	ON_UPDATE_COMMAND_UI(IDM_LIGHT, OnUpdateLight)
	ON_COMMAND(IDM_SMOOTH_CUBE, OnSmoothCube)
	ON_UPDATE_COMMAND_UI(IDM_SMOOTH_CUBE, OnUpdateSmoothCube)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLightView construction/destruction

CLightView::CLightView()
{
	// TODO: add construction code here
	
	m_pDC=NULL;

	m_selectObject=Sphere;
	
	m_colorRed=0.2f;
	m_colorGreen=0.5f;
	m_colorBlue=0.8f;
	m_bMaskRed=FALSE;
	m_bMaskGreen=FALSE;
	m_bMaskBlue=FALSE;

	m_lightAmb[0]=0.2f;		m_lightAmb[1]=0.2f;
	m_lightAmb[2]=0.2f;		m_lightAmb[3]=1.0f;

	m_lightDif[0]=1.0f;		m_lightDif[1]=1.0f;
	m_lightDif[2]=0.3f;		m_lightDif[3]=1.0f;

	m_lightSpe[0]=1.0f;		m_lightSpe[1]=1.0f;
	m_lightSpe[2]=1.0f;		m_lightSpe[3]=1.0f;

	m_lightPos[0]=1.0f;		m_lightPos[1]=1.0f;
	m_lightPos[2]=1.0f;		m_lightPos[3]=0.0f;

	m_materialAmb[0]=0.2f;	m_materialAmb[1]=0.2f;
	m_materialAmb[2]=0.2f;	m_materialAmb[3]=1.0f;
	 
	m_materialDif[0]=0.8f;	m_materialDif[1]=0.8f;
	m_materialDif[2]=0.8f;	m_materialDif[3]=1.0f;

	m_materialSpe[0]=0.0f;	m_materialSpe[1]=0.0f;
	m_materialSpe[2]=0.0f;	m_materialSpe[3]=1.0f;
	
	m_materialEmi[0]=0.0f;	m_materialEmi[1]=0.0f;
	m_materialEmi[2]=0.0f;	m_materialEmi[3]=1.0f;

	m_matShininess=0.0f;	
	m_bMaterial=TRUE;

	m_spotAmb[0]=0.2f;		m_spotAmb[1]=0.2f;
	m_spotAmb[2]=0.2f;		m_spotAmb[3]=1.0f;

	m_spotDif[0]=0.0f;		m_spotDif[1]=1.0f;
	m_spotDif[2]=1.0f;		m_spotDif[3]=1.0f;

	m_spotSpe[0]=0.6f;		m_spotSpe[1]=0.6f;
	m_spotSpe[2]=0.6f;		m_spotSpe[3]=1.0f;

	m_spotPos[0]=-1.0f;		m_spotPos[1]=0.0f;
	m_spotPos[2]=1.0f;		m_spotPos[3]=1.0f;

	m_spotDir[0]=1.0f;		m_spotDir[1]=0.0f;
	m_spotDir[2]=-1.0f;		m_spotDir[3]=1.0f;

	m_spotCutoff=40.0f;

	m_bConstant = FALSE;
	m_bLinear = FALSE;
	m_bQuadratic = FALSE;
	m_bSpotOnOff=TRUE;
	
	m_pOldPalette = NULL;
}

CLightView::~CLightView()
{
}

BOOL CLightView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLightView drawing

void CLightView::OnDraw(CDC* pDC)
{
	CLightDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
	
}

/////////////////////////////////////////////////////////////////////////////
// CLightView diagnostics

#ifdef _DEBUG
void CLightView::AssertValid() const
{
	CView::AssertValid();
}

void CLightView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLightDoc* CLightView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLightDoc)));
	return (CLightDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLightView message handlers

void CLightView::Init()
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

	CreateRGBPalette();

    hrc = wglCreateContext(m_pDC->GetSafeHdc());
    wglMakeCurrent(m_pDC->GetSafeHdc(), hrc);

    GetClientRect(&m_oldRect);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

BOOL CLightView::bSetupPixelFormat()
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

int CLightView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	Init();
	
	return 0;
}

void CLightView::OnDestroy() 
{
	HGLRC	hrc;

	hrc = ::wglGetCurrentContext();

    ::wglMakeCurrent(NULL,  NULL);
	
    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pOldPalette)
        m_pDC->SelectPalette(m_pOldPalette, FALSE);

    if (m_pDC)
        delete m_pDC;

	CView::OnDestroy();
	
}

void CLightView::OnSize(UINT nType, int cx, int cy) 
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

void CLightView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(0.2f, 0.5f, 0.8f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	//从下行添加绘制代码,使其位于glFinish()函数前:

	glColorMask(!m_bMaskRed,!m_bMaskGreen,!m_bMaskBlue,GL_TRUE);
	glColor3f(m_colorRed,m_colorGreen,m_colorBlue);	

	if(m_selectObject==SmoothCube)
	{
		DrawSmoothCube(); glFinish(); return;
	}

	if(m_bMaterial)
	{
		CreateMaterial(); 
		CreateLights();
	}	
	
	switch(m_selectObject)
	{
		case Cube:			DrawCube();break;
		case Sphere:		DrawSphere();break;
	}

	glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,1.0);
	glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,0.0);
	glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.0);

	glDisable(GL_LIGHTING);
	
	glFinish();
}

void CLightView::DrawCube()
{
	double dPoint1[]={-0.5,0.5,0.5};
	double dPoint2[]={-0.5,-0.5,0.5};
	double dPoint3[]={0.5,-0.5,0.5};
	double dNormal[3];
	
	glPushMatrix();
		
		glRotatef(25.0f,1.0f,0.0f,0.0f);
		glRotatef(20.0f,0.0f,1.0f,0.0f);
		glScalef(1.2f,1.2f,1.2f);

		///////前面
		CalculateNormal(dPoint1,dPoint2,dPoint3,dNormal);
		glBegin(GL_POLYGON);
			glNormal3dv(dNormal);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,0.5f,0.5f);
		glEnd();

		///////右面
		dPoint1[0]=0.5;	 dPoint1[1]=0.5;  dPoint1[2]=0.5;
		dPoint2[0]=0.5;	 dPoint2[1]=-0.5; dPoint2[2]=0.5;
		dPoint3[0]=0.5;	 dPoint3[1]=-0.5; dPoint3[2]=-0.5;
		CalculateNormal(dPoint1,dPoint2,dPoint3,dNormal);
		glBegin(GL_POLYGON);
			glNormal3dv(dNormal);
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
		glEnd();

		////////后面
		dPoint1[0]=0.5;	 dPoint1[1]=0.5;  dPoint1[2]=-0.5;
		dPoint2[0]=0.5;	 dPoint2[1]=-0.5; dPoint2[2]=-0.5;
		dPoint3[0]=-0.5; dPoint3[1]=-0.5; dPoint3[2]=-0.5;
		CalculateNormal(dPoint1,dPoint2,dPoint3,dNormal);
		glBegin(GL_POLYGON);
			glNormal3dv(dNormal);
			glVertex3f(0.5f,0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,-0.5f);
		glEnd();

		//////左面
		dPoint1[0]=-0.5; dPoint1[1]=0.5;  dPoint1[2]=-0.5;
		dPoint2[0]=-0.5; dPoint2[1]=-0.5; dPoint2[2]=-0.5;
		dPoint3[0]=-0.5; dPoint3[1]=-0.5; dPoint3[3]=0.5;
		CalculateNormal(dPoint1,dPoint2,dPoint3,dNormal);
		glBegin(GL_POLYGON);
			glNormal3dv(dNormal);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,-0.5f);
			glVertex3f(-0.5f,-0.5f,0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
		glEnd();

		////////底面
		
		dPoint1[0]=-0.5; dPoint1[1]=-0.5; dPoint1[2]=0.5;
		dPoint2[0]=-0.5; dPoint2[1]=-0.5; dPoint2[2]=-0.5;
		dPoint3[0]=0.5;	 dPoint3[1]=-0.5; dPoint3[3]=-0.5;
		CalculateNormal(dPoint1,dPoint2,dPoint3,dNormal);
		glBegin(GL_POLYGON);
			glNormal3dv(dNormal);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
			glVertex3f(0.5f,-0.5f,-0.5f);
		glEnd();

		///////上面
		dPoint1[0]=-0.5; dPoint1[1]=0.5;  dPoint1[2]=-0.5;
		dPoint2[0]=-0.5; dPoint2[1]=0.5;  dPoint2[2]=0.5;
		dPoint3[0]=0.5;	 dPoint3[1]=0.5;  dPoint3[3]=0.5;
		CalculateNormal(dPoint1,dPoint2,dPoint3,dNormal);
		glBegin(GL_POLYGON);
			glNormal3dv(dNormal);
			glVertex3f(-0.5f,0.5f,-0.5f);
			glVertex3f(-0.5f,0.5f,0.5f);
			glVertex3f(0.5f,0.5f,0.5f);
			glVertex3f(0.5f,0.5f,-0.5f);
		glEnd();

	glPopMatrix();

}


void CLightView::DrawSphere()
{
	//auxSolidSphere(0.6);

	GLUquadricObj* quadObj;
	glNewList(1,GL_COMPILE);
		quadObj=gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL);
		gluQuadricNormals(quadObj,GLU_SMOOTH);
		gluSphere(quadObj,0.7,32,32);
	glEndList();

	glCallList(1);

}

void CLightView::OnCube() 
{
	// TODO: Add your command handler code here
	m_selectObject=Cube;
	Invalidate();
	
}

void CLightView::OnUpdateCube(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_selectObject == Cube)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CLightView::OnSphere() 
{
	// TODO: Add your command handler code here
	m_selectObject=Sphere;
	Invalidate();
	
}

void CLightView::OnUpdateSphere(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_selectObject == Sphere)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CLightView::OnColorMaterial() 
{
	// TODO: Add your command handler code here
	CLightMaterial dlg;

	dlg.m_colorRed=m_colorRed;
	dlg.m_colorGreen=m_colorGreen;
	dlg.m_colorBlue=m_colorBlue;
	dlg.m_maskRed=m_bMaskRed;
	dlg.m_maskGreen=m_bMaskGreen;
	dlg.m_maskBlue=m_bMaskBlue;

	//材质
	dlg.m_matAmbientR=m_materialAmb[0];
	dlg.m_matAmbientG=m_materialAmb[1];
	dlg.m_matAmbientB=m_materialAmb[2];
	
	dlg.m_matDiffuseR=m_materialDif[0];
	dlg.m_matDiffuseG=m_materialDif[1];
	dlg.m_matDiffuseB=m_materialDif[2];
	
	dlg.m_matSpecularR=m_materialSpe[0];
	dlg.m_matSpecularG=m_materialSpe[1];
	dlg.m_matSpecularB=m_materialSpe[2];

	dlg.m_matEmissionR=m_materialEmi[0];
	dlg.m_matEmissionG=m_materialEmi[1];
	dlg.m_matEmissionB=m_materialEmi[2];

	dlg.m_matShininess=	m_matShininess;
	dlg.m_bMaterialOnOff=m_bMaterial;

	int responeDlg=dlg.DoModal();
	if(responeDlg == IDOK)
	{
		m_colorRed=dlg.m_colorRed;
		m_colorGreen=dlg.m_colorGreen;
		m_colorBlue=dlg.m_colorBlue;
		m_bMaskRed=dlg.m_maskRed;
		m_bMaskGreen=dlg.m_maskGreen;
		m_bMaskBlue=dlg.m_maskBlue;

		m_materialAmb[0]=dlg.m_matAmbientR;
		m_materialAmb[1]=dlg.m_matAmbientG;
		m_materialAmb[2]=dlg.m_matAmbientB;
	
		m_materialDif[0]=dlg.m_matDiffuseR;
		m_materialDif[1]=dlg.m_matDiffuseG;
		m_materialDif[2]=dlg.m_matDiffuseB;
	
		m_materialSpe[0]=dlg.m_matSpecularR;
		m_materialSpe[1]=dlg.m_matSpecularG;
		m_materialSpe[2]=dlg.m_matSpecularB;

		m_materialEmi[0]=dlg.m_matEmissionR;
		m_materialEmi[1]=dlg.m_matEmissionG;
		m_materialEmi[2]=dlg.m_matEmissionB;

		m_matShininess=dlg.m_matShininess;
		m_bMaterial=dlg.m_bMaterialOnOff;

		Invalidate();
		
	}

}

void CLightView::OnUpdateColorMaterial(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CLightView::CreateLights()
{
	glLightfv(GL_LIGHT0,GL_AMBIENT,m_lightAmb);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,m_lightDif);
	glLightfv(GL_LIGHT0,GL_SPECULAR,m_lightSpe);
	glLightfv(GL_LIGHT0,GL_POSITION,m_lightPos);

	glLightfv(GL_LIGHT1,GL_AMBIENT,m_spotAmb);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,m_spotDif);
	glLightfv(GL_LIGHT1,GL_SPECULAR,m_spotSpe);
	glLightfv(GL_LIGHT1,GL_POSITION,m_spotPos);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,m_spotDir);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,m_spotCutoff);
		
	if(m_bConstant)
		glLightf(GL_LIGHT1,GL_CONSTANT_ATTENUATION,2.0);
	if(m_bLinear)
		glLightf(GL_LIGHT1,GL_LINEAR_ATTENUATION,1.0);
	if(m_bQuadratic)
		glLightf(GL_LIGHT1,GL_QUADRATIC_ATTENUATION,0.8);
		
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	if(m_bSpotOnOff)
		glEnable(GL_LIGHT1);
	else
		glDisable(GL_LIGHT1);

}

void CLightView::OnLight() 
{
	// TODO: Add your command handler code here
	CLighting lightDlg;
	
	lightDlg.m_lightAmbientR=m_lightAmb[0];
	lightDlg.m_lightAmbientG=m_lightAmb[1];
	lightDlg.m_lightAmbientB=m_lightAmb[2];

	lightDlg.m_lightDiffuseR=m_lightDif[0];
	lightDlg.m_lightDiffuseG=m_lightDif[1];
	lightDlg.m_lightDiffuseB=m_lightDif[2];

	lightDlg.m_lightSpecularR=m_lightSpe[0];
	lightDlg.m_lightSpecularG=m_lightSpe[1];
	lightDlg.m_lightSpecularB=m_lightSpe[2];

	lightDlg.m_lightPositionX=m_lightPos[0];
	lightDlg.m_lightPositionY=m_lightPos[1];
	lightDlg.m_lightPositionZ=m_lightPos[2];

	/////////Spot
	lightDlg.m_spotAmbientR=m_spotAmb[0];
	lightDlg.m_spotAmbientG=m_spotAmb[1];
	lightDlg.m_spotAmbientB=m_spotAmb[2];

	lightDlg.m_spotDiffuseR=m_spotDif[0];
	lightDlg.m_spotDiffuseG=m_spotDif[1];
	lightDlg.m_spotDiffuseB=m_spotDif[2];

	lightDlg.m_spotSpecularR=m_spotSpe[0];
	lightDlg.m_spotSpecularG=m_spotSpe[1];
	lightDlg.m_spotSpecularB=m_spotSpe[2];

	lightDlg.m_spotPositionX=m_spotPos[0];
	lightDlg.m_spotPositionY=m_spotPos[1];
	lightDlg.m_spotPositionZ=m_spotPos[2];

	lightDlg.m_spotDirectionX=m_spotDir[0];
	lightDlg.m_spotDirectionY=m_spotDir[1];
	lightDlg.m_spotDirectionZ=m_spotDir[2];
	
	lightDlg.m_spotCutoff=m_spotCutoff;

	lightDlg.m_bConstant=m_bConstant;
	lightDlg.m_bLinear=m_bLinear;
	lightDlg.m_bQuadratic=m_bQuadratic;
	lightDlg.m_bSpotOnOff=m_bSpotOnOff;
	
	int responeDlg=lightDlg.DoModal();

	if(responeDlg==IDOK)
	{
		m_lightAmb[0]=lightDlg.m_lightAmbientR;
		m_lightAmb[1]=lightDlg.m_lightAmbientG;
		m_lightAmb[2]=lightDlg.m_lightAmbientB;

		m_lightDif[0]=lightDlg.m_lightDiffuseR;
		m_lightDif[1]=lightDlg.m_lightDiffuseG;
		m_lightDif[2]=lightDlg.m_lightDiffuseB;

		m_lightSpe[0]=lightDlg.m_lightSpecularR;
		m_lightSpe[1]=lightDlg.m_lightSpecularG;
		m_lightSpe[2]=lightDlg.m_lightSpecularB;

		m_lightPos[0]=lightDlg.m_lightPositionX;
		m_lightPos[1]=lightDlg.m_lightPositionY;
		m_lightPos[2]=lightDlg.m_lightPositionZ;

		//////////Spot
		m_spotAmb[0]=lightDlg.m_spotAmbientR;
		m_spotAmb[1]=lightDlg.m_spotAmbientG;
		m_spotAmb[2]=lightDlg.m_spotAmbientB;

		m_spotDif[0]=lightDlg.m_spotDiffuseR;
		m_spotDif[1]=lightDlg.m_spotDiffuseG;
		m_spotDif[2]=lightDlg.m_spotDiffuseB;

		m_spotSpe[0]=lightDlg.m_spotSpecularR;
		m_spotSpe[1]=lightDlg.m_spotSpecularG;
		m_spotSpe[2]=lightDlg.m_spotSpecularB;

		m_spotPos[0]=lightDlg.m_spotPositionX;
		m_spotPos[1]=lightDlg.m_spotPositionY;
		m_spotPos[2]=lightDlg.m_spotPositionZ;

		m_spotDir[0]=lightDlg.m_spotDirectionX;
		m_spotDir[1]=lightDlg.m_spotDirectionY;
		m_spotDir[2]=lightDlg.m_spotDirectionZ;
		
		m_spotCutoff=lightDlg.m_spotCutoff;

		m_bConstant=lightDlg.m_bConstant;
		m_bLinear=lightDlg.m_bLinear;
		m_bQuadratic=lightDlg.m_bQuadratic;
		m_bSpotOnOff=lightDlg.m_bSpotOnOff;

		Invalidate();
	}



}

void CLightView::OnUpdateLight(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CLightView::CreateMaterial()
{
	glMaterialfv(GL_FRONT,GL_AMBIENT,m_materialAmb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,m_materialDif);
	glMaterialfv(GL_FRONT,GL_SPECULAR,m_materialSpe);
	glMaterialfv(GL_FRONT,GL_EMISSION,m_materialEmi);
	glMaterialf(GL_FRONT,GL_SHININESS,m_matShininess);
}

void CLightView::CalculateNormal(GLdouble *dVertex1, 
			GLdouble *dVertex2, GLdouble *dVertex3, 
			GLdouble *dNormal)
{
	GLdouble dVector1[3],dVector2[3];
	dVector1[0]=dVertex2[0]-dVertex1[0];
	dVector1[1]=dVertex2[1]-dVertex1[1];
	dVector1[2]=dVertex2[2]-dVertex1[2];
	dVector2[0]=dVertex3[0]-dVertex1[0];
	dVector2[1]=dVertex3[1]-dVertex1[1];
	dVector2[2]=dVertex3[2]-dVertex1[2];

	dNormal[0]=dVector1[1]*dVector2[2]-dVector1[2]*dVector2[1];
	dNormal[1]=dVector1[2]*dVector2[0]-dVector1[0]*dVector2[2];
	dNormal[2]=dVector1[0]*dVector2[1]-dVector1[1]*dVector2[0];

	double dNormalLength=sqrt(dNormal[0]*dNormal[0]
			+dNormal[1]*dNormal[1]+dNormal[2]*dNormal[2]);
	if(dNormalLength!=0.0)
	{
		dNormal[0]=dNormal[0]/dNormalLength;
		dNormal[1]=dNormal[1]/dNormalLength;
		dNormal[2]=dNormal[2]/dNormalLength;
	}
	else
	{
		dNormal[0]=0.0;
		dNormal[1]=0.0;
		dNormal[2]=1.0;
	}

}

void CLightView::OnSmoothCube() 
{
	// TODO: Add your command handler code here
	m_selectObject=SmoothCube;
	Invalidate();

	
}

void CLightView::OnUpdateSmoothCube(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if( m_selectObject == SmoothCube)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CLightView::DrawSmoothCube()
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
		glRotatef(25.0f,1.0f,0.0f,0.0f);
		glRotatef(45.0f,0.0f,1.0f,0.0f);
		glScalef(1.2f,1.2f,1.2f);

		glBegin(GL_QUAD_STRIP);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-0.5f, 0.5f, 0.5f);
  
			glColor3f(1.0f, 0.0f, 1.0f);
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

	glEnable(GL_LIGHTING);
}

unsigned char CLightView::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits) 
	{

    case 1:
        val &= 0x1;
        return oneto8[val];
    case 2:
        val &= 0x3;
        return twoto8[val];
    case 3:
        val &= 0x7;
        return threeto8[val];

    default:
        return 0;
    }

}

void CLightView::CreateRGBPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *pPal;
    int n, i;
 
    n = ::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
    {
        n = 1 << pfd.cColorBits;
        pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

        ASSERT(pPal != NULL);

        pPal->palVersion = 0x300;
        pPal->palNumEntries = n;
        for (i=0; i<n; i++)
        {
            pPal->palPalEntry[i].peRed =
                    ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
            pPal->palPalEntry[i].peGreen =
                    ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
            pPal->palPalEntry[i].peBlue =
                    ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
            pPal->palPalEntry[i].peFlags = 0;
        }

        /* fix up the palette to include the default GDI palette */
        if ((pfd.cColorBits == 8)                           &&
            (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
            (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
            (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
           )
        {
			for (i = 1 ; i <= 12 ; i++)
                pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
        }

        m_cPalette.CreatePalette(pPal);
        delete pPal;

        m_pOldPalette = m_pDC->SelectPalette(&m_cPalette, FALSE);
        m_pDC->RealizePalette();
    }


}

BOOL CLightView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
	
}
