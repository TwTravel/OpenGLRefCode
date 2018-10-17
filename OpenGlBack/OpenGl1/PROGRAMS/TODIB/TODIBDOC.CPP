// ToDibDoc.cpp : implementation of the CToDibDoc class
//

#include "stdafx.h"
#include "ToDib.h"

#include "ToDibDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDibDoc

IMPLEMENT_DYNCREATE(CToDibDoc, CDocument)

BEGIN_MESSAGE_MAP(CToDibDoc, CDocument)
	//{{AFX_MSG_MAP(CToDibDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDibDoc construction/destruction

CToDibDoc::CToDibDoc()
{
	// TODO: add one-time construction code here

}

CToDibDoc::~CToDibDoc()
{
}

BOOL CToDibDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CToDibDoc serialization

void CToDibDoc::Serialize(CArchive& ar)
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
// CToDibDoc diagnostics

#ifdef _DEBUG
void CToDibDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CToDibDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CToDibDoc commands
