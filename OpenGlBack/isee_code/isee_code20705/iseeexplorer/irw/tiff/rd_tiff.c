/********************************************************************

	rd_tiff.c

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

					读取功能：以 调色板、灰度、RGB、YCbCr 方式存储的TIFF图
								象（包括多页图象），但不支持位深度超过32位
								的图象。
							  
	本文件编写人：	YZ				yzfree##yeah.net

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
		2002-4		第一个发布版


********************************************************************/


#include "libtiff\tiff.h"
#include "libtiff\tiffiop.h"
#include "rd_tiff.h"
#include <assert.h>
#include <stdio.h>




/*************************************************************************************
* 助手函数声明																		 *
*************************************************************************************/

static int get_tile_contig(ISE_TIFFRGBAImage*, uint32, uint32);
static int get_tile_separate(ISE_TIFFRGBAImage*, uint32, uint32);
static int get_strip_contig(ISE_TIFFRGBAImage*, uint32, uint32);
static int get_strip_separate(ISE_TIFFRGBAImage*, uint32, uint32);
static int sel_tile_contig_case(ISE_TIFFRGBAImage*);
static int sel_tile_separate_case(ISE_TIFFRGBAImage*);

static int is_ccitt_compression(TIFF* tif);
static void setorientation(ISE_TIFFRGBAImage* img);
static void _cal_scanline_pdest(ISE_TIFFRGBAImage *img, int x, int y, int *scanline, char **pdest);




/*************************************************************************************
* 注：本文件中的代码参考自libtiff库中的tif_getimage.c								 *
*************************************************************************************/





/*************************************************************************************
* 函数实现																			 *
*************************************************************************************/


/* 判断指定的TIFF图象是否可以转换为RGBA图象（tiff_read_rgba_image函数是否支持）*/
int tiff_to_rgba_ok(TIFF* tif)
{
    TIFFDirectory*	td = &tif->tif_dir;
    uint16			photometric;
    int				colorchannels;

	int				result = 0;

	assert(tif);

	__try
	{
		/* 修正BUG。本插件在不使用预读图象概略信息函数时，易发生堆栈溢出的现象，在此修正。*/
		/* ：经跟踪是下列代码引起的问题，在此注释掉。该问题可能是libtiff库内部问题，所以 */
		/*   暂不作深究。*/
		/* 此问题由 leye 发现并提出。YZ 修改。2002-5 */
		
		/* >>>>>>>>>>>> 以下为注释掉的代码：*/
		/* 寻找合适的解压缩函数
		if (!(*tif->tif_setupdecode)(tif))
		{
			result = -1; __leave;
		}
		*/
		/* <<<<<<<<<<< 修改结束 */
		
		/* 只支持 1、2、4、8 位的采样点位深度 */
		switch (td->td_bitspersample)
		{
		case 1: 
		case 2: 
		case 4:
		case 8: 
			break;
		default:
			result = -2; __leave;	/* 不支持的采样点位深度 */
		}
		
		/* 颜色通道数目 */
		colorchannels = td->td_samplesperpixel - td->td_extrasamples;
		
		/* 确定色系 */
		if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric))
		{
			/* 如果图象没有描述所用色系则自行判断 */
			switch (colorchannels)
			{
			case 1:
				photometric = PHOTOMETRIC_MINISBLACK;
				break;
			case 3:
				photometric = PHOTOMETRIC_RGB;
				break;
			default:
				result = -3; __leave;	/* 无法确定图象所用色系 */
			}
		}
		
		/* 虑除不能处理的存储格式 */
		switch (photometric)
		{
		case PHOTOMETRIC_MINISWHITE:
		case PHOTOMETRIC_MINISBLACK:
		case PHOTOMETRIC_PALETTE:
			if (td->td_planarconfig == PLANARCONFIG_CONTIG && td->td_samplesperpixel != 1 && td->td_bitspersample < 8 )
			{
				result = -4; __leave;	/* 不能支持的图象存储格式 */
			}
			break;
		case PHOTOMETRIC_YCBCR:
			if (td->td_planarconfig != PLANARCONFIG_CONTIG)
			{
				result = -4; __leave;
			}
			break;
		case PHOTOMETRIC_RGB: 
			if (colorchannels < 3)
			{
				result = -4; __leave;
			}
			break;
#ifdef CMYK_SUPPORT
		case PHOTOMETRIC_SEPARATED:
			if (td->td_inkset != INKSET_CMYK)
			{
				result = -4; __leave;
			}
			if (td->td_samplesperpixel < 4)
			{
				result = -4; __leave;
			}
			break;
#endif
		case PHOTOMETRIC_LOGL:
			if (td->td_compression != COMPRESSION_SGILOG)
			{
				result = -4; __leave;
			}
			break;
		case PHOTOMETRIC_LOGLUV:
			if (td->td_compression != COMPRESSION_SGILOG &&
				td->td_compression != COMPRESSION_SGILOG24)
			{
				result = -4; __leave;
			}
			if (td->td_planarconfig != PLANARCONFIG_CONTIG)
			{
				result = -4; __leave;
			}
			break;
		default:
			result = -3; __leave;
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			result = -5;		/* 读写错 */
		}
	}

    return result;	/* 返回 0 成功，返回非0表示图象不能被处理 */
}


/* 结束RGBA转换过程，释放img中的内存资源 */
void tiff_rgba_image_end(ISE_TIFFRGBAImage* img)
{
	assert(img);

    if (img->ycbcr)
		_TIFFfree(img->ycbcr), img->ycbcr = 0;

    if( img->redcmap )
	{
        _TIFFfree( img->redcmap );   img->redcmap = 0;
        _TIFFfree( img->greencmap ); img->greencmap = 0;
        _TIFFfree( img->bluecmap );  img->bluecmap = 0;
    }
}


/* 开始一个RGBA转换过程 */
int tiff_rgba_image_begin(ISE_TIFFRGBAImage* img, TIFF* tif, int stop, int type, void* pinfo)
{
    uint16		*sampleinfo;
    uint16		extrasamples; 
    uint16		planarconfig;
    uint16		compress;
    int			colorchannels, i, j;
    uint16		*red_orig, *green_orig, *blue_orig;
    int			n_color;
	int			result = 0;


	assert(img);
	assert(tif);

	__try
	{
		/* 初始化为正常值 */
		img->row_offset = 0;
		img->col_offset = 0;
		img->redcmap    = 0;
		img->greencmap  = 0;
		img->bluecmap   = 0;
		img->tif = tif;
		img->stoponerr  = stop;
		img->info_type  = type;
		img->ycbcr_mark = 0;

		if (type == 0)
		{
			img->ise.pinfo_str = (LPINFOSTR)pinfo;
		}
		else
		{
			img->ise.psub_str = (LPSUBIMGBLOCK)pinfo;
		}

		TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &img->bitspersample);
		TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &img->samplesperpixel);
		TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo);

		/* 设定ALPHA值 */
		img->alpha = 0;

		if (extrasamples == 1)
		{
			switch (sampleinfo[0])
			{
			case EXTRASAMPLE_ASSOCALPHA:
			case EXTRASAMPLE_UNASSALPHA:
				img->alpha = sampleinfo[0];
				break;
			}
		}

#if DEFAULT_EXTRASAMPLE_AS_ALPHA == 1
		if( !TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &img->photometric))
			img->photometric = PHOTOMETRIC_MINISWHITE;

		if( extrasamples == 0 && img->samplesperpixel == 4 && img->photometric == PHOTOMETRIC_RGB )
		{
			img->alpha = EXTRASAMPLE_ASSOCALPHA;
			extrasamples = 1;
		}
#endif

		/* 计算颜色通道数目 */
		colorchannels = img->samplesperpixel - extrasamples;

		TIFFGetFieldDefaulted(tif, TIFFTAG_COMPRESSION, &compress);
		TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &planarconfig);

		if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &img->photometric))
		{
			/* 设定图象所用色系 */
			switch (colorchannels)
			{
			case 1:
				if (is_ccitt_compression(tif))
					img->photometric = PHOTOMETRIC_MINISWHITE;
				else
					img->photometric = PHOTOMETRIC_MINISBLACK;
				break;
			case 3:
				img->photometric = PHOTOMETRIC_RGB;
				break;
			default:
				result = -3; __leave;	/* 无法确定图象所用色系 */
			}
		}

		/* 调色板计数设初值 */
		if (img->info_type == 0)	/* 主图象结构 */
		{
			img->ise.pinfo_str->pal_count = 0;
		}
		else
		{
			img->ise.psub_str->pal_count = 0;
		}
		
		switch (img->photometric)
		{
		case PHOTOMETRIC_PALETTE:
			if (img->samplesperpixel != 1)
			{
				result = -4; __leave;	/* 不能支持的图象存储格式 */
			}

			/* 读取调色板数据 */
			if (!TIFFGetField(tif, TIFFTAG_COLORMAP, &red_orig, &green_orig, &blue_orig))
			{
				result = -5; __leave;	/* 图象中没有必须的调色板数据 */
			}

			n_color = (1L << img->bitspersample);

			assert((img->redcmap==0) && (img->greencmap==0) && (img->bluecmap==0));

			/* 提取图象中的分离型调色板数据 */
			img->redcmap   = (uint16 *) _TIFFmalloc(sizeof(uint16)*n_color);
			img->greencmap = (uint16 *) _TIFFmalloc(sizeof(uint16)*n_color);
			img->bluecmap  = (uint16 *) _TIFFmalloc(sizeof(uint16)*n_color);

			if( !img->redcmap || !img->greencmap || !img->bluecmap )
			{
				result = -6; __leave;	/* 内存不足 */
			}

			_TIFFmemcpy(img->redcmap, red_orig, n_color * 2);
			_TIFFmemcpy(img->greencmap, green_orig, n_color * 2);
			_TIFFmemcpy(img->bluecmap, blue_orig, n_color * 2);
        
			/* 将16位调色板转为8位，并将其存储到数据包中 */
			if (img->info_type == 0)	/* 主图象结构 */
			{
				img->ise.pinfo_str->pal_count = n_color;
				for (i=0;i<n_color;i++)
				{
					((RGBQUAD*)(&img->ise.pinfo_str->palette[i]))->rgbRed   = (unsigned char)((((unsigned short)img->redcmap[i])>>8)&0xff);
					((RGBQUAD*)(&img->ise.pinfo_str->palette[i]))->rgbGreen = (unsigned char)((((unsigned short)img->greencmap[i])>>8)&0xff);
					((RGBQUAD*)(&img->ise.pinfo_str->palette[i]))->rgbBlue  = (unsigned char)((((unsigned short)img->bluecmap[i])>>8)&0xff);
					((RGBQUAD*)(&img->ise.pinfo_str->palette[i]))->rgbReserved = (unsigned char)0;
				}
			}
			else						/* 子图象结构 */
			{
				img->ise.psub_str->pal_count = n_color;
				for (i=0;i<n_color;i++)
				{
					((RGBQUAD*)(&img->ise.psub_str->palette[i]))->rgbRed   = (unsigned char)((((unsigned short)img->redcmap[i])>>8)&0xff);
					((RGBQUAD*)(&img->ise.psub_str->palette[i]))->rgbGreen = (unsigned char)((((unsigned short)img->greencmap[i])>>8)&0xff);
					((RGBQUAD*)(&img->ise.psub_str->palette[i]))->rgbBlue  = (unsigned char)((((unsigned short)img->bluecmap[i])>>8)&0xff);
					((RGBQUAD*)(&img->ise.psub_str->palette[i]))->rgbReserved = (unsigned char)0;
				}
			}
			break;
		case PHOTOMETRIC_MINISWHITE:
		case PHOTOMETRIC_MINISBLACK:
			if (img->samplesperpixel != 1)
			{
				result = -4; __leave;	/* 不能支持的图象存储格式 */
			}

			n_color = (1L << img->bitspersample);
			j = 256/n_color;
			
			/* 创建灰度图调色板 */
			if (img->info_type == 0)	/* 主图象结构 */
			{
				img->ise.pinfo_str->pal_count = n_color;
				
				if (img->photometric == PHOTOMETRIC_MINISWHITE)	/* 反相灰度图 */
				{
					for (i=0;i<n_color;i++)
					{
						img->ise.pinfo_str->palette[i] = (((n_color-i-1)*j)<<16)|(((n_color-i-1)*j)<<8)|((n_color-i-1)*j);
					}
					img->ise.pinfo_str->palette[0] = 0xffffff;	/* 修正白色数据 */
				}
				else											/* 正向灰度图 */
				{
					for (i=0;i<n_color;i++)
					{
						img->ise.pinfo_str->palette[i] = ((i*j)<<16)|((i*j)<<8)|(i*j);
					}
					img->ise.pinfo_str->palette[n_color-1] = 0xffffff;
				}
			}
			else						/* 子图象结构 */
			{
				img->ise.psub_str->pal_count = n_color;
				
				if (img->photometric == PHOTOMETRIC_MINISWHITE)
				{
					for (i=0;i<n_color;i++)
					{
						img->ise.psub_str->palette[i] = (((n_color-i-1)*j)<<16)|(((n_color-i-1)*j)<<8)|((n_color-i-1)*j);
					}
					img->ise.psub_str->palette[0] = 0xffffff;
				}
				else
				{
					for (i=0;i<n_color;i++)
					{
						img->ise.psub_str->palette[i] = ((i*j)<<16)|((i*j)<<8)|(i*j);
					}
					img->ise.psub_str->palette[n_color-1] = 0xffffff;
				}
			}
			break;
		case PHOTOMETRIC_YCBCR:
			if (planarconfig != PLANARCONFIG_CONTIG)
			{
				result = -4; __leave;
			}
			switch (compress)
			{
			case COMPRESSION_OJPEG:
			case COMPRESSION_JPEG:
				TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
				img->photometric = PHOTOMETRIC_RGB;
				img->ycbcr_mark  = 1;
				break;
			default:
				break;
			}
			break;
		case PHOTOMETRIC_RGB: 
			if (colorchannels < 3)
			{
				result = -4; __leave;
			}
			/* 修正BUG。本插件保存的JPEG压缩TIFF图象在读出时R、B分量颠倒，在此修正。*/
			/* ：JPEG压缩方式保存图象时，色系仍然为RGB，而不是YCBCR，所以必须添加如 */
			/*   下代码。*/
			/* 此问题由 leye 发现并提出。YZ 修改。2002-5 */

			/* >>>>>>>>>>>> 以下为新添加的代码：*/
			switch (compress)
			{
			case COMPRESSION_OJPEG:
			case COMPRESSION_JPEG:
				TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
				img->ycbcr_mark  = 1;
				break;
			default:
				break;
			}
			/* <<<<<<<<<<< 添加结束 */
			break;
		case PHOTOMETRIC_SEPARATED:
			{
				uint16 inkset;

				TIFFGetFieldDefaulted(tif, TIFFTAG_INKSET, &inkset);

				if (inkset != INKSET_CMYK)
				{
					result = -4; __leave;
				}
				if (img->samplesperpixel < 4)
				{
					result = -4; __leave;
				}
				break;
			}
		case PHOTOMETRIC_LOGL:
			if (compress != COMPRESSION_SGILOG)
			{
				result = -4; __leave;
			}

			TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_8BIT);
			img->photometric = PHOTOMETRIC_MINISBLACK;
			img->bitspersample = 8;
			break;
		case PHOTOMETRIC_LOGLUV:
			if (compress != COMPRESSION_SGILOG && compress != COMPRESSION_SGILOG24)
			{
				result = -4; __leave;
			}

			if (planarconfig != PLANARCONFIG_CONTIG)
			{
				result = -4; __leave;
			}

			TIFFSetField(tif, TIFFTAG_SGILOGDATAFMT, SGILOGDATAFMT_8BIT);

			img->photometric = PHOTOMETRIC_RGB;
			img->bitspersample = 8;
			break;
		default:
			result = -3; __leave;
		}
		

		img->ycbcr = 0;

		/* 获取图象宽、高及方向信息 */
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &img->width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &img->height);
		TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &img->orientation);

		img->isContig =	!(planarconfig == PLANARCONFIG_SEPARATE && colorchannels > 1);

		/* 选定解码函数 */
		if (img->isContig)	/* 紧凑型 */
		{
			img->get = TIFFIsTiled(tif) ? get_tile_contig : get_strip_contig;
			if (sel_tile_contig_case(img) == 0)
			{
				result = -8;/* 找不到合适的解码函数 */
			}
		}
		else				/* 分离型 */
		{
			img->get = TIFFIsTiled(tif) ? get_tile_separate : get_strip_separate;
			if (sel_tile_separate_case(img) == 0)
			{
				result = -8;
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			result = -7;	/* 读写错 */
		}
	}
	
    return result;
	/* 返回 0 表示成功，非 0 值表示失败 */
}


/* 获取图象象素数据 */
int tiff_rgba_image_get(ISE_TIFFRGBAImage* img, uint32 w, uint32 h)
{
	assert(img);
	assert(img->get);
	assert(img->put.any);
	assert(w && h);

    return (*img->get)(img, w, h);
	/* 返回 0 表示成功，非 0 表示错误 */
}


/* 判断TIFF图象是否采用了CCITT压缩格式 */
static int is_ccitt_compression(TIFF* tif)
{
    uint16 compress;
	
    TIFFGetField(tif, TIFFTAG_COMPRESSION, &compress);
	
    return (compress == COMPRESSION_CCITTFAX3 ||
		compress == COMPRESSION_CCITTFAX4 ||
		compress == COMPRESSION_CCITTRLE ||
		compress == COMPRESSION_CCITTRLEW);
}


/* 简化图象组织形式 */
static void setorientation(ISE_TIFFRGBAImage* img)
{
    switch (img->orientation)
	{
    case ORIENTATION_BOTRIGHT:
    case ORIENTATION_RIGHTBOT:
    case ORIENTATION_LEFTBOT:
		/* 视为倒向图 */
		img->orientation = ORIENTATION_BOTLEFT;
    case ORIENTATION_BOTLEFT:
		break;
    case ORIENTATION_TOPRIGHT:
    case ORIENTATION_RIGHTTOP:
    case ORIENTATION_LEFTTOP:
    default:
		/* 视为正向图 */
		img->orientation = ORIENTATION_TOPLEFT;
    case ORIENTATION_TOPLEFT:
		break;
    }
}


/* 四大GET历程之一：获取块状紧凑型图象数据 */
static int get_tile_contig(ISE_TIFFRGBAImage* img, uint32 w, uint32 h)
{
    TIFF	*tif = img->tif;
    ise_tileContigRoutine put = img->put.contig;
    uint16	orientation;
    uint32	col, row, y, rowstoread, ret = 1;
    uint32	pos;
    uint32	tw, th, tile_row_size;
    u_char	*buf = 0, *wrk_line = 0;
    int32	fromskew, toskew;
    uint32	nrow;
	int32	result = 0;

	__try
	{
		if ((buf=(u_char*)_TIFFmalloc(TIFFTileSize(tif))) == 0)
		{
			result = -1; __leave;		/* 内存不足 */
		}

		TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
		TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);

		setorientation(img);

		orientation = img->orientation;
		toskew = -(int32) (orientation == ORIENTATION_TOPLEFT ? tw+w : tw-w);

		for (row = 0, y = 0; row < h; row += nrow)
		{
			rowstoread = th - (row + img->row_offset) % th;
    		nrow = (row + rowstoread > h ? h - row : rowstoread);

			for (col = 0; col < w; col += tw) 
			{
				if (TIFFReadTile(tif, buf, col+img->col_offset, row+img->row_offset, 0, 0) < 0 && img->stoponerr)
				{
					result = -2; __leave;	/* 读写错误 */
				}

				tile_row_size = TIFFTileRowSize(tif);
				pos = ((row+img->row_offset) % th) * tile_row_size;

				if(orientation == ORIENTATION_BOTLEFT)
				{
					u_char *top_line, *bottom_line;
					uint32 t_row;

					assert(wrk_line == 0);

					if ((wrk_line=(u_char*)_TIFFmalloc(tile_row_size)) == 0)
					{
						result = -1; __leave;		/* 内存不足 */
					}

					for(t_row = 0; t_row < th / 2; t_row++)
					{
						top_line = buf + tile_row_size * t_row;
						bottom_line = buf + tile_row_size * (th-t_row-1);

						_TIFFmemcpy(wrk_line, top_line, tile_row_size);
						_TIFFmemcpy(top_line, bottom_line, tile_row_size);
						_TIFFmemcpy(bottom_line, wrk_line, tile_row_size);
					}

					_TIFFfree(wrk_line); wrk_line = 0;
				}

				/* 展开数据 */
				if (col + tw > w) 
				{
					uint32 npix = w - col;
					fromskew = tw - npix;
					(*put)(img, col, y,	npix, nrow, fromskew, toskew + fromskew, buf + pos);
				}
				else 
				{
					(*put)(img, col, y, tw, nrow, 0, toskew, buf + pos);
				}
			}

			y += (int32) nrow;

			/* 支持用户中断 */
			if (img->info_type == 0)
			{
				if (img->ise.pinfo_str->break_mark)
				{
					result = -3; __leave;
				}
			}
			else
			{
				if (img->ise.psub_str->parents->break_mark)
				{
					result = -3; __leave;
				}
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			result = -2;	/* 读写错 */
		}

		if (wrk_line)
			_TIFFfree(wrk_line), wrk_line = 0;

		if (buf)
			_TIFFfree(buf), buf = 0;
	}

    return (result);
}


/* 四大GET历程之一：获取块状分离型图象数据 */
static int get_tile_separate(ISE_TIFFRGBAImage* img, uint32 w, uint32 h)
{
    TIFF	*tif = img->tif;
    ise_tileSeparateRoutine put = img->put.separate;
    uint16	orientation;
    uint32	col, row, y, rowstoread;

    uint32	pos;
    uint32	tw, th;
    u_char* buf = 0;
    u_char* r;
    u_char* g;
    u_char* b;
    u_char* a;
    tsize_t tilesize;
    int32	fromskew, toskew;
    int		alpha = img->alpha;
    uint32	nrow;
	int32	result = 0;
	
	__try
	{
		tilesize = TIFFTileSize(tif);

		if ((buf=(u_char*) _TIFFmalloc(4*tilesize)) == 0)
		{
			result = -1; __leave;		/* 内存不足 */
		}

		r = buf;
		g = r + tilesize;
		b = g + tilesize;
		a = b + tilesize;

		/* ALPHA位将被初始化为 0 */
		if (!alpha)
			memset(a, 0x0, tilesize);

		TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
		TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);

		/* 简化图象方向标志 */
		setorientation(img);

		orientation = img->orientation;
		toskew = -(int32) (orientation == ORIENTATION_TOPLEFT ? tw+w : tw-w);


		/* 读取数据 */
		for (row = 0, y = 0; row < h; row += nrow) 
		{
			rowstoread = th - (row + img->row_offset) % th;
    		nrow = (row + rowstoread > h ? h - row : rowstoread);

			for (col = 0; col < w; col += tw) 
			{
				if (TIFFReadTile(tif, r, col+img->col_offset, row+img->row_offset,0,0) < 0 && img->stoponerr)
				{
					result = -2; __leave;	/* 读写错误 */
				}
				if (TIFFReadTile(tif, g, col+img->col_offset, row+img->row_offset,0,1) < 0 && img->stoponerr)
				{
					result = -2; __leave;
				}
				if (TIFFReadTile(tif, b, col+img->col_offset, row+img->row_offset,0,2) < 0 && img->stoponerr)
				{
					result = -2; __leave;
				}
				if (alpha && TIFFReadTile(tif,a,col+img->col_offset, row+img->row_offset,0,3) < 0 && img->stoponerr)
				{
					result = -2; __leave;
				}

				pos = ((row+img->row_offset) % th) * TIFFTileRowSize(tif);

				/* 展开数据 */
				if (col + tw > w) 
				{
					uint32 npix = w - col;
					fromskew = tw - npix;
					(*put)(img, col, y, npix, nrow, fromskew, toskew + fromskew, r + pos, g + pos, b + pos, a + pos);
				} 
				else 
				{
					(*put)(img, col, y, tw, nrow, 0, toskew, r + pos, g + pos, b + pos, a + pos);
				}
			}

			y += (int32) nrow;

			/* 支持用户中断 */
			if (img->info_type == 0)
			{
				if (img->ise.pinfo_str->break_mark)
				{
					result = -3; __leave;
				}
			}
			else
			{
				if (img->ise.psub_str->parents->break_mark)
				{
					result = -3; __leave;
				}
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			result = -2;	/* 读写错 */
		}
		
		if (buf)
			_TIFFfree(buf), buf = 0;
	}

    return (result);
}


/* 四大GET历程之一：获取带状紧凑型图象数据 */
static int get_strip_contig(ISE_TIFFRGBAImage* img, uint32 w, uint32 h)
{
    TIFF	*tif = img->tif;
    ise_tileContigRoutine put = img->put.contig;
    uint16	orientation;
    uint32	row, y, nrow, rowstoread;
    uint32	pos;
    u_char	*buf = 0;
    uint32	rowsperstrip;
    uint32	imagewidth = img->width;
    tsize_t scanline;
    int32	fromskew, toskew;
	int32	result = 0;
	
	__try
	{
		if ((buf=(u_char*) _TIFFmalloc(TIFFStripSize(tif))) == 0)
		{
			result = -1; __leave;		/* 内存不足 */
		}

		setorientation(img);

		/* 此处img->orientation只可能是ORIENTATION_TOPLEFT或ORIENTATION_BOTLEFT */
		orientation = img->orientation;
		toskew = -(int32) (orientation == ORIENTATION_TOPLEFT ? w+w : w-w);

		TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);

		scanline = TIFFScanlineSize(tif);
		fromskew = (w < imagewidth ? imagewidth - w : 0);

		/* 读取数据 */
		for (row = 0, y = 0; row < h; row += nrow) 
		{
			rowstoread = rowsperstrip - (row + img->row_offset) % rowsperstrip;
			nrow = (row + rowstoread > h ? h - row : rowstoread);
			if (TIFFReadEncodedStrip(tif,
									 TIFFComputeStrip(tif,row+img->row_offset, 0),
									 buf, 
									 ((row + img->row_offset)%rowsperstrip + nrow) * scanline) < 0 && img->stoponerr)
			{
				result = -2; __leave;		/* 读写错误 */
			}

			pos = ((row + img->row_offset) % rowsperstrip) * scanline;

			(*put)(img, 0, y, w, nrow, fromskew, toskew, buf + pos);

			y += (int32) nrow;

			/* 支持用户中断 */
			if (img->info_type == 0)
			{
				if (img->ise.pinfo_str->break_mark)
				{
					result = -3; __leave;
				}
			}
			else
			{
				if (img->ise.psub_str->parents->break_mark)
				{
					result = -3; __leave;
				}
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			result = -2;	/* 读写错 */
		}
		
		if (buf)
			_TIFFfree(buf), buf = 0;
	}

    return (result);
}


/* 四大GET历程之一：获取带状分离型图象数据 */
static int get_strip_separate(ISE_TIFFRGBAImage* img, uint32 w, uint32 h)
{
    TIFF* tif = img->tif;
    ise_tileSeparateRoutine put = img->put.separate;
    uint16 orientation;
    u_char *buf = 0;
    u_char *r, *g, *b, *a;
    uint32 row, y, nrow, rowstoread;
    uint32 pos;
    tsize_t scanline;
    uint32 rowsperstrip, offset_row;
    uint32 imagewidth = img->width;
    tsize_t stripsize;
    int32 fromskew, toskew;
    int alpha = img->alpha;
	int32	result = 0;
	
	__try
	{
		stripsize = TIFFStripSize(tif);

		if ((buf=(u_char *)_TIFFmalloc(4*stripsize)) == 0)
		{
			result = -1; __leave;		/* 内存不足 */
		}

		r = buf;
		g = r + stripsize;
		b = g + stripsize;
		a = b + stripsize;

		if (!alpha)
			memset(a, 0xff, stripsize);

		setorientation(img);

		orientation = img->orientation;
		toskew = -(int32) (orientation == ORIENTATION_TOPLEFT ? w+w : w-w);

		TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);

		scanline = TIFFScanlineSize(tif);
		fromskew = (w < imagewidth ? imagewidth - w : 0);

		/* 读取数据 */
		for (row = 0, y = 0; row < h; row += nrow) 
		{
			rowstoread = rowsperstrip - (row + img->row_offset) % rowsperstrip;    	
			nrow = (row + rowstoread > h ? h - row : rowstoread);
			offset_row = row + img->row_offset;

    		if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, offset_row, 0),
									 r, ((row + img->row_offset)%rowsperstrip + nrow) * scanline) < 0 && img->stoponerr)
			{
				result = -2; __leave;		/* 读写错误 */
			}
			if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, offset_row, 1),
									 g, ((row + img->row_offset)%rowsperstrip + nrow) * scanline) < 0 && img->stoponerr)
			{
				result = -2; __leave;
			}
			if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, offset_row, 2),
									 b, ((row + img->row_offset)%rowsperstrip + nrow) * scanline) < 0 && img->stoponerr)
			{
				result = -2; __leave;
			}
			if (alpha &&
				(TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, offset_row, 3),
									  a, ((row + img->row_offset)%rowsperstrip + nrow) * scanline) < 0 && img->stoponerr))
			{
				result = -2; __leave;
			}

			pos = ((row + img->row_offset) % rowsperstrip) * scanline;

			/* 展开数据 */
			(*put)(img, 0, y, w, nrow, fromskew, toskew, r + pos, g + pos, b + pos, a + pos);

			y += (int32) nrow;

			/* 支持用户中断 */
			if (img->info_type == 0)
			{
				if (img->ise.pinfo_str->break_mark)
				{
					result = -3; __leave;
				}
			}
			else
			{
				if (img->ise.psub_str->parents->break_mark)
				{
					result = -3; __leave;
				}
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			result = -2;	/* 读写错 */
		}
		
		if (buf)
			_TIFFfree(buf), buf = 0;
	}

    return (result);
}


/* 计算扫描行长度（以字节计）及象素写入点位置 */
static void _cal_scanline_pdest(ISE_TIFFRGBAImage *img, int x, int y, int *scanline, char **pdest)
{
	assert((scanline)&&(pdest));

	if (img->info_type == 0)
	{
		*scanline = DIBSCANLINE_WIDTHBYTES(img->ise.pinfo_str->bitcount * img->ise.pinfo_str->width);
		*pdest = ((unsigned char *)img->ise.pinfo_str->p_bit_data) + y*(*scanline) + (x*img->ise.pinfo_str->bitcount)/8;
	}
	else
	{
		*scanline = DIBSCANLINE_WIDTHBYTES(img->ise.psub_str->bitcount * img->ise.psub_str->width);
		*pdest = ((unsigned char *)img->ise.psub_str->p_bit_data) + y*(*scanline) + (x*img->ise.psub_str->bitcount)/8;
	}
}



/*****************************************************************************/
/* 以下为解码函数 */
/*****************************************************************************/

/* 调整象素位置，以符合ISee规范 */
#define TIFF_CVT_BITS4(c) \
{\
	c = (unsigned char)((((unsigned char)c)>>4) | (((unsigned char)c)<<4));\
}

#define TIFF_CVT_BITS2(c) \
{\
	c = (unsigned char)( ((((unsigned char)c)>>6)&0x3) | ((((unsigned char)c)>>2)&0xc) | \
						 ((((unsigned char)c)<<2)&0x30)| ((((unsigned char)c)<<6)&0xc0)	);\
}

#define TIFF_CVT_BITS1(c) \
{\
	c = (unsigned char)( ((((unsigned char)c)>>7)&0x1) | ((((unsigned char)c)>>5)&0x2) | \
						 ((((unsigned char)c)>>3)&0x4) | ((((unsigned char)c)>>1)&0x8) | \
						 ((((unsigned char)c)<<1)&0x10)| ((((unsigned char)c)<<3)&0x20)| \
						 ((((unsigned char)c)<<5)&0x40)| ((((unsigned char)c)<<7)&0x80) );\
}

/* 通用紧凑型数据函数原型 */
#define	DECLAREContigPutFunc(name) \
static void name(\
    ISE_TIFFRGBAImage* img, \
    uint32 x, uint32 y, \
    uint32 w, uint32 h, \
    int32 fromskew, int32 toskew, \
    u_char* pp \
)

/*
 * 8-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(put8bitcmaptile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;

	assert(img);

	_cal_scanline_pdest(img, x, y, &scanline, &pdest);

    (void) y;

    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = *pp;
			pp += samplesperpixel;
		}
		pdest += (scanline-w);
		pp += fromskew;
    }
}

/*
 * 4-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(put4bitcmaptile)
{
	int scanline;
	unsigned char *pdest;

    (void) x; (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew /= 2;

    while (h-- > 0)
	{
		unsigned char ch;
		uint32 _x;

		for (_x = w; _x >= 2; _x -= 2)
		{
			ch  = *pp;
			TIFF_CVT_BITS4(ch);	/* 交换象素顺序以符合ISee位流格式 */
			*pdest++ = ch;
			pp++;
		}
		if (_x)
		{
			ch  = (*pp)>>4;
			*pdest &= 0xf0;
			*pdest++ |= ch;
			pp++;
		}

		pdest += (scanline - w/2);
		pp += fromskew;
    }
}

/*
 * 2-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(put2bitcmaptile)
{
	int scanline;
	unsigned char *pdest;

	assert(img);
	
    (void) x; (void) y;

	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew /= 4;

    while (h-- > 0)
	{
		unsigned char ch;
		uint32 _x;

		for (_x = w; _x >= 4; _x -= 4)
		{
			ch = *pp;
			TIFF_CVT_BITS2(ch);
			*pdest++ = ch;
			pp++;
		}
		if (_x > 0)
		{
			ch = *pp++;
			TIFF_CVT_BITS2(ch);
			switch (_x)
			{ 
			case 3: 
				*pdest &= 0xc0;
				*pdest |= (ch&0x3f);
				break;
			case 2:
				*pdest &= 0xf0;
				*pdest |= (ch&0xf);
				break;
			case 1:
				*pdest &= 0xfc;
				*pdest |= (ch&0x3);
				break;
			}
			pdest++;
		}

		pdest += (scanline - w/4);
		pp += fromskew;
    }
}

/*
 * 1-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(put1bitcmaptile)
{
	int scanline;
	unsigned char *pdest;
	
	assert(img);
	
    (void) x; (void) y;

	_cal_scanline_pdest(img, x, y, &scanline, &pdest);

    fromskew /= 8;

    while (h-- > 0)
	{
		unsigned char ch;
		uint32 _x;

		for (_x = w; _x >= 8; _x -= 8)
		{
			ch = *pp;
			TIFF_CVT_BITS1(ch);
			*pdest++ = ch;
			pp++;
		}
		if (_x > 0)
		{
			ch = *pp++;
			TIFF_CVT_BITS1(ch);
			switch (_x)
			{ 
			case 7: 
				*pdest &= 0x80;
				*pdest |= (ch&0x7f);
				break;
			case 6: 
				*pdest &= 0xc0;
				*pdest |= (ch&0x3f);
				break;
			case 5: 
				*pdest &= 0xe0;
				*pdest |= (ch&0x1f);
				break;
			case 4: 
				*pdest &= 0xf0;
				*pdest |= (ch&0xf);
				break;
			case 3: 
				*pdest &= 0xf8;
				*pdest |= (ch&0x7);
				break;
			case 2:
				*pdest &= 0xfc;
				*pdest |= (ch&0x3);
				break;
			case 1:
				*pdest &= 0xfe;
				*pdest |= (ch&0x1);
				break;
			}
			pdest++;
		}
		
		pdest += (scanline - w/8);
		pp += fromskew;
    }
}

/*
 * 8-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(putgreytile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;

    (void) y;

	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = *pp;
			pp += samplesperpixel;
		}
		pdest += (scanline-w);
		pp += fromskew;
    }
}

/*
 * 16-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(put16bitbwtile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;

    (void) y;

	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
        {
			*pdest++ = *pp;
            pp += 2 * samplesperpixel;
        }
		pdest += (scanline-w);
		pp += fromskew;
    }
}

/*
 * 1-bit bilevel => colormap/RGB
 */
DECLAREContigPutFunc(put1bitbwtile)
{
	int scanline;
	unsigned char *pdest;
	
	assert(img);
	
    (void) x; (void) y;
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew /= 8;
	
    while (h-- > 0)
	{
		unsigned char ch;
		uint32 _x;
		
		for (_x = w; _x >= 8; _x -= 8)
		{
			ch = *pp;
			TIFF_CVT_BITS1(ch);
			*pdest++ = ch;
			pp++;
		}
		if (_x > 0)
		{
			ch = *pp++;
			TIFF_CVT_BITS1(ch);
			switch (_x)
			{ 
			case 7: 
				*pdest &= 0x80;
				*pdest |= (ch&0x7f);
				break;
			case 6: 
				*pdest &= 0xc0;
				*pdest |= (ch&0x3f);
				break;
			case 5: 
				*pdest &= 0xe0;
				*pdest |= (ch&0x1f);
				break;
			case 4: 
				*pdest &= 0xf0;
				*pdest |= (ch&0xf);
				break;
			case 3: 
				*pdest &= 0xf8;
				*pdest |= (ch&0x7);
				break;
			case 2:
				*pdest &= 0xfc;
				*pdest |= (ch&0x3);
				break;
			case 1:
				*pdest &= 0xfe;
				*pdest |= (ch&0x1);
				break;
			}
			pdest++;
		}
		
		pdest += (scanline - w/8);
		pp += fromskew;
    }
}

/*
 * 2-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(put2bitbwtile)
{
	int scanline;
	unsigned char *pdest;
	
	assert(img);
	
    (void) x; (void) y;
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew /= 4;
	
    while (h-- > 0)
	{
		unsigned char ch;
		uint32 _x;
		
		for (_x = w; _x >= 4; _x -= 4)
		{
			ch = *pp;
			TIFF_CVT_BITS2(ch);
			*pdest++ = ch;
			pp++;
		}
		if (_x > 0)
		{
			ch = *pp++;
			TIFF_CVT_BITS2(ch);
			switch (_x)
			{ 
			case 3: 
				*pdest &= 0xc0;
				*pdest |= (ch&0x3f);
				break;
			case 2:
				*pdest &= 0xf0;
				*pdest |= (ch&0xf);
				break;
			case 1:
				*pdest &= 0xfc;
				*pdest |= (ch&0x3);
				break;
			}
			pdest++;
		}
		
		pdest += (scanline - w/4);
		pp += fromskew;
    }
}

/*
 * 4-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(put4bitbwtile)
{
	int scanline;
	unsigned char *pdest;
	
    (void) x; (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew /= 2;
	
    while (h-- > 0)
	{
		unsigned char ch;
		uint32 _x;
		
		for (_x = w; _x >= 2; _x -= 2)
		{
			ch  = *pp;
			TIFF_CVT_BITS4(ch);	/* 交换象素顺序以符合ISee位流格式 */
			*pdest++ = ch;
			pp++;
		}
		if (_x)
		{
			ch  = (*pp)>>4;
			*pdest &= 0xf0;
			*pdest++ |= ch;
			pp++;
		}
		
		pdest += (scanline - w/2);
		pp += fromskew;
    }
}

/*
 * 8-bit packed samples, no Map => RGB
 */
DECLAREContigPutFunc(putRGBcontig8bittile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;

    (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);

    fromskew *= samplesperpixel;

    while (h-- > 0)
	{
		if (img->ycbcr_mark == 0)
		{
			for (x = w; x-- > 0;)
			{
				*pdest++ = pp[2]; *pdest++ = pp[1]; *pdest++ = pp[0]; pp += samplesperpixel;
			}
		}
		else
		{
			for (x = w; x-- > 0;)
			{
				*pdest++ = pp[0]; *pdest++ = pp[1]; *pdest++ = pp[2]; pp += samplesperpixel;
			}
		}
		
		pdest += (scanline - w*3);
		pp += fromskew;
    }
}

/*
 * 8-bit packed samples, w/ Map => RGB
 */
DECLAREContigPutFunc(putRGBcontig8bitMaptile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;

    (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);

    fromskew *= samplesperpixel;

    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = pp[2]; *pdest++ = pp[1]; *pdest++ = pp[0]; pp += samplesperpixel;
		}

		pdest += (scanline - w*3);
		pp += fromskew;
    }
}

/*
 * 8-bit packed samples => RGBA w/ associated alpha
 * (known to have Map == 0)
 */
DECLAREContigPutFunc(putRGBAAcontig8bittile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;

    (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);

    fromskew *= samplesperpixel;

    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = pp[2]; *pdest++ = pp[1];		/* BGRA */ 
			*pdest++ = pp[0]; *pdest++ = pp[3];
			pp += samplesperpixel;
		}
		
		pdest += (scanline - w*4);
		pp += fromskew;
    }
}

/*
 * 8-bit packed samples => RGBA w/ unassociated alpha
 * (known to have Map == 0)
 */
DECLAREContigPutFunc(putRGBUAcontig8bittile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;
	
    (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew *= samplesperpixel;
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = pp[2]; *pdest++ = pp[1];		/* BGRA */ 
			*pdest++ = pp[0]; *pdest++ = pp[3];		/* 生加工 :)，不进行ALPHA混合处理 */
			pp += samplesperpixel;
		}
		
		pdest += (scanline - w*4);
		pp += fromskew;
    }
}

/*
 * 16-bit packed samples => RGB
 */
DECLAREContigPutFunc(putRGBcontig16bittile)
{
	assert(0);			/* 不支持16位采样点 */
}

/*
 * 16-bit packed samples => RGBA w/ associated alpha
 * (known to have Map == 0)
 */
DECLAREContigPutFunc(putRGBAAcontig16bittile)
{
	assert(0);			/* 不支持16位采样点 */
}

/*
 * 16-bit packed samples => RGBA w/ unassociated alpha
 * (known to have Map == 0)
 */
DECLAREContigPutFunc(putRGBUAcontig16bittile)
{
	assert(0);			/* 不支持16位采样点 */
}

/*
 * 8-bit packed CMYK samples w/o Map => RGB
 *
 * NB: The conversion of CMYK->RGB is *very* crude.
 */
DECLAREContigPutFunc(putRGBcontig8bitCMYKtile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;
    uint16 r, g, b, k;

    (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew *= samplesperpixel;

    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			k = 255 - pp[3];
			r = (k*(255-pp[0]))/255;
			g = (k*(255-pp[1]))/255;
			b = (k*(255-pp[2]))/255;
			*pdest++ = (unsigned char)b;
			*pdest++ = (unsigned char)g;
			*pdest++ = (unsigned char)r;
			pp += samplesperpixel;
		}

		pdest += (scanline - w*3);
		pp += fromskew;
    }
}

/*
 * 8-bit packed CMYK samples w/Map => RGB
 *
 * NB: The conversion of CMYK->RGB is *very* crude.
 */
DECLAREContigPutFunc(putRGBcontig8bitCMYKMaptile)
{
	int scanline;
	unsigned char *pdest;
    int samplesperpixel = img->samplesperpixel;
    uint16 r, g, b, k;

    (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    fromskew *= samplesperpixel;

    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			k = 255 - pp[3];
			r = (k*(255-pp[0]))/255;
			g = (k*(255-pp[1]))/255;
			b = (k*(255-pp[2]))/255;
			*pdest++ = (unsigned char)b;
			*pdest++ = (unsigned char)g;
			*pdest++ = (unsigned char)r;
			pp += samplesperpixel;
		}
		
		pdest += (scanline - w*3);
		pp += fromskew;
    }
}

/* 通用分离型数据函数原型 */
#define	DECLARESepPutFunc(name) \
static void name(\
    ISE_TIFFRGBAImage* img,\
    uint32 x, uint32 y, \
    uint32 w, uint32 h,\
    int32 fromskew, int32 toskew,\
    u_char* r, u_char* g, u_char* b, u_char* a\
)

/*
 * 8-bit unpacked samples => RGB
 */
DECLARESepPutFunc(putRGBseparate8bittile)
{
	int scanline;
	unsigned char *pdest;

    (void) img; (void) y; (void) a;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = *b++;
			*pdest++ = *g++;
			*pdest++ = *r++;
		}
		r += fromskew;
		g += fromskew;
		b += fromskew;
		pdest += (scanline - w*3);
    }
}

/*
 * 8-bit unpacked samples => RGB
 */
DECLARESepPutFunc(putRGBseparate8bitMaptile)
{
	int scanline;
	unsigned char *pdest;

    (void) y; (void) a;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = *b++;
			*pdest++ = *g++;
			*pdest++ = *r++;
		}
		r += fromskew;
		g += fromskew;
		b += fromskew;
		pdest += (scanline - w*3);
    }
}

/*
 * 8-bit unpacked samples => RGBA w/ associated alpha
 */
DECLARESepPutFunc(putRGBAAseparate8bittile)
{
	int scanline;
	unsigned char *pdest;
	
    (void) img; (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = *b++;
			*pdest++ = *g++;
			*pdest++ = *r++;
			*pdest++ = *a++;
		}
		r += fromskew;
		g += fromskew;
		b += fromskew;
		a += fromskew;
		pdest += (scanline - w*4);
    }
}

/*
 * 8-bit unpacked samples => RGBA w/ unassociated alpha
 */
DECLARESepPutFunc(putRGBUAseparate8bittile)
{
	int scanline;
	unsigned char *pdest;
	
    (void) img; (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
    while (h-- > 0)
	{
		for (x = w; x-- > 0;)
		{
			*pdest++ = *b++;
			*pdest++ = *g++;
			*pdest++ = *r++;
			*pdest++ = *a++;
		}
		r += fromskew;
		g += fromskew;
		b += fromskew;
		a += fromskew;
		pdest += (scanline - w*4);
    }
}

/*
 * 16-bit unpacked samples => RGB
 */
DECLARESepPutFunc(putRGBseparate16bittile)
{
	assert(0);	/* 不支持16位采样点位深度 */
}

/*
 * 16-bit unpacked samples => RGBA w/ associated alpha
 */
DECLARESepPutFunc(putRGBAAseparate16bittile)
{
	assert(0);
}

/*
 * 16-bit unpacked samples => RGBA w/ unassociated alpha
 */
DECLARESepPutFunc(putRGBUAseparate16bittile)
{
	assert(0);
}


/* YCBCR转RGB宏定义 */
#define _YCBCR_RGB_PD0(qp) \
	Y = pp[qp];	\
	*pd0++ = clamptab[Y+Cbbtab[Cb]];	\
	*pd0++ = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];	\
	*pd0++ = clamptab[Y+Crrtab[Cr]]
#define _YCBCR_RGB_PD1(qp) \
	Y = pp[qp];	\
	*pd1++ = clamptab[Y+Cbbtab[Cb]];	\
	*pd1++ = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];	\
	*pd1++ = clamptab[Y+Crrtab[Cr]]
#define _YCBCR_RGB_PD2(qp) \
	Y = pp[qp];	\
	*pd2++ = clamptab[Y+Cbbtab[Cb]];	\
	*pd2++ = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];	\
	*pd2++ = clamptab[Y+Crrtab[Cr]]
#define _YCBCR_RGB_PD3(qp) \
	Y = pp[qp];	\
	*pd3++ = clamptab[Y+Cbbtab[Cb]];	\
	*pd3++ = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];	\
	*pd3++ = clamptab[Y+Crrtab[Cr]]


/*
 * 8-bit packed YCbCr samples w/ 4,4 subsampling => RGB
 */
DECLAREContigPutFunc(putcontig8bitYCbCr44tile)
{
    TIFFYCbCrToRGB* ycbcr = img->ycbcr;
	int* Crrtab = ycbcr->Cr_r_tab;
	int* Cbbtab = ycbcr->Cb_b_tab;
	int32* Crgtab = ycbcr->Cr_g_tab;
	int32* Cbgtab = ycbcr->Cb_g_tab;
	TIFFRGBValue* clamptab = ycbcr->clamptab;

	int scanline;
	unsigned char *pdest, *pd0, *pd1, *pd2, *pd3;
	
    (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);

	pd0 = pdest;
	pd1 = pd0+scanline;
	pd2 = pd1+scanline;
	pd3 = pd2+scanline;
	
    fromskew = (fromskew * 18) / 4;

    if ((h & 3) == 0 && (w & 3) == 0)
	{
        for (; h >= 4; h -= 4)
		{
            x = w>>2;
            do {
                int Cb = pp[16];
                int Cr = pp[17];
				int Y;

				_YCBCR_RGB_PD0(0);
				_YCBCR_RGB_PD0(1);
				_YCBCR_RGB_PD0(2);
				_YCBCR_RGB_PD0(3);

				_YCBCR_RGB_PD1(4);
				_YCBCR_RGB_PD1(5);
				_YCBCR_RGB_PD1(6);
				_YCBCR_RGB_PD1(7);
				
				_YCBCR_RGB_PD2(8);
				_YCBCR_RGB_PD2(9);
				_YCBCR_RGB_PD2(10);
				_YCBCR_RGB_PD2(11);
				
				_YCBCR_RGB_PD3(12);
				_YCBCR_RGB_PD3(13);
				_YCBCR_RGB_PD3(14);
				_YCBCR_RGB_PD3(15);
				
                pp += 18;
            } while (--x);
			pd0 = pd3+scanline-(w*3);
			pd1 = pd0+scanline;
			pd2 = pd1+scanline;
			pd3 = pd2+scanline;
            pp += fromskew;
        }
    }
	else
	{
        while (h > 0)
		{
            for (x = w; x > 0;)
			{
                int Cb = pp[16];
                int Cr = pp[17];
				int Y;

                switch (x)
				{
                default:
                    switch (h)
					{
					default:
						Y = pp[15];
						*(pd3+3*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd3+3*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd3+3*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 3:
						Y = pp[11];
						*(pd2+3*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd2+3*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd2+3*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 2:
						Y = pp[7];
						*(pd1+3*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd1+3*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+3*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[3];
						*(pd0+3*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd0+3*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+3*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 3:
                    switch (h)
					{
                    default:
						Y = pp[14];
						*(pd3+2*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd3+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd3+2*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 3:
						Y = pp[10];
						*(pd2+2*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd2+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd2+2*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 2:
						Y = pp[6];
						*(pd1+2*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd1+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+2*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[2];
						*(pd0+2*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd0+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+2*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 2:
                    switch (h)
					{
                    default:
						Y = pp[13];
						*(pd3+1*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd3+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd3+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 3:
						Y = pp[9];
						*(pd2+1*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd2+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd2+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 2:
						Y = pp[5];
						*(pd1+1*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd1+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[1];
						*(pd0+1*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd0+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 1:
                    switch (h)
					{
                    default:
						Y = pp[12];
						*(pd3+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd3+2) = clamptab[Y+Crrtab[Cr]];
                    case 3:
						Y = pp[8];
						*(pd2+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd2+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd2+2) = clamptab[Y+Crrtab[Cr]];
                    case 2:
						Y = pp[4];
						*(pd1+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd1+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[0];
						*(pd0+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd0+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+2) = clamptab[Y+Crrtab[Cr]];
                    }
                }
                if (x < 4)
				{
					pd0 += 3*x; pd1 += 3*x; pd2 += 3*x; pd3 += 3*x; x = 0;
                }
                else
				{
					pd0 += 3*4; pd1 += 3*4; pd2 += 3*4; pd3 += 3*4; x -= 4;
                }
                pp += 18;
            }
            if (h <= 4)
                break;
            h -= 4;

			pd0 = pd3+scanline-(w*3);
			pd1 = pd0+scanline;
			pd2 = pd1+scanline;
			pd3 = pd2+scanline;
            pp += fromskew;
        }
    }
}

/*
 * 8-bit packed YCbCr samples w/ 4,2 subsampling => RGB
 */
DECLAREContigPutFunc(putcontig8bitYCbCr42tile)
{
    TIFFYCbCrToRGB* ycbcr = img->ycbcr;
	int* Crrtab = ycbcr->Cr_r_tab;
	int* Cbbtab = ycbcr->Cb_b_tab;
	int32* Crgtab = ycbcr->Cr_g_tab;
	int32* Cbgtab = ycbcr->Cb_g_tab;
	TIFFRGBValue* clamptab = ycbcr->clamptab;
	
	int scanline;
	unsigned char *pdest, *pd0, *pd1;
	
    (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
	pd0 = pdest;
	pd1 = pd0+scanline;
	
    fromskew = (fromskew * 10) / 4;

    if ((h & 3) == 0 && (w & 1) == 0)		/* ??!! BUG-> if ((h&1)==0 && (w&3)==0) */
	{
        for (; h >= 2; h -= 2)
		{
            x = w>>2;
            do
			{
                int Cb = pp[8];
                int Cr = pp[9];
				int Y;
				
				_YCBCR_RGB_PD0(0);
				_YCBCR_RGB_PD0(1);
				_YCBCR_RGB_PD0(2);
				_YCBCR_RGB_PD0(3);
				
				_YCBCR_RGB_PD1(4);
				_YCBCR_RGB_PD1(5);
				_YCBCR_RGB_PD1(6);
				_YCBCR_RGB_PD1(7);
				
                pp += 10;
            } while (--x);
			pd0 = pd1+scanline-(w*3);
			pd1 = pd0+scanline;
            pp += fromskew;
        }
    }
	else
	{
        while (h > 0)
		{
            for (x = w; x > 0;)
			{
                int Cb = pp[8];
                int Cr = pp[9];
				int Y;

                switch (x)
				{
                default:
                    switch (h)
					{
                    default:
						Y = pp[7];
						*(pd1+3*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd1+3*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+3*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[3];
						*(pd0+3*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd0+3*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+3*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 3:
                    switch (h)
					{
                    default:
						Y = pp[6];
						*(pd1+2*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd1+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+2*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[2];
						*(pd0+2*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd0+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+2*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 2:
                    switch (h)
					{
                    default:
						Y = pp[5];
						*(pd1+1*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd1+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[1];
						*(pd0+1*3)   = clamptab[Y+Cbbtab[Cb]];
						*(pd0+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 1:
                    switch (h)
					{
                    default:
						Y = pp[4];
						*(pd1+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd1+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[0];
						*(pd0+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd0+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+2) = clamptab[Y+Crrtab[Cr]];
                    }
                }
                if (x < 4)
				{
					pd0 += 3*x; pd1 += 3*x; x = 0;
                }
                else
				{
					pd0 += 3*4; pd1 += 3*4; x -= 4;
                }
                pp += 10;
            }
            if (h <= 2)
                break;
            h -= 2;

			pd0 = pd1+scanline-(w*3);
			pd1 = pd0+scanline;
            pp += fromskew;
        }
    }
}

/*
 * 8-bit packed YCbCr samples w/ 4,1 subsampling => RGB
 */
DECLAREContigPutFunc(putcontig8bitYCbCr41tile)
{
    TIFFYCbCrToRGB* ycbcr = img->ycbcr;
	int* Crrtab = ycbcr->Cr_r_tab;
	int* Cbbtab = ycbcr->Cb_b_tab;
	int32* Crgtab = ycbcr->Cr_g_tab;
	int32* Cbgtab = ycbcr->Cb_g_tab;
	TIFFRGBValue* clamptab = ycbcr->clamptab;
	
	int scanline;
	unsigned char *pdest, *pd0;
	
    (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
	pd0 = pdest;
	
    do
	{
		x = w>>2;
		do
		{
		    int Cb = pp[4];
			int Cr = pp[5];
			int Y;

			_YCBCR_RGB_PD0(0);
			_YCBCR_RGB_PD0(1);
			_YCBCR_RGB_PD0(2);
			_YCBCR_RGB_PD0(3);

			pp += 6;
		} while (--x);

        if( (w&3) != 0 )
        {
			int Cb = pp[4];
			int Cr = pp[5];
			int Y;

            switch( (w&3) )
			{
			case 3:
				Y = pp[2];
				*(pd0+2*3+0) = clamptab[Y+Cbbtab[Cb]];
				*(pd0+2*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
				*(pd0+2*3+2) = clamptab[Y+Crrtab[Cr]];
			case 2:
				Y = pp[1];
				*(pd0+1*3+0) = clamptab[Y+Cbbtab[Cb]];
				*(pd0+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
				*(pd0+1*3+2) = clamptab[Y+Crrtab[Cr]];
			case 1:
				Y = pp[0];
				*(pd0+0) = clamptab[Y+Cbbtab[Cb]];
				*(pd0+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
				*(pd0+2) = clamptab[Y+Crrtab[Cr]];
			case 0: break;
            }

			pd0 += (w&3)*3;
            pp += 6;
        }

		pd0 = pd0+scanline-(w*3);
		pp += fromskew;
    } while (--h);
}

/*
 * 8-bit packed YCbCr samples w/ 2,2 subsampling => RGB
 */
DECLAREContigPutFunc(putcontig8bitYCbCr22tile)
{
    TIFFYCbCrToRGB* ycbcr = img->ycbcr;
	int* Crrtab = ycbcr->Cr_r_tab;
	int* Cbbtab = ycbcr->Cb_b_tab;
	int32* Crgtab = ycbcr->Cr_g_tab;
	int32* Cbgtab = ycbcr->Cb_g_tab;
	TIFFRGBValue* clamptab = ycbcr->clamptab;
	
	int scanline;
	unsigned char *pdest, *pd0, *pd1;

    (void) y;

	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
	pd0 = pdest;
	pd1 = pd0+scanline;
	
    fromskew = (fromskew * 6) / 2;

    if ((h & 1) == 0 && (w & 1) == 0)
	{
        for (; h >= 2; h -= 2)
		{
            x = w>>1;
            do
			{
                int Cb = pp[4];
                int Cr = pp[5];
				int Y;

				_YCBCR_RGB_PD0(0);
				_YCBCR_RGB_PD0(1);
				_YCBCR_RGB_PD1(2);
				_YCBCR_RGB_PD1(3);

                pp += 6;
            } while (--x);
			pd0 = pd1+scanline-(w*3);
			pd1 = pd0+scanline;
            pp += fromskew;
        }
    }
	else
	{
        while (h > 0)
		{
            for (x = w; x > 0;)
			{
                int Cb = pp[4];
                int Cr = pp[5];
				int Y;

                switch (x)
				{
                default:
                    switch (h)
					{
                    default:
						Y = pp[3];
						*(pd1+1*3+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd1+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[1];
						*(pd0+1*3+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd0+1*3+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+1*3+2) = clamptab[Y+Crrtab[Cr]];
                    }
                case 1:
                    switch (h)
					{
                    default:
						Y = pp[2];
						*(pd1+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd1+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd1+2) = clamptab[Y+Crrtab[Cr]];
                    case 1:
						Y = pp[0];
						*(pd0+0) = clamptab[Y+Cbbtab[Cb]];
						*(pd0+1) = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
						*(pd0+2) = clamptab[Y+Crrtab[Cr]];
                    }
                }
                if (x < 2)
				{
					pd0 += 3*x; pd1 += 3*x; x = 0;
                }
                else
				{
					pd0 += 3*2; pd1 += 3*2; x -= 2;
                }
                pp += 6;
            }
            if (h <= 2)
                break;
            h -= 2;

			pd0 = pd1+scanline-(w*3);
			pd1 = pd0+scanline;
            pp += fromskew;
        }
    }
}

/*
 * 8-bit packed YCbCr samples w/ 2,1 subsampling => RGB
 */
DECLAREContigPutFunc(putcontig8bitYCbCr21tile)
{
    TIFFYCbCrToRGB* ycbcr = img->ycbcr;
	int* Crrtab = ycbcr->Cr_r_tab;
	int* Cbbtab = ycbcr->Cb_b_tab;
	int32* Crgtab = ycbcr->Cr_g_tab;
	int32* Cbgtab = ycbcr->Cb_g_tab;
	TIFFRGBValue* clamptab = ycbcr->clamptab;
	
	int scanline;
	unsigned char *pdest, *pd0;
	
    (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
	pd0 = pdest;

    fromskew = (fromskew * 4) / 2;

    do
	{
		x = w>>1;
		do
		{
			int Cb = pp[2];
			int Cr = pp[3];
			int Y;

			_YCBCR_RGB_PD0(0);
			_YCBCR_RGB_PD0(1);

			pp += 4;
		} while (--x);

        if( (w&1) != 0 )
        {
		    int Cb = pp[2];
		    int Cr = pp[3];
			int Y;
            
			Y = pp[0];
			*pd0++ = clamptab[Y+Cbbtab[Cb]];
			*pd0++ = clamptab[Y + (int)((Cbgtab[Cb]+Crgtab[Cr])>>16)];
			*pd0++ = clamptab[Y+Crrtab[Cr]];

			pp += 4;
        }

		pd0 = pd0+scanline-(w*3);
		pp += fromskew;
    } while (--h);
}

/*
 * 8-bit packed YCbCr samples w/ no subsampling => RGB
 */
DECLAREContigPutFunc(putcontig8bitYCbCr11tile)
{
    TIFFYCbCrToRGB* ycbcr = img->ycbcr;
	int* Crrtab = ycbcr->Cr_r_tab;
	int* Cbbtab = ycbcr->Cb_b_tab;
	int32* Crgtab = ycbcr->Cr_g_tab;
	int32* Cbgtab = ycbcr->Cb_g_tab;
	TIFFRGBValue* clamptab = ycbcr->clamptab;
	
	int scanline;
	unsigned char *pdest, *pd0;
	
    (void) y;
	
	assert(img);
	
	_cal_scanline_pdest(img, x, y, &scanline, &pdest);
	
	pd0 = pdest;
	
    fromskew *= 3;

    do
	{
        x = w;
		do
		{
			int Cb = pp[1];
			int Cr = pp[2];
			int Y;

			_YCBCR_RGB_PD0(0);

			pp += 3;
		} while (--x);

		pd0 = pd0+scanline-w*3;
		pp += fromskew;
    } while (--h);
}

#undef  _YCBCR_RGB_PD0
#undef  _YCBCR_RGB_PD1
#undef  _YCBCR_RGB_PD2
#undef  _YCBCR_RGB_PD3

#define	LumaRed			coeffs[0]
#define	LumaGreen		coeffs[1]
#define	LumaBlue		coeffs[2]
#define	SHIFT			16
#define	FIX(x)			((int32)((x) * (1L<<SHIFT) + 0.5))
#define	ONE_HALF		((int32)(1<<(SHIFT-1)))



/*
 * Initialize the YCbCr->RGB conversion tables.  The conversion
 * is done according to the 6.0 spec:
 *
 *    R = Y + Cr*(2 - 2*LumaRed)
 *    B = Y + Cb*(2 - 2*LumaBlue)
 *    G =   Y
 *        - LumaBlue*Cb*(2-2*LumaBlue)/LumaGreen
 *        - LumaRed*Cr*(2-2*LumaRed)/LumaGreen
 *
 * To avoid floating point arithmetic the fractional constants that
 * come out of the equations are represented as fixed point values
 * in the range 0...2^16.  We also eliminate multiplications by
 * pre-calculating possible values indexed by Cb and Cr (this code
 * assumes conversion is being done for 8-bit samples).
 */
static void tiff_ycbcr_to_rgb_init(TIFFYCbCrToRGB* ycbcr, TIFF* tif)
{
    TIFFRGBValue* clamptab;
    float* coeffs;
    int i;

    clamptab = (TIFFRGBValue*)((tidata_t) ycbcr+TIFFroundup(sizeof (TIFFYCbCrToRGB), sizeof (long)));

    _TIFFmemset(clamptab, 0, 256);		/* v < 0 => 0 */

    ycbcr->clamptab = (clamptab += 256);

    for (i = 0; i < 256; i++)
		clamptab[i] = i;

    _TIFFmemset(clamptab+256, 255, 2*256);	/* v > 255 => 255 */

    TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRCOEFFICIENTS, &coeffs);

    _TIFFmemcpy(ycbcr->coeffs, coeffs, 3*sizeof (float));

    { 
		float f1 = 2-2*LumaRed;		int32 D1 = FIX(f1);
		float f2 = LumaRed*f1/LumaGreen;	int32 D2 = -FIX(f2);
		float f3 = 2-2*LumaBlue;		int32 D3 = FIX(f3);
		float f4 = LumaBlue*f3/LumaGreen;	int32 D4 = -FIX(f4);
		int x;

		ycbcr->Cr_r_tab = (int*) (clamptab + 3*256);
		ycbcr->Cb_b_tab = ycbcr->Cr_r_tab + 256;
		ycbcr->Cr_g_tab = (int32*) (ycbcr->Cb_b_tab + 256);
		ycbcr->Cb_g_tab = ycbcr->Cr_g_tab + 256;

		for (i = 0, x = -128; i < 256; i++, x++)
		{
			ycbcr->Cr_r_tab[i] = (int)((D1*x + ONE_HALF)>>SHIFT);
			ycbcr->Cb_b_tab[i] = (int)((D3*x + ONE_HALF)>>SHIFT);
			ycbcr->Cr_g_tab[i] = D2*x;
			ycbcr->Cb_g_tab[i] = D4*x + ONE_HALF;
		}
    }
}
#undef	SHIFT
#undef	ONE_HALF
#undef	FIX
#undef	LumaBlue
#undef	LumaGreen
#undef	LumaRed


static ise_tileContigRoutine init_ycbcr_conversion(ISE_TIFFRGBAImage* img)
{
    uint16 hs, vs;

    if (img->ycbcr == 0)
	{
		img->ycbcr = (TIFFYCbCrToRGB*) _TIFFmalloc(
			TIFFroundup(sizeof (TIFFYCbCrToRGB), sizeof (long))
			+ 4*256*sizeof (TIFFRGBValue) + 2*256*sizeof (int) + 2*256*sizeof (int32));

		if (img->ycbcr == 0)
		{
			TIFFError(TIFFFileName(img->tif),
			"No space for YCbCr->RGB conversion state");
			return (0);
		}

		tiff_ycbcr_to_rgb_init(img->ycbcr, img->tif);
    }
	else
	{
		float* coeffs;

		TIFFGetFieldDefaulted(img->tif, TIFFTAG_YCBCRCOEFFICIENTS, &coeffs);

		if (_TIFFmemcmp(coeffs, img->ycbcr->coeffs, 3*sizeof (float)) != 0)
		{
			tiff_ycbcr_to_rgb_init(img->ycbcr, img->tif);
		}
    }

    TIFFGetFieldDefaulted(img->tif, TIFFTAG_YCBCRSUBSAMPLING, &hs, &vs);

    switch ((hs<<4)|vs)
	{
    case 0x44: return ((ise_tileContigRoutine)putcontig8bitYCbCr44tile);
    case 0x42: return ((ise_tileContigRoutine)putcontig8bitYCbCr42tile);
    case 0x41: return ((ise_tileContigRoutine)putcontig8bitYCbCr41tile);
    case 0x22: return ((ise_tileContigRoutine)putcontig8bitYCbCr22tile);
    case 0x21: return ((ise_tileContigRoutine)putcontig8bitYCbCr21tile);
    case 0x11: return ((ise_tileContigRoutine)putcontig8bitYCbCr11tile);
    }

    return (ise_tileContigRoutine)(0);
}


/* 选择适当的转换程序用于紧凑型数据解压（展开）*/
static int sel_tile_contig_case(ISE_TIFFRGBAImage* img)
{
    ise_tileContigRoutine put = 0;

	switch (img->photometric)
	{
	case PHOTOMETRIC_RGB:
		switch (img->bitspersample)
		{
		case 8:
			if (img->alpha == EXTRASAMPLE_ASSOCALPHA)
				put = putRGBAAcontig8bittile;
			else if (img->alpha == EXTRASAMPLE_UNASSALPHA)
				put = putRGBUAcontig8bittile;
			else
				put = putRGBcontig8bittile;
			break;
		case 16:
			put = putRGBcontig16bittile;

			if (img->alpha == EXTRASAMPLE_ASSOCALPHA)
				put = putRGBAAcontig16bittile;
			else if (img->alpha == EXTRASAMPLE_UNASSALPHA)
				put = putRGBUAcontig16bittile;
			break;
		}
		break;
	case PHOTOMETRIC_SEPARATED:
		if (img->bitspersample == 8)
		{
			put = putRGBcontig8bitCMYKtile;
		}
		break;
	case PHOTOMETRIC_PALETTE:
		switch (img->bitspersample)
		{
		case 8:	put = put8bitcmaptile; break;
		case 4: put = put4bitcmaptile; break;
		case 2: put = put2bitcmaptile; break;
		case 1: put = put1bitcmaptile; break;
		}
		break;
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
		switch (img->bitspersample)
		{
		case 16: put = put16bitbwtile; break;
		case 8:  put = putgreytile; break;
		case 4:  put = put4bitbwtile; break;
		case 2:  put = put2bitbwtile; break;
		case 1:  put = put1bitbwtile; break;
		}
		break;
	case PHOTOMETRIC_YCBCR:
		if (img->bitspersample == 8)
			put = init_ycbcr_conversion(img);
		break;
	}

    return ((img->put.contig = put) != 0);
}


/* 选择适当的转换程序用于分离型数据解压（展开）*/
static int sel_tile_separate_case(ISE_TIFFRGBAImage* img)
{
    ise_tileSeparateRoutine put = 0;

	switch (img->photometric)
	{
	case PHOTOMETRIC_RGB:
		switch (img->bitspersample)
		{
		case 8:
			if (img->alpha == EXTRASAMPLE_ASSOCALPHA)
				put = putRGBAAseparate8bittile;
			else if (img->alpha == EXTRASAMPLE_UNASSALPHA)
				put = putRGBUAseparate8bittile;
			else
				put = putRGBseparate8bittile;
			break;
		case 16:
			put = putRGBseparate16bittile;

			if (img->alpha == EXTRASAMPLE_ASSOCALPHA)
				put = putRGBAAseparate16bittile;
			else if (img->alpha == EXTRASAMPLE_UNASSALPHA)
				put = putRGBUAseparate16bittile;
			break;
		}
		break;
	}

    return ((img->put.separate = put) != 0);
}


