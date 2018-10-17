/********************************************************************

	DemoAniEffDlg.h - ISee图像浏览器—图像处理模块测试类DemoAniEffDlg
	头文件

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

   功能实现：图像处理动态效果测试对话框

	文件版本：
		Build 010317
		Date  2001-03-17

********************************************************************/
// DemoAniEffDlg.h : header file
//

#if !defined(AFX_DEMOANIEFFDLG_H__9FA4ADE6_2360_11D5_8D9A_0000E839960D__INCLUDED_)
#define AFX_DEMOANIEFFDLG_H__9FA4ADE6_2360_11D5_8D9A_0000E839960D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDemoAniEffDlg dialog

class CDemoAniEffDlg : public CDialog
{
// Construction
public:
	HDRAWDIB hdd;
	INFOSTR m_sInfo[2];//图像I/O数据包
	LPINFOSTR m_lpInfo;
	int m_nCur;

	IMAGEPROCSTR m_sIPS;//图像处理数据包
	CDemoAniEffDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CDemoAniEffDlg();	

// Dialog Data
	//{{AFX_DATA(CDemoAniEffDlg)
	enum { IDD = IDD_DEMOANIEFF_DIALOG };
	CButton	m_ctlTest;
	CStatic	m_ctlPreview;
	int		m_nTestItem;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDemoAniEffDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDemoAniEffDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnFile();
	afx_msg void OnSetup();
	afx_msg void OnTest();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEMOANIEFFDLG_H__9FA4ADE6_2360_11D5_8D9A_0000E839960D__INCLUDED_)
