// Buffer.h : main header file for the BUFFER application
//

#if !defined(AFX_BUFFER_H__6BD43DC5_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_BUFFER_H__6BD43DC5_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CBufferApp:
// See Buffer.cpp for the implementation of this class
//

class CBufferApp : public CWinApp
{
public:
	CBufferApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBufferApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBufferApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUFFER_H__6BD43DC5_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_)
