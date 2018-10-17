// ImageView.h : interface of the CImageView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#if !defined(AFX_IMAGEVIEW_H__2B24058D_41B6_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_IMAGEVIEW_H__2B24058D_41B6_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CImageView : public CView
{
protected: // create from serialization only
	CImageView();
	DECLARE_DYNCREATE(CImageView)

// Attributes
public:
	CImageDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLuint fontOffset;

// Generated message map functions
protected:
	void DrawText();
	void BitmapFont();
	void myImage();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CImageView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ImageView.cpp
inline CImageDoc* CImageView::GetDocument()
   { return (CImageDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEVIEW_H__2B24058D_41B6_11D2_83B2_0080C832FCF3__INCLUDED_)
