// JgfzView.h : interface of the CJgfzView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#if !defined(AFX_JGFZVIEW_H__AD0CFFCD_8176_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_JGFZVIEW_H__AD0CFFCD_8176_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#define   BAR_1			1
#define   BAR_2			2
#define   TAOTONG       3
#define   TAOGUA		4
#define   ZOUCHENG_X	5
#define   ZIZUOUP_1		6
#define	  ZIZUOUP_2		7
#define   ZOUCHENG_S	8
 

enum{Move, Static};

enum{AbsoluteMovement,RelativeTrace1,RelativeMovement1,RelativeTrace2,RelativeMovement2,
		RelativeTrace3,RelativeMovement3};


class CJgfzView : public CView
{
protected: // create from serialization only
	CJgfzView();
	DECLARE_DYNCREATE(CJgfzView)

// Attributes
public:
	CJgfzDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJgfzView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CJgfzView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	UINT  m_MoveStatic;

	GLfloat m_rotateAngle;
	GLfloat m_speedFactor;
	
	GLfloat m_height;
	GLfloat m_bar1Length;
	GLfloat m_bar2Length;
	GLfloat m_radius;
	GLfloat m_taotongLength;
	GLfloat m_taotongRadius;
	BOOL	m_radiusFlag;

	GLdouble m_zizuoCoefficient;
	GLdouble m_zizuoThick;
	GLdouble m_zizuoWidth1;
	GLdouble m_zizuoWidth2;
	GLdouble m_zizuoOffset;
	
	UINT	m_absoluteRelative;
	
	BOOL	m_copyImage;

// Generated message map functions
protected:
	void EnableLighting();
	void DrawRelativeCurve1();
	void DrawBackground();
	void DrawXY();
	void DrawCoordinate();
	void DrawStatic();
	void DrawMove();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CJgfzView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnAbsolutemovement();
	afx_msg void OnControlMove();
	afx_msg void OnControlStatic();
	afx_msg void OnRelativemove1();
	afx_msg void OnTrace1();
	afx_msg void OnSpeedFast();
	afx_msg void OnSpeedSlow();
	afx_msg void OnSpeedUltra();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in JgfzView.cpp
inline CJgfzDoc* CJgfzView::GetDocument()
   { return (CJgfzDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JGFZVIEW_H__AD0CFFCD_8176_11D2_83B3_0080C832FCF3__INCLUDED_)
