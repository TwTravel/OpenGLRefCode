// TextureView.h : interface of the CTextureView class
//
/////////////////////////////////////////////////////////////////////////////

#include "gl\gl.h"
#include "gl\glu.h"
#include "dib.h"

enum{Texture,DibTexture,AutoTexture,Mipmap};

#if !defined(AFX_TEXTUREVIEW_H__38EF946D_4632_11D2_83B2_0080C832FCF3__INCLUDED_)
#define AFX_TEXTUREVIEW_H__38EF946D_4632_11D2_83B2_0080C832FCF3__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTextureView : public CView
{
protected: // create from serialization only
	CTextureView();
	DECLARE_DYNCREATE(CTextureView)

// Attributes
public:
	CTextureDoc* GetDocument();

	CClientDC	*m_pDC;
	CRect		m_oldRect;
		
	CDib* m_pDib;
	CDib* m_pTDib;

	CPalette    m_cPalette;
    CPalette    *m_pOldPalette;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextureView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	void CreateRGBPalette();
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	virtual ~CTextureView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	GLint  m_typeControl;

	GLint m_iWindowWidth;
	GLint m_iWindowHeight;

// Generated message map functions
protected:
	void Mipmapping();
	void TexGenerate();
	void BitmapTexture();
	void TexturePlane();
	void DrawScene();
	BOOL bSetupPixelFormat();
	void Init();
	//{{AFX_MSG(CTextureView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTexture();
	afx_msg void OnUpdateTexture(CCmdUI* pCmdUI);
	afx_msg void OnDibTexture();
	afx_msg void OnUpdateDibTexture(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnAutoTexture();
	afx_msg void OnUpdateAutoTexture(CCmdUI* pCmdUI);
	afx_msg void OnMipmapping();
	afx_msg void OnUpdateMipmapping(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TextureView.cpp
inline CTextureDoc* CTextureView::GetDocument()
   { return (CTextureDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTUREVIEW_H__38EF946D_4632_11D2_83B2_0080C832FCF3__INCLUDED_)
