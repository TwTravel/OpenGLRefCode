// Effect.h : main header file for the EFFECT application
//

#if !defined(AFX_EFFECT_H__C659A047_4A4F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_EFFECT_H__C659A047_4A4F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CEffectApp:
// See Effect.cpp for the implementation of this class
//

class CEffectApp : public CWinApp
{
public:
	CEffectApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CEffectApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECT_H__C659A047_4A4F_11D2_83B2_0080C832FCF3__INCLUDED_)
