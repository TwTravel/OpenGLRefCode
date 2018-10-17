/********************************************************************

	xbm.c

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
	���ļ���;��	ISeeͼ���������XBMͼ���дģ��ʵ���ļ�

			��ȡ���ܣ�X10��X11��ʽXBMͼ�񣨶�ע����Ϣ����
						�ݴ��ԣ���
							  
			���湦�ܣ�X11��ʽXBM
							   

	���ļ���д�ˣ�	YZ			yzfree##yeah.net

	���ļ��汾��	20113
	����޸��ڣ�	2002-1-13

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-1		��һ�η������°汾����ʼ�汾��2.0��


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

#include "xbm.h"


#ifdef WIN32
#define MAX_PATH_LEN	_MAX_PATH
#define MAX_DRIVE_LEN	_MAX_DRIVE
#define MAX_DIR_LEN		_MAX_DIR
#define MAX_FNAME_LEN	_MAX_FNAME
#define MAX_EXT_LEN		_MAX_EXT
#else
#define MAX_PATH_LEN	512				/* Ӧ�޸�ΪLinux�е���Ӧֵ */
#define MAX_DRIVE_LEN	32
#define MAX_DIR_LEN		512
#define MAX_FNAME_LEN	512
#define MAX_EXT_LEN		512
#endif


IRWP_INFO			 xbm_irwp_info;			/* �����Ϣ�� */

static int			 hex_table[256];		/* �ַ�ת���ֱ� */
static unsigned char rwbuff[512];			/* ��д������ */


#ifdef WIN32
CRITICAL_SECTION	xbm_get_info_critical;	/* xbm_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	xbm_load_img_critical;	/* xbm_load_image�����Ĺؼ��� */
CRITICAL_SECTION	xbm_save_img_critical;	/* xbm_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _xbm_is_valid_img(LPINFOSTR pinfo_str);

void CALLAGREEMENT _init_hex_table(void);
int CALLAGREEMENT _read_integer(ISFILE	*fp);
int CALLAGREEMENT _read_until_data(ISFILE *fp);
int CALLAGREEMENT _read_line(ISFILE *fp, unsigned char *buff);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&xbm_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&xbm_get_info_critical);
			InitializeCriticalSection(&xbm_load_img_critical);
			InitializeCriticalSection(&xbm_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&xbm_get_info_critical);
			DeleteCriticalSection(&xbm_load_img_critical);
			DeleteCriticalSection(&xbm_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

XBM_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&xbm_irwp_info;
}

#else

XBM_API LPIRWP_INFO CALLAGREEMENT xbm_get_plugin_info()
{
	_init_irwp_info(&xbm_irwp_info);

	return (LPIRWP_INFO)&xbm_irwp_info;
}

XBM_API void CALLAGREEMENT xbm_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

XBM_API void CALLAGREEMENT xbm_detach_plugin()
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
	/* ��λ����һ������32λ��ʽ�޷���ʾ��*/
	lpirwp_info->irwp_save.bitcount = 1UL;
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
				(const char *)"�Ǻǣ�XBM���С������������˼��:)");
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
XBM_API int CALLAGREEMENT xbm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256];
	unsigned int	w, h;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&xbm_get_info_critical);

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

			/* ȥ���ļ��ײ�����Ŀո�ע�͵��ַ� */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ����һ������ */
			if (_read_line(pfile, rwbuff) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡͼ����ֵ */
			if ((sscanf(rwbuff, "#define %s %u", agb_name, &w) == 2) && 
				(strlen(agb_name) >= 6) &&
				(strcmp(agb_name+strlen(agb_name)-6, "_width") == 0))
			{
				pinfo_str->width = (unsigned long)w;
			}
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}


			/* ����һ������ */
			if (_read_line(pfile, rwbuff) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡͼ��߶�ֵ */
			if ((sscanf(rwbuff, "#define %s %u", agb_name, &h) == 2) && 
				(strlen(agb_name) >= 7) &&
				(strcmp(agb_name+strlen(agb_name)-7, "_height") == 0))
			{
				pinfo_str->height = (unsigned long)h;
			}
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}

			if ((w == 0)||(h == 0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_XBM;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression  = ICS_XBM_TEXT;
			
			pinfo_str->order		= 0;	/* ���� */
			pinfo_str->bitcount		= 1;	/* XBMΪ��ɫͼ��λ��Ϊ 1 */
			
			pinfo_str->b_mask = 0;			/* ����ͼ������������ */
			pinfo_str->g_mask = 0;
			pinfo_str->r_mask = 0;
			pinfo_str->a_mask = 0;
				
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&xbm_get_info_critical);
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
XBM_API int CALLAGREEMENT xbm_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256], *p;
	unsigned int	w, h, version;
	int				i, linesize, result, j;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&xbm_load_img_critical);

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

			/* ���ļ���λ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ȥ���ļ��ײ�����Ŀո�ע�͵��ַ� */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ����һ������ */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ȡͼ����ֵ */
				if ((sscanf(rwbuff, "#define %s %u", agb_name, &w) == 2) && 
					(strlen(agb_name) >= 6) &&
					(strcmp(agb_name+strlen(agb_name)-6, "_width") == 0))
				{
					pinfo_str->width = (unsigned long)w;
				}
				else
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				
				/* ����һ������ */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ȡͼ��߶�ֵ */
				if ((sscanf(rwbuff, "#define %s %u", agb_name, &h) == 2) && 
					(strlen(agb_name) >= 7) &&
					(strcmp(agb_name+strlen(agb_name)-7, "_height") == 0))
				{
					pinfo_str->height = (unsigned long)h;
				}
				else
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* ���ͼ����������Ƿ�Ϸ� */
				if ((w == 0)||(h == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_XBM;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression  = ICS_XBM_TEXT;
				
				pinfo_str->order		= 0;	/* ���� */
				pinfo_str->bitcount		= 1;	/* XBMΪ��ɫͼ��λ��Ϊ 1 */
				
				pinfo_str->b_mask = 0;			/* ����ͼ������������ */
				pinfo_str->g_mask = 0;
				pinfo_str->r_mask = 0;
				pinfo_str->a_mask = 0;

				pinfo_str->data_state = 1;		/* ���ñ�� */
			}
			else
			{
				/* ����ļ������Ϣ */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			/* ����ȵ���Ϣ�� */
			while(1)
			{
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				if (sscanf(rwbuff, "#define %s %u", agb_name, &w) == 2)
					continue;
				else
					break;
			}
					
			/* ���õ�ɫ������ */
			pinfo_str->pal_count = 2;
			/* ʵ��λ����Ϊ1������Ϊ0������0����Ϊ��ɫ��1����Ϊ��ɫ */
			pinfo_str->palette[0] = 0xffffffUL;
			pinfo_str->palette[1] = 0UL;

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
			
			/* ��ʼ���������� */			
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			
			
			version = 11;		/* ȱʡ��X11 XBM ��ʽ */

			/* �ж�XBM�汾 */
			while (1)
			{
				if (sscanf(rwbuff, "static short %s = {", agb_name) == 1)
					version = 10;
				else
				{
					if (sscanf(rwbuff, "static unsigned char %s = {", agb_name) == 1)
						version = 11;
					else
						if (sscanf(rwbuff, "static char %s = {", agb_name) == 1)
							version = 11;
						else
							continue;
				}

				p = (unsigned char *)strrchr(agb_name, '_');

				/* ���_bits[]��� */
				if (p == (unsigned char *)0)
					p = (unsigned char *)agb_name;
				else
					p++;

				if (strcmp((const char *)"bits[]",(char *)p) == 0)
					break;

				/* ����һ������ */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			/* ��ʼ��ת��ֵ�� */
			_init_hex_table();

			if (version == 11)
			{
				/* ���ж��� */
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					for (j=0;j<(int)(pinfo_str->width);j+=8)
					{
						result = _read_integer(pfile);
						if (result == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}
						*(unsigned char *)(pinfo_str->p_bit_data+(i*linesize)+j/8) = (unsigned char)result;
					}

					pinfo_str->process_current = i+1;
					
					/* ֧���û��ж� */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
			}
			else
			{
				assert(version == 10);
				/* ���ж��� */
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					for (j=0;j<(int)(pinfo_str->width);j+=16)
					{
						result = _read_integer(pfile);
						if (result == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}
						*(unsigned short *)(pinfo_str->p_bit_data+(i*linesize)+j/8) = (unsigned short)result;
					}
					
					pinfo_str->process_current = i+1;
					
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

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xbm_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
XBM_API int CALLAGREEMENT xbm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	static char		path_buffer[MAX_PATH_LEN];
	static char		drive[MAX_DRIVE_LEN];
	static char		fname[MAX_FNAME_LEN];
	static char		ext[MAX_EXT_LEN];
	static char		dir[MAX_DIR_LEN];

	static char		buffer[MAX_PATH_LEN];

	int				i, j, linesize;
	unsigned char	*p;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&xbm_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_xbm_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ֽ���ļ�������ȷ��XBM�п��߱��������� */
			if (psct->mark[1] == 'F')
				strcpy(path_buffer, (const char *)psct->name);	/* �ļ� */
			else
				strcpy(path_buffer, (const char *)"temp_name");	/* �������� */

			_splitpath(path_buffer, drive, dir, fname, ext);
			
			/* �ų��ļ����ж����'.'�ַ������ַ��������ڱ�������*/
			p = (unsigned char *)strchr((const char *)fname, (int)'.');
			if (p)
				*p = 0;


			/* ��λ���ļ��ײ� */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
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

			/* д����߱������� */
			sprintf(buffer, "#define %s_width %u\n", fname, pinfo_str->width);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			sprintf(buffer, "#define %s_height %u\n", fname, pinfo_str->height);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ע����ģ�齫�������ȵ���Ϣ */

			/* �����ʽΪX11 */
			sprintf(buffer, "static unsigned char %s_bits[] = {\n", fname);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			strcpy(buffer, (const char *)"   ");
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			assert(pinfo_str->bitcount == 1);

			linesize = (pinfo_str->width+7)/8;

			/* д��ͼ������ */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				p = (unsigned char *)(pinfo_str->pp_line_addr[i]);

				for (j=0;j<linesize;j++)
				{
					/* ��β�����ж� */
					if (((i+1) == (int)(pinfo_str->height))&&((j+1) == linesize))
					{
						sprintf(buffer, "0x%02x };", (unsigned char)(*p++));
						if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
					}
					else
					{
						sprintf(buffer, "0x%02x, ", (unsigned char)(*p++));
						if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
					}
				}

				strcpy(buffer, "\n   ");
				if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* �ۼӲ���ֵ */
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
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xbm_save_img_critical);
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
int CALLAGREEMENT _xbm_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(xbm_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (xbm_irwp_info.irwp_save.img_num)
		if (xbm_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */

	return 0;
}


/* ��ʼ���ַ�ֵ�� */
void CALLAGREEMENT _init_hex_table()
{
	int i;

	for (i=(int)'0';i<=(int)'9';i++)
		hex_table[i] = i - (int)'0';

	for (i=(int)'a';i<=(int)'f';i++)
		hex_table[i] = i - (int)'a' + 10;

	for (i=(int)'A';i<=(int)'F';i++)
		hex_table[i] = i - (int)'A' + 10;

	hex_table['x'] = 0;
	hex_table['X'] = 0;

	hex_table[' '] = -1;
	hex_table[','] = -1;
	hex_table['}'] = -1;
	hex_table['\n'] = -1;
	hex_table['\t'] = -1;
}


/* ��ȡһ������ */
int CALLAGREEMENT _read_integer(ISFILE *fp)
{
	int	c, flag = 0, value = 0;
	
	while (1)
	{
		if (isio_read((void*)&c, 1, 1, fp) == -1)		
			return -1;								/* ��������ļ����� */
		
		c &= 0xff;

		if (isxdigit(c))
		{
			value = (value << 4) + hex_table[c];
			flag++;
			continue;
		}

		if ((hex_table[c]<0) && flag)		/* ','�Ϳո�ı�ֵС��0 */
			break;
	}

	return value;
}


/* ��������ַ�������ע����Ϣ����ֱ����Ч����λ�� */
int CALLAGREEMENT _read_until_data(ISFILE *fp)
{
	char buff[4];
	long ioloc;
	int  wanlev;

	while(1)
	{
		if (isio_read((void*)buff, 1, 1, fp) == -1)		
			return -1;								/* ��������ļ����� */

		if (buff[0] == ' ')
			continue;
		else if (buff[0] == '\t')
			continue;
		else if (buff[0] == '\n')
			continue;
		else if (buff[0] == '\r')
			continue;
		else if (buff[0] == '/')
		{
			if (isio_read((void*)buff, 1, 1, fp) == -1)		
				return -1;
			if (buff[0] == '*')				/* ע����Ϣ��ʼ */
			{
				wanlev = 0;
				while (1)					/* ���ע����Ϣ */
				{
					if (isio_read((void*)buff, 1, 1, fp) == -1)		
						return -1;
					if (buff[0] == '*')
						wanlev = 1;
					else
					{
						if (buff[0] != '/')
							wanlev = 0;
						else if ((buff[0] == '/')&&(wanlev == 1))
							break;
						continue;
					}
				}
			}
			else
			{
				ioloc = isio_tell(fp);		/* ����дλ�ú���һ�ֽ� */
				ioloc --;
				if (isio_seek(fp, ioloc, SEEK_SET) == -1)
					return -1;
			}
			continue;
		}
		else
		{
			ioloc = isio_tell(fp);		/* ����дλ�ú���һ�ֽ� */
			ioloc --;
			if (isio_seek(fp, ioloc, SEEK_SET) == -1)
				return -1;
			break;
		}
	}

	return 0;
}


/* ��һ������ */
int CALLAGREEMENT _read_line(ISFILE *fp, unsigned char *buff)
{
	int i = 0;

	while(1)
	{
		if (isio_read((void*)&(buff[i]), 1, 1, fp) == -1)		
			return -1;								/* ��������ļ����� */
		
		if (buff[i] == '\r')
			continue;
		else if (buff[i] == '\n')					/* ȥ��/r/n���� */
		{
			buff[i++] = (unsigned char)0;
			break;
		}
		else
			i++;
	}

	return i;
}
