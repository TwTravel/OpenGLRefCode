// TetrisDoc.cpp : implementation of the CTetrisDoc class
//

#include "stdafx.h"
#include "Tetris.h"

#include "TetrisDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTetrisDoc

IMPLEMENT_DYNCREATE(CTetrisDoc, CDocument)

BEGIN_MESSAGE_MAP(CTetrisDoc, CDocument)
	//{{AFX_MSG_MAP(CTetrisDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTetrisDoc construction/destruction

CTetrisDoc::CTetrisDoc()
{
	// TODO: add one-time construction code here

}

CTetrisDoc::~CTetrisDoc()
{
}

BOOL CTetrisDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTetrisDoc serialization

void CTetrisDoc::Serialize(CArchive& ar)
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
// CTetrisDoc diagnostics

#ifdef _DEBUG
void CTetrisDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTetrisDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTetrisDoc commands
