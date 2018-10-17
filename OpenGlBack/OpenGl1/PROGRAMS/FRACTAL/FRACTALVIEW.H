// FractalView.h : interface of the CFractalView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum{RabtToTos,Fractal};

#if !defined(AFX_FRACTALVIEW_H__19ED91CD_7FDF_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_FRACTALVIEW_H__19ED91CD_7FDF_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CFractalView : public CView
{
protected: // create from serialization only
	CFractalView();
	DECLARE_DYNCREATE(CFractalView)

// Attributes
public:
	CFractalDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFractalView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFractalView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	int m_controlType;

// Generated message map functions
protected:
	void CreateFractals(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	void myFractal();
	GLfloat MoveY(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
	GLfloat MoveX(GLfloat x1,GLfloat  y1,GLfloat  x2,GLfloat  y2);
	GLdouble Distance(GLfloat x1,GLfloat y1,GLfloat x2,GLfloat y2);
	void Chase();
	void MoveThem(GLfloat* fa,GLfloat fb,GLfloat ft);
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CFractalView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRabbit();
	afx_msg void OnUpdateRabbit(CCmdUI* pCmdUI);
	afx_msg void OnFractal();
	afx_msg void OnUpdateFractal(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in FractalView.cpp
inline CFractalDoc* CFractalView::GetDocument()
   { return (CFractalDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRACTALVIEW_H__19ED91CD_7FDF_11D2_83B3_0080C832FCF3__INCLUDED_)
