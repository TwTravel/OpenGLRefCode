/********************************************************************

	tiff.c

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
	本文件用途：	ISee图像浏览器—TIFF图像读写模块实现文件

					读取功能：以 调色板、灰度、RGB、YCbCr 方式存储的TIFF图
								象（包括多页图象），但不支持位深度超过32位
								的图象。
  
					保存功能：可将1、4、8、24位的单页或多页图象保存为对
								应的TIFF图象。
	

	本文件编写人：	YZ				yzfree##sina.com
					xuhappy			xuxuhappy##sina.com
					zjucypher		xubo.cy##263.net

	本文件版本：	20511
	最后修改于：	2002-5-11

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------

    引用代码编写人及版权信息：
	----------------------------------------------------------------
	代码类别：	TIFF编解代码库(LibTiff)		目录：libtiff
	
	Copyright (c) 1988-1997 Sam Leffler
	Copyright (c) 1991-1997 Silicon Graphics, Inc.
	  
	Permission to use, copy, modify, distribute, and sell this software and 
	its documentation for any purpose is hereby granted without fee, provided
	that (i) the above copyright notices and this permission notice appear in
	all copies of the software and related documentation, and (ii) the names of
	Sam Leffler and Silicon Graphics may not be used in any advertising or
	publicity relating to the software without the specific, prior written
	permission of Sam Leffler and Silicon Graphics.
	
	THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
	EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
	WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  

	IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
	ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
	OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
	WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
	LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
	OF THIS SOFTWARE.
	----------------------------------------------------------------
			
	修正历史：

		2002-5		修正读取JPEG压缩方式的TIFF图象时R、B分量颠倒的问题。
					修正不能正确读取PhotoShop导出的TIFF图象的问题。
					leye发现，YZ修正。

		2002-4		发布新版代码。支持ISeeIO，及以JPEG方式存储的TIFF
					图象。保存功能支持1、4、8、24位深度。支持多页TIFF。
		
		2001-2		加入了保存功能
		2001-1		修正RGBA转换函数中存在的问题，并改进了步进控制代码
		2001-1		核心码改用libtiff库
		2000-7		第一个发布版


********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */

/*###################################################################

  移植注释：以下代码使用了WIN32系统的SEH(结构化异常处理)及多线程同步
			对象“关键段”，在移植时应转为Linux的相应语句。

  备注：本插件需要JPEG插件中的jpeglib 6b库的支持，所以在编译本插件之
			前请先编译JPEG插件。

  #################################################################*/


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* 缩短windows.h文件的编译时间 */
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libtiff\tiff.h"
#include "libtiff\tiffio.h"
#include "libtiff\tiffiop.h"
#include ".\tiff.h"
#include ".\rd_tiff.h"
#include ".\wr_tiff.h"


IRWP_INFO			tiff_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	tiff_get_info_critical;	/* tiff_get_image_info函数的关键段 */
CRITICAL_SECTION	tiff_load_img_critical;	/* tiff_load_image函数的关键段 */
CRITICAL_SECTION	tiff_save_img_critical;	/* tiff_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _tiff_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave);

static int  CALLAGREEMENT _get_mask(int bitcount, int alpha, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&tiff_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&tiff_get_info_critical);
			InitializeCriticalSection(&tiff_load_img_critical);
			InitializeCriticalSection(&tiff_save_img_critical);

			/* 替换缺省的错误及警告处理函数 */
			TIFFSetWarningHandler(_TIFFwarningHandler);
			TIFFSetErrorHandler(_TIFFerrorHandler);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&tiff_get_info_critical);
			DeleteCriticalSection(&tiff_load_img_critical);
			DeleteCriticalSection(&tiff_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

TIFF_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&tiff_irwp_info;
}

#else

TIFF_API LPIRWP_INFO CALLAGREEMENT tiff_get_plugin_info()
{
	_init_irwp_info(&tiff_irwp_info);

	return (LPIRWP_INFO)&tiff_irwp_info;
}

TIFF_API void CALLAGREEMENT tiff_init_plugin()
{
	/* 初始化多线程同步对象 */
}

TIFF_API void CALLAGREEMENT tiff_detach_plugin()
{
	/* 销毁多线程同步对象 */
}

#endif /* WIN32 */


/* 初始化插件信息结构 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info)
{
	assert(lpirwp_info);

	/* 初始化结构变量 */
	memset((void*)lpirwp_info, 0, sizeof(IRWP_INFO));

	/* 版本号。（十进制值，十位为主版本号，个位为副版本，*/
	lpirwp_info->irwp_version = MODULE_BUILDID;
	/* 插件名称 */
	strcpy((char*)(lpirwp_info->irwp_name), MODULE_NAME);
	/* 本模块函数前缀 */
	strcpy((char*)(lpirwp_info->irwp_func_prefix), MODULE_FUNC_PREFIX);


	/* 插件的发布类型。0－调试版插件，1－发布版插件 */
#ifdef _DEBUG
	lpirwp_info->irwp_build_set = 0;
#else
	lpirwp_info->irwp_build_set = 1;
#endif


	/* 功能标识 （##需手动修正） */
	lpirwp_info->irwp_function = IRWP_READ_SUPP | IRWP_WRITE_SUPP;

	/* 设置模块支持的保存位深 */
	/* ################################################################# */
	/* 各位数减一，否则32位格式无法表示。此处请用TIFF正确的位深填写！*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(1-1)) | (1UL<<(4-1)) | \
		(1UL<<(8-1)) | (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 2;	/* 2－允许保存多副图像 */
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 1;
	lpirwp_info->irwp_save.para[0].count = 2;	/* 两种压缩方式 */
	strcpy((char*)lpirwp_info->irwp_save.para[0].desc, (const char *)"压缩方式");

	lpirwp_info->irwp_save.para[0].value[0] = 0;		/* 不压缩 */
	lpirwp_info->irwp_save.para[0].value[1] = 1;		/* JPEG   */
	
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[0], (const char *)"不压缩");
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[1], (const char *)"JPEG压缩算法");

	
	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 4;	

	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"TIFF格式的复杂性超乎我的想象了:)");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
		(const char *)"xuhappy");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
		(const char *)"xuxuhappy##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
		(const char *)":)");
	/* ---------------------------------[2] － 第三组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_name), 
		(const char *)"zjucypher");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_email), 
		(const char *)"xubo.cy##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_message), 
		(const char *)":)");
	/* ---------------------------------[3] － 第四组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_name), 
				(const char *)"Sam Leffler");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_email), 
		(const char *)"tiff##olympiakos.com");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_message), 
				(const char *)":)");
	/* ---------------------------------[4] － 第五组 -------------- */
	/* 后续开发者信息可加在此处。
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	*/
	/* ------------------------------------------------------------- */


	/* 插件描述信息（扩展名信息）*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* 别名个数（##需手动修正） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"TIFF");

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
TIFF_API int CALLAGREEMENT tiff_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	TIFF			*tif = 0;

	unsigned long	alpha;
	unsigned short	*sampleinfo, extrasamples;
	unsigned short	bpp, spp, orgi, compression, phint;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&tiff_get_info_critical);

			/* 打开指定流 */
			if ((tif=TIFFOpen((const char*)psct, "rmH")) == (TIFF*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 检查图象是否可以被本插件读取 */
			switch (tiff_to_rgba_ok(tif))
			{
			case	0:
				break;
			case	-1:	/* 受损或不被支持的图象 */
			case	-2:
			case	-3:
			case	-4:
				b_status = ER_BADIMAGE; __leave; break;
			case	-5:
				b_status = ER_FILERWERR; __leave; break;
			default:
				b_status = ER_SYSERR; __leave; break;
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_TIFF;			/* 图像文件格式（后缀名） */
			
			/* 获取文件的长度、图像的宽度、高度等信息 */
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &(pinfo_str->width));
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &(pinfo_str->height));

			TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
			TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
			TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo);
			TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &phint);
			
			if ((extrasamples == 1)&&(
				(sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA)||
				(sampleinfo[0] == EXTRASAMPLE_UNASSALPHA)))
				alpha = 1;
			else
				alpha = 0;

			switch (phint)
			{
			case	PHOTOMETRIC_RGB:		/* RGB color model */
				/* 如果存在ALPHA数据则将其计算入象素位深中 */
				pinfo_str->bitcount = bpp*(spp-((alpha) ? 0 : extrasamples));
				break;
			case	PHOTOMETRIC_SEPARATED:	/* !color separations */
				pinfo_str->bitcount = bpp*(spp-1);	/* CMYK共四个采样点，去除一个 */
				break;
			case	PHOTOMETRIC_MINISWHITE:	/* min value is white */
			case	PHOTOMETRIC_MINISBLACK:	/* min value is black */
			case	PHOTOMETRIC_CIELAB:		/* !1976 CIE L*a*b* */
			case	PHOTOMETRIC_ICCLAB:		/* ICC L*a*b* [Adobe TIFF Technote 4] */
			case	PHOTOMETRIC_ITULAB:		/* ITU L*a*b* */
			case	PHOTOMETRIC_LOGL:		/* CIE Log2(L) */
			case	PHOTOMETRIC_LOGLUV:		/* CIE Log2(L) (u',v') */
			case	PHOTOMETRIC_PALETTE:	/* color map indexed */
			case	PHOTOMETRIC_YCBCR:		/* !CCIR 601 */
			case	PHOTOMETRIC_MASK:		/* $holdout mask */
			default:
				pinfo_str->bitcount = bpp*spp;
				break;
			}

			/* 检测图象位深是否过大 */
			if ((pinfo_str->bitcount > ISEEIRW_MAX_BITCOUNT) || (pinfo_str->bitcount == 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orgi);
			switch (orgi)
			{
			case	ORIENTATION_TOPLEFT:
			case	ORIENTATION_TOPRIGHT:
			case	ORIENTATION_RIGHTTOP:
			case	ORIENTATION_LEFTTOP:
				pinfo_str->order = 0;		/* 正向图 */
				break;
			case	ORIENTATION_BOTRIGHT:
			case	ORIENTATION_BOTLEFT:
			case	ORIENTATION_RIGHTBOT:
			case	ORIENTATION_LEFTBOT:
				pinfo_str->order = 1;		/* 倒向图 */
				break;
			default:
				b_status = ER_BADIMAGE; __leave; break;
			}

			/* 获取图象的压缩方式 */
			TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
			
			switch (compression)
			{
			case    COMPRESSION_NONE:			pinfo_str->compression = ICS_TIFF_NONE; break;
			case	COMPRESSION_CCITTRLE:		pinfo_str->compression = ICS_TIFF_CCITTRLE; break;
			case	COMPRESSION_CCITTFAX3:		pinfo_str->compression = ICS_TIFF_CCITTAX3; break;
			case	COMPRESSION_CCITTFAX4:		pinfo_str->compression = ICS_TIFF_CCITTFAX4; break;
			case	COMPRESSION_LZW:			pinfo_str->compression = ICS_TIFF_LZW; break;
			case	COMPRESSION_OJPEG:			pinfo_str->compression = ICS_TIFF_OJPEG; break;
			case	COMPRESSION_JPEG:			pinfo_str->compression = ICS_TIFF_JPEG; break;
			case	COMPRESSION_NEXT:			pinfo_str->compression = ICS_TIFF_NEXT; break;
			case	COMPRESSION_CCITTRLEW:		pinfo_str->compression = ICS_TIFF_CCITTRLEW; break;
			case	COMPRESSION_PACKBITS:		pinfo_str->compression = ICS_TIFF_PACKBITS; break;
			case	COMPRESSION_THUNDERSCAN:	pinfo_str->compression = ICS_TIFF_THUNDERSCAN; break;
			case	COMPRESSION_IT8CTPAD:		pinfo_str->compression = ICS_TIFF_IT8CTPAD; break;
			case	COMPRESSION_IT8LW:			pinfo_str->compression = ICS_TIFF_IT8LW; break;
			case	COMPRESSION_IT8MP:			pinfo_str->compression = ICS_TIFF_IT8MP; break;
			case	COMPRESSION_IT8BL:			pinfo_str->compression = ICS_TIFF_IT8BL; break;
			case	COMPRESSION_PIXARFILM:		pinfo_str->compression = ICS_TIFF_PIXARFILM; break;
			case	COMPRESSION_PIXARLOG:		pinfo_str->compression = ICS_TIFF_PIXARLOG; break;
			case	COMPRESSION_DEFLATE:		pinfo_str->compression = ICS_TIFF_DEFLATE; break;
			case	COMPRESSION_ADOBE_DEFLATE:	pinfo_str->compression = ICS_TIFF_ADOBE_DEFLATE; break;
			case	COMPRESSION_DCS:			pinfo_str->compression = ICS_TIFF_DCS; break;
			case	COMPRESSION_JBIG:			pinfo_str->compression = ICS_TIFF_JBIG; break;
			case	COMPRESSION_SGILOG:			pinfo_str->compression = ICS_TIFF_SGILOG; break;
			case	COMPRESSION_SGILOG24:		pinfo_str->compression = ICS_TIFF_SGILOG24; break;
			default:
				pinfo_str->compression = ICS_UNKONW; break;
			}
			
			if (_get_mask(pinfo_str->bitcount, alpha, 
				(unsigned long *)(&pinfo_str->b_mask), 
				(unsigned long *)(&pinfo_str->g_mask), 
				(unsigned long *)(&pinfo_str->r_mask),
				(unsigned long *)(&pinfo_str->a_mask)) == 1)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (tif)
				TIFFClose(tif);

			LeaveCriticalSection(&tiff_get_info_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		pinfo_str->data_state = 0;
		b_status = ER_SYSERR;
	}

	return (int)b_status;
}


/* 读取图像位数据 */
TIFF_API int CALLAGREEMENT tiff_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	TIFF			*tif = 0;
	ISE_TIFFRGBAImage img;
	LPSUBIMGBLOCK	subimg_tmp;
	
	int				i, j, linesize;
	unsigned short	bpp, spp, orgi, compression, photo_inter;
	unsigned long	alpha;
	unsigned short	*sampleinfo, extrasamples;
	LPSUBIMGBLOCK	lpsub = 0, lplast = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&tiff_load_img_critical);

			memset((void*)&img, 0, sizeof(ISE_TIFFRGBAImage));
			pinfo_str->process_total = 1;
			pinfo_str->process_current = 0;

			/* 打开指定流 */
			if ((tif=TIFFOpen((const char*)psct, "rmH")) == (TIFF*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 获取与位深度有关的数据 */
			TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
			TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
			TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo);
			TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photo_inter);
			
			if ((extrasamples == 1)&&(
				(sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA)||
				(sampleinfo[0] == EXTRASAMPLE_UNASSALPHA)))
				alpha = 1;
			else
				alpha = 0;

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 检查图象是否可以被本插件读取 */
				switch (tiff_to_rgba_ok(tif))
				{
				case	0:
					break;
				case	-1:	/* 受损或不被支持的图象 */
				case	-2:
				case	-3:
				case	-4:
					b_status = ER_BADIMAGE; __leave; break;
				case	-5:
					b_status = ER_FILERWERR; __leave; break;
				default:
					b_status = ER_SYSERR; __leave; break;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_TIFF;			/* 图像文件格式（后缀名） */
				
				/* 获取文件的长度、图像的宽度、高度等信息 */
				TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &(pinfo_str->width));
				TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &(pinfo_str->height));

				switch (photo_inter)
				{
				case	PHOTOMETRIC_RGB:		/* RGB全彩色模式 */
					/* 如果存在ALPHA数据则将其计算入象素位深中 */
					pinfo_str->bitcount = bpp*(spp-((alpha) ? 0 : extrasamples));
					break;
				case	PHOTOMETRIC_SEPARATED:	/* CMYK色系 */
					pinfo_str->bitcount = bpp*(spp-1);	/* CMYK共四个采样点，去除一个 */
					break;
				case	PHOTOMETRIC_MINISWHITE:	/* 灰度图，最小值为白色 */
				case	PHOTOMETRIC_MINISBLACK:	/* 灰度图，最小值为黑色 */
				case	PHOTOMETRIC_CIELAB:		/* 1976年CIE的Lab色系 */
				case	PHOTOMETRIC_ICCLAB:		/* ICC Lab */
				case	PHOTOMETRIC_ITULAB:		/* ITU Lab */
				case	PHOTOMETRIC_LOGL:		/* CIE Log2(L) */
				case	PHOTOMETRIC_LOGLUV:		/* CIE Log2(L) (u',v') */
				case	PHOTOMETRIC_PALETTE:	/* 调色板模式 */
				case	PHOTOMETRIC_YCBCR:		/* YCBCR色系（JPEG） */
				case	PHOTOMETRIC_MASK:		/* 掩码图（单色） */
				default:
					pinfo_str->bitcount = bpp*spp;
					break;
				}
				
				/* 检测图象位深是否过大 */
				if ((pinfo_str->bitcount > ISEEIRW_MAX_BITCOUNT) || (pinfo_str->bitcount == 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orgi);
				switch (orgi)
				{
				case	ORIENTATION_TOPLEFT:
				case	ORIENTATION_TOPRIGHT:
				case	ORIENTATION_RIGHTTOP:
				case	ORIENTATION_LEFTTOP:
					pinfo_str->order = 0;		/* 正向图 */
					break;
				case	ORIENTATION_BOTRIGHT:
				case	ORIENTATION_BOTLEFT:
				case	ORIENTATION_RIGHTBOT:
				case	ORIENTATION_LEFTBOT:
					pinfo_str->order = 1;		/* 倒向图 */
					break;
				default:
					b_status = ER_BADIMAGE; __leave; break;
				}

				/* 获取图象的压缩方式 */
				TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compression);
				switch (compression)
				{
				case    COMPRESSION_NONE:			pinfo_str->compression = ICS_TIFF_NONE; break;
				case	COMPRESSION_CCITTRLE:		pinfo_str->compression = ICS_TIFF_CCITTRLE; break;
				case	COMPRESSION_CCITTFAX3:		pinfo_str->compression = ICS_TIFF_CCITTAX3; break;
				case	COMPRESSION_CCITTFAX4:		pinfo_str->compression = ICS_TIFF_CCITTFAX4; break;
				case	COMPRESSION_LZW:			pinfo_str->compression = ICS_TIFF_LZW; break;
				case	COMPRESSION_OJPEG:			pinfo_str->compression = ICS_TIFF_OJPEG; break;
				case	COMPRESSION_JPEG:			pinfo_str->compression = ICS_TIFF_JPEG; break;
				case	COMPRESSION_NEXT:			pinfo_str->compression = ICS_TIFF_NEXT; break;
				case	COMPRESSION_CCITTRLEW:		pinfo_str->compression = ICS_TIFF_CCITTRLEW; break;
				case	COMPRESSION_PACKBITS:		pinfo_str->compression = ICS_TIFF_PACKBITS; break;
				case	COMPRESSION_THUNDERSCAN:	pinfo_str->compression = ICS_TIFF_THUNDERSCAN; break;
				case	COMPRESSION_IT8CTPAD:		pinfo_str->compression = ICS_TIFF_IT8CTPAD; break;
				case	COMPRESSION_IT8LW:			pinfo_str->compression = ICS_TIFF_IT8LW; break;
				case	COMPRESSION_IT8MP:			pinfo_str->compression = ICS_TIFF_IT8MP; break;
				case	COMPRESSION_IT8BL:			pinfo_str->compression = ICS_TIFF_IT8BL; break;
				case	COMPRESSION_PIXARFILM:		pinfo_str->compression = ICS_TIFF_PIXARFILM; break;
				case	COMPRESSION_PIXARLOG:		pinfo_str->compression = ICS_TIFF_PIXARLOG; break;
				case	COMPRESSION_DEFLATE:		pinfo_str->compression = ICS_TIFF_DEFLATE; break;
				case	COMPRESSION_ADOBE_DEFLATE:	pinfo_str->compression = ICS_TIFF_ADOBE_DEFLATE; break;
				case	COMPRESSION_DCS:			pinfo_str->compression = ICS_TIFF_DCS; break;
				case	COMPRESSION_JBIG:			pinfo_str->compression = ICS_TIFF_JBIG; break;
				case	COMPRESSION_SGILOG:			pinfo_str->compression = ICS_TIFF_SGILOG; break;
				case	COMPRESSION_SGILOG24:		pinfo_str->compression = ICS_TIFF_SGILOG24; break;
				default:
					pinfo_str->compression = ICS_UNKONW; break;
				}
				
				/* 设定掩码值 */
				if (_get_mask(pinfo_str->bitcount, alpha, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->data_state = 1;
			}				

			/* 设置图像个数 */
			pinfo_str->imgnumbers = TIFFNumberOfDirectories(tif);
			
			/* 步进值为图象页数 */
			pinfo_str->process_total = pinfo_str->imgnumbers;
			pinfo_str->process_current = 0;

			/* 按页读取图象 */
			for (j=0;j<(int)pinfo_str->imgnumbers;j++,pinfo_str->process_current++)
			{
				if (j == 0)
				{
					/* 首幅图象 */
					switch (tiff_rgba_image_begin(&img, tif, 1/*严格型*/, 0, (void*)pinfo_str))
					{
					case	-1:
					case	-2:
					case	-3:
					case	-4:
					case	-5:
					case	-8:
						b_status = ER_BADIMAGE; __leave; break;
					case	-6:
						b_status = ER_MEMORYERR; __leave; break;
					case	-7:
						b_status = ER_FILERWERR; __leave; break;
					case	0:	/* 成功 */
						break;
					default:
						assert(0);
						b_status = ER_SYSERR;
						__leave;
						break;
					}

					/* 取得扫描行尺寸 */
					linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
					
					assert(pinfo_str->p_bit_data == (unsigned char *)0);
					/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
					pinfo_str->p_bit_data = (unsigned char *)malloc(linesize * pinfo_str->height + 4);
					if (!pinfo_str->p_bit_data)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					assert(pinfo_str->pp_line_addr == (void**)0);
					/* 分配行首地址数组 */
					pinfo_str->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo_str->height);
					if (!pinfo_str->pp_line_addr)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					/* 初始化行首地址数组 */
					if (pinfo_str->order == 0)		/* 正向 */
					{
						for (i=0;i<(int)(pinfo_str->height);i++)
							pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
					}
					else							/* 倒向 */
					{
						for (i=0;i<(int)(pinfo_str->height);i++)
							pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
					}

					/* 读取象素数据 */
					if (tiff_rgba_image_get(&img, pinfo_str->width, pinfo_str->height))
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* 清除img结构用于下一次读取操作 */
					tiff_rgba_image_end(&img);
				}
				else
				{
					/* 分配子图象节点 */
					if ((lpsub=_alloc_SUBIMGBLOCK()) == 0)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					lpsub->number  = j;				/* 子图象序号由 1 开始 */
					lpsub->parents = pinfo_str;

					/* 加入子图象链表尾部 */
					if (lplast == 0)
					{
						pinfo_str->psubimg = lpsub;	/* 首幅子图象 */
						lplast = lpsub;
						
						lpsub->prev = 0;
						lpsub->next = 0;
					}
					else
					{
						lpsub->prev = lplast;		/* 后续子图象 */
						lpsub->next = 0;

						lplast->next = lpsub;
						lplast = lpsub;
					}
					
					/* 转向下一幅图象（IFD） */
					if (TIFFReadDirectory(tif) == 0)
					{
						b_status = ER_BADIMAGE; __leave;
					}

					/* 检查图象是否可以被本插件读取 */
					switch (tiff_to_rgba_ok(tif))
					{
					case	0:
						break;
					case	-1:	/* 受损或不被支持的图象 */
					case	-2:
					case	-3:
					case	-4:
						b_status = ER_BADIMAGE; __leave; break;
					case	-5:
						b_status = ER_FILERWERR; __leave; break;
					default:
						b_status = ER_SYSERR; __leave; break;
					}
					
					/* 获取图像的宽度、高度、位深度等信息 */
					TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &(lpsub->width));
					TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &(lpsub->height));
					TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &bpp);
					TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
					TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo);
					TIFFGetFieldDefaulted(tif, TIFFTAG_PHOTOMETRIC, &photo_inter);
					
					if ((extrasamples == 1)&&(
						(sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA)||
						(sampleinfo[0] == EXTRASAMPLE_UNASSALPHA)))
						alpha = 1;
					else
						alpha = 0;
					
					/* 计算图象位深度 */
					switch (photo_inter)
					{
					case	PHOTOMETRIC_RGB:
						/* 如果存在ALPHA数据则将其计算入象素位深中 */
						lpsub->bitcount = bpp*(spp-((alpha) ? 0 : extrasamples));
						break;
					case	PHOTOMETRIC_SEPARATED:
						lpsub->bitcount = bpp*(spp-1);	/* CMYK共四个采样点，去除一个 */
						break;
					case	PHOTOMETRIC_MINISWHITE:
					case	PHOTOMETRIC_MINISBLACK:
					case	PHOTOMETRIC_CIELAB:
					case	PHOTOMETRIC_ICCLAB:
					case	PHOTOMETRIC_ITULAB:
					case	PHOTOMETRIC_LOGL:
					case	PHOTOMETRIC_LOGLUV:
					case	PHOTOMETRIC_PALETTE:
					case	PHOTOMETRIC_YCBCR:
					case	PHOTOMETRIC_MASK:
					default:
						lpsub->bitcount = bpp*spp;
						break;
					}
					
					/* 检测图象位深是否过大 */
					if ((lpsub->bitcount > ISEEIRW_MAX_BITCOUNT) || (lpsub->bitcount == 0))
					{
						b_status = ER_BADIMAGE; __leave;
					}
					
					/* 判断图象的存放方向 */
					TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orgi);
					switch (orgi)
					{
					case	ORIENTATION_TOPLEFT:
					case	ORIENTATION_TOPRIGHT:
					case	ORIENTATION_RIGHTTOP:
					case	ORIENTATION_LEFTTOP:
						lpsub->order = 0;		/* 正向图 */
						break;
					case	ORIENTATION_BOTRIGHT:
					case	ORIENTATION_BOTLEFT:
					case	ORIENTATION_RIGHTBOT:
					case	ORIENTATION_LEFTBOT:
						lpsub->order = 1;		/* 倒向图 */
						break;
					default:
						b_status = ER_BADIMAGE; __leave; break;
					}
					
					memset((void*)&img, 0, sizeof(ISE_TIFFRGBAImage));

					/* 准备读取操作 */
					switch (tiff_rgba_image_begin(&img, tif, 1, 1, (void*)lpsub))
					{
					case	-1:
					case	-2:
					case	-3:
					case	-4:
					case	-5:
					case	-8:
						b_status = ER_BADIMAGE; __leave; break;
					case	-6:
						b_status = ER_MEMORYERR; __leave; break;
					case	-7:
						b_status = ER_FILERWERR; __leave; break;
					case	0:	/* 成功 */
						break;
					default:
						assert(0);
						b_status = ER_SYSERR;
						__leave;
						break;
					}
					
					/* 取得扫描行尺寸 */
					linesize = _calcu_scanline_size(lpsub->width, lpsub->bitcount);
					
					assert(lpsub->p_bit_data == (unsigned char *)0);
					/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
					lpsub->p_bit_data = (unsigned char *)malloc(linesize * lpsub->height + 4);
					if (!lpsub->p_bit_data)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					assert(lpsub->pp_line_addr == (void**)0);
					/* 分配行首地址数组 */
					lpsub->pp_line_addr = (void **)malloc(sizeof(void*) * lpsub->height);
					if (!lpsub->pp_line_addr)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					/* 初始化行首地址数组 */
					if (lpsub->order == 0)		/* 正向 */
					{
						for (i=0;i<(int)(lpsub->height);i++)
							lpsub->pp_line_addr[i] = (void *)(lpsub->p_bit_data+(i*linesize));
					}
					else						/* 倒向 */
					{
						for (i=0;i<(int)(lpsub->height);i++)
							lpsub->pp_line_addr[i] = (void *)(lpsub->p_bit_data+((lpsub->height-i-1)*linesize));
					}
					
					/* 读取图象 */
					if (tiff_rgba_image_get(&img, lpsub->width, lpsub->height))
					{
						b_status = ER_FILERWERR; __leave;
					}
					
					tiff_rgba_image_end(&img);
				}

				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}

			
			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* 释放子图像 */
				if (pinfo_str->psubimg != 0)
				{
					while(subimg_tmp=pinfo_str->psubimg->next)
					{
						_free_SUBIMGBLOCK(pinfo_str->psubimg);
						pinfo_str->psubimg = subimg_tmp;
					}
					_free_SUBIMGBLOCK(pinfo_str->psubimg);
					pinfo_str->psubimg = 0;
				}
				if (pinfo_str->p_bit_data)
				{
					free(pinfo_str->p_bit_data);
					pinfo_str->p_bit_data = (unsigned char *)0;
				}
				if (pinfo_str->pp_line_addr)
				{
					free(pinfo_str->pp_line_addr);
					pinfo_str->pp_line_addr = (void**)0;
				}
				if (pinfo_str->data_state == 2)
					pinfo_str->data_state =1;	/* 自动降级 */
			}

			tiff_rgba_image_end(&img);

			if (tif)
				TIFFClose(tif);
			
			LeaveCriticalSection(&tiff_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
TIFF_API int CALLAGREEMENT tiff_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	TIFF			*tif = 0;
	
	LPSUBIMGBLOCK	psub;
	int				i;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&tiff_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_tiff_is_valid_img(pinfo_str, lpsave) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			/* 模式串中的'l'只适用于Intel类CPU（包括AMD等），如移植至
			 * 非Intel CPU（如MOTO CPU）则需改为'b'lH */
			if ((tif=TIFFOpen((const char*)psct, "wlH")) == (TIFF*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			psub = pinfo_str->psubimg;

			for (i=0;i<(int)pinfo_str->imgnumbers;i++)
			{
				if (i == 0)	/* 保存主图象 */
				{
					switch (tiff_write_master_img(tif, pinfo_str, lpsave))
					{
					case	0:			/* 写入成功 */
						break;
					case	-3:
						b_status = ER_MEMORYERR; __leave; break;
					case	-5:
						b_status = ER_FILERWERR; __leave; break;
					default:
					case	-1:
					case	-2:
					case	-4:
						assert(0); b_status = ER_SYSERR; __leave; break;
					}
				}
				else
				{
					/* 转入下一个子图象 */
					if (TIFFWriteDirectory(tif) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					assert(psub);		/* 此时必然会有子图象节点 */

					switch (tiff_write_servant_img(tif, psub, lpsave))
					{
					case	0:			/* 写入成功 */
						break;
					case	-2:			/* 因此前子图象没有做位深判断，所以在此判断 */
						b_status = ER_NSIMGFOR; __leave; break;
					case	-3:
						b_status = ER_MEMORYERR; __leave; break;
					case	-5:
						b_status = ER_FILERWERR; __leave; break;
					default:
					case	-1:
					case	-4:
						assert(0); b_status = ER_SYSERR; __leave; break;
					}
					
					psub = psub->next;
				}
			}

			
			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (tif)
				TIFFClose(tif);
			
			LeaveCriticalSection(&tiff_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}





/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/* 内部辅助函数 */

/* 计算扫描行尺寸(四字节对齐) */
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _tiff_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	int i;

	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(tiff_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (tiff_irwp_info.irwp_save.img_num)
		if (tiff_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */

	for (i=0;i<tiff_irwp_info.irwp_save.para[0].count;i++)
		if (tiff_irwp_info.irwp_save.para[0].value[i] == lpsave->para_value[0])
			break;
		
	return (i==tiff_irwp_info.irwp_save.para[0].count) ? -3:0;		/* -3:无效的压缩选择 */
}


/* 获取各颜色分量的值 */
static int CALLAGREEMENT _get_mask(int bitcount, int alpha, 
							unsigned long * pb_mask, 
							unsigned long * pg_mask, 
							unsigned long * pr_mask, 
							unsigned long * pa_mask)
{
	int result = 0;
	
	switch(bitcount)
	{
	case	1:
	case	2:
	case	3:
	case	4:
	case	5:
	case	6:
	case	7:
	case	8:
		/* 调色板位图无掩码 */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:		/* 555 格式 */
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:		/* 888 格式 */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;

		if (alpha)
			*pa_mask = 0xff000000;
		else
			*pa_mask = 0x0;
		break;
	default:		/* 非法格式 */
		result = -1;
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}
	
	return result;	/* 0-成功， -1－非法的图像格式 */
}


/* 分配一个子图像节点，并初始化节点内数据 */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void)
{
	LPSUBIMGBLOCK	p_subimg = 0;
	
	/* 申请子图像信息节点(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* 内存不足 */
	
	/* 初始化为 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));
	
	p_subimg->number = 1;
	p_subimg->colorkey = -1;
	
	return p_subimg;
}


/* 释放一个子图像节点，包括其中的位缓冲区及行首地址数组 */
void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);
	assert(p_node->number > 0);
	
	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);
	
	if (p_node->p_bit_data)
		free(p_node->p_bit_data);
	
	free(p_node);
}


