// TwoDShapeDoc.cpp : implementation of the CTwoDShapeDoc class
//

#include "stdafx.h"
#include "TwoDShape.h"

#include "TwoDShapeDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeDoc

IMPLEMENT_DYNCREATE(CTwoDShapeDoc, CDocument)

BEGIN_MESSAGE_MAP(CTwoDShapeDoc, CDocument)
	//{{AFX_MSG_MAP(CTwoDShapeDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeDoc construction/destruction

CTwoDShapeDoc::CTwoDShapeDoc()
{
	// TODO: add one-time construction code here

}

CTwoDShapeDoc::~CTwoDShapeDoc()
{
}

BOOL CTwoDShapeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeDoc serialization

void CTwoDShapeDoc::Serialize(CArchive& ar)
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
// CTwoDShapeDoc diagnostics

#ifdef _DEBUG
void CTwoDShapeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTwoDShapeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTwoDShapeDoc commands
