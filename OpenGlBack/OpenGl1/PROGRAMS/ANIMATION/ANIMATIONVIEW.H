// AnimationView.h : interface of the CAnimationView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"


#if !defined(AFX_ANIMATIONVIEW_H__361B17ED_40B1_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_ANIMATIONVIEW_H__361B17ED_40B1_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CAnimationView : public CView
{
protected: // create from serialization only
	CAnimationView();
	DECLARE_DYNCREATE(CAnimationView)

// Attributes
public:
	CAnimationDoc* GetDocument();
	
	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnimationView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAnimationView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLfloat  m_fAngleY;
    GLfloat  m_fAngleX;
    GLfloat  m_fAngleZ;
	BOOL	 m_play;

// Generated message map functions
protected:
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CAnimationView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPlayPause();
	afx_msg void OnUpdatePlayPause(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AnimationView.cpp
inline CAnimationDoc* CAnimationView::GetDocument()
   { return (CAnimationDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIMATIONVIEW_H__361B17ED_40B1_11D2_83B2_0080C832FCF3__INCLUDED_)
