// ToDibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ToDib.h"
#include "ToDibDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDibDlg dialog


CToDibDlg::CToDibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToDibDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToDibDlg)
	m_leftX = 0;
	m_height = 0;
	m_width = 0;
	m_bottomY = 0;
	//}}AFX_DATA_INIT
}


void CToDibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToDibDlg)
	DDX_Text(pDX, IDC_LEFTX, m_leftX);
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	DDX_Text(pDX, IDC_BOTTONY, m_bottomY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToDibDlg, CDialog)
	//{{AFX_MSG_MAP(CToDibDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDibDlg message handlers





