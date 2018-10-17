// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "test.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_INFO_DISP, OnInfoDisp)
	ON_MESSAGE(WM_FILENUM_DISP, OnFilenumDisp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_PLUGIN,
	ID_INDICATOR_INFO,
	ID_INDICATOR_REV,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneInfo(1, indicators[1], 0, 60);
	m_wndStatusBar.SetPaneInfo(2, indicators[2], 0, 100);
	m_wndStatusBar.SetPaneInfo(3, indicators[3], 0, 40);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

afx_msg LRESULT CMainFrame::OnInfoDisp(WPARAM wParam, LPARAM lParam)
{
	LPINFOSTR	pinfo = (LPINFOSTR)lParam;
	char		pbuf[128];

	strcpy(pbuf, (const char*)wParam);

	m_wndStatusBar.SetPaneText(1, pbuf);
	
	sprintf(pbuf, "%d¡Á%d¡Á%d", pinfo->width, pinfo->height, pinfo->bitcount);
	
	m_wndStatusBar.SetPaneText(2, pbuf);
	
	return 0;
}

afx_msg LRESULT CMainFrame::OnFilenumDisp(WPARAM wParam, LPARAM lParam)
{
	char		pbuf[128];
	
	sprintf(pbuf, "%d/%d", wParam, lParam);
	
	m_wndStatusBar.SetPaneText(3, pbuf);

	return 0;
}