/********************************************************************

	gif.h

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
	本文件用途：	ISee图像浏览器—GIF图像读写模块定义文件
	
					读取功能：1~8位、静态或动态GIF图像
					保存功能：因GIF-LZW专利期到2003年才失效，所以
							  暂不能提供保存功能
	  
	本文件编写人：	YZ			yzfree##sina.com
		
	本文件版本：	11227
	最后修改于：	2001-12-27
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2001-12		修正了GIF-LZW解压算法中存在的一处错误，并
						加强了_get_imginfo()函数的容错性。(B01)
		  2001-7		发布新版代码
		  2000-9		第一个测试版发布
		  2000-8		Sam编写了GIF读写库


	GIF图像文件格式版权声明：
	---------------------------------------------------------------
	The Graphics Interchange Format(c) is the Copyright property of
	CompuServe Incorporated. GIF(sm) is a Service Mark property of
	CompuServe Incorporated.
	---------------------------------------------------------------

********************************************************************/



#ifndef __GIF_MODULE_INC__
#define __GIF_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("GIF插件")
#define MODULE_FUNC_PREFIX				("gif_")
#define MODULE_FILE_POSTFIX				("GIF")

/* 文件判别标志 */
#define GIF_MARKER_87a					("GIF87a")
#define GIF_MARKER_89a					("GIF89a")


#ifdef WIN32	/* Windows */

#	ifdef  GIF_EXPORTS
#	define GIF_API __declspec(dllexport)
#	else
#	define GIF_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define GIF_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* GIF版本标识串长度 */
#define GIFVERSIZE					6

struct _tagGIFHEADER;
typedef struct _tagGIFHEADER		GIFHEADER;
typedef struct _tagGIFHEADER		*LPGIFHEADER;

struct _tagGIFINFO;
typedef struct _tagGIFINFO			GIFINFO;
typedef struct _tagGIFINFO			*LPGIFINFO;

struct _tagGIFRGB;
typedef struct _tagGIFRGB			GIFRGB;
typedef struct _tagGIFRGB			*LPGIFRGB;

struct _tagISEERGB;
typedef struct _tagISEERGB			ISEERGB;
typedef struct _tagISEERGB			*LPISEERGB;

struct _tagIMAGEDATAINFO;
typedef struct _tagIMAGEDATAINFO	IMAGEDATAINFO;
typedef struct _tagIMAGEDATAINFO	*LPIMAGEDATAINFO;

struct _tagGIFINSIDEINFO;
typedef struct _tagGIFINSIDEINFO	GIFINSIDEINFO;
typedef struct _tagGIFINSIDEINFO	*LPGIFINSIDEINFO;

struct _tagLZWTABLE;
typedef struct _tagLZWTABLE			LZWTABLE;
typedef struct _tagLZWTABLE			*LPLZWTABLE;

union _tagCODEWORD;
typedef union _tagCODEWORD			CODEWORD;
typedef union _tagCODEWORD			*LPCODEWORD;

struct _tagGRAPHCTRL;
typedef struct _tagGRAPHCTRL		GRAPHCTRL;
typedef struct _tagGRAPHCTRL		*LPGRAPHCTRL;

struct _tagNOTEHCTRL;
typedef struct _tagNOTEHCTRL		NOTEHCTRL;
typedef struct _tagNOTEHCTRL		*LPNOTEHCTRL;

struct _tagTEXTCTRL;
typedef struct _tagTEXTCTRL			TEXTCTRL;
typedef struct _tagTEXTCTRL			*LPTEXTCTRL;

struct _tagAPPCTRL;
typedef struct _tagAPPCTRL			APPCTRL;
typedef struct _tagAPPCTRL			*LPAPPCTRL;


/*@@@@@@@@@@@@@@@GIF87a 和 GIF89a 共用的结构@@@@@@@@@@@@@@@@@@*/

struct _tagGIFINFO
{
	unsigned short FileType;		/* 89a or 87a */
	unsigned short ColorNum;		/* 颜色数 */
	unsigned short BitCount;		/* 图像的位深度 */
	unsigned short Width;
	unsigned short Height;
	unsigned short FrameCount;		/* 共有几幅图 */
	unsigned char  bkindex;			/* 背景色索引 */
	unsigned char  InitPixelBits;	/* 压缩数据起始数据Bits位数 */
};


/* GIF调色板元素结构 */
struct _tagGIFRGB
{
	unsigned char bRed;
	unsigned char bGreen;
	unsigned char bBlue;
};

/* ISee调色板元素结构 */
struct _tagISEERGB
{
	unsigned char bBlue;
	unsigned char bGreen;
	unsigned char bRed;
	unsigned char rev;
};

/* GIF图像头结构 */
struct _tagGIFHEADER
{
	unsigned char	gif_type[GIFVERSIZE];	/* 图像版本(87a or 89a) */
	unsigned short	scr_width;				/* 逻辑屏宽度 */
	unsigned short	scr_height;				/* 逻辑屏高度 */
	unsigned char	global_flag;			/* 全局标记 */
	unsigned char	bg_color;				/* 背景色索引 */
	unsigned char	aspect_radio;			/* 89a版的长宽比 */
};


/* 图像描述符 */
struct _tagIMAGEDATAINFO
{
	unsigned char	label;					/* 标签值：0x2c */
	unsigned short	left; 					/* 子图像左上角在逻辑屏幕中的位置 */
	unsigned short	top;
	unsigned short	width;
	unsigned short	height;
	unsigned char	local_flag;				/* 本地位域 */
};

#define	GIF_MAXCOLOR	256

/* GIF综合信息结构 */
struct _tagGIFINSIDEINFO
{
	int				type;					/* GIF版本：87a = 2 89a = 1 */

	unsigned long	scr_width;				/* 逻辑屏信息 */
	unsigned long	scr_height;
	unsigned long	scr_bitcount;
	GIFRGB			p_pal[GIF_MAXCOLOR];	/* 全局调色板数据 */
	
	unsigned long	first_img_width;		/* 首幅图像信息 */
	unsigned long	first_img_height;
	unsigned long	first_img_bitcount;
	GIFRGB			p_first_pal[GIF_MAXCOLOR];	
	
	int				img_count;				/* 共有几幅图 */
	int				bk_index;				/* 背景色索引 */
};


#define LZWTABLESIZE	4096	/* GIF-LZW 串表尺寸 */

/*	GIF-LZW 字典单元结构 */
struct _tagLZWTABLE	 
{
	short	prefix;			/* 前缀码索引（无前缀码时为-1） */
	short	code;			/* 尾码 */
	int		used;			/* 是否被占用标志：0－未占用，1－已占用 */
};


/* GIF-LZW编码单元 */
union _tagCODEWORD
{
	unsigned char bcode[sizeof(unsigned long)];
	unsigned long lcode;
};


/*--------------------------------------------------------
  注：以下结构只适用于GIF89a格式的图像文件 
--------------------------------------------------------*/

/* 图形控制扩展块 */
struct _tagGRAPHCTRL
{
	unsigned char	extintr;			/* 0x21	*/
	unsigned char	label;				/* 0xF9 */
	unsigned char	block_size;			/* 0x04 */
	unsigned char	field;
	unsigned short	delay_time;			/* 延迟时间 */
	unsigned char	tran_color_index;	/* 透明色索引 */
	unsigned char	block_terminator;	/* 0x00 */
};

/* 注释扩展块 */
struct _tagNOTEHCTRL
{
	unsigned char extintr;				/* 0x21	*/
	unsigned char label;				/* 0xfe */
};

/* 文本扩展块 */
struct _tagTEXTCTRL
{
	unsigned char	extintr;			/* 0x21 */
	unsigned char	label;				/* 0x01 */
	unsigned char	block_size;			/* 0x0c */
	unsigned short	left;
	unsigned short	top;
	unsigned short	width;
	unsigned short	height; 
	unsigned char	fore_color_index;	/* 前景色索引 */
	unsigned char	bk_color_index;		/* 背景色索引 */
};

/* 应用程序扩展块 */
struct _tagAPPCTRL
{
	unsigned char extintr;				/* 0x21	*/
	unsigned char label;				/* 0xff	*/
	unsigned char block_size;			/* 0xb	*/
	unsigned char identifier[8];
	unsigned char authentication[3];
};



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern GIF_API LPIRWP_INFO  CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern GIF_API LPIRWP_INFO  CALLAGREEMENT gif_get_plugin_info(void);
extern GIF_API int			CALLAGREEMENT gif_init_plugin(void);
extern GIF_API int			CALLAGREEMENT gif_detach_plugin(void);
#endif	/* WIN32 */

extern GIF_API int	CALLAGREEMENT gif_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern GIF_API int	CALLAGREEMENT gif_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern GIF_API int	CALLAGREEMENT gif_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __GIF_MODULE_INC__ */
