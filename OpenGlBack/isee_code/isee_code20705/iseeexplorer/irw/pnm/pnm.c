/********************************************************************

	pnm.c

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
	���ļ���;��	ISeeͼ���������PBM��PGM��PPMͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�PBM��PGM��PPM �ı���������ͼ��
							  
					���湦�ܣ��ɽ�1��8��24λͼ�񱣴�Ϊ��Ӧ��
							  PBM��PGM��PPM �ı��������ͼ���ʽ
							   

	���ļ���д�ˣ�	YZ			yzfree##yeah.net

	���ļ��汾��	20207
	����޸��ڣ�	2002-2-7

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-2	��һ�������棨�°�ӿڣ�������汾2.0


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
#include <ctype.h>
#include <assert.h>

#include "pnm.h"


IRWP_INFO			pnm_irwp_info;			/* �����Ϣ�� */

static enum pnm_type						/* PNM ���Ͷ��� */
{
	PBM = 1,
	PGM,
	PPM
};

#ifdef WIN32
CRITICAL_SECTION	pnm_get_info_critical;	/* pnm_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	pnm_load_img_critical;	/* pnm_load_image�����Ĺؼ��� */
CRITICAL_SECTION	pnm_save_img_critical;	/* pnm_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _pnm_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave);
int CALLAGREEMENT _read_number(ISFILE *fp);
int CALLAGREEMENT _write_number(ISFILE *fp, unsigned int num, int mark);
int CALLAGREEMENT _is_pnm_hahaha(char *pmc);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&pnm_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&pnm_get_info_critical);
			InitializeCriticalSection(&pnm_load_img_critical);
			InitializeCriticalSection(&pnm_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&pnm_get_info_critical);
			DeleteCriticalSection(&pnm_load_img_critical);
			DeleteCriticalSection(&pnm_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PNM_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pnm_irwp_info;
}

#else

PNM_API LPIRWP_INFO CALLAGREEMENT pnm_get_plugin_info()
{
	_init_irwp_info(&pnm_irwp_info);

	return (LPIRWP_INFO)&pnm_irwp_info;
}

PNM_API void CALLAGREEMENT pnm_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PNM_API void CALLAGREEMENT pnm_detach_plugin()
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
	/* ��λ����һ������32λ��ʽ�޷���ʾ���˴�����PNM��ȷ��λ����д��*/
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(8-1)) | (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1��ֻ�ܱ���һ��ͼ�� */

	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 1;
	lpirwp_info->irwp_save.para[0].count = 2;		/* �������ִ洢��ʽ���ı��������� */
	strcpy((char*)lpirwp_info->irwp_save.para[0].desc, (const char *)"�洢��ʽ");
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[0], (const char *)"�ı�");
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[1], (const char *)"������");
	lpirwp_info->irwp_save.para[0].value[0] = 0;	/* 0 �����ı���1 ��������� */
	lpirwp_info->irwp_save.para[0].value[1] = 1;

	
	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 1;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"�Ǻǣ��Ҷ��벻��Ҫ˵ʲô��:-D");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 2;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"PGM");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"PPM");

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
PNM_API int CALLAGREEMENT pnm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	char			buff[4];
	int				kind, ascii_mark;
	int				width, height, maxval;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&pnm_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ��ȡ��ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			memset((void*)buff, 0, 4);

			if (isio_read((void*)buff, 2, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж����Ƿ���һ��PNMͼ�� */
			if ((kind=_is_pnm_hahaha(buff)) == -1)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			if (kind > 3)
			{
				ascii_mark = 0;			/* �����Ƹ�ʽ */
				kind -= 3;
			}
			else
			{
				ascii_mark = 1;			/* ASCII ��ʽ */
			}
			
			/* ��ȡͼ��������� */
			if ((width=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			if ((height=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* ����ֵ��� */
			if ((width <= 0)||(height <= 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* PGM��PPM������һ�������ɫ��������ֵ */
			if (kind != PBM)
			{
				if ((maxval=_read_number(pfile)) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				/* ISeeϵͳ��֧�ֵ�����ɫ��������8λ��ͼ�� */
				if ((maxval <= 0)||(maxval > 255))
				{
					b_status = ER_BADIMAGE; __leave;
				}
			}
			else
			{
				maxval = 255;
			}

			
			/* ��дͼ����Ϣ */
			pinfo_str->imgtype		 = IMT_RESSTATIC;	/* ͼ�������� */
			if (kind == PBM)							/* ͼ������ʽ����׺���� */
				pinfo_str->imgformat = IMF_PBM;
			else if (kind == PGM)
				pinfo_str->imgformat = IMF_PGM;
			else if (kind == PPM)
				pinfo_str->imgformat = IMF_PPM;
			else
				assert(0);
			if (ascii_mark == 1)						/* ͼ������ѹ����ʽ */
				pinfo_str->compression  = ICS_PNM_TEXT;
			else if (ascii_mark == 0)
				pinfo_str->compression  = ICS_PNM_BIN;
			else
				assert(0);
			
			pinfo_str->width		= width;
			pinfo_str->height		= height;
			pinfo_str->order		= 0;				/* ���� */

			pinfo_str->b_mask = 0x0;
			pinfo_str->g_mask = 0x0;
			pinfo_str->r_mask = 0x0;
			pinfo_str->a_mask = 0x0;
													
			if (kind == PBM)							/* ͼ��λ�� */
				pinfo_str->bitcount = 1;
			else if (kind == PGM)
				pinfo_str->bitcount = 8;				/* ��ɫ��ͼ */
			else if (kind == PPM)
			{
				pinfo_str->bitcount = 24;

				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0xff000000;
			}
			else
				assert(0);
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (AbnormalTermination())
				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
				
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pnm_get_info_critical);
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
PNM_API int CALLAGREEMENT pnm_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	char			buff[4], *p, ch;
	int				kind, ascii_mark, i, y, x, r, linesize, cr, cb, cg;
	int				width, height, maxval;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pnm_load_img_critical);

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

			/* ������λ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��PNMͼ���ײ���־��Ϣ */
			if (isio_read((void*)buff, 2, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж����Ƿ���һ��PNMͼ�� */
			if ((kind=_is_pnm_hahaha(buff)) == -1)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			if (kind > 3)
			{
				ascii_mark = 0;			/* �����Ƹ�ʽ */
				kind -= 3;
			}
			else
			{
				ascii_mark = 1;			/* ASCII ��ʽ */
			}

			/* ��ȡͼ��������� */
			if ((width=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			if ((height=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* PGM��PPM������һ�������ɫ��������ֵ */
			if (kind != PBM)
			{
				if ((maxval=_read_number(pfile)) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				/* ISeeϵͳ��֧�ֵ�����ɫ��������8λ��ͼ�� */
				if ((maxval <= 0)||(maxval > 255))
				{
					b_status = ER_BADIMAGE; __leave;
				}
			}
			else
			{
				maxval = 255;
			}

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ����ֵ��� */
				if ((width <= 0)||(height <= 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ��дͼ����Ϣ */
				pinfo_str->imgtype		 = IMT_RESSTATIC;	/* ͼ�������� */
				if (kind == PBM)							/* ͼ������ʽ����׺���� */
					pinfo_str->imgformat = IMF_PBM;
				else if (kind == PGM)
					pinfo_str->imgformat = IMF_PGM;
				else if (kind == PPM)
					pinfo_str->imgformat = IMF_PPM;
				else
					assert(0);
				if (ascii_mark == 1)						/* ͼ������ѹ����ʽ */
					pinfo_str->compression  = ICS_PNM_TEXT;
				else if (ascii_mark == 0)
					pinfo_str->compression  = ICS_PNM_BIN;
				else
					assert(0);
				
				pinfo_str->width		= width;
				pinfo_str->height		= height;
				pinfo_str->order		= 0;				/* ���� */

				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
														
				if (kind == PBM)							/* ͼ��λ�� */
					pinfo_str->bitcount = 1;
				else if (kind == PGM)
					pinfo_str->bitcount = 8;				/* ��ɫ��ͼ */
				else if (kind == PPM)
				{
					pinfo_str->bitcount = 24;

					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0xff000000;
				}
				else
					assert(0);
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}


			/* ���õ�ɫ������ */
			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->pal_count = 2;
				pinfo_str->palette[0] = 0xffffff;
				pinfo_str->palette[1] = 0x0;
				break;
			case	8:
				pinfo_str->pal_count = 256;
				for (i=0;i<256;i++)
				{
					pinfo_str->palette[i] = (i<<16)|(i<<8)|(i<<0);
				}
				break;
			case	24:
				pinfo_str->pal_count = 0;
				break;
			default:
				assert(0);
				pinfo_str->pal_count = 0;
				break;
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
			
			/* ��ʼ���������� */			
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			
			/* ������ɫ�������������� */
			r = 255 / maxval;

			/* ���ж��� */
			if (pinfo_str->compression == ICS_PNM_TEXT)
			{
				for (y=0; y<height; y++)
				{
					/* ȡ�����׵�ַ */
					p = (char*)pinfo_str->pp_line_addr[y];

					if (pinfo_str->imgformat == IMF_PBM)
					{
						memset((void*)p, 0, linesize);	/* ��Ϊ����ֻ�С��򡯲�����������Ҫ�ȳ�ʼ��Ϊ0 */

						for (x=0; x<width; x++)
						{
							do {
								if (isio_read((void*)buff, 1, 1, pfile) == 0)
								{
									b_status = ER_FILERWERR; __leave;
								}
							} while(isspace((int)buff[0]));

							/* ���̵�Դ���ݼ��� :) */
							if ((buff[0] != '0')&&(buff[0] != '1'))
							{
								b_status = ER_BADIMAGE; __leave;
							}
							buff[0] -= '0';

							p[x/8] |= buff[0]<<(x%8);	/* ͷ�������ֽڵͶ�(ISee format) */
						}
					}
					else if (pinfo_str->imgformat == IMF_PGM)
					{
						for (x=0; x<width; x++)
						{
							if ((i=_read_number(pfile)) == -1)
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							if (i > maxval)
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							i *= r;	/* ������ɫ������Χ�� 0��255 */
							
							p[x] = (char)i;
						}
					}
					else if (pinfo_str->imgformat == IMF_PPM)
					{
						for (x=0; x<width; x++)
						{
							if (((cr=_read_number(pfile)) == -1)||
								((cg=_read_number(pfile)) == -1)||
							    ((cb=_read_number(pfile)) == -1))
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							if ((cr > maxval)||(cg > maxval)||(cb > maxval))
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							/* ������ɫ������Χ�� 0��255 */
							cr *= r;
							cg *= r;
							cb *= r;
							
							p[x*3]   = (char)cb;
							p[x*3+1] = (char)cg;
							p[x*3+2] = (char)cr;	/* BGR ˳�� (ISee format) */
						}
					}
					else
					{
						assert(0);
					}

					pinfo_str->process_current = y+1;
					
					/* ֧���û��ж� */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
			}
			else	/* ICS_PNM_BIN */
			{
				for (y=0; y<height; y++)
				{
					/* ȡ�����׵�ַ */
					p = (char*)pinfo_str->pp_line_addr[y];
					
					if (pinfo_str->imgformat == IMF_PBM)
					{
						if (isio_read((void*)p, (size_t)((width+7)>>3), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						/* �ߵ��������У�ʹ֮����ISee��ʽ������ */
						for (x=0; x<width; x+=8)
						{
							for (i=0, ch=0; i<8; i++)
							{
								ch |= ((p[x/8]&(1<<i))>>i)<<(7-i);
							}
							p[x/8] = ch;
						}
					}
					else if (pinfo_str->imgformat == IMF_PGM)
					{
						if (isio_read((void*)p, width, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						/* ������������ˣ��õ�0��255��Χ����ɫ����ֵ */
						for (x=0; x<width; x++)
						{
							p[x] *= (char)r;
						}
					}
					else if (pinfo_str->imgformat == IMF_PPM)
					{
						if (isio_read((void*)p, (size_t)(width*3), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						/* ����R��B����λ�ã�ʹ֮����ISee��ʽ */
						for (x=0; x<width; x++)
						{
							buff[0] = p[x*3];
							p[x*3]  = p[x*3+2];
							p[x*3+2]= buff[0];

							p[x*3]   *= (char)r;		/* ���ŷ��� */
							p[x*3+1] *= (char)r;
							p[x*3+2] *= (char)r;
						}
					}
					else
					{
						assert(0);
					}

					pinfo_str->process_current = y+1;
					
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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
			}

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pnm_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
PNM_API int CALLAGREEMENT pnm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				ascii_mark;			/* �洢��ʽ */
	int				kind;				/* �����ʽ����׺����*/
	char			buffer[64], *p, ch;
	int				i, x, y, mark = 0;
	unsigned int	num;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pnm_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_pnm_is_valid_img(pinfo_str, lpsave) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��λ�����ײ� */
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
			
			
			/* 0 �����ı���ʽ��1��������Ʒ�ʽ */
			ascii_mark = (lpsave->para_value[0] == 0) ? 1 : 0;
			/* ȷ�������ʽ */
			switch (pinfo_str->bitcount)
			{
			case	1:
				kind = PBM;
				break;
			case	8:
				kind = PGM;
				break;
			case	24:
				kind = PPM;
				break;
			default:
				assert(0);			/* ��ģ�鲻�ܱ����������ָ�ʽ�����ͼ�� */
				b_status = ER_NSIMGFOR;
				__leave;
				break;
			}

			/* д����ͷ����Ϣ�ṹ */
			sprintf((char*)buffer, "P%d\n#Created by ISeeExplorer PNM Plug-in 2.0\n%d %d", ((ascii_mark == 1) ? kind:(kind+3)), (int)pinfo_str->width, (int)pinfo_str->height);

			switch (kind)
			{
			case	PBM:
				strcat((char*)buffer, (const char *)"\n");
				break;
			case	PGM:
			case	PPM:
				strcat((char*)buffer, (const char *)" 255\n");
				break;
			default:
				assert(0);
				b_status = ER_NSIMGFOR;
				__leave;
				break;
			}

			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д��ͼ������ */
			if (ascii_mark == 1)	/* �ı���ʽ */
			{
				if (kind == PBM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];

						for (x=0; x<(int)pinfo_str->width; x++)
						{
							mark++;
							if (_write_number(pfile, (unsigned int)(unsigned char)((p[x/8]&(1<<(x%8)))>>(x%8)), ((mark<32) ? 0:1)) == -1)
							{
								b_status = ER_FILERWERR; __leave;
							}
							/* ��ΪPNM˵�����й涨ÿ�����ݵ��ַ������ܳ���70���ַ���������mark���� */
							if (mark >= 32)
							{
								mark = 0;
							}
						}

						/* �ۼӲ���ֵ */
						pinfo_str->process_current = y+1;
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else if (kind == PGM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x++)
						{
							((unsigned long *)buffer)[0] = pinfo_str->palette[(unsigned int)(unsigned char)p[x]];
							/* ��RGB����ת��Ϊ�Ҷ����ݡ��Ҷ�ת����ʽ (3*r+6*g+b)/10 */
							num = (((unsigned int)(unsigned char)buffer[2])*3+((unsigned int)(unsigned char)buffer[1])*6+((unsigned int)(unsigned char)buffer[0])*1)/(unsigned int)10;
							mark++;
							if (_write_number(pfile, num, ((mark<16) ? 0:1)) == -1)
							{
								b_status = ER_FILERWERR; __leave;
							}
							if (mark >= 16)
							{
								mark = 0;
							}
						}
						
						/* �ۼӲ���ֵ */
						pinfo_str->process_current = y+1;
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else /* if (kind == PPM) */
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x++)
						{
							mark++;
							/* д��R��G��B���� */
							if ((_write_number(pfile, (unsigned int)(unsigned char)p[x*3+2], 0) == -1)||
								(_write_number(pfile, (unsigned int)(unsigned char)p[x*3+1], 0) == -1)||
								(_write_number(pfile, (unsigned int)(unsigned char)p[x*3+0], ((mark<5) ? 0:1)) == -1))
							{
								b_status = ER_FILERWERR; __leave;
							}
							if (mark >= 5)
							{
								mark = 0;
							}
						}
						
						/* �ۼӲ���ֵ */
						pinfo_str->process_current = y+1;
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
			}
			else					/* �����Ʒ�ʽ */
			{
				assert(ascii_mark == 0);

				/* �����Ʒ�ʽ��ÿ���ַ�����Ҫ�� */

				if (kind == PBM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x+=8)
						{
							/* ��ISee��ʽ��ת��PBM��ʽ */
							for (i=0, ch=0; i<8; i++)
							{
								ch |= ((p[x/8]&(1<<i))>>i)<<(7-i);
							}
							if (isio_write((const void *)&ch, 1, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;/* д�ļ�ʧ�� */
							}
						}
						
						/* �ۼӲ���ֵ */
						pinfo_str->process_current = y+1;
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else if (kind == PGM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						if (isio_write((const void *)p, pinfo_str->width, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;/* д�ļ�ʧ�� */
						}
						
						/* �ۼӲ���ֵ */
						pinfo_str->process_current = y+1;
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else /* if (kind == PPM) */
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x++)
						{
							/* д��R��G��B���� */
							if ((isio_write((const void *)&(p[x*3+2]), 1, 1, pfile) == 0)||
								(isio_write((const void *)&(p[x*3+1]), 1, 1, pfile) == 0)||
								(isio_write((const void *)&(p[x*3+0]), 1, 1, pfile) == 0))
							{
								b_status = ER_FILERWERR; __leave;
							}
						}
						
						/* �ۼӲ���ֵ */
						pinfo_str->process_current = y+1;
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
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
			
			LeaveCriticalSection(&pnm_save_img_critical);
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
int CALLAGREEMENT _pnm_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	int i;

	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(pnm_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (pnm_irwp_info.irwp_save.img_num)
	{
		if (pnm_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* ͼ���������ȷ */
			}
		}
	}

	for (i=0;i<pnm_irwp_info.irwp_save.para[0].count;i++)
	{
		if (pnm_irwp_info.irwp_save.para[0].value[i] == lpsave->para_value[0])
		{
			break;
		}
	}
		
	return (i==pnm_irwp_info.irwp_save.para[0].count) ? -3:0;		/* -3:��Ч�Ĵ洢��ʽ */
}


/* ��ָ�����ж�ȡһ���Ǹ����� */
int CALLAGREEMENT _read_number(ISFILE *fp)
{
	unsigned char buff[4];			/* �߽���룬ʵ��ֻʹ�õ�һ���ֽ� */
	int number = 0;
	
	/* ����հ׷��ţ�����ACSII��0x9~0xd��0x20��*/
	do {
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;				/* ������������� */
		
		/* ���ע����Ϣ��PNM��ע����Ϣ��#�ſ�ʼ��ֱ����β��*/
		if (buff[0] == '#')
		{
			do
			{
				if (isio_read((void*)buff, 1, 1, fp) == 0)
				{
					return -1;
				}
			} while ((buff[0] != '\r')&&(buff[0] != '\n'));
		}

	} while (isspace((int)(buff[0])));
	
	/* ��ȡ��ֵ */
	do {
		number *= 10;
		number += buff[0]-'0';
		
		if (isio_read((void*)buff, 1, 1, fp) == 0)
		{
			return -1;
		}

	} while (isdigit(buff[0]));
	
	return(number);		/* ���ظ�����-1����ʾ������ȡʧ�� */
}


/* дһ���Ǹ���������markΪ��׺��־��0 ��׺�ո����1��׺ \n�� */
int CALLAGREEMENT _write_number(ISFILE *fp, unsigned int num, int mark)
{
	char buff[32];

	assert(num >= 0);

	_itoa(num, (char*)buff, 10);	/* ��ֲʱӦת��ΪUNIX��Ӧ���� */

	if (mark == 1)
	{
		strcat((char*)buff, (const char *)"\n");
	}
	else
	{
		strcat((char*)buff, (const char *)" ");
	}

	if (isio_write((const void *)buff, strlen(buff), 1, fp) == 0)
	{
		return -1;					/* д�ļ�ʧ�� */
	}
	
	return 0;						/* �ɹ� */
}


/* PNM ����־�б� */
int CALLAGREEMENT _is_pnm_hahaha(char *pmc)
{
	char c;

	assert(pmc != 0);

	if (*pmc++ != 'P')
		return -1;		/* ��PNMͼ�� */

	c = (*pmc)-'0';

	/*
	 * P1	PBM, ascii
	 * P2	PGM, ascii
	 * P3	PPM, ascii
	 * P4	PBM, binary
	 * P5	PGM, binary
	 * P6	PPM, binary
	 */

	if ((c < 1)||(c > 6))
		return -1;		/* ��PNMͼ�� */

	return (int)c;		/* ����PNM��ʽ��ʶ�� 1~6 */
}


