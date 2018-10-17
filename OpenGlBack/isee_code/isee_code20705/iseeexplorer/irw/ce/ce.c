/********************************************************************

	ce.c

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
	本文件用途：	ISee 图像转换引擎
	本文件编写人：	YZ			yzfree##sina.com
	
	本文件版本：	20416
	最后修改于：	2002-4-16
	  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
		地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2002-4		修正格式转换函数中颜色分量重叠的BUG，发现者是
					小组成员 tearshark。
		2001-12		小组成员 menghui 发现格式转换函数中缺少对多页
					图象的支持。（此BUG尚未修正）
		2001-7		发布第一个测试版

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



/* 1位图象象素翻转（适应Windows GUI要求）*/
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


/* 4位图象象素翻转（适应Windows GUI要求）*/
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


/* 图象垂直翻转 */
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
		return -1;		/* 内存不足 */
}


/* 将2、3、4位的图象转换为标准4位位图格式 */
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

	/* 初始化为 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);

	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}

	/* 倒向图 */
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


/* 将5、6、7、8位的图象转换为标准8位位图格式 */
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
	
	/* 初始化为 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* 倒向图 */
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


/* 读取下一个象素数据（单字节版）*/
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


/* 将9、10、11、12、13、14、15、16位的图象转换为标准16位(555)位图格式 */
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
	
	/* 申请一个新的数据包，用于存放转换后的数据 */
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
	
	/* 初始化为 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* 倒向图 */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));
	
	/* 目标格式为16位555 */
	presult->r_mask = 0x7c00;
	presult->g_mask = 0x3e0;
	presult->b_mask = 0x1f;
	presult->a_mask = 0;

	/* 如果源格式与目标格式相同，则无需转换，直接拷贝 */
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
	
	/* 获取原图象掩码数据 */
	mr = (unsigned short)psou->r_mask;
	mg = (unsigned short)psou->g_mask;
	mb = (unsigned short)psou->b_mask;

	/* 提取掩码偏移 */
	br = _get_offset_bits((unsigned long)psou->r_mask);
	bg = _get_offset_bits((unsigned long)psou->g_mask);
	bb = _get_offset_bits((unsigned long)psou->b_mask);

	/* 右移至0位 */
	mr >>= br;
	mg >>= bg;
	mb >>= bb;

	dmr = (unsigned short)presult->r_mask;
	dmg = (unsigned short)presult->g_mask;
	dmb = (unsigned short)presult->b_mask;
	
	/* 提取掩码偏移 */
	dbr = _get_offset_bits((unsigned long)presult->r_mask);
	dbg = _get_offset_bits((unsigned long)presult->g_mask);
	dbb = _get_offset_bits((unsigned long)presult->b_mask);
	
	/* 右移至0位 */
	dmr >>= dbr;
	dmg >>= dbg;
	dmb >>= dbb;

	/* 转换象素数据 */
	for (i=0;i<(int)psou->height;i++)
	{
		p = (unsigned char *)psou->pp_line_addr[i];
		pdes = (unsigned short *)(presult->pp_line_addr[i]);
		row_bit_count = 0;

		for (j=0;j<(int)psou->width;j++)
		{
			/* 获取下一个象素 */
			code = _get_next_pix2(p, row_bit_count/8, row_bit_count%8, psou->bitcount);
			row_bit_count += psou->bitcount;

			/* 分离出各颜色分量 */
			sr = ((unsigned short)code>>br)&mr;
			sg = ((unsigned short)code>>bg)&mg;
			sb = ((unsigned short)code>>bb)&mb;

			/* 如果源分量范围大于目标分量范围，则保留分量高位，移出低位 */
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
	
			/* 合成象素 */
			descode = sr|sg|sb;

			/* 回写入缓冲区 */
			*(unsigned short *)(pdes+j) = (unsigned short)descode;
		}
	}
	
	return presult;
}


/* 将17、18、19、20、21、22、23、24位的图象转换为标准24位(888)位图格式 */
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
	
	/* 初始化为 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* 倒向图 */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));
	
	/* 目标格式为24位 */
	presult->r_mask = 0xff0000;
	presult->g_mask = 0xff00;
	presult->b_mask = 0xff;
	presult->a_mask = 0;


	/* 如果源格式与目标格式相同，则无需转换，直接拷贝 */
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
	
	/* 提取掩码偏移 */
	br = _get_offset_bits((unsigned long)psou->r_mask);
	bg = _get_offset_bits((unsigned long)psou->g_mask);
	bb = _get_offset_bits((unsigned long)psou->b_mask);
	
	mr >>= br;
	mg >>= bg;
	mb >>= bb;
	
	dmr = (unsigned short)presult->r_mask;
	dmg = (unsigned short)presult->g_mask;
	dmb = (unsigned short)presult->b_mask;
	
	/* 提取掩码偏移 */
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


/* 将25、26、27、28、29、30、31、32位的图象转换为标准32位(888)位图格式 */
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
	
	/* 初始化为 0 */
	memset((void*)presult->p_bit_data, 0, linesize*psou->height);
	
	presult->pp_line_addr = (void **)malloc(psou->height*sizeof(void*));
	if (!presult->pp_line_addr)
	{
		isirw_free_INFOSTR(presult);
		return 0;
	}
	
	/* 倒向图 */
	for (i=0;i<(int)psou->height;i++)
		presult->pp_line_addr[i] = (void*)(unsigned char *)(presult->p_bit_data+((psou->height-1-i)*linesize));

	/* 目标格式为32位888格式 */
	presult->r_mask = 0xff0000;
	presult->g_mask = 0xff00;
	presult->b_mask = 0xff;
	presult->a_mask = 0;
	
	/* 如果源格式与目标格式相同，则无需转换，直接拷贝 */
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
	
	/* 提取掩码偏移 */
	br = _get_offset_bits((unsigned long)psou->r_mask);
	bg = _get_offset_bits((unsigned long)psou->g_mask);
	bb = _get_offset_bits((unsigned long)psou->b_mask);
	
	mr >>= br;
	mg >>= bg;
	mb >>= bb;
	
	dmr = (unsigned short)presult->r_mask;
	dmg = (unsigned short)presult->g_mask;
	dmb = (unsigned short)presult->b_mask;
	
	/* 提取掩码偏移 */
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


/* 读取下一个象素数据（单字节版）*/
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


/* 获取掩码位置（即掩码右移到0位时所需的移动次数） */
int CALLAGREEMENT _get_offset_bits(unsigned long mask)
{
	register result = 0;

	/* 注：掩码必须是连位的，不能有跳接现象 */
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

