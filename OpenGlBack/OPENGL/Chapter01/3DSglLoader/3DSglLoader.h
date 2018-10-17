// 3DSglLoader.h : main header file for the 3DSGLLOADER application
//

#if !defined(AFX_MYSDOPENGL_H__991FDB70_0EB8_4CD6_B11E_20D1AAAF5177__INCLUDED_)
#define AFX_MYSDOPENGL_H__991FDB70_0EB8_4CD6_B11E_20D1AAAF5177__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy3DSglLoaderApp:
// See 3DSglLoader.cpp for the implementation of this class
//

class CMy3DSglLoaderApp : public CWinApp
{
public:
	CMy3DSglLoaderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy3DSglLoaderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CMy3DSglLoaderApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSDOPENGL_H__991FDB70_0EB8_4CD6_B11E_20D1AAAF5177__INCLUDED_)
