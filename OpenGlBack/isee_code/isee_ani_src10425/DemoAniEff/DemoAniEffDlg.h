/********************************************************************

	DemoAniEffDlg.h - ISeeͼ���������ͼ����ģ�������DemoAniEffDlg
	ͷ�ļ�

    ��Ȩ����(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    ��һ����������������������������������������GNU ͨ�����֤
	�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ�������
	��ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�����֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�����֤(GPL)�ĸ����������û�У�
	д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	���ߣ�¥�L��(leye)
   e-mail:louxiulin@263.net

   ����ʵ�֣�ͼ����̬Ч�����ԶԻ���

	�ļ��汾��
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
	INFOSTR m_sInfo[2];//ͼ��I/O���ݰ�
	LPINFOSTR m_lpInfo;
	int m_nCur;

	IMAGEPROCSTR m_sIPS;//ͼ�������ݰ�
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
