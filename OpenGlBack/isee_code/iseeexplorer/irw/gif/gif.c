/********************************************************************

	gif.c

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
	���ļ���;��	ISeeͼ���������GIFͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�1~8λ����̬��̬GIFͼ��
					���湦�ܣ���GIF-LZWר���ڵ�2003���ʧЧ������
							  �ݲ����ṩ���湦��
	  
	���ļ���д�ˣ�	YZ			yzfree##sina.com
		
	���ļ��汾��	11227
	����޸��ڣ�	2001-12-27
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		  2001-12		������GIF-LZW��ѹ�㷨�д��ڵ�һ�����󣬲�
						��ǿ��_get_imginfo()�������ݴ��ԡ�(B01)
		  2001-7		�����°����
		  2000-9		��һ�����԰淢��
		  2000-8		Sam��д��GIF��д��


	GIFͼ���ļ���ʽ��Ȩ������
	---------------------------------------------------------------
	The Graphics Interchange Format(c) is the Copyright property of
	CompuServe Incorporated. GIF(sm) is a Service Mark property of
	CompuServe Incorporated.
	---------------------------------------------------------------

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

#include "gif.h"


/* �����Ϣ�� */
IRWP_INFO			gif_irwp_info;


#ifdef WIN32
CRITICAL_SECTION	gif_get_info_critical;	/* gif_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	gif_load_img_critical;	/* gif_load_image�����Ĺؼ��� */
CRITICAL_SECTION	gif_save_img_critical;	/* gif_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void			CALLAGREEMENT _init_irwp_info(LPIRWP_INFO);
int				CALLAGREEMENT _get_imginfo(unsigned char *, unsigned long, LPGIFINSIDEINFO);
void			CALLAGREEMENT _get_imgdata_size(unsigned char *, unsigned long *, unsigned long *);

void			CALLAGREEMENT _init_lzw_table(LZWTABLE *, int, int);
unsigned char * CALLAGREEMENT _get_lzw_datablock(unsigned char *, unsigned long);
unsigned short	CALLAGREEMENT _get_next_codeword(unsigned char *, unsigned long, unsigned long, unsigned long, unsigned long);

LPSUBIMGBLOCK	CALLAGREEMENT _alloc_SUBIMGBLOCK(unsigned char *, unsigned char *, LPGRAPHCTRL);
void			CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK);

int				CALLAGREEMENT _load_static_gif(LPINFOSTR, unsigned char *, unsigned long, LPGIFINSIDEINFO);
int				CALLAGREEMENT _load_dyn_gif(LPINFOSTR, unsigned char *, unsigned long, LPGIFINSIDEINFO);
int				CALLAGREEMENT _build_bkimg(LPINFOSTR, LPGIFINSIDEINFO);
int				CALLAGREEMENT _load_subimg_serial(LPINFOSTR, unsigned char *, unsigned long);
LPSUBIMGBLOCK	CALLAGREEMENT _decomp_LZW_to_BMP(unsigned char *, unsigned char *, LPGRAPHCTRL, unsigned long *);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&gif_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&gif_get_info_critical);
			InitializeCriticalSection(&gif_load_img_critical);
			InitializeCriticalSection(&gif_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&gif_get_info_critical);
			DeleteCriticalSection(&gif_load_img_critical);
			DeleteCriticalSection(&gif_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

GIF_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&gif_irwp_info;
}

#else

GIF_API LPIRWP_INFO CALLAGREEMENT bmp_get_plugin_info()
{
	_init_irwp_info(&gif_irwp_info);

	return (LPIRWP_INFO)&gif_irwp_info;
}

GIF_API void CALLAGREEMENT gif_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

GIF_API void CALLAGREEMENT gif_detach_plugin()
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
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 2;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"��GIF��ʽ���ƶ��߿���:)");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"Sam");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	/* ---------------------------------[2] �� ������ -------------- */
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
GIF_API int CALLAGREEMENT gif_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	void			*ptmp = (void*)0;

	GIFHEADER		header;
	GIFINSIDEINFO	inside;

	int				type = 0;
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&gif_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ��ȡ������ */
			stream_length = isio_length(pfile);

			/* ��Ч��ͼ��������Ӧ����ͷ�ṹ�ߴ�+�ս��� */
			if (stream_length < (sizeof(GIFHEADER)+1))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ��ȡ��ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (isio_read((void*)&header, sizeof(GIFHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ж��Ƿ�����Ч��ͼ���� */
			if (!memcmp((const void *)header.gif_type, (const char *)GIF_MARKER_89a, strlen((const char *)GIF_MARKER_89a)))
				type = 1;
			else if (!memcmp((const void *)header.gif_type, (const char *)GIF_MARKER_87a, strlen((const char *)GIF_MARKER_87a)))
				type = 2;
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}

			
			/* �����ڴ棬���ڱ�������GIF�� */
			ptmp = malloc(stream_length);
			if (!ptmp)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡ����GIF�����ڴ� */
			if (isio_read(ptmp, stream_length, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ��ȡͼ����Ϣ */
			if (_get_imginfo((unsigned char *)ptmp, stream_length, &inside) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* ���ֻ��һ��ͼ������Ϊ��̬ͼ������ж��ͼ����
			   Ϊ��̬�������߼�����Ϊ����ͼ��(ͼ�����+1)��
			*/
			assert(inside.img_count);

			inside.img_count = (inside.img_count > 1) ? inside.img_count+1 : 1;

													/* ͼ�������� */
			pinfo_str->imgtype	= (inside.img_count>1) ? IMT_RESDYN : IMT_RESSTATIC;
			pinfo_str->imgformat= IMF_GIF;			/* ͼ������ʽ����׺���� */
			pinfo_str->compression = ICS_GIFLZW;	/* ͼ��ѹ����ʽ */

			/* ����Ƕ�̬GIF����ȡ���߼��������� */
			if (pinfo_str->imgtype == IMT_RESDYN)
			{
				pinfo_str->width	= inside.scr_width;
				pinfo_str->height	= inside.scr_height;
				pinfo_str->order	= 0;			/* GIFͼ��Ϊ���� */
				/* ���û��ȫ�ֵ�ɫ�����ݣ���ȡ�׷�ͼ���Ӧ���� */
				if (inside.scr_bitcount == 0)
					pinfo_str->bitcount = inside.first_img_bitcount;
				else
					pinfo_str->bitcount	= inside.scr_bitcount;
			}
			else
			{
				pinfo_str->width	= inside.first_img_width;
				pinfo_str->height	= inside.first_img_height;
				pinfo_str->order	= 0;			/* GIFͼ��Ϊ���� */
				pinfo_str->bitcount	= inside.first_img_bitcount;
			}

			/* GIF��Ϊ����ͼ������û���������� */
			pinfo_str->b_mask = 0x0;
			pinfo_str->g_mask = 0x0;
			pinfo_str->r_mask = 0x0;
			pinfo_str->a_mask = 0x0;

			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (ptmp)
				free(ptmp);
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&gif_get_info_critical);
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
GIF_API int CALLAGREEMENT gif_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	void			*ptmp = (void*)0;
	int				result;
	
	GIFINSIDEINFO	inside;
	LPSUBIMGBLOCK	subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&gif_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ��ʼ������ָʾ��������ģ����һ������Ҫ�����������ݣ�
			   ���Զ��û��ж�֧�ֵĲ��ã�*/
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;

			/* �ж��û��Ƿ�Ҫ��ֹ���� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ��ȡ������ */
			stream_length = isio_length(pfile);

			/* �����ڴ棬���ڱ�������GIF�� */
			ptmp = malloc(stream_length);
			if (!ptmp)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ��ȡ����GIF�����ڴ� */
			if (isio_read(ptmp, stream_length, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ��ȡͼ����Ϣ */
			if (_get_imginfo((unsigned char *)ptmp, stream_length, &inside) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			inside.img_count = (inside.img_count > 1) ? inside.img_count+1 : 1;
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ͼ�������� */
				pinfo_str->imgtype	= (inside.img_count>1) ? IMT_RESDYN : IMT_RESSTATIC;
				pinfo_str->imgformat= IMF_GIF;			/* ͼ������ʽ����׺���� */
				pinfo_str->compression = ICS_GIFLZW;	/* ͼ��ѹ����ʽ */
				
				/* ����Ƕ�̬GIF����ȡ���߼��������� */
				if (pinfo_str->imgtype == IMT_RESDYN)
				{
					pinfo_str->width	= inside.scr_width;
					pinfo_str->height	= inside.scr_height;
					pinfo_str->order	= 0;			/* GIFͼ��Ϊ���� */
					/* ���û��ȫ�ֵ�ɫ�����ݣ���ȡ�׷�ͼ���Ӧ���� */
					if (inside.scr_bitcount == 0)
						pinfo_str->bitcount = inside.first_img_bitcount;
					else
						pinfo_str->bitcount	= inside.scr_bitcount;
				}
				else
				{
					pinfo_str->width	= inside.first_img_width;
					pinfo_str->height	= inside.first_img_height;
					pinfo_str->order	= 0;			/* GIFͼ��Ϊ���� */
					pinfo_str->bitcount	= inside.first_img_bitcount;
				}
				
				/* GIF��Ϊ����ͼ������û���������� */
				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}			


			/* ����GIFͼ�� */
			if (pinfo_str->imgtype == IMT_RESSTATIC)	/* ��̬ */
			{
				result = _load_static_gif(pinfo_str, ptmp, stream_length, &inside);
			}
			else if (pinfo_str->imgtype == IMT_RESDYN)	/* ��̬ */
			{
				result = _load_dyn_gif(pinfo_str, ptmp, stream_length, &inside);
			}
			else
				assert(0);		/* ��ƴ��� */

			if (result == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* �ͷ���ͼ������ */
				if (pinfo_str->psubimg != 0)
				{
					while(subimg_tmp=pinfo_str->psubimg)
					{
						_free_SUBIMGBLOCK(pinfo_str->psubimg);
						pinfo_str->psubimg = subimg_tmp;
					}
					_free_SUBIMGBLOCK(pinfo_str->psubimg);
					pinfo_str->psubimg = 0;
				}
				
				/* �ͷ���ͼ�� */
				if (pinfo_str->pp_line_addr != 0)
					free(pinfo_str->pp_line_addr);
				if (pinfo_str->p_bit_data != 0)
					free(pinfo_str->p_bit_data);

				pinfo_str->pp_line_addr = 0;
				pinfo_str->p_bit_data   = 0;
				pinfo_str->pal_count    = 0;

				pinfo_str->imgnumbers = 0;

				if (pinfo_str->data_state == 2)
					pinfo_str->data_state =1;	/* �Զ����� */
			}

			if (ptmp)
				free(ptmp);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&gif_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}




/* ����ͼ���ݲ���ʵ�֣� */
GIF_API int CALLAGREEMENT gif_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&gif_save_img_critical);
	
			/* ############################################################# */
			/* �ڴ˴����뱣����� */
			/* ############################################################# */

			b_status = ER_NOTSUPPORT;	/* ������ʱ��֧��д���� */
		}
		__finally
		{
			LeaveCriticalSection(&gif_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}




/*==================================================================*/
/* �ڲ����ֺ��� */
/*==================================================================*/


/* ���뾲̬GIFͼ������ */
int CALLAGREEMENT _load_static_gif(
	LPINFOSTR pinfo_str, 
	unsigned char *stream, 
	unsigned long stream_len,
	LPGIFINSIDEINFO p_info)
{
	assert(pinfo_str->imgtype == IMT_RESSTATIC);
	
	if (_load_subimg_serial(pinfo_str, stream, stream_len) == -1)
		return -1;
	
	assert(pinfo_str->imgnumbers == 1);
	
	memcpy((void*)pinfo_str->palette, (const void *)pinfo_str->psubimg->palette, sizeof(unsigned long)*MAX_PALETTE_COUNT);
	pinfo_str->pal_count = pinfo_str->psubimg->pal_count;
	pinfo_str->pp_line_addr = pinfo_str->psubimg->pp_line_addr;
	pinfo_str->p_bit_data = pinfo_str->psubimg->p_bit_data;
	
	memset((void*)pinfo_str->psubimg, 0, sizeof(SUBIMGBLOCK));
	
	_free_SUBIMGBLOCK(pinfo_str->psubimg);
	
	pinfo_str->psubimg = 0;
	
	return 0;
}



/* ���붯̬GIFͼ������ */
int CALLAGREEMENT _load_dyn_gif(
	LPINFOSTR pinfo_str, 
	unsigned char *stream, 
	unsigned long stream_len, 
	LPGIFINSIDEINFO p_info)
{
	LPGIFHEADER		pgif_header = (LPGIFHEADER)stream;
	LPSUBIMGBLOCK	subimg_tmp;
	
	assert(pinfo_str->imgtype == IMT_RESDYN);
	
	/* ������ͼ������ */
	if (_load_subimg_serial(pinfo_str, stream, stream_len) == -1)
		return -1;
	
	assert(pinfo_str->imgnumbers > 1);
	
	/* ���뱳��ͼ */
	if (_build_bkimg(pinfo_str, p_info) == -1)
	{
		/* �ͷ���ͼ������ */
		if (pinfo_str->psubimg != 0)
		{
			while(subimg_tmp=pinfo_str->psubimg)
			{
				_free_SUBIMGBLOCK(pinfo_str->psubimg);
				pinfo_str->psubimg = subimg_tmp;
			}
			_free_SUBIMGBLOCK(pinfo_str->psubimg);
			pinfo_str->psubimg = 0;
		}
		
		pinfo_str->imgnumbers = 0;
		
		return -1;
	}
	
	/* ����ͼҲ������ͼ������� */
	pinfo_str->imgnumbers++;
	
	return 0;
}


/* ��ȡGIFͼ����Ϣ
/
/	����p_filecontentָ��Ļ������������������GIF������
/	����filesize��ָ��p_filecontent��ָ��Ļ������ĳߴ磨��GIF���ĳߴ磩
/	�ɹ�����0��ʧ�ܷ���-1��ͼ��������Щ�����棬���ѳɹ���ȡ������1
/	ע�������һ����̬GIF��ͼ���������1����ͼ��Ŀ������ݽ���
/		�߼����Ŀ���Ϊ׼������Ǿ�̬GIF������ͼ�񣩣�����ͼ��
/		�Ŀ�������Ϊ׼��
*/
int CALLAGREEMENT _get_imginfo(unsigned char *p_filecontent, unsigned long filesize, LPGIFINSIDEINFO p_info)
{
	unsigned char *	p = p_filecontent;
	int				type = 0, i;
	unsigned char	data_len;
	int				color_num;
	int				first_img_mark = 0;
	unsigned long	len = 0;

	GIFHEADER		gifHeader;
	IMAGEDATAINFO	image_data;	/* ͼ�����ݿ� */
	
	
	assert(p_filecontent);
	assert(p_info);
	
	memcpy(&gifHeader,p,sizeof(GIFHEADER));

	if (!memcmp((const void *)gifHeader.gif_type, (const char *)GIF_MARKER_89a, strlen((const char *)GIF_MARKER_89a)))
		type = 1;
	else if (!memcmp((const void *)gifHeader.gif_type, (const char *)GIF_MARKER_87a, strlen((const char *)GIF_MARKER_87a)))
		type = 2;
	else
		return -1;			/* �Ƿ�ͼ������ */

	/* ��д�������� */
	p_info->type = type;
	p_info->scr_width = (unsigned long)gifHeader.scr_width;
	p_info->scr_height= (unsigned long)gifHeader.scr_height;
	p_info->bk_index  = (int)gifHeader.bg_color;
	
	if ((p_info->scr_width==0)||(p_info->scr_height==0))
		return -1;			/* �Ƿ�ͼ������ */

	if (gifHeader.global_flag&0x80)
	{
		p_info->scr_bitcount = (unsigned long)((gifHeader.global_flag&0x7)+1);

		/* ���Ա���ɫ�����ĺϷ��� */
		if (p_info->bk_index >= (1<<p_info->scr_bitcount))
			p_info->bk_index = (1<<p_info->scr_bitcount) - 1;	/* �Ƿ����ݴ�������������� */
			/* return -1; �����Դͼ������Ҫ���ϸ�Ļ����ɽ����� */
			/* һ�еĴ����ñ���return -1;����滻��*/
	}
	else
	{
		p_info->scr_bitcount = (unsigned long)0;
		p_info->bk_index = 0;
	}


	/* ���GIF��ͷ���߼���Ļ������ */
	p += sizeof(GIFHEADER);
	len += sizeof(GIFHEADER);
	
	/* �ж��Ƿ����ȫ�ֵ�ɫ�壬�����������ȡ���ݲ�����õ�ɫ�� */
	if (gifHeader.global_flag&0x80)
	{
		color_num = 1 << p_info->scr_bitcount;

		if (color_num > GIF_MAXCOLOR)
			return -1;			/* �����ͼ�� */

		if ((sizeof(GIFHEADER)+color_num*sizeof(GIFRGB)) >= (int)filesize)
			return -1;			/* �����ͼ�� */

		len += color_num*sizeof(GIFRGB);
		
		for (i=0;i<color_num;i++)
		{
			p_info->p_pal[i].bRed	= *p++;
			p_info->p_pal[i].bGreen = *p++;
			p_info->p_pal[i].bBlue	= *p++;
		}
	}
	else
	{
		/* ���û��ȫ�ֵ�ɫ�����ݣ����ûҶ�ͼ���ݳ�ʼ����ɫ������ */
		for (i=0;i<GIF_MAXCOLOR;i++)
		{
			p_info->p_pal[i].bRed   = (unsigned char)i;
			p_info->p_pal[i].bGreen = (unsigned char)i;
			p_info->p_pal[i].bBlue  = (unsigned char)i;
		}
	}
	
	/* �ֽ������� */
	while (1)
	{
		if (p[0] == 0x2c)	/* ͼ����������[�ֲ���ɫ��]��ͼ�����ݿ� */
		{
			if ((len+sizeof(IMAGEDATAINFO)) >= filesize)
				return -1;				/* �����ͼ�� */

			/* ���ͼ���������ṹ */
			memcpy(&image_data, p, sizeof(IMAGEDATAINFO));
			p += sizeof(IMAGEDATAINFO);
			len += sizeof(IMAGEDATAINFO);

			if (image_data.local_flag&0x80)
			{
				if ((len+(1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB)) >= filesize)
					return -1;
				else
					len += (1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB);
			}

			/* ��д�׷�ͼ����Ϣ */
			if (first_img_mark == 0)
			{
				p_info->first_img_width = (unsigned long)image_data.width;
				p_info->first_img_height= (unsigned long)image_data.height;
				if (image_data.local_flag&0x80)
					p_info->first_img_bitcount = (unsigned long)((image_data.local_flag&0x7)+1);
				else
					p_info->first_img_bitcount = p_info->scr_bitcount;

				if ((p_info->first_img_width==0)||(p_info->first_img_height==0))
					return -1;			/* �����ͼ�� */

				p_info->img_count = 0;

				/* ��ȡ�ֲ���ɫ������ */
				if (image_data.local_flag&0x80) 
				{
					color_num = 1 << p_info->first_img_bitcount;

					if (color_num > GIF_MAXCOLOR)
						return -1;		/* �����ͼ�� */
					
					for (i=0;i<color_num;i++)
					{
						p_info->p_first_pal[i].bRed   = *p++;
						p_info->p_first_pal[i].bGreen = *p++;
						p_info->p_first_pal[i].bBlue  = *p++;
					}
				}
				else
					memmove((void*)p_info->p_first_pal, (const void *)p_info->p_pal, sizeof(GIFRGB)*GIF_MAXCOLOR);

				first_img_mark = 1;
			}
			else
			{
				/* ���׷�ͼ�����ӵ�оֲ���ɫ�����ݣ����� */
				if (image_data.local_flag&0x80)
					p += (1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB);
			}

			if ((len+1) >= filesize)
				return -1;

			p++;				/* Խ��GIF-LZW��С����ֵ */
			len++;
			data_len = 1;
			
			/* �����ѹ����ͼ�������ӿ� */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* �����ͼ���� */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
			/* ͼ�������1 */
			p_info->img_count++;
		}
		else if (p[0] == 0x21 && p[1] == 0xf9 && p[2] == 0x4)	/* ͼ�ο�����չ�� */
		{
			if ((len+sizeof(GRAPHCTRL)) >= filesize)
				return -1;	/* �����ͼ���� */

			p += sizeof(GRAPHCTRL);
			len += sizeof(GRAPHCTRL);
		}
		else if (p[0] == 0x21 && p[1] == 0x1 && p[2] == 0xc)	/* �ı���չ�� */
		{
			if ((len+sizeof(TEXTCTRL)) >= filesize)
				return -1;	/* �����ͼ���� */

			p += sizeof(TEXTCTRL);
			len += sizeof(TEXTCTRL);

			data_len = 1;
			
			/* ����ı��ӿ�����  */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* �����ͼ���� */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xff && p[2] == 0xb)	/* Ӧ�ó�����չ�� */
		{
			if ((len+sizeof(APPCTRL)) >= filesize)
				return -1;	/* �����ͼ���� */

			p += sizeof(APPCTRL);
			len += sizeof(APPCTRL);

			data_len = 1;
			
			/* ����ı��ӿ�����  */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* �����ͼ���� */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xfe)					/* ע����չ�� */
		{
			if ((len+sizeof(NOTEHCTRL)) >= filesize)
				return -1;	/* �����ͼ���� */

			p += sizeof(NOTEHCTRL);
			len += sizeof(NOTEHCTRL);

			data_len = 1;
			
			/* ����ı��ӿ�����  */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* �����ͼ���� */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
		}
		else if (p[0] == 0x0)									/* �˳�������ӿ��β������������ĵ���û��˵�������ֿ��β�����˴�Ϊ������� */
		{
			p++;
			len++;
		}
		else if (p[0] == 0x3b)									/* ���� */
			break;
		else 
			return -1;	/* �����ͼ���� */
		
		if (len >= filesize)
			return -1;	/* �����ͼ���� */
	}

	if ((len+1) != filesize)
		return 1;		/* ��ͼ����β���������ݣ���������GIF�����ݣ����� */
						/* ��ͼ�񱻸������������Ϣ�������ⲻӰ��ͼ���  */
						/* ������ȡ�������Դ���:)�����Է���1������-1     */
	/* �ɹ����� */
	return 0;
}


/* ��������LZWѹ�����ݽ�ѹΪBMP���ݣ�������Щ���ݱ�����һ��������������ݽڵ��� */
LPSUBIMGBLOCK CALLAGREEMENT _decomp_LZW_to_BMP(
	unsigned char *plzw, 			/* ͼ����׵�ַ */
	unsigned char *stream,			/* GIF���׵�ַ */ 
	LPGRAPHCTRL p_imgctrl, 			/* �����Ч��ͼ�������չ�� */
	unsigned long *block_size)		/* ����ͼ�����ݿ��ʵ�ʳߴ� */ 
{
	LZWTABLE		str_table[LZWTABLESIZE];				/* GIF-LZW ���� */
	unsigned char	cw_buf[LZWTABLESIZE];					/* ����ִ�ʱʹ�õĻ����� */

	LPGIFHEADER		p_header = (LPGIFHEADER)stream;			/* GIF��ͷ�ṹ */
	LPIMAGEDATAINFO p_imginfo = (LPIMAGEDATAINFO)plzw;
	unsigned char   *p_data, *p;
	
	unsigned char	first_char;
	int				cw_buf_len, percw;
	int				linesize, init_bit_len;
	unsigned long	imgdata_size, imgblock_size;

	int				lzw_clear_code, lzw_eoi_code, cur_bit_len;
	int				cur_insert_pos, cur_code, old_code, old_code_mark;
	/* Ŀ�껺������ǰдλ����ز��� */
	int				cur_row, cur_x, cur_row_bit_count, intr, pass;
	unsigned short	code_buf;
	unsigned long	bit_count;
	
	int				warninglevel = 0;
	unsigned char   *p_lzwdata = 0;
	LPSUBIMGBLOCK	p_subimg   = 0;
	
	assert(plzw&&((*plzw) == 0x2c));
	assert(stream&&((*stream) == 'G'));
	
	__try
	{
		/* ����һ����ͼ��ڵ� */
		if ((p_subimg=_alloc_SUBIMGBLOCK(plzw, stream, p_imgctrl)) == 0)
			return 0;					/* �ڴ治���ͼ�����ݷǷ� */
		
		assert((p_subimg->p_bit_data)&&(p_subimg->pp_line_addr));

		/* ����ͼ������ƫ�� */
		if (p_imginfo->local_flag&0x80)
			p_data = plzw+sizeof(IMAGEDATAINFO)+(1<<p_subimg->bitcount)*sizeof(GIFRGB);
		else
			p_data = plzw+sizeof(IMAGEDATAINFO);
		
		/* DIB�гߴ� */
		linesize = DIBSCANLINE_WIDTHBYTES(p_subimg->width*p_subimg->bitcount);

		p = p_data;

		/* ��ȡ��ʼλ����ֵ */
		init_bit_len = (int)*p++;

		/* ��ȡ��LZW���ݳ��Ⱥ�LZW���ݿ鳤�� */
		_get_imgdata_size(p, &imgdata_size, &imgblock_size);

		/* ����ʵ��ͼ��飨LZW�飩�ĳ��� */
		*block_size = imgblock_size;

		/* ��LZW������ȡ��һ���µġ��������ڴ���� */
		if ((p_lzwdata=_get_lzw_datablock(p, imgdata_size)) == 0)
		{
			_free_SUBIMGBLOCK(p_subimg);
			return 0;					/* �ڴ治�� */
		}


		/* ����ǰ�ĳ�ʼ������ */
		p = p_lzwdata;

		intr = (p_imginfo->local_flag&0x40) ? 1:0;	/* �����־ */
		cur_row = cur_x = cur_row_bit_count = 0;	/* Ŀ��ͼ�񻺳����С���λ�� */
		pass = 1;									/* ����ͨ��ֵ */

		lzw_clear_code = 1<<init_bit_len;			/* ����� */
		lzw_eoi_code   = lzw_clear_code+1;			/* �ս��� */
		cur_bit_len    = init_bit_len+1;			/* ��ǰ�볤�� */
		cur_insert_pos = lzw_eoi_code+1;			/* ��ǰ�������λ�� */
		bit_count      = 0;							/* �����ж�ȡ������λ���� */
		old_code       = 0;							/* ǰ׺�� */
		old_code_mark  = 0;							/* �ױ�־ */


		/* ��ʼ���� */
		while((cur_code=(int)_get_next_codeword(p, imgdata_size, bit_count/8, bit_count%8, cur_bit_len)) != lzw_eoi_code)
		{
			if (cur_code == lzw_clear_code)				/* ����� */
			{
				bit_count      += cur_bit_len;
				cur_bit_len    = init_bit_len+1;
				cur_insert_pos = lzw_eoi_code+1;

				old_code       = 0;
				old_code_mark  = 0;

				/* ��ʼ������ */
				_init_lzw_table((LZWTABLE*)str_table, LZWTABLESIZE, init_bit_len);

				warninglevel = 0;	/* ������� */
				continue;
			}
			else if (cur_code < cur_insert_pos)			/* �����ڴ����� */
			{
				if (warninglevel)	/* ��ֹд������� */
				{
					_free_SUBIMGBLOCK(p_subimg); 
					p_subimg = 0;
					__leave;
				}

				/* ������� */
				cw_buf_len = 0;
				percw = cur_code;

				/* �����ֶ�Ӧ��ͼ�����ݴ����뻺���� */
				while(percw != -1)
				{
					/* assert(str_table[percw].used == 1); */
					cw_buf[cw_buf_len++] = (unsigned char)str_table[percw].code;
					percw = (int)str_table[percw].prefix;
				}

				/* д��Ŀ��ͼ�񻺳��� */
				while(cw_buf_len > 0)
				{
					if (intr)	/* �Ƿ��ǽ���ģʽ */
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0; cur_row_bit_count = 0;

							/* �������� */
							if(pass == 1)
								cur_row += 8;
							if(pass == 2)
								cur_row += 8;
							if(pass == 3)
								cur_row += 4;
							if(pass == 4)
								cur_row += 2;

							if(cur_row >= p_subimg->height)
							{
								pass++;
								cur_row = 0x10 >> pass;
							}

							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
							
						}
					}
					else
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0;
							cur_row_bit_count = 0;
							cur_row++;
							
							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
						}
					}

					/* ���ͼ�����ݲ��ƶ���Ԥ��λ�� */
					code_buf = (unsigned short)(unsigned char)(cw_buf[cw_buf_len-1]);
					code_buf &= (unsigned short)((1<<(p_subimg->bitcount))-1);
					code_buf <<= cur_row_bit_count%8;

					/* ���������ݰ�����λд��ͼ�񻺳��� */
					*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8)) |= (unsigned char)code_buf;
					if (((cur_row_bit_count%8)+p_subimg->bitcount) > 8)
						*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8+1)) |= (unsigned char)(code_buf>>8);

					cur_x++;
					cur_row_bit_count += p_subimg->bitcount;

					cw_buf_len--;
				}
					
				/* �Ƿ��ǵ�һ�ζ��� */
				if (old_code_mark)
				{
					/* �򴮱�����������֣�������������£� */
					percw = cur_code;

					/* ȡ�õ�ǰ���ֵ�һ���ַ� */
					while(percw != -1)
					{
						/* assert(str_table[percw].used == 1); */
						first_char = (unsigned char)str_table[percw].code;
						percw = (int)str_table[percw].prefix;
					}

					/* ��������ֵ�LZW������ */
					str_table[cur_insert_pos].used = 1;
					str_table[cur_insert_pos].code = (short)first_char;
					str_table[cur_insert_pos].prefix = old_code;

					/* ����λ�ú��� */
					cur_insert_pos++;
				}
				else
					old_code_mark = 1;					/* ��һ�ν��� */

				old_code = cur_code;
				bit_count += cur_bit_len;
			}
			else										/* �����ڵı��� */
			{
				if (warninglevel)	/* ��ֹд������� */
				{
					_free_SUBIMGBLOCK(p_subimg); 
					p_subimg = 0;
					__leave;
				}

				/* �򴮱�����������֣���������������£� */
				percw = old_code;

				/* ȡ��ǰ׺���ֵ�һ���ַ� */
				while(percw != -1)
				{
					/* assert(str_table[percw].used == 1); */
					first_char = (unsigned char)str_table[percw].code;
					percw = (int)str_table[percw].prefix;
				}

				/* ��������� */
				str_table[cur_insert_pos].used = 1;
				str_table[cur_insert_pos].code = (short)first_char;
				str_table[cur_insert_pos].prefix = old_code;

				cur_insert_pos++;

				/* ������� */
				cw_buf_len = 0;
				/* ����ո����ɵ������� */
				percw = cur_insert_pos-1;
				
				/* �����ֶ�Ӧ��ͼ�����ݴ����뻺���� */
				while(percw != -1)
				{
					/* assert(str_table[percw].used == 1); */
					cw_buf[cw_buf_len++] = (unsigned char)str_table[percw].code;
					percw = (int)str_table[percw].prefix;
				}
				
				while(cw_buf_len > 0)
				{
					if (intr)	/* �Ƿ��ǽ���ģʽ */
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0; cur_row_bit_count = 0;
							
							if(pass == 1)
								cur_row += 8;
							if(pass == 2)
								cur_row += 8;
							if(pass == 3)
								cur_row += 4;
							if(pass == 4)
								cur_row += 2;
							
							if(cur_row >= p_subimg->height)
							{
								pass++;
								cur_row = 0x10 >> pass;
							}

							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
						}
					}
					else
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0;
							cur_row_bit_count = 0;
							cur_row++;

							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
						}
					}
					
					code_buf = (unsigned short)(cw_buf[cw_buf_len-1]);
					code_buf &= (unsigned short)((1<<(p_subimg->bitcount))-1);
					code_buf <<= cur_row_bit_count%8;
					
					/* ���������ݰ�����λд��ͼ�񻺳��� */
					*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8)) |= (unsigned char)code_buf;
					if (((cur_row_bit_count%8)+p_subimg->bitcount) > 8)
						*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8+1)) |= (unsigned char)(code_buf>>8);
					
					cur_x++;
					cur_row_bit_count += p_subimg->bitcount;
					
					cw_buf_len--;
				}
				
				old_code = cur_code;
				bit_count += cur_bit_len;
			}

			if ((cur_insert_pos>>cur_bit_len) > 0)
				cur_bit_len++;

			if (cur_bit_len > 12)
			{
				cur_bit_len = 12;
				warninglevel = 1;	/* ���뾯��״̬ */
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			if (p_subimg)
				_free_SUBIMGBLOCK(p_subimg);
			p_subimg = 0;
		}
		if (p_lzwdata)
			free(p_lzwdata);
		p_lzwdata = 0;
	}
	
	return p_subimg;
}


int CALLAGREEMENT _load_subimg_serial(LPINFOSTR pinfo_str, unsigned char *stream, unsigned long stream_len)
{
	unsigned char *p = stream;
	unsigned char *pimg;
	int type = 0;
	unsigned char data_len;
	unsigned long block_size;
	int			  first_img_mark = 0;

	GIFHEADER		gifHeader;
	IMAGEDATAINFO	image_data;			/* ͼ�����ݿ� */
	GRAPHCTRL		cur_craph_ctrl;		/* ��ǰ��ͼ��������� */

	LPSUBIMGBLOCK	subimg_list = 0;	/* ��ͼ������ */
	LPSUBIMGBLOCK	subimg_tail = 0;
	LPSUBIMGBLOCK	subimg_tmp  = 0;
	
	
	assert(p);
	assert(pinfo_str);
	
	memset((void*)&cur_craph_ctrl, 0, sizeof(GRAPHCTRL));
	memcpy(&gifHeader,p,sizeof(GIFHEADER));

	/* ���GIF��ͷ���߼���Ļ������ */
	p += sizeof(GIFHEADER);

	/* ���ȫ�ֵ�ɫ�� */
	if (gifHeader.global_flag&0x80)
		p += (1<<((gifHeader.global_flag&0x7)+1))*sizeof(GIFRGB);
	
	/* ��ʼ��ͼ��������� */
	cur_craph_ctrl.extintr			= 0x21;
	cur_craph_ctrl.label			= 0xf9;
	cur_craph_ctrl.block_size		= 0x4;
	cur_craph_ctrl.delay_time		= 0;
	cur_craph_ctrl.field			= 0;
	cur_craph_ctrl.tran_color_index = -1;
	cur_craph_ctrl.block_terminator = 0;
	

	/* �ֽ������� */
	while (1)
	{
		if (p[0] == 0x2c)	/* ͼ����������[�ֲ���ɫ��]��ͼ�����ݿ� */
		{
			/* ����ͼ���������׵�ַ */
			pimg = p;

			/* ���ͼ���������ṹ */
			memcpy(&image_data,p,sizeof(IMAGEDATAINFO));
			p += sizeof(IMAGEDATAINFO);
			
			/* ���ӵ�оֲ���ɫ�����ݣ����� */
			if (image_data.local_flag&0x80)
				p += (1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB);

			/* ����ͼ����н��� */
			subimg_tmp = _decomp_LZW_to_BMP(pimg, stream, &cur_craph_ctrl, &block_size);

			/* �������ʧ�ܣ��ͷ���ͼ���������ش��� */
			if (subimg_tmp == 0)
			{
				if (subimg_list != 0)
				{
					while(subimg_tmp=subimg_list->next)
					{
						_free_SUBIMGBLOCK(subimg_list);
						subimg_list = subimg_tmp;
					}
					_free_SUBIMGBLOCK(subimg_list);
					subimg_list = 0;
				}
				return -1;
			}
			
			/* ����ͼ�������ͼ������β�� */
			if (subimg_list == 0)
			{				  
				subimg_list = subimg_tmp;	/* ����ͷ */
				subimg_tail = subimg_tmp;	/* ����β */

				subimg_tmp->prev = 0;
				subimg_tmp->next = 0;
				subimg_tmp->number = 1;
				subimg_tmp->parents = pinfo_str;
			}
			else
			{
				subimg_tmp->prev = subimg_tail;
				subimg_tmp->next = 0;
				subimg_tmp->number = subimg_tail->number+1;
				subimg_tmp->parents = pinfo_str;

				assert(subimg_tail->next == 0);

				subimg_tail->next = subimg_tmp;
				subimg_tail = subimg_tmp;
			}

			/* ͼ�������1 */
			pinfo_str->imgnumbers++;

			/* ���ͼ������ */
			p += block_size;
		}
		else if (p[0] == 0x21 && p[1] == 0xf9 && p[2] == 0x4)	/* ͼ�ο�����չ�� */
		{
			memcpy((void*)&cur_craph_ctrl, (const void *)p, sizeof(GRAPHCTRL));
			p += sizeof(GRAPHCTRL);
		}
		else if (p[0] == 0x21 && p[1] == 0x1 && p[2] == 0xc)	/* �ı���չ�� */
		{
			p += sizeof(TEXTCTRL);
			data_len = 1;
			
			/* ����ı��ӿ�����  */
			while (data_len)
			{
				data_len = *p++;
				p += data_len;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xff && p[2] == 0xb)	/* Ӧ�ó�����չ�� */
		{
			p += sizeof(APPCTRL);
			data_len = 1;
			
			/* ����ı��ӿ�����  */
			while (data_len)
			{
				data_len = *p++;
				p += data_len;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xfe)					/* ע����չ�� */
		{
			p += sizeof(NOTEHCTRL);
			data_len = 1;
			
			/* ����ı��ӿ�����  */
			while (data_len)
			{
				data_len = *p++;
				p += data_len;
			}
		}
		else if (p[0] == 0x0)									/* �˳�������ӿ��β������������ĵ���û��˵�������ֿ��β�����˴�Ϊ������� */
		{
			p++;
		}
		else if (p[0] == 0x3b)									/* ���� */
			break;
		else 
		{
			/* �ͷ���ͼ������ */
			if (subimg_list != 0)
			{
				while(subimg_tmp=subimg_list->next)
				{
					_free_SUBIMGBLOCK(subimg_list);
					subimg_list = subimg_tmp;
				}
				_free_SUBIMGBLOCK(subimg_list);
				subimg_list = 0;
			}

			pinfo_str->imgnumbers = 0;

			return -1;	/* �����ͼ���� */
		}
	}

	pinfo_str->psubimg = subimg_list;

	/* �ɹ����� */
	return 0;
}



/* ��ʼ��LZW���� */
void CALLAGREEMENT _init_lzw_table(LZWTABLE *pt, int table_size, int init_bit_len)
{
	int i, len;

	assert(init_bit_len <= 12);

	memset((void*)pt, 0, sizeof(LZWTABLE)*table_size);

	len = 1<<init_bit_len;

	/* ���� */
	for (i=0;i<len;i++)
	{
		pt[i].prefix = -1;
		pt[i].code   = (short)i;
		pt[i].used   = 1;
	}

	/* ����� */
	pt[len].prefix = -1;
	pt[len].code   = -1;
	pt[len].used   = 1;

	/* ������ */
	pt[len+1].prefix = -1;
	pt[len+1].code   = -1;
	pt[len+1].used   = 1;
}


/* ����һ����ͼ��ڵ㣬����ʼ���ڵ������� */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(
	 unsigned char *plzw, 			/* ͼ����׵�ַ */
	 unsigned char *stream,			/* GIF���׵�ַ */ 
	 LPGRAPHCTRL p_imgctrl)			/* �����Ч��ͼ�������չ�� */
{
	LPGIFHEADER		p_header = (LPGIFHEADER)stream;
	LPIMAGEDATAINFO p_imginfo = (LPIMAGEDATAINFO)plzw;
	LPGIFRGB		p_pal;

	int				linesize, i, colorkey;
	
	LPSUBIMGBLOCK	p_subimg = 0;


	/* ������ͼ����Ϣ�ڵ�(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* �ڴ治�� */
	
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	/* ������ͼ��ڵ����� */
	p_subimg->left = (int)p_imginfo->left;
	p_subimg->top = (int)p_imginfo->top;
	p_subimg->width = (int)p_imginfo->width;
	p_subimg->height= (int)p_imginfo->height;

	if ((p_subimg->width==0)||(p_subimg->height==0))
	{
		free(p_subimg);				/* �Ƿ�ͼ������ */
		return 0;
	}

	/* ��ȡλ����ֵ */
	if (p_imginfo->local_flag&0x80)
	{
		p_subimg->bitcount = (p_imginfo->local_flag&0x7)+1;
		p_pal = (LPGIFRGB)(unsigned char *)(plzw+sizeof(IMAGEDATAINFO));
	}
	else
	{
		if (!(p_header->global_flag&0x80))
		{
			free(p_subimg);
			return 0;				/* �Ƿ�ͼ�񣨼�û����ͼ��λ��Ҳû��ȫ��λ��ֵ��*/
		}
		else
		{
			p_subimg->bitcount = (p_header->global_flag&0x7)+1;
			p_pal = (LPGIFRGB)(unsigned char *)(p_header+1);
		}
	}
	
	/* DIB�гߴ� */
	linesize = DIBSCANLINE_WIDTHBYTES(p_subimg->width*p_subimg->bitcount);

	/* ����Ŀ��ͼ�񻺳�����+4 �� ���ӵ�4�ֽڻ������� */
	if ((p_subimg->p_bit_data = (unsigned char *)malloc(linesize*p_subimg->height+4)) == 0)
	{
		free(p_subimg);
		return 0;					/* �ڴ治�� */
	}

	/* �����е�ַ���� */
	if ((p_subimg->pp_line_addr = (void**)malloc(sizeof(void*)*p_subimg->height)) == 0)
	{
		free(p_subimg->p_bit_data);
		free(p_subimg);
		return 0;					/* �ڴ治�� */
	}

	/* ��ʼ��ͼ�񻺳��� */
	memset((void*)p_subimg->p_bit_data, 0, linesize*p_subimg->height);

	/* GIFΪ����ͼ�� */
	p_subimg->order   = 0;

	for (i=0;i<p_subimg->height;i++)
		p_subimg->pp_line_addr[i] = (void *)(p_subimg->p_bit_data+(i*linesize));
	
	/* GIFΪ����ͼ������������ */
	p_subimg->r_mask = 0;
	p_subimg->g_mask = 0;
	p_subimg->b_mask = 0;
	p_subimg->a_mask = 0;


	/* ���÷�ʽ��0 - δָ�����÷�ʽ
				 1 - �������κδ�����
				 2 - �Ա���ɫ����
				 3 - �ظ�ԭ���ı���ͼ�� */
	p_subimg->dowith= (int)((p_imgctrl->field&0x1c)>>2);
	/* ����ʾ��һ����ͼ��ǰ�Ƿ���Ҫ�ȴ�ĳһ�¼� */
	p_subimg->userinput = (int)((p_imgctrl->field&0x2)>>1);
	/* �ӳ�ʱ�䣨1/1000 s��*/
	p_subimg->time = (int)p_imgctrl->delay_time;
	/* ����͸��ɫ���������û��͸��ɫ������Ϊ-1 */
	colorkey = (p_imgctrl->field&0x1) ? p_imgctrl->tran_color_index : -1;


	/* ���Ƶ�ɫ������ */
	p_subimg->pal_count = 1 << p_subimg->bitcount;

	for (i=0;i<p_subimg->pal_count;i++)
	{
		((LPISEERGB)(&(p_subimg->palette[i])))->bRed	= p_pal[i].bRed;
		((LPISEERGB)(&(p_subimg->palette[i])))->bGreen	= p_pal[i].bGreen;
		((LPISEERGB)(&(p_subimg->palette[i])))->bBlue	= p_pal[i].bBlue;
		((LPISEERGB)(&(p_subimg->palette[i])))->rev		= 0;
	}

	if (colorkey != -1)
		p_subimg->colorkey = p_subimg->palette[colorkey];
	else
		p_subimg->colorkey = (unsigned long)-1;

	
	p_subimg->number  = 0;			/* ��ͼ�������к�����Ϊ0 */
	
	p_subimg->parents = 0;
	p_subimg->prev    = 0;
	p_subimg->next    = 0;
	
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


/* ��������ͼ����Զ�̬ͼ�� */
int CALLAGREEMENT _build_bkimg(LPINFOSTR pinfo_str, LPGIFINSIDEINFO p_info)
{
	int linesize, i;
	unsigned char *pline = 0;
	unsigned char *pimg  = 0;
	void **ppaddr = 0;
	unsigned char bkindex = p_info->bk_index;
	
	int byte_pos, bit_pos, bit_count;
	unsigned char c;
	
	assert((pinfo_str->width > 0)&&(pinfo_str->height > 0));
	
	linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);
	
	/* ���������л����� */
	if ((pline = (unsigned char *)malloc(linesize)) == 0)
		return -1;

	/* ��ʼ��Ϊ0 */
	memset((void*)pline, 0, linesize);
	
	/* ���䱳��ͼ�񻺳�����+4 �� ���ӵ�4�ֽڻ������� */
	if ((pimg = (unsigned char *)malloc(linesize*pinfo_str->height+4)) == 0)
	{
		free(pline);
		return -1;
	}
	
	/* �������׵�ַ���� */
	if ((ppaddr = (void**)malloc(sizeof(void*)*pinfo_str->height)) == 0)
	{
		free(pimg);
		free(pline);
		return -1;
	}
	
	/* ��ʼ�����׵�ַ��GIF���� */
	for (i=0;i<(int)pinfo_str->height;i++)
		ppaddr[i] = (void*)(pimg+(i*linesize));
	
	assert(pinfo_str->bitcount <= 8);
	assert(bkindex < (1<<pinfo_str->bitcount));
	
	/* ��д������ */
	byte_pos = bit_pos = bit_count = 0;
	for (i=0;i<(int)pinfo_str->width;i++)
	{
		byte_pos = bit_count/8;
		bit_pos  = bit_count%8;
		
		c = (unsigned char)bkindex;
		c <<= bit_pos;
		pline[byte_pos] |= c;
		
		if ((bit_pos+pinfo_str->bitcount) > 8)
		{
			c = (unsigned char)bkindex;
			c >>= 8-bit_pos;
			pline[byte_pos+1] |= c;
		}
		
		bit_count += pinfo_str->bitcount;
	}
	
	/* ������ͼ��ʼ��Ϊ����ɫ�ĵ�ɫͼ */
	for (i=0;i<(int)pinfo_str->height;i++)
		memmove((void*)ppaddr[i], (const void *)pline, linesize);
	
	
	pinfo_str->pp_line_addr = ppaddr;
	pinfo_str->p_bit_data   = pimg;
	pinfo_str->pal_count = 1<<pinfo_str->bitcount;
	
	/* ���Ƶ�ɫ������ */
	for (i=0;i<(int)pinfo_str->pal_count;i++)
	{
		((LPISEERGB)(&(pinfo_str->palette[i])))->bRed   = p_info->p_pal[i].bRed;
		((LPISEERGB)(&(pinfo_str->palette[i])))->bGreen = p_info->p_pal[i].bGreen;
		((LPISEERGB)(&(pinfo_str->palette[i])))->bBlue  = p_info->p_pal[i].bBlue;
		((LPISEERGB)(&(pinfo_str->palette[i])))->rev    = 0;
	}
	
	free(pline);
	
	return 0;
}


/* ����ͼ��λ���ݵĳߴ磨������ͼ�����ݳߴ��ͼ�����ݿ������ߴ磩
/		
/	ע����ߴ罫�����ײ��ĳ�ʼλ�õ��ֽڡ�
*/
void CALLAGREEMENT _get_imgdata_size(
	unsigned char *stream,			/* ָ��ͼ�����ݿ��׵�ַ��ָ�루�ѿ���˳�ʼλ�����ֽڣ�*/
	unsigned long *imgdata_size, 
	unsigned long *imgblock_size)
{
	unsigned char *p = stream;
	unsigned long data_count, block_count;
	unsigned char data_len;

	assert(p);

	data_count = block_count = 0UL;

	while(1)
	{
		data_len = *p++;				/* ��ȡ���������ֽ�ֵ��������ָ�� */
		p += data_len;
		data_count += (unsigned long)data_len;
		block_count += ((unsigned long)data_len+1);
										/* ��1�Ǹ����������ӿ�ĳ����ֽ� */
		if (data_len == 0) 
			break;
	}

	block_count++;						/* ���LZW��ʼλֵ��һ���ֽ� */
	
	*imgdata_size  = data_count;
	*imgblock_size = block_count;
}


/* ��GIF�еĴ�LZWͼ��������ȡ���µ��ڴ���� 
/
/	����ڴ����ʧ�ܣ�����0
*/
unsigned char * CALLAGREEMENT _get_lzw_datablock(
	unsigned char *stream,				/* ͼ����׵�ַ���ѿ����ʼλ�����ֽڣ�*/
	unsigned long lzw_len)
{
	unsigned char *p1, *p2, *pdata, data_len;
	unsigned long count = 0UL;

	assert(stream);
	
	/* �����ڴ�飬���ڴ�Ŵ�LZW���� */
	if ((pdata = (unsigned char *)malloc(lzw_len+1)) == (unsigned char *)0)
		return (unsigned char *)0;
	
	p1 = stream;
	p2 = pdata;
	
	while (1)
	{
		data_len = *p1++;
		if (data_len == 0x00) 
			break;

		/* ��ֹ�������ݸ��ų��� */
		count += (unsigned long)data_len;
		if (count > lzw_len)
		{
			free(pdata);
			return (unsigned char *)0;
		}

		/* ��LZW���ݸ��Ƶ��·�����ڴ���� */
		memcpy(p2, p1, data_len);

		p1 += data_len;
		p2 += data_len;
	}

	assert(count == lzw_len);
	
	return pdata;
}


/* ȡ��LZW����һ��������  */
unsigned short CALLAGREEMENT _get_next_codeword(
	unsigned char *code_stream,			/* LZW�������׵�ַ */
	unsigned long code_stream_len,		/* ������ */
	unsigned long offset_byte,			/* ��ǰ�������ֽ�ƫ�� */
	unsigned long offset_bits,			/* ��ǰ������λƫ�� */
	unsigned long code_word_bit_size)	/* ����λ���� */
{
	CODEWORD	cw;
	LPCODEWORD	p_code_word;

	assert(code_word_bit_size <= 12);
	
	p_code_word = (LPCODEWORD)(unsigned char *)(code_stream + offset_byte);

	/* 12λ��ȡ3���ֽڼ��� */
	if ((offset_byte+0) < code_stream_len)
		cw.bcode[0] = p_code_word->bcode[0];
	if ((offset_byte+1) < code_stream_len)
		cw.bcode[1] = p_code_word->bcode[1];
	if ((offset_byte+2) < code_stream_len)
		cw.bcode[2] = p_code_word->bcode[2];
	
	/* �����������ֵͶ� */
	cw.lcode >>= offset_bits;
	cw.lcode  &= ((1<<code_word_bit_size)-1);
	
	return (unsigned short)cw.lcode;
}


