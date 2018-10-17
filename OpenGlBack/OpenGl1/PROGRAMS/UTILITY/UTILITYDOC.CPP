// UtilityDoc.cpp : implementation of the CUtilityDoc class
//

#include "stdafx.h"
#include "Utility.h"

#include "UtilityDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUtilityDoc

IMPLEMENT_DYNCREATE(CUtilityDoc, CDocument)

BEGIN_MESSAGE_MAP(CUtilityDoc, CDocument)
	//{{AFX_MSG_MAP(CUtilityDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUtilityDoc construction/destruction

CUtilityDoc::CUtilityDoc()
{
	// TODO: add one-time construction code here

}

CUtilityDoc::~CUtilityDoc()
{
}

BOOL CUtilityDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CUtilityDoc serialization

void CUtilityDoc::Serialize(CArchive& ar)
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
// CUtilityDoc diagnostics

#ifdef _DEBUG
void CUtilityDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CUtilityDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUtilityDoc commands
