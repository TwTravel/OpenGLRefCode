// MaterialPage.cpp : implementation file
//

#include "stdafx.h"
#include "matlight.h"
#include "MaterialPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMaterialPage property page

IMPLEMENT_DYNCREATE(CMaterialPage, CPropertyPage)

CMaterialPage::CMaterialPage() : CPropertyPage(CMaterialPage::IDD)
{
	//{{AFX_DATA_INIT(CMaterialPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMaterialPage::~CMaterialPage()
{
}

void CMaterialPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaterialPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMaterialPage, CPropertyPage)
	//{{AFX_MSG_MAP(CMaterialPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaterialPage message handlers

BOOL CMaterialPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
