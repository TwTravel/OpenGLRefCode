// LightPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLightPage dialog

class CLightPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CLightPage)

// Construction
public:
	CLightPage();
	~CLightPage();

// Dialog Data
	//{{AFX_DATA(CLightPage)
	enum { IDD = IDD_LIGHT };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CLightPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CLightPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
