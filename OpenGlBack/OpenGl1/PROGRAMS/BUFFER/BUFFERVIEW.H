// BufferView.h : interface of the CBufferView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum{StencilMask,StencilClip,AntiAccum,
		NonAntiAccum,Focus};

#define ACCUMLIST	1

#if !defined(AFX_BUFFERVIEW_H__6BD43DCD_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_BUFFERVIEW_H__6BD43DCD_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CBufferView : public CView
{
protected: // create from serialization only
	CBufferView();
	DECLARE_DYNCREATE(CBufferView)

// Attributes
public:
	CBufferDoc* GetDocument();
	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBufferView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBufferView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLint m_typeControl;

// Generated message map functions
protected:
	void FocusDepth();
	void myPerspective(GLdouble fovy, GLdouble aspect, 
		GLdouble znear, GLdouble zfar, GLdouble eyedx, 
		GLdouble eyedy, GLdouble focus);
	void myFrustum(GLdouble left, GLdouble right, 
		GLdouble bottom,GLdouble top, GLdouble znear,
		GLdouble zfar, 	GLdouble eyedx, 
		GLdouble eyedy, GLdouble focus);
	void NonAccumuScene();
	void RenderObjects();
	void AccumlateScene();
	void StencilAppClip();
	void StencilAppMask();
	void DrawSphere(GLdouble dRadius);
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CBufferView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnStencilMask();
	afx_msg void OnUpdateStencilMask(CCmdUI* pCmdUI);
	afx_msg void OnStencilClip();
	afx_msg void OnUpdateStencilClip(CCmdUI* pCmdUI);
	afx_msg void OnAntiscene();
	afx_msg void OnUpdateAntiscene(CCmdUI* pCmdUI);
	afx_msg void OnNonAntiaccum();
	afx_msg void OnCancelMode();
	afx_msg void OnUpdateNonAntiaccum(CCmdUI* pCmdUI);
	afx_msg void OnFocusDepth();
	afx_msg void OnUpdateFocusDepth(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BufferView.cpp
inline CBufferDoc* CBufferView::GetDocument()
   { return (CBufferDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUFFERVIEW_H__6BD43DCD_5A2F_11D2_83B2_0080C832FCF3__INCLUDED_)
