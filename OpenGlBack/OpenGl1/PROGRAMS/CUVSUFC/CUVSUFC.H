// CuvSufc.h : main header file for the CUVSUFC application
//

#if !defined(AFX_CUVSUFC_H__AA14DC05_522F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_CUVSUFC_H__AA14DC05_522F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CCuvSufcApp:
// See CuvSufc.cpp for the implementation of this class
//

class CCuvSufcApp : public CWinApp
{
public:
	CCuvSufcApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCuvSufcApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCuvSufcApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUVSUFC_H__AA14DC05_522F_11D2_83B2_0080C832FCF3__INCLUDED_)
