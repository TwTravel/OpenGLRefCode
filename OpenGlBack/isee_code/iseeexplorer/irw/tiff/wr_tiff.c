/********************************************************************

	wr_tiff.c

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
	本文件用途：	ISee图像浏览器—TIFF图像读模块辅助实现文件

					保存功能：可将1、4、8、24位的单页或多页图象保存
								为对应的TIFF图象。
  
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

		2002-4		第一个发布版


********************************************************************/

#include <stdio.h>
#include <assert.h>

#include "libtiff\tiff.h"
#include "libtiff\tiffiop.h"
#include "wr_tiff.h"



/* 辅助函数 */
static void CALLAGREEMENT _ABGR_ARGB(unsigned char *pbits, unsigned long width);
static void CALLAGREEMENT _tiff_1_to_cross(unsigned char *pbits, unsigned long width);
static void CALLAGREEMENT _tiff_4_to_cross(unsigned char *pbits, unsigned long width);




/* 写主图象 */
int tiff_write_master_img(TIFF *tif, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	unsigned short	photometric, compression, *pal = 0;
	unsigned long	rowsperstrip = -1L;
	unsigned long	linebytes;
	
	int				i, j, cnum, tnum;
	unsigned char	*buf = 0;

	int				result = 0;

	assert(tif&&pinfo_str);

	__try
	{
		/* 设置压缩方式 */
		switch (lpsave->para_value[0])
		{
		case	0:
			compression = COMPRESSION_NONE; break;
		case	1:
			compression = COMPRESSION_JPEG; break;
		default:
			result = -1; __leave; break;			/* 指定了非法压缩方式 */
		}

		/* 设置色系 */
		switch (pinfo_str->bitcount)
		{
		case	1:
		case	4:
		case	8:
			photometric = PHOTOMETRIC_PALETTE;
			compression = COMPRESSION_NONE;			/* 如果是调色板图则不能使用JPEG压缩方式 */
			break;
		case	24:
			photometric = PHOTOMETRIC_RGB; break;
		default:
			assert(0); result = -2; __leave; break;	/* 源图象位深度不合法 */
		}
		
		
		/* 在存入数据前，先设置tiff文件的tag值 */
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, pinfo_str->height);
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, pinfo_str->width);
		/* 单平面方式，图像数据排列为RGB...RGB..RGB.. */
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

		/* 设置每个像素的取样数及调色板参数 */
		switch (pinfo_str->bitcount)
		{
		case	1:
		case	4:
		case	8:
			{
				int	palcnt = 1<<(pinfo_str->bitcount);	/* 调色板项个数 */

				TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL, 1);
				TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, pinfo_str->bitcount);
				
				assert((int)pinfo_str->pal_count == palcnt);
				
				/* 制作TIFF风格的调色板 */
				if ((pal=(unsigned short *)_TIFFmalloc(palcnt*sizeof(unsigned short)*3)) == 0)
				{
					result = -3; __leave; break;		/* 内存不足 */
				}
				for (j=0;j<palcnt;j++)
				{
					(pal+0*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&pinfo_str->palette[j]))->rgbRed))<<8;
					(pal+1*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&pinfo_str->palette[j]))->rgbGreen))<<8;
					(pal+2*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&pinfo_str->palette[j]))->rgbBlue))<<8;
				}
				
				/* 写入调色板数据 */
				TIFFSetField(tif, TIFFTAG_COLORMAP, pal+0*palcnt, pal+1*palcnt, pal+2*palcnt);
			}
			break;
		case	24:
			TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL, 3);
			TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, 8);
			break;
		default:
			assert(0); result = -2; __leave; break;
		}


		/* 图像所采用的色彩系统 */
		TIFFSetField(tif,TIFFTAG_PHOTOMETRIC, photometric);
		TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

		/* 根据压缩法设定进一步的参数 */
		switch (compression)
		{
		case	COMPRESSION_JPEG:
			TIFFSetField(tif, TIFFTAG_JPEGQUALITY, 75);		/* 缺省品质系数定为75 */
			TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
			break;
		case	COMPRESSION_NONE:
			break;
		default:
			assert(0); result = -4; __leave; break;	/* 系统不稳定 */
		}
		
		cnum = 0; tnum = (int)pinfo_str->imgnumbers;

		/* 如果是多页图象则设置页号及页属性 */
		if (pinfo_str->imgnumbers > 1)
		{
			TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
			TIFFSetField(tif, TIFFTAG_PAGENUMBER, cnum, tnum);
		}

		/* 设定带状数据中每一‘条’的扫描行个数 */
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,	TIFFDefaultStripSize(tif, rowsperstrip));

		linebytes = DIBSCANLINE_WIDTHBYTES(pinfo_str->bitcount * pinfo_str->width);

		/* 申请行缓冲区（+4是防止R、B转换时发生内存读写越界） */
		if ((buf=(unsigned char *)_TIFFmalloc(max(TIFFScanlineSize(tif),(int)linebytes)+4)) == 0)
		{
			result = -3; __leave;				/* 内存不足 */
		}
		_TIFFmemset(buf, 0, max(TIFFScanlineSize(tif),(int)linebytes)+4);

		
		/* 将象素数据按带状方式存入TIFF */
		for (i=0;i<(int)pinfo_str->height;i++)
		{
			_TIFFmemcpy(buf, pinfo_str->pp_line_addr[i], linebytes);

			/* TIFF规格定义的1位和4位象素排列方式与ISee位流格式不同，所以需要转换 */
			if (pinfo_str->bitcount == 1)
			{
				_tiff_1_to_cross(buf, pinfo_str->width);
			}
			else if (pinfo_str->bitcount == 4)
			{
				_tiff_4_to_cross(buf, pinfo_str->width);
			}
			else if ((pinfo_str->bitcount == 24)&&(compression != COMPRESSION_JPEG))
			{
				/* Sam的libtiff库中TIFFWriteScanline函数要求传入的buf排列为BGRA方式 
				 * ，正好与ISee位流格式相反，所以对24位图象做R、B对调。
				 * 另：JPEG压缩方式因为采用了JPEGLIB库，所以不用调换。*/
				_ABGR_ARGB(buf, pinfo_str->width);
			}

			/* 写入图象 */
			if (TIFFWriteScanline(tif, buf, i, 0) < 0)
			{
				result = -5; __leave;			/* 写入时发生错误 */
			}
		}
	}
	__finally
	{
		if (pal)
			_TIFFfree(pal);
		
		if (buf)
			_TIFFfree(buf);
	}

	return result;
}


/* 写辅（子）图象 */
int tiff_write_servant_img(TIFF *tif, LPSUBIMGBLOCK psub, LPSAVESTR lpsave)
{
	unsigned short	photometric, compression, *pal = 0;
	unsigned long	rowsperstrip = -1L;
	unsigned long	linebytes;
	
	int				i, j, cnum, tnum;
	unsigned char	*buf = 0;

	int				result = 0;


	assert(tif&&psub);

	__try
	{
		/* 设置压缩方式 */
		switch (lpsave->para_value[0])
		{
		case	0:
			compression = COMPRESSION_NONE; break;
		case	1:
			compression = COMPRESSION_JPEG; break;
		default:
			result = -1; __leave; break;			/* 指定了非法压缩方式 */
		}

		/* 设置色系 */
		switch (psub->bitcount)
		{
		case	1:
		case	4:
		case	8:
			photometric = PHOTOMETRIC_PALETTE;
			compression = COMPRESSION_NONE;			/* 如果是调色板图则不能使用JPEG压缩方式 */
			break;
		case	24:
			photometric = PHOTOMETRIC_RGB; break;
		default:
			result = -2; __leave; break;			/* 源图象位深度不合法 */
		}
		
		
		/* 在存入数据前，先设置tiff文件的tag值 */
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, psub->height);
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, psub->width);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

		/* 每个像素取样数 */
		switch (psub->bitcount)
		{
		case	1:
		case	4:
		case	8:
			{
				int	palcnt = 1<<(psub->bitcount);	/* 调色板项个数 */
				
				TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL, 1);
				TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, psub->bitcount);
				
				assert((int)psub->pal_count == palcnt);
				
				/* 制作TIFF风格的调色板 */
				if ((pal=(unsigned short *)_TIFFmalloc(palcnt*sizeof(unsigned short)*3)) == 0)
				{
					result = -3; __leave; break;		/* 内存不足 */
				}
				for (j=0;j<palcnt;j++)
				{
					(pal+0*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&psub->palette[j]))->rgbRed))<<8;
					(pal+1*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&psub->palette[j]))->rgbGreen))<<8;
					(pal+2*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&psub->palette[j]))->rgbBlue))<<8;
				}
				
				/* 写入调色板数据 */
				TIFFSetField(tif, TIFFTAG_COLORMAP, pal+0*palcnt, pal+1*palcnt, pal+2*palcnt);
			}
			break;
		case	24:
			TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL, 3);
			TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, 8);
			break;
		default:
			assert(0); result = -2; __leave; break;
		}


		TIFFSetField(tif,TIFFTAG_PHOTOMETRIC, photometric);
		TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

		switch (compression)
		{
		case COMPRESSION_JPEG:
			TIFFSetField(tif, TIFFTAG_JPEGQUALITY, 75);		/* 缺省品质系数定为75 */
			TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
			break;
		case	COMPRESSION_NONE:
			break;
		default:
			assert(0); result = -4; __leave; break;	/* 系统不稳定 */
		}

		cnum = (int)psub->number;
		tnum = (int)psub->parents->imgnumbers;
		
		TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
		TIFFSetField(tif, TIFFTAG_PAGENUMBER, cnum, tnum);

		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,	TIFFDefaultStripSize(tif, rowsperstrip));

		linebytes = DIBSCANLINE_WIDTHBYTES(psub->bitcount * psub->width);
		
		if ((buf=(unsigned char *)_TIFFmalloc(max(TIFFScanlineSize(tif),(int)linebytes)+4)) == 0)
		{
			result = -3; __leave;				/* 内存不足 */
		}
		_TIFFmemset(buf, 0, max(TIFFScanlineSize(tif),(int)linebytes)+4);

		
		/* 将象素数据按带状方式存入TIFF */
 		for (i=0;i<(int)psub->height;i++)
		{
			_TIFFmemcpy(buf, psub->pp_line_addr[i], linebytes);

			if (psub->bitcount == 1)
			{
				_tiff_1_to_cross(buf, psub->width);
			}
			else if (psub->bitcount == 4)
			{
				_tiff_4_to_cross(buf, psub->width);
			}
			else if ((psub->bitcount == 24)&&(compression != COMPRESSION_JPEG))
			{
				_ABGR_ARGB(buf, psub->width);
			}

			if (TIFFWriteScanline(tif, buf, i, 0) < 0)
			{
				result = -5; __leave;			/* 写入时发生错误 */
			}
		}
	}
	__finally
	{
		if (pal)
			_TIFFfree(pal);
		
		if (buf)
			_TIFFfree(buf);
	}

	return result;
}


/* 红、蓝分量转换（24位版）*/
void CALLAGREEMENT _ABGR_ARGB(unsigned char *pbits, unsigned long width)
{
	__asm
	{
		push esi;
		push ecx;
		
		mov esi, pbits;
		mov edx, width;
		
loop_for_pick_up:
		
		dec edx;
		mov eax, dword ptr [esi];		/* 提取一个象素到eax，高8位将保持不变 */
		mov ebx, eax;
		and ebx, 0FFh;					/* 分离出R、B分量值 */
		mov ecx, eax;
		and ecx, 0FF0000h;
		
		shl ebx, 10h;
		shr ecx, 10h;
		and eax, 0FF00FF00h;
		or  eax, ebx;
		or  eax, ecx;					/* 交换R和B的位置 */
		
		mov dword ptr [esi], eax;		/* 回写转换后的象素数据 */
		
		inc esi;
		inc esi;
		inc esi;
		
		cmp edx, 0h;
		jne loop_for_pick_up;
		
		pop ecx;
		pop esi;
	}
}


/* 1位图象象素翻转（适应TIFF要求）*/
void CALLAGREEMENT _tiff_1_to_cross(unsigned char *pbits, unsigned long width)
{
	unsigned char tmp;
	unsigned short us;
	int	j, bit, mask, linesize;
	
	assert(pbits);
	
	linesize = DIBSCANLINE_WIDTHBYTES(1*width);
	
	for (j=0;j<linesize;j++)
	{
		us  = (unsigned short)pbits[j] << 8;

		for (tmp=0, bit=1, mask=7;bit<16;bit+=2, mask--)
		{
			tmp |= (us>>bit)&(1<<mask);
		}

		pbits[j] = tmp;
	}
}


/* 4位图象象素翻转（适应TIFF要求）*/
void CALLAGREEMENT _tiff_4_to_cross(unsigned char *pbits, unsigned long width)
{
	int j;
	
	assert(pbits);
	
	for (j=0;j<(int)((width+1)/2);j++)
	{
		pbits[j] = (pbits[j]>>4)|(pbits[j]<<4);
	}
}

