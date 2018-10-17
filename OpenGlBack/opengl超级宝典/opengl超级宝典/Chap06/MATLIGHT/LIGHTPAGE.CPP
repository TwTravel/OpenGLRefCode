// LightPage.cpp : implementation file
//

#include "stdafx.h"
#include "matlight.h"
#include "LightPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLightPage property page

IMPLEMENT_DYNCREATE(CLightPage, CPropertyPage)

CLightPage::CLightPage() : CPropertyPage(CLightPage::IDD)
{
	//{{AFX_DATA_INIT(CLightPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CLightPage::~CLightPage()
{
}

void CLightPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLightPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLightPage, CPropertyPage)
	//{{AFX_MSG_MAP(CLightPage)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLightPage message handlers
