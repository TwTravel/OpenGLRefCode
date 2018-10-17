/********************************************************************

	bmp.c

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
	���ļ���;��	ISeeͼ���������BMPͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�Windows ��� ��1��32λλͼ��RLEλͼ
							  OS/2 ��� �� 1��4��8��24λλͼ
					���湦�ܣ�Windows ��� ��1��4��8��16(555)��24��
								32(888)λλͼ

	���ļ���д�ˣ�	YZ			yzfree##sina.com

	���ļ��汾��	10709
	����޸��ڣ�	2001-7-9

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2001-7		����RLE�����е�һ������
					1��4λͼ���������ISeeλ����ʽ
					����Ŀ��ͼ�񻺳��������ֽ�
					��ǿ���ݴ���

		2001-6		�����°����
		2001-5		���¶���ģ��Ľӿڼ��������
		2000-10		������һ����Ϊ�ش��BUG
		2000-7		��һ�����԰淢��


********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */

/*###################################################################

  ��ֲע�ͣ����´���ʹ����WIN32ϵͳ��SEH(�ṹ���쳣����)�����߳�ͬ��
			���󡰹ؼ��Ρ�������ֲʱӦתΪLinux����Ӧ��䡣

  #################################################################*/


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* ����windows.h�ļ��ı���ʱ�� */
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "bmp.h"


IRWP_INFO			bmp_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	bmp_get_info_critical;	/* bmp_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	bmp_load_img_critical;	/* bmp_load_image�����Ĺؼ��� */
CRITICAL_SECTION	bmp_save_img_critical;	/* bmp_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _get_mask(LPBMP_INFO pInfo, unsigned long * pb_mask, 
			  unsigned long * pg_mask, unsigned long * pr_mask, unsigned long * pa_mask);
unsigned char * CALLAGREEMENT _write2buff4RLE(int, int, unsigned char, unsigned char *);
unsigned char * CALLAGREEMENT _write2buff8RLE(int, int, unsigned char, unsigned char *);
int CALLAGREEMENT _calcu_scanline_size(int w, int bit);
int CALLAGREEMENT _bmp_is_valid_img(LPINFOSTR pinfo_str);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&bmp_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&bmp_get_info_critical);
			InitializeCriticalSection(&bmp_load_img_critical);
			InitializeCriticalSection(&bmp_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&bmp_get_info_critical);
			DeleteCriticalSection(&bmp_load_img_critical);
			DeleteCriticalSection(&bmp_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

BMP_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&bmp_irwp_info;
}

#else

BMP_API LPIRWP_INFO CALLAGREEMENT bmp_get_plugin_info()
{
	_init_irwp_info(&bmp_irwp_info);

	return (LPIRWP_INFO)&bmp_irwp_info;
}

BMP_API void CALLAGREEMENT bmp_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

BMP_API void CALLAGREEMENT bmp_detach_plugin()
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

	/* ����bmpģ��֧�ֵı���λ�� */
	/* ################################################################# */
	/* ## 2001-8-9  YZ �޸ģ���λ����һ������32λ��ʽ�޷���ʾ��*/
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(4-1)) | (1UL<<(8-1)) | \
		(1UL<<(16-1)) | (1UL<<(24-1)) | (1UL<<(32-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1��ֻ�ܱ���һ��ͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 1;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"����BMP��ʵҲͦ���ӵ�:)");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	/* ������������Ϣ�ɼ��ڴ˴���
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"##");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	*/
	/* ------------------------------------------------------------- */


	/* ���������Ϣ����չ����Ϣ��*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* ����������##���ֶ������� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 2;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
				(const char *)"DIB");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
				(const char *)"RLE");

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
BMP_API int CALLAGREEMENT bmp_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	BMP_FILE_HEADER bfh;
	LPBMP_INFO		pdibinfo = (LPBMP_INFO)0;
	unsigned long	dib_info_len = 0;
	int				style;				/* λͼ���0��WINDOWS��1��OS/2 */

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&bmp_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ��ȡ�ļ�ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&bfh, sizeof(BMP_FILE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж��Ƿ�����Ч��λͼ�ļ� */
			if (bfh.bfType == DIB_HEADER_MARKER)
				style = 0;
			else if (bfh.bfType == DIB_HEADER_MARKER_OS2)
				style = 1;
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ��Ϣͷ����ɫ������OS/2��� */
			dib_info_len = bfh.bfOffBits - sizeof(BMP_FILE_HEADER);

			/* ��һЩͼ�񱣴�Ĳ��Ǻ����棬ͼ���ǩ��Windows��񣬶�����ȴ��OS/2��� */
			if (style == 0)
				if (dib_info_len == sizeof(BMP_CORE_HEADER))
					style = 1;

			if ((pdibinfo = (LPBMP_INFO)malloc(dib_info_len)) == (LPBMP_INFO)0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, sizeof(BMP_FILE_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)pdibinfo, dib_info_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ������Ч�Լ��� */
			if (style == 0)
			{
				if ((pdibinfo->bmiHeader.biHeight == 0)||(pdibinfo->bmiHeader.biWidth == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
			{
				assert(style == 1);
				if ((((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight == 0)||(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_BMP;			/* ͼ���ļ���ʽ����׺���� */

			if (style == 0)
			{
				switch(pdibinfo->bmiHeader.biCompression)	/* ͼ���ѹ����ʽ */
				{
				case	0:
					pinfo_str->compression = ICS_RGB; break;
				case	1:
					pinfo_str->compression = ICS_RLE8; break;
				case	2:
					pinfo_str->compression = ICS_RLE4; break;
				case	3:
					pinfo_str->compression = ICS_BITFIELDS; break;
				default:
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
				pinfo_str->compression = ICS_RGB;	/* OS/2 ��ѹ��λͼ��ʽ */
			

			/* ��дͼ����Ϣ */
			if (style == 0)
			{
				pinfo_str->width	= pdibinfo->bmiHeader.biWidth;
				pinfo_str->height	= (unsigned long)abs(pdibinfo->bmiHeader.biHeight);
				pinfo_str->order	= (pdibinfo->bmiHeader.biHeight < 0) ? 0 : 1;
				pinfo_str->bitcount	= pdibinfo->bmiHeader.biBitCount;
				
				if (_get_mask(pdibinfo, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
			{
				pinfo_str->width	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth);
				pinfo_str->height	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight);
				pinfo_str->order	= 1;	/* OS/2λͼ���ǵ���� */
				pinfo_str->bitcount	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcBitCount);

				/* OS/2 λͼ����Чλ��ֻ�У�1��4��8��24 */
				if (pinfo_str->bitcount == 24)
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else
				{
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					pinfo_str->a_mask = 0x0;
				}
			}
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pdibinfo)
				free(pdibinfo);
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&bmp_get_info_critical);
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
BMP_API int CALLAGREEMENT bmp_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	BMP_FILE_HEADER bfh;
	LPBMP_INFO		pdibinfo = (LPBMP_INFO)0;

	unsigned long	dib_info_len;		/* DIB��Ϣ�ṹ�ߴ� */
	unsigned long	dib_data_len;		/* DIBλ���ݳߴ� */
	unsigned int	pal_num;			/* ��ɫ������� */
	int				i, linesize;
	int				style;				/* λͼ���0��WINDOWS��1��OS/2 */
	
	int				bEOF = 0;			/* ͼ���β��־ */
	int				line = 0;			/* ��ǰ�� */
	unsigned char	pix, pix2;			/* ��ǰֵ */

	unsigned char	buf, tmp, *pc;
	unsigned short	us;
	int				cnv_count, bit, mask;
	
	unsigned char	*pbit = (unsigned char *)0;
	unsigned char	*prle = (unsigned char *)0;
	unsigned char	*prlesou = (unsigned char *)0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&bmp_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ���ļ�ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&bfh, sizeof(BMP_FILE_HEADER), 1, pfile) == 0)
			{
				b_status = (pinfo_str->data_state == 1) ? ER_FILERWERR:ER_NONIMAGE; 
				__leave;
			}
			
			/* �ж��Ƿ�����Ч��λͼ�ļ� */
			if (bfh.bfType == DIB_HEADER_MARKER)
				style = 0;
			else if (bfh.bfType == DIB_HEADER_MARKER_OS2)
				style = 1;
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}


			/* ���� BMP_INFO �ĳߴ� */
			dib_info_len = bfh.bfOffBits - sizeof(BMP_FILE_HEADER);
			
			/* ��һЩͼ�񱣴�Ĳ��Ǻ����棬ͼ���ǩ��Windows��񣬶�����ȴ��OS/2��� */
			if (style == 0)
				if (dib_info_len == sizeof(BMP_CORE_HEADER))
					style = 1;
				
			/* ������ BMP_INFO ��Ϣ���ڴ�� */
			if ((pdibinfo = (LPBMP_INFO)malloc(dib_info_len)) == (LPBMP_INFO)0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ����Ϣͷ�ṹ */
			if (isio_seek(pfile, sizeof(BMP_FILE_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)pdibinfo, dib_info_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ������Ч�Լ��� */
				if (style == 0)
				{
					if ((pdibinfo->bmiHeader.biHeight == 0)||(pdibinfo->bmiHeader.biWidth == 0))
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
				{
					assert(style == 1);
					if ((((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight == 0)||(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth == 0))
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}

				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_BMP;			/* ͼ���ļ���ʽ����׺���� */
				
				if (style == 0)
				{
					switch(pdibinfo->bmiHeader.biCompression)	/* ͼ���ѹ����ʽ */
					{
					case	0:
						pinfo_str->compression = ICS_RGB; break;
					case	1:
						pinfo_str->compression = ICS_RLE8; break;
					case	2:
						pinfo_str->compression = ICS_RLE4; break;
					case	3:
						pinfo_str->compression = ICS_BITFIELDS; break;
					default:
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
					pinfo_str->compression = ICS_RGB;

				if (style == 0)
				{
					pinfo_str->width	= pdibinfo->bmiHeader.biWidth;
					pinfo_str->height	= (unsigned long)abs(pdibinfo->bmiHeader.biHeight);
					pinfo_str->order	= (pdibinfo->bmiHeader.biHeight < 0) ? 0 : 1;
					pinfo_str->bitcount	= pdibinfo->bmiHeader.biBitCount;
					
					if (_get_mask(pdibinfo, 
						(unsigned long *)(&pinfo_str->b_mask), 
						(unsigned long *)(&pinfo_str->g_mask), 
						(unsigned long *)(&pinfo_str->r_mask),
						(unsigned long *)(&pinfo_str->a_mask)) == 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
				{
					pinfo_str->width	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth);
					pinfo_str->height	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight);
					pinfo_str->order	= 1;	/* OS/2λͼ���ǵ���� */
					pinfo_str->bitcount	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcBitCount);
					
					/* OS/2 λͼ����Чλ��ֻ�У�1��4��8��24 */
					if (pinfo_str->bitcount == 24)
					{
						pinfo_str->b_mask = 0xff;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->r_mask = 0xff0000;
						pinfo_str->a_mask = 0x0;
					}
					else
					{
						pinfo_str->b_mask = 0x0;
						pinfo_str->g_mask = 0x0;
						pinfo_str->r_mask = 0x0;
						pinfo_str->a_mask = 0x0;
					}
				}
				
				pinfo_str->data_state = 1;
			}


			/* ���õ�ɫ������ */
			if (pinfo_str->bitcount <= 8)
			{
				pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

				if (style == 0)
				{
					pal_num = (pdibinfo->bmiHeader.biClrUsed == 0) ? pinfo_str->pal_count : pdibinfo->bmiHeader.biClrUsed;
					if (pal_num > pinfo_str->pal_count)
					{
						b_status = ER_NONIMAGE; __leave;
					}
					memmove((void*)(pinfo_str->palette), (const void *)(pdibinfo->bmiColors), sizeof(RGB_QUAD)*pal_num);
				}
				else
				{
					for (i=0;i<(int)pinfo_str->pal_count;i++)
					{
						((LPRGB_TRIPLE)(&pinfo_str->palette[i]))->rgbtBlue = ((LPBMP_CORE)pdibinfo)->bmciColors[i].rgbtBlue;
						((LPRGB_TRIPLE)(&pinfo_str->palette[i]))->rgbtGreen = ((LPBMP_CORE)pdibinfo)->bmciColors[i].rgbtGreen;
						((LPRGB_TRIPLE)(&pinfo_str->palette[i]))->rgbtRed = ((LPBMP_CORE)pdibinfo)->bmciColors[i].rgbtRed;
						pinfo_str->palette[i] &= 0xffffff;	/* ����ֽ����� */
					}
				}
			}
			else
			{
				pinfo_str->pal_count = 0;
			}


			/* ȡ��λ���ݳ��� */
			dib_data_len = isio_length(pfile) - bfh.bfOffBits;
			/* ȡ��ɨ���гߴ� */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			/* ����ͼ����� */
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;
			
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


			/* �Ƿ���RLE */
			if ((pinfo_str->compression == ICS_RLE4)||(pinfo_str->compression == ICS_RLE8))
			{
				assert(style == 0);

				/* �����ڴ棬���ڴ��RLEԭʼ���� */
				prlesou = prle = (unsigned char *)malloc(dib_data_len);
				if (!prle)
				{
					b_status = ER_MEMORYERR; __leave;
				}

				/* ��ȡRLEԭʼ���� */
				if (isio_seek(pfile, bfh.bfOffBits, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				if (isio_read((void*)prle, dib_data_len, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ΪRLE��ѹ��׼������ʱ��Ϊдָ�����飩 */
				for (i=0;i<(int)(pinfo_str->height);i++)
					pinfo_str->pp_line_addr[i] = (void*)(pinfo_str->p_bit_data+(i*linesize));
				
				if (pinfo_str->compression == ICS_RLE4)
				{
					_write2buff4RLE(0, 0, 0, (unsigned char*)pinfo_str->pp_line_addr[line]);	/* ��ʼ��д���� */

					/* �Ƚ�RLE�����ݽ�ѹ�ɱ�׼��BMP���� */
					while(!bEOF)
					{
						pix = *prle++;
						
						if (pix == 0)	/* ���Է�ʽ */
						{
							pix2 = *prle++;
							switch(pix2)
							{
							case	0:		/* �н�β */
								line++;
								_write2buff4RLE(2, 0, 0, (unsigned char *)pinfo_str->pp_line_addr[line]);/* ָ���µ�ƫ�Ƶ�ַ */
								break;
							case	1:		/* ͼ���β */
								bEOF = 1;
								break;
							case	2:		/* ��ǰ��ַƫ�� */
								pix = *prle++; pix2 = *prle++;
								line += pix2;	/* ����pix2�� */
								_write2buff4RLE(2, pix%2, 0, (unsigned char *)((unsigned char *)pinfo_str->pp_line_addr[line]+(unsigned long)pix/2));
								break;
							default:		/* ����ѹ�������� */
								for (i=0;i<pix2/2;i++)
								{
									pix = *(prle+i);
									_write2buff4RLE(1, 2, pix, (unsigned char *)0);
								}
								if (pix2%2)
								{	/* ������©�ĵ������� */
									pix = *(prle+i);
									_write2buff4RLE(1, 1, pix, (unsigned char *)0);
								}
								/* (pix2+1)/2�ǽ����ض��뵽�ֽڱ߽��ϣ��ټ�1����2�Ƕ��뵽�ֱ߽��� */
								prle = (unsigned char *)(prle + ((pix2+1)/2+1)/2*2);	/* �ֶ��� */
								break;
							}
						}
						else			/* ���뷽ʽ */
						{
							/* pix == ��ͬ���صĸ��� */
							pix2 = *prle++;	/* pix2 == ���� */
							_write2buff4RLE(1, (int)pix, pix2, (unsigned char *)0);
						}
					}
					
					_write2buff4RLE(3, 0, 0, (unsigned char *)0);	/* ����д���� */

					/* 4λλͼ���ߵ��ֽ����������� */
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						pc = (unsigned char *)(pinfo_str->pp_line_addr[i]);
						for (cnv_count=0;cnv_count<linesize;cnv_count++)
						{
							buf = *(unsigned char *)(pc+cnv_count);
							tmp = (buf>>4)|(buf<<4);
							*(unsigned char *)(pc+cnv_count) = tmp;
						}
					}
				}
				else
				{
					_write2buff8RLE(0, 0, 0, (unsigned char *)pinfo_str->pp_line_addr[line]);	/* ��ʼ��д���� */
					
					while(!bEOF)
					{
						pix = *prle++;
						
						if (pix == 0)	/* ���Է�ʽ */
						{
							pix2 = *prle++;
							switch(pix2)
							{
							case	0:		/* �н�β */
								line++;
								_write2buff8RLE(2, 0, 0, (unsigned char *)pinfo_str->pp_line_addr[line]);/* ָ���µ�ƫ�Ƶ�ַ */
								break;
							case	1:		/* ͼ���β */
								bEOF = 1;
								break;
							case	2:		/* ��ǰ��ַƫ�� */
								pix = *prle++; pix2 = *prle++;
								line += pix2;	/* ����pix2�� */
								_write2buff8RLE(2, 0, 0, (unsigned char *)((unsigned char *)pinfo_str->pp_line_addr[line]+(unsigned long)pix));
								break;
							default:		/* ����ѹ�������� */
								for (i=0;i<pix2;i++)
								{
									pix = *(prle+i);
									_write2buff8RLE(1, 1, pix, (unsigned char *)0);
								}
								/* (pix2+1)/2�ǽ����ض��뵽�ֽڱ߽��ϣ��ټ�1����2�Ƕ��뵽�ֱ߽��� */
								prle = (unsigned char *)(prle + ((pix2+1)/2)*2);	/* �ֶ��� */
								break;
							}
						}
						else			/* ���뷽ʽ */
						{
							/* pix == ��ͬ���صĸ��� */
							pix2 = *prle++;	/* pix2 == ���� */
							_write2buff8RLE(1, (int)pix, pix2, (unsigned char *)0);
						}
					}
					
					_write2buff8RLE(3, 0, 0, (unsigned char *)0);	/* ����д���� */
				}

				/* ��ʼ�����׵�ַ��RLEλͼ���ǵ���ģ� */
				for (i=0;i<(int)(pinfo_str->height);i++)
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
			else
			{
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

				/* ��ȡλ���ݵ��ڴ� */
				if (isio_seek(pfile, bfh.bfOffBits, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pbit = pinfo_str->p_bit_data;

				/* ���ж��� */
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					if (isio_read((void*)pbit, linesize, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* �����1λλͼ����ߵ��ֽ����������� */
					if (pinfo_str->bitcount == 1)
					{
						for (cnv_count=0;cnv_count<linesize;cnv_count++)
						{
							buf = *(unsigned char *)(pbit+cnv_count);
							us  = ((unsigned short)buf)<<8;
							tmp = 0;
							for (bit=1, mask=7;bit<16;bit+=2, mask--)
								tmp |= (us>>bit)&(1<<mask);

							*(unsigned char *)(pbit+cnv_count) = tmp;
						}
					}

					/* �����4λλͼ����ߵ��ֽ����������� */
					if (pinfo_str->bitcount == 4)
					{
						for (cnv_count=0;cnv_count<linesize;cnv_count++)
						{
							buf = *(unsigned char *)(pbit+cnv_count);
							tmp = (buf>>4)|(buf<<4);
							*(unsigned char *)(pbit+cnv_count) = tmp;
						}
					}

					pinfo_str->process_current = i+1;
					pbit = (unsigned char *)(pbit + linesize);

					/* ֧���û��ж� */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
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

			if (prlesou)
				free(prlesou);
			if (pdibinfo)
				free(pdibinfo);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&bmp_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� ��ֻ�ܱ���Windows����λͼ��*/
BMP_API int CALLAGREEMENT bmp_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	BMP_FILE_HEADER bfh;
	LPBMP_INFO		pdibinfo = (LPBMP_INFO)0;
	
	unsigned long	dib_info_len;		/* DIB��Ϣ�ṹ�ߴ� */
	unsigned char	*plinebuf = (unsigned char *)0;
	int				i, linesize;
	
	unsigned char	buf, tmp;
	unsigned short	us;
	int				cnv_count, bit, mask;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&bmp_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_bmp_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}


			/* ����λͼ��Ϣͷ�ṹ�ĳߴ磨������ɫ�壩 */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	4:
			case	8:
				dib_info_len = sizeof(BMP_INFO_HEADER) + pinfo_str->pal_count*sizeof(RGB_QUAD);
				break;
			case	24:
				dib_info_len = sizeof(BMP_INFO_HEADER);
				break;
			case	16:
			case	32:
				dib_info_len = sizeof(BMP_INFO_HEADER) + 3 * sizeof(unsigned long);
				break;
			default:
				assert(0);
				b_status = ER_SYSERR; __leave;
			}

			/* ������ BMP_INFO ��Ϣ���ڴ�� */
			if ((pdibinfo = (LPBMP_INFO)malloc(dib_info_len)) == (LPBMP_INFO)0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			pdibinfo->bmiHeader.biSize			= sizeof(BMP_INFO_HEADER);
			pdibinfo->bmiHeader.biPlanes		= 1;
			pdibinfo->bmiHeader.biBitCount		= (unsigned short)pinfo_str->bitcount;
			pdibinfo->bmiHeader.biCompression	= ((pinfo_str->bitcount == 16)||(pinfo_str->bitcount == 32)) ? 3 : 0;
			pdibinfo->bmiHeader.biWidth			= pinfo_str->width;
			/* ȫ������Ϊ����ͼ */
			pdibinfo->bmiHeader.biHeight		= (long)pinfo_str->height;
			pdibinfo->bmiHeader.biSizeImage		= 0;
			pdibinfo->bmiHeader.biClrImportant	= 0;
			pdibinfo->bmiHeader.biClrUsed		= 0;
			pdibinfo->bmiHeader.biXPelsPerMeter = 0;
			pdibinfo->bmiHeader.biYPelsPerMeter = 0;

			if (pdibinfo->bmiHeader.biCompression == 0)
				memmove((void*)pdibinfo->bmiColors, (const void *)pinfo_str->palette, pinfo_str->pal_count * sizeof(RGB_QUAD));
			else
			{
				*((unsigned long *)&(pdibinfo->bmiColors[0])) = pinfo_str->r_mask;
				*((unsigned long *)&(pdibinfo->bmiColors[1])) = pinfo_str->g_mask;
				*((unsigned long *)&(pdibinfo->bmiColors[2])) = pinfo_str->b_mask;
			}

			/* ��дλͼ�ļ�ͷ�ṹ */
			bfh.bfSize		= sizeof(BMP_FILE_HEADER);
			bfh.bfType		= DIB_HEADER_MARKER;
			bfh.bfOffBits	= bfh.bfSize+dib_info_len;
			bfh.bfReserved1 = bfh.bfReserved2 = 0;
				
			/* ���ý���ֵ */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ȡ��ɨ���гߴ� */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			
			/* ����д���ݻ����� */
			if ((plinebuf=(unsigned char *)malloc(linesize)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д���ļ�ͷ�ṹ */
			if (isio_write((const void *)&bfh, sizeof(BMP_FILE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* д����Ϣͷ�ṹ */
			if (isio_write((const void *)pdibinfo, dib_info_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д��ͼ��λ���� 
				ע�������ͼ�����ݱ�����ISeeλ����ʽ��������ܻ����ʧ��
			*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				memcpy((void*)plinebuf, (const void *)(unsigned char *) \
					(pinfo_str->pp_line_addr[pinfo_str->height-1-i]), linesize);

				/* �����1λλͼ����ߵ��ֽ����������� */
				if (pinfo_str->bitcount == 1)
				{
					for (cnv_count=0;cnv_count<linesize;cnv_count++)
					{
						buf = *(unsigned char *)(plinebuf+cnv_count);
						us  = ((unsigned short)buf)<<8;
						tmp = 0;
						for (bit=1, mask=7;bit<16;bit+=2, mask--)
							tmp |= (us>>bit)&(1<<mask);
						
						*(unsigned char *)(plinebuf+cnv_count) = tmp;
					}
				}
				
				/* �����4λλͼ����ߵ��ֽ����������� */
				if (pinfo_str->bitcount == 4)
				{
					for (cnv_count=0;cnv_count<linesize;cnv_count++)
					{
						buf = *(unsigned char *)(plinebuf+cnv_count);
						tmp = (buf>>4)|(buf<<4);
						*(unsigned char *)(plinebuf+cnv_count) = tmp;
					}
				}
				
				/* һ�ɱ���Ϊ����ͼ */
				if (isio_write((const void*)plinebuf, linesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i+1;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}

			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (plinebuf)
				free(plinebuf);
			if (pdibinfo)
				free(pdibinfo);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&bmp_save_img_critical);
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


/* ��ȡ����ɫ������ֵ */
int CALLAGREEMENT _get_mask(LPBMP_INFO pInfo, 
	unsigned long * pb_mask, 
	unsigned long * pg_mask, 
	unsigned long * pr_mask, 
	unsigned long * pa_mask)
{
	int result = 0;
	unsigned long *pMark;

	assert(pInfo->bmiHeader.biSize);

	switch(pInfo->bmiHeader.biBitCount)
	{
	case	1:
	case	4:
	case	8:
		/* ��ɫ��λͼ������ */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:
		/*******************************************
				16λ����ʾ��ͼ

			  ���ֽ�      ���ֽ�
			0000 0000   0000 0000 
						   1 1111--B	555��ʽ
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	565��ʽ
				  111   111 -------G
			1111 1   --------------R

		* Win95 ϵͳֻ֧���������ָ�ʽ��NT֧�ֶ��ָ�ʽ��
		*******************************************/
		if (pInfo->bmiHeader.biCompression == 0)
		{
			*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
			*pb_mask = 0x1f;		*pa_mask = 0;
			break;
		}
		else
		{
			pMark = (unsigned long *)((LPBMP_INFO_HEADER)pInfo+1);

			*pr_mask = pMark[0];		*pg_mask = pMark[1];
			*pb_mask = pMark[2];		*pa_mask = 0;
			break;
		}
		break;
	case	24:
		/* assert(pInfo->bmiHeader.biCompression == 0); */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:
		/******************************************************
						32λ����ʾ��ͼ
			    ��               ->                 ��
		0000 0000   0000 0000   0000 0000   0000 0000 	888��ʽ
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		* Win95 ϵͳֻ֧����һ�ָ�ʽ
		******************************************************/
		if (pInfo->bmiHeader.biCompression == 0)
		{
			*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
			*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		}
		else
		{
			pMark = (unsigned long *)((LPBMP_INFO_HEADER)pInfo+1);
			
			*pr_mask = pMark[0];		*pg_mask = pMark[1];
			*pb_mask = pMark[2];		*pa_mask = 0;
		}
		break;
	default:
		result = 1;

		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}

	return result;	/* 0-�ɹ��� 1���Ƿ���ͼ���ʽ */
}


/* ��4λ������д����
   �β�mark���⣺0-��ʼ������  1-д����  2-�ƶ���ָ����  3-��������  4-��ȡ��ǰ��ַ
   ע����mark����2ʱ������num����Ϊ���س�2���������룬����iCurOff��ֵ
*/
unsigned char * CALLAGREEMENT _write2buff4RLE(int mark, int num, unsigned char pix, unsigned char *lpDesData)
{
	static unsigned char *lpDes;		/* ����������ַ */
	static unsigned char *lpCur;		/* ��������ǰд��ַ */
	static int			  iCurOff;		/* ���ֽ��еĵ�ǰλ�ã�0-�߰��ֽ�  1-�Ͱ��ֽ� */
	int i;
	unsigned char pixa[2];
	
	switch(mark)
	{
	case	0:		/* ��ʼ������ */
		lpDes = lpDesData;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	1:		/* д���� */
		pixa[0] = pix>>4;
		pixa[1] = pix&0xf;
		for (i=0;i<num;i++)
		{
			if (iCurOff == 0)	/* ������ */
			{
				*lpCur = pixa[i%2]<<4;
				iCurOff = 1;
			}
			else				/* ������ */
			{
				assert(iCurOff == 1);
				*lpCur |= pixa[i%2];
				lpCur++;
				iCurOff = 0;
			}
		}
		break;
	case	2:		/* �ƶ���ָ���� */
		lpCur = lpDesData;
		iCurOff = num;
		break;
	case	3:		/* �������� */
		lpDes = (unsigned char *)0;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	4:		/* ��ȡ��ǰ��ַ */
		break;
	default:
		assert(0);
		break;
	}
	
	return lpCur;
}



/* ��8λ������д���أ�ֻ��_from_8_RLE_read()�����ڲ����ã�
   �β�mark���⣺0-��ʼ������  1-д����  2-�ƶ���ָ����  3-��������  4-��ȡ��ǰ��ַ
*/
unsigned char * CALLAGREEMENT _write2buff8RLE(int mark, int num, unsigned char pix, unsigned char *lpDesData)
{
	static unsigned char *lpDes;		/* ����������ַ */
	static unsigned char *lpCur;		/* ��������ǰд��ַ */
	int i;
	
	switch(mark)
	{
	case	0:		/* ��ʼ������ */
		lpDes = lpDesData;
		lpCur = lpDes;
		break;
	case	1:		/* д���� */
		for (i=0;i<num;i++)
			*lpCur++ = pix;
		break;
	case	2:		/* �ƶ���ָ���� */
		lpCur = lpDesData;
		break;
	case	3:		/* �������� */
		lpDes = (unsigned char *)0;
		lpCur = lpDes;
		break;
	case	4:		/* ��ȡ��ǰ��ַ */
		break;
	default:
		assert(0);
		break;
	}
	return lpCur;
}



/* ����ɨ���гߴ�(���ֽڶ���) */
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* �жϴ����ͼ���Ƿ���Ա����� */
int CALLAGREEMENT _bmp_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* ## 2001-8-9  YZ �޸ģ�λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(bmp_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (bmp_irwp_info.irwp_save.img_num)
		if (bmp_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */

	return 0;
}