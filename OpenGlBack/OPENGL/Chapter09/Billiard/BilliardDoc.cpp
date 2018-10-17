// BilliardDoc.cpp : implementation of the CBilliardDoc class
//

#include "stdafx.h"
#include "Billiard.h"

#include "BilliardDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBilliardDoc

IMPLEMENT_DYNCREATE(CBilliardDoc, CDocument)

BEGIN_MESSAGE_MAP(CBilliardDoc, CDocument)
	//{{AFX_MSG_MAP(CBilliardDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBilliardDoc construction/destruction

CBilliardDoc::CBilliardDoc()
{
	// TODO: add one-time construction code here

}

CBilliardDoc::~CBilliardDoc()
{
}

BOOL CBilliardDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CBilliardDoc serialization

void CBilliardDoc::Serialize(CArchive& ar)
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
// CBilliardDoc diagnostics

#ifdef _DEBUG
void CBilliardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CBilliardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBilliardDoc commands
