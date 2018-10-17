// ShadowView.h : interface of the CShadowView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#if !defined(AFX_SHADOWVIEW_H__2F29B08D_8086_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_SHADOWVIEW_H__2F29B08D_8086_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CShadowView : public CView
{
protected: // create from serialization only
	CShadowView();
	DECLARE_DYNCREATE(CShadowView)

// Attributes
public:
	CShadowDoc* GetDocument();
	
	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShadowView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CShadowView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLfloat		m_lightPos_x;
	GLfloat		m_lightPos_y;
	GLfloat		m_lightPos_z;
// Generated message map functions
protected:
	
	void	Ground();
	void	CreateSun();
	void	DrawScene();
	BOOL	bSetupPixelFormat();
	void	Init();
	//{{AFX_MSG(CShadowView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ShadowView.cpp
inline CShadowDoc* CShadowView::GetDocument()
   { return (CShadowDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHADOWVIEW_H__2F29B08D_8086_11D2_83B3_0080C832FCF3__INCLUDED_)
