/********************************************************************

	flt_anieff.h - ISeeͼ���������ͼ����ģ����flt_anieffͷ�ļ�

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

   ����ʵ�֣�����ӿں�������

	�ļ��汾��
		Build 010317
		Date  2001-03-17

********************************************************************/
//// flt_anieff.h : main header file for the flt_anieff DLL
//

#if !defined(AFX_flt_anieff_H__5BCD2105_1B19_11D5_8D9A_0000E839960D__INCLUDED_)
#define AFX_flt_anieff_H__5BCD2105_1B19_11D5_8D9A_0000E839960D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFlt_anieffApp
// See flt_anieff.cpp for the implementation of this class
//

class CFlt_anieffApp : public CWinApp
{
public:
	CFlt_anieffApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFlt_anieffApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFlt_anieffApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////

// �ӿں������� �� ��һ�㣬Ψһ�������ϵ�Ľӿ�
int WINAPI AccessAniEffFilter(LPIMAGEPROCSTR lpInfo);

//  ���ô����� �� �ڶ��㺯��
//  ALPHA���ᡪ����һ��ͼƬ�����������ĸ������ĳ������������
int _fnCMD_ALPHASCROLL(LPIMAGEPROCSTR lpInfo);
//  �������ɡ���ǰͼƬ���µ��ϱ��������ɣ��Գ���һ��ͼ
int _fnCMD_BURNUP(LPIMAGEPROCSTR lpInfo);
//  ��ӳ�������뵭��Ч������ǰͼƬ��������һ��ͼƬ����Ƭ
int _fnCMD_FADEIN(LPIMAGEPROCSTR lpInfo);
//  ���ֱ����䡪��ǰͼƬ���������䣬�Գ���һ��ͼƬ
int _fnCMD_LINEUP(LPIMAGEPROCSTR lpInfo);

//////////��������
//	��ʱ�����������ߣ�����
//  e-mail:hugesoft@yeah.net
void Delay(DWORD dwDelayTime);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_flt_anieff_H__5BCD2105_1B19_11D5_8D9A_0000E839960D__INCLUDED_)
