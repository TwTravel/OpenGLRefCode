/********************************************************************

	psd.c

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
			http://iseeexplorer.cosoft.org.cn

	���ŵ���

			yzfree##sina.com

	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������PSDͼ���дģ��ʵ���ļ�
	
					��ȡ���ܣ�
								BITMAPɫ,1λ,RLEѹ������ѹ��; 
								RGBɫ,8λ,RLEѹ������ѹ��; 
								�Ҷ�,8λ,RLEѹ������ѹ��; 
								Duotone, 8λ,RLEѹ������ѹ��;
								CMYKɫ,8λ,RLEѹ������ѹ��; 
								Labɫ,8λ,RLEѹ������ѹ��;
								* ��֧��16λ/ͨ������֧�ֶ�ͨ��ͼ��

					���湦�ܣ���֧��
	  
	���ļ���д�ˣ�	Janhail		janhail##sina.com
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20423
	����޸��ڣ�	2002-4-23
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-4		�����°���롣֧��ISeeIO���ı��ȡ���������ڴ�ռ��
					�������ӶԵ�ɫͼRLEѹ����֧�֣��Ľ�CMYK->RGB�㷨��
					ȥ����16λ/ͨ��ͼ���֧�֣�ȥ���Զ�ͨ��ͼ���֧�֡�
		
		2001-1		������һЩBUG
		2000-10		��һ�����԰淢��

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

#include "psd.h"


IRWP_INFO			psd_irwp_info;			/* �����Ϣ�� */

#ifdef WIN32
CRITICAL_SECTION	psd_get_info_critical;	/* psd_get_image_info�����Ĺؼ��� */
CRITICAL_SECTION	psd_load_img_critical;	/* psd_load_image�����Ĺؼ��� */
CRITICAL_SECTION	psd_save_img_critical;	/* psd_save_image�����Ĺؼ��� */
#else
/* Linux��Ӧ����� */
#endif


/* �ڲ����ֺ��� */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);

static int  CALLAGREEMENT _is_support_format( unsigned short, unsigned short );
static int	CALLAGREEMENT _skip_block( ISFILE *pfile );
static int  CALLAGREEMENT _get_rle_header(ISFILE *pfile, LPPSDFILEHEADER psdheader, unsigned short **prle_header);
static int  CALLAGREEMENT _read_line(ISFILE *pfile, long pos, unsigned short size, signed char *pdata);
static int  CALLAGREEMENT _decode_rle_line(signed char *psou, long ssize, unsigned char *pdes, long dsize);

static enum EXERESULT CALLAGREEMENT read_psd(ISFILE *pfile, LPINFOSTR pInfo, LPPSDFILEHEADER psdheader);
static enum EXERESULT CALLAGREEMENT _read_bitmap(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_grayscale_index_duotone(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_rgb(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_cmyk(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_lab(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);


										  

#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, unsigned long ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* ��ʼ�������Ϣ�� */
			_init_irwp_info(&psd_irwp_info);

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&psd_get_info_critical);
			InitializeCriticalSection(&psd_load_img_critical);
			InitializeCriticalSection(&psd_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&psd_get_info_critical);
			DeleteCriticalSection(&psd_load_img_critical);
			DeleteCriticalSection(&psd_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PSD_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&psd_irwp_info;
}

#else

PSD_API LPIRWP_INFO CALLAGREEMENT psd_get_plugin_info()
{
	_init_irwp_info(&psd_irwp_info);

	return (LPIRWP_INFO)&psd_irwp_info;
}

PSD_API void CALLAGREEMENT psd_init_plugin()
{
	/* ��ʼ�����߳�ͬ������ */
}

PSD_API void CALLAGREEMENT psd_detach_plugin()
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

	/* ����ģ�鱣�����֧�ֵ�λ�� */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
	/* ���������趨���������޸Ĵ�ֵ��##���ֶ������� */
	lpirwp_info->irwp_save.count = 0;

	/* ����������������������Ϣ����Ч��ĸ�������##���ֶ�������*/
	lpirwp_info->irwp_author_count = 2;


	/* ��������Ϣ��##���ֶ������� */
	/* ---------------------------------[0] �� ��һ�� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"Janhail");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"janhail##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"�벻Ҫд��������!! ^_^");
	/* ---------------------------------[1] �� �ڶ��� -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"�İ�����۵ù�Ǻ:)");
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
PSD_API int CALLAGREEMENT psd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	PSDFILEHEADER	psd_header;
	unsigned short	compression;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ������ݰ���������ͼ��λ���ݣ������ٸı���е�ͼ����Ϣ */	

	__try
	{
		__try
		{
			/* ����ؼ��� */
			EnterCriticalSection(&psd_get_info_critical);

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

			if (isio_read((void*)&psd_header, sizeof(PSDFILEHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* �ж��ļ�ͷ����PSD�ļ���־('8BPS')��Version��־(1)
			   PSD �� Version һ���� 1������������ľʹ��� */
			if ( (psd_header.Signature != PSD_SIGNATURE_MARKER) ||\
				(psd_header.Version != PSD_VERSION_MARKER) )
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			psd_header.Channels	= EXCHANGE_WORD( psd_header.Channels ) ;
			psd_header.Rows		= EXCHANGE_DWORD( psd_header.Rows ) ;
			psd_header.Columns	= EXCHANGE_DWORD( psd_header.Columns ) ;
			psd_header.Depth	= EXCHANGE_WORD( psd_header.Depth ) ;
			psd_header.Mode		= EXCHANGE_WORD( psd_header.Mode ) ;
			
			/* ͨ���������� 1 �� 24 ֮�䣬����ķ�Χ�� 1 �� 30000 ֮�� */
			if( psd_header.Channels<1 || psd_header.Channels>24 \
				|| psd_header.Rows<1 || psd_header.Rows>30000 \
				|| psd_header.Columns<1 || psd_header.Columns>30000 )
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* �ж��ļ���ʽ��λ��ȣ��Ƿ�֧�� */
			if( _is_support_format( psd_header.Mode, psd_header.Depth ) )
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
			pinfo_str->imgformat	= IMF_PSD;			/* ͼ���ļ���ʽ����׺���� */

			/* ��дͼ����Ϣ */
			pinfo_str->width	= psd_header.Columns ;
			pinfo_str->height	= psd_header.Rows ;
			pinfo_str->order	= 0 ;

			/* ����ɫϵ��ͨ�����Ƿ�ISee������� */
			switch( psd_header.Mode )
			{
			case 0:	/* Bitmap */			/* ������ɫϵֻ������һ��ͨ�� */
			case 1:	/* Grayscale */
			case 2:	/* Indexed */
			case 8:	/* Duotone (���Ҷȶ�ȡ) */
				if (psd_header.Channels != 1)
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth;
				break;
			case 3:	/* RGB */				/* RGBɫϵͨ��Ϊ3ͨ����������һ��ALPHAͨ�� */
				if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
				break;
			case 4:	/* CMYK */				/* CMYKɫϵ�������ٸ���ALPHAͨ�� */
				if (psd_header.Channels != 4)
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth * 3;
				break;
			case 7:	/* Multichannel */		/* �ݲ�֧�ֶ�ͨ��ͼ�� */
				b_status = ER_BADIMAGE; __leave; break;
			case 9:	/* Lab */
				if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
				break;
			default:/* Error */
				assert(0); b_status = ER_SYSERR; __leave; break;
			}
			

			/* ��ȡͼ���ѹ����ʽ */

			/* ������ɫ���(ֻ������ͼ��˫ɫ��ͼ��duotone�����е�ɫ����Ϣ) */
			if (_skip_block( pfile ))
			{
				b_status = ER_FILERWERR; __leave;
			}
			/* ����ͼ����Դ�� */
			if (_skip_block( pfile ))
			{
				b_status = ER_FILERWERR; __leave;
			}
			/* ����ͼ�����ɰ�� */
			if (_skip_block( pfile ))
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* ��ȡѹ��������Ϣ */
			if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			compression = EXCHANGE_WORD( compression ) ;

			switch (compression)
			{
			case	1:
				pinfo_str->compression = ICS_RLE8 ;
				break;
			case	0:
				/* ����Ҳ�з� 888 ��RGB ��ֻ��8λ�ĻҶȵ� */
				if( psd_header.Depth == 1 )
					pinfo_str->compression = ICS_RGB;			/* ����һЩ��������׼�� */
				else
					pinfo_str->compression = ICS_BITFIELDS ;	/* ��λ�� */
				break;
			default:
				b_status = ER_BADIMAGE; __leave; break;			/* ����ݲ�֧��ZIPѹ�� */
			}


			/* ����ͼ���������� */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	8:
				pinfo_str->r_mask = 0;
				pinfo_str->g_mask = 0;
				pinfo_str->b_mask = 0;
				pinfo_str->a_mask = 0;
				break;
			case	24:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0;
				break;
			case	32:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0xff000000;
				break;
			}

			/* �趨���ݰ�״̬ */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&psd_get_info_critical);
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
PSD_API int CALLAGREEMENT psd_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PSDFILEHEADER	psd_header;

	unsigned short	compression;
	int				linesize, i;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* ���ݰ��в��ܴ���ͼ��λ���� */	

	__try
	{
		__try
		{
			EnterCriticalSection(&psd_load_img_critical);

			/* ���� */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ���ļ�ͷ�ṹ */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&psd_header, sizeof(PSDFILEHEADER), 1, pfile) == 0)
			{
				b_status = (pinfo_str->data_state == 1) ? ER_FILERWERR:ER_NONIMAGE; 
				__leave;
			}
			
			/* �ж��ļ�ͷ����PSD�ļ���־('8BPS')��Version��־(1)
			   PSD �� Version һ���� 1������������ľʹ��� */
			if ( (psd_header.Signature != PSD_SIGNATURE_MARKER) ||\
				(psd_header.Version != PSD_VERSION_MARKER) )
			{
				b_status = ER_NONIMAGE; __leave;
			}

			psd_header.Channels	= EXCHANGE_WORD( psd_header.Channels ) ;
			psd_header.Rows		= EXCHANGE_DWORD( psd_header.Rows ) ;
			psd_header.Columns	= EXCHANGE_DWORD( psd_header.Columns ) ;
			psd_header.Depth	= EXCHANGE_WORD( psd_header.Depth ) ;
			psd_header.Mode		= EXCHANGE_WORD( psd_header.Mode ) ;
			
			/* �����ͼ��δ���ù���ȡ��Ϣ����������дͼ����Ϣ */
			if (pinfo_str->data_state == 0)
			{
				/* ͨ���������� 1 �� 24 ֮��
				   ����ķ�Χ�� 1 �� 30000 ֮�� */
				if( psd_header.Channels<1 || psd_header.Channels>24 \
					|| psd_header.Rows<1 || psd_header.Rows>30000 \
					|| psd_header.Columns<1 || psd_header.Columns>30000 )
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				if( _is_support_format( psd_header.Mode, psd_header.Depth ) )
				{	/* �ļ��ĸ�ʽ��֧��(Ҳ������PSD����֧�ֵ��ļ�) */
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* ͼ���ļ����� */
				pinfo_str->imgformat	= IMF_PSD;			/* ͼ���ļ���ʽ����׺���� */

				/* ��дͼ����Ϣ */
				pinfo_str->width	= psd_header.Columns ;
				pinfo_str->height	= psd_header.Rows ;
				pinfo_str->order	= 0 ;

				/* ����ɫϵ��ͨ�����Ƿ�ISee������� */
				switch( psd_header.Mode )
				{
				case 0:	/* Bitmap */			/* ������ɫϵֻ������һ��ͨ�� */
				case 1:	/* Grayscale */
				case 2:	/* Indexed */
				case 8:	/* Duotone (���Ҷȶ�ȡ) */
					if (psd_header.Channels != 1)
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth;
					break;
				case 3:	/* RGB */				/* RGBɫϵͨ��Ϊ3ͨ����������һ��ALPHAͨ�� */
					if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
					break;
				case 4:	/* CMYK */				/* CMYKɫϵ�������ٸ���ALPHAͨ�� */
					if (psd_header.Channels != 4)
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth * 3;
					break;
				case 7:	/* Multichannel */		/* �ݲ�֧�ֶ�ͨ��ͼ�� */
					b_status = ER_BADIMAGE; __leave; break;
				case 9:	/* Lab */
					if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
					break;
				default:/* Error */
					assert(0); b_status = ER_SYSERR; __leave; break;
				}
				

				/* ��ȡͼ���ѹ����ʽ */

				/* ������ɫ��Ρ�ͼ����Դ�Ρ�ͼ�����ɰ�� */
				if (_skip_block( pfile )||_skip_block( pfile )||_skip_block( pfile ))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ȡѹ��������Ϣ */
				if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				compression = EXCHANGE_WORD( compression ) ;

				switch (compression)
				{
				case	1:
					pinfo_str->compression = ICS_RLE8 ;
					break;
				case	0:
					/* ����Ҳ�з� 888 ��RGB ��ֻ��8λ�ĻҶȵ� */
					if( psd_header.Depth == 1 )
						pinfo_str->compression = ICS_RGB;			/* ����һЩ��������׼�� */
					else
						pinfo_str->compression = ICS_BITFIELDS ;	/* ��λ�� */
					break;
				default:
					b_status = ER_BADIMAGE; __leave; break;			/* ����ݲ�֧��ZIPѹ�� */
				}


				/* ����ͼ���������� */
				switch (pinfo_str->bitcount)
				{
				case	1:
				case	8:
					pinfo_str->r_mask = 0;
					pinfo_str->g_mask = 0;
					pinfo_str->b_mask = 0;
					pinfo_str->a_mask = 0;
					break;
				case	24:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0;
					break;
				case	32:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0xff000000;
					break;
				}

				/* �趨���ݰ�״̬ */
				pinfo_str->data_state = 1;
			}


			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
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

			/* PSDͼ��������� */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}

			/* ��ȡλ���ݵ��ڴ� */
			if ((b_status=read_psd(pfile, pinfo_str, &psd_header)) != ER_SUCCESS)
			{
				__leave;
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

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&psd_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* ����ͼ�� */
PSD_API int CALLAGREEMENT psd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&psd_save_img_critical);
	
			b_status = ER_NOTSUPPORT;
			/* ----------------------------------------------------------------->
			   Janhail:
			       1������ΪPSD�ĸ�ʽ���⣬��ʹ����Ҳֻ�ܱ���ɶ�ͨ��(���)�ĵ��ű���ͼ
			       2��PhotoShop ��ʶ��󲿷ֵ�ͼ���ʽ
			       3����PhotoShop ��PSD ��ʽ��Ӧ�ú���
			     ���Բ�Ҫ��������. &_* ���ǲ����е�����"��!!" ��Ҫ����˵���
			   ----------------------------------------------------------------->
			   YZ:
			       �Ǻ�:)����Ҳ��Ϊ��ʱ�Ȳ�ʵ�ֱ��湦�ܣ�Ҳ���漰��ʹ����ɵ����⡣
			*/

			/* �������� */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			LeaveCriticalSection(&psd_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/***********************************************************************************\
$                                                                                   $
$    ������ enum EXERESULT read_psd( ISFILE *pfile, 								$
$										LPINFOSTR pInfo, 							$
$										LPPSDFILEHEADER psdheader)                  $
$           ��ָ���ļ�����Ŀ�껺����                                                $
$                                                                                   $
$    ������ file        ָ���ļ�                                                    $
$           pInfo       ͼ���д���ݰ��ṹָ��                                      $
$			psdheader	PSD�ļ���Ϣͷ												$
$                                                                                   $
$    ���أ�enum EXERESULT �ͣ��μ�iseeirw.h����Ӧ����                               $
$                                                                                   $
\***********************************************************************************/
static enum EXERESULT CALLAGREEMENT read_psd(ISFILE *pfile, LPINFOSTR pInfo, LPPSDFILEHEADER psdheader)
{
	unsigned long	temp_data;

	int				j;
	unsigned long	imgdata;

	static char		palbuf[768];
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pInfo&&psdheader);


	__try
	{
		if (isio_seek(pfile, sizeof(PSDFILEHEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		/* ������ɫ��Ρ�ͼ����Դ�Ρ�ͼ�����ɰ�� */
		if (_skip_block( pfile )||_skip_block( pfile )||_skip_block( pfile ))
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* �����������ƫ�� */
		imgdata = isio_tell(pfile);

		
		/* �����ȡ�������� */
		switch (psdheader->Mode)
		{
		case	0:		/* ��ɫ�ڰ�ͼ */
			pInfo->pal_count = 2;
			
			pInfo->palette[0] = 0xffffff;
			pInfo->palette[1] = 0x0;

			if ((b_status=_read_bitmap(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	1:		/* �Ҷ�ͼ */
			pInfo->pal_count = 256;
			
			/* �˹��ϳɻҶȵ�ɫ�� */
			for (j=0;j<(int)pInfo->pal_count;j++)
			{
				pInfo->palette[j] = (unsigned long)((j<<16)|(j<<8)|(j));
			}

			if ((b_status=_read_grayscale_index_duotone(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	2:		/* ��ɫ��ͼ */
			pInfo->pal_count = 256;
			
			if (isio_seek(pfile, sizeof(PSDFILEHEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* ��ȡ��ɫ��ߴ�ֵ */
			if (isio_read((void*)&temp_data, sizeof(unsigned long), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave; break;
			}
			
			temp_data = EXCHANGE_DWORD( temp_data ) ;
			
			/* �����ɫ��ߴ粻��768��256��3��������Ϊ�ǻ���ͼ�� */
			if (temp_data != 768)
			{
				b_status = ER_BADIMAGE; __leave; break;
			}
			
			/* ��ȡ��ɫ����Ϣ */
			if (isio_read((void*)palbuf, 256*3, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave; break;
			}
			
			/* �������ݰ� */
			for (j=0;j<(int)pInfo->pal_count;j++)
			{
				pInfo->palette[j] = (unsigned long)(((unsigned long)(unsigned char)palbuf[j]<<16)|((unsigned long)(unsigned char)palbuf[j+256]<<8)|((unsigned long)(unsigned char)palbuf[j+512]));
			}

			if ((b_status=_read_grayscale_index_duotone(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	3:		/* RGBͼ */
			pInfo->pal_count = 0;

			if ((b_status=_read_rgb(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	4:		/* CMYKͼ */
			pInfo->pal_count = 0;

			if ((b_status=_read_cmyk(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	8:		/* ˫ɫ��ͼ */
			pInfo->pal_count = 256;
			
			/* �˹��ϳɻҶȵ�ɫ�� */
			for (j=0;j<(int)pInfo->pal_count;j++)
			{
				pInfo->palette[j] = (unsigned long)((j<<16)|(j<<8)|(j));
			}

			/* ˫ɫ��ͼ����Ϊ�Ҷ�ͼ���ȡ */
			if ((b_status=_read_grayscale_index_duotone(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	9:		/* Labͼ */
			pInfo->pal_count = 0;

			/* Lab��δ������ʽת�� */
			if ((b_status=_read_lab(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		default:
			b_status = ER_BADIMAGE; __leave; break;
		}
	}
	__finally
	{
	}

	return b_status;
}


/* ��BITMAP��ͼ�����ݰ� */
static enum EXERESULT CALLAGREEMENT _read_bitmap(ISFILE *pfile, 
										  LPINFOSTR pInfo, 
										  LPPSDFILEHEADER psdheader,
										  unsigned long imgdata)
{
	int				i, j, w, h, linesize;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	__try
	{
		/* ��λ������������ʼ�� */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ����ѹ���㷨�� */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* ��ת���� */
		compression = EXCHANGE_WORD(compression);

		w = (int)pInfo->width;
		h = (int)pInfo->height;

		/* ȡ��PSDɨ���гߴ磨һ�ֽڶ��룺Janhail��*/
		linesize = (w+7)/8;
		
		if (compression == 0)			/* NONE */
		{
			for (i=0; i<h; i++)
			{
				/* ��ȡһ�� */
				if (isio_read((void*)pInfo->pp_line_addr[i], linesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* ת�������ֽ���˳���Է���ISee��ʽ */
				for (j=0; j<linesize; j++)
				{
					CVT_BITS1((((unsigned char *)pInfo->pp_line_addr[i])[j]));
				}

				/* �ۼӲ���ֵ */
				pInfo->process_current = i;

				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* ��ȡRLE�г������� */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}
			
			assert(prle_header);
			
			/* ����RLE�����ݻ����� */
			if ((buf=(signed char *)malloc(linesize)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			assert(pInfo->p_bit_data);
			
			/* ��ȡRLE���ݲ������ѹ�����ݰ� */
			for (i=0; i<(int)(pInfo->height); i++)
			{
				if (_read_line(pfile, -1, prle_header[i], buf))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ��ѹһ�е�RLE���ݣ����Դ������ݵ������*/
				_decode_rle_line(buf, (long)prle_header[i], (unsigned char *)(pInfo->pp_line_addr[i]), (long)linesize);
				
				for (j=0; j<(int)linesize; j++)
				{
					CVT_BITS1((((unsigned char *)pInfo->pp_line_addr[i])[j]));
				}

				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf)
			free(buf);
		
		if (prle_header)
			free(prle_header);
	}

	return b_status;
}


/* ���Ҷȡ�������˫ɫ��ͼ�����ݰ� */
static enum EXERESULT CALLAGREEMENT _read_grayscale_index_duotone(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, w, h, linesize;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	__try
	{
		/* ��λ������������ʼ�� */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ����ѹ���㷨�� */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ��ת���� */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		/* ȡ��PSDɨ���гߴ� */
		linesize = w;
		
		if (compression == 0)			/* NONE */
		{
			for (i=0; i<h; i++)
			{
				/* ��ȡһ�� */
				if (isio_read((void*)pInfo->pp_line_addr[i], linesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* ��ȡRLE�г������� */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);

			/* ����RLE�����ݻ����� */
			if ((buf=(signed char *)malloc(psdheader->Columns*2+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			assert(pInfo->p_bit_data);

			/* ��ȡRLE���ݲ������ѹ�����ݰ� */
			for (i=0; i<(int)(pInfo->height); i++)
			{
				if (_read_line(pfile, -1, prle_header[i], buf))
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* ��ѹһ�е�RLE���ݣ����Դ������ݵ������*/
				_decode_rle_line(buf, (long)prle_header[i], (unsigned char *)(pInfo->pp_line_addr[i]), (long)psdheader->Columns);

				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}



/* ��RGBͼ�����ݰ� */
static enum EXERESULT CALLAGREEMENT _read_rgb(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, j, w, h;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0, *buf2 = 0;
	unsigned long	chan_imgsize, rpos, gpos, bpos, apos;
	unsigned char	*pr, *pg, *pb, *pa, *pdesc;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	__try
	{
		/* ��λ������������ʼ�� */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ����ѹ���㷨�� */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ��ת���� */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		if (compression == 0)			/* NONE */
		{
			/* һ��ͨ���������ݵĳߴ� */
			chan_imgsize = (unsigned long)w * (unsigned long)h;
			
			/* �����ͨ�������е�ƫ�� */
			rpos = (unsigned long)isio_tell(pfile);
			gpos = rpos + chan_imgsize;
			bpos = gpos + chan_imgsize;
			apos = bpos + chan_imgsize;
			
			/* ���������ݻ����� */
			if ((buf=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ��ͨ���ڻ������е���ʼλ�� */
			pr = buf;
			pg = pr+(psdheader->Columns+4);
			pb = pg+(psdheader->Columns+4);
			pa = pb+(psdheader->Columns+4);

			for (i=0; i<h; i++)
			{
				/* ��ȡһ�е�RGB���� */
				if (_read_line(pfile, rpos, (unsigned short)psdheader->Columns, (signed char *)pr))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, gpos, (unsigned short)psdheader->Columns, (signed char *)pg))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, (unsigned short)psdheader->Columns, (signed char *)pb))
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* �����һ������ͨ�������ȡ */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, apos, (unsigned short)psdheader->Columns, (signed char *)pa))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				rpos += (unsigned long)psdheader->Columns;
				gpos += (unsigned long)psdheader->Columns;
				bpos += (unsigned long)psdheader->Columns;
				apos += (unsigned long)psdheader->Columns;

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);

				/* �ϳ�RGB�������� */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					*pdesc++ = pb[j];
					*pdesc++ = pg[j];
					*pdesc++ = pr[j];

					if (psdheader->Channels == 4)
					{
						*pdesc++ = pa[j];
					}
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* ��ȡRLE�г������� */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);


			/* һ��ͨ���������ݵĳߴ� */
			chan_imgsize = 0UL;
			
			/* �����ͨ�������е�ƫ�� */
			rpos = (unsigned long)isio_tell(pfile);

			for (i=(int)psdheader->Rows*0; i<(int)psdheader->Rows*1; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			gpos = rpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*1; i<(int)psdheader->Rows*2; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			bpos = gpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*2; i<(int)psdheader->Rows*3; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			apos = bpos + chan_imgsize;
			

			/* ����RLE�����ݻ����� */
			if ((buf=(signed char *)malloc((psdheader->Columns*2+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* �����ѹ��������ݻ����� */
			if ((buf2=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ��ͨ���ڻ������е���ʼλ�� */
			pr = buf2;
			pg = pr+(psdheader->Columns+4);
			pb = pg+(psdheader->Columns+4);
			pa = pb+(psdheader->Columns+4);

			assert(pInfo->p_bit_data);

			/* ��ȡRLE���ݲ������ѹ�����ݰ� */
			for (i=0; i<(int)(pInfo->height); i++)
			{
#ifdef _DEBUG
				memset((void*)buf, 0, (psdheader->Columns*2+4)*4);
				memset((void*)buf2, 0, (psdheader->Columns+4)*4);
#endif
				if (_read_line(pfile, rpos, prle_header[psdheader->Rows*0+i], (signed char *)(buf+psdheader->Columns*2*0)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, gpos, prle_header[psdheader->Rows*1+i], (signed char *)(buf+psdheader->Columns*2*1)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, prle_header[psdheader->Rows*2+i], (signed char *)(buf+psdheader->Columns*2*2)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* �����һ������ͨ�������ȡ */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, apos, prle_header[psdheader->Rows*3+i], (signed char *)(buf+psdheader->Columns*2*3)))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				rpos += (unsigned long)prle_header[psdheader->Rows*0+i];
				gpos += (unsigned long)prle_header[psdheader->Rows*1+i];
				bpos += (unsigned long)prle_header[psdheader->Rows*2+i];

				if (psdheader->Channels == 4)
				{
					apos += (unsigned long)prle_header[psdheader->Rows*3+i];
				}
				
				/* ��ѹRLE���ݣ����Դ������ݵ������*/
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*0), (long)prle_header[psdheader->Rows*0+i], pr, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*1), (long)prle_header[psdheader->Rows*1+i], pg, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*2), (long)prle_header[psdheader->Rows*2+i], pb, (long)psdheader->Columns);

				if (psdheader->Channels == 4)
				{
					_decode_rle_line((signed char *)(buf+psdheader->Columns*2*3), (long)prle_header[psdheader->Rows*3+i], pa, (long)psdheader->Columns);
				}

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);
				
				/* �ϳ�RGB�������� */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					*pdesc++ = pb[j];
					*pdesc++ = pg[j];
					*pdesc++ = pr[j];
					
					if (psdheader->Channels == 4)
					{
						*pdesc++ = pa[j];
					}
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf2)
			free(buf2);

		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}


/* ��CMYKͼ�����ݰ� */
static enum EXERESULT CALLAGREEMENT _read_cmyk(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, j, w, h;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0, *buf2 = 0;
	unsigned long	chan_imgsize, cpos, mpos, ypos, kpos;
	unsigned char	*pc, *pm, *py, *pk, *pdesc, r, g, b;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psdheader->Channels == 4);		/* ֻ�ܴ���4ͨ������ */

	__try
	{
		/* ��λ������������ʼ�� */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ����ѹ���㷨�� */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ��ת���� */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		if (compression == 0)			/* NONE */
		{
			/* һ��ͨ���������ݵĳߴ� */
			chan_imgsize = (unsigned long)w * (unsigned long)h;
			
			/* �����ͨ�������е�ƫ�� */
			cpos = (unsigned long)isio_tell(pfile);
			mpos = cpos + chan_imgsize;
			ypos = mpos + chan_imgsize;
			kpos = ypos + chan_imgsize;
			
			/* ���������ݻ����� */
			if ((buf=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ��ͨ���ڻ������е���ʼλ�� */
			pc = buf;
			pm = pc+(psdheader->Columns+4);
			py = pm+(psdheader->Columns+4);
			pk = py+(psdheader->Columns+4);

			for (i=0; i<h; i++)
			{
				/* ��ȡһ�е�RGB���� */
				if (_read_line(pfile, cpos, (unsigned short)psdheader->Columns, (signed char *)pc))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, mpos, (unsigned short)psdheader->Columns, (signed char *)pm))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, ypos, (unsigned short)psdheader->Columns, (signed char *)py))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, kpos, (unsigned short)psdheader->Columns, (signed char *)pk))
				{
					b_status = ER_FILERWERR; __leave;
				}

				cpos += (unsigned long)psdheader->Columns;
				mpos += (unsigned long)psdheader->Columns;
				ypos += (unsigned long)psdheader->Columns;
				kpos += (unsigned long)psdheader->Columns;
				
				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);

				/* �ϳ�RGB�������� */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					/* ����һ���ֲܴڵ�ת����ʽ */
					CVT_CMYK_TO_RGB(((unsigned char)pc[j]), ((unsigned char)pm[j]), ((unsigned char)py[j]), ((signed char)pk[j]), r, g, b);

					*pdesc++ = b;
					*pdesc++ = g;
					*pdesc++ = r;
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* ��ȡRLE�г������� */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);


			/* һ��ͨ���������ݵĳߴ� */
			chan_imgsize = 0UL;
			
			/* �����ͨ�������е�ƫ�� */
			cpos = (unsigned long)isio_tell(pfile);

			for (i=(int)psdheader->Rows*0; i<(int)psdheader->Rows*1; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			mpos = cpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*1; i<(int)psdheader->Rows*2; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			ypos = mpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*2; i<(int)psdheader->Rows*3; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			kpos = ypos + chan_imgsize;
			

			/* ����RLE�����ݻ����� */
			if ((buf=(signed char *)malloc((psdheader->Columns*2+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* �����ѹ��������ݻ����� */
			if ((buf2=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ��ͨ���ڻ������е���ʼλ�� */
			pc = buf2;
			pm = pc+(psdheader->Columns+4);
			py = pm+(psdheader->Columns+4);
			pk = py+(psdheader->Columns+4);

			assert(pInfo->p_bit_data);

			/* ��ȡRLE���ݲ������ѹ�����ݰ� */
			for (i=0; i<(int)(pInfo->height); i++)
			{
#ifdef _DEBUG
				memset((void*)buf, 0, (psdheader->Columns*2+4)*4);
				memset((void*)buf2, 0, (psdheader->Columns+4)*4);
#endif
				if (_read_line(pfile, cpos, prle_header[psdheader->Rows*0+i], (signed char *)(buf+psdheader->Columns*2*0)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, mpos, prle_header[psdheader->Rows*1+i], (signed char *)(buf+psdheader->Columns*2*1)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, ypos, prle_header[psdheader->Rows*2+i], (signed char *)(buf+psdheader->Columns*2*2)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, kpos, prle_header[psdheader->Rows*3+i], (signed char *)(buf+psdheader->Columns*2*3)))
				{
					b_status = ER_FILERWERR; __leave;
				}

				cpos += (unsigned long)prle_header[psdheader->Rows*0+i];
				mpos += (unsigned long)prle_header[psdheader->Rows*1+i];
				ypos += (unsigned long)prle_header[psdheader->Rows*2+i];
				kpos += (unsigned long)prle_header[psdheader->Rows*3+i];
				
				/* ��ѹRLE���ݣ����Դ������ݵ������*/
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*0), (long)prle_header[psdheader->Rows*0+i], pc, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*1), (long)prle_header[psdheader->Rows*1+i], pm, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*2), (long)prle_header[psdheader->Rows*2+i], py, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*3), (long)prle_header[psdheader->Rows*3+i], pk, (long)psdheader->Columns);

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);
				
				/* �ϳ�RGB�������� */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					CVT_CMYK_TO_RGB(((unsigned char)pc[j]), ((unsigned char)pm[j]), ((unsigned char)py[j]), ((signed char)pk[j]), r, g, b);
					
					*pdesc++ = b;
					*pdesc++ = g;
					*pdesc++ = r;
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf2)
			free(buf2);

		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}


/* ��Labͼ�����ݰ� */
static enum EXERESULT CALLAGREEMENT _read_lab(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, j, w, h;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0, *buf2 = 0;
	unsigned long	chan_imgsize, lpos, apos, bpos, hpos;
	unsigned char	*pl, *pa, *pb, *ph, *pdesc;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert((psdheader->Channels == 3)||(psdheader->Channels == 4));

	__try
	{
		/* ��λ������������ʼ�� */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ����ѹ���㷨�� */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* ��ת���� */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		if (compression == 0)			/* NONE */
		{
			/* һ��ͨ���������ݵĳߴ� */
			chan_imgsize = (unsigned long)w * (unsigned long)h;
			
			/* �����ͨ�������е�ƫ�� */
			lpos = (unsigned long)isio_tell(pfile);
			apos = lpos + chan_imgsize;
			bpos = apos + chan_imgsize;
			hpos = bpos + chan_imgsize;
			
			/* ���������ݻ����� */
			if ((buf=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* ��ͨ���ڻ������е���ʼλ�� */
			pl = buf;
			pa = pl+(psdheader->Columns+4);
			pb = pa+(psdheader->Columns+4);
			ph = pb+(psdheader->Columns+4);

			for (i=0; i<h; i++)
			{
				/* ��ȡһ�е�RGB���� */
				if (_read_line(pfile, lpos, (unsigned short)psdheader->Columns, (signed char *)pl))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, apos, (unsigned short)psdheader->Columns, (signed char *)pa))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, (unsigned short)psdheader->Columns, (signed char *)pb))
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* �����һ������ͨ�������ȡ */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, hpos, (unsigned short)psdheader->Columns, (signed char *)ph))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				lpos += (unsigned long)psdheader->Columns;
				apos += (unsigned long)psdheader->Columns;
				bpos += (unsigned long)psdheader->Columns;
				hpos += (unsigned long)psdheader->Columns;

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);

				/* �ϳ�RGB�������� */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					/* Lab to RGB���˴�û����Lab to RGB��ת��������ȱ��....�� */
					*pdesc++ = pb[j];
					*pdesc++ = pa[j];
					*pdesc++ = pl[j];

					if (psdheader->Channels == 4)
					{
						*pdesc++ = ph[j];
					}
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* ��ȡRLE�г������� */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);


			/* һ��ͨ���������ݵĳߴ� */
			chan_imgsize = 0UL;
			
			/* �����ͨ�������е�ƫ�� */
			lpos = (unsigned long)isio_tell(pfile);

			for (i=(int)psdheader->Rows*0; i<(int)psdheader->Rows*1; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			apos = lpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*1; i<(int)psdheader->Rows*2; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			bpos = apos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*2; i<(int)psdheader->Rows*3; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			hpos = bpos + chan_imgsize;
			

			/* ����RLE�����ݻ����� */
			if ((buf=(signed char *)malloc((psdheader->Columns*2+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* �����ѹ��������ݻ����� */
			if ((buf2=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* ��ͨ���ڻ������е���ʼλ�� */
			pl = buf2;
			pa = pl+(psdheader->Columns+4);
			pb = pa+(psdheader->Columns+4);
			ph = pb+(psdheader->Columns+4);

			assert(pInfo->p_bit_data);

			/* ��ȡRLE���ݲ������ѹ�����ݰ� */
			for (i=0; i<(int)(pInfo->height); i++)
			{
#ifdef _DEBUG
				memset((void*)buf, 0, (psdheader->Columns*2+4)*4);
				memset((void*)buf2, 0, (psdheader->Columns+4)*4);
#endif
				if (_read_line(pfile, lpos, prle_header[psdheader->Rows*0+i], (signed char *)(buf+psdheader->Columns*2*0)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, apos, prle_header[psdheader->Rows*1+i], (signed char *)(buf+psdheader->Columns*2*1)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, prle_header[psdheader->Rows*2+i], (signed char *)(buf+psdheader->Columns*2*2)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* �����һ������ͨ�������ȡ */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, hpos, prle_header[psdheader->Rows*3+i], (signed char *)(buf+psdheader->Columns*2*3)))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				lpos += (unsigned long)prle_header[psdheader->Rows*0+i];
				apos += (unsigned long)prle_header[psdheader->Rows*1+i];
				bpos += (unsigned long)prle_header[psdheader->Rows*2+i];

				if (psdheader->Channels == 4)
				{
					hpos += (unsigned long)prle_header[psdheader->Rows*3+i];
				}
				
				/* ��ѹRLE���ݣ����Դ������ݵ������*/
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*0), (long)prle_header[psdheader->Rows*0+i], pl, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*1), (long)prle_header[psdheader->Rows*1+i], pa, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*2), (long)prle_header[psdheader->Rows*2+i], pb, (long)psdheader->Columns);

				if (psdheader->Channels == 4)
				{
					_decode_rle_line((signed char *)(buf+psdheader->Columns*2*3), (long)prle_header[psdheader->Rows*3+i], ph, (long)psdheader->Columns);
				}

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);
				
				/* �ϳ�RGB�������� */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					/* Lab to RGB���˴�û����Lab to RGB��ת��������ȱ��....�� */
					*pdesc++ = pb[j];
					*pdesc++ = pa[j];
					*pdesc++ = pl[j];
					
					if (psdheader->Channels == 4)
					{
						*pdesc++ = ph[j];
					}
				}
				
				/* �ۼӲ���ֵ */
				pInfo->process_current = i;
				
				/* ֧���û��ж� */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf2)
			free(buf2);

		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}


/* ��ȡRLE�г������� */
static int CALLAGREEMENT _get_rle_header(ISFILE *pfile, LPPSDFILEHEADER psdheader, unsigned short **prle_header)
{
	int					header_size, i;
	unsigned short		*ps;

	assert(pfile&&psdheader&&prle_header);

	*prle_header = (unsigned short *)0;

	/* ���鳤�ȣ��ֽڵ�λ�� */
	header_size = psdheader->Rows * psdheader->Channels * sizeof(unsigned short);

	/* �����̼��赱ǰ����дλ����ѹ����ʽ��֮��*/
	if ((ps=(unsigned short *)malloc(header_size)) == 0)
	{
		return -1;		/* �ڴ治�� */
	}

	/* ��ȡRLE�г������� */
	if (isio_read((void*)ps, header_size, 1, pfile) == 0)
	{
		free(ps);
		return -2;		/* ������ */
	}

	/* ��ת���� */
	for (i=0; i<(int)(header_size/sizeof(unsigned short)); i++)
	{
		ps[i] = EXCHANGE_WORD(ps[i]);
	}

	*prle_header = ps;

	return 0;			/* �ɹ� */
}


/* ��ȡһ��RLEѹ������ */
static int CALLAGREEMENT _read_line(ISFILE *pfile, long pos, unsigned short size, signed char *pdata)
{
	assert(pdata);

	if (pos >= 0)
	{
		if (isio_seek(pfile, pos, SEEK_SET) == -1)
		{
			return -1;		/* ������ */
		}
	}

	/* ��ȡRLE���� */
	if (isio_read((void*)pdata, size, 1, pfile) == 0)
	{
		return -1;		/* ������ */
	}
	
	return 0;
}


/* ��ѹһ��RLE���� */
static int CALLAGREEMENT _decode_rle_line(signed char *psou, long ssize, unsigned char *pdes, long dsize)
{
	signed char		n;
	unsigned char	b;
	int				mark = 0;

	assert(psou&&pdes);

	while ((ssize>0) && (dsize>0))
	{
		/* ��ȡһ�ֽڵ�Դ���� */
		n = *psou++, ssize--;
		
		if (n < 0)
		{
			if (n == -128)
			{
				continue;
			}

			/* ����������ֽڵ��ظ����� */
            n = -n + 1;

			/* �����������ת���ܼ��֣����в����ͻ��������¡�*/
            if( dsize < (long)(unsigned int)(unsigned char)n )
            {
                /* ���Ŀ�껺�����޷����� n �ֽڵ����ݣ�������һЩ���ݡ����ݴ��� */
                n = (signed char)(unsigned char)dsize, dsize = 0, mark = 1;
            }
			else
			{
				dsize -= (long)(unsigned int)(unsigned char)n;
			}

			b = *psou++, ssize--;

			/* д���ظ����� */
			while ((int)(unsigned int)(unsigned char)n-- > 0)
			{
				*pdes++ = b;
			}
		}
		else
		{
			if (dsize < ((long)(unsigned int)(unsigned char)n+1))
            {
                /* ���Ŀ�껺�����޷����ɽ�ѹ���ݣ��������������� */
                n = (signed char)((unsigned char)dsize - 1), mark = 1;
            }
			/* ֱ�ӿ���δ��ѹ�������� */
            memcpy(pdes, psou, (size_t)(unsigned int)(unsigned char)(++n));

			pdes += (unsigned int)(unsigned char)n; dsize -= (unsigned int)(unsigned char)n;
			psou += (unsigned int)(unsigned char)n; ssize -= (unsigned int)(unsigned char)n;
		}
	}

	if (dsize > 0)
	{
		mark = 1;
	}

	/* ����ֵ��0���ɹ���1��Դ�����д��󣬵��Ѿ����ݴ��� */
	return mark;
}


/***********************************************************************************\
$                                                                                   $
$    ������int _is_support_format( unsigned short, unsigned short ) ;               $
$          �����ɫģʽ��ɫ���Ƿ�֧��                                             $
$                                                                                   $
$    ������color_mode   ��ɫ��ģʽ                                                  $
$                 Bitmap=0, Grayscale=1, Indexed=2, RGB=3,                          $
$                 CMYK=4, Multichannel=7, Duotone=8, Lab=9                          $
$          color_pixel  ��ɫ��λ��                                                  $
$                 PSD ��ʽֻ�� 1, 8, 16                                             $
$                                                                                   $
$    ���أ�int ��                                                                   $
$                 �ɹ�֧��=0, ��ɫģʽ��֧��=1, λ�֧��=2                        $
$                                                                                   $
\***********************************************************************************/
int	CALLAGREEMENT _is_support_format( unsigned short color_mode, unsigned short color_pixel )
{
	/* �����ǲ��� color_mode ,color_pixel ����ȷ�� */
	switch( color_mode )
	{
	case 0:	/* Bitmap */
		if( color_pixel == 1)	break ;
		return	2 ;
	case 1:	/* Grayscale */
	case 2:	/* Indexed */
	case 3:	/* RGB */
	case 4:	/* CMYK */
	case 8:	/* Duotone (���Ҷȵķ�����ȡ) */
	case 9:	/* Lab */
		if( color_pixel == 8 )	break ;
		return	2 ;
	case 7:	/* Multichannel���ݲ�֧�ֶ�ͨ��ͼ�� */
	default:/* Unknow -> Error */
		return	1 ;
	}
	return 0 ;
}


/***********************************************************************************\
$                                                                                   $
$    ������int _skip_block( ISFILE *pfile ) ;                                       $
$          ������һ���飬���ƶ����Ķ�дָ��                                         $
$                                                                                   $
$    ������pfile   ͼ����ָ��                                                       $
$                                                                                   $
$    ���أ�int ��                                                                   $
$                 �ɹ�=0, ��ʧ��=-1                                                 $
$                                                                                   $
\***********************************************************************************/
int	CALLAGREEMENT _skip_block( ISFILE *pfile )
{
	unsigned long	temp_data;

	if (isio_read((void*)&temp_data, sizeof(unsigned long), 1, pfile) == 0)
	{
		return -1;
	}

	temp_data = EXCHANGE_DWORD( temp_data ) ;

	if( temp_data )
	{
		if (isio_seek(pfile, temp_data, SEEK_CUR) == -1)
		{
			return -1;
		}
	}

	return 0;
}


