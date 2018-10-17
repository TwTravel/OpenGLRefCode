/********************************************************************

	pcx.c

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
	���ļ���;��	ISeeͼ���������PCXͼ���дģ��ʵ���ļ�
	
					��ȡ���ܣ�1��4��8��24λͼ��
					���湦�ܣ�1��4��8��24λͼ��
	  
	���ļ���д�ˣ�	freedvlp	feelfree##263.net���������д�ˣ�
					ChK			ChK##163.net��д�����д�ˣ�
					YZ			yzfree##sina.com���İ湤����
		
	���ļ��汾��	10818
	����޸��ڣ�	2001-8-18
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-8		�����°���롣����չ�˸�ģ��ı��湦�ܣ�
					��ǿ��ģ���ݴ��ԣ������ģ��Ķ�д�ٶȡ�
			  
		2001-4		�����ͼ��ı��湦�ܣ�24λ��
		2000-8		����˶�ȡͼ�񲿷ֵ�ȫ������
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

#include "pcx.h"


IRWP_INFO			pcx_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	pcx_get_info_critical;	/* pcx_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	pcx_load_img_critical;	/* pcx_load_image�����Ĺؼ��� */
CRITICAL_SECTION	pcx_save_img_critical;	/* pcx_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _pcx_is_valid_img(LPINFOSTR pinfo_str);

char * CALLAGREEMENT _read_line(char *p_buf, char *p_sou, int linesize, char *p_base, int data_size);
int	   CALLAGREEMENT _write_line(char *p_buf, char *p_sou, int linesize);
void   CALLAGREEMENT _get_palette(LPINFOSTR pinfo_str, PCX_HEADER *p_header, ISFILE *pfile);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&pcx_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&pcx_get_info_critical);
			InitializeCriticalSection(&pcx_load_img_critical);
			InitializeCriticalSection(&pcx_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&pcx_get_info_critical);
			DeleteCriticalSection(&pcx_load_img_critical);
			DeleteCriticalSection(&pcx_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PCX_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pcx_irwp_info;
}

#else

PCX_API LPIRWP_INFO CALLAGREEMENT pcx_get_plugin_info()
{
	_init_irwp_info(&pcx_irwp_info);

	return (LPIRWP_INFO)&pcx_irwp_info;
}

PCX_API void CALLAGREEMENT pcx_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PCX_API void CALLAGREEMENT pcx_detach_plugin()
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
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(4-1)) | (1UL<<(8-1)) | (1UL<<(24-1));

	lpirwp_info->irwp_save.img_num = 1;	/* 1��ֻ�ܱ���һ��ͼ�� */
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 3;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"freedvlp");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"feelfree##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"^_^");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"ChK");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"ChK##163.net");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"^_^");
	/* ---------------------------------[2] �� ������ -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_email), 
		(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_message), 
		(const char *)"^_^");
	/* ---------------------------------[3] �� ������ -------------- */
	/* ������������Ϣ�ɼ��ڴ˴���
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_message), 
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
PCX_API int CALLAGREEMENT pcx_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCX_HEADER		phr;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&pcx_get_info_critical);

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

			if (isio_read((void*)&phr, sizeof(PCX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			

			/* ����ļ���־ */
			if (phr.byManufacturer != (unsigned char)PCX_FLAG)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* ������Ч�Լ��� */
			if (((phr.wRight-phr.wLeft)==0)||((phr.wBottom-phr.wTop)==0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_PCX;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression  = ICS_PCXRLE;		/* ͼ���ѹ����ʽ */
			
			/* ��дͼ����Ϣ */
			pinfo_str->width	= (unsigned long)((phr.wRight-phr.wLeft)+1);
			pinfo_str->height	= (unsigned long)((phr.wBottom-phr.wTop)+1);
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= phr.byBits*phr.byPlanes;
			
			/* ��д�������� */
			if (pinfo_str->bitcount > 8)	/* 24λ */
			{
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
			}
			else
			{
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

			LeaveCriticalSection(&pcx_get_info_critical);
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
PCX_API int CALLAGREEMENT pcx_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCX_HEADER		phr;
	int				i, j, k, linesize, bufsize;
	unsigned long	data_len;
	
	unsigned char	buf, tmp;
	unsigned short	us;
	int				cnv_count, bit, bitmask;
	
	unsigned char	*pbit = (unsigned char *)0;
	unsigned char	*ptmp = (unsigned char *)0;
	unsigned char	*plinebuf = (unsigned char *)0;

	unsigned char	*psou, *pdesline;
	unsigned char	*pla3, *pla2, *pla1, *pla0;
	unsigned char	pb3, pb2, pb1, pb0, tbuf, tofs;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pcx_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total = 1;
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

			if (isio_read((void*)&phr, sizeof(PCX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			
			/* ����ļ���־ */
			if (phr.byManufacturer != (unsigned char)PCX_FLAG)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* ������Ч�Լ��� */
			if (((phr.wRight-phr.wLeft)==0)||((phr.wBottom-phr.wTop)==0))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_PCX;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression  = ICS_PCXRLE;		/* ͼ���ѹ����ʽ */
				
				/* ��дͼ����Ϣ */
				pinfo_str->width	= (unsigned long)((phr.wRight-phr.wLeft)+1);
				pinfo_str->height	= (unsigned long)((phr.wBottom-phr.wTop)+1);
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= phr.byBits*phr.byPlanes;
				
				/* ��д�������� */
				if (pinfo_str->bitcount > 8)	/* 24λ */
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else
				{
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					pinfo_str->a_mask = 0x0;
				}
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}


			/* ���õ�ɫ������ */
			_get_palette(pinfo_str, &phr, pfile);


			/* ȡ��ͼ�����ݳ��� */
			data_len = isio_length(pfile) - sizeof(PCX_HEADER);

			if ((pinfo_str->bitcount == 8)&&(phr.wPaletteType == 1))
				data_len -= 256*3+1;		/* +1 : 0xc0

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

			/* ��ʼ�����׵�ַ���飨PCXͼ��Ϊ���� */
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));


			bufsize = (int)phr.wLineBytes * phr.byPlanes;

			/* �������ڴ����ʱ��ѹ���ݵĻ����� */
			plinebuf = (unsigned char *)malloc(bufsize);
			if (plinebuf == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}


			/* ��ȡλ���ݵ��ڴ� */
			if (isio_seek(pfile, sizeof(PCX_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ������ʱ�ڴ�飬���ڴ������PCXͼ������ */
			ptmp = (unsigned char *)malloc(data_len);
			if (ptmp == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			/* ��ԭʼͼ������ */
			if (isio_read((void*)ptmp, data_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total = pinfo_str->height;
			psou = ptmp;
			pbit = pinfo_str->p_bit_data;

			/* ���ж��� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				if ((psou=_read_line((char *)plinebuf, (char *)psou, bufsize, (char *)ptmp, data_len)) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* Ŀ�껺������ַ */
				pdesline = (unsigned char *)pinfo_str->pp_line_addr[i];

				switch (pinfo_str->bitcount)
				{
				case	1:
					/* 1λͼ�����ݷ�ת�������ֽڵͶ˶�Ӧ���ͼ������ */
					for (cnv_count=0;cnv_count<bufsize;cnv_count++)
					{
						buf = *(unsigned char *)(plinebuf+cnv_count);
						us  = ((unsigned short)buf)<<8;
						tmp = 0;
						for (bit=1, bitmask=7;bit<16;bit+=2, bitmask--)
							tmp |= (us>>bit)&(1<<bitmask);
						
						*pdesline++ = tmp;
					}
					break;
				case	2:
				case	3:
					/* ���޸ġ���δ�ҵ���ص�����������ϣ�����Ϊ1����壩*/
					memcpy((void*)pdesline, (const void *)plinebuf, bufsize);
					break;
				case	4:
					/* 1λ��4��� */
					pla0 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*0));
					pla1 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*1));
					pla2 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*2));
					pla3 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*3));

					for (k=0;k<(int)pinfo_str->width;k+=8)
					{
						pb0 = *pla0++; pb1 = *pla1++; 
						pb2 = *pla2++; pb3 = *pla3++;

						/* ��λΪ��ߵ����� */
						for (j=7;(j>=0)&&((k+(7-j))<(int)pinfo_str->width);j--)
						{
							tofs = 1 << j; tbuf = 0;

							tbuf |= (pb0&tofs) ? 0x1 : 0;
							tbuf |= (pb1&tofs) ? 0x2 : 0;
							tbuf |= (pb2&tofs) ? 0x4 : 0;
							tbuf |= (pb3&tofs) ? 0x8 : 0;

							/* ISeeλ����ʽ�����������ֽڵ�λ */
							if (j%2)
								*pdesline = tbuf;
							else
							{
								tbuf <<= 4;
								*pdesline++ |= tbuf;
							}
						}
					}
					break;
				case	8:
					/* 8λͼ������ֱ�ӿ��� */
					memcpy((void*)pdesline, (const void *)plinebuf, bufsize);
					break;
				case	24:
					/* 8λ��3��� */
					pla0 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*0));
					pla1 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*1));
					pla2 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*2));
					
					/* BGR˳�� */
					for (k=0;k<(int)pinfo_str->width;k++)
					{
						*pdesline++ = *pla2++;	/* B */
						*pdesline++ = *pla1++;	/* G */
						*pdesline++ = *pla0++;	/* R */
					}
					break;
				default:
					b_status = ER_BADIMAGE; __leave;
				}

				/* ���� */
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
			}

			if (plinebuf)
				free(plinebuf);
			if (ptmp)
				free(ptmp);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pcx_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
PCX_API int CALLAGREEMENT pcx_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCX_HEADER		phr;

	int				i, j, k, expsize, commsize;

	unsigned char	*psou, *pdes;

	unsigned char   *pexpend = (unsigned char *)0;
	unsigned char   *pexpend2= (unsigned char *)0;
	unsigned char   *pdesbuf = (unsigned char *)0;

	LPPCX_PALITEM	p8pal = (LPPCX_PALITEM)0;

	unsigned char	*pla3, *pla2, *pla1, *pla0;
	unsigned char	pb3, pb2, pb1, pb0, tbuf;
	
	unsigned char	buf, tmp;
	unsigned short	us;
	int				cnv_count, bit, bitmask;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pcx_save_img_critical);
	
			/* �ж��Ƿ�����֧�ֵ�ͼ������ */
			if (_pcx_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* �������ͬ��������������������ȷ����ʾ */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ����PCXͼ����Ϣ�ṹ */
			phr.byManufacturer = (unsigned char)PCX_FLAG;

			phr.byVersion  = 5;
			phr.byEncoding = 1;
			phr.wLeft      = 0;
			phr.wTop       = 0;
			assert(pinfo_str->width); 
			assert(pinfo_str->height);
			phr.wRight     = (unsigned short)(pinfo_str->width - 1);
			phr.wBottom    = (unsigned short)(pinfo_str->height - 1);

			phr.byReserved   = 0;

			/* ȱʡ����ֵ�����޸� */
			phr.wXResolution = 300;	/* DPI */
			phr.wYResolution = 300;
			phr.wScrWidth    = 800; /* PIXEL */
			phr.wScrDepth    = 600;

			memset((void*)phr.byFiller, 0, 54);
			
			switch (pinfo_str->bitcount)
			{
			case	1:
				phr.byBits = 1;
				phr.byPlanes = 1;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+15)/16)*2);
				phr.wPaletteType = 1;	/* ��ɫ */

				for (i=0;i<2;i++)
				{
					phr.ptPalette[i].blue  = ((LPPALITEM)(&pinfo_str->palette[i]))->blue;
					phr.ptPalette[i].green = ((LPPALITEM)(&pinfo_str->palette[i]))->green;
					phr.ptPalette[i].red   = ((LPPALITEM)(&pinfo_str->palette[i]))->red;
				}

				expsize = phr.wLineBytes*1;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)0;
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*1*2);	/* �����������������ڴ� */
				if ((pexpend==0)||(pdesbuf==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				break;
			case	4:
				phr.byBits = 1;
				phr.byPlanes = 4;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+15)/16)*2);
				phr.wPaletteType = 1;	/* ��ɫ */
				
				for (i=0;i<16;i++)
				{
					phr.ptPalette[i].blue  = ((LPPALITEM)(&pinfo_str->palette[i]))->blue;
					phr.ptPalette[i].green = ((LPPALITEM)(&pinfo_str->palette[i]))->green;
					phr.ptPalette[i].red   = ((LPPALITEM)(&pinfo_str->palette[i]))->red;
				}

				expsize = phr.wLineBytes*4;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)malloc(DIBSCANLINE_WIDTHBYTES(pinfo_str->width*4));
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*4*2);
				if ((pexpend==0)||(pdesbuf==0)||(pexpend2==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				break;
			case	8:
				phr.byBits = 8;
				phr.byPlanes = 1;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+1)/2)*2);
				phr.wPaletteType = 0;	/* ��ɫ */
				
				expsize = phr.wLineBytes*1;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)0;
				p8pal   = (LPPCX_PALITEM)malloc(sizeof(PCX_PALITEM)*256);
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*1*2);
				if ((pexpend==0)||(pdesbuf==0)||(p8pal==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				/* ��ɫ�����ݽ���дͼ�����ݺ�ϳ� */
				break;
			case	24:
				phr.byBits = 8;
				phr.byPlanes = 3;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+1)/2)*2);
				phr.wPaletteType = 0;	/* ��ɫ */
				
				expsize = phr.wLineBytes*3;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)malloc(DIBSCANLINE_WIDTHBYTES(pinfo_str->width*3*8));
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*3*2);
				if ((pexpend==0)||(pdesbuf==0)||(pexpend2==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				/* �޵�ɫ������ */
				break;
			default:
				assert(0);
				break;
			}


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
			
			/* д����Ϣ�ṹ */
			if (isio_write((const void *)&phr, sizeof(PCX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* д��ͼ������ 
				ע�������ͼ�����ݱ�����ISeeλ����ʽ��������ܻ����ʧ��
			*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				psou = (unsigned char *)pinfo_str->pp_line_addr[i];
				pdes = pexpend;

				/* չ��Ϊ����ʽ */
				switch (pinfo_str->bitcount)
				{
				case	1:
					for (cnv_count=0;cnv_count<phr.wLineBytes;cnv_count++)
					{
						buf = *(unsigned char *)(psou+cnv_count);
						us  = ((unsigned short)buf)<<8;
						tmp = 0;
						for (bit=1, bitmask=7;bit<16;bit+=2, bitmask--)
							tmp |= (us>>bit)&(1<<bitmask);
						
						*pdes++ = tmp;
					}
					break;
				case	4:
					/* �ֽ���˳��ת */
					for (cnv_count=0;cnv_count<(int)((pinfo_str->width+1)/2);cnv_count++)
					{
						buf = *(unsigned char *)(psou+cnv_count);
						tmp = (buf>>4)|(buf<<4);
						*(unsigned char *)(pexpend2+cnv_count) = tmp;
					}
					
					/* ��ʼ��չ�������� */
					memset((void*)pexpend, 0, phr.wLineBytes*4);

					pla0 = (unsigned char *)(pexpend+(phr.wLineBytes*0));
					pla1 = (unsigned char *)(pexpend+(phr.wLineBytes*1));
					pla2 = (unsigned char *)(pexpend+(phr.wLineBytes*2));
					pla3 = (unsigned char *)(pexpend+(phr.wLineBytes*3));

					/* ��ͼ������չ��Ϊ4����� */
					for (k=0, j=7;k<(int)pinfo_str->width;k++, j--)
					{
						tbuf = *(unsigned char *)(pexpend2+k/2);

						if (k%2)
							tbuf &= 0xf;
						else
							tbuf >>= 4;

						pb0 = ((tbuf&0x1)>>0)<<j;
						pb1 = ((tbuf&0x2)>>1)<<j;
						pb2 = ((tbuf&0x4)>>2)<<j;
						pb3 = ((tbuf&0x8)>>3)<<j;

						*pla0 |= pb0;
						*pla1 |= pb1;
						*pla2 |= pb2;
						*pla3 |= pb3;

						if (j == 0)
						{
							j = 8; pla0++; pla1++; pla2++; pla3++;
						}
					}
					break;
				case	8:
					/* 8λͼ��ֱ�ӿ��� */
					memcpy((void*)pdes, (const void *)psou, phr.wLineBytes);
					break;
				case	24:
					/* �ֽ���˳��ת */
					for (cnv_count=0;cnv_count<(int)pinfo_str->width;cnv_count++)
					{
						*(unsigned char *)(pexpend2+cnv_count*3+2) = *(unsigned char *)(psou+cnv_count*3+0);
						*(unsigned char *)(pexpend2+cnv_count*3+1) = *(unsigned char *)(psou+cnv_count*3+1);
						*(unsigned char *)(pexpend2+cnv_count*3+0) = *(unsigned char *)(psou+cnv_count*3+2);
					}

					/* ��ʼ��չ�������� */
					memset((void*)pexpend, 0, phr.wLineBytes*3);
					
					pla0 = (unsigned char *)(pexpend+(phr.wLineBytes*0)); /*R*/
					pla1 = (unsigned char *)(pexpend+(phr.wLineBytes*1)); /*G*/
					pla2 = (unsigned char *)(pexpend+(phr.wLineBytes*2)); /*B*/
					
					/* ��ͼ������չ��Ϊ3�����--RGB */
					for (k=0;k<(int)pinfo_str->width;k++)
					{
						*pla0++ = *(unsigned char *)(pexpend2+k*3+0);
						*pla1++ = *(unsigned char *)(pexpend2+k*3+1);
						*pla2++ = *(unsigned char *)(pexpend2+k*3+2);
					}
					break;
				}

				/* ʹ��RLE�㷨��չ����������ݽ���ѹ�� */
				commsize = _write_line((char*)pdesbuf, (char*)pexpend, expsize);

				/* д��Ŀ���� */
				if (isio_write((const void*)pdesbuf, commsize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* �����8λͼ����������������256ɫ��ɫ������ */
				if ((pinfo_str->bitcount == 8)&&((i+1)==(int)(pinfo_str->height)))
				{
					pdesbuf[0] = (unsigned char)0xc;	/* 0xc Ϊ��־�� */
					if (isio_write((const void*)pdesbuf, 1, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					for (k=0;k<256;k++)
					{
						p8pal[k].blue  = ((LPPALITEM)(&pinfo_str->palette[k]))->blue;
						p8pal[k].green = ((LPPALITEM)(&pinfo_str->palette[k]))->green;
						p8pal[k].red   = ((LPPALITEM)(&pinfo_str->palette[k]))->red;
					}

					if (isio_write((const void*)p8pal, sizeof(PCX_PALITEM)*256, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
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
		}
		__finally
		{
			if (p8pal)
				free(p8pal);
			if (pexpend2)
				free(pexpend2);
			if (pexpend)
				free(pexpend);
			if (pdesbuf)
				free(pdesbuf);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pcx_save_img_critical);
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

/* ����һ��PCX_RLE���ݣ������뵽ָ���Ļ������� */
char * CALLAGREEMENT _read_line(char *p_buf, char *p_sou, int linesize, char *p_base, int data_size)
{
	int cur_offset = 0;
	unsigned char count, conv;
	
	while(cur_offset < linesize)
	{
		if((p_sou-p_base) >= data_size)
		{
			p_sou = (char *)0; break;
		}

		count = (unsigned char)*p_sou++;

		if((count&0xc0) == 0xc0)			/* ����ֵ */
		{
			count &= 0x3f;
			conv = (unsigned char)*p_sou++;	/* ���� */

			while(count--)
				p_buf[cur_offset++] = conv;	/* �ظ�д������ */
		}
		else								/* ���� */
			p_buf[cur_offset++] = count;
	}
	
	return p_sou;	/* ������һ�еĶ�ȡ��ַ������������ݴ�����0 */
}


/* дһ�����ݵ�PCX_RLE������ */
int CALLAGREEMENT _write_line(char *p_buf, char *p_sou, int linesize)
{
	int				m = 0;
	int				commsize = 0;
	unsigned char	v, n;
	
	while(m < linesize)
	{
		v = p_sou[m];
		
		/* ȡ����������ͬ���ֽ���n */
		n = 1;
		while (((m+n)<linesize) && (n<64) && (v==(unsigned char)(p_sou[m+n])))
			n++;

		if (n > 63 )
			n = 63;
		
		if (n == 1)		/* δ��ѹ�� */
		{
			if (v < 0xc0)
				p_buf[commsize++] = v;
			else
			{
				p_buf[commsize++] = (char)0xc1;
				p_buf[commsize++] = v;
			}
		}
		else			/* ѹ�� */
		{
			p_buf[commsize++] = (char)(n+0xc0);
			p_buf[commsize++] = v;
		}
		m += n;
	}

	return commsize;	/* ѹ����������гߴ� */
}
	

/* ��ȡ��ɫ������ */
void CALLAGREEMENT _get_palette(LPINFOSTR pinfo_str, PCX_HEADER *p_header, ISFILE *pfile)
{
	int i;

	PCX_PALITEM		tmp_palbuf[256];
#ifdef WIN32
	PALETTEENTRY	palcolor[256];
	HDC				hdc;
#endif

	assert(pfile);

	if (pinfo_str->bitcount <= 8)
	{
		pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

		if (p_header->wPaletteType <= 1)		/* ��ɫ��ɫ��YZ: 0������Ϊ��ɫ�� */
		{
			if (p_header->byVersion == 3)		/* ʹ��ϵͳ��ɫ�� */
			{
#ifdef WIN32
				hdc = GetDC((HWND)0);
				GetSystemPaletteEntries(hdc, 0, pinfo_str->pal_count, (LPPALETTEENTRY)palcolor);
				ReleaseDC((HWND)0, hdc);

				for (i=0;i<(int)pinfo_str->pal_count;i++)
				{
					((LPPALITEM)(&pinfo_str->palette[i]))->blue = palcolor[i].peBlue;
					((LPPALITEM)(&pinfo_str->palette[i]))->green= palcolor[i].peGreen;
					((LPPALITEM)(&pinfo_str->palette[i]))->red  = palcolor[i].peRed;
					((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
				}
#else
				/* ȱʡΪ�Ҷȵ�ɫ�壬�ɸ���ΪLinux GUIϵͳ��ȱʡ��ɫ�� */
				switch (pinfo_str->bitcount)
				{
				case	1:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0xffffff;
					break;
				case	2:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0x555555;
					pinfo_str->palette[2] = 0xaaaaaa;
					pinfo_str->palette[3] = 0xffffff;
					break;
				case	3:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0x242424;
					pinfo_str->palette[2] = 0x484848;
					pinfo_str->palette[3] = 0x6c6c6c;
					pinfo_str->palette[4] = 0x909090;
					pinfo_str->palette[5] = 0xb4b4b4;
					pinfo_str->palette[6] = 0xd8d8d8;
					pinfo_str->palette[7] = 0xffffff;
					break;
				case	4:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0x111111;
					pinfo_str->palette[2] = 0x222222;
					pinfo_str->palette[3] = 0x333333;
					pinfo_str->palette[4] = 0x444444;
					pinfo_str->palette[5] = 0x555555;
					pinfo_str->palette[6] = 0x666666;
					pinfo_str->palette[7] = 0x777777;
					pinfo_str->palette[8] = 0x888888;
					pinfo_str->palette[9] = 0x999999;
					pinfo_str->palette[10] = 0xaaaaaa;
					pinfo_str->palette[11] = 0xbbbbbb;
					pinfo_str->palette[12] = 0xcccccc;
					pinfo_str->palette[13] = 0xdddddd;
					pinfo_str->palette[14] = 0xeeeeee;
					pinfo_str->palette[15] = 0xffffff;
					break;
				case	8:
					for (i=0;i<(int)256;i++)
						pinfo_str->palette[i] = i|i<<8|i<<16;
					break;
				}
#endif
			}
			else							/* �Դ���ɫ�� */
			{
				if (pinfo_str->bitcount == 8)
				{
					__try
					{
						/* ��ȡ256ɫ��ɫ�����ݣ���β����*/
						isio_seek(pfile, -256L*3, SEEK_END);
						isio_read((void*)tmp_palbuf, 256*3, 1, pfile);
					}
					__except(EXCEPTION_CONTINUE_SEARCH)
					{
						;
					}

					/* ���Ƶ�ɫ�����ݣ�8λ�� */
					for (i=0;i<(int)pinfo_str->pal_count;i++)
					{
						((LPPALITEM)(&pinfo_str->palette[i]))->blue = tmp_palbuf[i].blue;
						((LPPALITEM)(&pinfo_str->palette[i]))->green= tmp_palbuf[i].green;
						((LPPALITEM)(&pinfo_str->palette[i]))->red  = tmp_palbuf[i].red;
						((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
					}
				}
				else
				{	/* ���Ƶ�ɫ�����ݣ�1��2��3��4λ�� */
					for (i=0;i<(int)pinfo_str->pal_count;i++)
					{
						((LPPALITEM)(&pinfo_str->palette[i]))->blue  = p_header->ptPalette[i].blue;
						((LPPALITEM)(&pinfo_str->palette[i]))->green = p_header->ptPalette[i].green;
						((LPPALITEM)(&pinfo_str->palette[i]))->red   = p_header->ptPalette[i].red;
						((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
					}
				}
			}
		}
		else								/* �Ҷȣ�2�� */
		{
			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0xffffff;
				break;
			case	2:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0x555555;
				pinfo_str->palette[2] = 0xaaaaaa;
				pinfo_str->palette[3] = 0xffffff;
				break;
			case	3:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0x242424;
				pinfo_str->palette[2] = 0x484848;
				pinfo_str->palette[3] = 0x6c6c6c;
				pinfo_str->palette[4] = 0x909090;
				pinfo_str->palette[5] = 0xb4b4b4;
				pinfo_str->palette[6] = 0xd8d8d8;
				pinfo_str->palette[7] = 0xffffff;
				break;
			case	4:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0x111111;
				pinfo_str->palette[2] = 0x222222;
				pinfo_str->palette[3] = 0x333333;
				pinfo_str->palette[4] = 0x444444;
				pinfo_str->palette[5] = 0x555555;
				pinfo_str->palette[6] = 0x666666;
				pinfo_str->palette[7] = 0x777777;
				pinfo_str->palette[8] = 0x888888;
				pinfo_str->palette[9] = 0x999999;
				pinfo_str->palette[10] = 0xaaaaaa;
				pinfo_str->palette[11] = 0xbbbbbb;
				pinfo_str->palette[12] = 0xcccccc;
				pinfo_str->palette[13] = 0xdddddd;
				pinfo_str->palette[14] = 0xeeeeee;
				pinfo_str->palette[15] = 0xffffff;
				break;
			case	8:
				for (i=0;i<(int)256;i++)
					pinfo_str->palette[i] = i|i<<8|i<<16;
				break;
			}
		}
	}
	else
	{
		pinfo_str->pal_count = 0;		/* �޵�ɫ������ */
	}
}


/* �жϴ����ͼ���Ƿ���Ա����� */
int CALLAGREEMENT _pcx_is_valid_img(LPINFOSTR pinfo_str)
{
	if (!(pcx_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* ��֧�ֵ�λ��ͼ�� */
	
	assert(pinfo_str->imgnumbers);
	
	if (pcx_irwp_info.irwp_save.img_num)
		if (pcx_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* ͼ���������ȷ */
			
	return 0;
}
