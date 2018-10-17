// GLTransView.h : interface of the CGLTransView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum{Cube,Translate,Rotate,Scale,Clip,Assemble,Truck};

#if !defined(AFX_GLTRANSVIEW_H__80A3B92D_25FE_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_GLTRANSVIEW_H__80A3B92D_25FE_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CGLTransView : public CView
{
protected: // create from serialization only
	CGLTransView();
	DECLARE_DYNCREATE(CGLTransView)

// Attributes
public:
	CGLTransDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGLTransView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL bSetupPixelFormat();
	void Init();
	virtual ~CGLTransView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
	int m_controlTrans;

// Generated message map functions
protected:
	void DrawWheel(GLdouble dWheelRadius);
	void DrawQuadrangle(GLdouble dQuadWidth,GLdouble dQuadHeight);
	void PfmMatrixStack();
	void PfmAssemble();
	void PfmClip();
	void PfmScale();
	void PfmRotate();
	void PfmTranslate();
	void DrawCube(GLdouble dLength);
	void DrawScene();
	//{{AFX_MSG(CGLTransView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCube();
	afx_msg void OnUpdateCube(CCmdUI* pCmdUI);
	afx_msg void OnTranslate();
	afx_msg void OnUpdateTranslate(CCmdUI* pCmdUI);
	afx_msg void OnRotate();
	afx_msg void OnUpdateRotate(CCmdUI* pCmdUI);
	afx_msg void OnScale();
	afx_msg void OnUpdateScale(CCmdUI* pCmdUI);
	afx_msg void OnClip();
	afx_msg void OnUpdateClip(CCmdUI* pCmdUI);
	afx_msg void OnAssemble();
	afx_msg void OnUpdateAssemble(CCmdUI* pCmdUI);
	afx_msg void OnMatrixStack();
	afx_msg void OnUpdateMatrixStack(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GLTransView.cpp
inline CGLTransDoc* CGLTransView::GetDocument()
   { return (CGLTransDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GLTRANSVIEW_H__80A3B92D_25FE_11D2_83B2_0080C832FCF3__INCLUDED_)
