/********************************************************************

	sgi.h

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
	本文件用途：	ISee图像浏览器—SGI图像读写模块定义文件
	
					读取功能：8、24、32位SGI图象（包括压缩与未压缩）
							  
					保存功能：24位非压缩格式
							   
	  
	本文件编写人：	YZ			yzfree##yeah.net
		
	本文件版本：	20428
	最后修改于：	2002-4-28
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

			2002-4		发布第一个新版本

********************************************************************/



#ifndef __SGI_MODULE_INC__
#define __SGI_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("SGI插件")
#define MODULE_FUNC_PREFIX				("sgi_")
#define MODULE_FILE_POSTFIX				("sgi")

/* 文件判别标志 */
#define SGI_MAGIC						0x1da

#ifdef WIN32	/* Windows */

#	ifdef  SGI_EXPORTS
#	define SGI_API __declspec(dllexport)
#	else
#	define SGI_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define SGI_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


#define SGI_REV0_SIZE		4
#define SGI_REV1_SIZE		404
#define SGI_NAME_SIZE		80


/* 文件头结构定义 */
typedef struct _tag_sgi_header
{
	unsigned short	magic;					/* 文件标识 */
	unsigned char	storage;				/* 存储格式 */
	unsigned char	bpc;					/* 每个通道象素的字节数 */
	unsigned short	dim;					/* 维数 */
	unsigned short	width;					/* 宽度 */
	unsigned short	hight;					/* 高度 */
	unsigned short	channel;				/* 通道数 */
	long			pixmin;					/* 最小象素值 */
	long			pixmax;					/* 最大象素值 */
	char			rev0[SGI_REV0_SIZE];	/* 保留 */
	char			imgname[SGI_NAME_SIZE];	/* 图象描述串 */
	long			palid;					/* 调色板ID */
	char			rev1[SGI_REV1_SIZE];	/* 保留 */
} SGIHEADER, *LPSGIHEADER;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"						/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern SGI_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern SGI_API LPIRWP_INFO	CALLAGREEMENT sgi_get_plugin_info(void);
extern SGI_API int			CALLAGREEMENT sgi_init_plugin(void);
extern SGI_API int			CALLAGREEMENT sgi_detach_plugin(void);
#endif	/* WIN32 */

extern SGI_API int	CALLAGREEMENT sgi_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern SGI_API int	CALLAGREEMENT sgi_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern SGI_API int	CALLAGREEMENT sgi_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __SGI_MODULE_INC__ */
