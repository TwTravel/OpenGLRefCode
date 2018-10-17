/********************************************************************

	ras.c

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

			isee##vip.163.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������Sun RASͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��ɶ�ȡ1��8��24��32λѹ����δѹ��RASͼ��
							  
					���湦�ܣ��ṩ24λ��ѹ����ʽ�ı��湦��

	���ļ���д�ˣ�	
					netmap		beahuang##hotmail.com
					YZ			yzfree##yeah.net

	���ļ��汾��	20610
	����޸��ڣ�	2002-6-10

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	ע�����ļ��Ĳ��ֺ����ο���FreeImageͼ����Դ���룬�ڴ���
			Floris van den Berg (flvdberg@wxs.nl)
			Herv Drolon (drolon@iut.univ-lehavre.fr)
		��ʾ��л������������˽�FreeImageͼ������������
		�����Ĺٷ���վ��
			http://www.6ixsoft.com/
	----------------------------------------------------------------
	������ʷ��

		2002-6		��һ���汾�������°棩

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

#include "ras.h"


IRWP_INFO			ras_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	ras_get_info_critical;	/* ras_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	ras_load_img_critical;	/* ras_load_image�����Ĺؼ��� */
CRITICAL_SECTION	ras_save_img_critical;	/* ras_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _ras_is_valid_img(LPINFOSTR pinfo_str);

enum EXERESULT CALLAGREEMENT _read_data(ISFILE *, unsigned char *, long, int, int);





#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&ras_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&ras_get_info_critical);
			InitializeCriticalSection(&ras_load_img_critical);
			InitializeCriticalSection(&ras_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&ras_get_info_critical);
			DeleteCriticalSection(&ras_load_img_critical);
			DeleteCriticalSection(&ras_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

RAS_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&ras_irwp_info;
}

#else

RAS_API LPIRWP_INFO CALLAGREEMENT ras_get_plugin_info()
{
	_init_irwp_info(&ras_irwp_info);

	return (LPIRWP_INFO)&ras_irwp_info;
}

RAS_API void CALLAGREEMENT ras_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

RAS_API void CALLAGREEMENT ras_detach_plugin()
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
	/* ��λ����һ������32λ��ʽ�޷���ʾ���˴�����RAS��ȷ��λ����д��*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1��ֻ�ܱ���һ��ͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 2;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"netmap");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"beahuang##hotmail.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"How do you do!");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"һֻ���ϣ����Ҳȡ��Ҳȡ��Ҳȡ��������˵��ɨ�ز�����������������");
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

	/* ����������##���ֶ���������RAS��ʽ�б�����ο�BMP����б��������ã� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
RAS_API int CALLAGREEMENT ras_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	SUNHeaderinfo	header;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&ras_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ��ȡͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&header, sizeof(SUNHeaderinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ת��X86���� */
			header.ras_magic     = EXCHANGE_DWORD((header.ras_magic));
			header.ras_width     = EXCHANGE_DWORD((header.ras_width));
			header.ras_height    = EXCHANGE_DWORD((header.ras_height));
			header.ras_depth     = EXCHANGE_DWORD((header.ras_depth));
			header.ras_length    = EXCHANGE_DWORD((header.ras_length));
			header.ras_type      = EXCHANGE_DWORD((header.ras_type));
			header.ras_maptype   = EXCHANGE_DWORD((header.ras_maptype));
			header.ras_maplength = EXCHANGE_DWORD((header.ras_maplength));
			
			/* �ж��Ƿ���RASͼ�� */
			if (header.ras_magic != RAS_MAGIC)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ͼ�����������Ƿ�Ϸ���RT_FORMAT_TIFF��RT_FORMAT_IFF��
			   RT_EXPERIMENTAL��ʽ������֧�֣�*/
			if ((header.ras_type != RT_OLD) && 
				(header.ras_type != RT_STANDARD) && 
				(header.ras_type != RT_BYTE_ENCODED) && 
				(header.ras_type != RT_FORMAT_RGB))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* �жϳߴ�ֵ�Ƿ�Ϸ� */
			if ((header.ras_width == 0)||(header.ras_height == 0)||(header.ras_depth == 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			switch (header.ras_depth)
			{
			case	1:
			case	8:
			case	24:
			case	32:
				break;
			default:
				b_status = ER_BADIMAGE; __leave;	/* �������֧���������ָ�ʽ֮���ͼ��*/
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_RAS;			/* ͼ���ļ���ʽ����׺���� */

			switch (header.ras_type)					/* ͼ���ѹ����ʽ */
			{
			case RT_OLD:
			case RT_STANDARD:
			case RT_FORMAT_RGB:
				pinfo_str->compression = ICS_RGB;
				break;
			case RT_BYTE_ENCODED:
				pinfo_str->compression = ICS_RLE8;
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave;
			}
			
			/* ��дͼ����Ϣ */
			pinfo_str->width	= header.ras_width;
			pinfo_str->height	= header.ras_height;
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= header.ras_depth;
			
			/* ��д�������� */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	8:
				SETMASK_8(pinfo_str);
				break;
			case	24:
				SETMASK_24(pinfo_str);
				break;
			case	32:
				SETMASK_32(pinfo_str);
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave;
			}
		
			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&ras_get_info_critical);
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
RAS_API int CALLAGREEMENT ras_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				linesize, linelength, fill, i, j, numcolors, isrle, isrgb, fillchar;
	unsigned char	c, *r = 0, *g, *b, *colormap = 0, *buf = 0, *bp, *bits;
	SUNHeaderinfo	header;
	LPPALITEM		pal;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&ras_load_img_critical);

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

			if (isio_read((void*)&header, sizeof(SUNHeaderinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ת��X86���� */
			header.ras_magic     = EXCHANGE_DWORD((header.ras_magic));
			header.ras_width     = EXCHANGE_DWORD((header.ras_width));
			header.ras_height    = EXCHANGE_DWORD((header.ras_height));
			header.ras_depth     = EXCHANGE_DWORD((header.ras_depth));
			header.ras_length    = EXCHANGE_DWORD((header.ras_length));
			header.ras_type      = EXCHANGE_DWORD((header.ras_type));
			header.ras_maptype   = EXCHANGE_DWORD((header.ras_maptype));
			header.ras_maplength = EXCHANGE_DWORD((header.ras_maplength));
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				if (header.ras_magic != RAS_MAGIC)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				if ((header.ras_type != RT_OLD) && 
					(header.ras_type != RT_STANDARD) && 
					(header.ras_type != RT_BYTE_ENCODED) && 
					(header.ras_type != RT_FORMAT_RGB))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				if ((header.ras_width == 0)||(header.ras_height == 0)||(header.ras_depth == 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				switch (header.ras_depth)
				{
				case	1:
				case	8:
				case	24:
				case	32:
					break;
				default:
					b_status = ER_BADIMAGE; __leave;	/* �������֧���������ָ�ʽ֮���ͼ��*/
				}

				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_RAS;			/* ͼ���ļ���ʽ����׺���� */

				switch (header.ras_type)					/* ͼ���ѹ����ʽ */
				{
				case RT_OLD:
				case RT_STANDARD:
				case RT_FORMAT_RGB:
					pinfo_str->compression = ICS_RGB;
					break;
				case RT_BYTE_ENCODED:
					pinfo_str->compression = ICS_RLE8;
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave;
				}
				
				pinfo_str->width	= header.ras_width;
				pinfo_str->height	= header.ras_height;
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= header.ras_depth;
				
				switch (pinfo_str->bitcount)
				{
				case	1:
				case	8:
					SETMASK_8(pinfo_str);
					break;
				case	24:
					SETMASK_24(pinfo_str);
					break;
				case	32:
					SETMASK_32(pinfo_str);
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave;
				}

				pinfo_str->data_state = 1;
			}
			
			pinfo_str->process_total   = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			isrle = 0;
			isrgb = 0;
			
			/* �趨�������ݴ�ŷ�ʽ���Ƿ�ѹ������RGB�ŷ�˳��*/
			switch (header.ras_type)
			{
			case RT_OLD:
			case RT_STANDARD:		/* ��ѹ����BGR���� */
				break;
			case RT_BYTE_ENCODED:	/* ѹ�� */
				isrle = 1;
				break;
			case RT_FORMAT_RGB:		/* ��ѹ����RGB���� */
				isrgb = 1;
				break;
			case RT_FORMAT_TIFF:	/* ��֧�ֵĴ洢��ʽ */
			case RT_FORMAT_IFF:
			default:
				b_status = ER_BADIMAGE; __leave; break;
			}
	

			/* ���õ�ɫ������ */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	8:
				pinfo_str->pal_count = numcolors = 1 << header.ras_depth;
				
				switch (header.ras_maptype)
				{
				case RMT_NONE:			/* �˹��ϳɻҶȵ�ɫ�� */
					pal = (LPPALITEM)pinfo_str->palette;
					
					for (i = 0; i < numcolors; i++)
					{
						c = (unsigned char)((255*i)/(numcolors-1));

						pal[i].red	 = c;
						pal[i].green = c;
						pal[i].blue	 = c;
						pal[i].reserved = 0;
					}
					break;
				case RMT_EQUAL_RGB:		/* ��ȡ��ɫ������ */
					pal = (LPPALITEM)pinfo_str->palette;
					
					/* �ж�ͼ���ɫ�峤�����ֵ�Ƿ�Ϸ� */
					if (header.ras_maplength > (3*numcolors*sizeof(unsigned char)))
					{
						b_status = ER_BADIMAGE; __leave; break;
					}

					/* �������ܷ����ɫ�建���� */
					if ((r=(unsigned char *)malloc(3*numcolors*sizeof(unsigned char))) == 0)
					{
						b_status = ER_MEMORYERR; __leave; break;
					}

					/* ����ʵ�ʵ�ɫ������� */
					numcolors = header.ras_maplength/3;
					
					g = r + numcolors;
					b = g + numcolors;
					
					/* �����ɫ������ */
					if (isio_read((void*)r, header.ras_maplength, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave; break;
					}
					
					for (i = 0; i < numcolors; i++)
					{
						pal[i].red	 = r[i];
						pal[i].green = g[i];
						pal[i].blue	 = b[i];
						pal[i].reserved = 0;
					}
					break;
				case RMT_RAW:			/* �������ɫ������ */
					/* δ�ܸ������ֵ�ɫ��ĸ�ʽ����ʱ���� */
					if ((colormap=(unsigned char *)malloc(header.ras_maplength * sizeof(unsigned char))) == 0)
					{
						b_status = ER_MEMORYERR; __leave; break;
					}
					
					if (isio_read((void*)colormap, header.ras_maplength, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave; break;
					}
					break;
				default:				/* �Ƿ��ĵ�ɫ���ǩֵ */
					b_status = ER_BADIMAGE; __leave; break;
				}
				break;
			case	24:
			case	32:
				/* ���ͼ���д��ڵ�ɫ�����ݣ���˵����һ����������ͼ�� */
				if (header.ras_maptype != RMT_NONE)
				{
					b_status = ER_BADIMAGE; __leave; break;
				}

				pinfo_str->pal_count = 0;

				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}
			
			
			/* ȡ��ɨ���гߴ� */
			linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);
			
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
			
			/* ��ʼ�����׵�ַ���飨RASͼ��Ϊ���� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}
			
			if (header.ras_depth == 1)
				linelength = (int)((header.ras_width/8)+(header.ras_width%8 ? 1 : 0));
			else
				linelength = (int)header.ras_width;

			/* RASɨ����Ϊ˫�ֽڶ��� */
			fill = (linelength % 2) ? 1 : 0;

			
			/* ��ʼ����ȡ���� */
			_read_data(pfile, 0, 0, 0, 1);

			/* ���ж��� */
			switch (pinfo_str->bitcount)
			{
			case 1:
			case 8:
				for (i=0; i<(int)pinfo_str->height; i++)
				{
					bits = pinfo_str->p_bit_data+i*linesize;

					/* ����һ�е����� */
					if ((b_status=_read_data(pfile, bits, linelength, isrle, 0)) != ER_SUCCESS)
					{
						__leave; break;
					}

					/* ����еĻ�����ȡ��β����ֽ� */
					if (fill)
					{
						if ((b_status=_read_data(pfile, (unsigned char *)&fillchar, fill, isrle, 0)) != ER_SUCCESS)
						{
							__leave; break;
						}
					}

					pinfo_str->process_current = i;
					
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
				break;
			case 24:
				/* ���仺���� */
				if ((buf=(unsigned char *)malloc(pinfo_str->width*3)) == 0)
				{
					b_status = ER_MEMORYERR; __leave; break;
				}

				for (i=0; i<(int)pinfo_str->height; i++)
				{
					bits = pinfo_str->p_bit_data+i*linesize;

					if ((b_status=_read_data(pfile, buf, pinfo_str->width*3, isrle, 0)) != ER_SUCCESS)
					{
						__leave; break;
					}

					bp = buf;

					for (j=0; j<(int)pinfo_str->width; j++)
					{
						bits[2] = *bp++; /* red */
						bits[1] = *bp++; /* green */
						bits[0] = *bp++; /* blue */
						
						bits += 3;
					}

#if 0
					/* ������δ�����ʵ��ͼ���ʽ��ͻ��isrgbΪ��ʱ������ΪRGB���У�*/
					/* ��ʱ������δ��� */
					if (isrgb)		/* RGB���� */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[2] = *bp++; /* red */
							bits[1] = *bp++; /* green */
							bits[0] = *bp++; /* blue */
							
							bits += 3;
						}
					}
					else			/* BGR���� */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[0] = *bp++; /* blue */
							bits[1] = *bp++; /* green */
							bits[2] = *bp++; /* red */
							
							bits += 3;
						}
					}
#endif

					if (fill)
					{
						if ((b_status=_read_data(pfile, (unsigned char *)&fillchar, fill, isrle, 0)) != ER_SUCCESS)
						{
							__leave; break;
						}
					}

					pinfo_str->process_current = i;
					
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
				break;
			case 32:
				/* ���仺���� */
				if ((buf=(unsigned char *)malloc(pinfo_str->width*4)) == 0)
				{
					b_status = ER_MEMORYERR; __leave; break;
				}
				
				for (i=0; i<(int)pinfo_str->height; i++)
				{
					bits = pinfo_str->p_bit_data+i*linesize;
					
					if ((b_status=_read_data(pfile, buf, pinfo_str->width*4, isrle, 0)) != ER_SUCCESS)
					{
						__leave; break;
					}
					
					bp = buf;
					
					for (j=0; j<(int)pinfo_str->width; j++)
					{
						bits[3] = *bp++; /* empty */
						bits[2] = *bp++; /* red */
						bits[1] = *bp++; /* green */
						bits[0] = *bp++; /* blue */
						
						bits += 4;
					}

#if 0
					/* ע��ԭ����� */
					if (isrgb)		/* RGB���� */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[3] = *bp++; /* empty */
							bits[2] = *bp++; /* red */
							bits[1] = *bp++; /* green */
							bits[0] = *bp++; /* blue */
							
							bits += 4;
						}
					}
					else			/* BGR���� */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[3] = *bp++; /* empty */
							bits[0] = *bp++; /* blue */
							bits[1] = *bp++; /* green */
							bits[2] = *bp++; /* red */
							
							bits += 4;
						}
					}
#endif
					
					if (fill)
					{
						if ((b_status=_read_data(pfile, (unsigned char *)&fillchar, fill, isrle, 0)) != ER_SUCCESS)
						{
							__leave; break;
						}
					}

					pinfo_str->process_current = i;
					
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
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

			if (buf)
				free(buf);

			if (colormap)
				free(colormap);

			if (r)
				free(r);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&ras_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
RAS_API int CALLAGREEMENT ras_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, j, wlen;
	unsigned char	*bits, *pbuf = 0;
	SUNHeaderinfo	header;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&ras_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_ras_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* ���ý���ֵ */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��дͷ�ṹ */
			header.ras_magic     = RAS_MAGIC;
			header.ras_width     = (int)pinfo_str->width;
			header.ras_height    = (int)pinfo_str->height;
			header.ras_depth     = (int)pinfo_str->bitcount;
			header.ras_length    = (header.ras_width*3+((header.ras_width*3)%2))*header.ras_height;
			header.ras_type      = RT_STANDARD;
			header.ras_maptype   = RMT_NONE;
			header.ras_maplength = 0;

			/* ת��68K���� */
			header.ras_magic     = EXCHANGE_DWORD((header.ras_magic));
			header.ras_width     = EXCHANGE_DWORD((header.ras_width));
			header.ras_height    = EXCHANGE_DWORD((header.ras_height));
			header.ras_depth     = EXCHANGE_DWORD((header.ras_depth));
			header.ras_length    = EXCHANGE_DWORD((header.ras_length));
			header.ras_type      = EXCHANGE_DWORD((header.ras_type));
			header.ras_maptype   = EXCHANGE_DWORD((header.ras_maptype));
			header.ras_maplength = EXCHANGE_DWORD((header.ras_maplength));
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д��ͷ�ṹ��Ŀ���� */
			if (isio_write((const void*)&header, sizeof(SUNHeaderinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			wlen = pinfo_str->width*3+((pinfo_str->width*3)%2);
			
			/* ����д��������ɨ���У�*/
			if ((pbuf=(unsigned char *)malloc(wlen+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* д��ͼ�����ݣ������ͼ�����ݱ�����ISeeλ����ʽ��������ܻ����ʧ�棩*/
			for (i=0; i<(int)(pinfo_str->height); i++)
			{
				bits = (unsigned char *)pinfo_str->pp_line_addr[i];

				/* �ߵ�R��B������ע���ĵ�������ʵ��ͼ���ʽ����������ACDSee��ʾЧ��Ϊ׼ */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j*3+0] = bits[j*3+2];
					pbuf[j*3+1] = bits[j*3+1];
					pbuf[j*3+2] = bits[j*3+0];
				}

				/* д��Ŀ���� */
				if (isio_write((const void*)pbuf, wlen, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i;
				
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
			if (pbuf)
				free(pbuf);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&ras_save_img_critical);
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
int CALLAGREEMENT _ras_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(ras_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (ras_irwp_info.irwp_save.img_num)
	{
		if (ras_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* ͼ���������ȷ */
			}
		}
	}

	return 0;
}


/* ��ȡָ����Ŀ������������������*/
static enum EXERESULT CALLAGREEMENT _read_data(ISFILE *pfile, 
											   unsigned char *buf, 
											   long length, 
											   int rle, 
											   int init)
{
	static unsigned char repchar, remaining;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	/* ��ʼ������ֵ */
	if (init)
	{
		repchar = remaining = 0;	/* ��̬�������ظ�����ʱ��Ҫ��ʱ�ĳ�ʼ�� */

		return b_status;
	}

	__try
	{
		__try
		{
			/* �����RLEѹ�����ݣ����Ƚ��� */
			if (rle)
			{
				while(length--)
				{
					if (remaining)
					{
						remaining--;
						*(buf++)= repchar;
					}
					else
					{
						/* ��ȡ��ʶ�ֽ� */
						if (isio_read((void*)&repchar, 1, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						/* �ж��Ƿ�����ѹ�����ܵ��ֶ� */
						if (repchar == RESC)
						{
							if (isio_read((void*)&remaining, 1, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;
							}
							/* ѹ������Ϊ1�ֽڵ�0x80 */
							if (remaining == 0)
							{
								*(buf++)= RESC;
							}
							else	/* �ظ����� */
							{
								/* ��ȡ�ظ��ֽ� */
								if (isio_read((void*)&repchar, 1, 1, pfile) == 0)
								{
									b_status = ER_FILERWERR; __leave;
								}
								
								*(buf++)= repchar;
							}
						}
						else	/* δѹ�����ֽ����� */
						{
							*(buf++)= repchar;
						}
					}
				}
			}
			else	/* δѹ��ͼ�� */
			{
				if (isio_read((void*)buf, length, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
		}
		__finally
		{
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_FILERWERR;
	}

	return b_status;
}
