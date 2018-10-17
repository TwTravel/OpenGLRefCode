// matlightView.h : interface of the CMatlightView class
//
/////////////////////////////////////////////////////////////////////////////

class CMatlightView : public CView
{
protected: // create from serialization only
	CMatlightView();
	DECLARE_DYNCREATE(CMatlightView)

// Attributes
public:
	CMatlightDoc* GetDocument();
	BOOL m_bPrinting;

	CPalette m_GLPalette;	// Logical Palette
	HDC m_hDC;

// OpenGL Attributes
	HGLRC m_hRC;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMatlightView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL DestroyWindow();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMatlightView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void PrintSettings(CDC* pDC);
	void InitializePalette(HDC hDC);

// Generated message map functions
protected:
	//{{AFX_MSG(CMatlightView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in matlightView.cpp
inline CMatlightDoc* CMatlightView::GetDocument()
   { return (CMatlightDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
