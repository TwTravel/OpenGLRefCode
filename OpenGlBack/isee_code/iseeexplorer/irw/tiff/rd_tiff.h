/********************************************************************

	rd_tiff.h

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
	���ļ���;��	ISeeͼ���������TIFFͼ���ģ�鸨�������ļ�

					��ȡ���ܣ��� ��ɫ�塢�Ҷȡ�RGB��YCbCr ��ʽ�洢��TIFFͼ
								�󣨰�����ҳͼ�󣩣�����֧��λ��ȳ���32λ
								��ͼ��
							  
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

		2002-5		������ȡJPEGѹ����ʽ��TIFFͼ��ʱR��B�����ߵ������⡣
					����������ȷ��ȡPhotoShop������TIFFͼ������⡣
					leye���֣�YZ������
		2002-4		��һ��������


********************************************************************/


#ifndef __RD_TIFF_INC__
#define __RD_TIFF_INC__


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
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
	int		stoponerr;			/* ��������ʱ�Ƿ�������ı�־ */
	int		isContig;			/* �������ݴ�ŷ�ʽ */
	int		alpha;
	uint32	width;
	uint32	height;
	uint16	bitspersample;
	uint16	samplesperpixel;
	uint16	orientation;		/* ͼ���ŷ�λ */
	uint16	photometric;		/* ͼ����ʹ�õ�ɫϵ */
	uint16*	redcmap;			/* TIFFͼ��ķ���ʽ��ɫ������ */
	uint16*	greencmap;
	uint16*	bluecmap;

								/* ��Ӧ�Ķ�ȡ���� */
	int	(*get)(ISE_TIFFRGBAImage*, uint32, uint32);

	union
	{
	    void (*any)(ISE_TIFFRGBAImage*);
	    ise_tileContigRoutine	contig;
	    ise_tileSeparateRoutine	separate;
	} put;						/* ����չ����תRGBA������ */

	TIFFYCbCrToRGB	*ycbcr;		/* YCbCr ת���� */

	int			row_offset;
	int			col_offset;

	int			info_type;		/* ��Ӧ��ISee���ݰ����� */
	int			ycbcr_mark;
	union
	{
		LPINFOSTR		pinfo_str;
		LPSUBIMGBLOCK	psub_str;
	} ise;						/* ���ݰ�ָ�� */

};



/* �ӿں��� */
int  tiff_to_rgba_ok(TIFF* tif);
void tiff_rgba_image_end(ISE_TIFFRGBAImage* img);
int  tiff_rgba_image_begin(ISE_TIFFRGBAImage* img, TIFF* tif, int stop, int type, void* pinfo);
int	 tiff_rgba_image_get(ISE_TIFFRGBAImage* img, uint32 w, uint32 h);


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif

#endif /* __RD_TIFF_INC__ */
