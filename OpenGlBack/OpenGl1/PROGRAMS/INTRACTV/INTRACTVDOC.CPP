// IntractvDoc.cpp : implementation of the CIntractvDoc class
//

#include "stdafx.h"
#include "Intractv.h"

#include "IntractvDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIntractvDoc

IMPLEMENT_DYNCREATE(CIntractvDoc, CDocument)

BEGIN_MESSAGE_MAP(CIntractvDoc, CDocument)
	//{{AFX_MSG_MAP(CIntractvDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntractvDoc construction/destruction

CIntractvDoc::CIntractvDoc()
{
	// TODO: add one-time construction code here

}

CIntractvDoc::~CIntractvDoc()
{
}

BOOL CIntractvDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CIntractvDoc serialization

void CIntractvDoc::Serialize(CArchive& ar)
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
// CIntractvDoc diagnostics

#ifdef _DEBUG
void CIntractvDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIntractvDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIntractvDoc commands
