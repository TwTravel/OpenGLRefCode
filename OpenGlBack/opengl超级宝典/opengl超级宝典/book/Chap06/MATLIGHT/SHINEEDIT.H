// ShineEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShineEdit dialog

#ifndef __SHINEEDIT
#define __SHINEEDIT

class CShineEdit : public CPropertyPage
{
	DECLARE_DYNCREATE(CShineEdit)

// Construction
public:
	CShineEdit();
	~CShineEdit();

	int GetValue(void);
	void SetValue(int nValue);

	CPropertySheet *m_pParent;

	virtual void OnOK();

// Dialog Data
	//{{AFX_DATA(CShineEdit)
	enum { IDD = IDD_SHINE };
	CSpinButtonCtrl	m_Spinner;
	int		m_ShineValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CShineEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CShineEdit)
	virtual BOOL OnInitDialog();
	afx_msg void OnDeltaposSpnShine(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeEdtShine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


#endif
