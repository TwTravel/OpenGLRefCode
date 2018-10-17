#if !defined(AFX_TODIBDLG_H__217BE4A3_80B1_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_TODIBDLG_H__217BE4A3_80B1_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ToDibDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CToDibDlg dialog

class CToDibDlg : public CDialog
{
// Construction
public:
	CToDibDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CToDibDlg)
	enum { IDD = IDD_DIALOG1 };
	int		m_leftX;
	int		m_height;
	int		m_width;
	int		m_bottomY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDibDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CToDibDlg)
 	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODIBDLG_H__217BE4A3_80B1_11D2_83B4_0080C832FCF3__INCLUDED_)
