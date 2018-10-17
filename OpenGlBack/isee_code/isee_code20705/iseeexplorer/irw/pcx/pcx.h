/********************************************************************

	pcx.h

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
	本文件用途：	ISee图像浏览器—PCX图像读写模块定义文件
	
					读取功能：1、4、8、24位图像
					保存功能：1、4、8、24位图像
	  
	本文件编写人：	freedvlp	feelfree##263.net（读代码编写人）
					ChK			ChK##163.net（写代码编写人）
					YZ			yzfree##sina.com（改版工作）
		
	本文件版本：	10818
	最后修改于：	2001-8-18
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-8		发布新版代码。并扩展了该模块的保存功能，
					增强了模块容错性，提高了模块的读写速度。
			  
		2001-4		完成了图像的保存功能（24位）
		2000-8		完成了读取图像部分的全部代码
		2000-6		第一个测试版发布


********************************************************************/



#ifndef __PCX_MODULE_INC__
#define __PCX_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PCX插件")
#define MODULE_FUNC_PREFIX				("pcx_")
#define MODULE_FILE_POSTFIX				("PCX")

/* 文件判别标志 */
#define PCX_FLAG						0xa


#ifdef WIN32	/* Windows */

#	ifdef  PCX_EXPORTS
#	define PCX_API __declspec(dllexport)
#	else
#	define PCX_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PCX_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* PCX调色板项结构 */
typedef struct _tag_pcx_palette_item
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} PCX_PALITEM, *LPPCX_PALITEM;


/* PCX信息结构 */
typedef struct _tag_pcx_header
{
	unsigned char  byManufacturer;
	unsigned char  byVersion;
	unsigned char  byEncoding;
	unsigned char  byBits;
	unsigned short wLeft;
	unsigned short wTop;
	unsigned short wRight;
	unsigned short wBottom;
	unsigned short wXResolution;
	unsigned short wYResolution;
	PCX_PALITEM    ptPalette[16];
	unsigned char  byReserved;
	unsigned char  byPlanes;
	unsigned short wLineBytes;
	unsigned short wPaletteType;
	unsigned short wScrWidth;
	unsigned short wScrDepth;
	unsigned char  byFiller[54];
}  PCX_HEADER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PCX_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PCX_API LPIRWP_INFO	CALLAGREEMENT pcx_get_plugin_info(void);
extern PCX_API int			CALLAGREEMENT pcx_init_plugin(void);
extern PCX_API int			CALLAGREEMENT pcx_detach_plugin(void);
#endif	/* WIN32 */

extern PCX_API int	CALLAGREEMENT pcx_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PCX_API int	CALLAGREEMENT pcx_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PCX_API int	CALLAGREEMENT pcx_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PCX_MODULE_INC__ */
