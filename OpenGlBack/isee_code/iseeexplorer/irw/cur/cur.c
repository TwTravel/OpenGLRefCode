/********************************************************************

	cur.c

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
			http://www.vchelp.net
			http://www.chinafcu.com

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������CURͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�1��4��8��16��24��32λ���ͼ��
							  
					���湦�ܣ���֧��
							   

	���ļ���д�ˣ�	
					YZ		yzfree##yeah.net

	���ļ��汾��	20327
	����޸��ڣ�	2002-3-27

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-3		�����°�ģ�顣ȥ����������Windowsϵͳ�й�
							��API���ã�ʹ�����������ֲ��
			2000-8		��һ�������档


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

#include "cur.h"


IRWP_INFO			cur_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	cur_get_info_critical;	/* cur_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	cur_load_img_critical;	/* cur_load_image�����Ĺؼ��� */
CRITICAL_SECTION	cur_save_img_critical;	/* cur_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _read_cur_header(ISFILE *file, CURDIR **cur);
int CALLAGREEMENT _get_mask(int bitcount, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
int CALLAGREEMENT _read_image_block(ISFILE *file, unsigned long offset, unsigned long len, unsigned char **image);
int CALLAGREEMENT _compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand);
int CALLAGREEMENT _conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark);

static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&cur_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&cur_get_info_critical);
			InitializeCriticalSection(&cur_load_img_critical);
			InitializeCriticalSection(&cur_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&cur_get_info_critical);
			DeleteCriticalSection(&cur_load_img_critical);
			DeleteCriticalSection(&cur_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

CUR_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&cur_irwp_info;
}

#else

CUR_API LPIRWP_INFO CALLAGREEMENT cur_get_plugin_info()
{
	_init_irwp_info(&cur_irwp_info);

	return (LPIRWP_INFO)&cur_irwp_info;
}

CUR_API void CALLAGREEMENT cur_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

CUR_API void CALLAGREEMENT cur_detach_plugin()
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
	lpirwp_info->irwp_function = IRWP_READ_SUPP;

	/* ����ģ��֧�ֵı���λ�� */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
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
				(const char *)"Hello!!! ������ĸ�^_^");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
CUR_API int CALLAGREEMENT cur_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	LPCURDIR		lpir = 0;
	LPBITMAPINFOHEADER lpbmi = 0;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&cur_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ��ȡ�ļ�ͷ��Ϣ */
			switch (_read_cur_header(pfile, (CURDIR **)&lpir))
			{
			case	-1:		/* �ļ���дʧ�� */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* ����ļ����� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* �ڴ治�� */
				b_status = ER_MEMORYERR; __leave; break;
			case	-4:		/* ����һ������ļ� */
				b_status = ER_NONIMAGE;  __leave; break;
			case	0:		/* �ɹ� */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_CUR;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression	= ICS_RGB;			/* ͼ���ѹ����ʽ */
			
			/* ��ȡ��ͼ�����ݿ� */
			switch (_read_image_block(pfile, (unsigned long)lpir->entries[0].image_offset, (unsigned long)lpir->entries[0].bytes_in_res, (unsigned char **)&lpbmi))
			{
			case	-1:		/* �ļ���дʧ�� */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* ����ļ����� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* �ڴ治�� */
				b_status = ER_MEMORYERR; __leave; break;
			case	0:		/* �ɹ� */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			/* ��дͼ����Ϣ */
			pinfo_str->width	= (unsigned long)lpbmi->biWidth;
			pinfo_str->height	= (unsigned long)lpbmi->biHeight/2;	/* XOR��AND ͼ�ܸ߶� */
			pinfo_str->order	= 1;	/* ���ǵ���ͼ */
			pinfo_str->bitcount = (unsigned long)lpbmi->biBitCount;
			
			if (_get_mask(pinfo_str->bitcount, 
				(unsigned long *)(&pinfo_str->b_mask), 
				(unsigned long *)(&pinfo_str->g_mask), 
				(unsigned long *)(&pinfo_str->r_mask),
				(unsigned long *)(&pinfo_str->a_mask)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (lpbmi)
				free(lpbmi);
			
			if (lpir)
				free(lpir);
			
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&cur_get_info_critical);
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
CUR_API int CALLAGREEMENT cur_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	LPCURDIR		lpir = 0;
	unsigned char	*lpimg = 0;
	int				i;
	
	LPBITMAPINFOHEADER	lpbmi;
	LPSUBIMGBLOCK		lpsub = 0, lplast;
	LPSUBIMGBLOCK		subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&cur_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ################ �������д��ȡͼ��Ĵ��� ################ */
			/* ��ȡ�ļ�ͷ��Ϣ */
			switch (_read_cur_header(pfile, (CURDIR **)&lpir))
			{
			case	-1:		/* �ļ���дʧ�� */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* ����ļ����� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* �ڴ治�� */
				b_status = ER_MEMORYERR; __leave; break;
			case	-4:		/* ����һ������ļ� */
				b_status = ER_NONIMAGE;  __leave; break;
			case	0:		/* �ɹ� */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* ��ȡ��ͼ�����ݿ� */
			switch (_read_image_block(pfile, (unsigned long)lpir->entries[0].image_offset, (unsigned long)lpir->entries[0].bytes_in_res, &lpimg))
			{
			case	-1:		/* �ļ���дʧ�� */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* ����ļ����� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* �ڴ治�� */
				b_status = ER_MEMORYERR; __leave; break;
			case	0:		/* �ɹ� */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			lpbmi = (LPBITMAPINFOHEADER)lpimg;
			assert(lpbmi);
			
			pinfo_str->process_total = (int)lpir->count;
			assert(lpir->count > 0);
			pinfo_str->process_current = 0;
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_CUR;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression	= ICS_RGB;			/* ͼ���ѹ����ʽ */
				
				/* ��дͼ����Ϣ */
				pinfo_str->width	= (unsigned long)lpbmi->biWidth;
				pinfo_str->height	= (unsigned long)lpbmi->biHeight/2;	/* XOR��AND ͼ�ܸ߶� */
				pinfo_str->order	= 1;	/* ���ǵ���ͼ */
				pinfo_str->bitcount = (unsigned long)lpbmi->biBitCount;
				
				if (_get_mask(pinfo_str->bitcount, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}
	
			/* ����ͼ����� */
			pinfo_str->imgnumbers = (unsigned long)lpir->count;
			assert(pinfo_str->psubimg == 0);
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;
			
			/* ����֡ͼ�����ݰ��� */
			switch (_conv_image_block(lpbmi, (void *)pinfo_str, CUR_PRI_IMAGE))
			{
			case	-2:		/* ����ļ����� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* �ڴ治�� */
				b_status = ER_MEMORYERR; __leave; break;
			case	0:		/* �ɹ� */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* ��ȡ����֡ͼ�����������ã��ͷ� */
			free(lpimg);
			lpimg = (unsigned char *)lpbmi = (unsigned char *)0;

			pinfo_str->process_current = 1;
			
			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			lplast = 0;

			/* ������ͼ�� */
			for (i=0;i<(int)(pinfo_str->imgnumbers-1)/*��ͼ����*/;i++)
			{
				if ((lpsub=_alloc_SUBIMGBLOCK()) == 0)
				{
					b_status = ER_MEMORYERR; __leave;
				}

				lpsub->number = i+1;	/* ��ͼ������� 1 ��ʼ */
				lpsub->parents = pinfo_str;

				/* ������ͼ������β�� */
				if (lplast == 0)
				{
					pinfo_str->psubimg = lpsub;
					lplast = lpsub;
					
					lpsub->prev = 0;
					lpsub->next = 0;
				}
				else
				{
					lpsub->prev = lplast;
					lpsub->next = 0;
					lplast->next = lpsub;
					lplast = lpsub;
				}

				/* ��ȡ��ͼ�����ݿ� */
				switch (_read_image_block(pfile, (unsigned long)lpir->entries[i+1].image_offset, (unsigned long)lpir->entries[i+1].bytes_in_res, &lpimg))
				{
				case	-1:		/* �ļ���дʧ�� */
					b_status = ER_FILERWERR; __leave; break;
				case	-2:		/* ����ļ����� */
					b_status = ER_BADIMAGE;  __leave; break;
				case	-3:		/* �ڴ治�� */
					b_status = ER_MEMORYERR; __leave; break;
				case	0:		/* �ɹ� */
					break;
				default:
					b_status = ER_SYSERR;    __leave; break;
				}

				lpbmi = (LPBITMAPINFOHEADER)lpimg;

				/* ת����ͼ�����ݰ��� */
				switch (_conv_image_block(lpbmi, (void *)lpsub, CUR_SUB_IMAGE))
				{
				case	-2:		/* ����ļ����� */
					b_status = ER_BADIMAGE;  __leave; break;
				case	-3:		/* �ڴ治�� */
					b_status = ER_MEMORYERR; __leave; break;
				case	0:		/* �ɹ� */
					break;
				default:
					b_status = ER_SYSERR;    __leave; break;
				}

				free(lpimg);
				lpimg = (unsigned char *)lpbmi = (unsigned char *)0;
				
				pinfo_str->process_current++;
				
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

			if (lpimg)
				free(lpimg);
			
			if (lpir)
				free(lpir);
			
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&cur_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
CUR_API int CALLAGREEMENT cur_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&cur_save_img_critical);
	
			b_status = ER_NOTSUPPORT;

		}
		__finally
		{
			LeaveCriticalSection(&cur_save_img_critical);
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


/* ��ȡCURͷ�ṹ������ͼ�������ͼ��ߴ�ȣ��������м򵥵ļ��� */
int CALLAGREEMENT _read_cur_header(ISFILE *file, CURDIR **cur)
{
	CURDIR		curdir;
	int			infosize, result = 0;
	LPCURDIR	lpir = 0;

	assert((file!=0)&&(cur != 0));

	__try
	{
		/* ��λ���ļ��ײ� */
		if (isio_seek(file, 0, SEEK_SET) == -1)
		{
			result = -1; __leave;		/* �ļ���дʧ�� */
		}
		
		/* ��ȡ����ļ���Ϣͷ�ṹ */
		if (isio_read((void*)&curdir, sizeof(CURDIR), 1, file) == 0)
		{
			result = -4; __leave;		/* ����һ������ļ� */
		}

		/* �ж��ļ���־ */
		if ((curdir.reserved != 0)||(curdir.type != CURTYPE))
		{
			result = -4; __leave;		/* ����һ������ļ� */
		}

		/* ���ٻ����һ��ͼ�� */
		if (curdir.count < 1)
		{
			result = -2; __leave;		/* ����ļ����� */
		}


		/* ��������Ϣ��ĳߴ� */
		infosize = sizeof(CURDIR)+(curdir.count-1)*sizeof(CURDIRENTRY);

		/* �������ڴ�Ź����Ϣ�Ļ������ڴ�� */
		if ((lpir=(LPCURDIR)malloc(infosize)) == 0)
		{
			result = -3; __leave;		/* �ڴ治�� */
		}

		/* ��ȡ�����Ϣ */
		if (isio_seek(file, 0, SEEK_SET) == -1)
		{
			result = -1; __leave;
		}

		if (isio_read((void*)lpir, infosize, 1, file) == 0)
		{
			result = -2; __leave;		/* ����ļ�����Ҳ�п������ļ���д����ͼ������Ŀ����Ը��� */
		}
	}
	__finally
	{
		if ((result != 0)||(AbnormalTermination()))
		{
			if (lpir)
			{
				free(lpir);	lpir = 0;
			}
			if (result == 0)
			{
				result = -1;	/* ������쳣���ܶ������ļ���дʧ������� */
			}
		}
	}

	*cur = lpir;	/* ��������������쳣ʱ��*cur������ 0 */

	return result;	/* ���� 0 �ɹ����� 0 ֵʧ�ܡ�*curָ����ڴ���ڲ���ҪʱӦ�ͷš� */
}


/* ��ȡ����ɫ������ֵ */
int CALLAGREEMENT _get_mask(int bitcount, 
	unsigned long * pb_mask, 
	unsigned long * pg_mask, 
	unsigned long * pr_mask, 
	unsigned long * pa_mask)
{
	int result = 0;

	switch(bitcount)
	{
	case	1:
	case	4:
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
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	default:		/* �Ƿ���ʽ */
		result = -1;
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}

	return result;	/* 0-�ɹ��� -1���Ƿ���ͼ���ʽ */
}


/* ��ȡһ�������������ݿ飨����BITMAPINFOHEADER�ṹ��XOR���ݺ�AND���ݣ�*/
int CALLAGREEMENT _read_image_block(ISFILE *file, unsigned long offset, unsigned long len, unsigned char **image)
{
	unsigned char *	tmp = 0;
	int				result = 0;

	assert((file != 0)&&(image != 0));

	/* ���ݺϷ��Լ�� */
	if ((offset == 0)||(len == 0))
	{
		return -2;		/* ����ļ��������ݲ��Ϸ���*/
	}
	
	__try
	{
		/* ��λ��дλ�� */
		if (isio_seek(file, offset, SEEK_SET) == -1)
		{
			result = -1; __leave;		/* �ļ���дʧ�� */
		}
		
		if ((tmp=(unsigned char *)malloc(len)) == 0)
		{
			result = -3; __leave;		/* �ڴ治�� */
		}

		/* ��ȡͼ������ */
		if (isio_read((void*)tmp, len, 1, file) == 0)
		{
			result = -2; __leave;		/* ����ļ����� */
		}
		
		if (((LPBITMAPINFOHEADER)tmp)->biSize != sizeof(BITMAPINFOHEADER))
		{
			result = -2; __leave;		/* ����ļ����� */
		}
	}
	__finally
	{
		if ((result != 0)||(AbnormalTermination()))
		{
			if (tmp)
			{
				free(tmp);	tmp = 0;
			}
			if (result == 0)
			{
				result = -1;			/* �ļ���дʧ�� */
			}
		}
	}

	*image = tmp;
	
	return result;	/* ���� 0 �ɹ����� 0 ֵʧ�ܡ�*tmpָ����ڴ���ڲ���ҪʱӦ�ͷš� */
}


/* ��������ݿ�תΪ���ݰ��� */
int CALLAGREEMENT _conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark)
{
	LPINFOSTR		pinfo = (LPINFOSTR)0;
	LPSUBIMGBLOCK	psubinfo = (LPSUBIMGBLOCK)0;
	LPBITMAPINFO	pbi = (LPBITMAPINFO)lpbmi;
	
	unsigned char	*p, *pxor, *pand;
	unsigned long	pal_num = 0;
	int				i, linesize, linesize2;

	assert((lpbmi)&&(lpdest));
	assert((mark == CUR_PRI_IMAGE)||(mark == CUR_SUB_IMAGE));

	if (mark == CUR_PRI_IMAGE)
	{
		pinfo = (LPINFOSTR)lpdest;

		/* ���õ�ɫ������ */
		if (lpbmi->biBitCount <= 8)
		{
			pinfo->pal_count = 1UL << pinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? pinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > pinfo->pal_count)
			{
				return -2;		/* ͼ������ */
			}
			memmove((void*)(pinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			pinfo->pal_count = 0;
		}

		/* XOR����ɨ���гߴ� */
		linesize  = _calcu_scanline_size(pinfo->width, pinfo->bitcount);
		/* AND����ɨ���гߴ� */
		linesize2 = _calcu_scanline_size(pinfo->width, 1);
		
		assert(pinfo->p_bit_data == (unsigned char *)0);
		
		/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
		pinfo->p_bit_data = (unsigned char *)malloc(linesize * pinfo->height + 4);
		if (!pinfo->p_bit_data)
		{						
			return -3;			/* �ڴ治�� */
		}
		
		assert(pinfo->pp_line_addr == (void**)0);
		/* �������׵�ַ���� */
		pinfo->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo->height);
		if (!pinfo->pp_line_addr)
		{
			free(pinfo->p_bit_data); 
			pinfo->p_bit_data = 0; 
			return -3;			/* �ڴ治�� */
		}

		/* ��ʼ�����׵�ַ���飨���� */
		for (i=0;i<(int)(pinfo->height);i++)
		{
			pinfo->pp_line_addr[i] = (void *)(pinfo->p_bit_data+((pinfo->height-i-1)*linesize));
		}
		
		/* ����XOR���������׵�ַ */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);

		/* ����XOR�������� */
		memmove((void*)pinfo->p_bit_data, (const void *)p, (linesize*pinfo->height));

		/* ȡ�� XOR �����׵�ַ */
		pxor = (unsigned char *)pinfo->p_bit_data;
		/* ���� AND �����׵�ַ */
		pand = p + linesize * pinfo->height;

		/* �ϳ�ͼ�� */
		for (i=0;i<(int)pinfo->height;i++)
		{
			if (_compose_img(pinfo->bitcount, pinfo->width, pxor, pand) != 0)
			{
				free(pinfo->p_bit_data); 
				pinfo->p_bit_data = 0; 
				free(pinfo->pp_line_addr);
				pinfo->pp_line_addr = 0;

				return -2;		/* �����ͼ�� */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}
	else
	{
		psubinfo = (LPSUBIMGBLOCK)lpdest;

		/* ��дͼ����Ϣ */
		psubinfo->width		= (int)lpbmi->biWidth;
		psubinfo->height	= (int)lpbmi->biHeight/2;	/* XOR��AND ͼ�ܸ߶� */
		psubinfo->order		= 1;	/* ���ǵ���ͼ */
		psubinfo->bitcount	= (int)lpbmi->biBitCount;

		if (_get_mask(psubinfo->bitcount, 
			(unsigned long *)(&psubinfo->b_mask), 
			(unsigned long *)(&psubinfo->g_mask), 
			(unsigned long *)(&psubinfo->r_mask),
			(unsigned long *)(&psubinfo->a_mask)) == -1)
		{
			return -2;		/* ͼ������ */
		}

		psubinfo->left = psubinfo->top = 0;
		psubinfo->dowith =psubinfo->userinput = psubinfo->time = 0;
		psubinfo->colorkey = -1;	/* ��Ϊ�޹ؼ�ɫ */

		/* ���õ�ɫ������ */
		if (lpbmi->biBitCount <= 8)
		{
			psubinfo->pal_count = 1UL << psubinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? psubinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > (unsigned long)psubinfo->pal_count)
			{
				return -2;		/* ͼ������ */
			}
			memmove((void*)(psubinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			psubinfo->pal_count = 0;
		}

		
		/* ȡ��XOR����ɨ���гߴ� */
		linesize  = _calcu_scanline_size(psubinfo->width, psubinfo->bitcount);
		/* ����AND����ɨ���гߴ� */
		linesize2 = _calcu_scanline_size(psubinfo->width, 1);
		
		assert(psubinfo->p_bit_data == (unsigned char *)0);
		
		/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
		psubinfo->p_bit_data = (unsigned char *)malloc(linesize * psubinfo->height + 4);
		if (!psubinfo->p_bit_data)
		{						
			return -3;			/* �ڴ治�� */
		}
		
		assert(psubinfo->pp_line_addr == (void**)0);
		/* �������׵�ַ���� */
		psubinfo->pp_line_addr = (void **)malloc(sizeof(void*) * psubinfo->height);
		if (!psubinfo->pp_line_addr)
		{
			free(psubinfo->p_bit_data); 
			psubinfo->p_bit_data = 0; 
			return -3;			/* �ڴ治�� */
		}
		
		/* ��ʼ�����׵�ַ���飨���� */
		for (i=0;i<(int)(psubinfo->height);i++)
		{
			psubinfo->pp_line_addr[i] = (void *)(psubinfo->p_bit_data+((psubinfo->height-i-1)*linesize));
		}
		
		/* �������������׵�ַ */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);
		
		/* ������������ */
		memmove((void*)psubinfo->p_bit_data, (const void *)p, (linesize*psubinfo->height));
		
		/* ���� XOR �����׵�ַ */
		pxor = (unsigned char *)psubinfo->p_bit_data;
		/* ���� AND �����׵�ַ */
		pand = p + linesize * psubinfo->height;
		
		/* �ϳ�ͼ��ʹ��XOR������AND���ݣ�*/
		for (i=0;i<psubinfo->height;i++)
		{
			if (_compose_img(psubinfo->bitcount, psubinfo->width, pxor, pand) != 0)
			{
				free(psubinfo->p_bit_data); 
				psubinfo->p_bit_data = 0;
				free(psubinfo->pp_line_addr);
				psubinfo->pp_line_addr = 0;

				return -2;		/* �����ͼ�� */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}

	return 0;		/* ���� 0 �ɹ����� 0 ʧ�� */
}


/* �ϳ�һ��ɨ���е�XORͼ��ANDͼ��������ISeeλ����ʽת�������������XORͼ�� */
int CALLAGREEMENT _compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand)
{
	unsigned char buf[32], tmp, *p = pxor, isc = 0;
	int i, j, k, l, a, bytesize;

	assert(bitcount > 0);
	assert(pxor&&pand);

	/* һ�δ��� 8 ������ */
	for (i=0, k=0;i<width;i+=8, k++)
	{
		/* ȡ�� 8 �����ص�AND���ݣ��� 1 ���ֽڣ� */
		tmp = *(pand+k);

		/* ��AND����תΪXOR���ظ�ʽ */
		switch (bitcount)
		{
		case	1:
			buf[0] = tmp;
			bytesize = 1;
			break;
		case	4:
			for (l=0;l<8;l+=2)
			{
				buf[l/2]  = ((tmp>>(7-l))&0x1) ? 0xf0 : 0x0;
				buf[l/2] |= ((tmp>>(7-l-1))&0x1) ? 0xf : 0x0;
			}
			bytesize = 4;
			break;
		case	8:
			for (l=0;l<8;l++)
			{
				buf[l] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
			}
			bytesize = ((width-i) < 8) ? (width-i) : 8;
			break;
		case	16:
			for (l=0;l<8;l++)
			{
				buf[l*2] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*2+1] = buf[l*2]&0x7f;		/* 555��ʽ */
			}
			bytesize = ((width-i) < 8) ? (width-i)*2 : 16;
			break;
		case	24:
			for (l=0;l<8;l++)
			{
				buf[l*3] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*3+1] = buf[l*3];
				buf[l*3+2] = buf[l*3];
			}
			bytesize = ((width-i) < 8) ? (width-i)*3 : 24;
			break;
		case	32:
			for (l=0;l<8;l++)
			{
				buf[l*4] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*4+1] = buf[l*4];
				buf[l*4+2] = buf[l*4];
				buf[l*4+3] = 0;					/* 888��ʽ */
			}
			bytesize = ((width-i) < 8) ? (width-i)*4 : 32;
			break;
		default:
			return -1;							/* ��֧�ֵ�λ��� */
			break;
		}

		/* �ϳ�ͼ��һ�κϳ� 8 �����أ�*/
		for (j=0;j<bytesize;j++)
		{
			*p++ ^= buf[j];
		}

		isc = 0;

		/* �� 1 λ��ͼ��תΪISeeλ����ʽ */
		if (bitcount == 1)
		{
			for (a=0;a<8;a++)
			{
				isc |= (((*(p-1))>>(7-a))&0x1)<<a;
			}
			*(p-1) = isc;
		}

		/* �� 4 λ��ͼ��תΪISeeλ����ʽ */
		if (bitcount == 4)
		{
			for (a=0;a<4;a++)
			{
				isc  = (*(p-4+a))>>4;
				isc |= (*(p-4+a))<<4;
				*(p-4+a) = isc;
			}
		}
	}

	return 0;	/* ���� 0 ��ʾ�ɹ������ط� 0 ֵ��ʾʧ�� */
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

	return p_subimg;
}


/* �ͷ�һ����ͼ��ڵ㣬�������е�λ�����������׵�ַ���� */
void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);

	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);

	if (p_node->p_bit_data)
		free(p_node->p_bit_data);

	free(p_node);
}

