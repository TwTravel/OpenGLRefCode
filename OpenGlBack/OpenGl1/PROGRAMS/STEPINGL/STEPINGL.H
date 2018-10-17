// StepinGl.h : main header file for the STEPINGL application
//

#if !defined(AFX_STEPINGL_H__4D9DCF87_170C_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_STEPINGL_H__4D9DCF87_170C_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CStepinGlApp:
// See StepinGl.cpp for the implementation of this class
//

class CStepinGlApp : public CWinApp
{
public:
	CStepinGlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStepinGlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CStepinGlApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEPINGL_H__4D9DCF87_170C_11D2_83B2_0080C832FCF3__INCLUDED_)
