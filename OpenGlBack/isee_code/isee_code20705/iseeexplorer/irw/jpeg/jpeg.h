/********************************************************************

	jpeg.h

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
	本文件用途：	ISee图像浏览器—JPEG图像读写模块定义文件
	
					读取功能：8位灰度图像、24位真彩图像
					保存功能：8位灰度图像、24位真彩图像
						    （可设定品质系数）
	  
	本文件编写人：	orbit		Inte2000##263.net
					YZ			yzfree##sina.com
		
	本文件版本：	10715
	最后修改于：	2001-7-15
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-7		发布新版代码
		2000-12		改进了进度控制代码
		2000-9		第一个测试版发布


	引用代码声明：
	---------------------------------------------------------------
	本插件使用了IJG的jpeglib6b函数库，感谢他们所做的工作！

	"this software is based in part on the work of the Independent 
	JPEG Group"
  
********************************************************************/



#ifndef __JPEG_MODULE_INC__
#define __JPEG_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("JPEG插件")
#define MODULE_FUNC_PREFIX				("jpeg_")
#define MODULE_FILE_POSTFIX				("JPG")


#ifdef WIN32	/* Windows */

#	ifdef  JPEG_EXPORTS
#	define JPEG_API __declspec(dllexport)
#	else
#	define JPEG_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define JPEG_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* JPEG图像标识字 */
#define JPG_SOI_MARK            0xD8FF


/* 错误处理信息结构 */
struct my_error_mgr
{
	struct jpeg_error_mgr		pub;	
	jmp_buf						setjmp_buffer;
};

typedef struct my_error_mgr		*my_error_ptr;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern JPEG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern JPEG_API LPIRWP_INFO CALLAGREEMENT jpeg_get_plugin_info(void);
extern JPEG_API int			CALLAGREEMENT jpeg_init_plugin(void);
extern JPEG_API int			CALLAGREEMENT jpeg_detach_plugin(void);
#endif	/* WIN32 */

extern JPEG_API int CALLAGREEMENT jpeg_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern JPEG_API int	CALLAGREEMENT jpeg_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern JPEG_API int	CALLAGREEMENT jpeg_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __JPEG_MODULE_INC__ */
