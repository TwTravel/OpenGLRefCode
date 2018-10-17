/********************************************************************

	flt_anieff.h - ISee图像浏览器—图像处理模块类flt_anieff头文件

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

   功能实现：对外接口函数定义

	文件版本：
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

// 接口函数声明 — 第一层，唯一与外界联系的接口
int WINAPI AccessAniEffFilter(LPIMAGEPROCSTR lpInfo);

//  调用处理函数 — 第二层函数
//  ALPHA卷轴——下一张图片从上下左右四个方向的某个方向卷轴进来
int _fnCMD_ALPHASCROLL(LPIMAGEPROCSTR lpInfo);
//  火焰吞噬——前图片从下到上被火焰吞噬，显出下一张图
int _fnCMD_BURNUP(LPIMAGEPROCSTR lpInfo);
//  叠映——淡入淡出效果，当前图片淡出，下一张图片淡入片
int _fnCMD_FADEIN(LPIMAGEPROCSTR lpInfo);
//  随机直线填充——前图片被随机线填充，显出下一张图片
int _fnCMD_LINEUP(LPIMAGEPROCSTR lpInfo);

//////////辅助函数
//	延时函数——作者：马翔
//  e-mail:hugesoft@yeah.net
void Delay(DWORD dwDelayTime);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_flt_anieff_H__5BCD2105_1B19_11D5_8D9A_0000E839960D__INCLUDED_)
