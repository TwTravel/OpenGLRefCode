/********************************************************************

	rle.h

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
	本文件用途：	ISee图像浏览器—Utah RLE图像读写模块定义文件
	
					读取功能：可读取8位、24位、32位RLE图象（最多
								允许三个色彩通道，一个ALPHA通道）
					保存功能：可将8位、24位图象保存为RLE图象（不
								压缩形式）
	  
	本文件编写人：	
					YZ			yzfree##yeah.net
	
	本文件版本：	20522
	最后修改于：	2002-5-22
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2002-5		第一个发布版（新版）

********************************************************************/



#ifndef __RLE_MODULE_INC__
#define __RLE_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("RLE插件")
#define MODULE_FUNC_PREFIX				("rle_")
#define MODULE_FILE_POSTFIX				("rle")

/* 文件判别标志 */
#define RLE_MAGIC						((unsigned short)0xcc52)


#ifdef WIN32	/* Windows */

#	ifdef  RLE_EXPORTS
#	define RLE_API __declspec(dllexport)
#	else
#	define RLE_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define RLE_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 长操作数判别位 */
#define     LONGOPCODE			0x40

/* RLE流操作码定义 */
#define	    RLE_SKIPLINE_OP		1
#define	    RLE_SETCOLOR_OP		2
#define	    RLE_SKIPPIXELS_OP	3
#define	    RLE_BYTEDATA_OP		5
#define	    RLE_RUNDATA_OP		6
#define	    RLE_EOF_OP			7

#define     RLE_CLEARFIRST_F	0x1		/* 如果设置的话，解压前将清除缓冲区（使用背景色） */
#define	    RLE_NO_BACKGROUND_F	0x2		/* 如果设置的话，表明流未提供背景色信息 */
#define	    RLE_ALPHA_F			0x4		/* 如果设置的话，表明流中存在ALPHA通道（通道号-1） */
#define	    RLE_COMMENT_F		0x8		/* 如果设置的话，表明流中存在注释信息 */

#define		RLE_OPCODE(s)		(s[0] & ~(unsigned char)LONGOPCODE)
#define		RLE_LONGP(s)		(s[0] & (unsigned char)LONGOPCODE)
#define		RLE_DATUM(s)		(s[1] & (unsigned char)0xff)


/* RLE头结构定义 */
typedef struct
{
	unsigned short	magic;				/* RLE判别标志（0xcc52） */
	unsigned short	xpos;				/* 图象的x、y原点坐标（左下角，第一象限）*/
	unsigned short	ypos;
	unsigned short	width;				/* 图象的宽度和高度（象素单位）*/
	unsigned short	height;
	unsigned char	flags;				/* 标志位 */
	unsigned char	channels;			/* 颜色通道数。不包括ALPHA通道 */
	unsigned char	chan_bits;			/* 每个颜色通道的位宽度（当前有效值只有8）*/
	unsigned char	pal_chan_count;		/* 调色板项的通道数 */
	unsigned char	pal_chan_bits;		/* 调色板通道的位宽度 */
} RLE_HEADER, *LPRLE_HEADER;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern RLE_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern RLE_API LPIRWP_INFO	CALLAGREEMENT rle_get_plugin_info(void);
extern RLE_API int			CALLAGREEMENT rle_init_plugin(void);
extern RLE_API int			CALLAGREEMENT rle_detach_plugin(void);
#endif	/* WIN32 */

extern RLE_API int	CALLAGREEMENT rle_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern RLE_API int	CALLAGREEMENT rle_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern RLE_API int	CALLAGREEMENT rle_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __RLE_MODULE_INC__ */
