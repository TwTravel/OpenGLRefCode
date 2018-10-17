/********************************************************************

	rla.c

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
	���ļ���;��	ISeeͼ���������Alias/Wavefront RLAͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��ɶ�ȡ3ͨ��24λѹ��rgbɫϵ��ͼ�󣬲�֧��matte��
								auxͨ��
							  
					���湦�ܣ��ɽ�24λͼ�󱣴�Ϊ3ͨ��RLE8ѹ����rgbɫϵͼ��

	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net

	���ļ��汾��	20509
	����޸��ڣ�	2002-5-9

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-5		��һ�������棨�°棩


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

#include "rla.h"


IRWP_INFO			rla_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	rla_get_info_critical;	/* rla_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	rla_load_img_critical;	/* rla_load_image�����Ĺؼ��� */
CRITICAL_SECTION	rla_save_img_critical;	/* rla_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);
int CALLAGREEMENT _rla_is_valid_img(LPINFOSTR pinfo_str);

static void CALLAGREEMENT _swap_byte(LPRLA_HEADER prla);
static int  CALLAGREEMENT _decode_scanline(unsigned char *, int, unsigned char *, int);
static int CALLAGREEMENT _encode_scanline(unsigned char *, int, unsigned char *);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&rla_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&rla_get_info_critical);
			InitializeCriticalSection(&rla_load_img_critical);
			InitializeCriticalSection(&rla_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&rla_get_info_critical);
			DeleteCriticalSection(&rla_load_img_critical);
			DeleteCriticalSection(&rla_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

RLA_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&rla_irwp_info;
}

#else

RLA_API LPIRWP_INFO CALLAGREEMENT rla_get_plugin_info()
{
	_init_irwp_info(&rla_irwp_info);

	return (LPIRWP_INFO)&rla_irwp_info;
}

RLA_API void CALLAGREEMENT rla_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

RLA_API void CALLAGREEMENT rla_detach_plugin()
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
	/* ��λ����һ������32λ��ʽ�޷���ʾ���˴�����RLA��ȷ��λ����д��*/
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
				(const char *)"RLA�Ĳ���ͼƬ�����Ұ���");
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

	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
RLA_API int CALLAGREEMENT rla_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	RLA_HEADER		rlaheader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&rla_get_info_critical);

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

			if (isio_read((void*)&rlaheader, sizeof(RLA_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ת������ */
			_swap_byte(&rlaheader);

			/* �ж��Ƿ��ǺϷ���RLAͼ���� */
			if ((rlaheader.revision != (short)RLA_REVISION) || \
				((rlaheader.matte_type != 0)&&(rlaheader.matte_type != 4)) || \
				((rlaheader.storage_type != 0)&&(rlaheader.storage_type != 4)) || \
				(rlaheader.window.left > rlaheader.window.right) || \
				(rlaheader.window.bottom > rlaheader.window.top) || \
				(rlaheader.active_window.left > rlaheader.active_window.right) || \
				(rlaheader.active_window.bottom > rlaheader.active_window.top))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ֻ֧��RGBɫϵ��RLAͼ�� */
			if (strcmp((const char *)rlaheader.chan, "rgb"))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* ֻ֧��24λRLAͼ�󣬲��ҷ���matteͨ�����ݣ����ݰ���ʱ�޷�����matte���ݣ�*/
			if ((rlaheader.num_chan != 3) || (rlaheader.chan_bits != 8))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* ���ͼ����߲����Ƿ�Ϸ� */
			if (((rlaheader.active_window.right - rlaheader.active_window.left) < 0) || \
				((rlaheader.active_window.top - rlaheader.active_window.bottom) < 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}


			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_RLA;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression  = ICS_RLE8;

			pinfo_str->width	= (unsigned long)(rlaheader.active_window.right - rlaheader.active_window.left + 1);
			pinfo_str->height	= (unsigned long)(rlaheader.active_window.top - rlaheader.active_window.bottom + 1);
			pinfo_str->order	= 1;
			pinfo_str->bitcount	= 24;					/* 3ͨ����8λ */
			
			pinfo_str->b_mask = 0xff;
			pinfo_str->g_mask = 0xff00;
			pinfo_str->r_mask = 0xff0000;
			pinfo_str->a_mask = 0x0;
			

			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&rla_get_info_critical);
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
RLA_API int CALLAGREEMENT rla_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, linesize, j;
	unsigned int	*poffset = 0;
	unsigned char	*punpack = 0, *pspl = 0, *p;
	unsigned char	*pr, *pg, *pb;
	unsigned short	rl, gl, bl;
	
	RLA_HEADER		rlaheader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&rla_load_img_critical);

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

			/* ���ļ�ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&rlaheader, sizeof(RLA_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ת������ */
			_swap_byte(&rlaheader);
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				if ( (rlaheader.revision != (short)RLA_REVISION) || \
					((rlaheader.matte_type != 0)&&(rlaheader.matte_type != 4)) || \
					((rlaheader.storage_type != 0)&&(rlaheader.storage_type != 4)) || \
					(rlaheader.window.left > rlaheader.window.right) || \
					(rlaheader.window.bottom > rlaheader.window.top) || \
					(rlaheader.active_window.left > rlaheader.active_window.right) || \
					(rlaheader.active_window.bottom > rlaheader.active_window.top) )
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				if (strcmp((const char *)rlaheader.chan, "rgb"))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				if ((rlaheader.num_chan != 3) || (rlaheader.chan_bits != 8))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				if (((rlaheader.active_window.right - rlaheader.active_window.left) < 0) || \
					((rlaheader.active_window.top - rlaheader.active_window.bottom) < 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_RLA;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression  = ICS_RLE8;
				
				pinfo_str->width	= (unsigned long)(rlaheader.active_window.right - rlaheader.active_window.left + 1);
				pinfo_str->height	= (unsigned long)(rlaheader.active_window.top - rlaheader.active_window.bottom + 1);
				pinfo_str->order	= 1;
				pinfo_str->bitcount	= 24;					/* 3ͨ����8λ */
				
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
				
				pinfo_str->data_state = 1;
			}				
			
			/* �õ�ɫ����Ϊ 0 */
			pinfo_str->pal_count = 0;
			
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
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
			
			/* �������ڴ洢ɨ����ƫ�Ʊ���ڴ� */
			if ((poffset=(int*)malloc(4*pinfo_str->height)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ��ȡƫ�Ʊ� */
			if (isio_read((void*)poffset, 4*pinfo_str->height, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ת��ƫ��ֵ���� */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				poffset[i] = EXCHANGE_DWORD(poffset[i]);
			}
			
			/* ����ѹ�����ݻ����� */
			if ((punpack=(unsigned char *)malloc(pinfo_str->width*(pinfo_str->bitcount/8)*2)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			pr = punpack;
			pg = pr+pinfo_str->width*2;
			pb = pg+pinfo_str->width*2;

			/* �����ѹ������ */
			if ((pspl=(unsigned char *)malloc(pinfo_str->width*(pinfo_str->bitcount/8))) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			

			/* ���ж��� */
			for (p=pinfo_str->p_bit_data, i=0; i<(int)(pinfo_str->height); i++)
			{
				/* ��λɨ���ж�ȡλ�� */
				if (isio_seek(pfile, poffset[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ֻ��ȡǰ3��ͨ�����ݣ�R��G��B����matteͨ���ݲ���ȡ */

				/* ��R */
				if (isio_read((void*)&rl, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				rl = EXCHANGE_WORD(rl);

				if (isio_read((void*)pr, rl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��G */
				if (isio_read((void*)&gl, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				gl = EXCHANGE_WORD(gl);
				
				if (isio_read((void*)pg, gl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��B */
				if (isio_read((void*)&bl, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				bl = EXCHANGE_WORD(bl);
				
				if (isio_read((void*)pb, bl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* �������ݺϷ��� */
				if ((rl==0)||(gl==0)||(bl==0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ��ѹ�� */
				if (_decode_scanline(pr, rl, pspl, (int)pinfo_str->width) != 0)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				if (_decode_scanline(pg, gl, pspl+pinfo_str->width, (int)pinfo_str->width) != 0)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				if (_decode_scanline(pb, bl, pspl+pinfo_str->width*2, (int)pinfo_str->width) != 0)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				p = pinfo_str->p_bit_data + (linesize*i);
				
				/* �ϳ�Ŀ�����أ���д�����ݰ� */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					*p++ = ((unsigned char *)(pspl+pinfo_str->width*2))[j];
					*p++ = ((unsigned char *)(pspl+pinfo_str->width))[j];
					*p++ = ((unsigned char *)(pspl))[j];
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

			if (pspl)
				free(pspl);

			if (punpack)
				free(punpack);

			if (poffset)
				free(poffset);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&rla_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
RLA_API int CALLAGREEMENT rla_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	int				i, j, off;
	unsigned int	*poffset = 0;
	unsigned char	*ppack = 0, *pspl = 0, *p;
	unsigned char	*pr, *pg, *pb;
	unsigned short	rl, gl, bl, l;
	
	RLA_HEADER		rlaheader;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&rla_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_rla_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			memset((void*)&rlaheader, 0, sizeof(RLA_HEADER));

			/* ��д��ͷ�ṹ */
			rlaheader.window.left   = 0;
			rlaheader.window.bottom = 0;
			rlaheader.window.right  = (short)(pinfo_str->width-1);
			rlaheader.window.top    = (short)(pinfo_str->height-1);
			
			rlaheader.active_window.left   = 0;
			rlaheader.active_window.bottom = 0;
			rlaheader.active_window.right  = (short)(pinfo_str->width-1);
			rlaheader.active_window.top    = (short)(pinfo_str->height-1);

			rlaheader.revision  = (short)RLA_REVISION;

			/* 3ͨ����R��G��B */
			strcpy(rlaheader.chan, "rgb");
			rlaheader.num_chan  = 3;
			rlaheader.chan_bits	= 8;

			strcpy(rlaheader.desc, "Saved by ISee RLA plug-in." );
			strcpy(rlaheader.program, "ISee Explorer.");
			
			/* ͼ����� */
			rlaheader.frame = 1;

			strcpy( rlaheader.gamma, "2.2" );

			strcpy( rlaheader.red_pri  , "0.670 0.330" );
			strcpy( rlaheader.green_pri, "0.210 0.710" );
			strcpy( rlaheader.blue_pri , "0.140 0.080" );
			strcpy( rlaheader.white_pt , "0.310 0.316" );
			
			strcpy( rlaheader.aspect, "user defined" );
			
			/* תΪMOTO���� */
			_swap_byte(&rlaheader);

			/* ���ý���ֵ */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			/* д��ͷ�ṹ */
			if (isio_write((const void *)&rlaheader, sizeof(RLA_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ����ƫ�Ʊ� */
			if ((poffset=(unsigned int *)malloc(4*pinfo_str->height)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ����ѹ�����ݻ����� */
			if ((ppack=(unsigned char *)malloc(pinfo_str->width*(pinfo_str->bitcount/8)*2)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			pr = ppack;
			pg = pr+pinfo_str->width*2;
			pb = pg+pinfo_str->width*2;
			
			/* ���䵥ͨ�������� */
			if ((pspl=(unsigned char *)malloc(pinfo_str->width+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ��λͨ������д�����ʼ�㣨���ͷ�ṹ��ƫ�Ʊ� */
			if (isio_seek(pfile, sizeof(RLA_HEADER)+(4*pinfo_str->height), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			off = isio_tell(pfile);

			/* ����д�� */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				p = (unsigned char *)(pinfo_str->pp_line_addr[i]);

				poffset[(int)pinfo_str->height-i-1] = EXCHANGE_DWORD(off);

				/* ��ȡ R ͨ������ */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pspl[j] = p[j*3+2];
				}
				/* ѹ�� R ͨ������ */
				rl = (unsigned short)_encode_scanline(pspl, (int)pinfo_str->width, pr);

				/* ��ȡ G ͨ������ */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pspl[j] = p[j*3+1];
				}
				/* ѹ�� G ͨ������ */
				gl = (unsigned short)_encode_scanline(pspl, (int)pinfo_str->width, pg);
				
				/* ��ȡ B ͨ������ */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pspl[j] = p[j*3+0];
				}
				/* ѹ�� B ͨ������ */
				bl = (unsigned short)_encode_scanline(pspl, (int)pinfo_str->width, pb);
				

				/* д��ѹ��������ݣ�˳��Ϊ��R��G��B */
				l = EXCHANGE_WORD(rl);
				if (isio_write((const void *)&l, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_write((const void *)pr, rl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				l = EXCHANGE_WORD(gl);
				if (isio_write((const void *)&l, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_write((const void *)pg, gl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				l = EXCHANGE_WORD(bl);
				if (isio_write((const void *)&l, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_write((const void *)pb, bl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			
				pinfo_str->process_current = i;
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
				
				/* ��ȡ��һ�е�д��λ�� */
				off = isio_tell(pfile);
			}
			
			/* ��λƫ�Ʊ�д��� */
			if (isio_seek(pfile, sizeof(RLA_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* д��ƫ�Ʊ� */
			if (isio_write((const void *)poffset, 4*pinfo_str->height, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pspl)
				free(pspl);
			
			if (ppack)
				free(ppack);

			if (poffset)
					free(poffset);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&rla_save_img_critical);
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
int CALLAGREEMENT _rla_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* λ����һ������32λ��ʽ�޷���ʾ����LONGΪ32λ��*/
	if (!(rla_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (rla_irwp_info.irwp_save.img_num)
	{
		if (rla_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* ͼ���������ȷ */
			}
		}
	}

	return 0;
}


/* ��RLA_HEADER�ṹ�е�short���������ת�� */
static void CALLAGREEMENT _swap_byte(LPRLA_HEADER prla)
{
	assert(prla);
	
	prla->window.bottom = EXCHANGE_WORD(prla->window.bottom);
	prla->window.left   = EXCHANGE_WORD(prla->window.left);
	prla->window.right  = EXCHANGE_WORD(prla->window.right);
	prla->window.top    = EXCHANGE_WORD(prla->window.top);

	prla->active_window.bottom = EXCHANGE_WORD(prla->active_window.bottom);
	prla->active_window.left   = EXCHANGE_WORD(prla->active_window.left);
	prla->active_window.right  = EXCHANGE_WORD(prla->active_window.right);
	prla->active_window.top    = EXCHANGE_WORD(prla->active_window.top);

	prla->frame         = EXCHANGE_WORD(prla->frame);
	prla->storage_type  = EXCHANGE_WORD(prla->storage_type);
	prla->num_chan      = EXCHANGE_WORD(prla->num_chan);
	prla->num_matte     = EXCHANGE_WORD(prla->num_matte);
	prla->num_aux       = EXCHANGE_WORD(prla->num_aux);
	prla->revision      = EXCHANGE_WORD(prla->revision);
	prla->job_num       = EXCHANGE_DWORD(prla->job_num);
	prla->field         = EXCHANGE_WORD(prla->field);
	prla->chan_bits     = EXCHANGE_WORD(prla->chan_bits);
	prla->matte_type    = EXCHANGE_WORD(prla->matte_type);
	prla->matte_bits    = EXCHANGE_WORD(prla->matte_bits);
	prla->aux_type      = EXCHANGE_WORD(prla->aux_type);
	prla->aux_bits      = EXCHANGE_WORD(prla->aux_bits);
	prla->next          = EXCHANGE_DWORD(prla->next);
}


/* ����һ��RLE8��ͨ��ѹ�����ݣ�һ��ɨ�����е�һ��ͨ���� */
static int CALLAGREEMENT _decode_scanline(unsigned char *src, int src_len, unsigned char *dec, int dec_len)
{
	unsigned char count;

	assert(src&&dec&&src_len&&dec_len);

	while (src_len > 0)
	{
		/* ȡ����ֵ */
		count = *src++;
		src_len--;

		if (src_len == 0)
		{
			return 2;			/* Դ���ݲ��� */
		}

		if (count < (unsigned char)128)
		{
			count++;

			/* �Ϸ��Լ�� */
			if (((int)(unsigned int)count) > dec_len)
			{
				return 1;		/* Դ���ݷǷ� */
			}
			else
			{
				dec_len -= (int)(unsigned int)count;
			}

			/* �ظ������� */
			while (count-- > 0)
			{
				*dec++ = *src;
			}
			src++;
			src_len--;
		}
		else
		{
			/* ����δѹ�������� */
			for (count=(unsigned char)(256-(int)(unsigned int)count); count > 0; src_len--, dec_len--, count--)
			{
				if (dec_len == 0)
				{
					return 1;
				}
				else
				{
					*dec++ = *src++;
				}
			}
		}
	}

	if (dec_len != 0)
	{
		return 3;			/* Դ������ȱ�� */
	}
	else
	{
		return 0;			/* �ɹ���ѹ�� */
	}
}


/* ѹ��һ��ͨ�������ݣ�һ��ɨ�����е�һ��ͨ���� */
static int CALLAGREEMENT _encode_scanline(unsigned char *src, int src_len, unsigned char *dec)
{
	int				len = 0;
	unsigned char	count;
	
	assert(src&&src_len&&dec);

	while (src_len > 0)
	{
		if ((src_len>1)&&(src[0]==src[1]))
		{
			/* �ۼ��ظ��ֽڸ��� */
			for (count=2; (int)(unsigned int)count<src_len; count++)
			{
				if (src[(int)(unsigned int)count] != src[(int)(unsigned int)count-1])
				{
					break;
				}
				if ((int)(unsigned int)count >= 127)
				{
					break;
				}
			}
			
			/* д�����ֵ */
			*dec++ = count - (unsigned char)1;
			len++;
			
			/* д���ظ�����ֵ */
			*dec++ = *src;
			len++;

			src_len -= (int)(unsigned int)count;
			src += (int)(unsigned int)count;
		}
		else
		{
			/* �ۼƲ��ظ��ֽڸ��� */
			for (count=1; (int)(unsigned int)count<src_len; count++)
			{
				if (((src_len-(int)(unsigned int)count)>1) && (src[(int)(unsigned int)count]==src[(int)(unsigned int)count+1]))
				{
					break;
				}
				if ((int)(unsigned int)count >= 127)
				{
					break;
				}
			}
			
			/* д�����ֵ */
			*dec++ = (unsigned char)((int)256 - (int)(unsigned int)count);
			len++;
			
			/* ����δѹ������ */
			for (; count-->0; len++, src_len--)
			{
				*dec++ = *src++;
			}
		}
	}
	
	return len;	/* ����ѹ�����ͨ�����ݳ��� */
}




