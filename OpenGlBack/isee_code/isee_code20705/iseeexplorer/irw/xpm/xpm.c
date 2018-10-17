/********************************************************************

	xpm.c

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
	���ļ���;��	ISeeͼ���������XPMͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�XPM v3��ʽͼ�񣨶�ע����Ϣ�����ݴ��ԣ���
					���湦�ܣ�1��4��8λͼ��ע���������ȵ���Ϣ��

	���ļ���д�ˣ�	YZ			yzfree##yeah.net

	���ļ��汾��	20125
	����޸��ڣ�	2002-1-25

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

#include "xpm.h"
#include "rgbtab.h"

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


IRWP_INFO			 xpm_irwp_info;			/* �����Ϣ�� */
#define CTSIZE		 16						/* ��ɫת�������ڱ��湦�� */
static char			 ct1[17] = " *.o#+@O$81ugcms";
static unsigned short clbuff[256];			/* ��ɫ�������� */
static unsigned char  rwbuff[512];			/* ��д������ */


#ifdef WIN32
CRITICAL_SECTION	xpm_get_info_critical;	/* xpm_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	xpm_load_img_critical;	/* xpm_load_image�����Ĺؼ��� */
CRITICAL_SECTION	xpm_save_img_critical;	/* xpm_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _xpm_is_valid_img(LPINFOSTR pinfo_str);

int CALLAGREEMENT _read_until_data(ISFILE *fp);
int CALLAGREEMENT _read_line(ISFILE *fp, char *buff);
int CALLAGREEMENT _skip_spec(ISFILE *fp);
int CALLAGREEMENT _distill_comm(ISFILE *fp, char *comm, int commsize);
int CALLAGREEMENT _read_color_line(LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp);
int CALLAGREEMENT _parse_color_line(char *line, int cpp, char *colname, unsigned long *rgb);
int CALLAGREEMENT _read_pix_line(unsigned long *pixbuf, int width, int bitcount, LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&xpm_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&xpm_get_info_critical);
			InitializeCriticalSection(&xpm_load_img_critical);
			InitializeCriticalSection(&xpm_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&xpm_get_info_critical);
			DeleteCriticalSection(&xpm_load_img_critical);
			DeleteCriticalSection(&xpm_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

XPM_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&xpm_irwp_info;
}

#else

XPM_API LPIRWP_INFO CALLAGREEMENT xpm_get_plugin_info()
{
	_init_irwp_info(&xpm_irwp_info);

	return (LPIRWP_INFO)&xpm_irwp_info;
}

XPM_API void CALLAGREEMENT xpm_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

XPM_API void CALLAGREEMENT xpm_detach_plugin()
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
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(4-1)) | (1UL<<(8-1));
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
				(const char *)"XPM �����������ͦ�鷳:)");
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
XPM_API int CALLAGREEMENT xpm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256];
	int				w, h, cpp, ncol, i;
	char			*p;
	LPXPMCOLOR		pcol = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&xpm_get_info_critical);

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

			/* ȥ���ļ��ײ�����Ŀո��ַ� */
			if (_skip_spec(pfile) != 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡע����Ϣ */
			if (_distill_comm(pfile, rwbuff, 512) < 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ���XPM�ļ��ײ���־ */
			if (strcmp(rwbuff, XPM_MARK) != 0)
			{
				b_status = ER_NONIMAGE; __leave;
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

			/* ��ȡͼ�����ݱ����� */
			if (rwbuff[0] == 's')
			{
				if (sscanf(rwbuff, "static char * %s[] = {", agb_name) != 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else if (rwbuff[0] == 'c')
			{
				if (sscanf(rwbuff, "char * %s[] = {", agb_name) != 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* �������Я����[]���� */
			p = strrchr(agb_name, (int)'[');
			if (p)
				*p = '\0';

			/* ȥ������Ŀո�ע�͵��ַ� */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ����ͼ����ߵ����� */
			if (_read_line(pfile, rwbuff) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡͼ����ߡ�CPP����ɫֵ */
			if (sscanf(rwbuff, "\"%d %d %d %d", &w, &h, &ncol, &cpp) != 4)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ͼ�����ݺϷ��Լ�� */
			/* ע����CPPֵ����7ʱ��ģ����Ϊ���ǷǷ���������Ϊ����ʵ������
					��Ӧ�û���������ͼ��CPPֵ��*/
			if ((w == 0)||(h == 0)||(cpp == 0)||(cpp > 7)||(ncol == 0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* ȥ������Ŀո�ע�͵��ַ� */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ����ͼ����ɫ�� */
			pcol = (LPXPMCOLOR)malloc(sizeof(XPMCOLOR)*ncol);
			if (!pcol)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ��ȡ��ɫ��Ϣ */
			if (_read_color_line(pcol, ncol, cpp, pfile) < 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ��дͼ����Ϣ */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_XPM;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression  = ICS_XPM_TEXT;
			
			pinfo_str->width		= w;
			pinfo_str->height		= h;
			pinfo_str->order		= 0;	/* ���� */


			/* �����Ƿ���͸��ɫ */
			for (i=0;i<ncol;i++)
				if (pcol[i].attrib == 1)
					break;

			if (i < ncol)						/* ��͸��ɫ��ͼ��תΪ32λ��������͸��ɫ */
			{
				pinfo_str->bitcount = 32;

				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0xff000000;
			}
			else
			{
				if (ncol <= 2)
					pinfo_str->bitcount	= 1;
				else if (ncol <= 16)
					pinfo_str->bitcount	= 4;
				else if (ncol <= 256)
					pinfo_str->bitcount	= 8;
				else
					pinfo_str->bitcount	= 32;

				if (pinfo_str->bitcount == 32)	/* ��͸��ɫ32λͼ�� */
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else							/* ��ɫ��ͼ�� */
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
			if (AbnormalTermination())
				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
			if (pcol)
				free(pcol);
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&xpm_get_info_critical);
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
XPM_API int CALLAGREEMENT xpm_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256];
	int				w, h, cpp, ncol;
	int				i, linesize, result;
	char			*p;
	LPXPMCOLOR		pcol = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&xpm_load_img_critical);

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
				/* ȥ���ļ��ײ�����Ŀո��ַ� */
				if (_skip_spec(pfile) != 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ��ȡע����Ϣ */
				if (_distill_comm(pfile, rwbuff, 512) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				/* ���XPM�ļ��ײ���־ */
				if (strcmp(rwbuff, XPM_MARK) != 0)
				{
					b_status = ER_NONIMAGE; __leave;
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

				/* ��ȡͼ�����ݱ����� */
				if (rwbuff[0] == 's')
				{
					if (sscanf(rwbuff, "static char * %s[] = {", agb_name) != 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else if (rwbuff[0] == 'c')
				{
					if (sscanf(rwbuff, "char * %s[] = {", agb_name) != 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
				{
					b_status = ER_NONIMAGE; __leave;
				}

				/* �������Я����[]���� */
				p = strrchr(agb_name, (int)'[');
				if (p)
					*p = '\0';

				/* ȥ������Ŀո�ע�͵��ַ� */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ����һ������ */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ��ȡͼ����ߡ�CPP����ɫֵ */
				if (sscanf(rwbuff, "\"%d %d %d %d", &w, &h, &ncol, &cpp) != 4)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				/* ͼ�����ݺϷ��Լ�� */
				/* ע����CPPֵ����7ʱ��ģ����Ϊ���ǷǷ���������Ϊ����ʵ������
						û��������ͼ��CPPֵ��*/
				if ((w == 0)||(h == 0)||(cpp == 0)||(cpp > 7)||(ncol == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* ȥ������Ŀո�ע�͵��ַ� */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ����ͼ����ɫ�� */
				pcol = (LPXPMCOLOR)malloc(sizeof(XPMCOLOR)*ncol);
				if (!pcol)
				{
					b_status = ER_MEMORYERR; __leave;
				}
				
				/* ��ȡ��ɫ��Ϣ */
				if (_read_color_line(pcol, ncol, cpp, pfile) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_XPM;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression  = ICS_XPM_TEXT;
				
				pinfo_str->width		= w;
				pinfo_str->height		= h;
				pinfo_str->order		= 0;	/* ���� */
				
				/* �����Ƿ���͸��ɫ */
				for (i=0;i<ncol;i++)
					if (pcol[i].attrib == 1)
						break;
					
				if (i < ncol)						/* ��͸��ɫ��ͼ��תΪ32λ��������͸��ɫ */
				{
					pinfo_str->bitcount = 32;
					
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0xff000000;
				}
				else
				{
					if (ncol <= 2)
						pinfo_str->bitcount	= 1;
					else if (ncol <= 16)
						pinfo_str->bitcount	= 4;
					else if (ncol <= 256)
						pinfo_str->bitcount	= 8;
					else
						pinfo_str->bitcount	= 32;
					
					if (pinfo_str->bitcount == 32)	/* ��͸��ɫ32λͼ�� */
					{
						pinfo_str->b_mask = 0xff;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->r_mask = 0xff0000;
						pinfo_str->a_mask = 0x0;
					}
					else							/* ��ɫ��ͼ�� */
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
			else
			{
				/* ȥ���ļ��ײ�����Ŀո��ַ� */
				if (_skip_spec(pfile) != 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ��ȡע����Ϣ */
				if (_distill_comm(pfile, rwbuff, 512) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				/* ȥ���ļ��ײ�����Ŀո�ע�͵��ַ� */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ��������������� */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ȥ������Ŀո�ע�͵��ַ� */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ����ͼ����ߡ�CPP������ */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ��ȡͼ����ߡ�CPP����ɫֵ */
				if (sscanf(rwbuff, "\"%d %d %d %d", &w, &h, &ncol, &cpp) != 4)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				/* ���ߵ����ݵĺϷ����Ѿ��жϹ��ˣ����Դ˴��������ж� */

				/* ȥ������Ŀո�ע�͵��ַ� */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ����ͼ����ɫ�� */
				pcol = (LPXPMCOLOR)malloc(sizeof(XPMCOLOR)*ncol);
				if (!pcol)
				{
					b_status = ER_MEMORYERR; __leave;
				}
				/* ��ȡ��ɫ��Ϣ */
				if (_read_color_line(pcol, ncol, cpp, pfile) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			
			/* ȥ������Ŀո�ע�͵��ַ�����λ���������ݴ� */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ���õ�ɫ������ */
			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->pal_count = 2;
				break;
			case	4:
				pinfo_str->pal_count = 16;
				break;
			case	8:
				pinfo_str->pal_count = 256;
				break;
			default:
				pinfo_str->pal_count = 0;
				break;
			}

			for (i=0;i<ncol;i++)
				pinfo_str->palette[i] = pcol[i].rgb;


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
			
			
			/* ���ж��� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				result = _read_pix_line((unsigned long *)pinfo_str->pp_line_addr[i], (int)pinfo_str->width, (int)pinfo_str->bitcount, pcol, ncol, cpp, pfile);

				if (result == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				else if ((result == -2)||(result == -3))
				{
					b_status = ER_BADIMAGE; __leave;
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

			if (pcol)
				free(pcol);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xpm_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
XPM_API int CALLAGREEMENT xpm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	static char		path_buffer[MAX_PATH_LEN];
	static char		drive[MAX_DRIVE_LEN];
	static char		fname[MAX_FNAME_LEN];
	static char		ext[MAX_EXT_LEN];
	static char		dir[MAX_DIR_LEN];

	static char		buffer[MAX_PATH_LEN];

	int				i, j, k, linesize, ncol, cpp, ctloc0, ctloc1;
	unsigned char	*p, tmpc;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&xpm_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_xpm_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ֽ���ļ�������ȷ��XPM�п��߱��������� */
			if (psct->mark[1] == 'F')
				strcpy(path_buffer, (const char *)psct->name);	/* �ļ� */
			else
				strcpy(path_buffer, (const char *)"image");	/* �������� */

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

			/* д���ļ��ײ���־�� */
			sprintf(buffer, "%s\n", XPM_MARK);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* д����������� */
			sprintf(buffer, "static char * %s_xpm[] = {\n", fname);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ֻ�����ɫ��ͼ����Ϊ��ɫ��Ŀ�����ͼ���ʺ���XPM��ʽ���� */
			assert(pinfo_str->pal_count <= 256);

			ncol = pinfo_str->pal_count;		/* ȡ����ɫ�� */
			cpp  = (ncol <= CTSIZE) ? 1 : 2;	/* ȷ����������� */

			/* д����ߵȱ��������� */
			sprintf(buffer, "\"%d %d %d %d\",\n", pinfo_str->width, pinfo_str->height, ncol, cpp);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* д����ɫ���� */
			for (i=0;i<ncol;i++)
			{
				ctloc0 = i/CTSIZE;
				ctloc1 = i%CTSIZE;

				if (cpp == 1)
				{
					sprintf(buffer, "\"%c c #%06X\",\n", ct1[ctloc1], pinfo_str->palette[i]);
					if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				else	/* cpp == 2 */
				{
					sprintf(buffer, "\"%c%c c #%06X\",\n", ct1[ctloc0], ct1[ctloc1], pinfo_str->palette[i]);
					if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
			}

			/* ȡ��ɨ���гߴ� */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			/* д��ͼ������ */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				p = (unsigned char *)(pinfo_str->pp_line_addr[i]);

				/* д��ǰ������ */
				buffer[0] = '\"'; buffer[1] = '\0';
				if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* д��ͼ�������� */
				switch (pinfo_str->bitcount)
				{
				case	1:
					assert(cpp == 1);		/* ��ɫͼCPPֵ�ض�Ϊ1 */
					for (j=0;j<(int)pinfo_str->width;j+=8)
					{
						tmpc = *p++;
						for (k=0;((k<8)&&((j+k)<(int)pinfo_str->width));k++)
						{
							if ((tmpc>>k)&1)
								buffer[0] = ct1[1];
							else
								buffer[0] = ct1[0];
							buffer[1] = 0;
							if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;
							}
						}
					}
					break;
				case	4:
					assert(cpp == 1);
					for (j=0;j<(int)pinfo_str->width;j+=2)
					{
						tmpc = *p++;
						for (k=0;((k<2)&&((j+k)<(int)pinfo_str->width));k++)
						{
							buffer[0] = ct1[((tmpc>>(k*4))&0xf)];
							buffer[1] = 0;
							if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;
							}
						}
					}
					break;
				case	8:					/* 8λ��ͼ��CPPֵΪ2 */
					assert(cpp == 2);
					for (j=0;j<(int)pinfo_str->width;j++)
					{
						tmpc = *p++;
						buffer[0] = ct1[tmpc/CTSIZE];
						buffer[1] = ct1[tmpc%CTSIZE];
						buffer[2] = 0;
						if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
					}
					break;
				default:
					assert(0);		/* ϵͳ���ȶ� */
					b_status = ER_SYSERR; 
					__leave;
					break;
				}

				/* ��β�����ж� */
				if ((i+1) == (int)(pinfo_str->height))
				{
					buffer[0] = '\"'; buffer[1] = '}'; buffer[2] = ';'; buffer[3] = '\n'; buffer[4] = 0;
				}
				else
				{
					buffer[0] = '\"'; buffer[1] = ','; buffer[2] = '\n'; buffer[3] = 0;
				}
				/* д���׺���� */
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
			if (AbnormalTermination())
				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xpm_save_img_critical);
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
int CALLAGREEMENT _xpm_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(xpm_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (xpm_irwp_info.irwp_save.img_num)
		if (xpm_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */

	return 0;
}


/* ����հ��ַ��������ո�TAB�����س����з��� */
int CALLAGREEMENT _skip_spec(ISFILE *fp)
{
	char buff[4];
	long ioloc;
	
	while(1)
	{
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;					/* ��������ļ����� */
		
		if (buff[0] == ' ')
			continue;
		else if (buff[0] == '\t')
			continue;
		else if (buff[0] == '\n')
			continue;
		else if (buff[0] == '\r')
			continue;
		else
		{
			ioloc = isio_tell(fp);		/* ����дλ�ú���һ�ֽ� */
			ioloc --;
			if (isio_seek(fp, ioloc, SEEK_SET) == -1)
				return -1;
			break;
		}
	}

	return 0;	/* �ɹ� */
}


/* ��ȡע����Ϣ */
int CALLAGREEMENT _distill_comm(ISFILE *fp, char *comm, int commsize)
{
	char buff[4];
	long ioloc;
	int  wanlev, count;
	
	if (commsize < 5)
		return -3;						/* ������������С��������Ҫ5���ֽ� */

	if (isio_read((void*)buff, 2, 1, fp) == 0)
		return -1;					/* ��������ļ����� */
	
	buff[2] = 0;

	if (strcmp(buff, "/*"))
	{
		ioloc = isio_tell(fp);		/* ����дλ�ú���2�ֽ� */
		ioloc -= 2;
		if (isio_seek(fp, ioloc, SEEK_SET) == -1)
			return -1;
		
		return -2;					/* ��ǰλ�ò���ע����Ϣ */
	}
	
	strcpy(comm, buff);

	wanlev = 0; count = 2;

	while (1)						/* ���ע����Ϣ */
	{
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;

		comm[count++] = buff[0];
		
		if (count == commsize)
			return -4;				/* ע����Ϣ�������ѳ��������������ߴ� */

		if (buff[0] == '*')
			wanlev = 1;
		else
		{
			if (buff[0] != '/')
				wanlev = 0;
			else if (wanlev == 1)
				break;
			continue;
		}
	}

	comm[count] = 0;
	
	return count;	/* �ɹ� */
}


/* ��������ַ�������ע����Ϣ����ֱ����Ч����λ�� */
int CALLAGREEMENT _read_until_data(ISFILE *fp)
{
	char buff[4];
	long ioloc;
	int  wanlev;

	while(1)
	{
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;						/* ��������ļ����� */

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
			if (isio_read((void*)buff, 1, 1, fp) == 0)
				return -1;
			if (buff[0] == '*')				/* ע����Ϣ��ʼ */
			{
				wanlev = 0;
				while (1)					/* ���ע����Ϣ */
				{
					if (isio_read((void*)buff, 1, 1, fp) == 0)
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
int CALLAGREEMENT _read_line(ISFILE *fp, char *buff)
{
	int i = 0;

	while(1)
	{
		if (isio_read((void*)&(buff[i]), 1, 1, fp) == 0)
			return -1;								/* ��������ļ����� */
		
		if (buff[i] == '\r')
			continue;
		else if (buff[i] == '\n')					/* ȥ��/r/n���� */
		{
			buff[i++] = (char)0;
			break;
		}
		else
			i++;
	}

	return i;
}


/* ��ȡͼ���ļ��е�������ɫ���� */
int CALLAGREEMENT _read_color_line(LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp)
{
	static char buff[256];
	int	i, result;

	for (i=0;i<ncol;i++)
	{
		if (_read_line(fp, (char *)buff) == -1)
			return -1;						/* �ļ���д�� */

		result = _parse_color_line((char *)buff, cpp, pcol[i].pix, &(pcol[i].rgb));

		switch (result)
		{
		case	-1:
			return -2;						/* �ļ����ݴ��� */
		case	0:
			pcol[i].attrib = 0;
			break;
		case	1:
			pcol[i].attrib = 1;
			break;
		case	-2:
		default:
			assert(0);
			return -3;						/* ϵͳ���ȶ� */
		}
	}

	return i;
}


#define XPM_MONO_MASK	1
#define XPM_COLOR_MASK	2
#define XPM_GRAY_MASK	4

/* ������ɫ���� */
int CALLAGREEMENT _parse_color_line(char *line, int cpp, char *colname, unsigned long *rgb)
{
	static char mbuf[128], cbuf[128], gbuf[128];
	unsigned long mrgb, crgb, grgb;
	int  matt, catt, gatt;
	char *p, *coldat;
	char nametype;
	int  result, bmask = 0;
	
	assert((line)&&(strlen(line)));

	if ((p=strchr(line, (int)'\"')) == 0)
		return -1;						/* Դ���ݸ�ʽ����ȷ */

	p++;								/* ���ǰ������ */

	if (strlen(p) <= (size_t)cpp)
		return -1;

	memcpy((void*)colname, (const void *)p, cpp);	/* �������������� */

	p += cpp;							/* ������������� */
	
	matt = catt = gatt = 0;

	while (*p)
	{
		SKIPSPACE(p);					/* �����հ׷� */
		if (*p == '\"')
			break;

		nametype = *p;
		SKIPNONSPACE(p);
		SKIPSPACE(p);
		coldat = p;
		SKIPNONSPACE(p);

		switch (nametype)
		{
		case	'S':
		case	's':					/* �����������ƴ� */
			continue;
		case	'M':
		case	'm':
			matt = result = _color_to_rgb(coldat, p - coldat, &mrgb);
			if ((matt == 0)||(matt == 1))
				bmask |= XPM_MONO_MASK;
			break;
		case	'C':
		case	'c':
			catt = result = _color_to_rgb(coldat, p - coldat, &crgb);
			if ((catt == 0)||(catt == 1))
				bmask |= XPM_COLOR_MASK;
			break;
		case	'G':
		case	'g':
			gatt = result = _color_to_rgb(coldat, p - coldat, &grgb);
			if ((gatt == 0)||(gatt == 1))
				bmask |= XPM_GRAY_MASK;
			break;
		default:
			return -1;					/* Դ���ݸ�ʽ����ȷ */
		}

		if ((result != 1)&&(result != 0)&&(result != -1))
		{
			assert(0);					/* ϵͳ���ȶ� :) */
			return -2;
		}
	}

	/* �˴�������ɫ��ԭ���ǣ��в�ɫ���ݾͲ��ûҶ����ݣ��лҶ�
		���ݾͲ��úڰ����ݣ�������ڰ����ݶ�û���򷵻ش���*/
	if (bmask & XPM_COLOR_MASK)
	{ *rgb = crgb; return catt; }
	else if (bmask & XPM_GRAY_MASK)
	{ *rgb = grgb; return gatt; }
	else if (bmask &XPM_MONO_MASK)
	{ *rgb = mrgb; return matt; }
	else
		return -1;						/* Դͼ��������ȱ�� */
}


/* ��ȡһ���������� */
int CALLAGREEMENT _read_pix_line(unsigned long *pixbuf, int width, int bitcount, LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp)
{
	static char filebuf[256], buf[8];
	char *p;
	int i, index;

	/* ȥ������Ŀո�ע���ַ� */
	if (_read_until_data(fp) == -1)
		return -1;				/* �ļ���д�� */

	if (_read_line(fp, filebuf) == -1)
		return -1;

	p = strchr(filebuf, '\"');	/* ��λ��ǰ�����Ŵ� */
	if (p == 0)
		return -2;				/* �Ƿ������ظ�ʽ */

	if (strlen(p) < 3)
		return -3;				/* �������ż�һ�����ط�������г��� */
	
	p++;						/* ���ǰ������ */

	switch (bitcount)
	{
	case	1:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* �Ƿ��������� */
			assert(index < 2);
			if (i%8)
				((unsigned char *)pixbuf)[i/8] |= (unsigned char)index<<(i%8);
			else
				((unsigned char *)pixbuf)[i/8] = (unsigned char)index&0x1;
			p += cpp;
		}
		break;
	case	4:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* �Ƿ��������� */
			assert(index < 16);
			if (i%2)
				((unsigned char *)pixbuf)[i/2] |= (unsigned char)index<<4;
			else
				((unsigned char *)pixbuf)[i/2] = (unsigned char)index&0xf;
			p += cpp;
		}
		break;
	case	8:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* �Ƿ��������� */
			assert(index < 256);
			((unsigned char *)pixbuf)[i] = (unsigned char)index;
			p += cpp;
		}
		break;
	case	32:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* �Ƿ��������� */
			pixbuf[i] = pcol[index].rgb;
			p += cpp;
		}
		break;
	default:
		assert(0);
		return -3;				/* ϵͳ���ȶ� */
	}

	return 0;					/* �ɹ� */
}
