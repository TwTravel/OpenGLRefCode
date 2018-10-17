// CuvSufcView.h : interface of the CCuvSufcView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"
#include "dib.h"

enum{BezierCurve,EvenBezier,NurbsCurve,
	BezierSurfaceMesh,BezierSurfaceFill,
	BezierSurfaceTex,NurbsSurfaceMesh,
	NurbsSurfaceFill,NurbsSurfacePach,
	NurbsSurfaceTrim};

#if !defined(AFX_CUVSUFCVIEW_H__AA14DC0D_522F_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_CUVSUFCVIEW_H__AA14DC0D_522F_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCuvSufcView : public CView
{
protected: // create from serialization only
	CCuvSufcView();
	DECLARE_DYNCREATE(CCuvSufcView)

// Attributes
public:
	CCuvSufcDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

	CDib* m_pDib;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCuvSufcView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCuvSufcView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	GLint	m_typeControl;
	GLuint  m_nurbsSurfaceMode;
	
// Generated message map functions
protected:
	void NurbsSufacTrim();
	void NurbsSurface();
	void BzrSufacTex();
	void Lighting();
	void BzrSufacFill();
	void BzrSufacMesh();
	void DrawNurbsCurve();
	void EvenBezierCurve();
	void DrawBezierCurve();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CCuvSufcView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBezierCurve();
	afx_msg void OnUpdateBezierCurve(CCmdUI* pCmdUI);
	afx_msg void OnEvenBezierCuv();
	afx_msg void OnUpdateEvenBezierCuv(CCmdUI* pCmdUI);
	afx_msg void OnNurbsCurve();
	afx_msg void OnUpdateNurbsCurve(CCmdUI* pCmdUI);
	afx_msg void OnBzrsufacMesh();
	afx_msg void OnUpdateBzrsufacMesh(CCmdUI* pCmdUI);
	afx_msg void OnBzrsufacFill();
	afx_msg void OnUpdateBzrsufacFill(CCmdUI* pCmdUI);
	afx_msg void OnBzrsufacTexture();
	afx_msg void OnUpdateBzrsufacTexture(CCmdUI* pCmdUI);
	afx_msg void OnNurbssufacMesh();
	afx_msg void OnUpdateNurbssufacMesh(CCmdUI* pCmdUI);
	afx_msg void OnNurbssufacFill();
	afx_msg void OnUpdateNurbssufacFill(CCmdUI* pCmdUI);
	afx_msg void OnNurbssufacPach();
	afx_msg void OnUpdateNurbssufacPach(CCmdUI* pCmdUI);
	afx_msg void OnNurbssufacTrim();
	afx_msg void OnUpdateNurbssufacTrim(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CuvSufcView.cpp
inline CCuvSufcDoc* CCuvSufcView::GetDocument()
   { return (CCuvSufcDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUVSUFCVIEW_H__AA14DC0D_522F_11D2_83B2_0080C832FCF3__INCLUDED_)
