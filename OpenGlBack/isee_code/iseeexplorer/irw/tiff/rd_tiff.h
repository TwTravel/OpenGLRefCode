/********************************************************************

	rd_tiff.h

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
	本文件用途：	ISee图像浏览器—TIFF图像读模块辅助定义文件

					读取功能：以 调色板、灰度、RGB、YCbCr 方式存储的TIFF图
								象（包括多页图象），但不支持位深度超过32位
								的图象。
							  
	本文件编写人：	YZ				yzfree##yeah.net

	本文件版本：	20418
	最后修改于：	2002-4-19

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
		2002-4		第一个发布版


********************************************************************/


#ifndef __RD_TIFF_INC__
#define __RD_TIFF_INC__


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* 调用者数据接口定义文件 */
#endif



#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

typedef struct _ise_TIFFRGBAImage ISE_TIFFRGBAImage;

typedef void (*ise_tileContigRoutine)
    (ISE_TIFFRGBAImage*, uint32, uint32, uint32, uint32, int32, int32, unsigned char*);

typedef void (*ise_tileSeparateRoutine)
    (ISE_TIFFRGBAImage*, uint32, uint32, uint32, uint32, int32, int32,
	unsigned char*, unsigned char*, unsigned char*, unsigned char*);


struct _ise_TIFFRGBAImage
{
	TIFF*	tif;
	int		stoponerr;			/* 当错误发生时是否继续读的标志 */
	int		isContig;			/* 象素数据存放方式 */
	int		alpha;
	uint32	width;
	uint32	height;
	uint16	bitspersample;
	uint16	samplesperpixel;
	uint16	orientation;		/* 图象存放方位 */
	uint16	photometric;		/* 图象所使用的色系 */
	uint16*	redcmap;			/* TIFF图象的分离式调色板数据 */
	uint16*	greencmap;
	uint16*	bluecmap;

								/* 对应的读取函数 */
	int	(*get)(ISE_TIFFRGBAImage*, uint32, uint32);

	union
	{
	    void (*any)(ISE_TIFFRGBAImage*);
	    ise_tileContigRoutine	contig;
	    ise_tileSeparateRoutine	separate;
	} put;						/* 象素展开（转RGBA）函数 */

	TIFFYCbCrToRGB	*ycbcr;		/* YCbCr 转换表 */

	int			row_offset;
	int			col_offset;

	int			info_type;		/* 对应的ISee数据包类型 */
	int			ycbcr_mark;
	union
	{
		LPINFOSTR		pinfo_str;
		LPSUBIMGBLOCK	psub_str;
	} ise;						/* 数据包指针 */

};



/* 接口函数 */
int  tiff_to_rgba_ok(TIFF* tif);
void tiff_rgba_image_end(ISE_TIFFRGBAImage* img);
int  tiff_rgba_image_begin(ISE_TIFFRGBAImage* img, TIFF* tif, int stop, int type, void* pinfo);
int	 tiff_rgba_image_get(ISE_TIFFRGBAImage* img, uint32 w, uint32 h);


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif /* __RD_TIFF_INC__ */
