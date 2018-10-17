/********************************************************************

	pix.h

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2002 VCHelp coPathway ISee workgroup.
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
			http://cosoft.org.cn/projects/iseeexplorer

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—Alias PIX图像读写模块定义文件
	
					读取功能：可读取8、24位RLE压缩Alias PIX图象
							  
					保存功能：24位RLE压缩格式
							   
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	020507
	最后修改于：	2002-5-7
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-5		第一个发布版（新版）
			

********************************************************************/



#ifndef __PIX_MODULE_INC__
#define __PIX_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PIX插件")
#define MODULE_FUNC_PREFIX				("pix_")
#define MODULE_FILE_POSTFIX				("pix")

/* 文件判别标志 */


#ifdef WIN32	/* Windows */

#	ifdef  PIX_EXPORTS
#	define PIX_API __declspec(dllexport)
#	else
#	define PIX_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PIX_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 流头结构定义 */
typedef struct
{
	unsigned short	width;			/* 图象宽、高度（象素单位）*/
	unsigned short	height;
	unsigned short	rev0;			/* 已废弃的两个字，其值无意义 */
	unsigned short	rev1;
	unsigned short	bitcount;		/* 图象位深度（只能是24或8）*/
} PIX_HEADER, *LPPIX_HEADER;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PIX_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PIX_API LPIRWP_INFO	CALLAGREEMENT pix_get_plugin_info(void);
extern PIX_API int			CALLAGREEMENT pix_init_plugin(void);
extern PIX_API int			CALLAGREEMENT pix_detach_plugin(void);
#endif	/* WIN32 */

extern PIX_API int	CALLAGREEMENT pix_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PIX_API int	CALLAGREEMENT pix_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PIX_API int	CALLAGREEMENT pix_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PIX_MODULE_INC__ */
