// TetrisView.h : interface of the CTetrisView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYSDOPENGLVIEW_H__75C5AAEC_37B0_4A8B_9132_9A0C663F6DDC__INCLUDED_)
#define AFX_MYSDOPENGLVIEW_H__75C5AAEC_37B0_4A8B_9132_9A0C663F6DDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FontGL.h"

class CTetrisView : public CView
{
protected: // create from serialization only
	CTetrisView();
	DECLARE_DYNCREATE(CTetrisView)

// Attributes
public:
	CTetrisDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTetrisView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTetrisView();
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
	void DrawGameIntro();
	int DrawGameRun(GLvoid);
	void DrawGameWin();
	void DrawGameFinish();
	void InitGame();
	void NewBlock();
	void CreateTexture(UINT textureArray[], LPSTR strFileName, int textureID);
	bool CheckWin();
	bool MoveDown();
	bool MoveLeft();
	bool MoveRight();
	bool Rotate();

	CFontGL m_font;
	GLuint	texture[7];	
	bool  keys[256];

	bool  gameintro;
	bool  gamerun;
	bool  gamewin;
	bool  gamefinish;
	bool  onetime;
	bool  test;
	bool  running;
	bool  lineformed;
	
	int xpos;
	int ypos;
	int map[10][20];
	int nextblockmap[4][5];
	int type;
	int state;
	int newstate;
	int oldstate;
	int linemultiplier;
	int lines;
	int score;
	int speed;

	short level;
	short linesformed;
	short startlines;

struct ActiveBlocks
{
	int x[4];
	int y[4];
}*block, nextblock[4],currentblock[4];

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTetrisView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnGameNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TetrisView.cpp
inline CTetrisDoc* CTetrisView::GetDocument()
   { return (CTetrisDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYSDOPENGLVIEW_H__75C5AAEC_37B0_4A8B_9132_9A0C663F6DDC__INCLUDED_)
