/********************************************************************

	pix.c

	----------------------------------------------------------------
    ������֤ �� GPL
	��Ȩ���� (C) 2002 VCHelp coPathway ISee workgroup.
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
			http://cosoft.org.cn/projects/iseeexplorer

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������Alias PIXͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��ɶ�ȡ8��24λRLEѹ��Alias PIXͼ��
							  
					���湦�ܣ�24λRLEѹ����ʽ

	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net

	���ļ��汾��	020507
	����޸��ڣ�	2002-5-7

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-5		��һ�������棨�°棩


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

#include "pix.h"


IRWP_INFO			pix_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	pix_get_info_critical;	/* pix_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	pix_load_img_critical;	/* pix_load_image�����Ĺؼ��� */
CRITICAL_SECTION	pix_save_img_critical;	/* pix_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _pix_is_valid_img(LPINFOSTR pinfo_str);

static enum EXERESULT CALLAGREEMENT _read_line_alias24(ISFILE *, unsigned char *, int);
static enum EXERESULT CALLAGREEMENT _read_line_alias8(ISFILE *, unsigned char *, int);
static enum EXERESULT CALLAGREEMENT _write_line_alias24(ISFILE *, unsigned char *, int);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&pix_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&pix_get_info_critical);
			InitializeCriticalSection(&pix_load_img_critical);
			InitializeCriticalSection(&pix_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&pix_get_info_critical);
			DeleteCriticalSection(&pix_load_img_critical);
			DeleteCriticalSection(&pix_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PIX_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pix_irwp_info;
}

#else

PIX_API LPIRWP_INFO CALLAGREEMENT pix_get_plugin_info()
{
	_init_irwp_info(&pix_irwp_info);

	return (LPIRWP_INFO)&pix_irwp_info;
}

PIX_API void CALLAGREEMENT pix_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PIX_API void CALLAGREEMENT pix_detach_plugin()
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
	/* ��λ����һ������32λ��ʽ�޷���ʾ���˴�����PIX��ȷ��λ����д��*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(24-1));
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
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"PIX.... �򵥵�һ�ָ�ʽ�����ںڿͷ��:)");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 3;

	/* ������Щ��ʽ����8λ���ͼ�� */
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"MATTE");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"ALPHA");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[2]), 
		(const char *)"MASK");
	
	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
PIX_API int CALLAGREEMENT pix_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PIX_HEADER		pixheader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&pix_get_info_critical);

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

			if (isio_read((void*)&pixheader, sizeof(PIX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* תΪIntel���� */
			pixheader.width   = EXCHANGE_WORD((pixheader.width));
			pixheader.height  = EXCHANGE_WORD((pixheader.height));
			pixheader.bitcount= EXCHANGE_WORD((pixheader.bitcount));

			/* �ж��Ƿ�����Ч��Alias PIXͼ�����ָ�ʽ�ǳ��򵥣�����
			   �޷������ϸ���жϣ��������еĿ����Խ�������ʽ�Դ�һЩ��*/
			if ((pixheader.width == 0) || (pixheader.height == 0) || \
				((pixheader.bitcount!=24)&&(pixheader.bitcount!=8)))
			{
				b_status = ER_NONIMAGE; __leave; 
			}

			/* ��д���ݰ� */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_PIX;			/* ͼ���ļ���ʽ����׺���� */

			pinfo_str->compression  = ICS_RLE8;

			pinfo_str->width	= (unsigned long)pixheader.width;
			pinfo_str->height	= (unsigned long)pixheader.height;
			pinfo_str->order	= 0;					/* PIX ͼ��Ϊ����ͼ */
			pinfo_str->bitcount	= (unsigned long)pixheader.bitcount;
		
			/* ��д������Ϣ */
			switch (pixheader.bitcount)
			{
			case	24:
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
				break;
			case	8:
				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}


			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pix_get_info_critical);
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
PIX_API int CALLAGREEMENT pix_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, linesize;
	unsigned char	*p;
	
	PIX_HEADER		pixheader;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pix_load_img_critical);

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

			if (isio_read((void*)&pixheader, sizeof(PIX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* תΪIntel���� */
			pixheader.width   = EXCHANGE_WORD((pixheader.width));
			pixheader.height  = EXCHANGE_WORD((pixheader.height));
			pixheader.bitcount= EXCHANGE_WORD((pixheader.bitcount));
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				if ((pixheader.width == 0) || (pixheader.height == 0) || \
					((pixheader.bitcount!=24)&&(pixheader.bitcount!=8)))
				{
					b_status = ER_NONIMAGE; __leave; 
				}
				
				/* ��д���ݰ� */
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_PIX;			/* ͼ���ļ���ʽ����׺���� */
				
				pinfo_str->compression  = ICS_RLE8;
				
				pinfo_str->width	= (unsigned long)pixheader.width;
				pinfo_str->height	= (unsigned long)pixheader.height;
				pinfo_str->order	= 0;					/* PIX ͼ��Ϊ����ͼ */
				pinfo_str->bitcount	= (unsigned long)pixheader.bitcount;
				
				switch (pixheader.bitcount)
				{
				case	24:
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
					break;
				case	8:
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					pinfo_str->a_mask = 0x0;
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave; break;
				}
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}

			/* �˹��ϳɻҶ�ͼ��ɫ�� */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

				for (i=0; i<(int)pinfo_str->pal_count; i++)
				{
					pinfo_str->palette[i] = (unsigned long)((i<<16)|(i<<8)|(i<<0));
				}
			}
			else
			{
				pinfo_str->pal_count = 0;
			}

			
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

			/* ��д���׵�ַ���飨����*/
			for (i=0; i<(int)(pinfo_str->height); i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}
			

			/* ����..... */
			for (i=0; i<(int)(pinfo_str->height); i++)
			{
				p = pinfo_str->p_bit_data+linesize*i;

				/* ���ж�ȡ����ѹ�� */
				if (pinfo_str->bitcount == 24)
				{
					if ((b_status=_read_line_alias24(pfile, p, (int)pinfo_str->width)) != ER_SUCCESS)
					{
						__leave;
					}
				}
				else
				{
					if ((b_status=_read_line_alias8(pfile, p, (int)pinfo_str->width)) != ER_SUCCESS)
					{
						__leave;
					}
				}

				pinfo_str->process_current = i;

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

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pix_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
PIX_API int CALLAGREEMENT pix_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	int				i;
	unsigned char	*p;
	
	PIX_HEADER		pixheader;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pix_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_pix_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��д��ͷ�ṹ */
			pixheader.width   = (unsigned short)pinfo_str->width;
			pixheader.height  = (unsigned short)pinfo_str->height;
			pixheader.rev0    = 0;
			pixheader.rev1    = 0;
			assert(pinfo_str->bitcount == 24);
			pixheader.bitcount= 24;

			/* ת��ΪMOTO���� */
			pixheader.width   = EXCHANGE_WORD((pixheader.width));
			pixheader.height  = EXCHANGE_WORD((pixheader.height));
			pixheader.bitcount= EXCHANGE_WORD((pixheader.bitcount));
			
			
			/* д����ͷ�ṹ */
			if (isio_write((const void *)&pixheader, sizeof(PIX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ���б��루RLE8����д��Ŀ���� */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				p = (unsigned char *)pinfo_str->pp_line_addr[i];

				if ((b_status=_write_line_alias24(pfile, p, (int)pinfo_str->width)) != ER_SUCCESS)
				{
					__leave;
				}

				pinfo_str->process_current = i;
				
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
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pix_save_img_critical);
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
int CALLAGREEMENT _pix_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(pix_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (pix_irwp_info.irwp_save.img_num)
		if (pix_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */

	return 0;
}


/* ��ȡһ��24λRLEѹ������ */
static enum EXERESULT CALLAGREEMENT _read_line_alias24(ISFILE *pfile, unsigned char *pix, int size)
{
	int				i;
	int				count = 0;
	unsigned char	buffer[4];

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pix&&size);

	__try
	{
		while (count < size)
		{
			/* ��ȡһ��ѹ�����ݣ�ÿ��4�ֽڣ���һ���ֽ�Ϊ�ظ�����ֵ����
			   �����ֽ�ΪB��G��R���� */
			if (isio_read((void*)buffer, 1, 4, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ظ�����ֵ */
			count += (int)(unsigned int)(unsigned char)(buffer[0]);

			if (count > size)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* �����ݰ���д�ظ����� */
			for (i= 0; i<(int)(unsigned int)(unsigned char)buffer[0]; i++)
			{
				*pix++ = buffer[1]; *pix++ = buffer[2]; *pix++ = buffer[3];
			}
		}
	}
	__finally
	{
	  ;
	}

	return b_status;
}


/* ��ȡһ��8λRLEѹ������ */
static enum EXERESULT CALLAGREEMENT _read_line_alias8(ISFILE *pfile, unsigned char *pix, int size)
{
	int				i;
	int				count = 0;
	unsigned char	buffer[2];

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pix&&size);

	__try
	{
		while (count < size)
		{
			/* ��ȡһ��ѹ�����ݣ�ÿ��2�ֽڣ���һ���ֽ�Ϊ�ظ�����ֵ����
			   һ���ֽ�Ϊ��ɫ����ֵ */
			if (isio_read((void*)buffer, 1, 2, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ظ�����ֵ */
			count += (int)(unsigned int)(unsigned char)(buffer[0]);

			if (count > size)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* �����ݰ���д�ظ����� */
			for (i= 0; i<(int)(unsigned int)(unsigned char)buffer[0]; i++)
			{
				*pix++ = buffer[1];
			}
		}
	}
	__finally
	{
	  ;
	}

	return b_status;
}


/* ѹ��һ�����ݵ�24λRLE */
static enum EXERESULT CALLAGREEMENT _write_line_alias24(ISFILE *pfile, unsigned char *pix, int size)
{
	int				i;
	int				count;
	unsigned char	buffer[4];

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pix&&size);

	__try
	{
		for (count=0, i=0; i<size; i++)
		{
			if (count == 0)
			{
				buffer[0] = 1;
				buffer[1] = *pix++;	/* B */
				buffer[2] = *pix++;	/* G */
				buffer[3] = *pix++;	/* R */
				count = 1;
			}
			else
			{
				if ((*(pix+0) == buffer[1])&&(*(pix+1) == buffer[2])&&\
					(*(pix+2) == buffer[3])&&(count < 255))
				{
					count++;
					buffer[0] = (unsigned char)count;
					pix += 3;
				}
				else
				{
					/* д��һ��ѹ������ */
					if (isio_write((const void *)buffer, 1, 4, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					count = 0;

					/* �ղ�����ѭ��ֵ���� */
					i--;
				}
			}
		}

		/* д��ʣ������� */
		if (count != 0)
		{
			if (isio_write((const void *)buffer, 1, 4, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
		}
	}
	__finally
	{
	  ;
	}

	return b_status;
}
