/********************************************************************

	ce.h

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2001 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://isee.126.com
			http://iseeexplorer.cosoft.org.cn
			
	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee 图像转换引擎
	本文件编写人：	YZ			yzfree##sina.com

	本文件版本：	20416
	最后修改于：	2002-4-16

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
		地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-4		修正格式转换函数中颜色分量重叠的BUG，发现者是
					小组成员 tearshark。
		2001-12		小组成员 menghui 发现格式转换函数中缺少对多页
					图象的支持。（此BUG尚未修正）
		2001-7		第一个测试版发布


********************************************************************/



#ifndef __CE_INC__
#define __CE_INC__


#ifndef WIN32
#if defined(_WIN32) || defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */


#ifdef WIN32	/* Windows OS */

#	ifdef CE_EXPORTS
#	define CE_API __declspec(dllexport)
#	else
#	define CE_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux OS */

#	define CE_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"
#endif


#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif


CE_API void CALLAGREEMENT _1_to_cross(LPINFOSTR psou);
CE_API void CALLAGREEMENT _4_to_cross(LPINFOSTR psou);

CE_API int CALLAGREEMENT _v_conv(LPINFOSTR psou);

CE_API LPINFOSTR CALLAGREEMENT _23to4(LPINFOSTR psou);
CE_API LPINFOSTR CALLAGREEMENT _567to8(LPINFOSTR psou);

CE_API LPINFOSTR CALLAGREEMENT _9_10_11_12_13_14_15to16(LPINFOSTR psou);
CE_API LPINFOSTR CALLAGREEMENT _17_18_19_20_21_22_23to24(LPINFOSTR psou);
CE_API LPINFOSTR CALLAGREEMENT _25_26_27_28_29_30_31to32(LPINFOSTR psou);


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __CE_INC__ */
