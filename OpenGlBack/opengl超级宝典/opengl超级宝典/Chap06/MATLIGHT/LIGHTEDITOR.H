// LightEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLightEditor
#include "stdafx.h"
#include "matlight.h"
#include "matlightDoc.h"
#include "MainFrm.h"

#include "editor.h"

class CLightEditor : public CPropertySheet
{
	DECLARE_DYNAMIC(CLightEditor)

// Construction
public:
	CLightEditor(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CLightEditor(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CEditor *m_pAmbientPage;
	CEditor *m_pDiffusePage;
	CEditor *m_pSpecularPage;

	CMatlightDoc *m_pDocumentPointer;
// Operations
public:
	void Update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightEditor)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLightEditor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLightEditor)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg LONG updateFunc(UINT, LONG);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
