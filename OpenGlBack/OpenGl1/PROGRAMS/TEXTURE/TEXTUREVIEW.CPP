// TextureView.cpp : implementation of the CTextureView class
//

#include "stdafx.h"
#include "Texture.h"

#include "TextureDoc.h"
#include "TextureView.h"

#include "GL\glAux.h"

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
// CTextureView

IMPLEMENT_DYNCREATE(CTextureView, CView)

BEGIN_MESSAGE_MAP(CTextureView, CView)
	//{{AFX_MSG_MAP(CTextureView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDM_TEXTURE, OnTexture)
	ON_UPDATE_COMMAND_UI(IDM_TEXTURE, OnUpdateTexture)
	ON_COMMAND(IDM_DIB_TEXTURE, OnDibTexture)
	ON_UPDATE_COMMAND_UI(IDM_DIB_TEXTURE, OnUpdateDibTexture)
	ON_WM_ERASEBKGND()
	ON_COMMAND(IDM_AUTO_TEXTURE, OnAutoTexture)
	ON_UPDATE_COMMAND_UI(IDM_AUTO_TEXTURE, OnUpdateAutoTexture)
	ON_COMMAND(IDM_MIPMAPPING, OnMipmapping)
	ON_UPDATE_COMMAND_UI(IDM_MIPMAPPING, OnUpdateMipmapping)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureView construction/destruction

CTextureView::CTextureView()
{
	// TODO: add construction code here
	m_pDC=NULL;
	
	m_typeControl=Texture;

	m_iWindowWidth=400;
	m_iWindowHeight=435;


	m_pOldPalette = NULL;

}

CTextureView::~CTextureView()
{
}

BOOL CTextureView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style|=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTextureView drawing

void CTextureView::OnDraw(CDC* pDC)
{
	CTextureDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	DrawScene();
}

/////////////////////////////////////////////////////////////////////////////
// CTextureView diagnostics

#ifdef _DEBUG
void CTextureView::AssertValid() const
{
	CView::AssertValid();
}

void CTextureView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTextureDoc* CTextureView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTextureDoc)));
	return (CTextureDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextureView message handlers

void CTextureView::Init()
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

BOOL CTextureView::bSetupPixelFormat()
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

int CTextureView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	Init();
	
	return 0;
}

void CTextureView::OnDestroy() 
{
	// TODO: Add your message handler code here

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

void CTextureView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	
	m_iWindowWidth=cx;
	m_iWindowHeight=cy;

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

void CTextureView::DrawScene()
{
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-4.5f);

	switch(m_typeControl)
	{
		case Texture:		TexturePlane();  break;	
		case DibTexture:	BitmapTexture(); break;	
		case AutoTexture:	TexGenerate();	 break;
		case Mipmap:		Mipmapping();	 break;
	}

	glFinish();
}

void CTextureView::TexturePlane()
{
	GLubyte image[64][64][3];
	
	//	创建纹理
	int i,j,g,b,c;
	for(i=0;i<64;i++)
		for(j=0;j<64;j++)
		{
			c=((((i & 0x04)==0)^((j & 0x04))==0))*255;
			g=(4*i)%255;
			b=(j*i)%255;
			image[i][j][0]=(GLubyte)c;
			image[i][j][1]=(GLubyte)g;
			image[i][j][2]=(GLubyte)b;
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	//	定义纹理：
	glTexImage2D(GL_TEXTURE_2D,0,3,64,64,
		0,GL_RGB,GL_UNSIGNED_BYTE,image);
	
	//	控制纹理：
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	//	说明纹理映射方式：
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

	//	启动纹理映射：
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	//	定义纹理坐标和物体几何坐标：
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);glVertex3f(-1.0f,-1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);glVertex3f(-1.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	
}

void CTextureView::OnTexture() 
{
	// TODO: Add your command handler code here
	m_typeControl=Texture;
	Invalidate();
	
}

void CTextureView::OnUpdateTexture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == Texture)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}


void CTextureView::OnDibTexture() 
{
	// TODO: Add your command handler code here
	m_typeControl=DibTexture;
	Invalidate();
	
}

void CTextureView::OnUpdateDibTexture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == DibTexture)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTextureView::OnAutoTexture() 
{
	// TODO: Add your command handler code here
	m_typeControl=AutoTexture;
	Invalidate();
	
}

void CTextureView::OnUpdateAutoTexture(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == AutoTexture)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTextureView::OnMipmapping() 
{
	// TODO: Add your command handler code here
	m_typeControl=Mipmap;
	Invalidate();
	
}

void CTextureView::OnUpdateMipmapping(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_typeControl == Mipmap)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	
}

void CTextureView::BitmapTexture()
{
	/*//用于图像合成
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.0f,1.0f,0.0f,0.5f);
	*/

	m_pDib= new CDib("valley.BMP");

//	m_pDib= new CDib("wheat.BMP");
//	m_pDib= new CDib("hust.BMP");
//	m_pDib= new CDib("studio.BMP");
//	m_pDib= new CDib("sunset.BMP");
//	m_pDib= new CDib("brick.BMP");


	GLubyte* pTextureBits=(GLubyte*)m_pDib->GetDibBitsPtr();
	
	GLint width=m_pDib->GetDibWidth();
	GLint height=m_pDib->GetDibHeight();

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	//	定义纹理：
	glTexImage2D(GL_TEXTURE_2D,0,3,width,height,
		0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pTextureBits);
	
	//	控制纹理：
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	//	说明纹理映射方式：
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);

	//	启动纹理映射：
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	//	定义纹理坐标和物体几何坐标：
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f);glVertex3f(-1.0f,-1.0f,0.0f);
		glTexCoord2f(0.0f,1.0f);glVertex3f(-1.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();

/*
	//绘制第二幅用于合成的图像
*/
/*
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
		glVertex3f(-0.8f,0.8f,0.005f);
		glVertex3f(-0.8f,-0.80f,0.005f);
		glVertex3f(0.8f,-0.80f,0.005f);
		glVertex3f(0.8f,0.80f,0.005);
	glEnd();

	glDisable(GL_POLYGON_STIPPLE);
*/
	glDisable(GL_TEXTURE_2D);

}

unsigned char CTextureView::ComponentFromIndex(int i, UINT nbits, UINT shift)
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

void CTextureView::CreateRGBPalette()
{
    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *pPal;
    int n, i;
 
    n = ::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
    {
        n = 1 << pfd.cColorBits;
        pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) +
              n * sizeof(PALETTEENTRY)];

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
            (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) 	  &&
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

BOOL CTextureView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CTextureView::TexGenerate()
{
	GLfloat currentCoeff[]={1.0f,1.0f,1.0f,1.0f};	
	
	m_pDib= new CDib("wheat.BMP");

//	m_pDib= new CDib("brick.BMP");
//	m_pDib= new CDib("valley.BMP");
//	m_pDib= new CDib("hust.BMP");
//	m_pDib= new CDib("studio.BMP");
//	m_pDib= new CDib("sunset.BMP");

	GLubyte* pTextureBits=(GLubyte*)m_pDib->GetDibBitsPtr();
	
	GLint width=m_pDib->GetDibWidth();
	GLint height=m_pDib->GetDibHeight();

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);

	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,
		GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,
		GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D,0,3,width,height,
		0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pTextureBits);

//	纹理坐标的自动生成：

	glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
	glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);

	glTexGenfv(GL_S,GL_OBJECT_PLANE,currentCoeff);
	glTexGenfv(GL_T,GL_OBJECT_PLANE,currentCoeff);

//	启动纹理映射：
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);

	glEnable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	glMaterialf(GL_FRONT,GL_SHININESS,32.0);

	glPushMatrix();
		glRotatef(15.0f,1.0f,0.0f,0.0f);
		glRotatef(-5.0f,0.0f,1.0f,0.0f);
		auxSolidTeapot(0.8);
	glPopMatrix();

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	glDisable(GL_AUTO_NORMAL);
	glDisable(GL_NORMALIZE);

}

void CTextureView::Mipmapping()
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
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 32, 32, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap32);
    
	//第一级
	glTexImage2D(GL_TEXTURE_2D, 1, 3, 16, 16, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap16);

	//第二级
    glTexImage2D(GL_TEXTURE_2D, 2, 3, 8, 8, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap8);

	//第三级
    glTexImage2D(GL_TEXTURE_2D, 3, 3, 4, 4, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap4);

	//第四级
    glTexImage2D(GL_TEXTURE_2D, 4, 3, 2, 2, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap2);

	//第五级
    glTexImage2D(GL_TEXTURE_2D, 5, 3, 1, 1, 0,
         GL_RGBA, GL_UNSIGNED_BYTE, &mipmap1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
		GL_NEAREST_MIPMAP_NEAREST);
  
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
//					GL_LINEAR_MIPMAP_LINEAR);

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

	//视线回位
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0,1.0,-1.0,1.0,3.0,7.0);

}
