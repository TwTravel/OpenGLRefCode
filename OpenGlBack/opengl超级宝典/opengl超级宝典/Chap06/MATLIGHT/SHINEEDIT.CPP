// ShineEdit.cpp : implementation file
//

#include "stdafx.h"
#include "matlight.h"
#include "ShineEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShineEdit property page

IMPLEMENT_DYNCREATE(CShineEdit, CPropertyPage)

CShineEdit::CShineEdit() : CPropertyPage(CShineEdit::IDD)
	{
	//{{AFX_DATA_INIT(CShineEdit)
	m_ShineValue = 0;
	//}}AFX_DATA_INIT
	}

CShineEdit::~CShineEdit()
{
}

void CShineEdit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShineEdit)
	DDX_Control(pDX, IDC_SPN_SHINE, m_Spinner);
	DDX_Text(pDX, IDC_EDT_SHINE, m_ShineValue);
	DDV_MinMaxInt(pDX, m_ShineValue, 0, 128);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CShineEdit, CPropertyPage)
	//{{AFX_MSG_MAP(CShineEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_SHINE, OnDeltaposSpnShine)
	ON_EN_CHANGE(IDC_EDT_SHINE, OnChangeEdtShine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShineEdit message handlers

BOOL CShineEdit::OnInitDialog() 
	{
	char cBuff[16];
	CPropertyPage::OnInitDialog();
	
	m_Spinner.SetRange(0,128);

	sprintf(cBuff,"%d",m_ShineValue);
	SetDlgItemText(IDC_EDT_SHINE,cBuff);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CShineEdit::OnDeltaposSpnShine(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	*pResult = 0;
	}


int CShineEdit::GetValue(void)
	{
	return m_ShineValue;
	}


void CShineEdit::SetValue(int nValue)
	{
	m_ShineValue = nValue;
	}

void CShineEdit::OnChangeEdtShine() 
	{
	SetModified(TRUE);
	}

void CShineEdit::OnOK()
	{
	m_pParent->PostMessage(WM_UPDATE,0,0);
	}