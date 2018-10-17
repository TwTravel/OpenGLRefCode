// TwoDShapeView.h : interface of the CTwoDShapeView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum{Points,Lines,LineStipple,LineStrip,LineLoop,Polygons,
		PolygonStipple,Triangles,TriangleStrip,
		TriangleFan,Quadrilaterals,QuadStrip,
		NonConvex,Cube};


#if !defined(AFX_TWODSHAPEVIEW_H__AFB5CCED_1FB3_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_TWODSHAPEVIEW_H__AFB5CCED_1FB3_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTwoDShapeView : public CView
{
protected: // create from serialization only
	CTwoDShapeView();
	DECLARE_DYNCREATE(CTwoDShapeView)

// Attributes
public:
	CTwoDShapeDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTwoDShapeView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	
	BOOL bSetupPixelFormat();
	void Init();
	virtual ~CTwoDShapeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	int m_twoDShape;

	void DrawNonConvex();
	void DrawQuadStrip();
	void DrawQuadrilaterals();
	void DrawTriangleFan();
	void DrawTriangleStrip();
	void DrawTriangles();
	void DrawPolygonStipple();
	void DrawPolygons();
	void DrawLineLoop();
	void DrawLineStrip();
	void DrawLineStipple();
	void DrawLines();
	void DrawPoints();
	void DrawScene();
	void DrawCube();

// Generated message map functions
protected:
	//{{AFX_MSG(CTwoDShapeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCube();
	afx_msg void OnUpdateCube(CCmdUI* pCmdUI);
	afx_msg void OnLineLoop();
	afx_msg void OnUpdateLineLoop(CCmdUI* pCmdUI);
	afx_msg void OnLineStipple();
	afx_msg void OnUpdateLineStipple(CCmdUI* pCmdUI);
	afx_msg void OnLineStrip();
	afx_msg void OnUpdateLineStrip(CCmdUI* pCmdUI);
	afx_msg void OnLines();
	afx_msg void OnUpdateLines(CCmdUI* pCmdUI);
	afx_msg void OnNonconvex();
	afx_msg void OnUpdateNonconvex(CCmdUI* pCmdUI);
	afx_msg void OnPoints();
	afx_msg void OnUpdatePoints(CCmdUI* pCmdUI);
	afx_msg void OnPolygon();
	afx_msg void OnUpdatePolygon(CCmdUI* pCmdUI);
	afx_msg void OnPolygonStipple();
	afx_msg void OnUpdatePolygonStipple(CCmdUI* pCmdUI);
	afx_msg void OnQuadStrip();
	afx_msg void OnUpdateQuadStrip(CCmdUI* pCmdUI);
	afx_msg void OnQuadrilaterals();
	afx_msg void OnUpdateQuadrilaterals(CCmdUI* pCmdUI);
	afx_msg void OnTriangleFan();
	afx_msg void OnUpdateTriangleFan(CCmdUI* pCmdUI);
	afx_msg void OnTriangleStrip();
	afx_msg void OnUpdateTriangleStrip(CCmdUI* pCmdUI);
	afx_msg void OnTriangles();
	afx_msg void OnUpdateTriangles(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TwoDShapeView.cpp
inline CTwoDShapeDoc* CTwoDShapeView::GetDocument()
   { return (CTwoDShapeDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TWODSHAPEVIEW_H__AFB5CCED_1FB3_11D2_83B2_0080C832FCF3__INCLUDED_)
