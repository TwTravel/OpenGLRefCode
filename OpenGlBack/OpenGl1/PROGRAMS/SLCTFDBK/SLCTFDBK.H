// SlctFdbk.h : main header file for the SLCTFDBK application
//

#if !defined(AFX_SLCTFDBK_H__BD34BF05_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_SLCTFDBK_H__BD34BF05_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSlctFdbkApp:
// See SlctFdbk.cpp for the implementation of this class
//

class CSlctFdbkApp : public CWinApp
{
public:
	CSlctFdbkApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlctFdbkApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSlctFdbkApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLCTFDBK_H__BD34BF05_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_)
