// ModelDoc.cpp : implementation of the CModelDoc class
//

#include "stdafx.h"
#include "Model.h"

#include "ModelDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CModelDoc

IMPLEMENT_DYNCREATE(CModelDoc, CDocument)

BEGIN_MESSAGE_MAP(CModelDoc, CDocument)
	//{{AFX_MSG_MAP(CModelDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModelDoc construction/destruction

CModelDoc::CModelDoc()
{
	// TODO: add one-time construction code here

}

CModelDoc::~CModelDoc()
{
}

BOOL CModelDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CModelDoc serialization

void CModelDoc::Serialize(CArchive& ar)
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
// CModelDoc diagnostics

#ifdef _DEBUG
void CModelDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CModelDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CModelDoc commands
