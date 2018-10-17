/********************************************************************

	AnieffConfigDlg.cpp - ISee图像浏览器—图像处理模块类AnieffConfigDlg
	实现文件

    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用许可证
	条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根据你
	的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用许可证。

    你应该已经和程序一起收到一份GNU通用许可证(GPL)的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	作者：楼琇林(leye)
   e-mail:louxiulin@263.net

   功能实现：图像处理动态效果参数设置对话框

	文件版本：
		Build 010317
		Date  2001-03-17

********************************************************************/
// AnieffConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "flt_anieff.h"
#include "AnieffConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAnieffConfigDlg dialog


CAnieffConfigDlg::CAnieffConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAnieffConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAnieffConfigDlg)
	m_bLineUp = FALSE;
	m_bAlphaScroll = FALSE;
	m_bBurnUp = FALSE;
	m_bFadeIn = FALSE;
	m_strDelayTime = _T("毫秒(5秒)");
	m_nMilliSec = 5000;
	m_nFrames = 0;
	//}}AFX_DATA_INIT
}


void CAnieffConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAnieffConfigDlg)
	DDX_Control(pDX, IDC_DELAY, m_sldDelay);
	DDX_Check(pDX, IDC_LINEUP, m_bLineUp);
	DDX_Check(pDX, IDC_ALPHASCROLL, m_bAlphaScroll);
	DDX_Check(pDX, IDC_BURNUP, m_bBurnUp);
	DDX_Check(pDX, IDC_FADEIN, m_bFadeIn);
	DDX_Text(pDX, IDC_DELAYTIME, m_strDelayTime);
	DDX_Text(pDX, IDC_MILLISEC, m_nMilliSec);
	DDX_Text(pDX, IDC_FRAMES, m_nFrames);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAnieffConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CAnieffConfigDlg)
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_MILLISEC, OnChangeMillisec)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAnieffConfigDlg message handlers


BOOL CAnieffConfigDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CFlt_anieffApp* pApp = (CFlt_anieffApp*)AfxGetApp();
	m_bAlphaScroll=pApp->GetProfileInt("PCM_ANIEFF","PCM_ALPHASCROLL",false);
	m_bBurnUp=pApp->GetProfileInt("PCM_ANIEFF","PCM_BURNUP",false);
	m_bFadeIn=pApp->GetProfileInt("PCM_ANIEFF","PCM_FADEIN",false);
	m_bLineUp=pApp->GetProfileInt("PCM_ANIEFF","PCM_lINEUP",false);

	m_nMilliSec=pApp->GetProfileInt("PCM_ANIEFF","DELAYTIME",1000);
	m_nFrames=pApp->GetProfileInt("PCM_ANIEFF","FRAMES",24);
	m_sldDelay.SetRange(0,100,TRUE);
	m_sldDelay.SetTicFreq(10);
	m_sldDelay.SetLineSize(5);
	m_sldDelay.SetPageSize(10);
	m_sldDelay.SetPos(m_nMilliSec/100);
	m_strDelayTime.Format("毫秒(%2.1f秒)",m_nMilliSec/1000.0);
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAnieffConfigDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	UpdateData(TRUE);
	// TODO: Add your message handler code here and/or call default
	CSliderCtrl* pSlider=(CSliderCtrl*)pScrollBar;
	m_nMilliSec=pSlider->GetPos()*100;
	m_strDelayTime.Format("毫秒(%2.1f秒)",m_nMilliSec/1000.0);
	UpdateData(FALSE);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAnieffConfigDlg::OnChangeMillisec() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_sldDelay.SetPos(m_nMilliSec/100);
	m_strDelayTime.Format("毫秒(%2.1f秒)",m_nMilliSec/1000.0);
	UpdateData(FALSE);
}
