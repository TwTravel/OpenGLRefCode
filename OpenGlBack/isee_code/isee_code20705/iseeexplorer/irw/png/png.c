/********************************************************************

	png.c

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
	���ļ���;��	ISeeͼ���������PNGͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�1��2��4��8λ�Ҷ�ͼ��8λ��alphaͨ����
								�Ҷ�ͼ��24λRGBͼ��24λ��alphaͨ��
								��RGBͼ��1��2��4��8λ��ɫ��ͼ��
								ע��16λ�Ҷ�ͼ��48λRGBͼ���ڱ�ģ��
									�ڲ���ת����8λ��24λͼ��
									8λ��alphaͨ���ĻҶ�ͼ����ת��Ϊ
									32λRGBAͼ��
									����ת����Ҫ��Ϊ��ʹ������ݷ�
									��ISeeλ����ʽ��
					���湦�ܣ�1��2��4��8λ��ɫ��ͼ��24��32λRGBͼ��
								ע��32λͼЯ��8λalphaͨ����
									���ܱ���Ҷ�ͼ��
	  
	���ļ���д�ˣ�	Aurora		aurorazzf##sina.com
					YZ			yzfree##sina.com ���İ湤����
		
	���ļ��汾��	10809
	����޸��ڣ�	2001-8-9
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-8		�����°����
		2001-5		���¶���ģ��Ľӿڼ��������
		2001-3		������һЩBUG
		2000-7		������һЩBUG
		2000-6		��һ�����԰淢��


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

#include "png.h"
#include "libpng\lpng106\png.h"


IRWP_INFO			png_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	png_get_info_critical;	/* png_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	png_load_img_critical;	/* png_load_image�����Ĺؼ��� */
CRITICAL_SECTION	png_save_img_critical;	/* png_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _png_is_valid_img(LPINFOSTR pinfo_str);

void CALLAGREEMENT png_error_handler(png_structp, png_const_charp);
void CALLAGREEMENT png_isio_read(png_structp, png_bytep, png_size_t);
void CALLAGREEMENT png_isio_write(png_structp, png_bytep, png_size_t);
void CALLAGREEMENT png_isio_flush(png_structp);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&png_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&png_get_info_critical);
			InitializeCriticalSection(&png_load_img_critical);
			InitializeCriticalSection(&png_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&png_get_info_critical);
			DeleteCriticalSection(&png_load_img_critical);
			DeleteCriticalSection(&png_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PNG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&png_irwp_info;
}

#else

PNG_API LPIRWP_INFO CALLAGREEMENT png_get_plugin_info()
{
	_init_irwp_info(&png_irwp_info);

	return (LPIRWP_INFO)&png_irwp_info;
}

PNG_API void CALLAGREEMENT png_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PNG_API void CALLAGREEMENT png_detach_plugin()
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
	lpirwp_info->irwp_save.bitcount = \
		1UL|(1UL<<(2-1))|(1UL<<(4-1))|(1UL<<(8-1))|(1UL<<(24-1))|(1UL<<(32-1));
	lpirwp_info->irwp_save.img_num = 1;	/* 1��ֻ�ܱ���һ��ͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 2;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"Aurora");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"aurorazzf##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"��һ��");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
		(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
		(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
		(const char *)"�����ǵڶ�����:)");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}


/* ��ȡͼ����Ϣ */
PNG_API int CALLAGREEMENT png_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	png_structp		p_png = 0;
	png_infop		p_png_info = 0;
	PNGINFOHEADER	png_info_header;
	int				channels;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	
	
	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&png_get_info_critical);
			
			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ����PNG�ṹ������ʼ�� */
			if ((p_png=png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)pfile, png_error_handler, 0)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ����PNG��Ϣͷ�ṹ */
			if ((p_png_info=png_create_info_struct(p_png)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			if (setjmp(png_jmpbuf(p_png)))
			{
				/* PNG�����󽫷��ص����� */
				b_status = ER_FILERWERR; __leave;
			}

			/* �����Զ�������� */
			png_set_read_fn(p_png, (PVOID)pfile, png_isio_read);
			
			/* ���ļ���Ϣͷ��ֱ��IDAT��*/
			png_read_info(p_png, p_png_info);
			
			/* ��ȡͼ����Ϣ */
			png_get_IHDR(p_png, p_png_info, &png_info_header.Width, 
				&png_info_header.Height, &png_info_header.bit_depth, 
				&png_info_header.color_type, &png_info_header.interlace_type,
				&png_info_header.compression_type, &png_info_header.filter_type);
			

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_PNG;			/* ͼ���ļ���ʽ����׺���� */

			if (png_info_header.color_type == PNG_COLOR_TYPE_PALETTE)
			{
				pinfo_str->compression = ICS_PNG_PALETTE; channels = 1;
			}
			else if (png_info_header.color_type & PNG_COLOR_MASK_COLOR)
			{
				pinfo_str->compression = ICS_PNG_RGB; channels = 3;
			}
			else
			{
				pinfo_str->compression = ICS_PNG_GRAYSCALE; channels = 1;
			}

			/* ���ͼ�����alphaͨ�����򽫻Ҷ�ͼȫ��ת��Ϊ24λRGBͼ�� */
			if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
			{
				channels = 4;
				if (pinfo_str->compression == ICS_PNG_GRAYSCALE)
				{
					png_info_header.bit_depth = 8;
					pinfo_str->compression = ICS_PNG_RGB;
				}
			}

			/* ��PNGͼ�����صĲ�����λ�����8λʱ�������������Ϊ8λ
				����ISee�Ĳ�������ʽ�涨ÿ�������Ϊ32λλ���
			*/
			if (png_info_header.bit_depth > 8)
				png_info_header.bit_depth = 8;

			/* ���λ��ֵ���ܻ���ʵ��λ�ͬ���������ת����*/
			png_info_header.bit_depth *= channels;


			pinfo_str->width	= (unsigned long)png_info_header.Width;
			pinfo_str->height	= (unsigned long)png_info_header.Height;
			pinfo_str->order	= 0;		/* PNGȫ��Ϊ�Զ����µ�ɨ����˳�� */
			pinfo_str->bitcount	= png_info_header.bit_depth;
			
			switch (pinfo_str->compression)
			{
			case	ICS_PNG_PALETTE:
			case	ICS_PNG_GRAYSCALE:		/* �Ҷ�ͼ������Ϊ����ͼ�Դ� */
				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				break;
			case	ICS_PNG_RGB:			/* RGBͼ��ֻ��24λһ�֣���48λͼ���ѱ�����Ϊ24λ��*/
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				break;
			default:
				assert(0); 
				break;
			}

			/* ����alpha�������� */
			if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
				pinfo_str->a_mask = ((1UL << (png_info_header.bit_depth / channels)) - 1)<<(32 - (png_info_header.bit_depth / channels));
			else
				pinfo_str->a_mask = 0x0;


			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if(p_png)
				png_destroy_read_struct(&p_png, &p_png_info, (png_infopp)0);
			if (pfile)
				isio_close(pfile);
		
			LeaveCriticalSection(&png_get_info_critical);
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
PNG_API int CALLAGREEMENT png_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	png_structp		p_png = 0;
	png_infop		p_png_info = 0;
	PNGINFOHEADER	png_info_header;
	int				channels, i, linesize, intent, pass, row;
	double			screen_gamma, image_gamma;
	char			*gamma_str;

	unsigned char	buf, tmp, *pbit;
	unsigned short	us;
	int				bit, mask;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	
	
	__try
	{
		__try
		{
			EnterCriticalSection(&png_load_img_critical);
			
			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ����PNG�ṹ������ʼ�� */
			if ((p_png=png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)pfile, png_error_handler, 0)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ����PNG��Ϣͷ�ṹ */
			if ((p_png_info=png_create_info_struct(p_png)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			if (setjmp(png_jmpbuf(p_png)))
			{
				/* PNG�����󽫷��ص����� */
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �����Զ�������� */
			png_set_read_fn(p_png, (PVOID)pfile, png_isio_read);
			
			/* ���ļ���Ϣͷ��ֱ��IDAT��*/
			png_read_info(p_png, p_png_info);
			
			/* ��ȡͼ����Ϣ */
			png_get_IHDR(p_png, p_png_info, &png_info_header.Width, 
				&png_info_header.Height, &png_info_header.bit_depth, 
				&png_info_header.color_type, &png_info_header.interlace_type,
				&png_info_header.compression_type, &png_info_header.filter_type);
			

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_PNG;			/* ͼ���ļ���ʽ����׺���� */

				if (png_info_header.color_type == PNG_COLOR_TYPE_PALETTE)
				{
					pinfo_str->compression = ICS_PNG_PALETTE; channels = 1;
				}
				else if (png_info_header.color_type & PNG_COLOR_MASK_COLOR)
				{
					pinfo_str->compression = ICS_PNG_RGB; channels = 3;
				}
				else
				{
					pinfo_str->compression = ICS_PNG_GRAYSCALE; channels = 1;
				}

				/* ���ͼ�����alphaͨ�����򽫻Ҷ�ͼȫ��ת��Ϊ24λRGBͼ�� */
				if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
				{
					channels = 4;
					if (pinfo_str->compression == ICS_PNG_GRAYSCALE)
					{
						png_info_header.bit_depth = 8;
						pinfo_str->compression = ICS_PNG_RGB;
					}
				}
				
				/* ��PNGͼ�����صĲ�����λ�����8λʱ�������������Ϊ8λ
					����ISee�Ĳ�������ʽ�涨ÿ�������Ϊ32λλ���
				*/
				if (png_info_header.bit_depth > 8)
					png_info_header.bit_depth = 8;

				/* ���λ��ֵ���ܻ���ʵ��λ�ͬ���������ת����*/
				png_info_header.bit_depth *= channels;


				pinfo_str->width	= (unsigned long)png_info_header.Width;
				pinfo_str->height	= (unsigned long)png_info_header.Height;
				pinfo_str->order	= 0;		/* PNGȫ��Ϊ�Զ����µ�ɨ����˳�� */
				pinfo_str->bitcount	= png_info_header.bit_depth;
				
				switch (pinfo_str->compression)
				{
				case	ICS_PNG_PALETTE:
				case	ICS_PNG_GRAYSCALE:		/* �Ҷ�ͼ������Ϊ����ͼ�Դ� */
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					break;
				case	ICS_PNG_RGB:			/* RGBͼ��ֻ��24λһ�֣���48λͼ���ѱ�����Ϊ24λ��*/
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					break;
				default:
					assert(0); 
					break;
				}

				/* ����alpha�������� */
				if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
					pinfo_str->a_mask = ((1UL << (png_info_header.bit_depth / channels)) - 1)<<(32 - (png_info_header.bit_depth / channels));
				else
					pinfo_str->a_mask = 0x0;
			}


			/* ��ʼ����ɫ������ */
			if (pinfo_str->compression == ICS_PNG_PALETTE)
			{
				pinfo_str->pal_count = p_png_info->num_palette;

				if (pinfo_str->pal_count > 256)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* ���Ƶ�ɫ������ */
				for (i=0;i<(int)pinfo_str->pal_count;i++)
				{
					((LPPALITEM)(&pinfo_str->palette[i]))->blue  = p_png_info->palette[i].blue;
					((LPPALITEM)(&pinfo_str->palette[i]))->green = p_png_info->palette[i].green;
					((LPPALITEM)(&pinfo_str->palette[i]))->red   = p_png_info->palette[i].red;
					((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
				}
			}
			else if (pinfo_str->compression == ICS_PNG_GRAYSCALE)
			{
				assert(pinfo_str->bitcount <= 8);

				pinfo_str->pal_count = 1 << pinfo_str->bitcount;
				
				/* ģ���ɫ�� */
				switch (pinfo_str->bitcount)
				{
				case	1:
					pinfo_str->palette[0] = 0x0;
					pinfo_str->palette[1] = 0xffffff;
					break;
				case	2:
					pinfo_str->palette[0] = 0x0;
					pinfo_str->palette[1] = 0x3f3f3f;
					pinfo_str->palette[2] = 0x7f7f7f;
					pinfo_str->palette[3] = 0xffffff;
					break;
				case	4:
					pinfo_str->palette[0] = 0x0;
					pinfo_str->palette[1] = 0x1f1f1f;
					pinfo_str->palette[2] = 0x2f2f2f;
					pinfo_str->palette[3] = 0x3f3f3f;
					pinfo_str->palette[4] = 0x4f4f4f;
					pinfo_str->palette[5] = 0x5f5f5f;
					pinfo_str->palette[6] = 0x6f6f6f;
					pinfo_str->palette[7] = 0x7f7f7f;
					pinfo_str->palette[8] = 0x8f8f8f;
					pinfo_str->palette[9] = 0x9f9f9f;
					pinfo_str->palette[10] = 0xafafaf;
					pinfo_str->palette[11] = 0xbfbfbf;
					pinfo_str->palette[12] = 0xcfcfcf;
					pinfo_str->palette[13] = 0xdfdfdf;
					pinfo_str->palette[14] = 0xefefef;
					pinfo_str->palette[15] = 0xffffff;
					break;
				case	8:
					for (i=0;i<(int)pinfo_str->pal_count;i++)
						pinfo_str->palette[i] = ((unsigned long)i<<16)|(i<<8)|i;
					break;
				default:
					assert(0);
					break;
				}
			}
			else
				pinfo_str->pal_count = 0;


			/* ȡ��ɨ���гߴ� */
			linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);
			
			/* ����ͼ����� */
			pinfo_str->imgnumbers = 1;		/* ����Ϊ��̬ͼ�� */
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

			/* ��ʼ�����׵�ַ���� */
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			
			/* ��tRNSͨ��ת��Ϊalphaͨ�� */
			if (png_get_valid(p_png, p_png_info,PNG_INFO_tRNS)) 
				png_set_tRNS_to_alpha(p_png);
			
			/* ����alphaͨ���ĻҶ�ͼת��ΪRGBAͼ */
			if (p_png_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(p_png);

			/* ��ͼ��������RGBת��ΪBGR��ISeeλ����ʽ�� */
			png_set_bgr(p_png);
			
			/* ֪ͨPNGLIB��16λͼ������Ϊ8λ */
			png_set_strip_16(p_png);

			

			/* ������Ļ��ʾ��GAMMAֵ
			*  screen gamma��CRT_gamma�ͻ�������gamma��� 
			*/

			/* ��ʱ��ʵ���û�ָ����Ļgamma */
			if ((gamma_str = getenv("SCREEN_GAMMA")) != 0)
				screen_gamma = atof(gamma_str);
			else
				screen_gamma = 2.2;

			/* ��LIBPNG����gamma���� 
			*  �˴�Ӧ֧���û�����ͼ��gammaֵ 
			*/
			if (png_get_sRGB(p_png, p_png_info, &intent))
				png_set_gamma(p_png, screen_gamma, 0.45455);/* ��ͼ��gamma�Ĳ²� */
			else
			{
				if (png_get_gAMA(p_png, p_png_info, &image_gamma))
					png_set_gamma(p_png, screen_gamma, image_gamma);
				else
					png_set_gamma(p_png, screen_gamma, 0.45455);
			}

			/* ��INTERLACE������(ʹ��png_read_row��ͼ��ʱ����) */
			png_info_header.number_passes = png_set_interlace_handling(p_png);

			/* ���������任��������룬������Ϣ�ṹ */
			png_read_update_info(p_png, p_png_info);
			
			/* ��ʼ����ȡ���ȱ�־ */
			pinfo_str->process_total   = png_info_header.number_passes*pinfo_str->height;
			pinfo_str->process_current = 0;

			/* ׼����ȡͼ������ */
			png_start_read_image(p_png);



			/* ���ж�ȡͼ������ */
			for(pass = 0; pass < png_info_header.number_passes; pass++)
			{
				/* ÿ�ζ�ȡһ��ͼ�� */
				for (row = 0; row < (int)pinfo_str->height; row++)
				{
					/* ���� .....  :) */
					png_read_rows(p_png, (unsigned char **)&pinfo_str->pp_line_addr[row], 0, 1);

					/* �����1λλͼ����ߵ��ֽ����������� */
					if (pinfo_str->bitcount == 1)
					{
						pbit = (unsigned char *) pinfo_str->pp_line_addr[row];

						for (i=0;i<linesize;i++)
						{
							buf = *(unsigned char *)(pbit+i);
							us  = ((unsigned short)buf)<<8;
							tmp = 0;
							for (bit=1, mask=7;bit<16;bit+=2, mask--)
								tmp |= (us>>bit)&(1<<mask);
							
							*(unsigned char *)(pbit+i) = tmp;
						}
					}

					/* ���� */
					pinfo_str->process_current++;

					/* ֧���û��ж� */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
			}
			

			/* ��ȡͼ�󸽼���Ϣ������zTxt��sTxt��tIMEͨ������Ϣ
			*  ����Щ��Ϣ�ݲ������Ժ���Լӵ�pinfo_str�д��ݸ��ͻ�ģ��
			*/
			png_read_end(p_png, p_png_info);
			

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
			
			if(p_png)
				png_destroy_read_struct(&p_png, &p_png_info, (png_infopp)0);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&png_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}



/* ����ͼ�� */
PNG_API int CALLAGREEMENT png_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	png_structp		p_png = 0;
	png_infop		p_png_info = 0;
	png_colorp		p_palette = 0;
	unsigned int	color_type, bit_depth, i;
	unsigned long	number_passes, pass, row, height;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&png_save_img_critical);
			
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_png_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}
			
			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			

			/* ����PNG�ṹ������ʼ�� */
			if ((p_png=png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)pfile, png_error_handler, 0)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ����PNG��Ϣͷ�ṹ */
			if ((p_png_info=png_create_info_struct(p_png)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* �����ɫ����ͼ��������ڴ�� */
			if ((p_palette=(png_colorp)malloc(sizeof(png_color)*256)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* �����Զ���д���� */
			png_set_write_fn(p_png, (PVOID)pfile, png_isio_write, (png_flush_ptr)png_isio_flush);
			
			if (setjmp(png_jmpbuf(p_png)))
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ȷ��PNGɫ��ģʽ */
			if (pinfo_str->bitcount <= 8)		/* ��ɫ��ͼ�������ܱ���Ҷ�ͼ��*/
			{
				assert(pinfo_str->pal_count > 0);

				color_type = PNG_COLOR_TYPE_PALETTE;
				bit_depth  = pinfo_str->bitcount;

				for (i=0;i<pinfo_str->pal_count;i++)
				{
					p_palette[i].red   = (png_byte)((pinfo_str->palette[i] & 0xff0000)>>16);
					p_palette[i].green = (png_byte)((pinfo_str->palette[i] & 0xff00)>>8);
					p_palette[i].blue  = (png_byte)(pinfo_str->palette[i] & 0xff);
				}
			}
			else								/* RGBͼ����������alphaͨ����*/
			{
				assert(pinfo_str->pal_count == 0);

				bit_depth  = 8;

				if (pinfo_str->a_mask)
					color_type = PNG_COLOR_TYPE_RGB_ALPHA;
				else
					color_type = PNG_COLOR_TYPE_RGB;
			}
				
			/* ����PNG�ļ���Ϣͷ */
			png_set_IHDR(p_png, p_png_info, 
				pinfo_str->width, pinfo_str->height, 
				bit_depth, 
				color_type,
				PNG_INTERLACE_ADAM7, 
				PNG_COMPRESSION_TYPE_DEFAULT, 
				PNG_FILTER_TYPE_DEFAULT);
			
			/* ���õ�ɫ������ */
			if (color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_PLTE(p_png, p_png_info, p_palette, pinfo_str->pal_count);
			else
				png_set_PLTE(p_png, p_png_info, (png_colorp)0, 0);


			/* ����Ϣд����Ϣͷ */
			png_write_info(p_png, p_png_info);

			/* �����ý����ʽ��interlaced���洢 */
			number_passes = png_set_interlace_handling(p_png);

			height = pinfo_str->height;

			/* ��ʼ����ȡ���ȱ�־ */
			pinfo_str->process_total   = number_passes*height;
			pinfo_str->process_current = 0;
			
			for (pass=0;pass<number_passes;pass++)
			{ 
				/* һ��ֻд��һ�� */
				for (row=0;row<height;row++)
				{ 
					/* дѽ.......:) */
					png_write_rows(p_png, &((unsigned char *)(pinfo_str->pp_line_addr[row])), 1);

					/* ���� */
					pinfo_str->process_current++;
					
					/* ֧���û��ж� */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				} 
			} 
			
			/* �������д��tEXt, zTXt, and tIME.......... */
			
			/* д��IENDͨ�������� */
			png_write_end(p_png, p_png_info);

			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (p_palette)
				free(p_palette);

			if(p_png)
			{
				if (p_png_info)
					png_destroy_write_struct(&p_png, &p_png_info);
				else
					png_destroy_write_struct(&p_png, (png_infopp)0);
			}
			
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&png_save_img_critical);
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


/* �жϴ����ͼ���Ƿ���Ա����� */
int CALLAGREEMENT _png_is_valid_img(LPINFOSTR pinfo_str)
{
	if (!(png_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	
	assert(pinfo_str->imgnumbers);
	
	if (png_irwp_info.irwp_save.img_num)
		if (png_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */
			
	return 0;
}



/* PNG���Զ���������� */
void CALLAGREEMENT png_error_handler(png_structp png_ptr, png_const_charp message)
{
#ifdef PNG_SETJMP_SUPPORTED

#	ifdef USE_FAR_KEYWORD
	{
		jmp_buf jmpbuf;
		png_memcpy(jmpbuf,png_ptr->jmpbuf,sizeof(jmp_buf));
		longjmp(jmpbuf, 1);
	}
#	else
	longjmp(png_ptr->jmpbuf, 1);
#	endif

#else
	if (png_ptr == 0)
		PNG_ABORT();
#endif
}


/* PNG���Զ��������
 * ������
 *		data �� �������ݻ�����
 *		length �� �����ݳ���
 */
void CALLAGREEMENT png_isio_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
   ISFILE	*file;

   assert(png_ptr);
   assert(png_ptr->io_ptr);

   file = (ISFILE*)CVT_PTR(png_ptr->io_ptr);

   if (isio_read((void*)data, length, 1, file) == 0)
   {
	   png_error(png_ptr, "reading png image error");
   }
}


/* PNG���Զ���д����
 * ������
 *		data �� ���ݻ�����
 *		length �� ���ݳ���
 */
void CALLAGREEMENT png_isio_write(png_structp png_ptr, png_bytep data, png_size_t length)
{
	ISFILE	*file;
	
	assert(png_ptr);
	assert(png_ptr->io_ptr);
	
	file = (ISFILE*)CVT_PTR(png_ptr->io_ptr);
	
	if (isio_write((void*)data, length, 1, file) == 0)
	{
		png_error(png_ptr, "write png image error");
	}
}


/* PNG���Զ���FLUSH���� */
void CALLAGREEMENT png_isio_flush(png_structp png_ptr)
{
	ISFILE	*file;
	
	assert(png_ptr);
	assert(png_ptr->io_ptr);
	
	file = (ISFILE*)CVT_PTR(png_ptr->io_ptr);
	
	if (isio_flush(file)) 
	{
		png_error(png_ptr, "fulsh file error");
	}
}

