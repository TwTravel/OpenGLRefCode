/********************************************************************

	bmp.h

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
			http://iseeexplorer.cosoft.org.cn

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—BMP图像读写模块定义文件
	
					读取功能：Windows 风格 －1～32位位图、RLE位图
							  OS/2 风格 － 1、4、8、24位位图
					保存功能：Windows 风格 －1、4、8、16(555)、24、
								32(888)位位图
	  
	本文件编写人：	YZ			yzfree##sina.com
		
	本文件版本：	10709
	最后修改于：	2001-7-9
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-7		修正RLE解码中的一处错误。
					1、4位图像输出符合ISee位流格式
					增加目标图像缓冲区附加字节
					增强了容错性
			  
		2001-6		发布新版代码
		2001-5		重新定义模块的接口及输出功能
		2000-10		修正了一个较为重大的BUG
		2000-7		第一个测试版发布


********************************************************************/



#ifndef __BMP_MODULE_INC__
#define __BMP_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("BMP插件")
#define MODULE_FUNC_PREFIX				("bmp_")
#define MODULE_FILE_POSTFIX				("BMP")

/* 文件判别标志 (windows - 'BM', OS/2 - 'BA')*/
#define DIB_HEADER_MARKER				((unsigned short)('M'<<8)|'B')
#define DIB_HEADER_MARKER_OS2			((unsigned short)('A'<<8)|'B')


#ifdef WIN32	/* Windows */

#	ifdef  BMP_EXPORTS
#	define BMP_API __declspec(dllexport)
#	else
#	define BMP_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define BMP_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* 位图调色板项结构 （Windows 风格）*/
typedef struct _tag_rgb_quad{
    unsigned char	rgbBlue; 
    unsigned char	rgbGreen; 
    unsigned char	rgbRed; 
    unsigned char	rgbReserved; 
} RGB_QUAD, *LPRGB_QUAD; 

/* 位图调色板项结构 （OS/2 风格）*/
typedef struct _tag_rgb_triple{
    unsigned char	rgbtBlue; 
    unsigned char	rgbtGreen; 
    unsigned char	rgbtRed; 
} RGB_TRIPLE, *LPRGB_TRIPLE; 



/* 位图信息头结构 （Windows 风格）*/
typedef struct _tag_bmp_info_header{
	unsigned long	biSize;
	long			biWidth;
	long			biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned long	biCompression;
	unsigned long	biSizeImage;
	long			biXPelsPerMeter;
	long			biYPelsPerMeter;
	unsigned long	biClrUsed;
	unsigned long	biClrImportant;
} BMP_INFO_HEADER, *LPBMP_INFO_HEADER;

/* 位图信息头结构 （OS/2 风格）*/
typedef struct _tag_bmp_core_header{
	unsigned long	bcSize; 
	unsigned short	bcWidth; 
	unsigned short	bcHeight; 
	unsigned short	bcPlanes; 
	unsigned short	bcBitCount; 
} BMP_CORE_HEADER, *LPBMP_CORE_HEADER; 



/* 位图信息结构 （Windows 风格） */
typedef struct _tag_bmp_info{
	BMP_INFO_HEADER	bmiHeader;
	RGB_QUAD		bmiColors[1];
} BMP_INFO, *LPBMP_INFO;

/* 位图信息结构 （OS/2 风格） */
typedef struct _tag_bmp_core{
	BMP_CORE_HEADER bmciHeader; 
	RGB_TRIPLE      bmciColors[1]; 
} BMP_CORE, *LPBMP_CORE; 



/* 位图文件头结构 （Windows 、OS/2 公用）*/
typedef struct _tag_bmp_file_header{
	unsigned short	bfType;
	unsigned long	bfSize;
	unsigned short	bfReserved1;
	unsigned short	bfReserved2;
	unsigned long	bfOffBits;
} BMP_FILE_HEADER, *LPBMP_FILE_HEADER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern BMP_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern BMP_API LPIRWP_INFO	CALLAGREEMENT bmp_get_plugin_info(void);
extern BMP_API int			CALLAGREEMENT bmp_init_plugin(void);
extern BMP_API int			CALLAGREEMENT bmp_detach_plugin(void);
#endif	/* WIN32 */

extern BMP_API int	CALLAGREEMENT bmp_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern BMP_API int	CALLAGREEMENT bmp_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern BMP_API int	CALLAGREEMENT bmp_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __BMP_MODULE_INC__ */
