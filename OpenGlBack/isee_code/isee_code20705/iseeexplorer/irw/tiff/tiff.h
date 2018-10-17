/********************************************************************

	tiff.h

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
	本文件用途：	ISee图像浏览器—TIFF图像读写模块定义文件
	
					读取功能：可读以 调色板、灰度、RGB、YCbCr 方式存储的
								TIFF图象（包括多页图象），但不支持位深度
								超过32位的图象。
	  
					保存功能：可将1、4、8、24位的单页或多页图象保存为对
								应的TIFF图象。
								
	  
	本文件编写人：	YZ				yzfree##yeah.net
					xuhappy			xuxuhappy##sina.com
					zjucypher		xubo.cy##263.net
		
	本文件版本：	20511
	最后修改于：	2002-5-11
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------

	引用代码编写人及版权信息：
	----------------------------------------------------------------
	代码类别：	TIFF编解代码库(LibTiff)		目录：libtiff
	
	Copyright (c) 1988-1997 Sam Leffler
	Copyright (c) 1991-1997 Silicon Graphics, Inc.

	Permission to use, copy, modify, distribute, and sell this software and 
	its documentation for any purpose is hereby granted without fee, provided
	that (i) the above copyright notices and this permission notice appear in
	all copies of the software and related documentation, and (ii) the names of
	Sam Leffler and Silicon Graphics may not be used in any advertising or
	publicity relating to the software without the specific, prior written
	permission of Sam Leffler and Silicon Graphics.

	THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
	EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
	WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  

	IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
	ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
	OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
	WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
	LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
	OF THIS SOFTWARE.
	----------------------------------------------------------------

	修正历史：

		2002-5		修正读取JPEG压缩方式的TIFF图象时R、B分量颠倒的问题。
					修正不能正确读取PhotoShop导出的TIFF图象的问题。
					leye发现，YZ修正。
		
		2002-4		发布新版代码。支持ISeeIO，及以JPEG方式存储的TIFF
					图象。保存功能支持1、4、8、24位深度。支持多页TIFF。

		2001-2		加入了保存功能
		2001-1		修正RGBA转换函数中存在的问题，并改进了步进控制代码
		2001-1		核心码改用libtiff库
		2000-7		第一个发布版


********************************************************************/



#ifndef __TIFF_MODULE_INC__
#define __TIFF_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("TIFF插件")
#define MODULE_FUNC_PREFIX				("tiff_")
#define MODULE_FILE_POSTFIX				("tif")

/* 文件判别标志 */


#ifdef WIN32	/* Windows */

#	ifdef  TIFF_EXPORTS
#	define TIFF_API __declspec(dllexport)
#	else
#	define TIFF_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define TIFF_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern TIFF_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern TIFF_API LPIRWP_INFO	CALLAGREEMENT tiff_get_plugin_info(void);
extern TIFF_API int			CALLAGREEMENT tiff_init_plugin(void);
extern TIFF_API int			CALLAGREEMENT tiff_detach_plugin(void);
#endif	/* WIN32 */

extern TIFF_API int	CALLAGREEMENT tiff_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern TIFF_API int	CALLAGREEMENT tiff_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern TIFF_API int	CALLAGREEMENT tiff_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __TIFF_MODULE_INC__ */
