/********************************************************************

	tiff.c

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
	���ļ���;��	ISeeͼ���������TIFFͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��� ��ɫ�塢�Ҷȡ�RGB��YCbCr ��ʽ�洢��TIFFͼ
								�󣨰�����ҳͼ�󣩣�����֧��λ��ȳ���32λ
								��ͼ��
  
					���湦�ܣ��ɽ�1��4��8��24λ�ĵ�ҳ���ҳͼ�󱣴�Ϊ��
								Ӧ��TIFFͼ��
	

	���ļ���д�ˣ�	YZ				yzfree##sina.com
					xuhappy			xuxuhappy##sina.com
					zjucypher		xubo.cy##263.net

	���ļ��汾��	20511
	����޸��ڣ�	2002-5-11

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

		2002-5		������ȡJPEGѹ����ʽ��TIFFͼ��ʱR��B�����ߵ������⡣
					����������ȷ��ȡPhotoShop������TIFFͼ������⡣
					leye���֣�YZ������

		2002-4		�����°���롣֧��ISeeIO������JPEG��ʽ�洢��TIFF
					ͼ�󡣱��湦��֧��1��4��8��24λ��ȡ�֧�ֶ�ҳTIFF��
		
		2001-2		�����˱��湦��
		2001-1		����RGBAת�������д��ڵ����⣬���Ľ��˲������ƴ���
		2001-1		���������libtiff��
		2000-7		��һ��������


********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */

/*###################################################################

  ��ֲע�ͣ����´���ʹ����WIN32ϵͳ��SEH(�ṹ���쳣����)�����߳�ͬ��
			���󡰹ؼ��Ρ�������ֲʱӦתΪLinux����Ӧ��䡣

  ��ע���������ҪJPEG����е�jpeglib 6b���֧�֣������ڱ��뱾���֮
			ǰ���ȱ���JPEG�����

  #################################################################*/


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* ����windows.h�ļ��ı���ʱ�� */
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


IRWP_INFO			tiff_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	tiff_get_info_critical;	/* tiff_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	tiff_load_img_critical;	/* tiff_load_image�����Ĺؼ��� */
CRITICAL_SECTION	tiff_save_img_critical;	/* tiff_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
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
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&tiff_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&tiff_get_info_critical);
			InitializeCriticalSection(&tiff_load_img_critical);
			InitializeCriticalSection(&tiff_save_img_critical);

			/* �滻ȱʡ�Ĵ��󼰾��洦���� */
			TIFFSetWarningHandler(_TIFFwarningHandler);
			TIFFSetErrorHandler(_TIFFerrorHandler);
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
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
	/* ��ʼ�����߳�ͬ������ */
}

TIFF_API void CALLAGREEMENT tiff_detach_plugin()
{
	/* ���ٶ��߳�ͬ������ */
}

#endif /* WIN32 */


/* ��ʼ�������Ϣ�ṹ */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info)
{
	assert(lpirwp_info);

	/* ��ʼ���ṹ���� */
	memset((void*)lpirwp_info, 0, sizeof(IRWP_INFO));

	/* �汾�š���ʮ����ֵ��ʮλΪ���汾�ţ���λΪ���汾��*/
	lpirwp_info->irwp_version = MODULE_BUILDID;
	/* ������� */
	strcpy((char*)(lpirwp_info->irwp_name), MODULE_NAME);
	/* ��ģ�麯��ǰ׺ */
	strcpy((char*)(lpirwp_info->irwp_func_prefix), MODULE_FUNC_PREFIX);


	/* ����ķ������͡�0�����԰�����1���������� */
#ifdef _DEBUG
	lpirwp_info->irwp_build_set = 0;
#else
	lpirwp_info->irwp_build_set = 1;
#endif


	/* ���ܱ�ʶ ��##���ֶ������� */
	lpirwp_info->irwp_function = IRWP_READ_SUPP | IRWP_WRITE_SUPP;

	/* ����ģ��֧�ֵı���λ�� */
	/* ################################################################# */
	/* ��λ����һ������32λ��ʽ�޷���ʾ���˴�����TIFF��ȷ��λ����д��*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(1-1)) | (1UL<<(4-1)) | \
		(1UL<<(8-1)) | (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 2;	/* 2��������ัͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 1;
	lpirwp_info->irwp_save.para[0].count = 2;	/* ����ѹ����ʽ */
	strcpy((char*)lpirwp_info->irwp_save.para[0].desc, (const char *)"ѹ����ʽ");

	lpirwp_info->irwp_save.para[0].value[0] = 0;		/* ��ѹ�� */
	lpirwp_info->irwp_save.para[0].value[1] = 1;		/* JPEG   */
	
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[0], (const char *)"��ѹ��");
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[1], (const char *)"JPEGѹ���㷨");

	
	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 4;	

	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"TIFF��ʽ�ĸ����Գ����ҵ�������:)");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
		(const char *)"xuhappy");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
		(const char *)"xuxuhappy##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
		(const char *)":)");
	/* ---------------------------------[2] �� ������ -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_name), 
		(const char *)"zjucypher");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_email), 
		(const char *)"xubo.cy##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_message), 
		(const char *)":)");
	/* ---------------------------------[3] �� ������ -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_name), 
				(const char *)"Sam Leffler");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_email), 
		(const char *)"tiff##olympiakos.com");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_message), 
				(const char *)":)");
	/* ---------------------------------[4] �� ������ -------------- */
	/* ������������Ϣ�ɼ��ڴ˴���
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	*/
	/* ------------------------------------------------------------- */


	/* ���������Ϣ����չ����Ϣ��*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* ����������##���ֶ������� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"TIFF");

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
TIFF_API int CALLAGREEMENT tiff_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	TIFF			*tif = 0;

	unsigned long	alpha;
	unsigned short	*sampleinfo, extrasamples;
	unsigned short	bpp, spp, orgi, compression, phint;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&tiff_get_info_critical);

			/* ��ָ���� */
			if ((tif=TIFFOpen((const char*)psct, "rmH")) == (TIFF*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ���ͼ���Ƿ���Ա��������ȡ */
			switch (tiff_to_rgba_ok(tif))
			{
			case	0:
				break;
			case	-1:	/* ����򲻱�֧�ֵ�ͼ�� */
			case	-2:
			case	-3:
			case	-4:
				b_status = ER_BADIMAGE; __leave; break;
			case	-5:
				b_status = ER_FILERWERR; __leave; break;
			default:
				b_status = ER_SYSERR; __leave; break;
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_TIFF;			/* ͼ���ļ���ʽ����׺���� */
			
			/* ��ȡ�ļ��ĳ��ȡ�ͼ��Ŀ�ȡ��߶ȵ���Ϣ */
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
				/* �������ALPHA�����������������λ���� */
				pinfo_str->bitcount = bpp*(spp-((alpha) ? 0 : extrasamples));
				break;
			case	PHOTOMETRIC_SEPARATED:	/* !color separations */
				pinfo_str->bitcount = bpp*(spp-1);	/* CMYK���ĸ������㣬ȥ��һ�� */
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

			/* ���ͼ��λ���Ƿ���� */
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
				pinfo_str->order = 0;		/* ����ͼ */
				break;
			case	ORIENTATION_BOTRIGHT:
			case	ORIENTATION_BOTLEFT:
			case	ORIENTATION_RIGHTBOT:
			case	ORIENTATION_LEFTBOT:
				pinfo_str->order = 1;		/* ����ͼ */
				break;
			default:
				b_status = ER_BADIMAGE; __leave; break;
			}

			/* ��ȡͼ���ѹ����ʽ */
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
			
			
			/* �趨���ݰ�״̬ */
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


/* ��ȡͼ��λ���� */
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
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&tiff_load_img_critical);

			memset((void*)&img, 0, sizeof(ISE_TIFFRGBAImage));
			pinfo_str->process_total = 1;
			pinfo_str->process_current = 0;

			/* ��ָ���� */
			if ((tif=TIFFOpen((const char*)psct, "rmH")) == (TIFF*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ��ȡ��λ����йص����� */
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

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ���ͼ���Ƿ���Ա��������ȡ */
				switch (tiff_to_rgba_ok(tif))
				{
				case	0:
					break;
				case	-1:	/* ����򲻱�֧�ֵ�ͼ�� */
				case	-2:
				case	-3:
				case	-4:
					b_status = ER_BADIMAGE; __leave; break;
				case	-5:
					b_status = ER_FILERWERR; __leave; break;
				default:
					b_status = ER_SYSERR; __leave; break;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_TIFF;			/* ͼ���ļ���ʽ����׺���� */
				
				/* ��ȡ�ļ��ĳ��ȡ�ͼ��Ŀ�ȡ��߶ȵ���Ϣ */
				TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &(pinfo_str->width));
				TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &(pinfo_str->height));

				switch (photo_inter)
				{
				case	PHOTOMETRIC_RGB:		/* RGBȫ��ɫģʽ */
					/* �������ALPHA�����������������λ���� */
					pinfo_str->bitcount = bpp*(spp-((alpha) ? 0 : extrasamples));
					break;
				case	PHOTOMETRIC_SEPARATED:	/* CMYKɫϵ */
					pinfo_str->bitcount = bpp*(spp-1);	/* CMYK���ĸ������㣬ȥ��һ�� */
					break;
				case	PHOTOMETRIC_MINISWHITE:	/* �Ҷ�ͼ����СֵΪ��ɫ */
				case	PHOTOMETRIC_MINISBLACK:	/* �Ҷ�ͼ����СֵΪ��ɫ */
				case	PHOTOMETRIC_CIELAB:		/* 1976��CIE��Labɫϵ */
				case	PHOTOMETRIC_ICCLAB:		/* ICC Lab */
				case	PHOTOMETRIC_ITULAB:		/* ITU Lab */
				case	PHOTOMETRIC_LOGL:		/* CIE Log2(L) */
				case	PHOTOMETRIC_LOGLUV:		/* CIE Log2(L) (u',v') */
				case	PHOTOMETRIC_PALETTE:	/* ��ɫ��ģʽ */
				case	PHOTOMETRIC_YCBCR:		/* YCBCRɫϵ��JPEG�� */
				case	PHOTOMETRIC_MASK:		/* ����ͼ����ɫ�� */
				default:
					pinfo_str->bitcount = bpp*spp;
					break;
				}
				
				/* ���ͼ��λ���Ƿ���� */
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
					pinfo_str->order = 0;		/* ����ͼ */
					break;
				case	ORIENTATION_BOTRIGHT:
				case	ORIENTATION_BOTLEFT:
				case	ORIENTATION_RIGHTBOT:
				case	ORIENTATION_LEFTBOT:
					pinfo_str->order = 1;		/* ����ͼ */
					break;
				default:
					b_status = ER_BADIMAGE; __leave; break;
				}

				/* ��ȡͼ���ѹ����ʽ */
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
				
				/* �趨����ֵ */
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

			/* ����ͼ����� */
			pinfo_str->imgnumbers = TIFFNumberOfDirectories(tif);
			
			/* ����ֵΪͼ��ҳ�� */
			pinfo_str->process_total = pinfo_str->imgnumbers;
			pinfo_str->process_current = 0;

			/* ��ҳ��ȡͼ�� */
			for (j=0;j<(int)pinfo_str->imgnumbers;j++,pinfo_str->process_current++)
			{
				if (j == 0)
				{
					/* �׷�ͼ�� */
					switch (tiff_rgba_image_begin(&img, tif, 1/*�ϸ���*/, 0, (void*)pinfo_str))
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
					case	0:	/* �ɹ� */
						break;
					default:
						assert(0);
						b_status = ER_SYSERR;
						__leave;
						break;
					}

					/* ȡ��ɨ���гߴ� */
					linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
					
					assert(pinfo_str->p_bit_data == (unsigned char *)0);
					/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
					pinfo_str->p_bit_data = (unsigned char *)malloc(linesize * pinfo_str->height + 4);
					if (!pinfo_str->p_bit_data)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					assert(pinfo_str->pp_line_addr == (void**)0);
					/* �������׵�ַ���� */
					pinfo_str->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo_str->height);
					if (!pinfo_str->pp_line_addr)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					/* ��ʼ�����׵�ַ���� */
					if (pinfo_str->order == 0)		/* ���� */
					{
						for (i=0;i<(int)(pinfo_str->height);i++)
							pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
					}
					else							/* ���� */
					{
						for (i=0;i<(int)(pinfo_str->height);i++)
							pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
					}

					/* ��ȡ�������� */
					if (tiff_rgba_image_get(&img, pinfo_str->width, pinfo_str->height))
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* ���img�ṹ������һ�ζ�ȡ���� */
					tiff_rgba_image_end(&img);
				}
				else
				{
					/* ������ͼ��ڵ� */
					if ((lpsub=_alloc_SUBIMGBLOCK()) == 0)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					lpsub->number  = j;				/* ��ͼ������� 1 ��ʼ */
					lpsub->parents = pinfo_str;

					/* ������ͼ������β�� */
					if (lplast == 0)
					{
						pinfo_str->psubimg = lpsub;	/* �׷���ͼ�� */
						lplast = lpsub;
						
						lpsub->prev = 0;
						lpsub->next = 0;
					}
					else
					{
						lpsub->prev = lplast;		/* ������ͼ�� */
						lpsub->next = 0;

						lplast->next = lpsub;
						lplast = lpsub;
					}
					
					/* ת����һ��ͼ��IFD�� */
					if (TIFFReadDirectory(tif) == 0)
					{
						b_status = ER_BADIMAGE; __leave;
					}

					/* ���ͼ���Ƿ���Ա��������ȡ */
					switch (tiff_to_rgba_ok(tif))
					{
					case	0:
						break;
					case	-1:	/* ����򲻱�֧�ֵ�ͼ�� */
					case	-2:
					case	-3:
					case	-4:
						b_status = ER_BADIMAGE; __leave; break;
					case	-5:
						b_status = ER_FILERWERR; __leave; break;
					default:
						b_status = ER_SYSERR; __leave; break;
					}
					
					/* ��ȡͼ��Ŀ�ȡ��߶ȡ�λ��ȵ���Ϣ */
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
					
					/* ����ͼ��λ��� */
					switch (photo_inter)
					{
					case	PHOTOMETRIC_RGB:
						/* �������ALPHA�����������������λ���� */
						lpsub->bitcount = bpp*(spp-((alpha) ? 0 : extrasamples));
						break;
					case	PHOTOMETRIC_SEPARATED:
						lpsub->bitcount = bpp*(spp-1);	/* CMYK���ĸ������㣬ȥ��һ�� */
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
					
					/* ���ͼ��λ���Ƿ���� */
					if ((lpsub->bitcount > ISEEIRW_MAX_BITCOUNT) || (lpsub->bitcount == 0))
					{
						b_status = ER_BADIMAGE; __leave;
					}
					
					/* �ж�ͼ��Ĵ�ŷ��� */
					TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &orgi);
					switch (orgi)
					{
					case	ORIENTATION_TOPLEFT:
					case	ORIENTATION_TOPRIGHT:
					case	ORIENTATION_RIGHTTOP:
					case	ORIENTATION_LEFTTOP:
						lpsub->order = 0;		/* ����ͼ */
						break;
					case	ORIENTATION_BOTRIGHT:
					case	ORIENTATION_BOTLEFT:
					case	ORIENTATION_RIGHTBOT:
					case	ORIENTATION_LEFTBOT:
						lpsub->order = 1;		/* ����ͼ */
						break;
					default:
						b_status = ER_BADIMAGE; __leave; break;
					}
					
					memset((void*)&img, 0, sizeof(ISE_TIFFRGBAImage));

					/* ׼����ȡ���� */
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
					case	0:	/* �ɹ� */
						break;
					default:
						assert(0);
						b_status = ER_SYSERR;
						__leave;
						break;
					}
					
					/* ȡ��ɨ���гߴ� */
					linesize = _calcu_scanline_size(lpsub->width, lpsub->bitcount);
					
					assert(lpsub->p_bit_data == (unsigned char *)0);
					/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
					lpsub->p_bit_data = (unsigned char *)malloc(linesize * lpsub->height + 4);
					if (!lpsub->p_bit_data)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					assert(lpsub->pp_line_addr == (void**)0);
					/* �������׵�ַ���� */
					lpsub->pp_line_addr = (void **)malloc(sizeof(void*) * lpsub->height);
					if (!lpsub->pp_line_addr)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					/* ��ʼ�����׵�ַ���� */
					if (lpsub->order == 0)		/* ���� */
					{
						for (i=0;i<(int)(lpsub->height);i++)
							lpsub->pp_line_addr[i] = (void *)(lpsub->p_bit_data+(i*linesize));
					}
					else						/* ���� */
					{
						for (i=0;i<(int)(lpsub->height);i++)
							lpsub->pp_line_addr[i] = (void *)(lpsub->p_bit_data+((lpsub->height-i-1)*linesize));
					}
					
					/* ��ȡͼ�� */
					if (tiff_rgba_image_get(&img, lpsub->width, lpsub->height))
					{
						b_status = ER_FILERWERR; __leave;
					}
					
					tiff_rgba_image_end(&img);
				}

				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}

			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* �ͷ���ͼ�� */
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
					pinfo_str->data_state =1;	/* �Զ����� */
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


/* ����ͼ�� */
TIFF_API int CALLAGREEMENT tiff_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	TIFF			*tif = 0;
	
	LPSUBIMGBLOCK	psub;
	int				i;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&tiff_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_tiff_is_valid_img(pinfo_str, lpsave) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			/* ģʽ���е�'l'ֻ������Intel��CPU������AMD�ȣ�������ֲ��
			 * ��Intel CPU����MOTO CPU�������Ϊ'b'lH */
			if ((tif=TIFFOpen((const char*)psct, "wlH")) == (TIFF*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			psub = pinfo_str->psubimg;

			for (i=0;i<(int)pinfo_str->imgnumbers;i++)
			{
				if (i == 0)	/* ������ͼ�� */
				{
					switch (tiff_write_master_img(tif, pinfo_str, lpsave))
					{
					case	0:			/* д��ɹ� */
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
					/* ת����һ����ͼ�� */
					if (TIFFWriteDirectory(tif) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					assert(psub);		/* ��ʱ��Ȼ������ͼ��ڵ� */

					switch (tiff_write_servant_img(tif, psub, lpsave))
					{
					case	0:			/* д��ɹ� */
						break;
					case	-2:			/* ���ǰ��ͼ��û����λ���жϣ������ڴ��ж� */
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

			
			/* �������� */
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
/* �ڲ��������� */

/* ����ɨ���гߴ�(���ֽڶ���) */
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* �жϴ����ͼ���Ƿ���Ա����� */
int CALLAGREEMENT _tiff_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	int i;

	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(tiff_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (tiff_irwp_info.irwp_save.img_num)
		if (tiff_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */

	for (i=0;i<tiff_irwp_info.irwp_save.para[0].count;i++)
		if (tiff_irwp_info.irwp_save.para[0].value[i] == lpsave->para_value[0])
			break;
		
	return (i==tiff_irwp_info.irwp_save.para[0].count) ? -3:0;		/* -3:��Ч��ѹ��ѡ�� */
}


/* ��ȡ����ɫ������ֵ */
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
		/* ��ɫ��λͼ������ */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:		/* 555 ��ʽ */
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:		/* 888 ��ʽ */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;

		if (alpha)
			*pa_mask = 0xff000000;
		else
			*pa_mask = 0x0;
		break;
	default:		/* �Ƿ���ʽ */
		result = -1;
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}
	
	return result;	/* 0-�ɹ��� -1���Ƿ���ͼ���ʽ */
}


/* ����һ����ͼ��ڵ㣬����ʼ���ڵ������� */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void)
{
	LPSUBIMGBLOCK	p_subimg = 0;
	
	/* ������ͼ����Ϣ�ڵ�(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* �ڴ治�� */
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));
	
	p_subimg->number = 1;
	p_subimg->colorkey = -1;
	
	return p_subimg;
}


/* �ͷ�һ����ͼ��ڵ㣬�������е�λ�����������׵�ַ���� */
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


