/********************************************************************

	ico.h

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
			http://cosoft.org.cn/projects/iseeexplorer
			
	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—ICO图像读写模块定义文件
	
					读取功能：1、4、8、16、24、32位ICO图象。
							  
					保存功能：不支持
							   
	  
	本文件编写人：	
					YZ		yzfree##yeah.net
		
	本文件版本：	10103
	最后修改于：	2001-1-3
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
			  2002-3		发布新版模块。去除了所有与Windows系统有关
								的API调用，使代码更容易移植。
			  2001-1		修正了模块存在的一些BUG。
			  2000-8		加强了模块的容错性能。
			  2000-8		第一个发布版。

  
********************************************************************/



#ifndef __ICO_MODULE_INC__
#define __ICO_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("ICO插件")
#define MODULE_FUNC_PREFIX				("ico_")
#define MODULE_FILE_POSTFIX				("ico")

/* 文件判别标志 */


#ifdef WIN32	/* Windows */

#	ifdef  ICO_EXPORTS
#	define ICO_API __declspec(dllexport)
#	else
#	define ICO_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define ICO_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 图标类型定义 */
#define ICONTYPE	1


/* 主、子图象类型定义 */
enum ICO_DATA_TYPE
{
	ICO_PRI_IMAGE,
	ICO_SUB_IMAGE
};


/* 图标的信息头结构 */
typedef struct
{
	unsigned char	width;			/* 图象宽度 */
	unsigned char	height;			/* 图象高度 */
	unsigned char	color_count;	/* 颜色个数(如果位深度大于8时为0) */
	unsigned char	reserved;
	unsigned short	planes;			/* 位面数 */
	unsigned short	bit_count;		/* 每象素位数 */
	unsigned long	bytes_in_res;	/* 图象数据在资源中所占的字节数 */
	unsigned long	image_offset;	/* 图象数据的偏移 */
} ICONDIRENTRY, *LPICONDIRENTRY;


/* 图标文件信息头结构 */
typedef struct 
{
	unsigned short	reserved;		/* 保留 */
	unsigned short	type;			/* 资源类型(ICONTYPE为图标) */
	unsigned short	count;			/* 图象个数 */
	ICONDIRENTRY	entries[1];		/* 每一个图象的入口 */
} ICONDIR, *LPICONDIR;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern ICO_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern ICO_API LPIRWP_INFO	CALLAGREEMENT ico_get_plugin_info(void);
extern ICO_API int			CALLAGREEMENT ico_init_plugin(void);
extern ICO_API int			CALLAGREEMENT ico_detach_plugin(void);
#endif	/* WIN32 */

extern ICO_API int	CALLAGREEMENT ico_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern ICO_API int	CALLAGREEMENT ico_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern ICO_API int	CALLAGREEMENT ico_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ICO_MODULE_INC__ */
