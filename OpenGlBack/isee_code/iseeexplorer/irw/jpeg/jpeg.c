/********************************************************************

	jpeg.c

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
	���ļ���;��	ISeeͼ���������JPEGͼ���дģ��ʵ���ļ�
	
					��ȡ���ܣ�8λ�Ҷ�ͼ��24λ���ͼ��
					���湦�ܣ�8λ�Ҷ�ͼ��24λ���ͼ��
							  �����趨Ʒ��ϵ����
	  
	���ļ���д�ˣ�	orbit		Inte2000##263.net
					YZ			yzfree##sina.com
		
	���ļ��汾��	10715
	����޸��ڣ�	2001-7-15
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-7		�����°����
		2000-12		�Ľ��˽��ȿ��ƴ���
		2000-9		��һ�����԰淢��


	���ô���������
	---------------------------------------------------------------
	�����ʹ����IJG��jpeglib6b�����⣬��л���������Ĺ�����

	"this software is based in part on the work of the Independent 
	JPEG Group"
  
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
#include <setjmp.h>
#include <assert.h>

#include "jpeglib.h"
#include "jpeg.h"


IRWP_INFO			jpeg_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	jpeg_get_info_critical;	/* jpeg_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	jpeg_load_img_critical;	/* jpeg_load_image�����Ĺؼ��� */
CRITICAL_SECTION	jpeg_save_img_critical;	/* jpeg_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
METHODDEF(void)  CALLAGREEMENT	my_error_exit (j_common_ptr cinfo);
enum IMGCOMPRESS CALLAGREEMENT 	compress_type(J_COLOR_SPACE jcs);
void			 CALLAGREEMENT	_init_irwp_info(LPIRWP_INFO lpirwp_info);
int				 CALLAGREEMENT _jpeg_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&jpeg_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&jpeg_get_info_critical);
			InitializeCriticalSection(&jpeg_load_img_critical);
			InitializeCriticalSection(&jpeg_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&jpeg_get_info_critical);
			DeleteCriticalSection(&jpeg_load_img_critical);
			DeleteCriticalSection(&jpeg_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

JPEG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&jpeg_irwp_info;
}

#else

JPEG_API LPIRWP_INFO CALLAGREEMENT jpeg_get_plugin_info()
{
	_init_irwp_info(&jpeg_irwp_info);

	return (LPIRWP_INFO)&jpeg_irwp_info;
}

JPEG_API void CALLAGREEMENT jpeg_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

JPEG_API void CALLAGREEMENT jpeg_detach_plugin()
{
	/* ���ٶ��߳�ͬ������ */
}

#endif /* WIN32 */


/* ��ʼ�������Ϣ�ṹ */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info)
{
	int		i, j;

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
	/* ## 2001-8-9  YZ �޸ģ���λ����һ������32λ��ʽ�޷���ʾ��*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(8-1)) | (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;			/* 1��ֻ�ܱ���һ��ͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 1;
	lpirwp_info->irwp_save.para[0].count = 15;	/* ����15��Ʒ��ϵ�������ȷֲ���1��100��*/
	strcpy((char*)lpirwp_info->irwp_save.para[0].desc, (const char *)"Ʒ��ϵ��");
	j = lpirwp_info->irwp_save.para[0].count;
	/* ################################################################# */
	/* ## 2002-6-29  yygw �޸ģ�ԭ����������㲻��ȷ��                   */
	for (i=0;i<j;i++)
	{
		// lpirwp_info->irwp_save.para[0].value[i] = i*(100/j)+1;
        lpirwp_info->irwp_save.para[0].value[i] = i*99/(j-1)+1;
		sprintf((char*)lpirwp_info->irwp_save.para[0].value_desc[i], (const char *)"%d%%", lpirwp_info->irwp_save.para[0].value[i]);
	}
	// i--;
	// lpirwp_info->irwp_save.para[0].value[i] = 100;/* ���Ʒ�� */
	// sprintf((char*)lpirwp_info->irwp_save.para[0].value_desc[i], (const char *)"%d", lpirwp_info->irwp_save.para[0].value[i]);
	/* ################################################################# */


	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 2;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
		(const char *)"orbit");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
		(const char *)"Inte2000##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
		(const char *)"�������˵ģ�");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"�޹���ѽ���޹�������ѽ��ѽ�ϣ�");
	/* ---------------------------------[2] �� ������ -------------- */
	/* ������������Ϣ�ɼ��ڴ˴���*/
	/* ------------------------------------------------------------- */


	/* ���������Ϣ����չ����Ϣ��*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* ����������##���ֶ������� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
				(const char *)"JPEG");

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
JPEG_API int CALLAGREEMENT jpeg_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned short	fmark;

	struct jpeg_decompress_struct	cinfo;
	struct my_error_mgr				jerr;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&jpeg_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ��ȡ�ļ���־ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&fmark, sizeof(unsigned short), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж��Ƿ�����Ч���ļ� */
			if (fmark != JPG_SOI_MARK)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			cinfo.err = jpeg_std_error(&jerr.pub);
			jerr.pub.error_exit = my_error_exit;
	
			/* ���ô������ */
			if (setjmp(jerr.setjmp_buffer)) 
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* ����������� */
			jpeg_create_decompress(&cinfo);
			/* ָ��Դ������ */
			jpeg_stdio_src(&cinfo, (FILE*)pfile);
			/* ����ͼ����Ϣ */
			jpeg_read_header(&cinfo, 1);
			/* ���ٽ������ */			
			jpeg_destroy_decompress(&cinfo);
			
			
			/* ������Ч�Լ��� */
			if ((cinfo.image_width == 0)||(cinfo.image_height == 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_JPEG;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression	= compress_type(cinfo.jpeg_color_space);
														/* ͼ���ѹ����ʽ */
			if (pinfo_str->compression == ICS_UNKONW)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			/* ��дͼ����Ϣ */
			pinfo_str->width	= (unsigned long)cinfo.image_width;
			pinfo_str->height	= (unsigned long)cinfo.image_height;
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= (cinfo.num_components == 3) ? 24UL:8UL;
			
			if (pinfo_str->bitcount == 24)
			{
				if (pinfo_str->compression == ICS_JPEGYCbCr)
				{
					/* ע������ͼ��RGB˳������jmorecfg.h�ļ����޸ģ���BMP��ͬ */
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0;
				}
				else
				{
					/* ע��JPEG��RGB����˳����BMPͼ���෴ */
					pinfo_str->b_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff;
					pinfo_str->a_mask = 0;
				}
			}
			else
			{
				pinfo_str->b_mask = pinfo_str->g_mask = pinfo_str->r_mask = pinfo_str->a_mask = 0;
			}
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&jpeg_get_info_critical);
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
JPEG_API int CALLAGREEMENT jpeg_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned short	fmark;

	struct jpeg_decompress_struct	cinfo;
	struct my_error_mgr				jerr;
	unsigned char	*pdata;
	JSAMPARRAY		buffer;	
	int				row_stride, i, linesize;		
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&jpeg_load_img_critical);

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

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ж��Ƿ��ѻ�ȡ��ͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				if (isio_read((void*)&fmark, sizeof(unsigned short), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* �ж��Ƿ�����Ч���ļ� */
				if (fmark != JPG_SOI_MARK)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				if (isio_seek(pfile, 0, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			cinfo.err = jpeg_std_error(&jerr.pub);
			jerr.pub.error_exit = my_error_exit;

			/* ���ô��󷵻ص�ַ */
			if (setjmp(jerr.setjmp_buffer)) 
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* ����������� */
			jpeg_create_decompress(&cinfo);
			/* ָ��Դ������ */
			jpeg_stdio_src(&cinfo, (FILE*)pfile);
			/* ����ͼ����Ϣ */
			jpeg_read_header(&cinfo, 1);

			/* ���û����ȡ��ͼ����Ϣ������ȡ֮ */
			if (pinfo_str->data_state == 0)
			{
				/* ������Ч�Լ��� */
				if ((cinfo.image_width == 0)||(cinfo.image_height == 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_JPEG;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression	= compress_type(cinfo.jpeg_color_space);
				/* ͼ���ѹ����ʽ */
				if (pinfo_str->compression == ICS_UNKONW)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				/* ��дͼ����Ϣ */
				pinfo_str->width	= (unsigned long)cinfo.image_width;
				pinfo_str->height	= (unsigned long)cinfo.image_height;
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= (cinfo.num_components == 3) ? 24UL:8UL;
				
				if (pinfo_str->bitcount == 24)
				{
					if (pinfo_str->compression == ICS_JPEGYCbCr)
					{
						/* ע������ͼ��RGB˳������jmorecfg.h�ļ����޸ģ���BMP��ͬ */
						pinfo_str->r_mask = 0xff0000;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->b_mask = 0xff;
						pinfo_str->a_mask = 0;
					}
					else
					{
						pinfo_str->b_mask = 0xff0000;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->r_mask = 0xff;
						pinfo_str->a_mask = 0;
					}
				}
				else
				{
					pinfo_str->b_mask = pinfo_str->g_mask = pinfo_str->r_mask = pinfo_str->a_mask = 0;
				}
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}
			
			/* ���õ�ɫ������ */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

				for (i=0;i<256;i++)
					pinfo_str->palette[i] = (unsigned long)((i<<16)|(i<<8)|i);
			}
			else
				pinfo_str->pal_count = 0;
			
			/* ����ɨ���гߴ�(���ֽڶ���) */
			linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);

			/* ����ͼ����� */
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;
			
			assert(pinfo_str->p_bit_data == (unsigned char *)0);
			/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
			pinfo_str->p_bit_data = (unsigned char *)malloc(linesize * pinfo_str->height + 4);
			if (!pinfo_str->p_bit_data)
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_MEMORYERR; __leave;
			}
			
			assert(pinfo_str->pp_line_addr == (void**)0);
			/* �������׵�ַ���� */
			pinfo_str->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo_str->height);
			if (!pinfo_str->pp_line_addr)
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_MEMORYERR; __leave;
			}

			/* ��ʼ�����׵�ַ���飨����*/
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			

			/* ��ʼ���� */
			jpeg_start_decompress(&cinfo);
			
			/* ����ɨ���л��������� */
			row_stride = cinfo.output_width * cinfo.output_components;
			
			/* ������ָ�뻺���� */
			buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

			/* ���ȱ�����ʼ�� */
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			
			/* ������������:) */
			while (cinfo.output_scanline < cinfo.output_height) 
			{
				jpeg_read_scanlines(&cinfo, buffer, 1);
				pdata = (unsigned char *)(buffer[0]);

				memmove((void*)pinfo_str->pp_line_addr[cinfo.output_scanline-1], (const void*)pdata, cinfo.output_width*((cinfo.out_color_components == 3) ? 3:1));

				pinfo_str->process_current = cinfo.output_scanline;

				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					jpeg_destroy_decompress(&cinfo);
					b_status = ER_USERBREAK; __leave;
				}
			}
			
			/* ��ɽ��� */
			jpeg_finish_decompress(&cinfo);
			/* ���ٽ������ */			
			jpeg_destroy_decompress(&cinfo);
			
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

			LeaveCriticalSection(&jpeg_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� ��ֻ�ܱ���8��24λͼ��*/
JPEG_API int CALLAGREEMENT jpeg_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr			jerr;
	unsigned char				*lp;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&jpeg_save_img_critical);
	
			/* �жϴ����ͼ���Ƿ�֧�� */
			if (_jpeg_is_valid_img(pinfo_str, lpsave))
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ���ý���ֵ */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			

			/* ���ô��������� */
			cinfo.err = jpeg_std_error(&jerr.pub);
			jerr.pub.error_exit = my_error_exit;
			if (setjmp(jerr.setjmp_buffer)) 
			{
				jpeg_destroy_compress(&cinfo);
				b_status = ER_FILERWERR; __leave;
			}

			/* ����ѹ�����󼰲��� */
			jpeg_create_compress(&cinfo);
			jpeg_stdio_dest(&cinfo, (FILE*)pfile);

			cinfo.image_width  = pinfo_str->width;
			cinfo.image_height = pinfo_str->height;

			/* �������ظ�ʽ��Ϣ */
			if (pinfo_str->bitcount == 24)
			{
				cinfo.input_components = 3;		
				cinfo.in_color_space = JCS_RGB; 	
			}
			else if (pinfo_str->bitcount == 8)
			{
				cinfo.input_components = 1;		
				cinfo.in_color_space = JCS_GRAYSCALE; 	
			}
			else
				assert(0);

			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, lpsave->para_value[0], 1);		/* ����Ʒ��ϵ�� */
			jpeg_start_compress(&cinfo, 1);
			
			/* д��Ŀ���� */
			while (cinfo.next_scanline < cinfo.image_height) 
			{
				lp = (unsigned char *)pinfo_str->pp_line_addr[cinfo.next_scanline];
				jpeg_write_scanlines(&cinfo, &lp, 1);

				pinfo_str->process_current = cinfo.next_scanline;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					jpeg_destroy_compress(&cinfo);
					b_status = ER_USERBREAK; __leave;
				}
			}				

			/* ���ѹ�� */
			jpeg_finish_compress(&cinfo);
			jpeg_destroy_compress(&cinfo);
			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&jpeg_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ���������� */
METHODDEF(void) CALLAGREEMENT my_error_exit (j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	
	char buffer[JMSG_LENGTH_MAX];
	
	(*cinfo->err->format_message) (cinfo, buffer);
	
	longjmp(myerr->setjmp_buffer, 1);
}


/* ����ͼ��ɫ��ϵͳ */
enum IMGCOMPRESS CALLAGREEMENT compress_type(J_COLOR_SPACE jcs)
{
	enum IMGCOMPRESS rtn = ICS_UNKONW;

	switch(jcs)
	{
	case JCS_UNKNOWN:
		rtn = ICS_UNKONW;
		break;
	case JCS_GRAYSCALE:	
		rtn = ICS_JPEGGRAYSCALE;
		break;
	case JCS_RGB:
		rtn = ICS_JPEGRGB;
		break;		
	case JCS_YCbCr:
		rtn = ICS_JPEGYCbCr;
		break;		
	case JCS_CMYK:
		rtn = ICS_JPEGCMYK;
		break;		
	case JCS_YCCK:
		rtn = ICS_JPEGYCCK;
		break;
	default:
		rtn = ICS_UNKONW;
		break;
	}
	return rtn;
}


/* �жϴ����ͼ���Ƿ���Ա����� */
int CALLAGREEMENT _jpeg_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	int i;

	/* ################################################################# */
	/* ## 2001-8-9  YZ �޸ģ�λ����һ������32λ��ʽ�޷���ʾ��*/
	if (!(jpeg_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */
	
	assert(pinfo_str->imgnumbers);
	
	if (jpeg_irwp_info.irwp_save.img_num != (int)pinfo_str->imgnumbers)
		return -2;			/* ͼ���������ȷ */

	for (i=0;i<jpeg_irwp_info.irwp_save.para[0].count;i++)
		if (jpeg_irwp_info.irwp_save.para[0].value[i] == lpsave->para_value[0])
			break;

	return (i==jpeg_irwp_info.irwp_save.para[0].count) ? -3:0;		/* -3:��Ч��Ʒ��ϵ�� */
}

