// MatEditor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMatEditor
#include "matlightDoc.h"
#include "editor.h"
#include "shineEdit.h"

class CMatEditor : public CPropertySheet
{
	DECLARE_DYNAMIC(CMatEditor)

// Construction
public:
	CMatEditor(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMatEditor(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CEditor *m_pAmbientPage;
	CEditor *m_pDiffusePage;
	CEditor *m_pSpecularPage;
	CShineEdit *m_pShinePage;

	CMatlightDoc *m_pDocumentPointer;
// Operations
public:
	void Update();	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatEditor)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMatEditor();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMatEditor)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LONG updateFunc(UINT, LONG);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
