/********************************************************************

	pcd.c

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
			http://cosoft.org.cn/projects/iseeexplorer

	���ŵ���

			isee##vip.163.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������PCDͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��ɶ�ȡ24λPCDͼ��ǰ������
							  
					���湦�ܣ���֧��

	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net

	���ļ��汾��	20604
	����޸��ڣ�	2002-6-4

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-6		��һ�������棨�°棩

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

#include "pcd.h"


IRWP_INFO			pcd_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	pcd_get_info_critical;	/* pcd_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	pcd_load_img_critical;	/* pcd_load_image�����Ĺؼ��� */
CRITICAL_SECTION	pcd_save_img_critical;	/* pcd_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);

static enum EXERESULT CALLAGREEMENT _read_img(void *pinfo, int img_num, ISFILE *pfile);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);
static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK();
static LPSUBIMGBLOCK CALLAGREEMENT _allsub(int num, LPINFOSTR pinfo_str);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&pcd_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&pcd_get_info_critical);
			InitializeCriticalSection(&pcd_load_img_critical);
			InitializeCriticalSection(&pcd_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&pcd_get_info_critical);
			DeleteCriticalSection(&pcd_load_img_critical);
			DeleteCriticalSection(&pcd_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PCD_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pcd_irwp_info;
}

#else

PCD_API LPIRWP_INFO CALLAGREEMENT pcd_get_plugin_info()
{
	_init_irwp_info(&pcd_irwp_info);

	return (LPIRWP_INFO)&pcd_irwp_info;
}

PCD_API void CALLAGREEMENT pcd_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PCD_API void CALLAGREEMENT pcd_detach_plugin()
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

	/* ����ģ��֧�ֵı���λ�� */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num  = 0;
	lpirwp_info->irwp_save.count    = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 1;	


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"ACDSee��ĺ��б��£���֪�������Ǵ���Ū����PCD��ʽ����:)");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"pcds");
	
	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
PCD_API int CALLAGREEMENT pcd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	ord;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&pcd_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ȷ��PCDͼ�󳤶� */
			if (isio_seek(pfile, 0, SEEK_END) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �򵥵�ͨ��ͼ�󳤶���ȷ���Ƿ���PCDͼ�� */
			if (isio_tell(pfile) < PCD_MIN_SIZE)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ��ȡͼ������Ϣ */
			if (isio_seek(pfile, PCD_VERTICAL_LOC, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (isio_read((void*)&ord, sizeof(unsigned char), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_PCD;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression  = ICS_JPEGYCbCr;	/* ͼ���ѹ����ʽ */
			
			/* ��дͼ����Ϣ */
			pinfo_str->width	= PCD_1_W;				/* ȡ��һ��ͼ��ĳߴ����� */
			pinfo_str->height	= PCD_1_H;
			pinfo_str->order	= ((ord&PCD_VERTICAL_MASK) != 8);
			pinfo_str->bitcount	= PCD_DEF_BITCOUNT;		/* ��֧��8λ��PCD */
			
			/* ��д�������ݣ�24λ��*/
			SETMASK_24(pinfo_str);


			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pcd_get_info_critical);
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
PCD_API int CALLAGREEMENT pcd_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				linesize, i;
	unsigned char	ord;
	LPSUBIMGBLOCK	subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pcd_load_img_critical);

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

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				if (isio_seek(pfile, 0, SEEK_END) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* �򵥵�ͨ��ͼ�󳤶���ȷ���Ƿ���PCDͼ�� */
				if (isio_tell(pfile) < PCD_MIN_SIZE)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* ��ȡͼ������Ϣ */
				if (isio_seek(pfile, PCD_VERTICAL_LOC, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)&ord, sizeof(unsigned char), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_PCD;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression  = ICS_JPEGYCbCr;	/* ͼ���ѹ����ʽ */
				
				pinfo_str->width	= PCD_1_W;				/* ȡ��һ��ͼ��ĳߴ����� */
				pinfo_str->height	= PCD_1_H;
				pinfo_str->order	= ((ord&PCD_VERTICAL_MASK) != 8);
				pinfo_str->bitcount	= PCD_DEF_BITCOUNT;		/* ��֧��8λ��PCD */
				
				SETMASK_24(pinfo_str);

				pinfo_str->data_state = 1;
			}

			pinfo_str->process_total   = 3;		/* �����ʱֻ�ܶ�ȡǰ����ͼ�� */
			pinfo_str->process_current = 0;
			
			pinfo_str->pal_count = 0;

			/* ȡ��ɨ���гߴ� */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			
			/* ����ͼ����� */
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg	  = (LPSUBIMGBLOCK)0;
			
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
			if (pinfo_str->order == 0)		/* ���� */
			{
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
				}
			}
			else							/* ���� */
			{
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
				}
			}
			

			/* ������ͼ�� */
			if ((b_status=_read_img((void*)pinfo_str, 0, pfile)) != ER_SUCCESS)
			{
				__leave;
			}

			pinfo_str->process_current = 1;

			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			

			/* ����ڶ���ͼ����ӽڵ� */
			if ((subimg_tmp=_allsub(1, pinfo_str)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ����ڶ���ͼ�� */
			if ((b_status=_read_img((void*)subimg_tmp, 1, pfile)) != ER_SUCCESS)
			{
				__leave;
			}
			
			pinfo_str->process_current = 2;

			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			
			/* ���������ͼ����ӽڵ� */
			if ((subimg_tmp=_allsub(2, pinfo_str)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ���������ͼ�� */
			if ((b_status=_read_img((void*)subimg_tmp, 2, pfile)) != ER_SUCCESS)
			{
				__leave;
			}
			
			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}


			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* �ͷ���ͼ�� */
				if (pinfo_str->psubimg != 0)
				{
					while(subimg_tmp=pinfo_str->psubimg->next)
					{
						_free_SUBIMGBLOCK(pinfo_str->psubimg);
						pinfo_str->psubimg = subimg_tmp;
					}
					_free_SUBIMGBLOCK(pinfo_str->psubimg);
					pinfo_str->psubimg = 0;
				}

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
			
			LeaveCriticalSection(&pcd_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
PCD_API int CALLAGREEMENT pcd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&pcd_save_img_critical);
			b_status = ER_NOTSUPPORT;	/* ������ʱ��֧��д���� */			
		}
		__finally
		{
			LeaveCriticalSection(&pcd_save_img_critical);
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


/* ��ָ���������ӣ�ͼ��������ݰ� */
static enum EXERESULT CALLAGREEMENT _read_img(void *pinfo, int img_num, ISFILE *pfile)
{
	LPINFOSTR		pinfo_str = (LPINFOSTR)pinfo;
	LPSUBIMGBLOCK	psub_str  = (LPSUBIMGBLOCK)pinfo;

	int				w, h, bitcount, linesize, i, j;
	long			pos;
	unsigned char	*y1 = 0, *y2 = 0, *cbcr = 0, *pbits, *p;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pinfo&&pfile);
	assert((img_num < 3)&&(img_num >= 0));		/* �����ʱֻ�ܶ�ȡǰ����ͼ�� */

	__try
	{
		__try
		{
			/* ��ȡͼ��ߴ����� */
			if (img_num == 0)					/* ��һ��ͼ����ͼ��*/
			{
				w = (int)(pinfo_str->width);
				h = (int)(pinfo_str->height);
				bitcount = (int)(pinfo_str->bitcount);
				pbits = (unsigned char *)(pinfo_str->p_bit_data);
			}
			else								/* ����ͼ����ͼ��*/
			{
				w = (int)(psub_str->width);
				h = (int)(psub_str->height);
				bitcount = (int)(psub_str->bitcount);
				pbits = (unsigned char *)(psub_str->p_bit_data);
			}
			
			/* ����ɨ���гߴ� */
			linesize  = _calcu_scanline_size(w, bitcount);

			/* ����ϳɻ����� */
			y1   = (unsigned char *)malloc(w);
			y2   = (unsigned char *)malloc(w);
			cbcr = (unsigned char *)malloc(w);

			if ((y1==0)||(y2==0)||(cbcr==0))
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ����ͼ������ƫ�� */
			switch (img_num)
			{
			case	0:
				pos = PCD_1_OFF; break;
			case	1:
				pos = PCD_2_OFF; break;
			case	2:
				pos = PCD_3_OFF; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			/* �趨��ȡλ�� */
			if (isio_seek(pfile, pos, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* PCDͼ��߶�һ����ż�� */
			assert((h%2) == 0);

			/* ���ж������� */
			for (i=0; i<h; i+=2)
			{
				unsigned char *r, *g, *b;

				/* ��ȡ���е�YCbCr���ݣ�CbCr���ݺ��ã� */
				if (isio_read((void*)y1, w, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)y2, w, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)cbcr, w, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ȷ������д��λ�� */
				p = pbits+(i+0)*linesize;

				/* ת��YUV������RGB���� */
				for (j=0; j<w; j++)
				{
					b = p+j*3+0;
					g = p+j*3+1;
					r = p+j*3+2;
					YUV2RGB(y1[j], cbcr[j/2], cbcr[(w/2)+(j/2)], r, g, b);
				}

				/* ָ����һ�е�ַ */
				p = pbits+(i+1)*linesize;

				/* ת��YUV������RGB���� */
				for (j=0; j<w; j++)
				{
					b = p+j*3+0;
					g = p+j*3+1;
					r = p+j*3+2;
					YUV2RGB(y2[j], cbcr[j/2], cbcr[(w/2)+(j/2)], r, g, b);
				}
			}

		}
		__finally
		{
			if (y1)
				free(y1);
			if (y2)
				free(y2);
			if (cbcr)
				free(cbcr);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}

	return b_status;
}


/* ����һ����ͼ��ڵ㣬����ʼ���ڵ������� */
static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK()
{
	LPSUBIMGBLOCK	p_subimg;
	
	/* ������ͼ����Ϣ�ڵ�(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* �ڴ治�� */
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	p_subimg->number   = 1;			/* ��ͼ����ʼ���Ϊ 1 */
	p_subimg->colorkey = -1;		/* ͸��ɫֵ��Ϊ�� */
	
	return p_subimg;
}



/* �ͷ�һ����ͼ��ڵ㣬�������е�λ�����������׵�ַ���� */
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);
	assert(p_node->number > 0);
	
	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);
	
	if (p_node->p_bit_data)
		free(p_node->p_bit_data);
	
	free(p_node);
}


/* ���벢��ʼ����ͼ��ڵ� */
static LPSUBIMGBLOCK CALLAGREEMENT _allsub(int num, LPINFOSTR pinfo_str)
{
	LPSUBIMGBLOCK	subimg_tmp;
	int				i, linesize;

	/* ����ڵ� */
	if ((subimg_tmp=_alloc_SUBIMGBLOCK()) == 0)
	{
		return 0;
	}
	
	subimg_tmp->number  = num;			/* ��ͼ������� 1 ��ʼ */
	subimg_tmp->parents = pinfo_str;	/* ��ͼ���ַ */

	if (num == 1)
	{
		subimg_tmp->prev    = 0;
		subimg_tmp->next    = 0;
		pinfo_str->psubimg  = subimg_tmp;
		pinfo_str->imgnumbers = 2;

		subimg_tmp->width	= PCD_2_W;
		subimg_tmp->height	= PCD_2_H;
	}
	else if (num == 2)
	{
		subimg_tmp->prev    = pinfo_str->psubimg;
		subimg_tmp->next    = 0;
		pinfo_str->psubimg->next  = subimg_tmp;
		pinfo_str->imgnumbers = 3;

		subimg_tmp->width	= PCD_3_W;
		subimg_tmp->height	= PCD_3_H;
	}
	else
	{
		assert(0);									/* ֻ�ܴ���ڶ����͵�����ͼ�� */
		_free_SUBIMGBLOCK(subimg_tmp);
		return 0;
	}
	
	subimg_tmp->order	= pinfo_str->order;			/* ��ͬ����ͼ���� */
	subimg_tmp->bitcount= PCD_DEF_BITCOUNT;
	
	SETMASK_24(subimg_tmp);
	
	subimg_tmp->left = subimg_tmp->top = 0;
	subimg_tmp->dowith =subimg_tmp->userinput = subimg_tmp->time = 0;
	subimg_tmp->colorkey = -1;						/* ��Ϊ�޹ؼ�ɫ */
	subimg_tmp->pal_count = 0;
	
	linesize = _calcu_scanline_size(subimg_tmp->width, subimg_tmp->bitcount);
	
	assert(subimg_tmp->p_bit_data == (unsigned char *)0);
	
	/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
	subimg_tmp->p_bit_data = (unsigned char *)malloc(linesize * subimg_tmp->height + 4);
	if (!subimg_tmp->p_bit_data)
	{						
		_free_SUBIMGBLOCK(subimg_tmp); return 0;
	}
	
	assert(subimg_tmp->pp_line_addr == (void**)0);
	/* �������׵�ַ���� */
	subimg_tmp->pp_line_addr = (void **)malloc(sizeof(void*) * subimg_tmp->height);
	if (!subimg_tmp->pp_line_addr)
	{
		_free_SUBIMGBLOCK(subimg_tmp); return 0;
	}
	
	/* ��ʼ�����׵�ַ���飨���� */
	if (subimg_tmp->order == 0)		/* ���� */
	{
		for (i=0;i<(int)(subimg_tmp->height);i++)
		{
			subimg_tmp->pp_line_addr[i] = (void *)(subimg_tmp->p_bit_data+(i*linesize));
		}
	}
	else							/* ���� */
	{
		for (i=0;i<(int)(subimg_tmp->height);i++)
		{
			subimg_tmp->pp_line_addr[i] = (void *)(subimg_tmp->p_bit_data+((subimg_tmp->height-i-1)*linesize));
		}
	}
	
	return subimg_tmp;
}

