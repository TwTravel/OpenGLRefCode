// JgfzDoc.cpp : implementation of the CJgfzDoc class
//

#include "stdafx.h"
#include "Jgfz.h"

#include "JgfzDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJgfzDoc

IMPLEMENT_DYNCREATE(CJgfzDoc, CDocument)

BEGIN_MESSAGE_MAP(CJgfzDoc, CDocument)
	//{{AFX_MSG_MAP(CJgfzDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJgfzDoc construction/destruction

CJgfzDoc::CJgfzDoc()
{
	// TODO: add one-time construction code here

}

CJgfzDoc::~CJgfzDoc()
{
}

BOOL CJgfzDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CJgfzDoc serialization

void CJgfzDoc::Serialize(CArchive& ar)
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
// CJgfzDoc diagnostics

#ifdef _DEBUG
void CJgfzDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CJgfzDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CJgfzDoc commands
