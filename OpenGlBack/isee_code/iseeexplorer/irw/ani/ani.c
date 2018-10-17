/********************************************************************

	ani.c

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
	���ļ���;��	ISeeͼ���������ANIͼ���дģ��ʵ���ļ�

					��ȡ���ܣ�1��4��8��16��24��32λANIͼ��
							  
					���湦�ܣ���֧��
							   

	���ļ���д�ˣ�	
					YZ		yzfree##yeah.net

	���ļ��汾��	20330
	����޸��ڣ�	2002-3-30

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-3		������һ���汾���°棬֧��ISeeIOϵͳ��


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

#include "ani.h"


IRWP_INFO			ani_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	ani_get_info_critical;	/* ani_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	ani_load_img_critical;	/* ani_load_image�����Ĺؼ��� */
CRITICAL_SECTION	ani_save_img_critical;	/* ani_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* ��� */, int bit/* λ�� */);

int CALLAGREEMENT _is_ani(unsigned char *pani);
int CALLAGREEMENT _get_info(unsigned char *pani, LPINFOSTR pinfo_str);
int CALLAGREEMENT _get_image(unsigned char *pani, LPINFOSTR pinfo_str);
int CALLAGREEMENT _str_to_value(unsigned char *pstr);
unsigned char * CALLAGREEMENT _jump_and_jump_and_jump(unsigned char *ptag);
int CALLAGREEMENT _ani_compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand);
int CALLAGREEMENT _ani_conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark);

static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);

static int CALLAGREEMENT _get_mask(int bitcount, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
static int CALLAGREEMENT _set_rate(int index, unsigned long rate, LPINFOSTR pinfo_str);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&ani_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&ani_get_info_critical);
			InitializeCriticalSection(&ani_load_img_critical);
			InitializeCriticalSection(&ani_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&ani_get_info_critical);
			DeleteCriticalSection(&ani_load_img_critical);
			DeleteCriticalSection(&ani_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

ANI_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&ani_irwp_info;
}

#else

ANI_API LPIRWP_INFO CALLAGREEMENT ani_get_plugin_info()
{
	_init_irwp_info(&ani_irwp_info);

	return (LPIRWP_INFO)&ani_irwp_info;
}

ANI_API void CALLAGREEMENT ani_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

ANI_API void CALLAGREEMENT ani_detach_plugin()
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


	/* ���ܱ�ʶ */
	lpirwp_info->irwp_function = IRWP_READ_SUPP;

	/* ����ģ��֧�ֵı���λ�� */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
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
				(const char *)"he he and he he ...");
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

	/* ����������##���ֶ���������ANI��ʽ�б�����ο�BMP����б��������ã� */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* ���ó�ʼ����ϱ�־ */
	lpirwp_info->init_tag = 1;

	return;
}



/* ��ȡͼ����Ϣ */
ANI_API int CALLAGREEMENT ani_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned char	*pani  = 0, *p;

	unsigned long	ani_len;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&ani_get_info_critical);

			/* ��ָ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* ���ANI���������� */
			if ((ani_len=isio_length(pfile)) < (FORMTYPESEIZE*4))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ��λ���ļ�ͷ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �����ڴ��Դ��������ANIͼ�����ݣ�+8��Ϊ�˷�ֹ���뺯��������Խ�磩 */
			if ((pani=(unsigned char *)malloc(ani_len+8)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;	/* �ڴ治�� */
			}

			memset((void*)(pani+ani_len), 0, 8);

			/* ��ȡ������ANI���ݵ��ڴ��� */
			if (isio_read((void*)pani, ani_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �ж��Ƿ���ANIͼ�� */
			if (_is_ani(pani) != 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* ��λ�������������RIFF��־�ײ��� */
			p = (unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)+FORMTYPESEIZE);
			
			/* ��ȡ��һ�����ĸ�Ҫ��Ϣ */
			switch (_get_info(p, pinfo_str))
			{
			case	-1:		/* ����ļ����� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	0:		/* �ɹ� */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			pinfo_str->imgtype		= IMT_RESDYN;		/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_ANI;			/* ͼ���ļ���ʽ����׺���� */
			pinfo_str->compression	= ICS_RGB;			/* ͼ���ѹ����ʽ */

			
			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pani)
				free(pani);

			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&ani_get_info_critical);
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
ANI_API int CALLAGREEMENT ani_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned char	*pani  = 0, *p;
	unsigned long	ani_len;

	LPSUBIMGBLOCK	subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&ani_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* �����û��ж� */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ���ANI���������� */
			if ((ani_len=isio_length(pfile)) < (FORMTYPESEIZE*4))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* ��λ���ļ�ͷ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �����ڴ��Դ��������ANIͼ�����ݣ�+8��Ϊ�˷�ֹ���뺯��������Խ�磩 */
			if ((pani=(unsigned char *)malloc(ani_len+8)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;	/* �ڴ治�� */
			}
			
			memset((void*)(pani+ani_len), 0, 8);
			
			/* ��ȡ������ANI���ݵ��ڴ��� */
			if (isio_read((void*)pani, ani_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж��Ƿ���ANIͼ�� */
			if (_is_ani(pani) != 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* ��λ�������������RIFF��־�ײ��� */
			p = (unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)+FORMTYPESEIZE);
			
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ��ȡ��һ�����ĸ�Ҫ��Ϣ */
				switch (_get_info(p, pinfo_str))
				{
				case	-1:		/* ����ļ����� */
					b_status = ER_BADIMAGE;  __leave; break;
				case	0:		/* �ɹ� */
					break;
				default:
					b_status = ER_SYSERR;    __leave; break;
				}
				
				pinfo_str->imgtype		= IMT_RESDYN;		/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_ANI;			/* ͼ���ļ���ʽ����׺���� */
				pinfo_str->compression	= ICS_RGB;			/* ͼ���ѹ����ʽ */
				
				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}
			
			/* ��ȡͼ������ */
			switch (_get_image(p, pinfo_str))
			{
			case	0:		/* �ɹ� */
				break;
			case	-1:		/* �����ͼ�� */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* �ڴ治�� */
				b_status = ER_MEMORYERR; __leave; break;
			case	-4:		/* �û��ж� */
				b_status = ER_USERBREAK; __leave; break;
			case	-2:		/* ϵͳ�쳣 */
			default:
				assert(0);
				b_status = ER_SYSERR;    __leave; break;
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
				if (pinfo_str->play_order)
				{
					free(pinfo_str->play_order);
					pinfo_str->play_order = (unsigned long *)0;
				}

				if (pinfo_str->data_state == 2)
					pinfo_str->data_state =1;	/* �Զ����� */

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O��д�쳣 */
			}

			if (pani)
				free(pani);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&ani_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
ANI_API int CALLAGREEMENT ani_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&ani_save_img_critical);

			/* ################ ���������д����ͼ��Ĵ��� ################ */
			
			
			b_status = ER_NOTSUPPORT;	/* ��֧�ֱ��湦�� */			
			
			
			/* ############################################################ */
		}
		__finally
		{
			LeaveCriticalSection(&ani_save_img_critical);
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


/* �ж�ָ�����������Ƿ�����Ч��ANI���� */
int CALLAGREEMENT _is_ani(unsigned char *pani)
{
	assert(pani);

	if ( (memcmp((const char *)pani, (const char *)RIFF_STR, FORMTYPESEIZE)==0) && \
		 (memcmp((const char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), (const char *)ACON_STR, FORMTYPESEIZE)==0) )
	{
		return 0;	/* ��ANI������ */
	}
	else
	{
		return -1;	/* ��ANI���� */
	}
}


/* ��ȡ�׷����ĸ�Ҫ��Ϣ */
int CALLAGREEMENT _get_info(unsigned char *pani, LPINFOSTR pinfo_str)
{
	int		mark = 0;

	ANICHUNK			type;
	ANIHEADER			aniheader;
	LPANI_CURDIR		pdirect;
	LPBITMAPINFOHEADER	pbmi;


	assert((pani!=0)&&(pinfo_str!=0));

	while ((type=_str_to_value(pani)) != ANICHUNK_OUT)
	{
		switch (type)
		{
		case	ANICHUNK_RIFF:
			return -1;	/* �Ƿ����ݡ�pani��ָ��RIFF���ڲ������Բ�Ӧ�ٳ���RIFF���־ */
		case	ANICHUNK_LIST:
			pani += FORMTYPESEIZE + sizeof(unsigned long) + FORMTYPESEIZE;
			break;
		case	ANICHUNK_INAM:
		case	ANICHUNK_IART:
		case	ANICHUNK_RATE:
		case	ANICHUNK_SEQ:
		case	ANICHUNK_UNKONW:
			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ANIH:
			if ((mark & 0x2) != 0)
				return -1;			/* ֻ�ܴ���һ��ANIH�� */
			else
				mark |= 0x2;

			memcpy((void*)&aniheader, (const void *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), sizeof(ANIHEADER));

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ICON:
			if ((mark & 0x1) == 0)	/* ֻ��ȡ��һ�����ͼ����Ϣ */
			{
				/* ��������ǹ�����ݣ�Ҳ������ͼ�����ݣ������ǲ�ʹ����Щ���ݣ����� */
				/* ֱ�Ӷ�ȡ����BMPINFOHEADER�еĲ��������Բ������� */
				pdirect = (LPANI_CURDIR)(unsigned char *)(pani + FORMTYPESEIZE + sizeof(unsigned long));
				pbmi = (LPBITMAPINFOHEADER)(unsigned char *)(((unsigned char *)pdirect) + pdirect->entries[0].image_offset);
				
				if (pbmi->biSize != sizeof(BITMAPINFOHEADER))
					return -1;		/* �Ƿ����� */

				/* ��ȡ��һ��ͼ�����Ϣ�������ݰ���֡ */
				pinfo_str->width	= (unsigned long)pbmi->biWidth;
				pinfo_str->height	= (unsigned long)pbmi->biHeight/2;	/* XOR��AND ͼ�ܸ߶� */
				pinfo_str->order	= 1;	/* ���ǵ���ͼ */
				pinfo_str->bitcount = (unsigned long)pbmi->biBitCount;
				
				if ((pinfo_str->width == 0)||(pinfo_str->height == 0))
					return -1;

				if (_get_mask(pinfo_str->bitcount, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == -1)
				{
					return -1;
				}

				mark |= 0x1;	/* ������ȡ����ͼ����Ϣ */
			}
			pani = _jump_and_jump_and_jump(pani);
			break;
		default:
			assert(0);
			return -2;		/* ϵͳ�쳣 */
			break;
		}
	}

	if (mark == 3)
		return 0;	/* �ɹ� */
	else
		return -1;	/* ͼ���������� */
}


/* ��ȡ������� */
int CALLAGREEMENT _get_image(unsigned char *pani, LPINFOSTR pinfo_str)
{
	int				mark = 0, imgcount = 0, i, imgnum = 0;
	unsigned long	seqcount, ratecount;
	unsigned long	*rate = 0;		/* ���ʱ� */

	ANICHUNK			type;
	ANIHEADER			aniheader;
	LPANI_CURDIR		pdirect;
	LPBITMAPINFOHEADER	pbmi;
	LPSUBIMGBLOCK		lpsub = 0, lplast = 0;
	
	
	assert((pani!=0)&&(pinfo_str!=0));
	
	while ((type=_str_to_value(pani)) != ANICHUNK_OUT)
	{
		switch (type)
		{
		case	ANICHUNK_RIFF:
			if (rate)
				free(rate);
			return -1;	/* �Ƿ����ݡ�pani��ָ��RIFF���ڲ������Բ�Ӧ�ٳ���RIFF���־ */
		case	ANICHUNK_LIST:
			pani += FORMTYPESEIZE + sizeof(unsigned long) + FORMTYPESEIZE;
			break;
		case	ANICHUNK_RATE:
			ratecount = *(unsigned long *)(unsigned char *)(pani+FORMTYPESEIZE);
			/* ���䲥�����ʱ� */
			rate = (unsigned long *)malloc(ratecount);

			if (rate == 0)
			{
				return -3;
			}

			/* ���Ʊ����� */
			memcpy((void*)rate, (const void *)(unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), ratecount);

			/* ת��ΪԪ�ص�λ */
			ratecount /= sizeof(unsigned long);

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_SEQ:
			seqcount = *(unsigned long *)(unsigned char *)(pani+FORMTYPESEIZE);
			/* ���䲥��˳��� */
			pinfo_str->play_order = (unsigned long *)malloc(seqcount);

			if (pinfo_str->play_order == 0)
			{
				if (rate)
					free(rate);
				return -3;			/* �ڴ治�� */
			}

			/* ���Ʊ����� */
			memcpy((void*)pinfo_str->play_order, (const void *)(unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), seqcount);

			/* ת��ΪԪ�ص�λ */
			seqcount /= sizeof(unsigned long);
			/* ���ű���Ԫ�ظ��� */
			pinfo_str->play_number = seqcount;

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_INAM:		/* ��ʱ����ͼ��������Ϣ */
		case	ANICHUNK_IART:
		case	ANICHUNK_UNKONW:
			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ANIH:
			if ((mark & 0x2) != 0)
			{
				if (rate)
					free(rate);
				return -1;			/* ֻ�ܴ���һ��ANIH�� */
			}
			else
			{
				mark |= 0x2;
			}

			memcpy((void*)&aniheader, (const void *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), sizeof(ANIHEADER));

			if ((aniheader.fl & 0x1) == 0)
			{
				if (rate)
					free(rate);
				return -1;			/* ��������������������ݣ�RAW����ʽ�洢ͼ���ANI */
			}

			/* �趨�������� */
			pinfo_str->process_total   = aniheader.cFrames;
			pinfo_str->process_current = 0;

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ICON:
			if ((mark & 0x2) == 0)
			{
				if (rate)
					free(rate);
				
				return -1;			/* anih �������ICON���ǰ�棬������Ϊ�Ƿ����� */
			}

			pdirect = (LPANI_CURDIR)(unsigned char *)(pani + FORMTYPESEIZE + sizeof(unsigned long));
			pbmi = (LPBITMAPINFOHEADER)(unsigned char *)(((unsigned char *)pdirect) + pdirect->entries[0].image_offset);
			
			if (pbmi->biSize != sizeof(BITMAPINFOHEADER))
			{
				if (rate)
					free(rate);
				
				return -1;		/* �Ƿ����� */
			}

			imgcount = pdirect->count;

			if (imgcount < 1)	/* ����Ӧ��һ��ͼ�� */
			{
				if (rate)
					free(rate);
				
				return -1;
			}

			if ((mark & 0x1) == 0)	/* ��ȡ��֡���ͼ�� */
			{
				switch (_ani_conv_image_block(pbmi, (void *)pinfo_str, ANI_CUR_PRI_IMAGE))
				{
				case	-2:		/* ����ļ����� */
					if (rate)
						free(rate);
					return -1;
				case	-3:		/* �ڴ治�� */
					if (rate)
						free(rate);
					return -3;
				case	0:		/* �ɹ� */
					break;
				default:
					if (rate)
						free(rate);
					return -2;	/* ϵͳ�쳣 */
				}

				imgcount--;		/* ����������ͼ��������� */
				imgnum = 0;		/* ��ͼ��������� */
				lplast = 0;
				mark |= 0x1;
			}
				
			/* ֧���û��ж� */
			if (pinfo_str->break_mark)
			{
				if (rate)
					free(rate);
				return -4;	/* �û��ж� */
			}

			/* ������ͼ�� */
			while (imgcount)
			{
				if ((lpsub=_alloc_SUBIMGBLOCK()) == 0)
				{
					if (rate)
						free(rate);
					return -3;
				}
				
				lpsub->number = ++imgnum;	/* ��ͼ������� 1 ��ʼ */
				lpsub->parents = pinfo_str;
				
				/* ������ͼ������β�� */
				if (lplast == 0)
				{
					pinfo_str->psubimg = lpsub;	/* �׷���ͼ�� */
					lplast = lpsub;
					
					lpsub->prev = 0;
					lpsub->next = 0;
				}
				else
				{
					lpsub->prev = lplast;		/* ������ͼ�� */
					lpsub->next = 0;
					lplast->next = lpsub;
					lplast = lpsub;
				}

				/* ����ͼ�����ݵ�ַ */
				pbmi = (LPBITMAPINFOHEADER)(unsigned char *)(((unsigned char *)pdirect) + pdirect->entries[pdirect->count-imgcount].image_offset);
				
				/* ת����ͼ�����ݰ��� */
				switch (_ani_conv_image_block(pbmi, (void *)lpsub, ANI_CUR_SUB_IMAGE))
				{
				case	-2:		/* ����ļ����� */
					if (rate)
						free(rate);
					return -1;
				case	-3:		/* �ڴ治�� */
					if (rate)
						free(rate);
					return -3;
				case	0:		/* �ɹ� */
					break;
				default:
					if (rate)
						free(rate);
					return -2;
				}
				
				/* ֧���û��ж� */
				if (pinfo_str->break_mark)
				{
					if (rate)
						free(rate);
					return -4;
				}

				imgcount--;
			}

			/* �ۼӲ���ֵ�����������һ����SEQ��RATE��Ķ�ȡ��*/
			if ((pinfo_str->process_current+1) != pinfo_str->process_total)
				pinfo_str->process_current++;
			
			pani = _jump_and_jump_and_jump(pani);
			break;
		default:
			assert(0);
			if (rate)
				free(rate);
			return -2;		/* ϵͳ�쳣 */
			break;
		}
	}
	
	if (mark == 3)
	{
		pinfo_str->imgnumbers = imgnum+1;	/* ͼ���ܸ�����������֡ͼ��*/

		/* ����ÿ֡ͼ�����ʾ�ӳ�ʱ�� */
		if (rate)
		{
			if (pinfo_str->play_order)
			{
				for (i=0;i<(int)pinfo_str->play_number;i++)
				{
					if (_set_rate((int)pinfo_str->play_order[i], rate[i], pinfo_str) != 0)
					{
						free(rate);
						return -1;
					}
				}
			}
			else
			{
				for (i=0;i<(int)pinfo_str->imgnumbers;i++)
				{
					if (_set_rate((int)i, rate[i], pinfo_str) != 0)
					{
						free(rate);
						return -1;
					}
				}
			}
		}
		else
		{
			for (i=0;i<(int)pinfo_str->imgnumbers;i++)
			{
				if (_set_rate((int)i, aniheader.jifRate, pinfo_str) != 0)
				{
					return -1;
				}
			}
		}

		pinfo_str->colorkey = -1;

		if (rate)
			free(rate);

		return 0;	/* �ɹ� */
	}
	else
	{
		if (rate)
			free(rate);
		return -1;	/* ͼ���������� */
	}
}


/* ת�����ʾ��Ϊ��ʾ�� */
int CALLAGREEMENT _str_to_value(unsigned char *pstr)
{
	assert(pstr);

	if (memcmp((const char *)pstr, (const char *)ICON_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_ICON;
	else if (memcmp((const char *)pstr, (const char *)LIST_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_LIST;
	else if (memcmp((const char *)pstr, (const char *)ANIH_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_ANIH;
	else if (memcmp((const char *)pstr, (const char *)RATE_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_RATE;
	else if (memcmp((const char *)pstr, (const char *)SEQ_STR , FORMTYPESEIZE) == 0)
		return ANICHUNK_SEQ;
	else if (memcmp((const char *)pstr, (const char *)RIFF_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_RIFF;
	else if (memcmp((const char *)pstr, (const char *)INAM_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_INAM;
	else if (memcmp((const char *)pstr, (const char *)IART_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_IART;
	else if (memcmp((const char *)pstr, (const char *)NULL_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_OUT;	/* ������Ч���ݷ�Χ����������β���� */
	else
		return ANICHUNK_UNKONW;	/* δ֪��RIFF���־ */
}


/* ����һ���� */
unsigned char * CALLAGREEMENT _jump_and_jump_and_jump(unsigned char *ptag)
{
	int size;
	
	assert(ptag);
	
	size = (int)*(unsigned long *)(unsigned char *)(ptag+FORMTYPESEIZE);
	
	size += size%2;		/* ��ߴ���������������׷��һ���ֽڣ�RIFF��ʽ�涨�� */
	
	return (unsigned char *)(ptag + FORMTYPESEIZE + sizeof(unsigned long) + size);
}


/* ��ȡ����ɫ������ֵ */
int CALLAGREEMENT _get_mask(int bitcount, 
							unsigned long * pb_mask, 
							unsigned long * pg_mask, 
							unsigned long * pr_mask, 
							unsigned long * pa_mask)
{
	int result = 0;
	
	switch(bitcount)
	{
	case	1:
	case	4:
	case	8:
		/* ��ɫ��λͼ������ */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:		/* 555 ��ʽ */
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:		/* 888 ��ʽ */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	default:		/* �Ƿ���ʽ */
		result = -1;
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}
	
	return result;	/* 0-�ɹ��� -1���Ƿ���ͼ���ʽ */
}


/* ����ͼ����ʾ�ӳٲ�����ֻ��_get_image�������ã� */
int CALLAGREEMENT _set_rate(int index, unsigned long rate, LPINFOSTR pinfo_str)
{
	LPSUBIMGBLOCK	lpsub;
	
	if (index >= (int)pinfo_str->imgnumbers)
	{
		return -1;		/* �Ƿ������� */
	}
	
	if (index == 0)		/* ��֡ͼ�� */
	{
		pinfo_str->time = FPS_TO_MS(rate);
	}
	else				/* ��ͼ�� */
	{
		for (lpsub=pinfo_str->psubimg; lpsub != 0; lpsub=lpsub->next)
		{
			if (lpsub->number != index)
				continue;
			else
			{
				lpsub->time = FPS_TO_MS(rate);
				break;
			}
		}
		assert(lpsub);	/* �����ܳ����Ҳ�����Ӧ��ͼ������ */
	}
	
	return 0;
}


/* ����һ����ͼ��ڵ㣬����ʼ���ڵ������� */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void)
{
	LPSUBIMGBLOCK	p_subimg = 0;
	
	/* ������ͼ����Ϣ�ڵ�(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* �ڴ治�� */
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	p_subimg->number = 1;
	p_subimg->colorkey = -1;
	
	return p_subimg;
}


/* �ͷ�һ����ͼ��ڵ㣬�������е�λ�����������׵�ַ���� */
void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);
	assert(p_node->number > 0);
	
	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);
	
	if (p_node->p_bit_data)
		free(p_node->p_bit_data);
	
	free(p_node);
}


/* ��������ݿ�תΪ���ݰ��� */
int CALLAGREEMENT _ani_conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark)
{
	LPINFOSTR		pinfo = (LPINFOSTR)0;
	LPSUBIMGBLOCK	psubinfo = (LPSUBIMGBLOCK)0;
	LPBITMAPINFO	pbi = (LPBITMAPINFO)lpbmi;
	
	unsigned char	*p, *pxor, *pand;
	unsigned long	pal_num = 0;
	int				i, linesize, linesize2;

	assert((lpbmi)&&(lpdest));
	assert((mark == ANI_CUR_PRI_IMAGE)||(mark == ANI_CUR_SUB_IMAGE));

	if (mark == ANI_CUR_PRI_IMAGE)
	{
		pinfo = (LPINFOSTR)lpdest;

		/* ���õ�ɫ������ */
		if (lpbmi->biBitCount <= 8)
		{
			pinfo->pal_count = 1UL << pinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? pinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > pinfo->pal_count)
			{
				return -2;		/* ͼ������ */
			}
			memmove((void*)(pinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			pinfo->pal_count = 0;
		}

		/* XOR����ɨ���гߴ� */
		linesize  = _calcu_scanline_size(pinfo->width, pinfo->bitcount);
		/* AND����ɨ���гߴ� */
		linesize2 = _calcu_scanline_size(pinfo->width, 1);
		
		assert(pinfo->p_bit_data == (unsigned char *)0);
		
		/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
		pinfo->p_bit_data = (unsigned char *)malloc(linesize * pinfo->height + 4);
		if (!pinfo->p_bit_data)
		{						
			return -3;			/* �ڴ治�� */
		}
		
		assert(pinfo->pp_line_addr == (void**)0);
		/* �������׵�ַ���� */
		pinfo->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo->height);
		if (!pinfo->pp_line_addr)
		{
			free(pinfo->p_bit_data); 
			pinfo->p_bit_data = 0; 
			return -3;			/* �ڴ治�� */
		}

		/* ��ʼ�����׵�ַ���飨���� */
		for (i=0;i<(int)(pinfo->height);i++)
		{
			pinfo->pp_line_addr[i] = (void *)(pinfo->p_bit_data+((pinfo->height-i-1)*linesize));
		}
		
		/* ����XOR���������׵�ַ */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);

		/* ����XOR�������� */
		memmove((void*)pinfo->p_bit_data, (const void *)p, (linesize*pinfo->height));

		/* ȡ�� XOR �����׵�ַ */
		pxor = (unsigned char *)pinfo->p_bit_data;
		/* ���� AND �����׵�ַ */
		pand = p + linesize * pinfo->height;

		/* �ϳ�ͼ�� */
		for (i=0;i<(int)pinfo->height;i++)
		{
			if (_ani_compose_img(pinfo->bitcount, pinfo->width, pxor, pand) != 0)
			{
				free(pinfo->p_bit_data); 
				pinfo->p_bit_data = 0; 
				free(pinfo->pp_line_addr);
				pinfo->pp_line_addr = 0;

				return -2;		/* �����ͼ�� */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}
	else
	{
		psubinfo = (LPSUBIMGBLOCK)lpdest;

		/* ��дͼ����Ϣ */
		psubinfo->width		= (int)lpbmi->biWidth;
		psubinfo->height	= (int)lpbmi->biHeight/2;	/* XOR��AND ͼ�ܸ߶� */
		psubinfo->order		= 1;	/* ���ǵ���ͼ */
		psubinfo->bitcount	= (int)lpbmi->biBitCount;

		if (_get_mask(psubinfo->bitcount, 
			(unsigned long *)(&psubinfo->b_mask), 
			(unsigned long *)(&psubinfo->g_mask), 
			(unsigned long *)(&psubinfo->r_mask),
			(unsigned long *)(&psubinfo->a_mask)) == -1)
		{
			return -2;		/* ͼ������ */
		}

		psubinfo->left = psubinfo->top = 0;
		psubinfo->dowith =psubinfo->userinput = psubinfo->time = 0;
		psubinfo->colorkey = -1;	/* ��Ϊ�޹ؼ�ɫ */

		/* ���õ�ɫ������ */
		if (lpbmi->biBitCount <= 8)
		{
			psubinfo->pal_count = 1UL << psubinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? psubinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > (unsigned long)psubinfo->pal_count)
			{
				return -2;		/* ͼ������ */
			}
			memmove((void*)(psubinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			psubinfo->pal_count = 0;
		}

		
		/* ȡ��XOR����ɨ���гߴ� */
		linesize  = _calcu_scanline_size(psubinfo->width, psubinfo->bitcount);
		/* ����AND����ɨ���гߴ� */
		linesize2 = _calcu_scanline_size(psubinfo->width, 1);
		
		assert(psubinfo->p_bit_data == (unsigned char *)0);
		
		/* ����Ŀ��ͼ���ڴ�飨+4 �� β������4�ֽڻ������� */
		psubinfo->p_bit_data = (unsigned char *)malloc(linesize * psubinfo->height + 4);
		if (!psubinfo->p_bit_data)
		{						
			return -3;			/* �ڴ治�� */
		}
		
		assert(psubinfo->pp_line_addr == (void**)0);
		/* �������׵�ַ���� */
		psubinfo->pp_line_addr = (void **)malloc(sizeof(void*) * psubinfo->height);
		if (!psubinfo->pp_line_addr)
		{
			free(psubinfo->p_bit_data); 
			psubinfo->p_bit_data = 0; 
			return -3;			/* �ڴ治�� */
		}
		
		/* ��ʼ�����׵�ַ���飨���� */
		for (i=0;i<(int)(psubinfo->height);i++)
		{
			psubinfo->pp_line_addr[i] = (void *)(psubinfo->p_bit_data+((psubinfo->height-i-1)*linesize));
		}
		
		/* �������������׵�ַ */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);
		
		/* ������������ */
		memmove((void*)psubinfo->p_bit_data, (const void *)p, (linesize*psubinfo->height));
		
		/* ���� XOR �����׵�ַ */
		pxor = (unsigned char *)psubinfo->p_bit_data;
		/* ���� AND �����׵�ַ */
		pand = p + linesize * psubinfo->height;
		
		/* �ϳ�ͼ��ʹ��XOR������AND���ݣ�*/
		for (i=0;i<psubinfo->height;i++)
		{
			if (_ani_compose_img(psubinfo->bitcount, psubinfo->width, pxor, pand) != 0)
			{
				free(psubinfo->p_bit_data); 
				psubinfo->p_bit_data = 0;
				free(psubinfo->pp_line_addr);
				psubinfo->pp_line_addr = 0;

				return -2;		/* �����ͼ�� */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}

	return 0;		/* ���� 0 �ɹ����� 0 ʧ�� */
}


/* �ϳ�һ��ɨ���е�XORͼ��ANDͼ��������ISeeλ����ʽת�������������XORͼ�� */
int CALLAGREEMENT _ani_compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand)
{
	unsigned char buf[32], tmp, *p = pxor, isc = 0;
	int i, j, k, l, a, bytesize;

	assert(bitcount > 0);
	assert(pxor&&pand);

	/* һ�δ��� 8 ������ */
	for (i=0, k=0;i<width;i+=8, k++)
	{
		/* ȡ�� 8 �����ص�AND���ݣ��� 1 ���ֽڣ� */
		tmp = *(pand+k);

		/* ��AND����תΪXOR���ظ�ʽ */
		switch (bitcount)
		{
		case	1:
			buf[0] = tmp;
			bytesize = 1;	/* ʵ��Ҫ�������ֽ��� */
			break;
		case	4:
			for (l=0;l<8;l+=2)
			{
				buf[l/2]  = ((tmp>>(7-l))&0x1) ? 0xf0 : 0x0;
				buf[l/2] |= ((tmp>>(7-l-1))&0x1) ? 0xf : 0x0;
			}
			bytesize = 4;
			break;
		case	8:
			for (l=0;l<8;l++)
			{
				buf[l] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
			}
			bytesize = ((width-i) < 8) ? (width-i) : 8;
			break;
		case	16:
			for (l=0;l<8;l++)
			{
				buf[l*2] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*2+1] = buf[l*2]&0x7f;		/* 555��ʽ */
			}
			bytesize = ((width-i) < 8) ? (width-i)*2 : 16;
			break;
		case	24:
			for (l=0;l<8;l++)
			{
				buf[l*3] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*3+1] = buf[l*3];
				buf[l*3+2] = buf[l*3];
			}
			bytesize = ((width-i) < 8) ? (width-i)*3 : 24;
			break;
		case	32:
			for (l=0;l<8;l++)
			{
				buf[l*4] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*4+1] = buf[l*4];
				buf[l*4+2] = buf[l*4];
				buf[l*4+3] = 0;					/* 888��ʽ */
			}
			bytesize = ((width-i) < 8) ? (width-i)*4 : 32;
			break;
		default:
			return -1;							/* ��֧�ֵ�λ��� */
			break;
		}

		/* �ϳ�ͼ��һ�κϳ� 8 �����أ�*/
		for (j=0;j<bytesize;j++)
		{
			*p++ ^= buf[j];
		}

		isc = 0;

		/* �� 1 λ��ͼ��תΪISeeλ����ʽ */
		if (bitcount == 1)
		{
			for (a=0;a<8;a++)
			{
				isc |= (((*(p-1))>>(7-a))&0x1)<<a;
			}
			*(p-1) = isc;
		}

		/* �� 4 λ��ͼ��תΪISeeλ����ʽ */
		if (bitcount == 4)
		{
			for (a=0;a<4;a++)
			{
				isc  = (*(p-4+a))>>4;
				isc |= (*(p-4+a))<<4;
				*(p-4+a) = isc;
			}
		}
	}

	return 0;	/* ���� 0 ��ʾ�ɹ������ط� 0 ֵ��ʾʧ�� */
}


