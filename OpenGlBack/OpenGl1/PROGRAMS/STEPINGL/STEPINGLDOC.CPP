// StepinGlDoc.cpp : implementation of the CStepinGlDoc class
//

#include "stdafx.h"
#include "StepinGl.h"

#include "StepinGlDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStepinGlDoc

IMPLEMENT_DYNCREATE(CStepinGlDoc, CDocument)

BEGIN_MESSAGE_MAP(CStepinGlDoc, CDocument)
	//{{AFX_MSG_MAP(CStepinGlDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStepinGlDoc construction/destruction

CStepinGlDoc::CStepinGlDoc()
{
	// TODO: add one-time construction code here

}

CStepinGlDoc::~CStepinGlDoc()
{
}

BOOL CStepinGlDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CStepinGlDoc serialization

void CStepinGlDoc::Serialize(CArchive& ar)
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
// CStepinGlDoc diagnostics

#ifdef _DEBUG
void CStepinGlDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStepinGlDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStepinGlDoc commands
