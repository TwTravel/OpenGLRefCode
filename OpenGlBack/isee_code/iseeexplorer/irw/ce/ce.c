/********************************************************************

	ce.c

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
	���ļ���;��	ISee ͼ��ת������
	���ļ���д�ˣ�	YZ			yzfree##sina.com
	
	���ļ��汾��	20416
	����޸��ڣ�	2002-4-16
	  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
		��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-4		������ʽת����������ɫ�����ص���BUG����������
					С���Ա tearshark��
		2001-12		С���Ա menghui ���ָ�ʽת��������ȱ�ٶԶ�ҳ
					ͼ���֧�֡�����BUG��δ������
		2001-7		������һ�����԰�

********************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "iseeirw.h"
#include "ce.h"



unsigned int CALLAGREEMENT _get_next_pix(unsigned char *, int, int, int, int);
unsigned long CALLAGREEMENT _get_next_pix2(unsigned char *, int , int, int);
int CALLAGREEMENT _get_offset_bits(unsigned long);


#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}
#endif



/* 1λͼ�����ط�ת����ӦWindows GUIҪ��*/
CE_API void CALLAGREEMENT _1_to_cross(LPINFOSTR psou)
{
	unsigned char buf, tmp;
	unsigned short us;
	int	i, j, bit, mask, linesize;
	
	assert(psou);

	linesize = DIBSCANLINE_WIDTHBYTES(psou->bitcount*psou->width);
	
	for (i=0;i<(int)psou->height;i++)
	{
		for (j=0;j<linesize;j++)
		{
			buf = *((unsigned char *)(((unsigned char *)(psou->pp_line_addr[i]))+j));
			us  = (unsigned short)buf << 8;
			tmp = 0;
			for (bit=1, mask=7;bit<16;bit+=2, mask--)
			{
				tmp |= (us>>bit)&(1<<mask);
			}
			*((unsigned char *)(((unsigned char *)(psou->pp_line_addr[i]))+j)) = tmp;
		}
	}
}


/* 4λͼ�����ط�ת����ӦWindows GUIҪ��*/
CE_API void CALLAGREEMENT _4_to_cross(LPINFOSTR psou)
{
	unsigned char buf, tmp, *pc;
	int i, j, linesize;
	
	assert(psou);

	linesize = DIBSCANLINE_WIDTHBYTES(psou->bitcount*psou->width);

	for (i=0;i<(int)psou->height;i++)
	{
		for (j=0;j<(int)((psou->width+1)/2);j++)
		{
			pc = (unsigned char *)(((unsigned char *)(psou->pp_line_addr[i]))+j);
			buf = *pc;
			tmp = (buf>>4)|(buf<<4);
			*pc = tmp;
		}
	}
	
}


/* ͼ��ֱ��ת */
CE_API int CALLAGREEMENT _v_conv(LPINFOSTR psou)
{
	int linesize = DIBSCANLINE_WIDTHBYTES(psou->bitcount*psou->width);
	unsigned char *p = (unsigned char *)malloc(linesize);
	void *ptmp;
	int i;
	
	if (p)
	{
		for (i=0;i<(int)psou->height/2;i++)
		{
			memmove((void*)p, (const void *)psou->pp_line_addr[i], linesize);
			memmove((void*)psou->pp_line_addr[i], (const void *)(psou->pp_line_addr[psou->height-i-1]), linesize);
			memmove((void*)(psou->pp_line_addr[psou->height-i-1]), (const void *)p, linesize);

			ptmp = psou->pp_line_addr[i];
			psou->pp_line_addr[i] = psou->pp_line_addr[psou->height-i-1];
			psou->pp_line_addr[psou->height-i-1] = ptmp;
		}
		free(p);

		psou->order = psou->order==0;

		return 0;
	}
	else
		return -1;		/* �ڴ治�� */
}


/* ��2��3��4λ��ͼ��ת��Ϊ��׼4λλͼ��ʽ */
CE_API LPINFOSTR CALLAGREEMENT _23to4(LPINFOSTR psou)
{
	int row_bit_count, linesize, i, j;
	unsigned int code;
	unsigned char *p, *pdes;
	CRITICAL_SECTION  actmp;

	LPINFOSTR presult = isirw_alloc_INFOSTR();

	if (!presult)
		return 0;

	actmp = presult->access;
	memmove((void*)presult, (const void *)psou, sizeof(INFOSTR));
	presult->access = actmp;

	presult->imgtype = IMT_RESSTATIC;
	presult->bitcount = 4;
	presult->order = 1;
	presult->p_bit_data = 0;
	presult->pp_line_addr = 0;
	presult->pal_count = 1<<4;
	presult->psubimg = 0;
	presult->imgnumbers = 1;

	linesize = DIBSCANLINE_WIDTHBYTES(presult->bitcount*psou->width);

	presult->p_bit_data = (unsigned char *)malloc(linesize*psou->height+4);
	if (!presult->p_bit_data)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}

	/* ��ʼ��Ϊ 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);

	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}

	/* ����ͼ */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));
	
	memmove((void*)presult->palette, (const void *)psou->palette, sizeof(long)*psou->pal_count);

	for (i=0;i<(int)psou->height;i++)
	{
		p = (unsigned char *)psou->pp_line_addr[i];
		pdes = (unsigned char *)(presult->pp_line_addr[i]);
		row_bit_count = 0;
		for (j=0;j<(int)psou->width;j++)
		{
			code = _get_next_pix(p, DIBSCANLINE_WIDTHBYTES(psou->bitcount*psou->width), 
				row_bit_count/8, row_bit_count%8, psou->bitcount);
			row_bit_count += psou->bitcount;

			if (j%2)
				code <<= 4;

			*(unsigned char *)(pdes+j/2) |= (unsigned char)code;
		}
	}

	return presult;
}


/* ��5��6��7��8λ��ͼ��ת��Ϊ��׼8λλͼ��ʽ */
CE_API LPINFOSTR CALLAGREEMENT _567to8(LPINFOSTR psou)
{
	int row_bit_count, linesize, i, j;
	unsigned int code;
	unsigned char *p, *pdes;
	CRITICAL_SECTION  actmp;
	
	LPINFOSTR presult = isirw_alloc_INFOSTR();
	
	if (!presult)
		return 0;

	actmp = presult->access;
	memmove((void*)presult, (const void *)psou, sizeof(INFOSTR));
	presult->access = actmp;
	
	presult->imgtype = IMT_RESSTATIC;
	presult->bitcount = 8;
	presult->order = 1;
	presult->p_bit_data = 0;
	presult->pp_line_addr = 0;
	presult->pal_count = 1<<8;
	presult->psubimg = 0;
	presult->imgnumbers = 1;
	
	linesize = DIBSCANLINE_WIDTHBYTES(presult->bitcount*psou->width);
	
	presult->p_bit_data = (unsigned char *)malloc(linesize*psou->height+4);
	if (!presult->p_bit_data)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ����ͼ */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));
	
	
	for (i=0;i<(int)psou->height;i++)
	{
		p = (unsigned char *)psou->pp_line_addr[i];
		pdes = (unsigned char *)(presult->pp_line_addr[i]);
		row_bit_count = 0;
		for (j=0;j<(int)psou->width;j++)
		{
			code = _get_next_pix(p, DIBSCANLINE_WIDTHBYTES(psou->bitcount*psou->width), 
				row_bit_count/8, row_bit_count%8, psou->bitcount);
			row_bit_count += psou->bitcount;
			
			*(unsigned char *)(pdes+j) = (unsigned char)code;
		}
	}
	
	return presult;
}


/* ��ȡ��һ���������ݣ����ֽڰ棩*/
unsigned int CALLAGREEMENT _get_next_pix(
						 unsigned char *p, 
						 int stream_len, 
						 int offset_byte, 
						 int offset_bit, 
						 int bit_len)
{
	union{unsigned char b[2]; unsigned short us;} code;
	
	assert(p);
	
	if ((offset_byte+0) < stream_len)
		code.b[0] = *(unsigned char *)(p+offset_byte);
	if ((offset_byte+1) < stream_len)
		code.b[1] = *(unsigned char *)(p+offset_byte+1);
	
	code.us >>= offset_bit;
	code.us &= (1<<bit_len)-1;
	
	return (unsigned int)code.us;
}


/* ��9��10��11��12��13��14��15��16λ��ͼ��ת��Ϊ��׼16λ(555)λͼ��ʽ */
CE_API LPINFOSTR CALLAGREEMENT _9_10_11_12_13_14_15to16(LPINFOSTR psou)
{
	int row_bit_count, linesize, i, j;
	unsigned long	code;
	unsigned short	descode, sr, sb, sg;
	unsigned short	mr, mb, mg, dmr, dmb, dmg;
	int				br, bb, bg, dbr, dbb, dbg;
	unsigned char	*p;
	unsigned short	*pdes;
	CRITICAL_SECTION  actmp;
	
	/* ����һ���µ����ݰ������ڴ��ת��������� */
	LPINFOSTR presult = isirw_alloc_INFOSTR();
	
	if (!presult)
		return 0;
	
	actmp = presult->access;
	memmove((void*)presult, (const void *)psou, sizeof(INFOSTR));
	presult->access = actmp;
	
	presult->imgtype = IMT_RESSTATIC;
	presult->bitcount = 16;
	presult->order = 1;
	presult->p_bit_data = 0;
	presult->pp_line_addr = 0;
	presult->pal_count = 0;
	presult->psubimg = 0;
	presult->imgnumbers = 1;
	
	linesize = DIBSCANLINE_WIDTHBYTES(presult->bitcount*psou->width);
	
	presult->p_bit_data = (unsigned char *)malloc(linesize*psou->height+4);
	if (!presult->p_bit_data)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ����ͼ */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));
	
	/* Ŀ���ʽΪ16λ555 */
	presult->r_mask = 0x7c00;
	presult->g_mask = 0x3e0;
	presult->b_mask = 0x1f;
	presult->a_mask = 0;

	/* ���Դ��ʽ��Ŀ���ʽ��ͬ��������ת����ֱ�ӿ��� */
	if ((psou->bitcount == 16)&&(psou->r_mask == presult->r_mask)&& \
		(psou->g_mask == presult->g_mask)&&(psou->b_mask == presult->b_mask)&& \
		(psou->a_mask == presult->a_mask))
	{
		for (i=0;i<(int)psou->height;i++)
		{
			p = (unsigned char *)psou->pp_line_addr[i];
			pdes = (unsigned short *)(presult->pp_line_addr[i]);
			memmove((void*)pdes, (const void *)p, linesize);
		}
		return presult;
	}
	
	/* ��ȡԭͼ���������� */
	mr = (unsigned short)psou->r_mask;
	mg = (unsigned short)psou->g_mask;
	mb = (unsigned short)psou->b_mask;

	/* ��ȡ����ƫ�� */
	br = _get_offset_bits((unsigned long)psou->r_mask);
	bg = _get_offset_bits((unsigned long)psou->g_mask);
	bb = _get_offset_bits((unsigned long)psou->b_mask);

	/* ������0λ */
	mr >>= br;
	mg >>= bg;
	mb >>= bb;

	dmr = (unsigned short)presult->r_mask;
	dmg = (unsigned short)presult->g_mask;
	dmb = (unsigned short)presult->b_mask;
	
	/* ��ȡ����ƫ�� */
	dbr = _get_offset_bits((unsigned long)presult->r_mask);
	dbg = _get_offset_bits((unsigned long)presult->g_mask);
	dbb = _get_offset_bits((unsigned long)presult->b_mask);
	
	/* ������0λ */
	dmr >>= dbr;
	dmg >>= dbg;
	dmb >>= dbb;

	/* ת���������� */
	for (i=0;i<(int)psou->height;i++)
	{
		p = (unsigned char *)psou->pp_line_addr[i];
		pdes = (unsigned short *)(presult->pp_line_addr[i]);
		row_bit_count = 0;

		for (j=0;j<(int)psou->width;j++)
		{
			/* ��ȡ��һ������ */
			code = _get_next_pix2(p, row_bit_count/8, row_bit_count%8, psou->bitcount);
			row_bit_count += psou->bitcount;

			/* ���������ɫ���� */
			sr = ((unsigned short)code>>br)&mr;
			sg = ((unsigned short)code>>bg)&mg;
			sb = ((unsigned short)code>>bb)&mb;

			/* ���Դ������Χ����Ŀ�������Χ������������λ���Ƴ���λ */
			if (mr > dmr)
			{
				unsigned short shr = mr;
				while (shr > dmr)
				{
					sr >>= 1; shr >>= 1;
				}
			}
			if (mg > dmg)
			{
				unsigned short shr = mg;
				while (shr > dmg)
				{
					sg >>= 1; shr >>= 1;
				}
			}
			if (mb > dmb)
			{
				unsigned short shr = mb;
				while (shr > dmb)
				{
					sb >>= 1; shr >>= 1;
				}
			}

			sr <<= dbr;
			sg <<= dbg;
			sb <<= dbb;
	
			/* �ϳ����� */
			descode = sr|sg|sb;

			/* ��д�뻺���� */
			*(unsigned short *)(pdes+j) = (unsigned short)descode;
		}
	}
	
	return presult;
}


/* ��17��18��19��20��21��22��23��24λ��ͼ��ת��Ϊ��׼24λ(888)λͼ��ʽ */
CE_API LPINFOSTR CALLAGREEMENT _17_18_19_20_21_22_23to24(LPINFOSTR psou)
{
	int row_bit_count, linesize, i, j;
	unsigned long	code;
	unsigned long	descode, sr, sb, sg;
	unsigned long	mr, mb, mg, dmr, dmb, dmg;
	int				br, bb, bg, dbr, dbb, dbg;
	unsigned char	*p;
	unsigned char	*pdes;
	CRITICAL_SECTION  actmp;
	
	LPINFOSTR presult = isirw_alloc_INFOSTR();
	
	if (!presult)
		return 0;
	
	actmp = presult->access;
	memmove((void*)presult, (const void *)psou, sizeof(INFOSTR));
	presult->access = actmp;
	
	presult->imgtype = IMT_RESSTATIC;
	presult->bitcount = 24;
	presult->order = 1;
	presult->p_bit_data = 0;
	presult->pp_line_addr = 0;
	presult->pal_count = 0;
	presult->psubimg = 0;
	presult->imgnumbers = 1;
	
	linesize = DIBSCANLINE_WIDTHBYTES(presult->bitcount*psou->width);
	
	presult->p_bit_data = (unsigned char *)malloc(linesize*psou->height+4);
	if (!presult->p_bit_data)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ����ͼ */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));
	
	/* Ŀ���ʽΪ24λ */
	presult->r_mask = 0xff0000;
	presult->g_mask = 0xff00;
	presult->b_mask = 0xff;
	presult->a_mask = 0;


	/* ���Դ��ʽ��Ŀ���ʽ��ͬ��������ת����ֱ�ӿ��� */
	if ((psou->bitcount == 24)&&(psou->r_mask == presult->r_mask)&& \
		(psou->g_mask == presult->g_mask)&&(psou->b_mask == presult->b_mask)&& \
		(psou->a_mask == presult->a_mask))
	{
		for (i=0;i<(int)psou->height;i++)
		{
			p = (unsigned char *)psou->pp_line_addr[i];
			pdes = (unsigned char *)(presult->pp_line_addr[i]);
			memmove((void*)pdes, (const void *)p, linesize);
		}
		return presult;
	}

	mr = (unsigned long)psou->r_mask;
	mg = (unsigned long)psou->g_mask;
	mb = (unsigned long)psou->b_mask;
	
	/* ��ȡ����ƫ�� */
	br = _get_offset_bits((unsigned long)psou->r_mask);
	bg = _get_offset_bits((unsigned long)psou->g_mask);
	bb = _get_offset_bits((unsigned long)psou->b_mask);
	
	mr >>= br;
	mg >>= bg;
	mb >>= bb;
	
	dmr = (unsigned short)presult->r_mask;
	dmg = (unsigned short)presult->g_mask;
	dmb = (unsigned short)presult->b_mask;
	
	/* ��ȡ����ƫ�� */
	dbr = _get_offset_bits((unsigned long)presult->r_mask);
	dbg = _get_offset_bits((unsigned long)presult->g_mask);
	dbb = _get_offset_bits((unsigned long)presult->b_mask);
	
	dmr >>= dbr;
	dmg >>= dbg;
	dmb >>= dbb;
	
	for (i=0;i<(int)psou->height;i++)
	{
		p = (unsigned char *)psou->pp_line_addr[i];
		pdes = (unsigned char *)(presult->pp_line_addr[i]);
		row_bit_count = 0;
		for (j=0;j<(int)psou->width;j++)
		{
			code = _get_next_pix2(p, row_bit_count/8, row_bit_count%8, psou->bitcount);
			row_bit_count += psou->bitcount;

			sr = ((unsigned long)code>>br)&mr;
			sg = ((unsigned long)code>>bg)&mg;
			sb = ((unsigned long)code>>bb)&mb;
			
			if (mr > dmr)
			{
				unsigned long shr = mr;
				while (shr > dmr)
				{
					sr >>= 1; shr >>= 1;
				}
			}
			if (mg > dmg)
			{
				unsigned long shr = mg;
				while (shr > dmg)
				{
					sg >>= 1; shr >>= 1;
				}
			}
			if (mb > dmb)
			{
				unsigned long shr = mb;
				while (shr > dmb)
				{
					sb >>= 1; shr >>= 1;
				}
			}
			
			sr <<= dbr;
			sg <<= dbg;
			sb <<= dbb;
			
			descode = sr|sg|sb;
			
			*pdes++ = (unsigned char)(descode&0xff);
			*pdes++ = (unsigned char)((descode>>8)&0xff);
			*pdes++ = (unsigned char)((descode>>16)&0xff);
		}
	}
	
	return presult;
}


/* ��25��26��27��28��29��30��31��32λ��ͼ��ת��Ϊ��׼32λ(888)λͼ��ʽ */
CE_API LPINFOSTR CALLAGREEMENT _25_26_27_28_29_30_31to32(LPINFOSTR psou)
{
	int row_bit_count, linesize, i, j;
	unsigned long	code;
	unsigned long	descode, sr, sb, sg;
	unsigned long	mr, mb, mg, dmr, dmb, dmg;
	int				br, bb, bg, dbr, dbb, dbg;
	unsigned char	*p;
	unsigned long	*pdes;
	CRITICAL_SECTION  actmp;
	
	LPINFOSTR presult = isirw_alloc_INFOSTR();
	
	if (!presult)
		return 0;
	
	actmp = presult->access;
	memmove((void*)presult, (const void *)psou, sizeof(INFOSTR));
	presult->access = actmp;
	
	presult->imgtype = IMT_RESSTATIC;
	presult->bitcount = 32;
	presult->order = 1;
	presult->p_bit_data = 0;
	presult->pp_line_addr = 0;
	presult->pal_count = 0;
	presult->psubimg = 0;
	presult->imgnumbers = 1;
	
	linesize = DIBSCANLINE_WIDTHBYTES(presult->bitcount*psou->width);
	
	presult->p_bit_data = (unsigned char *)malloc(linesize*psou->height+4);
	if (!presult->p_bit_data)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* ����ͼ */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));

	/* Ŀ���ʽΪ32λ888��ʽ */
	presult->r_mask = 0xff0000;
	presult->g_mask = 0xff00;
	presult->b_mask = 0xff;
	presult->a_mask = 0;
	
	/* ���Դ��ʽ��Ŀ���ʽ��ͬ��������ת����ֱ�ӿ��� */
	if ((psou->bitcount == 32)&&(psou->r_mask == presult->r_mask)&& \
		(psou->g_mask == presult->g_mask)&&(psou->b_mask == presult->b_mask)&& \
		(psou->a_mask == presult->a_mask))
	{
		for (i=0;i<(int)psou->height;i++)
		{
			p = (unsigned char *)psou->pp_line_addr[i];
			pdes = (unsigned long *)(presult->pp_line_addr[i]);
			memmove((void*)pdes, (const void *)p, linesize);
		}
		return presult;
	}
	
	mr = (unsigned long)psou->r_mask;
	mg = (unsigned long)psou->g_mask;
	mb = (unsigned long)psou->b_mask;
	
	/* ��ȡ����ƫ�� */
	br = _get_offset_bits((unsigned long)psou->r_mask);
	bg = _get_offset_bits((unsigned long)psou->g_mask);
	bb = _get_offset_bits((unsigned long)psou->b_mask);
	
	mr >>= br;
	mg >>= bg;
	mb >>= bb;
	
	dmr = (unsigned short)presult->r_mask;
	dmg = (unsigned short)presult->g_mask;
	dmb = (unsigned short)presult->b_mask;
	
	/* ��ȡ����ƫ�� */
	dbr = _get_offset_bits((unsigned long)presult->r_mask);
	dbg = _get_offset_bits((unsigned long)presult->g_mask);
	dbb = _get_offset_bits((unsigned long)presult->b_mask);
	
	dmr >>= dbr;
	dmg >>= dbg;
	dmb >>= dbb;
	
	for (i=0;i<(int)psou->height;i++)
	{
		p = (unsigned char *)psou->pp_line_addr[i];
		pdes = (unsigned long *)(presult->pp_line_addr[i]);
		row_bit_count = 0;
		for (j=0;j<(int)psou->width;j++)
		{
			code = _get_next_pix2(p, row_bit_count/8, row_bit_count%8, psou->bitcount);
			row_bit_count += psou->bitcount;
			
			sr = ((unsigned long)code>>br)&mr;
			sg = ((unsigned long)code>>bg)&mg;
			sb = ((unsigned long)code>>bb)&mb;
			
			if (mr > dmr)
			{
				unsigned long shr = mr;
				while (shr > dmr)
				{
					sr >>= 1; shr >>= 1;
				}
			}
			if (mg > dmg)
			{
				unsigned long shr = mg;
				while (shr > dmg)
				{
					sg >>= 1; shr >>= 1;
				}
			}
			if (mb > dmb)
			{
				unsigned long shr = mb;
				while (shr > dmb)
				{
					sb >>= 1; shr >>= 1;
				}
			}
			
			sr <<= dbr;
			sg <<= dbg;
			sb <<= dbb;
			
			descode = sr|sg|sb;
			
			*(unsigned long *)(pdes+j) = descode;
		}
	}
	
	return presult;
}


/* ��ȡ��һ���������ݣ����ֽڰ棩*/
unsigned long CALLAGREEMENT _get_next_pix2(
							 unsigned char *p, 
							 int offset_byte, 
							 int offset_bit, 
							 int bit_len)
{
	union{unsigned char b[8]; unsigned long  us; __int64 up;} code;

	code.us = *(unsigned long*)(unsigned char *)(p+offset_byte);
	code.b[4] = *(unsigned char *)(p+offset_byte+4);

	code.up &= 0xffffffffff;

	code.up >>= offset_bit;
	code.up &= (1<<bit_len)-1;

	return code.us;
}


/* ��ȡ����λ�ã����������Ƶ�0λʱ������ƶ������� */
int CALLAGREEMENT _get_offset_bits(unsigned long mask)
{
	register result = 0;

	/* ע�������������λ�ģ��������������� */
	__asm
	{
		mov eax, mask;
get_offset_bits_loop:
		shr eax, 1;
		jc  get_offset_bits_endloop;
		inc result;
		cmp result, 32;
		je  get_offset_bits_enderr;
		jmp get_offset_bits_loop;
get_offset_bits_enderr:
		mov result, 0;
get_offset_bits_endloop:
	}

	assert(result < 32);
	
	return (int)result;
}

