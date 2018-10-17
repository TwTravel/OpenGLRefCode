// PrintGLDoc.cpp : implementation of the CPrintGLDoc class
//

#include "stdafx.h"
#include "PrintGL.h"

#include "PrintGLDoc.h"
#include "PrintGLView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintGLDoc

IMPLEMENT_DYNCREATE(CPrintGLDoc, CDocument)

BEGIN_MESSAGE_MAP(CPrintGLDoc, CDocument)
	//{{AFX_MSG_MAP(CPrintGLDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintGLDoc construction/destruction

CPrintGLDoc::CPrintGLDoc()
{

}

CPrintGLDoc::~CPrintGLDoc()
{
}

BOOL CPrintGLDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CPrintGLDoc serialization

void CPrintGLDoc::Serialize(CArchive& ar)
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
// CPrintGLDoc diagnostics

#ifdef _DEBUG
void CPrintGLDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPrintGLDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPrintGLDoc commands

