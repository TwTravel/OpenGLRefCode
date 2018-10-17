// matlightDoc.cpp : implementation of the CMatlightDoc class
//

#include "stdafx.h"
#include "matlight.h"

#include "matlightDoc.h"
#include "matlightView.h"

#include "lighteditor.h"
#include "mateditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMatlightDoc

IMPLEMENT_DYNCREATE(CMatlightDoc, CDocument)

BEGIN_MESSAGE_MAP(CMatlightDoc, CDocument)
	//{{AFX_MSG_MAP(CMatlightDoc)
	ON_COMMAND(ID_EDIT_LIGHT, OnEditLight)
	ON_COMMAND(ID_EDIT_MATERIAL, OnEditMaterial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMatlightDoc construction/destruction

CMatlightDoc::CMatlightDoc()
	{
	// TODO: add one-time construction code here
	nAlphaList = 1000;
	}

// Destructor for Document
CMatlightDoc::~CMatlightDoc()
	{
	// Delete display lists
	if(glIsList(nAlphaList))
		glDeleteLists(nAlphaList,128);
	}

BOOL CMatlightDoc::OnNewDocument()
	{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// Reinitialize Lighting Properties
	fLight0Pos[0] = -100.0f;
	fLight0Pos[1] = 200.0f;
	fLight0Pos[2] = 200.0f;
	fLight0Pos[3] = 1.0f;

	fLight0Ambient[0] = 0.6f;
	fLight0Ambient[1] = 0.6f;
	fLight0Ambient[2] = 0.6f;
	fLight0Ambient[3] = 1.0f;

	fLight0Diffuse[0] = 0.6f;
	fLight0Diffuse[1] = 0.6f;
	fLight0Diffuse[2] = 0.6f;
	fLight0Diffuse[3] = 1.0f;

	fLight0Specular[0] = 1.0f;
	fLight0Specular[1] = 1.0f;
	fLight0Specular[2] = 1.0f;
	fLight0Specular[3] = 1.0f;

	// Reinitialize Materials Properties
	// Materials Ambient reflectivity
	fMatAmbient[0] = 0.0f;
	fMatAmbient[1] = 0.0f;
	fMatAmbient[2] = 0.6f;
	fMatAmbient[3] = 1.0f;
	
	// Materials Diffuse reflectivity
	fMatDiffuse[0] = 0.0f;
	fMatDiffuse[1] = 0.0f;
	fMatDiffuse[2] = 0.9f;
	fMatDiffuse[3] = 1.0f;
	
	// Materials Specular reflectivity
	fMatSpecular[0] = 1.0f;	
	fMatSpecular[1] = 1.0f;	
	fMatSpecular[2] = 1.0f;	
	fMatSpecular[3] = 1.0f;	

	// Materials Shininess
	fShine = 100.0f;				

	SetTitle("OpenGL SuperBible");

	return TRUE;
	}

/////////////////////////////////////////////////////////////////////////////
// CMatlightDoc serialization

void CMatlightDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMatlightDoc diagnostics

#ifdef _DEBUG
void CMatlightDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMatlightDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMatlightDoc commands


// Setup and initialize the OpenGL environment
void CMatlightDoc::GLSetupRC(HDC hDC)
	{
	HFONT hFont;					// Font handle
	static GLYPHMETRICSFLOAT agmf[255];	// Throw away
	LOGFONT logfont;				// Logical Font structure

	// Set Background to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);	
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);	
	
	
	// Create display list for Alphabet (ABC)
	// Start by creating the font
	logfont.lfHeight = -10;
	logfont.lfWidth = 0;
	logfont.lfEscapement = 0;
	logfont.lfOrientation = 0;
	logfont.lfWeight = FW_BOLD;
	logfont.lfItalic = FALSE;
	logfont.lfUnderline = FALSE;
	logfont.lfStrikeOut = FALSE;
	logfont.lfCharSet = ANSI_CHARSET;
	logfont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logfont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logfont.lfQuality = DEFAULT_QUALITY;
	logfont.lfPitchAndFamily = DEFAULT_PITCH;
	strcpy(logfont.lfFaceName,"Arial");
	
	hFont = CreateFontIndirect(&logfont);
	SelectObject (hDC, hFont); 

	// Create the display list
	nAlphaList = glGenLists(128);

	// Just enough for ABC
	wglUseFontOutlines(hDC, 65, 3, nAlphaList, 0.0f, 0.2f, 
				WGL_FONT_POLYGONS, agmf);

	DeleteObject(hFont);
	}


void CMatlightDoc::OnEditLight() 
	{
	CLightEditor lightEdObj("Lighting Properties",NULL);
	
	lightEdObj.m_pDocumentPointer = this;
	
	lightEdObj.m_pAmbientPage->SetValues(fLight0Ambient);
	if(lightEdObj.DoModal() == IDOK)
		lightEdObj.Update();
	}


void CMatlightDoc::OnEditMaterial() 
	{
	CMatEditor matEdObj("Material Properties",NULL);
	
	matEdObj.m_pDocumentPointer = this;

	// Need to do ambient page first because, ambient sheet InitDialog
	// gets called too early
	matEdObj.m_pAmbientPage->SetValues(fMatAmbient);
	if(matEdObj.DoModal() == IDOK)
		matEdObj.Update();
	}
