// EffectDoc.cpp : implementation of the CEffectDoc class
//

#include "stdafx.h"
#include "Effect.h"

#include "EffectDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectDoc

IMPLEMENT_DYNCREATE(CEffectDoc, CDocument)

BEGIN_MESSAGE_MAP(CEffectDoc, CDocument)
	//{{AFX_MSG_MAP(CEffectDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectDoc construction/destruction

CEffectDoc::CEffectDoc()
{
	// TODO: add one-time construction code here

}

CEffectDoc::~CEffectDoc()
{
}

BOOL CEffectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CEffectDoc serialization

void CEffectDoc::Serialize(CArchive& ar)
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
// CEffectDoc diagnostics

#ifdef _DEBUG
void CEffectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEffectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEffectDoc commands
