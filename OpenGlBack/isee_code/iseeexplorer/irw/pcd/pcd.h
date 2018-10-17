/********************************************************************

	pcd.h

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

			isee##vip.163.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—PCD图像读写模块定义文件
	
					读取功能：可读取24位PCD图象（前三幅）
							  
					保存功能：不支持
							   
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20604
	最后修改于：	2002-6-4
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
		地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-6		第一个发布版（新版）

********************************************************************/



#ifndef __PCD_MODULE_INC__
#define __PCD_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PCD插件")
#define MODULE_FUNC_PREFIX				("pcd_")
#define MODULE_FILE_POSTFIX				("pcd")


/* 文件判别标志（PCD无详细的格式文档，柯达有意没有发布这些
 * 文档，所以只能通过判断图象最小长度来大概确定它是否是一个
 * PCD图象。当然，这种方式是很不精确的）
 */
#define PCD_MIN_SIZE					(1048576UL)
/* PCD图象方向信息位置 */
#define PCD_VERTICAL_LOC				72
#define PCD_VERTICAL_MASK				(0x3f)
/* PCD子图象尺寸（PCD图象是固定尺寸的）*/
#define PCD_1_W							192
#define PCD_1_H							128
#define PCD_2_W							384
#define PCD_2_H							256
#define PCD_3_W							768
#define PCD_3_H							512
#define PCD_4_W							1536
#define PCD_4_H							1024
#define PCD_5_W							3072
#define PCD_5_H							2048
/* PCD缺省图象位深度 */
#define PCD_DEF_BITCOUNT				24
/* PCD子图象数据在图象中的偏移（后两幅图象的偏移柯达公司没有公布） */
#define PCD_1_OFF						0x2000
#define PCD_2_OFF						0xb800
#define PCD_3_OFF						0x30000


#ifdef WIN32	/* Windows */

#	ifdef  PCD_EXPORTS
#	define PCD_API __declspec(dllexport)
#	else
#	define PCD_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PCD_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PCD_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PCD_API LPIRWP_INFO	CALLAGREEMENT pcd_get_plugin_info(void);
extern PCD_API int			CALLAGREEMENT pcd_init_plugin(void);
extern PCD_API int			CALLAGREEMENT pcd_detach_plugin(void);
#endif	/* WIN32 */

extern PCD_API int	CALLAGREEMENT pcd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PCD_API int	CALLAGREEMENT pcd_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PCD_API int	CALLAGREEMENT pcd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PCD_MODULE_INC__ */
