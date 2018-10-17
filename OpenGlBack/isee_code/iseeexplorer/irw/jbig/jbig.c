/********************************************************************

	jbig.c

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
	���ļ���;��	ISeeͼ���������JBIGͼ���дģ��ʵ���ļ�

					��ȡ���ܣ��ɶ�ȡ��ɫJBGͼ�󣨾��󲿷ֵ�ɨ�����ͼ��
							  
					���湦�ܣ���֧��
							   
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net

	���ļ��汾��	20530
	����޸��ڣ�	2002-5-30

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------

    ���ô����д�˼���Ȩ��Ϣ��
	----------------------------------------------------------------
	�������	JBIG�������(jbig kit)
	����ļ���  libjbig.h��libjbig.c��libjbigtab.c��libjbig.txt
	ʹ����ɣ�  GPL
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

#include "jbig.h"
#include "libjbig.h"


IRWP_INFO			jbig_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	jbig_get_info_critical;	/* jbig_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	jbig_load_img_critical;	/* jbig_load_image�����Ĺؼ��� */
CRITICAL_SECTION	jbig_save_img_critical;	/* jbig_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


#define	BUFFER_SIZE		(32*1024)			/* �����ݻ������ߴ磨32KB��*/


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&jbig_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&jbig_get_info_critical);
			InitializeCriticalSection(&jbig_load_img_critical);
			InitializeCriticalSection(&jbig_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&jbig_get_info_critical);
			DeleteCriticalSection(&jbig_load_img_critical);
			DeleteCriticalSection(&jbig_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

JBIG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&jbig_irwp_info;
}

#else

JBIG_API LPIRWP_INFO CALLAGREEMENT jbig_get_plugin_info()
{
	_init_irwp_info(&jbig_irwp_info);

	return (LPIRWP_INFO)&jbig_irwp_info;
}

JBIG_API void CALLAGREEMENT jbig_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

JBIG_API void CALLAGREEMENT jbig_detach_plugin()
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
	lpirwp_info->irwp_function = IRWP_READ_SUPP;

	/* ����ģ��֧�ֵı���λ�� */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num  = 0;
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
				(const char *)"��Markus Kuhn�¾���");
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

	/* ����������##���ֶ���������JBIG��ʽ�б�����ο�BMP����б��������ã� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"JBIG");
	
	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
JBIG_API int CALLAGREEMENT jbig_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	JBG_DEC_STATE	jds;
	unsigned char	*buf = 0, *p;
	int				len, cnt, result;
	unsigned long	xmax = 4294967295UL;
	unsigned long	ymax = 4294967295UL;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&jbig_get_info_critical);

			jbg_dec_init(&jds);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��������ݻ����� */
			if ((buf=(unsigned char *)malloc(BUFFER_SIZE)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* �趨ͼ������޳� */
			jbg_dec_maxsize(&jds, xmax, ymax);

			result = JBG_EAGAIN;
			
			do
			{
				/* �ֶζ�ȡѹ������ */
				if ((len=isio_read((void*)buf, 1, BUFFER_SIZE, pfile)) == 0)
				{
					if ((result == JBG_EOK_INTR)||(result == JBG_EOK))
					{
						break;
					}
					else
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				
				cnt = 0;
				
				p = buf;
				
				while (len > 0 && (result == JBG_EAGAIN || result == JBG_EOK))
				{
					/* �����ݵ�������� */
					result = jbg_dec_in(&jds, p, len, &cnt);
					
					p   += cnt;
					len -= cnt;
				}
			} while (result == JBG_EAGAIN || result == JBG_EOK);
			

			/* �ж�ָ��ͼ���Ƿ���JBIGͼ�� */
			switch (result)
			{
			case	JBG_EOK:
			case	JBG_EOK_INTR:
				break;
			case	JBG_ENOMEM:
				b_status = ER_MEMORYERR; __leave; break;
			case	JBG_EAGAIN:
			case	JBG_EABORT:
			case	JBG_EMARKER:
			case	JBG_ENOCONT:
			case	JBG_EINVAL:
			case	JBG_EIMPL:
				b_status = ER_NONIMAGE;  __leave; break;
			case	JBG_ESYSERR:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* ֻ֧�ֵ�ɫͼ�� */
			if (jbg_dec_getplanes(&jds) != 1)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_JBIG;			/* ͼ���ļ���ʽ����׺���� */
			
			pinfo_str->compression  = ICS_JBIG;			/* ѹ����ʽ */

			pinfo_str->width	= (unsigned long)jbg_dec_getwidth(&jds);
			pinfo_str->height	= (unsigned long)jbg_dec_getheight(&jds);
			pinfo_str->order	= 0;					/* ��Ϊ����ͼ */
			pinfo_str->bitcount	= (unsigned long)jbg_dec_getplanes(&jds);
			
			SETMASK_8(pinfo_str);						/* ��������Ϊȫ0 */
			

			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (buf)
				free(buf);

			jbg_dec_free(&jds);

			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&jbig_get_info_critical);
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
JBIG_API int CALLAGREEMENT jbig_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	JBG_DEC_STATE	jds;
	unsigned char	*buf = 0, *p, *bits;
	int				len, srclinesize, cnt, result, i, j, linesize, step;
	unsigned long	xmax = 4294967295UL;
	unsigned long	ymax = 4294967295UL;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&jbig_load_img_critical);

			jbg_dec_init(&jds);
			
			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total   = pinfo_str->height;
			pinfo_str->process_current = 0;

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��������ݻ����� */
			if ((buf=(unsigned char *)malloc(BUFFER_SIZE)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* �趨ͼ������޳� */
			jbg_dec_maxsize(&jds, xmax, ymax);
			
			result = JBG_EAGAIN;
			
			do
			{
				/* �ֶζ�ȡѹ������ */
				if ((len=isio_read((void*)buf, 1, BUFFER_SIZE, pfile)) == 0)
				{
					if ((result == JBG_EOK_INTR)||(result == JBG_EOK))
					{
						break;
					}
					else
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				
				cnt = 0;
				
				p = buf;
				
				while (len > 0 && (result == JBG_EAGAIN || result == JBG_EOK))
				{
					/* �����ݵ�������� */
					result = jbg_dec_in(&jds, p, len, &cnt);
					
					p   += cnt;
					len -= cnt;
				}
			} while (result == JBG_EAGAIN || result == JBG_EOK);
			
			
			/* �ж�ָ��ͼ���Ƿ���JBIGͼ�� */
			switch (result)
			{
			case	JBG_EOK:
			case	JBG_EOK_INTR:
				break;
			case	JBG_ENOMEM:
				b_status = ER_MEMORYERR; __leave; break;
			case	JBG_EAGAIN:
			case	JBG_EABORT:
			case	JBG_EMARKER:
			case	JBG_ENOCONT:
			case	JBG_EINVAL:
			case	JBG_EIMPL:
				b_status = ER_NONIMAGE;  __leave; break;
			case	JBG_ESYSERR:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				if (jbg_dec_getplanes(&jds) != 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_JBIG;			/* ͼ���ļ���ʽ����׺���� */
				
				pinfo_str->compression  = ICS_JBIG;			/* ѹ����ʽ */
				
				pinfo_str->width	= (unsigned long)jbg_dec_getwidth(&jds);
				pinfo_str->height	= (unsigned long)jbg_dec_getheight(&jds);
				pinfo_str->order	= 0;					/* ��Ϊ����ͼ */
				pinfo_str->bitcount	= (unsigned long)jbg_dec_getplanes(&jds);
				
				SETMASK_8(pinfo_str);						/* ��������Ϊȫ0 */
				
				pinfo_str->data_state = 1;
			}			
			
			/* ���õ�ɫ������ */
			assert(pinfo_str->bitcount == 1);

			pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->palette[0] = 0xffffff;
				pinfo_str->palette[1] = 0x0;
				break;
			case	4:
			case	8:
				assert(0);		/* ��Ӧ��ִ�е��˴�������Ĵ���ֻ��Ϊ������չ֮�� */

				step = 256/(int)pinfo_str->pal_count;

				for (i=0; i<(int)pinfo_str->pal_count; i++)
				{
					pinfo_str->palette[i] = step*(pinfo_str->pal_count-i)-1;
				}
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}


			/* ȡ��ɨ���гߴ� */
			linesize    = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			/* JBIG��ѹ���ɨ���г��ȣ��ֽڱ߽磩 */
			srclinesize	= (int)((pinfo_str->bitcount*pinfo_str->width+7)/8);
			
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
			
			/* ��д���׵�ַ���飨����ͼ��*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}

			/* ��ȡ��ѹ���ͼ�����ݵ�ַ */
			bits = jbg_dec_getimage(&jds, 0);
			
			/* ���ж��� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				p = (unsigned char *)pinfo_str->pp_line_addr[i];

				memcpy((void*)p, (const void *)bits, srclinesize);

				/* תΪISeeλ����ʽ */
				for (j=0; j<srclinesize; j++)
				{
					CVT_BITS1(p[j]);
				}

				bits += srclinesize;

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

			if (buf)
				free(buf);

			jbg_dec_free(&jds);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&jbig_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
JBIG_API int CALLAGREEMENT jbig_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* �������ͼ��λ���� */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&jbig_save_img_critical);
	
			/* ################ ���������д����ͼ��Ĵ��� ################ */
			
			b_status = ER_NOTSUPPORT;
			
			/* �ݲ�֧��JBIG�ı��湦�ܡ�����ͼƬһ�㶼����ɨ���ǡ����������
			   �ģ�����ֻ֧�ֶ����ܼ��ɡ���YZ
			*/

			/* ############################################################ */
			
			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&jbig_save_img_critical);
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

