// LightDoc.cpp : implementation of the CLightDoc class
//

#include "stdafx.h"
#include "Light.h"

#include "LightDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightDoc

IMPLEMENT_DYNCREATE(CLightDoc, CDocument)

BEGIN_MESSAGE_MAP(CLightDoc, CDocument)
	//{{AFX_MSG_MAP(CLightDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightDoc construction/destruction

CLightDoc::CLightDoc()
{
	// TODO: add one-time construction code here

}

CLightDoc::~CLightDoc()
{
}

BOOL CLightDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLightDoc serialization

void CLightDoc::Serialize(CArchive& ar)
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
// CLightDoc diagnostics

#ifdef _DEBUG
void CLightDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLightDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLightDoc commands
