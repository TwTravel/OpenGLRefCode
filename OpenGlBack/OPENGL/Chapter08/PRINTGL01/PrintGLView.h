// AgViewerGLView.h : interface of the CPrintGLView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGVIEWERGLVIEW_H__F3017F4F_3A97_11D2_9506_F6C490433B31__INCLUDED_)
#define AFX_AGVIEWERGLVIEW_H__F3017F4F_3A97_11D2_9506_F6C490433B31__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ClientCapture.h"
#include "PrintGLDoc.h"

class CPrintGLView : public CView
{
protected: // create from serialization only
	CPrintGLView();
	DECLARE_DYNCREATE(CPrintGLView)

// Attributes
public:
	CPrintGLDoc* GetDocument();

// Operations
public:
	CDC* GetCDC() {return m_pDC;}
	void Init();
	void CreateRGBPalette(void);
    BOOL bSetupPixelFormat(void);
    unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
/////////////////////////////////////////////////////////////////
//添加成员函数与成员变量
	BOOL RenderScene();
	BOOL SetupPixelFormat(void);
	void SetLogicalPalette(void);
	BOOL InitializeOpenGL(CDC* pDC);

	HGLRC		m_hRC;			//OpenGL绘制描述表
	HPALETTE	m_hPalette;		//OpenGL调色板
	CDC*	    m_pDC;			//OpenGL设备描述表
/////////////////////////////////////////////////////////////////
	GLfloat step,s;
	void DrawColorBox(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintGLView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrintPreview(CDC* pDC, CPrintInfo* pInfo, POINT point, CPreviewView* pView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrintGLView();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	BOOL m_bTimerEnabled;
	UINT m_uiTimerID;
	//{{AFX_MSG(CPrintGLView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnFilePrintPreview();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
//	BOOL m_PrintPreview;
	CClientCapture CapturedImage;

};



#ifndef _DEBUG  // debug version in AgViewerGLView.cpp
inline CPrintGLDoc* CPrintGLView::GetDocument()
   { return (CPrintGLDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AGVIEWERGLVIEW_H__F3017F4F_3A97_11D2_9506_F6C490433B31__INCLUDED_)
