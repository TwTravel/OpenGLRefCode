// EffectView.h : interface of the CEffectView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum{Alpha2D,Alpha3D,AntialiasLine,AntialiasPlgn,AntialiasCube,Fog};

#if !defined(AFX_EFFECTVIEW_H__C659A04F_4A4F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_EFFECTVIEW_H__C659A04F_4A4F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CEffectView : public CView
{
protected: // create from serialization only
	CEffectView();
	DECLARE_DYNCREATE(CEffectView)

// Attributes
public:
	CEffectDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEffectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	GLint	m_typeControl;

// Generated message map functions
protected:
	void AntiCube();
	GLint Sphere(GLdouble radius);
	void FogScene();
	void AntialiasTriangle();
	void AntialiasLineSphere();
	void DrawCube(GLdouble dLength,GLdouble dWidth,GLdouble dHeight);
	void BlendObjects();
	void BlendQuads();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CEffectView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAlpha2d();
	afx_msg void OnUpdateAlpha2d(CCmdUI* pCmdUI);
	afx_msg void OnAlpha3d();
	afx_msg void OnUpdateAlpha3d(CCmdUI* pCmdUI);
	afx_msg void OnAntiLine();
	afx_msg void OnUpdateAntiLine(CCmdUI* pCmdUI);
	afx_msg void OnAntiPlgn();
	afx_msg void OnUpdateAntiPlgn(CCmdUI* pCmdUI);
	afx_msg void OnAntiCube();
	afx_msg void OnUpdateAntiCube(CCmdUI* pCmdUI);
	afx_msg void OnFog();
	afx_msg void OnUpdateFog(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in EffectView.cpp
inline CEffectDoc* CEffectView::GetDocument()
   { return (CEffectDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTVIEW_H__C659A04F_4A4F_11D2_83B2_0080C832FCF3__INCLUDED_)
