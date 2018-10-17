// LightView.h : interface of the CLightView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"

enum {Cube,Sphere,SmoothCube};


#if !defined(AFX_LIGHTVIEW_H__9F8D41CD_2C7B_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_LIGHTVIEW_H__9F8D41CD_2C7B_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CLightView : public CView
{
protected: // create from serialization only
	CLightView();
	DECLARE_DYNCREATE(CLightView)

// Attributes
public:
	CLightDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;
    CPalette    m_cPalette;
    CPalette    *m_pOldPalette;


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void CreateRGBPalette();
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	virtual ~CLightView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	
	int			m_selectObject;

	GLfloat		m_colorRed;
	GLfloat		m_colorGreen;
	GLfloat		m_colorBlue;
	BOOL		m_bMaskRed;
	BOOL		m_bMaskGreen;
	BOOL		m_bMaskBlue;

	GLfloat		m_lightAmb[4];
	GLfloat		m_lightDif[4];
	GLfloat		m_lightSpe[4];
	GLfloat		m_lightPos[4];

	GLfloat		m_materialAmb[4];
	GLfloat		m_materialDif[4];
	GLfloat		m_materialSpe[4];
	GLfloat		m_materialEmi[4];
	GLfloat		m_matShininess;
	BOOL		m_bMaterial;
	
	GLfloat		m_spotAmb[4];
	GLfloat		m_spotDif[4];
	GLfloat		m_spotSpe[4];
	GLfloat		m_spotPos[4];
	GLfloat		m_spotDir[4];
	GLfloat		m_spotCutoff;
	
	BOOL	m_bConstant;
	BOOL	m_bLinear;
	BOOL	m_bQuadratic;
	BOOL	m_bSpotOnOff;

// Generated message map functions
protected:
	void DrawSmoothCube();
	void CalculateNormal(GLdouble  * dVertex1, GLdouble  * dVertex2, GLdouble  * dVertex3, GLdouble  * dNormal);
	void CreateMaterial();
	void CreateLights();
	void DrawSphere();
	void DrawCube();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CLightView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCube();
	afx_msg void OnUpdateCube(CCmdUI* pCmdUI);
	afx_msg void OnSphere();
	afx_msg void OnUpdateSphere(CCmdUI* pCmdUI);
	afx_msg void OnColorMaterial();
	afx_msg void OnUpdateColorMaterial(CCmdUI* pCmdUI);
	afx_msg void OnLight();
	afx_msg void OnUpdateLight(CCmdUI* pCmdUI);
	afx_msg void OnSmoothCube();
	afx_msg void OnUpdateSmoothCube(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LightView.cpp
inline CLightDoc* CLightView::GetDocument()
   { return (CLightDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIGHTVIEW_H__9F8D41CD_2C7B_11D2_83B2_0080C832FCF3__INCLUDED_)
