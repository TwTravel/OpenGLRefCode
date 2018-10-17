/********************************************************************

	tga.h

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
	本文件用途：	ISee图像浏览器—TGA图像读写模块实现文件
	
					读取功能：未压缩的8、15、16、24、32位图像，及
							采用RLE压缩的8、15、16、24、32位图像
					保存功能：未压缩的8、15、16、24、32位图像
	  
	  
	本文件编写人：	晓月儿			xiaoyueer##263.net
					YZ				yzfree##sina.com
		
	本文件版本：	11225
	最后修改于：	2001-12-25
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
		地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-12		发布新版代码（加快了图像读取速度，并增加
					了保存图像的功能）
			  
		2001-3		为提高模块读图像速度而修改代码
		2000-8		第一个测试版发布
				
********************************************************************/



#ifndef __TGA_MODULE_INC__
#define __TGA_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* 数据存储设备接口定义文件 */
#endif


/* 定义模块版本、名称、函数前缀、默认的文件名后缀 */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("TGA插件")
#define MODULE_FUNC_PREFIX				("tga_")
#define MODULE_FILE_POSTFIX				("TGA")


/**************TGA 图象类型 *************/
typedef enum{
	TGA_NULL		= 0,
	TGA_UNCPSPAL	= 1,
	TGA_UNCPSCOLOR	= 2,
	TGA_UNCPSGRAY	= 3,
	TGA_RLEPAL		= 9,
	TGA_RLECOLOR	= 10,
	TGA_RLEGRAY		= 11
}TGATYPE;

/**********TGA 图象数据存储类型(与bDescriptor对应位与) ***********/
typedef enum{
	TGA_HORZMIRROR = 0x10,
	TGA_VERTMIRROR = 0x20
}TGAMIRROR;


#ifdef WIN32	/* Windows */

#	ifdef  TGA_EXPORTS
#	define TGA_API __declspec(dllexport)
#	else
#	define TGA_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define TGA_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/**************TGA 文件头描述 ***************/
typedef struct {
	unsigned char  bID_Length;		/* 附加信息长度 */
	unsigned char  bPalType;		/* 调色板信息 */
	unsigned char  bImageType;		/* 图象类型(0,1,2,3,9,10,11) */
	unsigned short wPalFirstNdx;	/* 调色板第一个索引值 */
	unsigned short wPalLength;		/* 调色板索引数(以调色板单元为单位) */
	unsigned char  bPalBits;		/* 一个调色板单位位数(15,16,24,32) */
	unsigned short wLeft;			/* 图象左端坐标(基本无用) */
	unsigned short wBottom;			/* 图象底端坐标(基本无用) */
	unsigned short wWidth;			/* 图象宽度 */
	unsigned short wDepth;			/* 图象长度 */
	unsigned char  bBits;			/* 一个象素位数 */
	unsigned char  bDescriptor;		/* 附加特性描述  */
}TGAHEADER, *LPTGAHEADER;

/**************TGA 文件尾描述 ***************/
typedef struct {
	unsigned long  eao;				/* 扩展区偏移 */
	unsigned long  ddo;				/* 开发者区偏移 */
	unsigned char  info[16];		/* TRUEVISION-XFILE 商标字符串 */
	unsigned char  period;			/* 字符"." */
	unsigned char  zero;			/* 0 */
} TGAFOOTER, *LPTGAFOOTER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern TGA_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern TGA_API LPIRWP_INFO	CALLAGREEMENT tga_get_plugin_info(void);
extern TGA_API int			CALLAGREEMENT tga_init_plugin(void);
extern TGA_API int			CALLAGREEMENT tga_detach_plugin(void);
#endif	/* WIN32 */

extern TGA_API int	CALLAGREEMENT tga_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern TGA_API int	CALLAGREEMENT tga_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern TGA_API int	CALLAGREEMENT tga_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __TGA_MODULE_INC__ */
