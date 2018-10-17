/********************************************************************

	cur.h

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
	本文件用途：	ISee图像浏览器—CUR图像读写模块定义文件
	
					读取功能：1、4、8、16、24、32位光标图象
							  
					保存功能：不支持
							   
	  
	本文件编写人：	
					YZ		yzfree##yeah.net
		
	本文件版本：	20327
	最后修改于：	2002-3-27
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
			2002-3		发布新版模块。去除了所有与Windows系统有关
							的API调用，使代码更容易移植。
			2000-8		第一个发布版。


********************************************************************/



#ifndef __CUR_MODULE_INC__
#define __CUR_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("CUR插件")
#define MODULE_FUNC_PREFIX				("cur_")
#define MODULE_FILE_POSTFIX				("cur")


#ifdef WIN32	/* Windows */

#	ifdef  CUR_EXPORTS
#	define CUR_API __declspec(dllexport)
#	else
#	define CUR_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define CUR_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 光标类型定义 */
#define CURTYPE		2


/* 主、子图象类型定义 */
enum CUR_DATA_TYPE
{
	CUR_PRI_IMAGE,
	CUR_SUB_IMAGE
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
} CURDIRENTRY, *LPCURDIRENTRY;


/* 光标信息头结构 */
typedef struct 
{
	unsigned short	reserved;		/* 保留 */
	unsigned short	type;			/* 资源类型(CURTYPE为光标) */
	unsigned short	count;			/* 图象个数 */
	CURDIRENTRY		entries[1];		/* 每一个图象的入口 */
} CURDIR, *LPCURDIR;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"				/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern CUR_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern CUR_API LPIRWP_INFO	CALLAGREEMENT cur_get_plugin_info(void);
extern CUR_API int			CALLAGREEMENT cur_init_plugin(void);
extern CUR_API int			CALLAGREEMENT cur_detach_plugin(void);
#endif	/* WIN32 */

extern CUR_API int	CALLAGREEMENT cur_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern CUR_API int	CALLAGREEMENT cur_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern CUR_API int	CALLAGREEMENT cur_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __CUR_MODULE_INC__ */
