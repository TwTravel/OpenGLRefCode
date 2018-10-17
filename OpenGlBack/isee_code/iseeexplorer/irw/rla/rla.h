/********************************************************************

	rla.h

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
	本文件用途：	ISee图像浏览器—Alias/Wavefront RLA图像读写模块定义文件
	
					读取功能：可读取3通道24位压缩rgb色系的图象，不支持matte、
								aux通道
							  
					保存功能：可将24位图象保存为3通道RLE8压缩的rgb色系图象
							   
	本文件编写人：	
					YZ			yzfree##yeah.net
					
	本文件版本：	20509
	最后修改于：	2002-5-9
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-5		第一个发布版（新版）


********************************************************************/



#ifndef __RLA_MODULE_INC__
#define __RLA_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("RLA插件")
#define MODULE_FUNC_PREFIX				("rla_")
#define MODULE_FILE_POSTFIX				("rla")


/* 文件判别标志 */
#define RLA_REVISION					0xfffe


#ifdef WIN32	/* Windows */

#	ifdef  RLA_EXPORTS
#	define RLA_API __declspec(dllexport)
#	else
#	define RLA_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define RLA_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 窗口坐标结构 */
typedef struct
{
	short left;
	short right;
	short bottom;
	short top;
} RLA_WINDOW_S, *LPRLA_WINDOW_S;


/* RLA流头结构定义 */
typedef struct
{
	RLA_WINDOW_S window;			/* 背景窗口坐标 */
	RLA_WINDOW_S active_window;		/* 图象窗口坐标 */
	short frame;					/* 图象帧数 */
	short storage_type;				/* 图象数据压缩类型 */
	short num_chan;					/* 图象通道数（不包含matte、aux通道），一般为3 */
	short num_matte;				/* matte通道数，一般为1 */
	short num_aux;					/* 辅助通道数 */
	short revision;					/* 格式修订号，当前为0xfffe */
	char  gamma[16];				/* gamma值，一般为2.2 */
	char  red_pri[24];
	char  green_pri[24];
	char  blue_pri[24];
	char  white_pt[24];
	long  job_num;
	char  name[128];
	char  desc[128];				/* 图象内容描述 */
	char  program[64];				/* 创建图象的软件名称 */
	char  machine[32];				/* 创建图象的机器名称 */
	char  user[32];
	char  date[20];					/* 创建图象的日期、时间 */
	char  aspect[24];
	char  aspect_ratio[8];
	char  chan[32];					/* 所用色系，一般为"rgb" */
	short field;
	char  time[12];
	char  filter[32];
	short chan_bits;				/* 每通道的位深度 */
	short matte_type;
	short matte_bits;				/* matte通道位深度 */
	short aux_type;
	short aux_bits;					/* 辅助通道位深度 */
	char  aux[32];
	char  space[36];
	long  next;						/* 下一副子图象的数据偏移 */
} RLA_HEADER, *LPRLA_HEADER;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern RLA_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern RLA_API LPIRWP_INFO	CALLAGREEMENT rla_get_plugin_info(void);
extern RLA_API int			CALLAGREEMENT rla_init_plugin(void);
extern RLA_API int			CALLAGREEMENT rla_detach_plugin(void);
#endif	/* WIN32 */

extern RLA_API int	CALLAGREEMENT rla_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern RLA_API int	CALLAGREEMENT rla_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern RLA_API int	CALLAGREEMENT rla_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __RLA_MODULE_INC__ */
