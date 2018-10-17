// StepinGlView.h : interface of the CStepinGlView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#if !defined(AFX_STEPINGLVIEW_H__4D9DCF8F_170C_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_STEPINGLVIEW_H__4D9DCF8F_170C_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CStepinGlView : public CView
{
protected: // create from serialization only
	CStepinGlView();
	DECLARE_DYNCREATE(CStepinGlView)

// Attributes
public:
	CStepinGlDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStepinGlView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	virtual ~CStepinGlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CStepinGlView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in StepinGlView.cpp
inline CStepinGlDoc* CStepinGlView::GetDocument()
   { return (CStepinGlDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STEPINGLVIEW_H__4D9DCF8F_170C_11D2_83B2_0080C832FCF3__INCLUDED_)
