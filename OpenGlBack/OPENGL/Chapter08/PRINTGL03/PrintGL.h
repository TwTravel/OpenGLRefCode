// PrintGL.h : main header file for the PRINTGL application
//

#if !defined(AFX_PRINTGL_H__DEF59F07_6A94_11D2_8A14_00104B09950B__INCLUDED_)
#define AFX_PRINTGL_H__DEF59F07_6A94_11D2_8A14_00104B09950B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPrintGLApp:
// See PrintGL.cpp for the implementation of this class
//

class CPrintGLApp : public CWinApp
{
public:
	CPrintGLApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintGLApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPrintGLApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTGL_H__DEF59F07_6A94_11D2_8A14_00104B09950B__INCLUDED_)
