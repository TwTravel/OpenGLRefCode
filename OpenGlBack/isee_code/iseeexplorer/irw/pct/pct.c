/********************************************************************

	pct.c

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
	���ļ���;��	ISeeͼ��������� Apple PICTͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��ɶ�ȡ8��32λPCTͼ�󣬵����е�ʸ���ɷݽ�������
							  
					���湦�ܣ��ݲ�֧��
							   
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net

	���ļ��汾��	20516
	����޸��ڣ�	2002-5-16

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-5		��һ�η������°棩

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

#include "pct.h"


IRWP_INFO			pct_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	pct_get_info_critical;	/* pct_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	pct_load_img_critical;	/* pct_load_image�����Ĺؼ��� */
CRITICAL_SECTION	pct_save_img_critical;	/* pct_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT  _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);

static int CALLAGREEMENT _skip_ign_data(ISFILE *pfile, short opcode);

static int CALLAGREEMENT _read_dword(ISFILE *pfile, long *ps);
static int CALLAGREEMENT _read_word(ISFILE *pfile, short *ps);
static int CALLAGREEMENT _read_byte(ISFILE *pfile, char *ps);
static int CALLAGREEMENT _skip_and_skip(ISFILE *pfile, int n);

static int CALLAGREEMENT _decomp_rle8(unsigned char *, int, unsigned char *, int);
static int CALLAGREEMENT _find_index(LPPCT_COLORSPEC pcol, int len, int index);
static enum EXERESULT CALLAGREEMENT _skip_header(ISFILE *pfile);





#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&pct_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&pct_get_info_critical);
			InitializeCriticalSection(&pct_load_img_critical);
			InitializeCriticalSection(&pct_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&pct_get_info_critical);
			DeleteCriticalSection(&pct_load_img_critical);
			DeleteCriticalSection(&pct_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PCT_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pct_irwp_info;
}

#else

PCT_API LPIRWP_INFO CALLAGREEMENT pct_get_plugin_info()
{
	_init_irwp_info(&pct_irwp_info);

	return (LPIRWP_INFO)&pct_irwp_info;
}

PCT_API void CALLAGREEMENT pct_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PCT_API void CALLAGREEMENT pct_detach_plugin()
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

	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ�������*/
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
				(const char *)"PICT�е���WMF������Щ��PDF :)");
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

	/* ����������##���ֶ���������PCT��ʽ�б�����ο�BMP����б��������ã� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 2;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"PIC");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"PICT");
	
	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
PCT_API int CALLAGREEMENT pct_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCT_PIXMAP		pixmap;
	PCT_RECT		srrect, dcrect;
	short			mode;
	short			stmp;
	long			ltmp;

	int				mark = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&pct_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}

			/* ����ײ�������Ϣ�����Ա�������õ���Ϣ:��*/
			if ((b_status=_skip_header(pfile)) != ER_SUCCESS)
			{
				__leave;
			}

			/* Ԥ��һ��PCT������ */
			if (_read_word(pfile, &stmp))
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡPCT������ */
			while ((int)(unsigned int)(unsigned short)stmp != (int)(unsigned int)(unsigned short)0xffff)
			{
				switch ((int)(unsigned int)(unsigned short)stmp)
				{
				case PCTDEF_PACKBITSRECT:		/* 0x0098 - PackBitsRect����ֻ֧��8λ����ͼ�� */
					/*
					 * �˴���Apple�ĵ�������Щ΢�Ĳ�ͬ���ĵ�����˵�˴�����һ������
					 * ��PixMap�ṹ����ʵ�ⷢ��ͼ����ȱ�ٸýṹ�ײ���baseAddr��Ա��
					 * ���Դ˴���pixmap.rowBytes��������
					 */
					if (isio_read((void*)&(pixmap.rowBytes), sizeof(PCT_PIXMAP)-4, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					
					/* תΪX86���� */
					pixmap.rowBytes   = EXCHANGE_WORD((pixmap.rowBytes));		/* �п�� */
					pixmap.pmVersion  = EXCHANGE_WORD((pixmap.pmVersion));		/* PixMap �汾�� */
					pixmap.packType   = EXCHANGE_WORD((pixmap.packType));		/* ѹ������ */
					pixmap.pixelType  = EXCHANGE_WORD((pixmap.pixelType));		/* ���ظ�ʽ */
					pixmap.pixelSize  = EXCHANGE_WORD((pixmap.pixelSize));		/* ÿ���ص�����λ�� */
					pixmap.cmpCount   = EXCHANGE_WORD((pixmap.cmpCount));		/* ÿ���ص��߼���� */
					pixmap.cmpSize    = EXCHANGE_WORD((pixmap.cmpSize));		/* ÿ��ֵ��߼�λ�� */
					
					pixmap.packSize   = EXCHANGE_DWORD((pixmap.packSize));		/* ѹ����ĳߴ� */
					pixmap.hRes       = EXCHANGE_DWORD((pixmap.hRes));			/* ˮƽ�ֱ��� */
					pixmap.vRes       = EXCHANGE_DWORD((pixmap.vRes));			/* ��ֱ�ֱ��� */
					pixmap.planeBytes = EXCHANGE_DWORD((pixmap.planeBytes));	/* ��һ������ƫ�� */
					pixmap.pmTable    = EXCHANGE_DWORD((pixmap.pmTable));		/* ��ɫ��ṹ�ľ�� */
					pixmap.pmReserved = EXCHANGE_DWORD((pixmap.pmReserved));	/* ����������Ϊ0 */
					
					pixmap.bounds.bottom  = EXCHANGE_WORD((pixmap.bounds.bottom));	/* �߽����� */
					pixmap.bounds.left    = EXCHANGE_WORD((pixmap.bounds.left));
					pixmap.bounds.right   = EXCHANGE_WORD((pixmap.bounds.right));
					pixmap.bounds.top     = EXCHANGE_WORD((pixmap.bounds.top));
					
					_read_dword(pfile, &ltmp);
					_read_word(pfile, &stmp);		/* �����ǩֵ */
					_read_word(pfile, &stmp);		/* ����ɫ������� */
					
					/* ֻ֧��8λ����ͼ�� */
					if ((pixmap.pixelSize != 8)||(pixmap.pixelType != 0)|| \
						(pixmap.cmpCount != 1)||(pixmap.cmpSize != 8))
					{
						b_status = ER_BADIMAGE; __leave;
					}
					if ((unsigned int)stmp > (unsigned int)0xff)
					{
						b_status = ER_BADIMAGE; __leave;
					}
					
					/* �����ɫ������ */
					if (isio_seek(pfile, (stmp+1)*sizeof(PCT_COLORSPEC), SEEK_CUR) == -1)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* ����Դ���κ�Ŀ��������� */
					if (isio_read((void*)&srrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					if (isio_read((void*)&dcrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					
					/* ��ȡת��ģʽ�� */
					_read_word(pfile, &mode);
					
					mark = 1;

					break;
				case PCTDEF_DIRBITSRT:			/* 0x009A - DirectBitsRect ��ֻ֧��32λRGBͼ�� */
					/* ��ȡPixMap�ṹ */
					if (isio_read((void*)&pixmap, sizeof(PCT_PIXMAP), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					if (isio_read((void*)&srrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					if (isio_read((void*)&dcrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* תΪX86���� */
					pixmap.rowBytes   = EXCHANGE_WORD((pixmap.rowBytes));		/* �п�� */
					pixmap.pmVersion  = EXCHANGE_WORD((pixmap.pmVersion));		/* PixMap �汾�� */
					pixmap.packType   = EXCHANGE_WORD((pixmap.packType));		/* ѹ������ */
					pixmap.pixelType  = EXCHANGE_WORD((pixmap.pixelType));		/* ���ظ�ʽ */
					pixmap.pixelSize  = EXCHANGE_WORD((pixmap.pixelSize));		/* ÿ���ص�����λ�� */
					pixmap.cmpCount   = EXCHANGE_WORD((pixmap.cmpCount));		/* ÿ���ص��߼���� */
					pixmap.cmpSize    = EXCHANGE_WORD((pixmap.cmpSize));		/* ÿ��ֵ��߼�λ�� */

					pixmap.baseAddr   = EXCHANGE_DWORD((pixmap.baseAddr));
					pixmap.packSize   = EXCHANGE_DWORD((pixmap.packSize));		/* ѹ����ĳߴ� */
					pixmap.hRes       = EXCHANGE_DWORD((pixmap.hRes));			/* ˮƽ�ֱ��� */
					pixmap.vRes       = EXCHANGE_DWORD((pixmap.vRes));			/* ��ֱ�ֱ��� */
					pixmap.planeBytes = EXCHANGE_DWORD((pixmap.planeBytes));	/* ��һ������ƫ�� */
					pixmap.pmTable    = EXCHANGE_DWORD((pixmap.pmTable));		/* ��ɫ��ṹ�ľ�� */
					pixmap.pmReserved = EXCHANGE_DWORD((pixmap.pmReserved));	/* ����������Ϊ0 */

					pixmap.bounds.bottom  = EXCHANGE_WORD((pixmap.bounds.bottom));	/* �߽����� */
					pixmap.bounds.left    = EXCHANGE_WORD((pixmap.bounds.left));
					pixmap.bounds.right   = EXCHANGE_WORD((pixmap.bounds.right));
					pixmap.bounds.top     = EXCHANGE_WORD((pixmap.bounds.top));

					/* ����Ƿ�����֧�ֵ�ͼ�� */
					if ((pixmap.pixelSize != 32)||(pixmap.pixelType != 16)|| \
						(pixmap.cmpCount != 3)||(pixmap.cmpSize != 8)|| \
						(pixmap.packType != 4))
					{
						b_status = ER_BADIMAGE; __leave;
					}

					/* ��ȡת��ģʽ�� */
					_read_word(pfile, &mode);

					mark = 1;
					break;
				case PCTDEF_DIRBITSRGN:			/* 0x009B - DirectBitsRgn���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				case PCTDEF_BITSRECT:			/* 0x0090 - BitsRect���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				case PCTDEF_BITSRGN:			/* 0x0091 - BitsRgn���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				case PCTDEF_PACKBITSRGN:		/* 0x0099 - PackBitsRgn���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				default:
					/* ʸ����ͼ�����뽫������ */
					if (_skip_ign_data(pfile, stmp))
					{
						b_status = ER_FILERWERR; __leave;
					}
					break;
				}
				
				/* ����ѻ�ȡ��ͼ����Ϣ�����˳�ѭ�� */
				if (mark)
					break;

				/* ����һ�������� */
				if (_read_word(pfile, &stmp))
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			/* PCT��û���ҵ��ɶ���λͼ */
			if (mark == 0)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_PCT;			/* ͼ���ļ���ʽ����׺���� */
			
			pinfo_str->compression  = ICS_RLE8;

			pinfo_str->width	= (unsigned long)(pixmap.bounds.right-pixmap.bounds.left);
			pinfo_str->height	= (unsigned long)(pixmap.bounds.bottom-pixmap.bounds.top);
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= (unsigned long)pixmap.pixelSize;

			/* ��д�������� */
			if (pinfo_str->bitcount == 32)
			{
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
			}
			else
			{
				assert(pinfo_str->bitcount == 8);

				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
			}
			

			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pct_get_info_critical);
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
PCT_API int CALLAGREEMENT pct_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCT_PIXMAP		pixmap;
	PCT_RECT		srrect, dcrect;
	short			mode;
	short			stmp;
	long			ltmp;
	unsigned short	uscount;

	int				pal_count;
	LPPCT_COLORSPEC	pcol = 0;

	unsigned char	*pbuf = 0, *pdec = 0, *pr, *pg, *pb, *p;
	
	int				mark = 0, linesize, i, j, pix_count_len, dec_len, ind;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pct_load_img_critical);

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

			/* ����ײ�������Ϣ�����Ա�������õ���Ϣ:��*/
			if ((b_status=_skip_header(pfile)) != ER_SUCCESS)
			{
				__leave;
			}

			/* Ԥ��һ��PCT������ */
			if (_read_word(pfile, &stmp))
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡPCT������ */
			while ((int)(unsigned int)(unsigned short)stmp != (int)(unsigned int)(unsigned short)0xffff)
			{
				switch ((int)(unsigned int)(unsigned short)stmp)
				{
				case PCTDEF_PACKBITSRECT:		/* 0x0098 - PackBitsRect����ֻ֧��8λ����ͼ�� */
					/*
					 * �˴���Apple�ĵ�������Щ΢�Ĳ�ͬ���ĵ�����˵�˴�����һ������
					 * ��PixMap�ṹ����ʵ�ⷢ��ͼ����ȱ�ٸýṹ�ײ���baseAddr��Ա��
					 * ���Դ˴���pixmap.rowBytes��������
					 */
					if (isio_read((void*)&(pixmap.rowBytes), sizeof(PCT_PIXMAP)-4, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* תΪX86���� */
					pixmap.rowBytes   = EXCHANGE_WORD((pixmap.rowBytes));		/* �п�� */
					pixmap.pmVersion  = EXCHANGE_WORD((pixmap.pmVersion));		/* PixMap �汾�� */
					pixmap.packType   = EXCHANGE_WORD((pixmap.packType));		/* ѹ������ */
					pixmap.pixelType  = EXCHANGE_WORD((pixmap.pixelType));		/* ���ظ�ʽ */
					pixmap.pixelSize  = EXCHANGE_WORD((pixmap.pixelSize));		/* ÿ���ص�����λ�� */
					pixmap.cmpCount   = EXCHANGE_WORD((pixmap.cmpCount));		/* ÿ���ص��߼���� */
					pixmap.cmpSize    = EXCHANGE_WORD((pixmap.cmpSize));		/* ÿ��ֵ��߼�λ�� */
					
					pixmap.packSize   = EXCHANGE_DWORD((pixmap.packSize));		/* ѹ����ĳߴ� */
					pixmap.hRes       = EXCHANGE_DWORD((pixmap.hRes));			/* ˮƽ�ֱ��� */
					pixmap.vRes       = EXCHANGE_DWORD((pixmap.vRes));			/* ��ֱ�ֱ��� */
					pixmap.planeBytes = EXCHANGE_DWORD((pixmap.planeBytes));	/* ��һ������ƫ�� */
					pixmap.pmTable    = EXCHANGE_DWORD((pixmap.pmTable));		/* ��ɫ��ṹ�ľ�� */
					pixmap.pmReserved = EXCHANGE_DWORD((pixmap.pmReserved));	/* ����������Ϊ0 */
					
					pixmap.bounds.bottom  = EXCHANGE_WORD((pixmap.bounds.bottom));	/* �߽����� */
					pixmap.bounds.left    = EXCHANGE_WORD((pixmap.bounds.left));
					pixmap.bounds.right   = EXCHANGE_WORD((pixmap.bounds.right));
					pixmap.bounds.top     = EXCHANGE_WORD((pixmap.bounds.top));

					_read_dword(pfile, &ltmp);
					_read_word(pfile, &stmp);		/* �����ǩֵ */
					_read_word(pfile, &stmp);		/* ����ɫ������� */

					/* ֻ֧��8λ����ͼ�� */
					if ((pixmap.pixelSize != 8)||(pixmap.pixelType != 0)|| \
						(pixmap.cmpCount != 1)||(pixmap.cmpSize != 8))
					{
						b_status = ER_BADIMAGE; __leave;
					}

					if ((unsigned int)stmp > (unsigned int)0xff)
					{
						b_status = ER_BADIMAGE; __leave;
					}

					pal_count = (int)(unsigned int)(unsigned short)stmp + 1;

					/* �����ɫ�����ݻ����� */
					if ((pcol=(LPPCT_COLORSPEC)malloc(sizeof(PCT_COLORSPEC)*pal_count)) == 0)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					/* �����ɫ������ */
					if (isio_read((void*)pcol, sizeof(PCT_COLORSPEC)*pal_count, 1, pfile) == 0)
					{
						b_status = ER_BADIMAGE; __leave;
					}

					/* ����Դ���κ�Ŀ��������� */
					if (isio_read((void*)&srrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					if (isio_read((void*)&dcrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					
					/* ��ȡת��ģʽ�� */
					_read_word(pfile, &mode);
					
					mark = 1;
					
					break;
				case PCTDEF_DIRBITSRT:			/* 0x009A - DirectBitsRect ��ֻ֧��32λRGBͼ�� */
					/* ��ȡPixMap�ṹ */
					if (isio_read((void*)&pixmap, sizeof(PCT_PIXMAP), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					if (isio_read((void*)&srrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					if (isio_read((void*)&dcrect, sizeof(PCT_RECT), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* תΪX86���� */
					pixmap.rowBytes   = EXCHANGE_WORD((pixmap.rowBytes));		/* �п�� */
					pixmap.pmVersion  = EXCHANGE_WORD((pixmap.pmVersion));		/* PixMap �汾�� */
					pixmap.packType   = EXCHANGE_WORD((pixmap.packType));		/* ѹ������ */
					pixmap.pixelType  = EXCHANGE_WORD((pixmap.pixelType));		/* ���ظ�ʽ */
					pixmap.pixelSize  = EXCHANGE_WORD((pixmap.pixelSize));		/* ÿ���ص�����λ�� */
					pixmap.cmpCount   = EXCHANGE_WORD((pixmap.cmpCount));		/* ÿ���ص��߼���� */
					pixmap.cmpSize    = EXCHANGE_WORD((pixmap.cmpSize));		/* ÿ��ֵ��߼�λ�� */

					pixmap.baseAddr   = EXCHANGE_DWORD((pixmap.baseAddr));
					pixmap.packSize   = EXCHANGE_DWORD((pixmap.packSize));		/* ѹ����ĳߴ� */
					pixmap.hRes       = EXCHANGE_DWORD((pixmap.hRes));			/* ˮƽ�ֱ��� */
					pixmap.vRes       = EXCHANGE_DWORD((pixmap.vRes));			/* ��ֱ�ֱ��� */
					pixmap.planeBytes = EXCHANGE_DWORD((pixmap.planeBytes));	/* ��һ������ƫ�� */
					pixmap.pmTable    = EXCHANGE_DWORD((pixmap.pmTable));		/* ��ɫ��ṹ�ľ�� */
					pixmap.pmReserved = EXCHANGE_DWORD((pixmap.pmReserved));	/* ����������Ϊ0 */

					pixmap.bounds.bottom  = EXCHANGE_WORD((pixmap.bounds.bottom));/* �߽����� */
					pixmap.bounds.left    = EXCHANGE_WORD((pixmap.bounds.left));
					pixmap.bounds.right   = EXCHANGE_WORD((pixmap.bounds.right));
					pixmap.bounds.top     = EXCHANGE_WORD((pixmap.bounds.top));

					/* ��ȡת��ģʽ�� */
					_read_word(pfile, &mode);

					mark = 1;

					break;
				case PCTDEF_DIRBITSRGN:			/* 0x009B - DirectBitsRgn���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				case PCTDEF_BITSRECT:			/* 0x0090 - BitsRect���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				case PCTDEF_BITSRGN:			/* 0x0091 - BitsRgn���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				case PCTDEF_PACKBITSRGN:		/* 0x0099 - PackBitsRgn���ݲ�֧�� */
					b_status = ER_BADIMAGE; __leave; break;
				default:
					/* ʸ����ͼ�����뽫������ */
					if (_skip_ign_data(pfile, stmp))
					{
						b_status = ER_FILERWERR; __leave;
					}
					break;
				}
				
				/* ����ѻ�ȡ��ͼ����Ϣ�����˳�ѭ�� */
				if (mark)
					break;

				/* ����һ�������� */
				if (_read_word(pfile, &stmp))
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			/* PCT��û��λͼ */
			if (mark == 0)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_PCT;			/* ͼ���ļ���ʽ����׺���� */
				
				pinfo_str->compression  = ICS_RLE8;
				
				pinfo_str->width	= (unsigned long)(pixmap.bounds.right-pixmap.bounds.left);
				pinfo_str->height	= (unsigned long)(pixmap.bounds.bottom-pixmap.bounds.top);
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= (unsigned long)pixmap.pixelSize;
				
				if (pinfo_str->bitcount == 32)
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else
				{
					assert(pinfo_str->bitcount == 8);

					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					pinfo_str->a_mask = 0x0;
				}
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}
			
			/* ���õ�ɫ������ */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

				/* Դͼ���е�ɫ���������� */
				if ((int)pinfo_str->pal_count < pal_count)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* ����ɫ������תΪX86���� */
				for (i=0; i<pal_count; i++)
				{
					pcol[i].value     = EXCHANGE_WORD((pcol[i].value));
					pcol[i].rgb.blue  = EXCHANGE_WORD((pcol[i].rgb.blue));
					pcol[i].rgb.green = EXCHANGE_WORD((pcol[i].rgb.green));
					pcol[i].rgb.red   = EXCHANGE_WORD((pcol[i].rgb.red));
				}

				/* ��д��ɫ������ */
				for (i=0; i<pal_count; i++)
				{
					ind = _find_index(pcol, pal_count, i);

					if (ind == -1)
					{
						pinfo_str->palette[i] = 0x0;
					}
					else
					{
						pinfo_str->palette[i] = (((unsigned long)(pcol[ind].rgb.red>>8))<<16)  | \
												(((unsigned long)(pcol[ind].rgb.green>>8))<<8) | \
												(((unsigned long)(pcol[ind].rgb.blue>>8))<<0);
					}
				}
			}
			else
			{
				assert(pinfo_str->bitcount == 32);

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
			
			/* ��д���׵�ַ���� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}

			/* �����ѹ������ */
			if ((pbuf=(unsigned char *)malloc(linesize*2+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			if ((pdec=(unsigned char *)malloc(linesize+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ÿһѹ�����ײ��������Ŀ�ȣ��ֽڣ�*/
			switch (pixmap.pixelType)
			{
			case	0:		/* ����ͼ */
				pix_count_len = 1;
				dec_len = (int)pinfo_str->width;
				break;
			case	16:		/* 32λRGBͼ */
				pix_count_len = 2;
				dec_len = (int)pinfo_str->width * 3;
				break;
			default:
				b_status = ER_BADIMAGE; __leave; break;
			}

			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			/* ���ж����������� */
			for (i=0; i<(int)(pinfo_str->height);i++)
			{
				uscount = 0; 
				p = (unsigned char *)pinfo_str->p_bit_data+i*linesize;

				/* ��ѹ���г��� */
				if (isio_read((void*)&uscount, pix_count_len, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ���Ϊ2�ֽڵ�ѹ���г���ֵ����ת������ */
				if (pix_count_len == 2)
				{
					uscount = EXCHANGE_WORD(uscount);
				}

				/* ��һ�е�ѹ������ */
				if (isio_read((void*)pbuf, uscount, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ��ѹһ�����ݵ�pdec������ */
				if (_decomp_rle8(pbuf, (int)(unsigned int)uscount, pdec, dec_len))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* �ϳ����ص����ݰ� */
				if (pinfo_str->bitcount == 32)
				{
					pr = pdec;
					pg = pr+pinfo_str->width;
					pb = pg+pinfo_str->width;

					for (j=0; j<(int)pinfo_str->width; j++)
					{
						*p++ = *pb++;
						*p++ = *pg++;
						*p++ = *pr++;
						*p++ = (unsigned char)0;
					}
				}
				else
				{
					memmove((void*)p, (const void *)pdec, pinfo_str->width);
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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
			}

			if (pcol)
				free(pcol);

			if (pdec)
				free(pdec);

			if (pbuf)
				free(pbuf);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pct_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
PCT_API int CALLAGREEMENT pct_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&pct_save_img_critical);
	
			b_status = ER_NOTSUPPORT;

			/* ------------------------------------------------------------>
			   YZ:
				   PICT��ʽ����ݲ�ʵ�ֱ��湦�ܣ�PS���Խ�ͼ�󱣴�ΪPCT��ʽ��
			*/
			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			LeaveCriticalSection(&pct_save_img_critical);
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


/* �����޷�����Ĳ����� */
static int CALLAGREEMENT _skip_ign_data(ISFILE *pfile, short opcode)
{
	int		result = 0;
	long	ltmp;
	short	stmp;
	char	btmp;

	__try
	{
		switch ((int)(unsigned int)(unsigned short)opcode)
		{
		case PCTDEF_NOP:				/* No operation 0 */
		case PCTDEF_REV0017:			/* Reserved for Apple use Not determined */
		case PCTDEF_REV0018:			/* Reserved for Apple use Not determined */
		case PCTDEF_REV0019:			/* Reserved for Apple use Not determined */
		case PCTDEF_HILITEMODE:			/* HiliteMode Highlight mode flag: no data; this opcode is sent before a drawing operation that uses the highlight mode 0 */
		case PCTDEF_DEFHILITE:			/* DefHilite Use default highlight color; no data; set highlight to default (from low memory) 0 */
		case PCTDEF_FRAMESAMERT:		/* frameSameRect Rectangle (Rect) 0 */
		case PCTDEF_PAINTSAMERT:		/* paintSameRect Rectangle (Rect) 0 */
		case PCTDEF_ERASESAMERT:		/* eraseSameRect Rectangle (Rect) 0 */
		case PCTDEF_INVERTSAMERT:		/* invertSameRect Rectangle (Rect) 0 */
		case PCTDEF_FILLSAMERECT:		/* fillSameRect Rectangle (Rect) 0 */
		case PCTDEF_REV003D:			/* Reserved for Apple use 0 */
		case PCTDEF_REV003E:			/* Reserved for Apple use 0 */
		case PCTDEF_REV003F:			/* Reserved for Apple use 0 */
		case PCTDEF_FRAMESAMERRT:		/* frameSameRRect Rectangle (Rect) 0 */
		case PCTDEF_PAINTSAMERRT:		/* paintSameRRect Rectangle (Rect) 0 */
		case PCTDEF_ERASESAMERRT:		/* eraseSameRRect Rectangle (Rect) 0 */
		case PCTDEF_INVSAMERRT:			/* invertSameRRect Rectangle (Rect) 0 */
		case PCTDEF_FILLSAMERRT:		/* fillSameRRect Rectangle (Rect) 0 */
		case PCTDEF_REV004D:			/* Reserved for Apple use 0 */
		case PCTDEF_REV004E:			/* Reserved for Apple use 0 */
		case PCTDEF_REV004F:			/* Reserved for Apple use 0 */
		case PCTDEF_FRAMESAMEOV:		/* frameSameOval Rectangle (Rect) 0 */
		case PCTDEF_PAINTSAMEOV:		/* paintSameOval Rectangle (Rect) 0 */
		case PCTDEF_ERASESAMEOV:		/* eraseSameOval Rectangle (Rect) 0 */
		case PCTDEF_INVERTSAMEOV:		/* invertSameOval Rectangle (Rect) 0 */
		case PCTDEF_FILLSAMEOV:			/* fillSameOval Rectangle (Rect) 0 */
		case PCTDEF_REV005D:			/* Reserved for Apple use 0 */
		case PCTDEF_REV005E:			/* Reserved for Apple use 0 */
		case PCTDEF_REV005F:			/* Reserved for Apple use 0 */
		case PCTDEF_FRAMESAMEPL:		/* frameSamePoly (Not yet implemented) 0 */
		case PCTDEF_PAINTSAMEPL:		/* paintSamePoly (Not yet implemented) 0 */
		case PCTDEF_ERASESAMEPL:		/* eraseSamePoly (Not yet implemented) 0 */
		case PCTDEF_INVERTSAMEPL:		/* invertSamePoly (Not yet implemented) 0 */
		case PCTDEF_FILLSAMEPL:			/* fillSamePoly (Not yet implemented) 0 */
		case PCTDEF_REV007D:			/* Reserved for Apple use 0 */
		case PCTDEF_REV007E:			/* Reserved for Apple use 0 */
		case PCTDEF_REV007F:			/* Reserved for Apple use 0 */
		case PCTDEF_FRAMESAMERGN:		/* frameSameRgn (Not yet implemented) 0 */
		case PCTDEF_PAINTSAMERGN:		/* paintSameRgn (Not yet implemented) 0 */
		case PCTDEF_ERASESAMERGN:		/* eraseSameRgn (Not yet implemented) 0 */
		case PCTDEF_INVSAMERGN:			/* invertSameRgn (Not yet implemented) 0 */
		case PCTDEF_FILLSAMERGN:		/* fillSameRgn (Not yet implemented) 0 */
		case PCTDEF_REV008D:			/* Reserved for Apple use 0 */
		case PCTDEF_REV008E:			/* Reserved for Apple use 0 */
		case PCTDEF_REV008F:			/* Reserved for Apple use 0 */
			break;
		case PCTDEF_CLIP:				/* Clipping region Region size */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned short)stmp-2)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_BKPAT:				/* BkPat Background pattern 8 */
		case PCTDEF_PNPAT:				/* PnPat Pen pattern 8 */
		case PCTDEF_FILLPAT:			/* FillPat Fill pattern 8 */
		case PCTDEF_TXRATIO:			/* TxRatio Numerator (Point), denominator (Point) 8 */
		case PCTDEF_LINE:				/* Line pnLoc (Point), newPt (Point) 8 */
		case PCTDEF_GLYPHSTATE:			/* glyphState Data length (word), followed by these 1-byte Boolean values: outline preferred, preserve glyph, fractional widths, scaling disabled 8 */
		case PCTDEF_FRAMERECT:			/* frameRect Rectangle (Rect) 8 */
		case PCTDEF_PAINTRECT:			/* paintRect Rectangle (Rect) 8 */
		case PCTDEF_ERASERECT:			/* eraseRect Rectangle (Rect) 8 */
		case PCTDEF_INVERTRECT:			/* invertRect Rectangle (Rect) 8 */
		case PCTDEF_FILLRECT:			/* fillRect Rectangle (Rect) 8 */
		case PCTDEF_REV0035:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_REV0036:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_REV0037:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_FRAMERRECT:			/* frameRRect Rectangle (Rect)� 8 */
		case PCTDEF_PAINTRRECT:			/* paintRRect Rectangle (Rect)� 8 */
		case PCTDEF_ERASERRECT:			/* eraseRRect Rectangle (Rect)� 8 */
		case PCTDEF_INVERTRRECT:		/* invertRRect Rectangle (Rect)� 8 */
		case PCTDEF_FILLRRECT:			/* fillRRect Rectangle (Rect)� 8 */
		case PCTDEF_REV0045:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_REV0046:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_REV0047:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_FRAMEOVAL:			/* frameOval Rectangle (Rect) 8 */
		case PCTDEF_PAINTOVAL:			/* paintOval Rectangle (Rect) 8 */
		case PCTDEF_ERASEOVAL:			/* eraseOval Rectangle (Rect) 8 */
		case PCTDEF_INVERTOVAL:			/* invertOval Rectangle (Rect) 8 */
		case PCTDEF_FILLOVAL:			/* fillOval Rectangle (Rect) 8 */
		case PCTDEF_REV0055:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_REV0056:			/* Reserved for Apple use 8 bytes of data 8 */
		case PCTDEF_REV0057:			/* Reserved for Apple use 8 bytes of data 8 */
			if ((result=_skip_and_skip(pfile, 8)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_HILITECOLOR:		/* HiliteColor Highlight color (RGBColor) 6 */
		case PCTDEF_RGBBKCOL:			/* RGBBkCol Background color (RGBColor) 6 */
		case PCTDEF_OPCOLOR:			/* OpColor Opcolor (RGBColor) 6 */
		case PCTDEF_SHORTLINE:			/* ShortLine pnLoc (Point), dh (�128..127), dv (�128..127) 6 */
		case PCTDEF_RGBFGCOL:			/* RGBFgCol Foreground color (RGBColor) 6 */
			if ((result=_skip_and_skip(pfile, 6)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_SPEXTRA:			/* SpExtra Extra space (Fixed) 4 */
		case PCTDEF_PNSIZE:				/* PnSize Pen size (Point) 4 */
		case PCTDEF_OVSIZE:				/* OvSize Oval size (Point) 4 */
		case PCTDEF_ORIGIN:				/* Origin dh, dv (Integer) 4 */
		case PCTDEF_FGCOLOR:			/* FgColor Foreground color (Long) 4 */
		case PCTDEF_BKCOLOR:			/* BkColor Background color (Long) 4 */
		case PCTDEF_LINEFROM:			/* LineFrom newPt (Point) 4 */
		case PCTDEF_FRAMESAMEARC:		/* frameSameArc Rectangle (Rect) 4 */
		case PCTDEF_PAINTSAMEARC:		/* paintSameArc Rectangle (Rect) 4 */
		case PCTDEF_ERASESAMEARC:		/* eraseSameArc Rectangle (Rect) 4 */
		case PCTDEF_INVSAMEARC:			/* invertSameArc Rectangle (Rect) 4 */
		case PCTDEF_FILLSAMEARC:		/* fillSameArc Rectangle (Rect) 4 */
		case PCTDEF_REV006D:			/* Reserved for Apple use 4 bytes of data 4 */
		case PCTDEF_REV006E:			/* Reserved for Apple use 4 bytes of data 4 */
		case PCTDEF_REV006F:			/* Reserved for Apple use 4 bytes of data 4 */
			if ((result=_skip_and_skip(pfile, 4)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_TXFONT:				/* TxFont Font number for text (Integer) 2 */
		case PCTDEF_TXMODE:				/* TxMode Source mode (Integer) 2 */
		case PCTDEF_PNMODE:				/* PnMode Pen mode (Integer) 2 */
		case PCTDEF_TXSIZE:				/* TxSize Text size (Integer) 2 */
		case PCTDEF_PNLOCHFRAC:			/* PnLocHFrac Fractional pen position (Integer�low word of Fixed); 2*/
		case PCTDEF_CHEXTRA:			/* ChExtra Added width for nonspace characters (Integer) 2*/
		case PCTDEF_SHORTLINEFR:		/* ShortLineFrom dh (�128..127), dv (�128..127) 2 */
		case PCTDEF_SHORTCOMMENT:		/* ShortComment Kind (Integer) 2 */
		case PCTDEF_VERSION:			/* Version Version number of picture 2 */
			if ((result=_skip_and_skip(pfile, 2)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_TXFACE:				/* TxFace Text's font style (0..255) 1 */
		case PCTDEF_VERSIONOP:			/* VersionOp Version (0..255) 1 */
			if ((result=_skip_and_skip(pfile, 1)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_BKPIXPAT:			/* BkPixPat Background pixel pattern Variable; see Listing A-1 on page A-17 */
			break;
		case PCTDEF_PNPIXPAT:			/* PnPixPat Pen pixel pattern Variable; see Listing A-1 on page A-17 */
			break;
		case PCTDEF_FILLPIXPAT:			/* FillPixPat Fill pixel pattern Variable; see Listing A-1 on page A-17 */
			break;
		case PCTDEF_REV0024:			/* Reserved for Apple use Data length (Integer), data 2 + data length */
		case PCTDEF_REV0025:			/* Reserved for Apple use Data length (Integer), data 2 + data length */
		case PCTDEF_REV0026:			/* Reserved for Apple use Data length (Integer), data 2 + data length */
		case PCTDEF_REV0027:			/* Reserved for Apple use Data length (Integer), data 2 + data length */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, stmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_LONGTEXT:			/* LongText txLoc (Point), count (0..255), text 5 + text */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned char)btmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_DHTEXT:				/* DHText dh (0..255), count (0..255), text 2 + text */
		case PCTDEF_DVTEXT:				/* DVText dv (0..255), count (0..255), text 2 + text */
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned char)btmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_DHDVTEXT:			/* DHDVText dh (0..255), dv (0..255), count (0..255), text 3 + text */
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned char)btmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_FONTNAME:			/* fontName Data length (Integer), old font ID (Integer), name length (0..255), font name* 5 + name length */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_byte(pfile, &btmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned char)btmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_LINEJUSTIFY:		/* lineJustify Operand data length (Integer), intercharacter spacing (Fixed), total extra space for justification (Fixed) 10 */
			if ((result=_skip_and_skip(pfile, 10)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_REV002F:			/* Reserved for Apple use Data length (Integer), data 2 + data length */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned short)stmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_FRAMEARC:			/* frameArc Rectangle (Rect), startAngle, arcAngle 12 */
		case PCTDEF_PAINTARC:			/* paintArc Rectangle (Rect), startAngle, arcAngle 12 */
		case PCTDEF_ERASEARC:			/* eraseArc Rectangle (Rect), startAngle, arcAngle 12 */
		case PCTDEF_INVERARC:			/* invertArc Rectangle (Rect), startAngle,arcAngle 12 */
		case PCTDEF_FILLARC:			/* fillArc Rectangle (Rect), startAngle, arcAngle 12 */
		case PCTDEF_REV0065:			/* Reserved for Apple use 12 bytes of data 12 */
		case PCTDEF_REV0066:			/* Reserved for Apple use 12 bytes of data 12 */
		case PCTDEF_REV0067:			/* Reserved for Apple use 12 bytes of data 12 */
			if ((result=_skip_and_skip(pfile, 12)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_FRAMEPOLY:			/* framePoly Polygon (Poly) Polygon size */
		case PCTDEF_PAINTPOLY:			/* paintPoly Polygon (Poly) Polygon size */
		case PCTDEF_ERASEPOLY:			/* erasePoly Polygon (Poly) Polygon size */
		case PCTDEF_INVERTPOLY:			/* invertPoly Polygon (Poly) Polygon size */
		case PCTDEF_FILLPOLY:			/* fillPoly Polygon (Poly) Polygon size */
		case PCTDEF_REV0075:			/* Reserved for Apple use Polygon (Poly) Polygon size */
		case PCTDEF_REV0076:			/* Reserved for Apple use Polygon (Poly) Polygon size */
		case PCTDEF_REV0077:			/* Reserved for Apple use Polygon (Poly) Polygon size */
			if ((result=_read_word(pfile, &stmp))!= 0)
			{
				__leave;
			}
			ltmp = (long)(int)(unsigned int)(unsigned short)stmp;
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)ltmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_FRAMERGN:			/* frameRgn Region (Rgn) Region size */
		case PCTDEF_PAINTRGN:			/* paintRgn Region (Rgn) Region size */
		case PCTDEF_ERASERGN:			/* eraseRgn Region (Rgn) Region size */
		case PCTDEF_INVERTRGN:			/* invertRgn Region (Rgn) Region size */
		case PCTDEF_FILLRGN:			/* fillRgn Region (Rgn) Region size */
		case PCTDEF_REV0085:			/* Reserved for Apple use Region (Rgn) Region size */
		case PCTDEF_REV0086:			/* Reserved for Apple use Region (Rgn) Region size */
		case PCTDEF_REV0087:			/* Reserved for Apple use Region (Rgn) Region size */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			ltmp = (long)(int)(unsigned int)(unsigned short)stmp;
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)ltmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_BITSRECT:			/* 0x0090 - BitsRect CopyBits with clipped rectangle Variable��; see Listing A-2 on page A-17 */
		case PCTDEF_PACKBITSRECT:		/* 0x0098 - PackBitsRect Packed CopyBits with clipped rectangle Variable�; see Listing A-2 on page A-17 */
		case PCTDEF_BITSRGN:			/* 0x0091 - BitsRgn CopyBits with clipped region Variable��; see Listing A-3 on page A-18 */
		case PCTDEF_PACKBITSRGN:		/* 0x0099 - PackBitsRgn Packed CopyBits with clipped rectangle Variable�; see Listing A-3 on page A-18 */
		case PCTDEF_DIRBITSRT:			/* 0x009A - DirectBitsRect PixMap, srcRect, dstRect, mode (Integer), PixData Variable */
		case PCTDEF_DIRBITSRGN:			/* 0x009B - DirectBitsRgn PixMap, srcRect, dstRect, mode (Integer), maskRgn, PixData Variable */
			assert(0);					/* ��Щ������Ӧ�ɵ�����ȥ��������Ӧ�ó��������� */
			break;
		case PCTDEF_REV0092:			/* Reserved for Apple use Data length (Integer), data 2 + data length */
		case PCTDEF_REV0093:
		case PCTDEF_REV0094:
		case PCTDEF_REV0095:
		case PCTDEF_REV0096:
		case PCTDEF_REV0097:
		case PCTDEF_REV009C:
		case PCTDEF_REV009D:
		case PCTDEF_REV009E:
		case PCTDEF_REV009F:
		case PCTDEF_REV00A2:
		case PCTDEF_REV00A3:
		case PCTDEF_REV00A4:
		case PCTDEF_REV00A5:
		case PCTDEF_REV00A6:
		case PCTDEF_REV00A7:
		case PCTDEF_REV00A8:
		case PCTDEF_REV00A9:
		case PCTDEF_REV00AA:
		case PCTDEF_REV00AB:
		case PCTDEF_REV00AC:
		case PCTDEF_REV00AD:
		case PCTDEF_REV00AE:
		case PCTDEF_REV00AF:
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned short)stmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_LONGCOMMENT:		/* LongComment Kind (Integer), size (Integer), data 4 + data */
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_read_word(pfile, &stmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)(unsigned int)(unsigned short)stmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_OPENDPIC:			/* OpEndPic End of picture 2 */
										/* �����뽫�������� */
			break;
		case PCTDEF_HEADEROP:			/* HeaderOp For extended version 2: version (Integer), reserved (Integer), hRes, vRes (Fixed), srcRect, reserved (Long); for version 2: opcode 24 */
			if ((result=_skip_and_skip(pfile, 24)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_COMPQT:				/* CompressedQuickTime Data length (Long), data (private to	QuickTime) 4 + data length */
			if ((result=_read_dword(pfile, &ltmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)ltmp)) != 0)
			{
				__leave;
			}
			break;
		case PCTDEF_UNCOMPQT:			/* UncompressedQuickTime Data length (Long), data (private to QuickTime) 4 + data length */
			if ((result=_read_dword(pfile, &ltmp)) != 0)
			{
				__leave;
			}
			if ((result=_skip_and_skip(pfile, (int)ltmp)) != 0)
			{
				__leave;
			}
			break;
		default:
			{
				if (((int)(unsigned int)(unsigned short)opcode >= PCTDEF_REV00B0)&& \
					((int)(unsigned int)(unsigned short)opcode <= PCTDEF_REV00CF))
				{
					break;
				}
				else if (((int)(unsigned int)(unsigned short)opcode >= PCTDEF_REV00D0)&& \
					((int)(unsigned int)(unsigned short)opcode <= PCTDEF_REV00FE))
				{
					if ((result=_read_dword(pfile, &ltmp)) != 0)
					{
						__leave;
					}
					if ((result=_skip_and_skip(pfile, (int)ltmp)) != 0)
					{
						__leave;
					}
					break;
				}
				else if (((int)(unsigned int)(unsigned short)opcode >= PCTDEF_REV0100)&& \
					((int)(unsigned int)(unsigned short)opcode <= PCTDEF_REV01FF))
				{
					if ((result=_skip_and_skip(pfile, 2)) != 0)
					{
						__leave;
					}
					break;
				}
				else if (((int)(unsigned int)(unsigned short)opcode >= PCTDEF_REV0200)&& \
					((int)(unsigned int)(unsigned short)opcode <= PCTDEF_REV7FFF))
				{
					if ((result=_skip_and_skip(pfile, (((int)(unsigned int)(unsigned short)opcode)>>8)*2)) != 0)
					{
						__leave;
					}
					break;
				}
				else if (((int)(unsigned int)(unsigned short)opcode >= (int)(unsigned int)(unsigned short)PCTDEF_REV8000)&& \
					((int)(unsigned int)(unsigned short)opcode <= (int)(unsigned int)(unsigned short)PCTDEF_REV80FF))
				{
					break;
				}
				else if (((int)(unsigned int)(unsigned short)opcode >= (int)(unsigned int)(unsigned short)PCTDEF_REV8100)&& \
					((int)(unsigned int)(unsigned short)opcode <= (int)(unsigned int)(unsigned short)PCTDEF_REVFFFF))
				{
					if ((result=_read_dword(pfile, &ltmp)) != 0)
					{
						__leave;
					}
					if ((result=_skip_and_skip(pfile, (int)ltmp)) != 0)
					{
						__leave;
					}
					break;
				}
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
			result = -1;		/* ����д�� */
	}
	
	return result;
}


/* ��ȡһ��˫�֣�dword����תΪX86����*/
static int CALLAGREEMENT _read_dword(ISFILE *pfile, long *ps)
{
	if (isio_read((void*)ps, 4, 1, pfile) == 0)
	{
		return -1;		/* ��д�� */
	}

	*ps = EXCHANGE_DWORD((*ps));

	return 0;			/* ��ȡ�ɹ� */
}


/* ��ȡһ���֣�word����תΪX86����*/
static int CALLAGREEMENT _read_word(ISFILE *pfile, short *ps)
{
	if (isio_read((void*)ps, 2, 1, pfile) == 0)
	{
		return -1;		/* ��д�� */
	}
	
	*ps = EXCHANGE_WORD((*ps));
	
	return 0;			/* ��ȡ�ɹ� */
}


/* ��ȡһ���ֽڣ�byte��*/
static int CALLAGREEMENT _read_byte(ISFILE *pfile, char *ps)
{
	if (isio_read((void*)ps, 1, 1, pfile) == 0)
	{
		return -1;		/* ��д�� */
	}
	
	return 0;			/* ��ȡ�ɹ� */
}


/* ����дλ������ n ���ֽ� */
static int CALLAGREEMENT _skip_and_skip(ISFILE *pfile, int n)
{
	if (isio_seek(pfile, n, SEEK_CUR) == -1)
	{
		return -1;		/* ��λ���� */
	}

	return 0;
}


/* ����һ��RLE8���ݵ�ָ�������� */
static int CALLAGREEMENT _decomp_rle8(unsigned char *psrc, int src_len, unsigned char *pdec, int dec_len)
{
	unsigned char	count, data;

	assert(psrc&&pdec&&src_len&&dec_len);

	while (src_len)
	{
		count = *psrc++; src_len--;			/* ȡ������ֵ������0x7f��Ϊѹ�����ݼ��� */
											/* ��֮����δѹ�����ݼ��� */

		if (src_len == 0)
		{
			return -1;						/* Դ���ݲ���ȷ */
		}

		if (count > (unsigned char)0x7f)	/* �ظ����� */
		{
			data = *psrc++; src_len--;		/* ȡ��ѹ���ĵ��ֽ����� */

			count = (unsigned char)257 - count;

			if ((int)(unsigned int)count > dec_len)
			{
				return -1;					/* Դ���ݲ���ȷ */
			}

			while (count--)					/* д������ */
			{
				*pdec++ = data; dec_len--;
			}
		}
		else								/* δѹ������ */
		{
			count++;

			if ((int)(unsigned int)count > dec_len)
			{
				return -1;
			}
			if ((int)(unsigned int)count > src_len)
			{
				return -1;
			}

			while (count--)
			{
				*pdec++ = *psrc++; src_len--; dec_len--;
			}
		}
	}

	if (dec_len != 0)
	{
		return -1;
	}
	else
	{
		return 0;	/* ����ɹ� */
	}
}


/* ����ָ�������������±� */
static int CALLAGREEMENT _find_index(LPPCT_COLORSPEC pcol, int len, int index)
{
	int ind = -1, i;

	assert(pcol&&len);

	if (index > len)
	{
		return ind;
	}
	else if (pcol[index].value == index)
	{
		return index;
	}
	else
	{
		for (i=0; i<len; i++)
		{
			if (pcol[i].value == (short)index)
			{
				ind = i; break;
			}
		}
	}

	return ind;	/* -1 ��ʾ��Ч�����������±� */
}


/* ���PCTͼ���ײ���������Ϣ�� */
static enum EXERESULT CALLAGREEMENT _skip_header(ISFILE *pfile)
{
	PCT_MAGIC		magic;

	short			stmp;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile);

	__try
	{
		/* �����ͷ���հ�����PS������PCTͼ����д˿հ���������Ϊ512�ֽ�
		 * ������Ϊȫ0������Apple���ĵ��в�û�б��������һ������������
		 * �ĵ��Բ��ϵ�����ڻ��кܶࡣ
		*/
		if (isio_seek(pfile, 0x200, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* ��ȡͷ�ṹ */
		if (isio_read((void*)&magic, sizeof(PCT_MAGIC), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* תΪX86���� */
		magic.size   = EXCHANGE_WORD((magic.size));
		magic.top    = EXCHANGE_WORD((magic.top));
		magic.left   = EXCHANGE_WORD((magic.left));
		magic.bottom = EXCHANGE_WORD((magic.bottom));
		magic.right  = EXCHANGE_WORD((magic.right));

		/* �����ּ������µ�����word��ֵ��PICT��չ�汾2���еģ������
		 * ��ֻ֧��PICT����չ�汾2�������汾������֧�֡�
		 */
		if (_read_word(pfile, &stmp))			/* 0x0011 */
		{
			b_status = ER_BADIMAGE; __leave;
		}
		if (stmp != 0x11)
		{
			b_status = ER_BADIMAGE; __leave;
		}

		if (_read_word(pfile, &stmp))			/* 0x02ff */
		{
			b_status = ER_BADIMAGE; __leave;
		}
		if (stmp != 0x2ff)
		{
			b_status = ER_BADIMAGE; __leave;
		}

		if (_read_word(pfile, &stmp))			/* 0x0c00 */
		{
			b_status = ER_BADIMAGE; __leave;
		}
		if (stmp != 0xc00)
		{
			b_status = ER_BADIMAGE; __leave;
		}

		/* ����ͼ����Ϣ�飨24�ֽڳ����ÿ���û���ṩ��λͼ��������Ϣ��*/
		if (_skip_and_skip(pfile, 24))
		{
			b_status = ER_FILERWERR; __leave;
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			b_status = ER_FILERWERR;
		}
	}

	return b_status;
}


