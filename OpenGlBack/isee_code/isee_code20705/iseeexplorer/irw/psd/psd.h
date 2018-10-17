/********************************************************************

	psd.h

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
	本文件用途：	ISee图像浏览器—PSD图像读写模块定义文件
	
					读取功能：
								BITMAP色,1位,RLE压缩与无压缩; 
								RGB色,8位,RLE压缩与无压缩; 
								灰度,8位,RLE压缩与无压缩; 
								Duotone, 8位,RLE压缩与无压缩;
								CMYK色,8位,RLE压缩与无压缩; 
								Lab色,8位,RLE压缩与无压缩;
								* 不支持16位/通道，不支持多通道图象

					保存功能：不支持
	  
	本文件编写人：	Janhail		janhail##sina.com
					YZ			yzfree##yeah.net
		
	本文件版本：	20423
	最后修改于：	2002-4-23
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-4		发布新版代码。支持ISeeIO，改变读取方法减少内存占用
					量，增加对单色图RLE压缩法支持，改进CMYK->RGB算法。
					去除对16位/通道图象的支持，去除对多通道图象的支持。

		2001-1		修正了一些BUG
		2000-10		第一个测试版发布

********************************************************************/



#ifndef __PSD_MODULE_INC__
#define __PSD_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PSD插件")
#define MODULE_FUNC_PREFIX				("psd_")
#define MODULE_FILE_POSTFIX				("PSD")

/* 文件判别标志 */
#define PSD_SIGNATURE_MARKER			0x53504238	/* "8BPS" */
#define PSD_VERSION_MARKER				0x100		/* 256 ，这两个标志都为MOTO字序。*/


#ifdef WIN32	/* Windows */

#	ifdef  PSD_EXPORTS
#	define PSD_API			__declspec(dllexport)
#	else
#	define PSD_API			__declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PSD_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 文件信息头结构 */
typedef struct
{
	unsigned long		Signature;	/* 标签："8BPS" */
	unsigned short		Version;	/* 版本号。总是等于1 */
	unsigned short		Reserved0;
	unsigned short		Reserved1;	/* 保留6个字节 */
	unsigned short		Reserved2;
	unsigned short		Channels;	/* 通道数(范围：1～24) */
	unsigned long		Rows;		/* 行数	(范围: 1 ～ 30000) */
	unsigned long		Columns;	/* 列数 (范围: 1 ～ 30000) */
	unsigned short		Depth;		/* 每通道的位数(可能的值是: 1, 8, 16) */
	unsigned short		Mode;		/* 色彩模式，其值可能是下面的一种:*/
									/*  Bitmap		=0
										Grayscale	=1 
										Indexed		=2 
										RGB			=3 
										CMYK		=4 
										Multichannel=7 
										Duotone		=8 
										Lab			=9 */
}PSDFILEHEADER, *LPPSDFILEHEADER;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif


#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PSD_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PSD_API LPIRWP_INFO CALLAGREEMENT psd_get_plugin_info(void);
extern PSD_API int			CALLAGREEMENT psd_init_plugin(void);
extern PSD_API int			CALLAGREEMENT psd_detach_plugin(void);
#endif	/* WIN32 */

extern PSD_API int CALLAGREEMENT psd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PSD_API	int	CALLAGREEMENT psd_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PSD_API int	CALLAGREEMENT psd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PSD_MODULE_INC__ */
