/********************************************************************

	psd.h

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
	���ļ���;��	ISeeͼ���������PSDͼ���дģ�鶨���ļ�
	
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



#ifndef __PSD_MODULE_INC__
#define __PSD_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PSD���")
#define MODULE_FUNC_PREFIX				("psd_")
#define MODULE_FILE_POSTFIX				("PSD")

/* �ļ��б��־ */
#define PSD_SIGNATURE_MARKER			0x53504238	/* "8BPS" */
#define PSD_VERSION_MARKER				0x100		/* 256 ����������־��ΪMOTO����*/


#ifdef WIN32	/* Windows */

#	ifdef  PSD_EXPORTS
#	define PSD_API			__declspec(dllexport)
#	else
#	define PSD_API			__declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PSD_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* �ļ���Ϣͷ�ṹ */
typedef struct
{
	unsigned long		Signature;	/* ��ǩ��"8BPS" */
	unsigned short		Version;	/* �汾�š����ǵ���1 */
	unsigned short		Reserved0;
	unsigned short		Reserved1;	/* ����6���ֽ� */
	unsigned short		Reserved2;
	unsigned short		Channels;	/* ͨ����(��Χ��1��24) */
	unsigned long		Rows;		/* ����	(��Χ: 1 �� 30000) */
	unsigned long		Columns;	/* ���� (��Χ: 1 �� 30000) */
	unsigned short		Depth;		/* ÿͨ����λ��(���ܵ�ֵ��: 1, 8, 16) */
	unsigned short		Mode;		/* ɫ��ģʽ����ֵ�����������һ��:*/
									/*  Bitmap		=0
										Grayscale	=1 
										Indexed		=2 
										RGB			=3 
										CMYK		=4 
										Multichannel=7 
										Duotone		=8 
										Lab			=9 */
}PSDFILEHEADER, *LPPSDFILEHEADER;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif


#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PSD_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PSD_API LPIRWP_INFO CALLAGREEMENT psd_get_plugin_info(void);
extern PSD_API int			CALLAGREEMENT psd_init_plugin(void);
extern PSD_API int			CALLAGREEMENT psd_detach_plugin(void);
#endif	/* WIN32 */

extern PSD_API int CALLAGREEMENT psd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PSD_API	int	CALLAGREEMENT psd_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PSD_API int	CALLAGREEMENT psd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PSD_MODULE_INC__ */
