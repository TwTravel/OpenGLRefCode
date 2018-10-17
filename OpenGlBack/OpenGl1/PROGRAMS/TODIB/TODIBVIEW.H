// ToDibView.h : interface of the CToDibView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#if !defined(AFX_TODIBVIEW_H__7C3B2DEF_805E_11D2_83B4_0080C832FCF3__INCLUDED_)
#define AFX_TODIBVIEW_H__7C3B2DEF_805E_11D2_83B4_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

DECLARE_HANDLE(HDIB);

class CToDibView : public CView
{
protected: // create from serialization only
	CToDibView();
	DECLARE_DYNCREATE(CToDibView)

// Attributes
public:
	CToDibDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDibView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToDibView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLint	m_iListBase;
	CRect	m_currentRect;
	int		m_dibWidth;
	int		m_dibHeight;
	int		m_dibLeft;
	int		m_dibBottom;

// Generated message map functions
protected:
	void DrawSphere();
	void DrawBack();
	void InitObjects();
	void CalcuNormal(GLfloat* fV,GLfloat* fN);
	void Circle(GLfloat dRadius,
			GLfloat dThick,GLint n);
	void EnableLighting();
	void Artistic();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CToDibView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSaveToDib();
	afx_msg void OnDibSet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ToDibView.cpp
inline CToDibDoc* CToDibView::GetDocument()
   { return (CToDibDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODIBVIEW_H__7C3B2DEF_805E_11D2_83B4_0080C832FCF3__INCLUDED_)
