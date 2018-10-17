/********************************************************************

	sgi.c

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
	���ļ���;��	ISeeͼ���������SGIͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�8��24��32λSGIͼ�󣨰���ѹ����δѹ����
							  
					���湦�ܣ�24λ��ѹ����ʽ
							   

	���ļ���д�ˣ�	YZ			yzfree##yeah.net

	���ļ��汾��	20428
	����޸��ڣ�	2002-4-28

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-4		������һ���°汾

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

#include "sgi.h"


IRWP_INFO			sgi_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	sgi_get_info_critical;	/* sgi_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	sgi_load_img_critical;	/* sgi_load_image�����Ĺؼ��� */
CRITICAL_SECTION	sgi_save_img_critical;	/* sgi_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif

/* SGI��ͷ�ṹ */
static SGIHEADER	sgi_header;

/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _sgi_is_valid_img(LPINFOSTR pinfo_str);

static enum EXERESULT CALLAGREEMENT _read_uncom(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi);
static enum EXERESULT CALLAGREEMENT _read_rle8(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi);
static void _to_rgb(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *p, int width);
static void _to_rgba(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *pa, unsigned char *p, int width);
static int _read_long_tab(ISFILE *pfile, unsigned long *pl, int count);
static int _uncompression_rle8(unsigned char *prle, int rlesize, unsigned char *p, int bufsize);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&sgi_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&sgi_get_info_critical);
			InitializeCriticalSection(&sgi_load_img_critical);
			InitializeCriticalSection(&sgi_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&sgi_get_info_critical);
			DeleteCriticalSection(&sgi_load_img_critical);
			DeleteCriticalSection(&sgi_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

SGI_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&sgi_irwp_info;
}

#else

SGI_API LPIRWP_INFO CALLAGREEMENT sgi_get_plugin_info()
{
	_init_irwp_info(&sgi_irwp_info);

	return (LPIRWP_INFO)&sgi_irwp_info;
}

SGI_API void CALLAGREEMENT sgi_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

SGI_API void CALLAGREEMENT sgi_detach_plugin()
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


	/* ���ܱ�ʶ ��##���ֶ��������粻֧�ֱ��湦���뽫IRWP_WRITE_SUPP��ʶȥ���� */
	lpirwp_info->irwp_function = IRWP_READ_SUPP | IRWP_WRITE_SUPP;

	/* ����ģ��֧�ֵı���λ�� */
	/* ################################################################# */
	/* ��λ����һ������32λ��ʽ�޷���ʾ���˴�����SGI��ȷ��λ����д��*/
	lpirwp_info->irwp_save.bitcount = 1UL<<(24-1);
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
				(const char *)":)");
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

	/* ����������##���ֶ���������SGI��ʽ�б�����ο�BMP����б��������ã� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 3;
	
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"BW");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"RGB");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[2]), 
		(const char *)"RGBA");
	
	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
SGI_API int CALLAGREEMENT sgi_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE				*pfile = (ISFILE*)0;

	static SGIHEADER	sgi_header;

	enum EXERESULT		b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&sgi_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡ��ͷ�ṹ */
			if (isio_read((void*)&sgi_header, sizeof(SGIHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ת������ */
			sgi_header.magic  = EXCHANGE_WORD((sgi_header.magic));
			sgi_header.dim    = EXCHANGE_WORD((sgi_header.dim));
			sgi_header.width  = EXCHANGE_WORD((sgi_header.width));
			sgi_header.hight  = EXCHANGE_WORD((sgi_header.hight));
			sgi_header.channel= EXCHANGE_WORD((sgi_header.channel));

			sgi_header.pixmin = EXCHANGE_DWORD((sgi_header.pixmin));
			sgi_header.pixmax = EXCHANGE_DWORD((sgi_header.pixmax));
			sgi_header.palid  = EXCHANGE_DWORD((sgi_header.palid));


			/* �ж��Ƿ�����Ч����֧�֣���SGIͼ���� */
			if ((sgi_header.magic != SGI_MAGIC) || \
				(sgi_header.width == 0)		||		\
				(sgi_header.hight == 0)		||		\
				(sgi_header.channel == 0)	||		\
				(sgi_header.channel == 2)	||		\
				(sgi_header.channel > 4)	||		\
				(sgi_header.palid != 0)		||		\
				(sgi_header.dim == 0)		||		\
				(sgi_header.dim > 3)		||		\
				(sgi_header.bpc != 1))
			{
				b_status = ER_NONIMAGE; __leave;
			}


			/* ��дͼ����Ϣ */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ�������� */
			pinfo_str->imgformat	= IMF_SGI;			/* ͼ������ʽ����׺���� */
			
			pinfo_str->compression  = (sgi_header.storage == 0) ? ICS_RGB : ICS_RLE8;

			pinfo_str->width		= (unsigned long)sgi_header.width;
			pinfo_str->height		= (unsigned long)sgi_header.hight;
			pinfo_str->order		= 1;				/* SGI ͼ��Ϊ����ͼ */
			pinfo_str->bitcount		= (unsigned long)(sgi_header.bpc*sgi_header.channel*8);
			
			/* ������������ */
			switch (pinfo_str->bitcount)
			{
			case	8:
				pinfo_str->r_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->b_mask = 0x0;
				pinfo_str->a_mask = 0x0;
				break;
			case	24:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0x0;
				break;
			case	32:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0xff000000;
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

			LeaveCriticalSection(&sgi_get_info_critical);
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
SGI_API int CALLAGREEMENT sgi_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, linesize;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&sgi_load_img_critical);

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

			/* ��ȡ��ͷ�ṹ */
			if (isio_read((void*)&sgi_header, sizeof(SGIHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ת������ */
			sgi_header.magic  = EXCHANGE_WORD((sgi_header.magic));
			sgi_header.dim    = EXCHANGE_WORD((sgi_header.dim));
			sgi_header.width  = EXCHANGE_WORD((sgi_header.width));
			sgi_header.hight  = EXCHANGE_WORD((sgi_header.hight));
			sgi_header.channel= EXCHANGE_WORD((sgi_header.channel));
			
			sgi_header.pixmin = EXCHANGE_DWORD((sgi_header.pixmin));
			sgi_header.pixmax = EXCHANGE_DWORD((sgi_header.pixmax));
			sgi_header.palid  = EXCHANGE_DWORD((sgi_header.palid));

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* �ж��Ƿ�����Ч����֧�֣���SGIͼ���� */
				if ((sgi_header.magic != SGI_MAGIC) || \
					(sgi_header.width == 0)		||		\
					(sgi_header.hight == 0)		||		\
					(sgi_header.channel == 0)	||		\
					(sgi_header.channel == 2)	||		\
					(sgi_header.channel > 4)	||		\
					(sgi_header.palid != 0)		||		\
					(sgi_header.dim == 0)		||		\
					(sgi_header.dim > 3)		||		\
					(sgi_header.bpc != 1))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* ��дͼ����Ϣ */
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ�������� */
				pinfo_str->imgformat	= IMF_SGI;			/* ͼ������ʽ����׺���� */
				
				pinfo_str->compression  = (sgi_header.storage == 0) ? ICS_RGB : ICS_RLE8;
				
				pinfo_str->width		= (unsigned long)sgi_header.width;
				pinfo_str->height		= (unsigned long)sgi_header.hight;
				pinfo_str->order		= 1;				/* SGI ͼ��Ϊ����ͼ */
				pinfo_str->bitcount		= (unsigned long)(sgi_header.bpc*sgi_header.channel*8);
				
				/* ������������ */
				switch (pinfo_str->bitcount)
				{
				case	8:
					pinfo_str->r_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->b_mask = 0x0;
					pinfo_str->a_mask = 0x0;
					break;
				case	24:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0x0;
					break;
				case	32:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0xff000000;
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave; break;
				}

				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}
			
			
			/* �ϳɻҶȵ�ɫ�� */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

				for (i=0; i<256; i++)
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
			
			/* ��ʼ�����׵�ַ���飨���� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}

			/* ��ȡͼ�����ݰ� */
			if (sgi_header.storage == 0)
			{
				b_status = _read_uncom(pfile, pinfo_str, &sgi_header);
			}
			else
			{
				b_status = _read_rle8(pfile, pinfo_str, &sgi_header);
			}
			
			if (b_status != ER_SUCCESS)
			{
				__leave;
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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
			}

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&sgi_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
SGI_API int CALLAGREEMENT sgi_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	*pbuf = 0;
	int				i, j;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&sgi_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_sgi_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ���ý���ֵ */	
			pinfo_str->process_total   = 3;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* Դ���ݱ����Ǳ�׼��24λͼ�� */
			assert((pinfo_str->r_mask == 0xff0000)&&(pinfo_str->g_mask == 0xff00)&&(pinfo_str->b_mask == 0xff));

			/* ��д��ͷ�ṹ��ת��ΪMOTO���� */
			sgi_header.magic  = EXCHANGE_WORD((SGI_MAGIC));
			sgi_header.dim    = EXCHANGE_WORD((3));
			sgi_header.width  = EXCHANGE_WORD(((unsigned short)pinfo_str->width));
			sgi_header.hight  = EXCHANGE_WORD(((unsigned short)pinfo_str->height));
			sgi_header.channel= EXCHANGE_WORD((3));
			
			sgi_header.pixmin = EXCHANGE_DWORD((0));
			sgi_header.pixmax = EXCHANGE_DWORD((0xff));
			sgi_header.palid  = EXCHANGE_DWORD((0));

			sgi_header.storage= 0;
			sgi_header.bpc    = 1;

			memset((void*)sgi_header.rev0, 0, SGI_REV0_SIZE);
			memset((void*)sgi_header.rev1, 0, SGI_REV1_SIZE);
			memcpy((void*)sgi_header.imgname, (const void *)"iseeexplorer image.", 20);
			
			/* д���ļ�ͷ�ṹ */
			if (isio_write((const void *)&sgi_header, sizeof(SGIHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ������ɫ���������� */
			if ((pbuf=(unsigned char *)malloc(pinfo_str->width+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* д��R�������� */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j] = ((unsigned char *)(pinfo_str->pp_line_addr[((int)pinfo_str->height)-i-1]))[j*3+2];
				}
				if (isio_write((const void *)pbuf, pinfo_str->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			pinfo_str->process_current = 1;
			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}


			/* д��G�������� */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j] = ((unsigned char *)(pinfo_str->pp_line_addr[((int)pinfo_str->height)-i-1]))[j*3+1];
				}
				if (isio_write((const void *)pbuf, pinfo_str->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			pinfo_str->process_current = 2;
			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}


			/* д��B�������� */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j] = ((unsigned char *)(pinfo_str->pp_line_addr[((int)pinfo_str->height)-i-1]))[j*3+0];
				}
				if (isio_write((const void *)pbuf, pinfo_str->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pbuf)
				free(pbuf);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&sgi_save_img_critical);
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


/* �жϴ����ͼ���Ƿ���Ա����棨�粻֧�ֱ��湦�ܣ��ɽ�������ȥ���� */
int CALLAGREEMENT _sgi_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(sgi_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (sgi_irwp_info.irwp_save.img_num)
	{
		if (sgi_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* ͼ���������ȷ */
			}
		}
	}

	return 0;
}


/* ��δѹ��ͼ�����ݵ����ݰ��� */
static enum EXERESULT CALLAGREEMENT _read_uncom(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi)
{
	int				i, linesize;
	unsigned char	*p;
	long			fr, fg, fb, fa;
	unsigned char	*pr = 0, *pg = 0, *pb = 0, *pa = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pinfo_str&&psgi);

	__try
	{
		if (isio_seek(pfile, sizeof(SGIHEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		assert(pinfo_str->pp_line_addr);

		linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
		p = pinfo_str->p_bit_data;
		
		switch (pinfo_str->bitcount)
		{
		case	8:
			assert((psgi->channel == 1)&&(psgi->bpc == 1));

			/* ���ж�ȡ�������� */
			for (i=0; i<(int)psgi->hight; i++)
			{
				if (isio_read((void*)p, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i;
				p += linesize;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	24:
			assert((psgi->channel == 3)&&(psgi->bpc == 1));

			/* ����R��G��B��ʱ������ */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);

			if (!(pr&&pg&&pb))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}

			/* ����R��G��B�������������е�ƫ�� */
			fr = sizeof(SGIHEADER);
			fg = fr + psgi->width * psgi->hight;
			fb = fg + psgi->width * psgi->hight;

			/* ���ж�ȡ�������� */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* ��ɫ���� */
				if (isio_seek(pfile, fr, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pr, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ɫ���� */
				if (isio_seek(pfile, fg, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pg, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ��ɫ���� */
				if (isio_seek(pfile, fb, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pb, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				_to_rgb(pr, pg, pb, p, psgi->width);

				pinfo_str->process_current = i;
				p += linesize;

				fr += (long)psgi->width;
				fg += (long)psgi->width;
				fb += (long)psgi->width;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	32:
			assert((psgi->channel == 4)&&(psgi->bpc == 1));

			/* ����R��G��B��ʱ������ */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);
			pa = (unsigned char *)malloc(psgi->width);
			
			if (!(pr&&pg&&pb&&pa))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}
			
			/* ����R��G��B�������������е�ƫ�� */
			fr = sizeof(SGIHEADER);
			fg = fr + psgi->width * psgi->hight;
			fb = fg + psgi->width * psgi->hight;
			fa = fb + psgi->width * psgi->hight;
			
			/* ���ж�ȡ�������� */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* ��ɫ���� */
				if (isio_seek(pfile, fr, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pr, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ɫ���� */
				if (isio_seek(pfile, fg, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pg, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ɫ���� */
				if (isio_seek(pfile, fb, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pb, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ALPHAͨ������ */
				if (isio_seek(pfile, fa, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pa, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				_to_rgba(pr, pg, pb, pa, p, psgi->width);
				
				pinfo_str->process_current = i;
				p += linesize;
				
				fr += (long)psgi->width;
				fg += (long)psgi->width;
				fb += (long)psgi->width;
				fa += (long)psgi->width;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		default:
			assert(0); b_status = ER_SYSERR; __leave; break;
		}

	}
	__finally
	{
		if (pr)
			free(pr);
		if (pg)
			free(pg);
		if (pb)
			free(pb);
		if (pa)
			free(pa);
	}
	
	return b_status;
}


/* ��RLE8ѹ��ͼ�����ݵ����ݰ��� */
static enum EXERESULT CALLAGREEMENT _read_rle8(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi)
{
	int				i, linesize;
	unsigned char	*p;
	unsigned char	*pr = 0, *pg = 0, *pb = 0, *pa = 0, *ptmp = 0;
	unsigned long	*pofftab = 0, *plentab = 0;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(pfile&&pinfo_str&&psgi);
	
	__try
	{
		if (isio_seek(pfile, sizeof(SGIHEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* ����RLE��ѹ����ʱ������ */
		if ((ptmp=(unsigned char *)malloc(psgi->width*2+4)) == 0)
		{
			b_status = ER_MEMORYERR; __leave;
		}

		/* �������ڴ洢RLE��ƫ�Ƽ��г��ȵĻ����� */
		pofftab = (unsigned long *)malloc(psgi->hight*psgi->channel*sizeof(long));
		plentab = (unsigned long *)malloc(psgi->hight*psgi->channel*sizeof(long));

		if (!(pofftab&&plentab))
		{
			b_status = ER_MEMORYERR; __leave;
		}

		/* ����RLE��ƫ�Ʊ� */
		if (_read_long_tab(pfile, pofftab, psgi->hight*psgi->channel) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* ����RLE�г��ȱ� */
		if (_read_long_tab(pfile, plentab, psgi->hight*psgi->channel) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
		p = pinfo_str->p_bit_data;

		switch (pinfo_str->bitcount)
		{
		case	8:
			assert((psgi->channel == 1)&&(psgi->bpc == 1));
			
			/* ���ж�ȡ�������� */
			for (i=0; i<(int)psgi->hight; i++)
			{
				if (isio_seek(pfile, pofftab[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ѹ��RLE�� */
				if (_uncompression_rle8(ptmp, (int)plentab[i], p, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				pinfo_str->process_current = i;
				p += linesize;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	24:
			assert((psgi->channel == 3)&&(psgi->bpc == 1));
			
			/* ����R��G��B��ʱ������ */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);
			
			if (!(pr&&pg&&pb))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}
			
			/* ���ж�ȡ�������� */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* ��ɫ���� */
				if (isio_seek(pfile, pofftab[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i], pr, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* ��ɫ���� */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)], pg, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ��ɫ���� */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)*2], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)*2], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)*2], pb, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				_to_rgb(pr, pg, pb, p, psgi->width);
				
				pinfo_str->process_current = i;
				p += linesize;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	32:
			assert((psgi->channel == 4)&&(psgi->bpc == 1));
			
			/* ����R��G��B��ʱ������ */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);
			pa = (unsigned char *)malloc(psgi->width);
			
			if (!(pr&&pg&&pb&&pa))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}
			
			/* ���ж�ȡ�������� */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* ��ɫ���� */
				if (isio_seek(pfile, pofftab[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i], pr, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ��ɫ���� */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)], pg, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ��ɫ���� */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)*2], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)*2], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)*2], pb, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ALPHAͨ������ */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)*3], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)*3], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)*3], pa, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				_to_rgba(pr, pg, pb, pa, p, psgi->width);
				
				pinfo_str->process_current = i;
				p += linesize;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		default:
			assert(0); b_status = ER_SYSERR; __leave; break;
		}
		
	}
	__finally
	{
		if (pr)
			free(pr);
		if (pg)
			free(pg);
		if (pb)
			free(pb);
		if (pa)
			free(pa);
		if (ptmp)
			free(ptmp);
		if (pofftab)
			free(pofftab);
		if (plentab)
			free(plentab);
	}
	
	return b_status;
}


/* ���R��G��B������RGB���� */
static void _to_rgb(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *p, int width)
{
	int	i;

	assert(pr&&pg&&pb&&p&&width);

	for (i=0; i<width; i++)
	{
		*p++ = *pb++;
		*p++ = *pg++;
		*p++ = *pr++;
	}
}


/* ���R��G��B��A������RGBA���� */
static void _to_rgba(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *pa, unsigned char *p, int width)
{
	int	i;
	
	assert(pr&&pg&&pb&&pa&&p&&width);
	
	for (i=0; i<width; i++)
	{
		*p++ = *pb++;
		*p++ = *pg++;
		*p++ = *pr++;
		*p++ = *pa++;
	}
}


/* ��ȡlong�ͱ�ƫ�Ʊ���г��ȱ� */
static int _read_long_tab(ISFILE *pfile, unsigned long *pl, int count)
{
	int i;

	assert(pfile&&pl);

	if (isio_read((void*)pl, count*sizeof(long), 1, pfile) == 0)
	{
		return -1;		/* �������� */
	}

	/* ת������ */
	for (i=0; i<count; i++)
	{
		pl[i] = EXCHANGE_DWORD((pl[i]));
	}

	return 0;
}


/* ��ѹһ��RLE���� */
static int _uncompression_rle8(unsigned char *prle, int rlesize, unsigned char *p, int bufsize)
{
	int				bufcount = 0;
	unsigned char	pix, count;

	while (1)
	{
		/* ����ֵ */
		pix = *prle++; rlesize--;

		/* ��ȡ����ֵ���������ֵΪ0����˵��RLE�н����� */
		if ((count=(pix&0x7f)) == 0)
		{
			if ((bufcount == bufsize)&&(rlesize == 0))
				return 0;
			else
				return -1;
		}

		bufcount += (int)count;

		if (bufcount > bufsize)
		{
			return -1;		/* Դ�������� */
		}

		if(pix & 0x80)
		{
			if (rlesize < (int)count)
			{
				return -1;	/* ���� */
			}
			else
			{
				rlesize -= (int)count;
			}

			/* ����δѹ������ */
			while(count--)
			{
				*p++ = *prle++;
			}
		}
		else
		{
			if (rlesize == (int)0)
			{
				return -1;	/* ���� */
			}
			else
			{
				rlesize--;
			}
			
			pix = *prle++;

			/* �����ظ����� */
			while(count--)
			{
				*p++ = pix;
			}
		}

		if (bufcount == bufsize)
		{
			if (rlesize == 0)
				return 0;	/* �ɹ� */
			else if ((rlesize = 1)&&(((*prle)&0x7f) == 0))
				return 0;	/* �ɹ� */
			else
				return -1;
		}
	}

	return -1;				/* ���� */
}

