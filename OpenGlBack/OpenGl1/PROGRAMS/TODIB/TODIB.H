// ToDib.h : main header file for the TODIB application
//

#if !defined(AFX_TODIB_H__7C3B2DE7_805E_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_TODIB_H__7C3B2DE7_805E_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CToDibApp:
// See ToDib.cpp for the implementation of this class
//

class CToDibApp : public CWinApp
{
public:
	CToDibApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDibApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CToDibApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODIB_H__7C3B2DE7_805E_11D2_83B4_0080C832FCF3__INCLUDED_)
