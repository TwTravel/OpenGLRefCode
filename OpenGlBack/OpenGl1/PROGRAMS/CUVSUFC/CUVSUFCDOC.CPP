// CuvSufcDoc.cpp : implementation of the CCuvSufcDoc class
//

#include "stdafx.h"
#include "CuvSufc.h"

#include "CuvSufcDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcDoc

IMPLEMENT_DYNCREATE(CCuvSufcDoc, CDocument)

BEGIN_MESSAGE_MAP(CCuvSufcDoc, CDocument)
	//{{AFX_MSG_MAP(CCuvSufcDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcDoc construction/destruction

CCuvSufcDoc::CCuvSufcDoc()
{
	// TODO: add one-time construction code here

}

CCuvSufcDoc::~CCuvSufcDoc()
{
}

BOOL CCuvSufcDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCuvSufcDoc serialization

void CCuvSufcDoc::Serialize(CArchive& ar)
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
// CCuvSufcDoc diagnostics

#ifdef _DEBUG
void CCuvSufcDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCuvSufcDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcDoc commands
