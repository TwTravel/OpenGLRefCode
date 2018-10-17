/********************************************************************

	ras.h

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

			isee##vip.163.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—Sun RAS图像读写模块定义文件
	
					读取功能：可读取1、8、24、32位压缩与未压缩RAS图象
							  
					保存功能：提供24位非压缩格式的保存功能
	  
	本文件编写人：	
					netmap		beahuang##hotmail.com
					YZ			yzfree##yeah.net
	
	本文件版本：	20610
	最后修改于：	2002-6-10
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	注：本文件的部分函数参考了FreeImage图象库的源代码，在此向
			Floris van den Berg (flvdberg@wxs.nl)
			Herv Drolon (drolon@iut.univ-lehavre.fr)
		表示感谢！如果想更多的了解FreeImage图象库的情况，请拜
		访它的官方网站：
			http://www.6ixsoft.com/
	----------------------------------------------------------------
	修正历史：
			
		2002-6		第一个版本发布（新版）

********************************************************************/



#ifndef __RAS_MODULE_INC__
#define __RAS_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("RAS插件")
#define MODULE_FUNC_PREFIX				("ras_")
#define MODULE_FILE_POSTFIX				("ras")

/* 文件判别标志 */
#define RAS_MAGIC						0x59a66a95


#ifdef WIN32	/* Windows */

#	ifdef  RAS_EXPORTS
#	define RAS_API __declspec(dllexport)
#	else
#	define RAS_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define RAS_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* 文件结构定义 */
#define RT_OLD					0			/* 未压缩格式 */
#define RT_STANDARD				1			/* 未压缩格式 */
#define RT_BYTE_ENCODED			2			/* RLE压缩格式 */
#define RT_FORMAT_RGB			3			/* RGB格式 */
#define RT_FORMAT_TIFF			4
#define RT_FORMAT_IFF			5

#define RT_EXPERIMENTAL			0xFFFF

#define RMT_NONE				0			/* 无调色板数据 */
#define RMT_EQUAL_RGB			1
#define RMT_RAW					2

#define RESC					0x80		/* RLE压缩标识字 */
#define RAS_ROUNDROW			16			/* 扫描行对齐位数 */


/* RAS图象头结构 */
typedef struct _SUNHeaderinfo
{
    unsigned long	ras_magic;				/* 判别标志 */
	unsigned long	ras_width;				/* 宽度 */
	unsigned long	ras_height;				/* 高度 */
	unsigned long	ras_depth;				/* 位深度 */
	unsigned long	ras_length;				/* 图象数据长度（不包括头结构及可能的调色板）*/
	unsigned long	ras_type;				/* 格式类型 */
	unsigned long	ras_maptype;			/* 调色板数据存放方式 */
	unsigned long	ras_maplength;			/* 调色板长度（字节计）*/
} SUNHeaderinfo, *LPSUNHeaderinfo;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"						/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern RAS_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern RAS_API LPIRWP_INFO	CALLAGREEMENT ras_get_plugin_info(void);
extern RAS_API int			CALLAGREEMENT ras_init_plugin(void);
extern RAS_API int			CALLAGREEMENT ras_detach_plugin(void);
#endif	/* WIN32 */

extern RAS_API int	CALLAGREEMENT ras_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern RAS_API int	CALLAGREEMENT ras_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern RAS_API int	CALLAGREEMENT ras_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __RAS_MODULE_INC__ */
