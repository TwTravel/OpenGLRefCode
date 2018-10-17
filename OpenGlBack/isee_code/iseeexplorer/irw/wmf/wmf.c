/********************************************************************

	wmf.c

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
			http://cosoft.org.cn/projects/iseeexplorer

	���ŵ���

			isee##vip.163.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������WMFͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�	�ɶ�ȡWindows��׼Ԫ�ļ���Aldus�ɷ���Ԫ�ļ���
							  
					���湦�ܣ�	��֧�ֱ��湦�ܡ�
							   

	���ļ���д�ˣ�	YZ			yzfree##sina.com����һ�棩
					swstudio	swstudio##sohu.com���ڶ��棩

	���ļ��汾��	20621
	����޸��ڣ�	2002-6-21

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��
		
		2002-6		�����°���������ȱ��
		2002-5		�ڶ������������°棩
		2001-1		��������ע����Ϣ
		2000-7		�����ദBUG������ǿ��ģ����ݴ���
		2000-6		��һ���汾����


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

#include "wmf.h"


IRWP_INFO			wmf_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	wmf_get_info_critical;	/* wmf_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	wmf_load_img_critical;	/* wmf_load_image�����Ĺؼ��� */
CRITICAL_SECTION	wmf_save_img_critical;	/* wmf_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);

static int _verify_file(ISFILE* pfile);
static int _get_info(ISFILE* pfile, LPINFOSTR pinfo_str);
static int _load_metafile(ISFILE* pfile, HMETAFILE* phout);
static int _meta_to_raster(HMETAFILE hwmf, LPINFOSTR pinfo_str);
static int CALLBACK _EnumMetaFileProc(HDC hDC,  HANDLETABLE *lpHTable,  METARECORD *lpMFR, int nObj, LPARAM lpClientData);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&wmf_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&wmf_get_info_critical);
			InitializeCriticalSection(&wmf_load_img_critical);
			InitializeCriticalSection(&wmf_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&wmf_get_info_critical);
			DeleteCriticalSection(&wmf_load_img_critical);
			DeleteCriticalSection(&wmf_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

WMF_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&wmf_irwp_info;
}

#else

WMF_API LPIRWP_INFO CALLAGREEMENT wmf_get_plugin_info()
{
	_init_irwp_info(&wmf_irwp_info);

	return (LPIRWP_INFO)&wmf_irwp_info;
}

WMF_API void CALLAGREEMENT wmf_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

WMF_API void CALLAGREEMENT wmf_detach_plugin()
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
	lpirwp_info->irwp_author_count = 2;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"�ܸ����������������������ʸ��ͼ�Ĺ�������ʹ��ѧ���˲���Ԫ�ļ���֪ʶ^_^");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"swstudio");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"swstudio##sohu.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"GDI������ϵͳ��faint:)");
	/* ---------------------------------[2] �� ������ -------------- */
	/* ������������Ϣ�ɼ��ڴ˴���
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_message), 
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
				(const char *)"APM");

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}


/* ��ȡͼ����Ϣ */
WMF_API int CALLAGREEMENT wmf_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
#	ifdef WIN32

	ISFILE			*pfile = (ISFILE*)0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&wmf_get_info_critical);

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

			/* ��֤wmf����ȷ�� */
			if ((b_status = _verify_file(pfile)) != ER_SUCCESS)
			{
				__leave;
			}

			if ((b_status = _get_info(pfile, pinfo_str)) != ER_SUCCESS)
			{
				__leave;
			}

			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&wmf_get_info_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		pinfo_str->data_state = 0;
		b_status = ER_SYSERR;
	}

#	else

	b_status = ER_NOTSUPPORT;

#	endif /* WIN32 */

	return (int)b_status;
}


/* ��ȡͼ��λ���� */
WMF_API int CALLAGREEMENT wmf_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
#	ifdef WIN32

	ISFILE			*pfile = (ISFILE*)0;

	enum EXERESULT	b_status = ER_SUCCESS;

	HMETAFILE hwmf = NULL;
	
	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&wmf_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total   = 1;
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

			/* ��֤wmf�ļ�����Ч�� */
			if ((b_status = _verify_file(pfile)) != ER_SUCCESS)
			{
				__leave;
			}
			
			/* ����ǿյ����ݰ������Ȼ�ȡͼ���Ҫ��Ϣ���ɹ����ٶ�ȡͼ�� */
			if (pinfo_str->data_state == 0)
			{
				b_status = _get_info(pfile, pinfo_str);
				if (b_status != ER_SUCCESS)
				{
					__leave;
				}
				pinfo_str->data_state = 1;
			}
			
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;

			assert(pinfo_str->data_state == 1);

			if ((b_status = _load_metafile(pfile, &hwmf)) != ER_SUCCESS)
			{
				__leave;
			}

			assert(hwmf);

			if ((b_status = _meta_to_raster(hwmf, pinfo_str)) != ER_SUCCESS)
			{
				__leave;
			}
			
			/* ��д���ݰ��������� */
			pinfo_str->pal_count = 0;
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg = NULL;	
						

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

			if (hwmf != NULL)
			{
				DeleteMetaFile(hwmf);
			}
			
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&wmf_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}

#	else
	
	b_status = ER_NOTSUPPORT;
	
#	endif /* WIN32 */
	
	return (int)b_status;
}



/* ����ͼ�� */
WMF_API int CALLAGREEMENT wmf_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&wmf_save_img_critical);
	
			/* ��ǰ������֧�ֱ��湦�� */
			b_status = ER_NOTSUPPORT;
			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			LeaveCriticalSection(&wmf_save_img_critical);
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


/* �ж�ָ��ͼ���Ƿ���WMF */
static int _verify_file(ISFILE* pfile)
{
	enum EXERESULT	b_status = ER_SUCCESS;

	unsigned int	is_aldus, filesize, wmf_offset = 0;
	METAHEADER		meta_hdr;

	__try
	{
		__try
		{
			/* ������֤�ļ����ȣ���Ӧ��С��METAHEADER�ṹ�����һ��6�ֽڵ�β��¼���� */
			if (isio_seek(pfile, 0, SEEK_END) == -1
				|| (filesize = isio_tell(pfile)) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			if (filesize < (sizeof(METAHEADER) + 6))
			{
				b_status = ER_BADIMAGE; __leave;
			}
            
			/* ��ȡ�ļ���ͷ���ֽڣ��ж��ǲ���placeableԪ�ļ� */
			if (isio_seek(pfile, 0, SEEK_SET) == -1
				|| isio_read(&is_aldus, sizeof(is_aldus), 1, pfile) != 1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (is_aldus == ALDUSKEY)
			{
				ALDUSMETAHEADER aldus_hdr;
				WORD *pw;
				WORD checksum = 0;
				
				/*  ����ǣ��ٴ���֤�ļ����� */
				if (filesize < (ALDUSMETAHEADERSIZE + sizeof(METAHEADER) + 6))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* ��ȡplaceableԪ�ļ�ͷ����������ͼ�顣 */
				if (isio_seek(pfile, 0, SEEK_SET) == -1
					|| isio_read(&aldus_hdr, ALDUSMETAHEADERSIZE, 1, pfile) != 1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				for (pw = (WORD*)&aldus_hdr; pw < (WORD*)&aldus_hdr.checksum; pw++)
				{
					checksum ^= *pw;
				}

				/* ��ʧ�ܣ���ʾ�ļ����� */
				if (checksum != aldus_hdr.checksum)
				{
					b_status = ER_BADIMAGE; __leave;
				}

                wmf_offset = ALDUSMETAHEADERSIZE;
			}
							
			/* ��ȡwindows��׼Ԫ�ļ�ͷ�����м�¼��Ԫ�ļ���С��������placeableԪ�ļ�
			   ͷ���ڣ���WORDΪ��λ����Ԫ�ļ����Ȳ�Ӧ��С�ڴ� */
			if (isio_seek(pfile, wmf_offset, SEEK_SET) == -1
				|| isio_read(&meta_hdr, sizeof(METAHEADER), 1, pfile) != 1) 
			{
				b_status = ER_FILERWERR; __leave;
			}
			if (filesize < (meta_hdr.mtSize*2-wmf_offset))
			{
				b_status = ER_BADIMAGE; __leave;
			}
		}
		__finally
		{
			
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}

	return (int)b_status;
}


/* ��ȡͼ����Ϣ */
static int _get_info(ISFILE* pfile, LPINFOSTR pinfo_str)
{
	enum EXERESULT	b_status = ER_SUCCESS;
	
	unsigned int	is_aldus;
	HDC				hdc = NULL;

	assert(pfile);
	assert(pinfo_str);

	__try
	{
		__try
		{
			hdc = GetDC(GetDesktopWindow());
			if (hdc == NULL)
			{
				b_status = ER_SYSERR; __leave;
			}
						
			/* ����ͼ����߶ȣ���׼wmf�ļ�δ��¼�����Ϣ��Ĭ��Ϊ��Ļ���߶� */
			pinfo_str->width  = GetDeviceCaps(hdc, HORZRES);
			pinfo_str->height = GetDeviceCaps(hdc, VERTRES);

			/* ����ͼ��Ϊ������� */
			pinfo_str->order = 1;

			/* ����λ���,����wmf�ļ���δ��¼�����Ϣ��ֱ���趨Ϊ24bit */
			pinfo_str->bitcount = 24;

			/* ���ø���ɫ�������� */
			pinfo_str->r_mask = 0xff0000;
			pinfo_str->g_mask = 0xff00;
			pinfo_str->b_mask = 0xff;
			pinfo_str->a_mask = 0x0;

			/* ����ǿɷ���Ԫ�ļ������ļ�ͷ�л�ÿ�ȡ��߶� */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			if (isio_read(&is_aldus, sizeof(is_aldus), 1, pfile) != 1)
			{
				b_status = ER_FILERWERR; __leave;
			}
				
			if (is_aldus == ALDUSKEY)
			{
				ALDUSMETAHEADER hdr;
				float fx, fy;
				if (isio_seek(pfile, 0, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			
				if (isio_read(&hdr, ALDUSMETAHEADERSIZE, 1, pfile) != 1)
				{
					b_status = ER_FILERWERR; __leave;
				}
		
				/* ���ȼ���ͼ��ĵ�λ�ߴ� */
				fx = (float)(hdr.right-hdr.left);
				fy = (float)(hdr.bottom-hdr.top);
		
				/* hdr.inch��ʾһӢ���൱�ڶ��ٵ�λ���ɴ˼������Ӣ���ʾ��ͼ��ߴ磬
				   �ٳ�����Ļ��DPIֵ���Ӷ��õ�ͼ��������Ϊ��λ�ĳߴ�*/
				fx = (fx/(float)hdr.inch)*(float)GetDeviceCaps(hdc, LOGPIXELSX)+0.5f;
				fy = (fy/(float)hdr.inch)*(float)GetDeviceCaps(hdc, LOGPIXELSY)+0.5f;
				pinfo_str->width = (unsigned long)fx;
				pinfo_str->height = (unsigned long)fy;
			}
			/* ����ͼ�����͡��洢��ʽ������ѹ����ʽ */
			pinfo_str->imgtype     = IMT_VECTORSTATIC;
			pinfo_str->imgformat   = is_aldus == ALDUSKEY ? IMF_APM : IMF_WMF;
			pinfo_str->compression = ICS_GDIRECORD;
		}
		__finally
		{
			if (hdc != NULL)
			{
				ReleaseDC(GetDesktopWindow(), hdc);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
	
}


/* ��WMF���ݣ�������HMETAFILE��� */
static int _load_metafile(ISFILE* pfile, HMETAFILE* phout)
{
	enum EXERESULT	b_status = ER_SUCCESS;

	METAHEADER		meta_hdr;
	unsigned long	is_aldus;
	unsigned char	*meta_bits = NULL;
	int				wmf_offset = 0;

	assert(pfile);

	__try
	{
		__try
		{
			/* ��ȡ�ļ�ͷ��ʶ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1 
				|| isio_read(&is_aldus, sizeof(is_aldus), 1, pfile) != 1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			/* ����ǿɷ���Ԫ�ļ���APM��������APM�ļ�ͷ */
			if (is_aldus == ALDUSKEY)
			{
				wmf_offset = ALDUSMETAHEADERSIZE;
			}
			
			/* ����Ԫ�ļ����ݣ�����Ԫ�ļ�handle */
			memset(&meta_hdr, 0, sizeof(meta_hdr));
			if (isio_seek(pfile, wmf_offset, SEEK_SET) == -1
				|| isio_read(&meta_hdr, sizeof(meta_hdr), 1, pfile) != 1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			meta_bits = (unsigned char *)malloc(2*meta_hdr.mtSize);
			if (meta_bits == NULL)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, wmf_offset, SEEK_SET) == -1
				|| isio_read(meta_bits, 2, meta_hdr.mtSize, pfile) != meta_hdr.mtSize)
			{
				b_status = ER_FILERWERR; __leave;
			}

			*phout = SetMetaFileBitsEx(2*meta_hdr.mtSize, meta_bits);
			if (*phout == NULL)
			{
				b_status = ER_SYSERR; __leave;
			}

		}
		__finally
		{
			if (meta_bits != NULL)
			{
				free(meta_bits);
			}
			if (b_status != ER_SUCCESS || AbnormalTermination())
			{
				*phout = NULL;
			}

		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ��WMF����ת�����ݰ���ʸ������դ��*/
static int _meta_to_raster(HMETAFILE hwmf, LPINFOSTR pinfo_str)
{
	enum EXERESULT	b_status = ER_SUCCESS;

	unsigned char	*dib_bits = NULL;
	HDC				hdc = NULL, hComDC = NULL;
	HBITMAP			bmp = NULL, oldbmp = NULL;
	BITMAPINFO		bmi;
	RECT			rect;
	int				linesize;
	unsigned		i;
	unsigned char	**ppt;

	assert(hwmf);
	assert(pinfo_str);

	__try
	{
		__try
		{
			/* ����������ļ���DC����Ԫ�ļ����ŵ���DC */
			if ((hdc = GetDC(GetDesktopWindow())) == NULL)
			{
				b_status = ER_SYSERR; __leave;
			}

			memset(&bmi, 0, sizeof(bmi));
			bmi.bmiHeader.biSize          = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biWidth         = pinfo_str->width;
			bmi.bmiHeader.biHeight        = pinfo_str->height;
			bmi.bmiHeader.biPlanes        = 1;
			bmi.bmiHeader.biBitCount      = (WORD)pinfo_str->bitcount;
			bmi.bmiHeader.biCompression   = BI_RGB;
			bmi.bmiHeader.biXPelsPerMeter = GetDeviceCaps(hdc, HORZRES)*1000/GetDeviceCaps(hdc, HORZSIZE);
			bmi.bmiHeader.biYPelsPerMeter = GetDeviceCaps(hdc, VERTRES)*1000/GetDeviceCaps(hdc, VERTSIZE);

			/* ����λͼ��� */
            bmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &dib_bits, NULL, 0);
			if (bmp == NULL)
			{
				b_status = ER_SYSERR; __leave;
			}

			/* ��������DC */
			if ((hComDC = CreateCompatibleDC(hdc)) == NULL)
			{
				b_status = ER_SYSERR; __leave;
			}

			oldbmp = SelectObject(hComDC, bmp);
			
			/* ��ͼ�󱳾����Ϊ��ɫ */
			rect.left = rect.top = 0;
			rect.right = pinfo_str->width;
			rect.bottom = pinfo_str->height;
			FillRect(hComDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

			/* �����DC����WMF���� */
			SetMapMode(hComDC, MM_ANISOTROPIC);
			EnumMetaFile(hComDC, hwmf, _EnumMetaFileProc, (LPARAM)pinfo_str); 
			SelectObject(hComDC, oldbmp);

			/* ���õõ���λͼ����������ݰ������ */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			if ((pinfo_str->p_bit_data = (unsigned char*)malloc(linesize*pinfo_str->height)) == NULL
                || (pinfo_str->pp_line_addr = malloc(sizeof(unsigned char*)*pinfo_str->height)) == NULL)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ���������ݿ��������ݰ� */
			memcpy(pinfo_str->p_bit_data, dib_bits, linesize*pinfo_str->height);

			/* ��д���׵�ַ���� */
			ppt = (unsigned char**)pinfo_str->pp_line_addr;
			for (i = 0; i < pinfo_str->height; i++)
			{
				ppt[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
		}
		__finally
		{
			if (hdc != NULL)
			{
				ReleaseDC(GetDesktopWindow(), hdc);
			}
			if (hComDC != NULL)
			{
				DeleteDC(hComDC);
			}
			if (bmp != NULL)
			{
				DeleteObject(bmp);
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}

	return (int)b_status;
}


/* WMF���Ź��̻ص����� */
static int CALLBACK _EnumMetaFileProc(HDC hDC,  HANDLETABLE *lpHTable,  METARECORD *lpMFR, int nObj, LPARAM lpClientData)
{
	static int viewport_ext_set = 0;
	LPINFOSTR  pinfo_str = (LPINFOSTR)lpClientData; 

	PlayMetaFileRecord(hDC, lpHTable, lpMFR, nObj);

	switch (lpMFR->rdFunction)
	{
	case 0x20e:	/* SetViewportExt() */
		viewport_ext_set = !0;
		break;

	case 0x20c:	/* SetWindowExt() */
		/* ���Ԫ�ļ����趨��������(�߼����꣩��Χʱ����ǰû���޸�
		   �����꣨�豸���꣩��Χ�����Ǿ������趨������ʲôҲ������
		   �Է��޸��ļ����������� */
		if (!viewport_ext_set)
		{
            HDC hdc = GetDC(GetDesktopWindow());
			unsigned int cx, cy;
			float ratio;
            
			switch (pinfo_str->imgformat)
			{
			case IMF_APM:
				cx = pinfo_str->width;
				cy = pinfo_str->height;
				break;

			/* ����Ǳ�׼Ԫ�ļ������������귶Χ��ʹ�豸�����ݺ��
			   ���߼�����һ�£����Ҳ�����ͼ��Ŀ��߳ߴ� */
			case IMF_WMF:
				cx = pinfo_str->width;
				cy = pinfo_str->height;

				/* ! ע����������ݺ�ȣ�y/x)�ķ���: ��Ԫ�ļ���¼�У�������
				   ���Ժ����βα��෴��˳�򴢴�ģ����Ҳ�����hdc����˶���
				   SetWindowExt(),rdParm[0]����Y��Χ��rdParm[1]����X��Χ��*/
				ratio = (float)lpMFR->rdParm[0] / (float)lpMFR->rdParm[1];
				cy = (unsigned int)((float)cx * ratio);
				if (cy > pinfo_str->height)
				{
					cy = pinfo_str->height;
					cx = (unsigned int)((float)cy / ratio);
				}
				break;

			default:
				assert(0);
			}
			SetViewportExtEx(hDC, cx, cy, NULL);
			ReleaseDC(GetDesktopWindow(), hdc);
		}
		break;

	default:
		break;
	}

	return !0;
}
