// ModelView.h : interface of the CModelView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

#define PI ((float)3.14159265358979323846)

#define COMPUTE_SPHERE_VERTICES(rings, sections) \
   (((rings)+1)*(sections)+2)

#define COMPUTE_SPHERE_TRIANGLES(rings, sections) \
    (((rings)+1)*(sections)*2)

#define RINGS		50
#define SECTIONS	50

#define SVERTICES COMPUTE_SPHERE_VERTICES(RINGS, SECTIONS)
#define STRIANGLES COMPUTE_SPHERE_TRIANGLES(RINGS, SECTIONS)

#define RGB_COLOR(red, green, blue) \
    (((DWORD)(BYTE)(red) << 0) | \
     ((DWORD)(BYTE)(green) << 8) | \
     ((DWORD)(BYTE)(blue) << 16))
    
#define FRANDOM(x) (((float)rand() / RAND_MAX) * (x))

#define DROT		10.0f

#define NLISTS		3
    
/////////////////////////////////////////////////
#if !defined(AFX_MODELVIEW_H__3009D04D_7F26_11D2_83B3_0080C832FCF3__INCLUDED_)
#define AFX_MODELVIEW_H__3009D04D_7F26_11D2_83B3_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CModelView : public CView
{
protected: // create from serialization only
	CModelView();
	DECLARE_DYNCREATE(CModelView)

// Attributes
public:
	CModelDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModelView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CModelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	typedef struct
	{
		float x, y, z;			//µã
		float fNx, fNy, fNz;	//·¨Ïß
		DWORD dwColor;
	} VERTEX;

	typedef struct
	{
		int iV1;
		int iV2;
		int iV3;
	} TRIANGLE;		

	typedef struct _SPHERE
	{
		GLfloat xr, yr, zr;
		GLfloat fDXr, fDYr, fDZr;
	} SPHERE;

	SPHERE m_sphere;

	BOOL m_fDisplayList;

	int m_iCurrentMode;

	GLint m_iLists[NLISTS];

	DWORD m_dwTotalMillis;

	long m_iTotalTriangles;

	VERTEX m_vtxSphere[SVERTICES];

	TRIANGLE m_triSphere[STRIANGLES];

	BOOL	m_play;
	BOOL	m_controlMode;
	GLenum	m_selectMode;
	

// Generated message map functions
protected:
	void Key_S();
	void Key_Space();
	void ResetTotals();
	void PlayBall();
	void Init_Sphere_Data();
	void Vertex(int iVert);
	void UpdateSphere();
	void InitSphere();
	void GenerateSphere(float fRadius, int iRings, int iSections,
                    VERTEX *pvtx, TRIANGLE *ptri);
	void DrawStrips();
	void DrawTriangles();
	void DrawVertexArray();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CModelView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ModelView.cpp
inline CModelDoc* CModelView::GetDocument()
   { return (CModelDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODELVIEW_H__3009D04D_7F26_11D2_83B3_0080C832FCF3__INCLUDED_)
