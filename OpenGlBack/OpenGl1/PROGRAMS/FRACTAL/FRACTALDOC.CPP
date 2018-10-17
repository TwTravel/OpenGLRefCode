// FractalDoc.cpp : implementation of the CFractalDoc class
//

#include "stdafx.h"
#include "Fractal.h"

#include "FractalDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFractalDoc

IMPLEMENT_DYNCREATE(CFractalDoc, CDocument)

BEGIN_MESSAGE_MAP(CFractalDoc, CDocument)
	//{{AFX_MSG_MAP(CFractalDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFractalDoc construction/destruction

CFractalDoc::CFractalDoc()
{
	// TODO: add one-time construction code here

}

CFractalDoc::~CFractalDoc()
{
}

BOOL CFractalDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CFractalDoc serialization

void CFractalDoc::Serialize(CArchive& ar)
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
// CFractalDoc diagnostics

#ifdef _DEBUG
void CFractalDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFractalDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFractalDoc commands
