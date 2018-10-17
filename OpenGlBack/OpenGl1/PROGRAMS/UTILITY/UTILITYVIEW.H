// UtilityView.h : interface of the CUtilityView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#include "Dib.h"

enum{Quadrics,ScaleDib,AutoMipmap};

#if !defined(AFX_UTILITYVIEW_H__A6B1CFAF_4E65_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_UTILITYVIEW_H__A6B1CFAF_4E65_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CUtilityView : public CView
{
protected: // create from serialization only
	CUtilityView();
	DECLARE_DYNCREATE(CUtilityView)

// Attributes
public:
	CUtilityDoc* GetDocument();
	CClientDC	*m_pDC;
	CRect		m_oldRect;

	CDib* m_pDib;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUtilityView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUtilityView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLint m_typeControl;
	GLint m_iWindowWidth;
	GLint m_iWindowHeight;
// Generated message map functions
protected:
	void AutoMipmapping();
	void ScaleImage();
	void DrawQuadrics();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CUtilityView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnQuadrics();
	afx_msg void OnUpdateQuadrics(CCmdUI* pCmdUI);
	afx_msg void OnScaleImage();
	afx_msg void OnUpdateScaleImage(CCmdUI* pCmdUI);
	afx_msg void OnAutoMipmapping();
	afx_msg void OnUpdateAutoMipmapping(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UtilityView.cpp
inline CUtilityDoc* CUtilityView::GetDocument()
   { return (CUtilityDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UTILITYVIEW_H__A6B1CFAF_4E65_11D2_83B2_0080C832FCF3__INCLUDED_)
