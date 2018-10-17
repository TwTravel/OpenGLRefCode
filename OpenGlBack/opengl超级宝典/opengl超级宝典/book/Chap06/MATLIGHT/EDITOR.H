// Editor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditor dialog
#ifndef __EDITOR
#define __EDITOR

class CEditor : public CPropertyPage
{
	DECLARE_DYNCREATE(CEditor)

// Construction
public:
	CEditor();
	CEditor(UINT id, UINT idCaption);
	~CEditor();

	float fValues[4];

	void SetValues(float fVals[4]);
	void GetValues(float fVals[4]);

	CPropertySheet *m_pParent;

// Dialog Data
	//{{AFX_DATA(CEditor)
	enum { IDD = IDD_EDITOR };
	CSliderCtrl	m_SlideRed;
	CSliderCtrl	m_SlideGreen;
	CSliderCtrl	m_SlideBlue;
	CEdit	m_Red;
	CEdit	m_Green;
	CEdit	m_Blue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEditor)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEditor)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEdtBlue();
	afx_msg void OnChangeEdtGreen();
	afx_msg void OnChangeEdtRed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif
