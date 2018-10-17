// IntractvView.h : interface of the CIntractvView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#if !defined(AFX_INTRACTVVIEW_H__5BAB0C82_7D5E_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_INTRACTVVIEW_H__5BAB0C82_7D5E_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CIntractvView : public CView
{
protected: // create from serialization only
	CIntractvView();
	DECLARE_DYNCREATE(CIntractvView)

// Attributes
public:
	CIntractvDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntractvView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CIntractvView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLint	m_framesNum;
	GLint	m_widthX, m_widthY;
	GLint	m_checkerSize;
	GLint	m_contouring;
	GLint	m_curFrame;
	GLint	m_nextFrame;
	
	GLenum	m_smooth;
	GLenum	m_lighting;
	GLenum	m_stepControl;
	GLenum	m_rotate;
	GLenum	m_clearMask;
	GLenum	m_depth;

	GLfloat m_rotateAngle;
	GLfloat	m_height;

	BOOL	m_play;

	struct m_facet {
		float color[3];
		float normal[3];
	};

	struct m_coord {
		float vertex[3];
		float normal[3];
	};

	struct mesh {
		GLint m_widthX, m_widthY;
		GLint numFacets;
		GLint numCoords;
		GLint numFrames;
		struct m_coord *coords;
		struct m_facet *facets;
	} m_waveMesh;


// Generated message map functions
protected:
	void KeyDown_R();
	void KeyDown_F();
	void KeyDown_K();
	void KeyDown_D();
	void KeyDown_L();
	void KeyDown_S();
	void KeyDown_T();
	void Wave();
	void InitTexture();
	void InitMaterials();
	void InitMesh();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CIntractvView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in IntractvView.cpp
inline CIntractvDoc* CIntractvView::GetDocument()
   { return (CIntractvDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTRACTVVIEW_H__5BAB0C82_7D5E_11D2_83B3_0080C832FCF3__INCLUDED_)
