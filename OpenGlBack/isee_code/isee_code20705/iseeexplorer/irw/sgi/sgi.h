/********************************************************************

	sgi.h

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
	���ļ���;��	ISeeͼ���������SGIͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�8��24��32λSGIͼ�󣨰���ѹ����δѹ����
							  
					���湦�ܣ�24λ��ѹ����ʽ
							   
	  
	���ļ���д�ˣ�	YZ			yzfree##yeah.net
		
	���ļ��汾��	20428
	����޸��ڣ�	2002-4-28
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

			2002-4		������һ���°汾

********************************************************************/



#ifndef __SGI_MODULE_INC__
#define __SGI_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("SGI���")
#define MODULE_FUNC_PREFIX				("sgi_")
#define MODULE_FILE_POSTFIX				("sgi")

/* �ļ��б��־ */
#define SGI_MAGIC						0x1da

#ifdef WIN32	/* Windows */

#	ifdef  SGI_EXPORTS
#	define SGI_API __declspec(dllexport)
#	else
#	define SGI_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define SGI_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


#define SGI_REV0_SIZE		4
#define SGI_REV1_SIZE		404
#define SGI_NAME_SIZE		80


/* �ļ�ͷ�ṹ���� */
typedef struct _tag_sgi_header
{
	unsigned short	magic;					/* �ļ���ʶ */
	unsigned char	storage;				/* �洢��ʽ */
	unsigned char	bpc;					/* ÿ��ͨ�����ص��ֽ��� */
	unsigned short	dim;					/* ά�� */
	unsigned short	width;					/* ��� */
	unsigned short	hight;					/* �߶� */
	unsigned short	channel;				/* ͨ���� */
	long			pixmin;					/* ��С����ֵ */
	long			pixmax;					/* �������ֵ */
	char			rev0[SGI_REV0_SIZE];	/* ���� */
	char			imgname[SGI_NAME_SIZE];	/* ͼ�������� */
	long			palid;					/* ��ɫ��ID */
	char			rev1[SGI_REV1_SIZE];	/* ���� */
} SGIHEADER, *LPSGIHEADER;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"						/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern SGI_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern SGI_API LPIRWP_INFO	CALLAGREEMENT sgi_get_plugin_info(void);
extern SGI_API int			CALLAGREEMENT sgi_init_plugin(void);
extern SGI_API int			CALLAGREEMENT sgi_detach_plugin(void);
#endif	/* WIN32 */

extern SGI_API int	CALLAGREEMENT sgi_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern SGI_API int	CALLAGREEMENT sgi_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern SGI_API int	CALLAGREEMENT sgi_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __SGI_MODULE_INC__ */
