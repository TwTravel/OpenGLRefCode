/********************************************************************

	tga.c

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
	���ļ���;��	ISeeͼ���������TGAͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�δѹ����8��15��16��24��32λͼ�񣬼�
							����RLEѹ����8��15��16��24��32λͼ��
					���湦�ܣ�δѹ����8��15��16��24��32λͼ��

	���ļ���д�ˣ�	���¶�			xiaoyueer##263.net
					YZ				yzfree##sina.com

	���ļ��汾��	11225
	����޸��ڣ�	2001-12-25

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2001-12		�����°���루�ӿ���ͼ���ȡ�ٶȣ�������
					�˱���ͼ��Ĺ��ܣ�

		2001-3		Ϊ���ģ���ͼ���ٶȶ��޸Ĵ���
		2000-8		��һ�����԰淢��

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

#include "tga.h"


IRWP_INFO			tga_irwp_info;			/* �����Ϣ�� */
unsigned char		*tga_save_idinfo = "ISeeExplorer";
											/* ����ͼ��ʱע��ı�־����Ϣ */

#ifdef WIN32
CRITICAL_SECTION	tga_get_info_critical;	/* tga_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	tga_load_img_critical;	/* tga_load_image�����Ĺؼ��� */
CRITICAL_SECTION	tga_save_img_critical;	/* tga_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);

int	CALLAGREEMENT _decoding_line(unsigned char *pdest,unsigned char *psrc,int rleunit,int destlen);
int CALLAGREEMENT _get_mask(LPTGAHEADER pInfo, unsigned long * pb_mask, unsigned long * pg_mask, unsigned long * pr_mask, unsigned long * pa_mask);
int CALLAGREEMENT _calcu_scanline_size(int w, int bit);
int CALLAGREEMENT _tga_is_valid_img(LPINFOSTR pinfo_str);
							

#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&tga_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&tga_get_info_critical);
			InitializeCriticalSection(&tga_load_img_critical);
			InitializeCriticalSection(&tga_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&tga_get_info_critical);
			DeleteCriticalSection(&tga_load_img_critical);
			DeleteCriticalSection(&tga_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

TGA_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&tga_irwp_info;
}

#else

TGA_API LPIRWP_INFO CALLAGREEMENT tga_get_plugin_info()
{
	_init_irwp_info(&tga_irwp_info);

	return (LPIRWP_INFO)&tga_irwp_info;
}

TGA_API void CALLAGREEMENT tga_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

TGA_API void CALLAGREEMENT tga_detach_plugin()
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
	/* ## ��λ����һ������32λ��ʽ�޷���ʾ��*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(8-1)) | \
						(1UL<<(16-1)) | (1UL<<(24-1)) | (1UL<<(32-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1��ֻ�ܱ���һ��ͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 2;


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"���¶�");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"xiaoyueer##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"�Ұ��㣬��һֱ��Ŭ����");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"����Ҫ����İ湤�����������¶�����������:)");
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
TGA_API int CALLAGREEMENT tga_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	TGAHEADER		tgaHeader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&tga_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}

			/* ��ȡ������ */
			stream_length = isio_length(pfile);
			
			/* ��ȡ�ļ�ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&tgaHeader, sizeof(TGAHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж��ļ�ͷ��ͼ�����ͱ�־ */
			switch(tgaHeader.bImageType)
			{
			case	TGA_RLEPAL:
			case	TGA_RLECOLOR:
			case	TGA_RLEGRAY:
				/* ��Ч��ͼ��������Ӧ����ͷ�ṹ�ߴ磨�����жϣ� */
				if (stream_length < sizeof(TGAHEADER))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				break;		
			case	TGA_UNCPSPAL:
			case	TGA_UNCPSCOLOR:
			case	TGA_UNCPSGRAY:
				/* δѹ��ͼ��ɼ����ͼ�����ݵĳߴ磬�����жϵ��ܸ���ȷһЩ */
				if (stream_length < (sizeof(TGAHEADER)+tgaHeader.bID_Length+((tgaHeader.bBits+7)>>3)*tgaHeader.wDepth*tgaHeader.wWidth+tgaHeader.wPalLength*((tgaHeader.bPalBits+7)>>3)))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				break;
			case	TGA_NULL:	/* δ����ͼ�����ݵ��ļ�����Ϊ��Чͼ�� */
			default:			/* δ֪�ļ����� */
				b_status = ER_NONIMAGE; __leave;
				break;
			}

			/* ��������λ���Ƿ�Ϸ� */
			if ((tgaHeader.bBits != 8)&&(tgaHeader.bBits != 15)&& \
				(tgaHeader.bBits != 16)&&(tgaHeader.bBits != 24)&&\
				(tgaHeader.bBits != 32))
			{
				b_status = ER_BADIMAGE; __leave;
			}
			

			/* ��дͼ����Ϣ�ṹ */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_TGA;			/* ͼ���ļ���ʽ����׺���� */
			
			switch(tgaHeader.bImageType)				/* ͼ���ѹ����ʽ */
			{
			case	TGA_UNCPSPAL:
			case	TGA_UNCPSCOLOR:
			case	TGA_UNCPSGRAY:
				pinfo_str->compression = ICS_RGB; break;
			case	TGA_RLEPAL:
			case	TGA_RLECOLOR:
			case	TGA_RLEGRAY:
				switch(tgaHeader.bBits)
				{
				case	8:
					pinfo_str->compression = ICS_RLE8;  break;
				case	15:
				case	16:
					pinfo_str->compression = ICS_RLE16;	break;
				case	24:
					pinfo_str->compression = ICS_RLE24;	break;
				case	32:
					pinfo_str->compression = ICS_RLE32;	break;
				}
				break;
			default:
				assert(0);		/* ϵͳ���� */
				b_status = ER_SYSERR; 
				__leave;
				break;
			}
			
			
			/* ��дͼ����Ϣ */
			pinfo_str->width	= (unsigned long)tgaHeader.wWidth;
			pinfo_str->height	= (unsigned long)tgaHeader.wDepth;
			pinfo_str->order	= (tgaHeader.bDescriptor&TGA_VERTMIRROR) ? 0 : 1;
			pinfo_str->bitcount	= (unsigned long)(tgaHeader.bBits == 15) ? 16:tgaHeader.bBits;
			
			if (_get_mask(&tgaHeader, 
				(unsigned long *)(&pinfo_str->b_mask), 
				(unsigned long *)(&pinfo_str->g_mask), 
				(unsigned long *)(&pinfo_str->r_mask),
				(unsigned long *)(&pinfo_str->a_mask)) == 1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&tga_get_info_critical);
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
TGA_API int CALLAGREEMENT tga_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	TGAHEADER		tgaHeader;
	
	int				i, k, j, linesize, decoded;
	int				pallen = 0, firstindex = 0, newlinepos = 0, ile = 0;
	unsigned short	palentry;
	unsigned char	*pbuf = 0;
	unsigned char	*pbits;

	unsigned char	bpix;
	unsigned short	wpix;
	unsigned long	lpix;

	static unsigned char tmp_pal[1024];	/* ��ʱ��ɫ�建���� */

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&tga_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ʼ������ֵ */
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;

			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ��ȡ������ */
			stream_length = isio_length(pfile);
			
			/* ���ļ�ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&tgaHeader, sizeof(TGAHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* �ж��ļ�ͷ��ͼ�����ͱ�־ */
				switch(tgaHeader.bImageType)
				{
				case	TGA_RLEPAL:
				case	TGA_RLECOLOR:
				case	TGA_RLEGRAY:
					/* ��Ч��ͼ��������Ӧ����ͷ�ṹ�ߴ磨�����жϣ� */
					if (stream_length < sizeof(TGAHEADER))
					{
						b_status = ER_NONIMAGE; __leave;
					}
					break;		
				case	TGA_UNCPSPAL:
				case	TGA_UNCPSCOLOR:
				case	TGA_UNCPSGRAY:
					/* δѹ��ͼ��ɼ����ͼ�����ݵĳߴ磬�����жϵ��ܸ���ȷһЩ */
					if (stream_length < (sizeof(TGAHEADER)+tgaHeader.bID_Length+((tgaHeader.bBits+7)>>3)*tgaHeader.wDepth*tgaHeader.wWidth+tgaHeader.wPalLength*((tgaHeader.bPalBits+7)>>3)))
					{
						b_status = ER_NONIMAGE; __leave;
					}
					break;
				case	TGA_NULL:	/* δ����ͼ�����ݵ��ļ�����Ϊ��Чͼ�� */
				default:			/* δ֪�ļ����� */
					b_status = ER_NONIMAGE; __leave;
					break;
				}
				
				/* ��������λ���Ƿ�Ϸ� */
				if ((tgaHeader.bBits != 8)&&(tgaHeader.bBits != 15)&& \
					(tgaHeader.bBits != 16)&&(tgaHeader.bBits != 24)&&\
					(tgaHeader.bBits != 32))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				
				/* ��дͼ����Ϣ�ṹ */
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_TGA;			/* ͼ���ļ���ʽ����׺���� */
				
				switch(tgaHeader.bImageType)				/* ͼ���ѹ����ʽ */
				{
				case	TGA_UNCPSPAL:
				case	TGA_UNCPSCOLOR:
				case	TGA_UNCPSGRAY:
					pinfo_str->compression = ICS_RGB; break;
				case	TGA_RLEPAL:
				case	TGA_RLECOLOR:
				case	TGA_RLEGRAY:
					switch(tgaHeader.bBits)
					{
					case	8:
						pinfo_str->compression = ICS_RLE8;  break;
					case	15:
					case	16:
						pinfo_str->compression = ICS_RLE16;	break;
					case	24:
						pinfo_str->compression = ICS_RLE24;	break;
					case	32:
						pinfo_str->compression = ICS_RLE32;	break;
					}
					break;
				default:
					assert(0);		/* ϵͳ���� */
					b_status = ER_SYSERR; 
					__leave;
					break;
				}
				
				
				/* ��дͼ����Ϣ */
				pinfo_str->width	= (unsigned long)tgaHeader.wWidth;
				pinfo_str->height	= (unsigned long)tgaHeader.wDepth;
				pinfo_str->order	= (tgaHeader.bDescriptor&TGA_VERTMIRROR) ? 0 : 1;
				pinfo_str->bitcount	= (unsigned long)(tgaHeader.bBits == 15) ? 16:tgaHeader.bBits;
				
				/* ��д������Ϣ */
				if (_get_mask(&tgaHeader, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == 1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}


			/* ���õ�ɫ������ */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

						/* ���ɻҶ�ͼ��ɫ�� */
				if ((tgaHeader.bImageType == TGA_UNCPSGRAY)||(tgaHeader.bImageType == TGA_RLEGRAY))
				{
					for (i=0;i<(int)pinfo_str->pal_count;i++)
						pinfo_str->palette[i] = (i<<16)|(i<<8)|(i);
				}
				else	/* ������ת����ɫ�����ݴ��ļ� */
				{
					if (isio_seek(pfile, sizeof(TGAHEADER)+tgaHeader.bID_Length, SEEK_SET) == -1)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* TGA��ɫ���������ͨ��ARGB��ʽ��������Ҫ������жϹ��� */
					pallen = tgaHeader.wPalLength * ((tgaHeader.bPalBits+7) >> 3);
					firstindex = tgaHeader.wPalFirstNdx;

					if (isio_read((void*)tmp_pal, pallen, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* ת����ɫ������ */
					switch(tgaHeader.bPalBits)
					{
					case	15:
					case	16:		/* ת��2�ֽڵ�ɫ������ */
						for (i=0;i<pallen;i+=2)
						{
							if (firstindex >= 256)
							{
								b_status = ER_BADIMAGE; __leave;
							}

							palentry = *((unsigned short*)(tmp_pal+i));
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->blue  = (palentry&0x1f)<<3;
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->green = ((palentry>>5)&0x1f)<<3;
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->red   = ((palentry>>10)&0x1f)<<3;
							pinfo_str->palette[firstindex++] &= 0xffffff;	/* ����ֽ����� */

						}	
						break;
					case	24:		/* ת��3�ֽڵ�ɫ������ */
						for (i=0;i<pallen;i+=3)
						{
							if (firstindex >= 256)
							{
								b_status = ER_BADIMAGE; __leave;
							}

							((LPPALITEM)(&pinfo_str->palette[firstindex]))->blue  = tmp_pal[i];
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->green = tmp_pal[i+1];
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->red   = tmp_pal[i+2];
							pinfo_str->palette[firstindex++] &= 0xffffff;
						}
						break;
					case	32:		/* ת��4�ֽڵ�ɫ������ */
						for (i=0;i<pallen;i+=4)
						{
							if (firstindex >= 256)
							{
								b_status = ER_BADIMAGE; __leave;
							}

							pinfo_str->palette[firstindex] = *(unsigned long *)(tmp_pal+i);
							pinfo_str->palette[firstindex++] &= 0xffffff;
						}
						break;
					default:
						assert(0);		/* ϵͳ���� */
						b_status = ER_SYSERR; 
						__leave;
						break;
					}	
				}
			}
			else
			{
				pinfo_str->pal_count = 0;	/* �޵�ɫ������ */
			}
				
			/* ȡ��ɨ���гߴ� */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			
			/* ����ͼ����� */
			pinfo_str->imgnumbers = 1;		/* TGA�ļ���ֻ��һ��ͼ��*/
											/* Ӧ������ԭʼͼ����ͼ(Postage Stamp)��*/
											/* �����¶���δ����ʵ��	*/
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;		/* ��ͼ�����ÿ� */
			
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
			
			/* ����ͼ��ɨ���л����� */
			pbuf = (unsigned char *)malloc(pinfo_str->width*((pinfo_str->bitcount+7)>>3)+pinfo_str->width);	/* ��������RLEѹ����û��һ���ظ����� */
			if (!pbuf)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ��ʼ�����׵�ַ���� */
			if (pinfo_str->order == 0)		/* ���� */
			{
				for (i=0;i<(int)(pinfo_str->height);i++)
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}
			else							/* ���� */
			{
				for (i=0;i<(int)(pinfo_str->height);i++)
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
			
			/* ��ȡͼ������ */
			/*     ����ͼ���ļ���λ��ȵĲ�ͬ�����ò�ͬ�Ķ�ȡ���� */
			switch(tgaHeader.bImageType)
			{
			case	TGA_UNCPSPAL:
			case	TGA_UNCPSGRAY:
			case	TGA_UNCPSCOLOR:
				/* ��һ��ͼ���������ļ��е���ʼλ�� */
				newlinepos = sizeof(TGAHEADER)+tgaHeader.bID_Length+pallen;
				
				if (isio_seek(pfile, newlinepos, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* �ļ�ɨ���г��� */
				ile = pinfo_str->width*((pinfo_str->bitcount+7)>>3);
				
				pbits = pinfo_str->p_bit_data;

				/* ����Ϊ��λ��ȡͼ������ */
				if (tgaHeader.bDescriptor&TGA_HORZMIRROR)	/* ���ҷ��� */
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_read((void*)pbuf, ile, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						for (k=0;k<(int)pinfo_str->width;k++)
						{
							switch (pinfo_str->bitcount)
							{
							case	8:
								((unsigned char *)pbits)[k] = pbuf[pinfo_str->width-1-k];
								break;
							case	15:
							case	16:
								((unsigned short *)pbits)[k] = ((unsigned short *)pbuf)[pinfo_str->width-1-k];
								break;
							case	24:
								for (j=0;j<3;j++)
									((unsigned char *)pbits)[k*3+j] = pbuf[(pinfo_str->width-1-k)*3+j];
								break;
							case	32:
								((unsigned long *)pbits)[k] = ((unsigned long *)pbuf)[pinfo_str->width-1-k];
								break;
							default:
								assert(0);	/* ϵͳ���� */
								b_status = ER_SYSERR; 
								__leave;
								break;								
							}
						}
						
						/* ����ֵ�ۼ� */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_read((void*)pbits, ile, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						/* ����ֵ�ۼ� */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				break;
			case	TGA_RLEGRAY:
			case	TGA_RLEPAL:
			case	TGA_RLECOLOR:
				/* ��һ��ͼ���������ļ��е���ʼλ�� */
				newlinepos = sizeof(TGAHEADER)+tgaHeader.bID_Length+pallen;

				/* �ļ�ɨ���г��ȣ������������㣩 */
				ile = pinfo_str->width*((pinfo_str->bitcount+7)>>3)+pinfo_str->width;
				
				pbits = pinfo_str->p_bit_data;

				/* ����Ϊ��λ��ȡͼ������ */
				if (tgaHeader.bDescriptor&TGA_HORZMIRROR)	/* ���ҷ��� */
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_seek(pfile, newlinepos, SEEK_SET) == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						if (isio_read((void*)pbuf, 1, ile, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						/* ��ѹһRLEͼ���� */
						decoded = _decoding_line(pbits, pbuf, (pinfo_str->bitcount+7)>>3, pinfo_str->width*((pinfo_str->bitcount+7)>>3));
						
						if (decoded == -1)
						{
							b_status = ER_BADIMAGE; __leave;
						}
						
						assert(decoded);
						
						/* ��һ��ͼ���������ļ��е���ʼλ�� */
						newlinepos += decoded;

						/* ���ҵߵ�ͼ������ */
						for (k=0;k<(int)pinfo_str->width;k++)
						{
							switch (pinfo_str->bitcount)
							{
							case	8:
								bpix = ((unsigned char *)pbits)[k];
								((unsigned char *)pbits)[k] = ((unsigned char *)pbits)[pinfo_str->width-1-k];
								((unsigned char *)pbits)[pinfo_str->width-1-k] = bpix;
								break;
							case	15:
							case	16:
								wpix = ((unsigned short *)pbits)[k];
								((unsigned short *)pbits)[k] = ((unsigned short *)pbits)[pinfo_str->width-1-k];
								((unsigned short *)pbits)[pinfo_str->width-1-k] = wpix;
								break;
							case	24:
								for (j=0;j<3;j++)
								{
									bpix = ((unsigned char *)pbits)[k*3+j];
									((unsigned char *)pbits)[k*3+j] = ((unsigned char *)pbits)[(pinfo_str->width-1-k)*3+j];
									((unsigned char *)pbits)[(pinfo_str->width-1-k)*3+j] = bpix;
								}
								break;
							case	32:
								lpix = ((unsigned long *)pbits)[k];
								((unsigned long *)pbits)[k] = ((unsigned long *)pbits)[pinfo_str->width-1-k];
								((unsigned long *)pbits)[pinfo_str->width-1-k] = lpix;
								break;
							default:
								assert(0);	/* ϵͳ���� */
								b_status = ER_SYSERR; 
								__leave;
								break;								
							}
						}
						
						/* ����ֵ�ۼ� */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_seek(pfile, newlinepos, SEEK_SET) == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}

						if (isio_read((void*)pbuf, 1, ile, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}

						/* ��ѹһRLEͼ���� */
						decoded = _decoding_line(pbits, pbuf, (pinfo_str->bitcount+7)>>3, pinfo_str->width*((pinfo_str->bitcount+7)>>3));
						
						if (decoded == -1)
						{
							b_status = ER_BADIMAGE; __leave;
						}

						assert(decoded);

						/* ��һ��ͼ���������ļ��е���ʼλ�� */
						newlinepos += decoded;
						
						/* ����ֵ�ۼ� */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* ֧���û��ж� */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				break;
			default:
				assert(0);	/* ϵͳ���� */
				b_status = ER_SYSERR; 
				__leave;
				break;
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

			if (pbuf)
				free(pbuf);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&tga_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
TGA_API int CALLAGREEMENT tga_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	TGAHEADER		tgaHeader;
	TGAFOOTER		tgaFooter;
	int				i, wrlinesize;
	unsigned char   *pbits;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&tga_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_tga_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}


			/* ������Ϣ���ȣ��˴�������һ��"ISeeExplorer"�ַ��� */
			tgaHeader.bID_Length   = (unsigned char)strlen((const char *)tga_save_idinfo);
			/* ��ɫ����Ϣ���Ҷ�ͼ������Ҳ����������ɫ��ͼ�� */
			tgaHeader.bPalType     = (unsigned char) (pinfo_str->bitcount == 8) ? 1 : 0;
			/* ͼ������(ֻ��1��2��������) */
			tgaHeader.bImageType   = (unsigned char) (pinfo_str->bitcount == 8) ? TGA_UNCPSPAL : TGA_UNCPSCOLOR;
			/* ��ɫ���һ������ֵ */
			tgaHeader.wPalFirstNdx = 0;
			/* ��ɫ��������(�Ե�ɫ�嵥ԪΪ��λ) */
			tgaHeader.wPalLength   = (unsigned short)(pinfo_str->bitcount == 8) ? 256 : 0;
			/* һ����ɫ�嵥λλ��(ISee��ɫ������32λ��) */
			tgaHeader.bPalBits     = 32;
			/* ͼ�����¶����� */
			tgaHeader.wLeft        = 0;
			tgaHeader.wBottom      = 0;
			/* ͼ����߶� */
			tgaHeader.wWidth       = (unsigned short)pinfo_str->width;
			tgaHeader.wDepth       = (unsigned short)pinfo_str->height;
			/* ����λ�� */
			tgaHeader.bBits        = (unsigned char)pinfo_str->bitcount;
			/* ������������(������Aͨ����һ��Ϊ����ͼ) */
			tgaHeader.bDescriptor  = 0;

			
			/* ���ý���ֵ */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д���ļ�ͷ�ṹ */
			if (isio_write((const void *)&tgaHeader, sizeof(TGAHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* д����Ϣ */
			if (isio_write((const void *)tga_save_idinfo, strlen(tga_save_idinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д���ɫ������ */
			if (tgaHeader.bPalType == 1)
			{
				if (isio_write((const void *)pinfo_str->palette, pinfo_str->pal_count*4, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			/* TGAδѹ��ɨ���гߴ粢����4�ֽڶ���ģ�������Ҫ��ʵ�����ؼ����гߴ� */
			wrlinesize = pinfo_str->width * ((pinfo_str->bitcount+7)>>3);

			/* д��ͼ��λ���� 
				ע�������ͼ�����ݱ�����ISeeλ����ʽ��������ܻ����ʧ��
			*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				/* һ�ɱ���Ϊ����ͼ */
				pbits = (unsigned char *)(pinfo_str->pp_line_addr[pinfo_str->height-1-i]);

				if (isio_write((const void*)pbits, wrlinesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}

			/* ��д�ļ�β����Ϣ���°�TGA��ʽ��*/
			tgaFooter.eao = 0;
			tgaFooter.ddo = 0;
			memcpy((void*)tgaFooter.info, (const void *)"TRUEVISION-XFILE", 16);
			tgaFooter.period = '.';
			tgaFooter.zero = 0;

			if (isio_write((const void*)&tgaFooter, sizeof(TGAFOOTER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&tga_save_img_critical);
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

/* ��ȡ����ɫ������ֵ */
int CALLAGREEMENT _get_mask(LPTGAHEADER pInfo, 
	unsigned long * pb_mask, 
	unsigned long * pg_mask, 
	unsigned long * pr_mask, 
	unsigned long * pa_mask
	)
{
	int result = 0;

	assert(pInfo->bBits);

	switch(pInfo->bBits)
	{
	case	8:
		/* ��ɫ��λͼ������ */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	15:
		/*******************************************
				15��16λ����ʾ��ͼ - TGA

			  ���ֽ�      ���ֽ�
			0000 0000   0000 0000 
						   1 1111--B	15λ��ʽ
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	16λ��ʽ
				   11   111 -------G
			 111 11  --------------R
			1  --------------------A
		*******************************************/
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	16:
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0x8000;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:
		/******************************************************
						32λ����ʾ��ͼ - TGA
			    ��               ->                 ��
		0000 0000   0000 0000   0000 0000   0000 0000 	888��ʽ
		1111 1111  ------------------------------------A
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		******************************************************/
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	default:
		result = 1;

		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}

	return result;	/* 0-�ɹ��� 1���Ƿ���ͼ���ʽ */
}


/* ����ɨ���гߴ�(���ֽڶ���) */
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* �н�ѹ������psrcʵ�ʽ�ѹ�ֽ�������Դ�������󣬷���-1 */
int	CALLAGREEMENT _decoding_line(
		unsigned char *pdest,			/* Ŀ�껺���� */
		unsigned char * psrc,			/* RLEԴ����  */
		int rleunit,					/* rleunitΪ��ѹ��λ(in Bytes) */
		int destlen						/* destlenΪ��ѹĿ�곤�� */
		)
{
	int i, j, nCount;
	unsigned char *pdestEnd = pdest+destlen, *psrcOrg=psrc, *pdestOrg=pdest;
	unsigned char *ptmp;

	assert(psrc);
	assert(pdest);
	
	while(pdest<pdestEnd)
	{
		if(psrc[0]&0x80)						/* �ظ����� */
		{
			nCount=(psrc[0]&0x7F)+1;
			psrc++;
			for(i=0;i<nCount;i++)
			{
				ptmp = psrc;
				for (j=0;j<rleunit;j++)			/* ��ѹһ������(length equ rleunit) */
					*pdest++ = *ptmp++;
			}
			psrc+=rleunit;
		}
		else									/* ���ظ����� */
		{
			nCount=(psrc[0]&0x7F)+1;
			psrc++;
			memcpy(pdest,psrc,nCount*rleunit);	/* ���ظ�����ĳߴ���ܻ�Ƚϴ�����memcpy��Ч�ʿ��ܸ��� */
			pdest+=nCount*rleunit;
			psrc+=nCount*rleunit;
		}
	}

	if(pdest != pdestEnd)						/* ͼ�����ݷǷ������� -1 */
		return -1;

	return psrc-psrcOrg;						/* ��ѹ�ɹ������ر���ѹ�ֽ��� */
}


/* �жϴ����ͼ���Ƿ���Ա����� */
int CALLAGREEMENT _tga_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* ## λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(tga_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */
	
	assert(pinfo_str->imgnumbers);
	
	if (tga_irwp_info.irwp_save.img_num)
		if (tga_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */
			
	return 0;
}
