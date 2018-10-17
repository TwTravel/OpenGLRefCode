/********************************************************************

	iseecompress.h

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
			http://www.vchelp.net
			http://www.chinafcu.com

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—压缩及解压模块(X-Todo A03)
	
	本文件编写人：	wenbobo		wenbobo##263.net
		
	本文件版本：	11207
	最后修改于：	2001-12-07
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2001-12		第一次发布


	----------------------------------------------------------------
	注：本代码使用了UCL(实时数据压缩库)，在此向UCL的开发者们表示感谢！
	----------------------------------------------------------------
  
********************************************************************/


#ifdef WIN32	/* Windows */

#include <windows.h>

#	define CALLAGREEMENT	__cdecl
#	ifndef _DEUBG
#	ifndef NDEBUG
#	define NDEBUG
#	endif
#	endif

#else			/* Linux */

typedef unsigned char * LPBYTE;
typedef unsigned long   DWORD;

#	define CALLAGREEMENT
#	ifndef NDEBUG
#	ifndef _DEBUG
#	define _DEUBG
#	endif
#	endif

#endif	/*WIN32*/


extern int isc_compresslevel;


/* CI */
int CALLAGREEMENT ISeeCompress(LPBYTE, DWORD, LPBYTE *, DWORD *);
int CALLAGREEMENT ISeeDecompress(LPBYTE, DWORD, LPBYTE *, DWORD);

