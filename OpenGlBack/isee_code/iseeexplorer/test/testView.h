// testView.h : interface of the CTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTVIEW_H__E3EA57CC_5C2C_11D5_8853_9341F819D4F2__INCLUDED_)
#define AFX_TESTVIEW_H__E3EA57CC_5C2C_11D5_8853_9341F819D4F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "iseeirw.h"


#define		TESTPOPMENUREF_BEGIN		1000
#define		TESTPOPMENUREF_END			1200

class CTestView : public CView
{
protected: // create from serialization only
	CTestView();
	DECLARE_DYNCREATE(CTestView)

// Attributes
public:
	CTestDoc* GetDocument();

	HDRAWDIB		m_hDrawDib;

	LPINFOSTR		pinfo;
	LPBITMAPINFO	lpbmi;
	CMenu			cmenu;
	CMenu			cpopmenu;
	CMenu			*pimenu;
	
	bool			m_mark;

	IRWP_API_GET_IMG_INFO		fn_info;
	IRWP_API_LOAD_IMG			fn_load;
	IRWP_API_SAVE_IMG			fn_save;
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTestView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFileBegin();
	afx_msg void OnFilePause();
	afx_msg void OnFileStop();
	afx_msg void OnToolsExp();
	afx_msg void OnSpe1();
	afx_msg void OnSpe2();
	afx_msg void OnSpe3();
	afx_msg void OnSpe4();
	afx_msg void OnSpe5();
	afx_msg void OnSpe6();
	afx_msg void OnSpe7();
	afx_msg void OnUpdateSpe1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpe2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpe3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpe4(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpe5(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpe6(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpe7(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileBegin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileStop(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnToolsDirector();
	afx_msg void OnFilePrev();
	afx_msg void OnFileNext();
	//}}AFX_MSG
	afx_msg void OnPluginSel(UINT nID);
	afx_msg void OnUpdatePluginSel(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in testView.cpp
inline CTestDoc* CTestView::GetDocument()
   { return (CTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTVIEW_H__E3EA57CC_5C2C_11D5_8853_9341F819D4F2__INCLUDED_)
