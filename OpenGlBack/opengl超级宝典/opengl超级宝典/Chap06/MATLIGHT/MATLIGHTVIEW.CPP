// matlightView.cpp : implementation of the CMatlightView class
//

#include "stdafx.h"
#include "matlight.h"

#include "matlightDoc.h"
#include "matlightView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatlightView

IMPLEMENT_DYNCREATE(CMatlightView, CView)

BEGIN_MESSAGE_MAP(CMatlightView, CView)
	//{{AFX_MSG_MAP(CMatlightView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_PALETTECHANGED()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatlightView construction/destruction

CMatlightView::CMatlightView()
	{
	// TODO: add construction code here
	m_hRC = NULL;
	m_hDC = NULL;
	m_bPrinting = FALSE;
	}

CMatlightView::~CMatlightView()
{
}

BOOL CMatlightView::PreCreateWindow(CREATESTRUCT& cs)
{
	// Styles required by OpenGL
	cs.style |= (WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CS_OWNDC);

	return CView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CMatlightView printing

BOOL CMatlightView::OnPreparePrinting(CPrintInfo* pInfo)
{
	m_bPrinting = TRUE;
	
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMatlightView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	
}

void CMatlightView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	m_bPrinting = FALSE;
}



// Print the lighting and materials settings
void CMatlightView::PrintSettings(CDC* pDC)
	{
	CMatlightDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	TEXTMETRIC tm;
	int nRow;
	char cOutput[80];

	// Get the height of the characters and establish the first row
	pDC->GetTextMetrics(&tm);
	nRow = tm.tmHeight*3;

	// Header
	strcpy(cOutput,"// Material and Lighting Properties");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));

	// Lighting Properties
	nRow += tm.tmHeight*3;
	strcpy(cOutput,"// Lighting values and code");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Ambient Light
	sprintf(cOutput,"GLfloat fLtAmbient[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fLight0Ambient[0],pDoc->fLight0Ambient[1],
			pDoc->fLight0Ambient[2],pDoc->fLight0Ambient[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Diffuse Light
	sprintf(cOutput,"GLfloat fLtDiffuse[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fLight0Diffuse[0],pDoc->fLight0Diffuse[1],
			pDoc->fLight0Diffuse[2],pDoc->fLight0Diffuse[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Specular Light
	sprintf(cOutput,"GLfloat fLtSpecular[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fLight0Specular[0],pDoc->fLight0Specular[1],
			pDoc->fLight0Specular[2],pDoc->fLight0Specular[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Light Position
	sprintf(cOutput,"GLfloat fLtPosition[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fLight0Pos[0],pDoc->fLight0Pos[1],
			pDoc->fLight0Pos[2],pDoc->fLight0Pos[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	
	// Show the code
	nRow += (tm.tmHeight*2);

	strcpy(cOutput,"glEnable(GL_LIGHTING)");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glLightfv(GL_LIGHT0,GL_AMBIENT,fLtAmbient);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glLightfv(GL_LIGHT0,GL_DIFFUSE,fLtDiffuse);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;
	
	strcpy(cOutput,"glLightfv(GL_LIGHT0,GL_SPECULAR,fLtSpecular);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glLightfv(GL_LIGHT0,GL_POSITION,fLtPosition);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glEnable(GL_LIGHT0)");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += (tm.tmHeight*4);


	// Material Properties
	strcpy(cOutput,"// Material values and code");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Ambient Reflectivity
	sprintf(cOutput,"GLfloat fMatAmbient[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fMatAmbient[0],pDoc->fMatAmbient[1],
			pDoc->fMatAmbient[2],pDoc->fMatAmbient[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Diffuse Reflectivity
	sprintf(cOutput,"GLfloat fMatDiffuse[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fMatDiffuse[0],pDoc->fMatDiffuse[1],
			pDoc->fMatDiffuse[2],pDoc->fMatDiffuse[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Specular Reflectivity
	sprintf(cOutput,"GLfloat fMatSpecular[4] = { %1.2ff, %1.2ff, %1.2ff, %1.2ff };",
			pDoc->fMatSpecular[0],pDoc->fMatSpecular[1],
			pDoc->fMatSpecular[2],pDoc->fMatSpecular[3]);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	// Shine
	sprintf(cOutput,"GLfloat fShine = %1.2f;",pDoc->fShine);
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += (tm.tmHeight*3);

	
	// Material setting code
	strcpy(cOutput,"glMaterialfv(GL_FRONT,GL_AMBIENT,fMatAmbient);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glMaterialfv(GL_FRONT,GL_DIFFUSE,fMatDiffuse);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glMaterialfv(GL_FRONT,GL_SPECULAR,fMatSpecular);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;

	strcpy(cOutput,"glMaterialf(GL_FRONT,GL_SHININESS,fShine);");
	pDC->TextOut(10,nRow,cOutput,strlen(cOutput));
	nRow += tm.tmHeight;
	}

/////////////////////////////////////////////////////////////////////////////
// CMatlightView drawing
void CMatlightView::OnDraw(CDC* pDC)
	{
	CMatlightDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	GLUquadricObj *quadObj;

	// If printing or print preview, display data
	if(m_bPrinting)
		{
		PrintSettings(pDC);
		return;
		}


	if(m_hRC == NULL)
		pDC->TextOut(0,15,"Invalid Rendering Context!",26);
	else
		{
		// Make the rendering context current
		wglMakeCurrent(m_hDC,m_hRC);

		// Clear the window with current clearing color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Solid Yellow floor
		glDisable(GL_LIGHTING);
		glColor3f(1.0f, 1.0f, 0.0f);

		// Draw floor (CCC Wound)
		glBegin(GL_QUADS);
			glVertex3f(-75.0f,0.0f, -75.0f);
			glVertex3f(-75.0f,0.0f, 75.0f);
			glVertex3f(75.0f, 0.0f, 75.0f);
			glVertex3f(75.0f, 0.0f, -75.0f);
		glEnd();
		
		// Setup Quadric Styles
		quadObj = gluNewQuadric();
		gluQuadricDrawStyle(quadObj,GLU_FILL); // GLU_LINE
		gluQuadricNormals(quadObj,GLU_SMOOTH); // One for each vertex
		gluQuadricOrientation(quadObj,GLU_OUTSIDE); // Normals point out

		// Do Lighting stuff
		glEnable(GL_LIGHTING);
		glLightfv(GL_LIGHT0,GL_AMBIENT,pDoc->fLight0Ambient);
		glLightfv(GL_LIGHT0,GL_DIFFUSE,pDoc->fLight0Diffuse);
		glLightfv(GL_LIGHT0,GL_SPECULAR,pDoc->fLight0Specular);
		glLightfv(GL_LIGHT0,GL_POSITION,pDoc->fLight0Pos);
		glEnable(GL_LIGHT0);


		// Set Material Properties for all objects
		glMaterialfv(GL_FRONT,GL_AMBIENT,pDoc->fMatAmbient);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,pDoc->fMatDiffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,pDoc->fMatSpecular);
		glMaterialf(GL_FRONT,GL_SHININESS,pDoc->fShine);


		// Save Modelview Matrix State and draw the sphere
		glPushMatrix();
			glTranslatef(-40.0f, 20.0f, 0.0f);
			gluSphere(quadObj,20.0f, 25, 25);
		glPopMatrix();
		

		// Draw the cone
		glPushMatrix();
			glTranslatef(40.0f, 1.0f, 0.0f);
			glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
			gluCylinder(quadObj,20.0f, 0.0f, 40.0f, 25, 25);
		glPopMatrix();

		gluDeleteQuadric(quadObj);

		glPushMatrix();
		float fSize = 10.0f;
	
		glTranslatef(0.0f, fSize, 0.0f);
		glRotatef(45.0f, 0.0f, 1.0f, 0.0f);

		// Draw the Cube
		glBegin(GL_QUADS);

			// The Top
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(-fSize,fSize, -fSize);
			glVertex3f(-fSize,fSize, fSize);
			glVertex3f(fSize, fSize, fSize);
			glVertex3f(fSize, fSize, -fSize);
		
			// The Front
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(-fSize,fSize, fSize);
			glVertex3f(-fSize,-fSize, fSize);
			glVertex3f(fSize, -fSize, fSize);
			glVertex3f(fSize, fSize, fSize);

			// The Left Side (YZ plane)
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(-fSize,fSize, -fSize);
			glVertex3f(-fSize,-fSize, -fSize);
			glVertex3f(-fSize, -fSize, fSize);
			glVertex3f(-fSize, fSize, fSize);

			// The Right side
			glNormal3f(1.0f, 0.0f, 0.0f);
			glVertex3f(fSize, fSize, fSize);
			glVertex3f(fSize, fSize, -fSize);
			glVertex3f(fSize,-fSize, -fSize);
			glVertex3f(fSize,-fSize, fSize);
			
			// The Back Side
			glNormal3f(0.0f, 0.0f, -1.0f);
			glVertex3f(fSize, fSize, -fSize);
			glVertex3f(fSize, -fSize, -fSize);
			glVertex3f(-fSize,-fSize, -fSize);
			glVertex3f(-fSize,fSize, -fSize);
		glEnd();

		glPopMatrix();


		// Have to switch for 3D text
		glFrontFace(GL_CW);

		// Draw the string
		glListBase(pDoc->nAlphaList);
		glPushMatrix();
			glTranslatef(-30.0f, 0.0f, 40.0f);
			glScalef(30.0f, 30.0f, 30.0f); 
			glRotatef(-20.0f,0.0f, 1.0f, 0.0f);
			glCallList (1); 
			glCallList (2); 
			glCallList (3); 
		glPopMatrix();

		glFrontFace(GL_CCW);		

		// Swap our scene to the front
		SwapBuffers(m_hDC);

		// Allow other rendering contexts to co-exist
		wglMakeCurrent(m_hDC,NULL);
		}

	}


/////////////////////////////////////////////////////////////////////////////
// CMatlightView message handlers

void CMatlightView::OnSize(UINT nType, int cx, int cy) 
	{
	float fAspect;

	if(cy == 0)
		cy = 1;

	fAspect = (float)cx/(float)cy;

	CView::OnSize(nType, cx, cy);
	
	if(m_hRC == NULL)
		return;


	wglMakeCurrent(m_hDC,m_hRC);

	glViewport(0,0,cx,cy);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, fAspect, 100.0, 450.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Viewing transformation
	glTranslatef(0.0f, 0.0f, -250.0f);	
	glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(30.0f, 0.0f, 1.0f, 0.0f);

	wglMakeCurrent(NULL,NULL);
	}


BOOL CMatlightView::OnEraseBkgnd(CDC* pDC) 
	{
	if(m_hRC != NULL)
		// Do not take the time to erase the background between renderings
		return FALSE;


	return CView::OnEraseBkgnd(pDC);
	}



/////////////////////////////////////////////////////////////////////////////
// CMatlightView diagnostics

#ifdef _DEBUG
void CMatlightView::AssertValid() const
{
	CView::AssertValid();
}

void CMatlightView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMatlightDoc* CMatlightView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMatlightDoc)));
	return (CMatlightDoc*)m_pDocument;
}
#endif //_DEBUG

void CMatlightView::OnInitialUpdate() 
	{
	int nPixelFormat;		// Pixel Format

	CView::OnInitialUpdate();

	// Select the pixel format we would "like"
	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	// Size of this structure
		1,								// Version of this structure	
		PFD_DRAW_TO_WINDOW |			// Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |			// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,				// Double buffered mode
		PFD_TYPE_RGBA,					// RGBA Color mode
		24,								// Want 24bit color 
		0,0,0,0,0,0,					// Not used to select mode
		0,0,							// Not used to select mode
		0,0,0,0,0,						// Not used to select mode
		32,								// Size of depth buffer
		0,								// Not used to select mode
		0,								// Not used to select mode
		PFD_MAIN_PLANE,					// Draw in main plane
		0,								// Not used to select mode
		0,0,0 };						// Not used to select mode

	m_hDC = ::GetDC(m_hWnd);

	// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(m_hDC, &pfd);

	
	// If we can't select a pixel format, all is lost
	if(nPixelFormat == 0)
		{
		MessageBox("Could not select a compatible pixel format!",NULL,MB_OK | MB_ICONSTOP);
		return;
		}

	// Set the pixel format for the device context
	if(!SetPixelFormat(m_hDC, nPixelFormat, &pfd))
		{
		MessageBox("Could not set a valid pixel format!",NULL,MB_OK | MB_ICONSTOP);
		return;
		}


	// Create the palette if needed
	InitializePalette(m_hDC);


	// Create the rendering context
	m_hRC = wglCreateContext(m_hDC);

	if(m_hRC == NULL)
		{
		MessageBox("Could not create rendering context!",NULL,MB_OK | MB_ICONSTOP);
		return;
		}


	// Setup and initialize
	wglMakeCurrent(m_hDC,m_hRC);
	GetDocument()->GLSetupRC(m_hDC);

	// Deselect the rendering context	
	wglMakeCurrent(NULL,NULL);
	}

// Document data has been updated
void CMatlightView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
	{
	Invalidate(FALSE); // Don't need to erase, double buffered
	}


// Initializes the CPalette object
void CMatlightView::InitializePalette(HDC hDC)
	{
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange,GreenRange,BlueRange;
									// Range for each color entry (7,7,and 3)

	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if(!(pfd.dwFlags & PFD_NEED_PALETTE))
		return;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;	

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) +nColors*sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

	// Build mask of all 1's.  This creates a number represented by having
	// the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
	// pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) -1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) -1;

	// Loop through all the palette entries
	for(i = 0; i < nColors; i++)
		{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double) pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char) NULL;
		}

		
	// Create the palette
	m_GLPalette.CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC,(HPALETTE)m_GLPalette,FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);
	}


BOOL CMatlightView::OnQueryNewPalette() 
	{
	// If the palette was created.
	if((HPALETTE)m_GLPalette)
		{
		int nRet;

		// Selects the palette into the current device context
		VERIFY(SelectPalette(m_hDC, (HPALETTE)m_GLPalette, FALSE));

		// Map entries from the currently selected palette to
		// the system palette.  The return value is the number 
		// of palette entries modified.
		nRet = RealizePalette(m_hDC);

		// Repaint, forces remap of palette in current window
		InvalidateRect(NULL,FALSE);

		return nRet;
		}

	return CView::OnQueryNewPalette();
	}


void CMatlightView::OnPaletteChanged(CWnd* pFocusWnd) 
	{
	if(((HPALETTE)m_GLPalette != NULL))// && (pFocusWnd != this))
		{
		// Select the palette into the device context
		VERIFY(SelectPalette(m_hDC,(HPALETTE)m_GLPalette,FALSE));

		// Map entries to system palette
		VERIFY(RealizePalette(m_hDC) != GDI_ERROR);
				
		// Remap the current colors to the newly realized palette
		UpdateColors(m_hDC);

		return;
		}

	CView::OnPaletteChanged(pFocusWnd);
	}

// Cleanup, release the DC
BOOL CMatlightView::DestroyWindow() 
	{
	::ReleaseDC(m_hWnd,m_hDC);
	
	return CView::DestroyWindow();
	}

