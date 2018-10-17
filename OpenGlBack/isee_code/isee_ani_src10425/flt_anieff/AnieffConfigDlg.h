/********************************************************************

	AnieffConfigDlg.h - ISeeͼ���������ͼ����ģ����AnieffConfigDlg
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

   ����ʵ�֣�ͼ����̬Ч���������öԻ���

	�ļ��汾��
		Build 010317
		Date  2001-03-17

********************************************************************/
#if !defined(AFX_ANIEFFCONFIGDLG_H__B1FD3F85_2369_11D5_8D9A_0000E839960D__INCLUDED_)
#define AFX_ANIEFFCONFIGDLG_H__B1FD3F85_2369_11D5_8D9A_0000E839960D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AnieffConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAnieffConfigDlg dialog

class CAnieffConfigDlg : public CDialog
{
// Construction
public:
	CAnieffConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAnieffConfigDlg)
	enum { IDD = IDD_ANIEFF_CONFIG };
	CSliderCtrl	m_sldDelay;
	BOOL	m_bLineUp;
	BOOL	m_bAlphaScroll;
	BOOL	m_bBurnUp;
	BOOL	m_bFadeIn;
	CString	m_strDelayTime;
	UINT	m_nMilliSec;
	UINT	m_nFrames;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAnieffConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAnieffConfigDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeMillisec();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ANIEFFCONFIGDLG_H__B1FD3F85_2369_11D5_8D9A_0000E839960D__INCLUDED_)
