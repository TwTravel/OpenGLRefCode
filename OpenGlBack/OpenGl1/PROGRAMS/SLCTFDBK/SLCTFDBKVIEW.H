// SlctFdbkView.h : interface of the CSlctFdbkView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum{Select,Feedback};

#if !defined(AFX_SLCTFDBKVIEW_H__BD34BF0D_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_SLCTFDBKVIEW_H__BD34BF0D_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSlctFdbkView : public CView
{
protected: // create from serialization only
	CSlctFdbkView();
	DECLARE_DYNCREATE(CSlctFdbkView)

// Attributes
public:
	CSlctFdbkDoc* GetDocument();
	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlctFdbkView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSlctFdbkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLint BackColor[2][2],m_modeControl;
	GLfloat		y;
// Generated message map functions
protected:
	void DrawNonSelect();
	void Circle(GLfloat dRadius,GLfloat fLine,GLfloat n);
	void CreateSpheres();
	void OutputFloatValue(GLfloat position,
			GLfloat value);
	void VertexData(GLint size, 
		GLint *count, GLfloat *buffer);
	void OutputInfo(GLint size,GLfloat* buffer);
	void ApplyFeedback();
	void DrawFdbkObjects(GLenum mode);
	void WhatHits(GLint hits, GLuint* buffer);
	void DrawNameObjects(GLenum mode);
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CSlctFdbkView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelect();
	afx_msg void OnUpdateSelect(CCmdUI* pCmdUI);
	afx_msg void OnFeedback();
	afx_msg void OnUpdateFeedback(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SlctFdbkView.cpp
inline CSlctFdbkDoc* CSlctFdbkView::GetDocument()
   { return (CSlctFdbkDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SLCTFDBKVIEW_H__BD34BF0D_5C2D_11D2_83B3_0080C832FCF3__INCLUDED_)
