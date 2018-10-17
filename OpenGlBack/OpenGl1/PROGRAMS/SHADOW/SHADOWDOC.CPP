// ShadowDoc.cpp : implementation of the CShadowDoc class
//

#include "stdafx.h"
#include "Shadow.h"

#include "ShadowDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShadowDoc

IMPLEMENT_DYNCREATE(CShadowDoc, CDocument)

BEGIN_MESSAGE_MAP(CShadowDoc, CDocument)
	//{{AFX_MSG_MAP(CShadowDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShadowDoc construction/destruction

CShadowDoc::CShadowDoc()
{
	// TODO: add one-time construction code here

}

CShadowDoc::~CShadowDoc()
{
}

BOOL CShadowDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CShadowDoc serialization

void CShadowDoc::Serialize(CArchive& ar)
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
// CShadowDoc diagnostics

#ifdef _DEBUG
void CShadowDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CShadowDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CShadowDoc commands
