// Editor.cpp : implementation file
//

#include "stdafx.h"
#include "matlight.h"
#include "Editor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditor property page

IMPLEMENT_DYNCREATE(CEditor, CPropertyPage)

// Default constructor, does nothing
CEditor::CEditor()
	{
	fValues[0] = 0.0f;
	fValues[1] = 0.0f;
	fValues[2] = 0.0f;
	fValues[3] = 0.0f;
	}


CEditor::CEditor(UINT id, UINT idCaption) : CPropertyPage(id, idCaption)
	{
	//{{AFX_DATA_INIT(CEditor)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	fValues[0] = 0.0f;
	fValues[1] = 0.0f;
	fValues[2] = 0.0f;
	fValues[3] = 0.0f;
	}

CEditor::~CEditor()
{
}

void CEditor::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditor)
	DDX_Control(pDX, IDC_SLD_RED, m_SlideRed);
	DDX_Control(pDX, IDC_SLD_GREEN, m_SlideGreen);
	DDX_Control(pDX, IDC_SLD_BLUE, m_SlideBlue);
	DDX_Control(pDX, IDC_EDT_RED, m_Red);
	DDX_Control(pDX, IDC_EDT_GREEN, m_Green);
	DDX_Control(pDX, IDC_EDT_BLUE, m_Blue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditor, CPropertyPage)
	//{{AFX_MSG_MAP(CEditor)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDT_BLUE, OnChangeEdtBlue)
	ON_EN_CHANGE(IDC_EDT_GREEN, OnChangeEdtGreen)
	ON_EN_CHANGE(IDC_EDT_RED, OnChangeEdtRed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditor message handlers


BOOL CEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	
	return bRet;
}

int CEditor::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertyPage::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}


void CEditor::SetValues(float fVals[4])
	{
	fValues[0] = fVals[0];
	fValues[1] = fVals[1];
	fValues[2] = fVals[2];
	fValues[3] = fVals[3];
	}


void CEditor::GetValues(float fVals[4])
	{
	fVals[0] = fValues[0];
	fVals[1] = fValues[1];
	fVals[2] = fValues[2];
	fVals[3] = fValues[3];
	}



BOOL CEditor::OnInitDialog() 
	{
	char cBuff[16];
	CPropertyPage::OnInitDialog();
	
	m_SlideRed.SetRange(0, 1000, TRUE );
	m_SlideRed.SetPos((int)(fValues[0]*1000.0f));
	m_SlideRed.SetTicFreq(100);
	m_SlideRed.SetPageSize(100);
	sprintf(cBuff,"%.2f",fValues[0]);
	SetDlgItemText(IDC_EDT_RED,cBuff);
	

	m_SlideGreen.SetRange(0, 1000, TRUE );
	m_SlideGreen.SetPos((int)(fValues[1]*1000.0f));
	m_SlideGreen.SetTicFreq(100);
	m_SlideGreen.SetPageSize(100);
	sprintf(cBuff,"%.2f",fValues[1]);
	SetDlgItemText(IDC_EDT_GREEN,cBuff);

	m_SlideBlue.SetRange(0, 1000, TRUE );
	m_SlideBlue.SetPos((int)(fValues[2]*1000.0f));
	m_SlideBlue.SetTicFreq(100);
	m_SlideBlue.SetPageSize(100);
	sprintf(cBuff,"%.2f",fValues[2]);
	SetDlgItemText(IDC_EDT_BLUE,cBuff);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

void CEditor::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
	float fValue;
	char cBuff[16];

	fValue = (float)(((CSliderCtrl *)pScrollBar)->GetPos()) / 1000.0f;

	sprintf(cBuff,"%.2f",fValue);

	if(nSBCode == SB_ENDSCROLL)
		return;

	// Red Value
	if((CSliderCtrl *)pScrollBar == &m_SlideRed)
		{
		fValues[0] = fValue;
		SetDlgItemText(IDC_EDT_RED,cBuff);
		}

	// Green Value
	if((CSliderCtrl *)pScrollBar == &m_SlideGreen)
		{
		fValues[1] = fValue;
		SetDlgItemText(IDC_EDT_GREEN,cBuff);
		}

	// Blue Value
	if((CSliderCtrl *)pScrollBar == &m_SlideBlue)
		{
		fValues[2] = fValue;
		SetDlgItemText(IDC_EDT_BLUE,cBuff);
		}

	
	CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
	}


void CEditor::OnChangeEdtBlue() 
	{
	SetModified(TRUE);
	}

void CEditor::OnChangeEdtGreen() 
	{
	SetModified(TRUE);
	}

void CEditor::OnChangeEdtRed() 
	{
	SetModified(TRUE);
	}


void CEditor::OnOK()
	{
	m_pParent->PostMessage(WM_UPDATE,0,0);
	}