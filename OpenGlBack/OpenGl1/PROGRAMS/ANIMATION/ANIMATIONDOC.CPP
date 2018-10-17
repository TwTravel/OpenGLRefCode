// AnimationDoc.cpp : implementation of the CAnimationDoc class
//

#include "stdafx.h"
#include "Animation.h"

#include "AnimationDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnimationDoc

IMPLEMENT_DYNCREATE(CAnimationDoc, CDocument)

BEGIN_MESSAGE_MAP(CAnimationDoc, CDocument)
	//{{AFX_MSG_MAP(CAnimationDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnimationDoc construction/destruction

CAnimationDoc::CAnimationDoc()
{
	// TODO: add one-time construction code here

}

CAnimationDoc::~CAnimationDoc()
{
}

BOOL CAnimationDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CAnimationDoc serialization

void CAnimationDoc::Serialize(CArchive& ar)
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
// CAnimationDoc diagnostics

#ifdef _DEBUG
void CAnimationDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAnimationDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAnimationDoc commands
