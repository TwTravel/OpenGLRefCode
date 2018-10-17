/********************************************************************

	ani.h

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
	本文件用途：	ISee图像浏览器—ANI图像读写模块定义文件
	
					读取功能：1、4、8、16、24、32位ANI图象
							  
					保存功能：不支持
							   
	  
	本文件编写人：	
					YZ		yzfree##yeah.net
		
	本文件版本：	20330
	最后修改于：	2002-3-30
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

			2002-3		发布第一个版本（新版，支持ISeeIO系统）		


********************************************************************/



#ifndef __ANI_MODULE_INC__
#define __ANI_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("ANI插件")
#define MODULE_FUNC_PREFIX				("ani_")
#define MODULE_FILE_POSTFIX				("ani")

/* 文件判别标志 */


#ifdef WIN32	/* Windows */

#	ifdef  ANI_EXPORTS
#	define ANI_API __declspec(dllexport)
#	else
#	define ANI_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define ANI_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 光标类型定义 */
#define ANI_CURTYPE		2


/* 主、子图象类型定义 */
enum CUR_DATA_TYPE
{
	ANI_CUR_PRI_IMAGE,
	ANI_CUR_SUB_IMAGE
};


/* 光标的图象块信息头结构 */
typedef struct
{
	unsigned char	width;			/* 图象宽度 */
	unsigned char	height;			/* 图象高度 */
	unsigned char	color_count;	/* 颜色个数(如果位深度大于8时为0) */
	unsigned char	reserved;
	unsigned short	x_hotspot;		/* 热点X坐标 */
	unsigned short	y_hotspot;		/* 热点Y坐标 */
	unsigned long	bytes_in_res;	/* 图象数据在资源中所占的字节数 */
	unsigned long	image_offset;	/* 图象数据的偏移 */
} ANI_CURDIRENTRY, *LPANI_CURDIRENTRY;


/* 光标信息头结构 */
typedef struct 
{
	unsigned short	reserved;		/* 保留 */
	unsigned short	type;			/* 资源类型(CURTYPE为光标) */
	unsigned short	count;			/* 图象个数 */
	ANI_CURDIRENTRY	entries[1];		/* 每一个图象的入口 */
} ANI_CURDIR, *LPANI_CURDIR;



/* RIFF 块类型ID定义 */
#define	RIFF_STR	"RIFF"
#define LIST_STR	"LIST"

#define ACON_STR	"ACON"
#define INFO_STR	"INFO"
#define INAM_STR	"INAM"
#define IART_STR	"IART"

#define FRAM_STR	"fram"
#define ANIH_STR	"anih"
#define RATE_STR	"rate"
#define SEQ_STR		"seq "
#define ICON_STR	"icon"

#define NULL_STR	"\0\0\0\0"


typedef enum 
{
	ANICHUNK_OUT = 0,
	ANICHUNK_RIFF,
	ANICHUNK_LIST,
	ANICHUNK_INAM,
	ANICHUNK_IART,
	ANICHUNK_ANIH,
	ANICHUNK_RATE,
	ANICHUNK_SEQ,
	ANICHUNK_ICON,
	ANICHUNK_UNKONW				/* 未知类型 */
} ANICHUNK, *LPANICHUNK;

/* RIFF 类型串长度 */
#define FORMTYPESEIZE	4


/* ANI文件头结构 */
typedef struct _tagAnihead
{
	unsigned char	riff[4];
	unsigned long	imgsize;
	unsigned char	acon[4];
} ANIHEAD, PANIHEAD;


/* ANI通用块头部结构（标记+块大小）*/
typedef  struct  _tagAnitag{
	char			ck_id[4];	/* 块标记 */
	unsigned long	ck_size;	/* 块大小（字节计） */
} ANITAG, *PANITAG;


/* ANI文件信息数据区“anih”块结构 */
typedef  struct  _tagAniheader{
	unsigned long	cbSizeof;		/* 块大小总是36字节 */
	unsigned long	cFrames;		/* 保存的图象桢数 */
	unsigned long	cSteps;			/* 显示的图象桢数 */
	unsigned long	cx;				/* 图象宽度 */
	unsigned long	cy;				/* 图象高度 */
	unsigned long	cBitCount;		/* 颜色位数 */
	unsigned long	cPlanes;
	unsigned long	jifRate;		/* JIF速率 */
	unsigned long	fl;				/* AF_ICON/AF_SEQUENCE设置标记 */
} ANIHEADER, *PANIHEADER;


/* 将60分之一秒的单位数据转为千分之一秒的数据 */
#define		FPS_TO_MS(r)		(r*(1000/60))



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern ANI_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern ANI_API LPIRWP_INFO	CALLAGREEMENT ani_get_plugin_info(void);
extern ANI_API int			CALLAGREEMENT ani_init_plugin(void);
extern ANI_API int			CALLAGREEMENT ani_detach_plugin(void);
#endif	/* WIN32 */

extern ANI_API int	CALLAGREEMENT ani_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern ANI_API int	CALLAGREEMENT ani_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern ANI_API int	CALLAGREEMENT ani_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ANI_MODULE_INC__ */
