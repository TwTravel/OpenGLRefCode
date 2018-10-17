/********************************************************************

	pics.h

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
	本文件用途：	ISee图像浏览器—SoftImage PIC图像读写模块定义文件
	
					读取功能：可读取24位、32位（带一个ALPHA通道）的
								压缩与未压缩 SoftImage PIC 图象
	  
					保存功能：可将24位图象以不压缩的方式保存为3通道
								SoftImage PIC 图象
								
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20505
	最后修改于：	2002-5-5
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-5		发布第一个版本（新版）
			

********************************************************************/



#ifndef __PICS_MODULE_INC__
#define __PICS_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PICS插件")
#define MODULE_FUNC_PREFIX				("pics_")
#define MODULE_FILE_POSTFIX				("pic")


/* 文件判别标志 */
#define PICS_MAGIC						0x5380F634
#define PICS_PICT_STR					0x54434950		/* 'PICT' 四个字符 */


#ifdef WIN32	/* Windows */

#	ifdef  PICS_EXPORTS
#	define PICS_API __declspec(dllexport)
#	else
#	define PICS_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PICS_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 文件头结构定义 */
typedef struct
{
	unsigned int	magic;           /* PIC图象标签             */
	float			version;         /* 使用的格式版本号        */
	char			comment[80];     /* 图象描述字串            */
	char			id[4];           /* "PICT"四个字符          */
	unsigned short	width;           /* 图象宽度，象素单位      */
	unsigned short	height;          /* 图象高度，象素单位      */
	float			ratio;           /* 图象的宽高比            */
	unsigned short	fields;          /* 图象标志集              */
	unsigned short	padding;         /* 保留                    */
} PICS_HEADER, *LPPICS_HEADER;


/* 通道信息结构 */
typedef struct
{
	unsigned char	chained;
	unsigned char	size;			/* 每通道位数 */
	unsigned char	type;			/* 压缩方式 */
	unsigned char	channel;		/* 包含哪些通道(R、G、B、A...) */
} PICS_CHANNEL_INFO, *LPPICS_CHANNEL_INFO;



/* 数据类型 */
#define PIC_UNSIGNED_INTEGER	0x00
#define PIC_SIGNED_INTEGER		0x10	/* 未实现 */
#define PIC_SIGNED_FLOAT		0x20	/* 未实现 */


/* 压缩方法 */
#define PIC_UNCOMPRESSED		0x00
#define PIC_PURE_RUN_LENGTH		0x01
#define PIC_MIXED_RUN_LENGTH	0x02

/* 通道类型 */
#define PIC_RED_CHANNEL			0x80
#define PIC_GREEN_CHANNEL		0x40
#define PIC_BLUE_CHANNEL		0x20
#define PIC_ALPHA_CHANNEL		0x10
#define PIC_SHADOW_CHANNEL		0x08	/* 未实现 */
#define PIC_DEPTH_CHANNEL		0x04	/* 未实现 */
#define PIC_AUXILIARY_1_CHANNEL	0x02	/* 未实现 */
#define PIC_AUXILIARY_2_CHANNEL	0x01	/* 未实现 */



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PICS_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PICS_API LPIRWP_INFO	CALLAGREEMENT pics_get_plugin_info(void);
extern PICS_API int			CALLAGREEMENT pics_init_plugin(void);
extern PICS_API int			CALLAGREEMENT pics_detach_plugin(void);
#endif	/* WIN32 */

extern PICS_API int	CALLAGREEMENT pics_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PICS_API int	CALLAGREEMENT pics_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PICS_API int	CALLAGREEMENT pics_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PICS_MODULE_INC__ */
