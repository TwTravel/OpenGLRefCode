/********************************************************************

	wr_tiff.c

	----------------------------------------------------------------
    ������֤ �� GPL
	��Ȩ���� (C) 2001 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	��һ����������������������������������������GNU ͨ�ù�����
	��֤�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ���
	�����ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�ù������֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�ù������֤�ĸ�������Ŀ¼
	GPL.txt�ļ����������û�У�д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	�������ʹ�ñ����ʱ��ʲô������飬�������µ�ַ������ȡ����ϵ��

			http://isee.126.com
			http://iseeexplorer.cosoft.org.cn

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������TIFFͼ���ģ�鸨��ʵ���ļ�

					���湦�ܣ��ɽ�1��4��8��24λ�ĵ�ҳ���ҳͼ�󱣴�
								Ϊ��Ӧ��TIFFͼ��
  
	���ļ���д�ˣ�	YZ				yzfree##yeah.net

	���ļ��汾��	20418
	����޸��ڣ�	2002-4-19

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------

    ���ô����д�˼���Ȩ��Ϣ��
	----------------------------------------------------------------
	�������	TIFF�������(LibTiff)		Ŀ¼��libtiff
	
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
			
	������ʷ��

		2002-4		��һ��������


********************************************************************/

#include <stdio.h>
#include <assert.h>

#include "libtiff\tiff.h"
#include "libtiff\tiffiop.h"
#include "wr_tiff.h"



/* �������� */
static void CALLAGREEMENT _ABGR_ARGB(unsigned char *pbits, unsigned long width);
static void CALLAGREEMENT _tiff_1_to_cross(unsigned char *pbits, unsigned long width);
static void CALLAGREEMENT _tiff_4_to_cross(unsigned char *pbits, unsigned long width);




/* д��ͼ�� */
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
		/* ����ѹ����ʽ */
		switch (lpsave->para_value[0])
		{
		case	0:
			compression = COMPRESSION_NONE; break;
		case	1:
			compression = COMPRESSION_JPEG; break;
		default:
			result = -1; __leave; break;			/* ָ���˷Ƿ�ѹ����ʽ */
		}

		/* ����ɫϵ */
		switch (pinfo_str->bitcount)
		{
		case	1:
		case	4:
		case	8:
			photometric = PHOTOMETRIC_PALETTE;
			compression = COMPRESSION_NONE;			/* ����ǵ�ɫ��ͼ����ʹ��JPEGѹ����ʽ */
			break;
		case	24:
			photometric = PHOTOMETRIC_RGB; break;
		default:
			assert(0); result = -2; __leave; break;	/* Դͼ��λ��Ȳ��Ϸ� */
		}
		
		
		/* �ڴ�������ǰ��������tiff�ļ���tagֵ */
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, pinfo_str->height);
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, pinfo_str->width);
		/* ��ƽ�淽ʽ��ͼ����������ΪRGB...RGB..RGB.. */
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

		/* ����ÿ�����ص�ȡ��������ɫ����� */
		switch (pinfo_str->bitcount)
		{
		case	1:
		case	4:
		case	8:
			{
				int	palcnt = 1<<(pinfo_str->bitcount);	/* ��ɫ������� */

				TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL, 1);
				TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, pinfo_str->bitcount);
				
				assert((int)pinfo_str->pal_count == palcnt);
				
				/* ����TIFF���ĵ�ɫ�� */
				if ((pal=(unsigned short *)_TIFFmalloc(palcnt*sizeof(unsigned short)*3)) == 0)
				{
					result = -3; __leave; break;		/* �ڴ治�� */
				}
				for (j=0;j<palcnt;j++)
				{
					(pal+0*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&pinfo_str->palette[j]))->rgbRed))<<8;
					(pal+1*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&pinfo_str->palette[j]))->rgbGreen))<<8;
					(pal+2*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&pinfo_str->palette[j]))->rgbBlue))<<8;
				}
				
				/* д���ɫ������ */
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


		/* ͼ�������õ�ɫ��ϵͳ */
		TIFFSetField(tif,TIFFTAG_PHOTOMETRIC, photometric);
		TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(tif, TIFFTAG_COMPRESSION, compression);

		/* ����ѹ�����趨��һ���Ĳ��� */
		switch (compression)
		{
		case	COMPRESSION_JPEG:
			TIFFSetField(tif, TIFFTAG_JPEGQUALITY, 75);		/* ȱʡƷ��ϵ����Ϊ75 */
			TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
			break;
		case	COMPRESSION_NONE:
			break;
		default:
			assert(0); result = -4; __leave; break;	/* ϵͳ���ȶ� */
		}
		
		cnum = 0; tnum = (int)pinfo_str->imgnumbers;

		/* ����Ƕ�ҳͼ��������ҳ�ż�ҳ���� */
		if (pinfo_str->imgnumbers > 1)
		{
			TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
			TIFFSetField(tif, TIFFTAG_PAGENUMBER, cnum, tnum);
		}

		/* �趨��״������ÿһ��������ɨ���и��� */
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,	TIFFDefaultStripSize(tif, rowsperstrip));

		linebytes = DIBSCANLINE_WIDTHBYTES(pinfo_str->bitcount * pinfo_str->width);

		/* �����л�������+4�Ƿ�ֹR��Bת��ʱ�����ڴ��дԽ�磩 */
		if ((buf=(unsigned char *)_TIFFmalloc(max(TIFFScanlineSize(tif),(int)linebytes)+4)) == 0)
		{
			result = -3; __leave;				/* �ڴ治�� */
		}
		_TIFFmemset(buf, 0, max(TIFFScanlineSize(tif),(int)linebytes)+4);

		
		/* ���������ݰ���״��ʽ����TIFF */
		for (i=0;i<(int)pinfo_str->height;i++)
		{
			_TIFFmemcpy(buf, pinfo_str->pp_line_addr[i], linebytes);

			/* TIFF������1λ��4λ�������з�ʽ��ISeeλ����ʽ��ͬ��������Ҫת�� */
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
				/* Sam��libtiff����TIFFWriteScanline����Ҫ�����buf����ΪBGRA��ʽ 
				 * ��������ISeeλ����ʽ�෴�����Զ�24λͼ����R��B�Ե���
				 * ��JPEGѹ����ʽ��Ϊ������JPEGLIB�⣬���Բ��õ�����*/
				_ABGR_ARGB(buf, pinfo_str->width);
			}

			/* д��ͼ�� */
			if (TIFFWriteScanline(tif, buf, i, 0) < 0)
			{
				result = -5; __leave;			/* д��ʱ�������� */
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


/* д�����ӣ�ͼ�� */
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
		/* ����ѹ����ʽ */
		switch (lpsave->para_value[0])
		{
		case	0:
			compression = COMPRESSION_NONE; break;
		case	1:
			compression = COMPRESSION_JPEG; break;
		default:
			result = -1; __leave; break;			/* ָ���˷Ƿ�ѹ����ʽ */
		}

		/* ����ɫϵ */
		switch (psub->bitcount)
		{
		case	1:
		case	4:
		case	8:
			photometric = PHOTOMETRIC_PALETTE;
			compression = COMPRESSION_NONE;			/* ����ǵ�ɫ��ͼ����ʹ��JPEGѹ����ʽ */
			break;
		case	24:
			photometric = PHOTOMETRIC_RGB; break;
		default:
			result = -2; __leave; break;			/* Դͼ��λ��Ȳ��Ϸ� */
		}
		
		
		/* �ڴ�������ǰ��������tiff�ļ���tagֵ */
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, psub->height);
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, psub->width);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

		/* ÿ������ȡ���� */
		switch (psub->bitcount)
		{
		case	1:
		case	4:
		case	8:
			{
				int	palcnt = 1<<(psub->bitcount);	/* ��ɫ������� */
				
				TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL, 1);
				TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE, psub->bitcount);
				
				assert((int)psub->pal_count == palcnt);
				
				/* ����TIFF���ĵ�ɫ�� */
				if ((pal=(unsigned short *)_TIFFmalloc(palcnt*sizeof(unsigned short)*3)) == 0)
				{
					result = -3; __leave; break;		/* �ڴ治�� */
				}
				for (j=0;j<palcnt;j++)
				{
					(pal+0*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&psub->palette[j]))->rgbRed))<<8;
					(pal+1*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&psub->palette[j]))->rgbGreen))<<8;
					(pal+2*palcnt)[j] = ((unsigned short)(((RGBQUAD*)(&psub->palette[j]))->rgbBlue))<<8;
				}
				
				/* д���ɫ������ */
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
			TIFFSetField(tif, TIFFTAG_JPEGQUALITY, 75);		/* ȱʡƷ��ϵ����Ϊ75 */
			TIFFSetField(tif, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RGB);
			break;
		case	COMPRESSION_NONE:
			break;
		default:
			assert(0); result = -4; __leave; break;	/* ϵͳ���ȶ� */
		}

		cnum = (int)psub->number;
		tnum = (int)psub->parents->imgnumbers;
		
		TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
		TIFFSetField(tif, TIFFTAG_PAGENUMBER, cnum, tnum);

		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP,	TIFFDefaultStripSize(tif, rowsperstrip));

		linebytes = DIBSCANLINE_WIDTHBYTES(psub->bitcount * psub->width);
		
		if ((buf=(unsigned char *)_TIFFmalloc(max(TIFFScanlineSize(tif),(int)linebytes)+4)) == 0)
		{
			result = -3; __leave;				/* �ڴ治�� */
		}
		_TIFFmemset(buf, 0, max(TIFFScanlineSize(tif),(int)linebytes)+4);

		
		/* ���������ݰ���״��ʽ����TIFF */
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
				result = -5; __leave;			/* д��ʱ�������� */
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


/* �졢������ת����24λ�棩*/
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
		mov eax, dword ptr [esi];		/* ��ȡһ�����ص�eax����8λ�����ֲ��� */
		mov ebx, eax;
		and ebx, 0FFh;					/* �����R��B����ֵ */
		mov ecx, eax;
		and ecx, 0FF0000h;
		
		shl ebx, 10h;
		shr ecx, 10h;
		and eax, 0FF00FF00h;
		or  eax, ebx;
		or  eax, ecx;					/* ����R��B��λ�� */
		
		mov dword ptr [esi], eax;		/* ��дת������������� */
		
		inc esi;
		inc esi;
		inc esi;
		
		cmp edx, 0h;
		jne loop_for_pick_up;
		
		pop ecx;
		pop esi;
	}
}


/* 1λͼ�����ط�ת����ӦTIFFҪ��*/
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


/* 4λͼ�����ط�ת����ӦTIFFҪ��*/
void CALLAGREEMENT _tiff_4_to_cross(unsigned char *pbits, unsigned long width)
{
	int j;
	
	assert(pbits);
	
	for (j=0;j<(int)((width+1)/2);j++)
	{
		pbits[j] = (pbits[j]>>4)|(pbits[j]<<4);
	}
}

