// TextureDoc.cpp : implementation of the CTextureDoc class
//

#include "stdafx.h"
#include "Texture.h"

#include "TextureDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextureDoc

IMPLEMENT_DYNCREATE(CTextureDoc, CDocument)

BEGIN_MESSAGE_MAP(CTextureDoc, CDocument)
	//{{AFX_MSG_MAP(CTextureDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextureDoc construction/destruction

CTextureDoc::CTextureDoc()
{
	// TODO: add one-time construction code here

}

CTextureDoc::~CTextureDoc()
{
}

BOOL CTextureDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTextureDoc serialization

void CTextureDoc::Serialize(CArchive& ar)
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
// CTextureDoc diagnostics

#ifdef _DEBUG
void CTextureDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTextureDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextureDoc commands
