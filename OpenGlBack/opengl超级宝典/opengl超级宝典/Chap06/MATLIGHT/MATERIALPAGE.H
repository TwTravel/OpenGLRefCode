// MaterialPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMaterialPage dialog

class CMaterialPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CMaterialPage)

// Construction
public:
	CMaterialPage();
	~CMaterialPage();

// Dialog Data
	//{{AFX_DATA(CMaterialPage)
	enum { IDD = IDD_MATERIAL };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMaterialPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMaterialPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
