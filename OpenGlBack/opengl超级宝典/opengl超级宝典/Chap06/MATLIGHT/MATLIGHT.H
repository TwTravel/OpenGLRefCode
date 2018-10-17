// matlight.h : main header file for the MATLIGHT application
//
#ifndef __MATLIGHT__
#define __MATLIGHT__


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define WM_UPDATE	WM_USER+1000

/////////////////////////////////////////////////////////////////////////////
// CMatlightApp:
// See matlight.cpp for the implementation of this class
//

class CMatlightApp : public CWinApp
{
public:
	CMatlightApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatlightApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMatlightApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
#endif
