// SlctFdbkDoc.cpp : implementation of the CSlctFdbkDoc class
//

#include "stdafx.h"
#include "SlctFdbk.h"

#include "SlctFdbkDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkDoc

IMPLEMENT_DYNCREATE(CSlctFdbkDoc, CDocument)

BEGIN_MESSAGE_MAP(CSlctFdbkDoc, CDocument)
	//{{AFX_MSG_MAP(CSlctFdbkDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkDoc construction/destruction

CSlctFdbkDoc::CSlctFdbkDoc()
{
	// TODO: add one-time construction code here

}

CSlctFdbkDoc::~CSlctFdbkDoc()
{
}

BOOL CSlctFdbkDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkDoc serialization

void CSlctFdbkDoc::Serialize(CArchive& ar)
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
// CSlctFdbkDoc diagnostics

#ifdef _DEBUG
void CSlctFdbkDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSlctFdbkDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkDoc commands
