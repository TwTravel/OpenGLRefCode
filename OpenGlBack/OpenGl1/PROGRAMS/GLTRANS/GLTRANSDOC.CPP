// GLTransDoc.cpp : implementation of the CGLTransDoc class
//

#include "stdafx.h"
#include "GLTrans.h"

#include "GLTransDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGLTransDoc

IMPLEMENT_DYNCREATE(CGLTransDoc, CDocument)

BEGIN_MESSAGE_MAP(CGLTransDoc, CDocument)
	//{{AFX_MSG_MAP(CGLTransDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGLTransDoc construction/destruction

CGLTransDoc::CGLTransDoc()
{
	// TODO: add one-time construction code here

}

CGLTransDoc::~CGLTransDoc()
{
}

BOOL CGLTransDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CGLTransDoc serialization

void CGLTransDoc::Serialize(CArchive& ar)
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
// CGLTransDoc diagnostics

#ifdef _DEBUG
void CGLTransDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGLTransDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGLTransDoc commands
