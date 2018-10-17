// 3DSglLoaderDoc.cpp : implementation of the CMy3DSglLoaderDoc class
//

#include "stdafx.h"
#include "3DSglLoader.h"

#include "3DSglLoaderDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMy3DSglLoaderDoc

IMPLEMENT_DYNCREATE(CMy3DSglLoaderDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy3DSglLoaderDoc, CDocument)
	//{{AFX_MSG_MAP(CMy3DSglLoaderDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy3DSglLoaderDoc construction/destruction

CMy3DSglLoaderDoc::CMy3DSglLoaderDoc()
{
	// TODO: add one-time construction code here

}

CMy3DSglLoaderDoc::~CMy3DSglLoaderDoc()
{
}

BOOL CMy3DSglLoaderDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CMy3DSglLoaderDoc serialization

void CMy3DSglLoaderDoc::Serialize(CArchive& ar)
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
// CMy3DSglLoaderDoc diagnostics

#ifdef _DEBUG
void CMy3DSglLoaderDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMy3DSglLoaderDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMy3DSglLoaderDoc commands
