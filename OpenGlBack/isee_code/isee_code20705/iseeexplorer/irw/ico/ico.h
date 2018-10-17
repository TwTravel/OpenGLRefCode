/********************************************************************

	ico.h

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

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������ICOͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�1��4��8��16��24��32λICOͼ��
							  
					���湦�ܣ���֧��
							   
	  
	���ļ���д�ˣ�	
					YZ		yzfree##yeah.net
		
	���ļ��汾��	10103
	����޸��ڣ�	2001-1-3
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
			  2002-3		�����°�ģ�顣ȥ����������Windowsϵͳ�й�
								��API���ã�ʹ�����������ֲ��
			  2001-1		������ģ����ڵ�һЩBUG��
			  2000-8		��ǿ��ģ����ݴ����ܡ�
			  2000-8		��һ�������档

  
********************************************************************/



#ifndef __ICO_MODULE_INC__
#define __ICO_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("ICO���")
#define MODULE_FUNC_PREFIX				("ico_")
#define MODULE_FILE_POSTFIX				("ico")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  ICO_EXPORTS
#	define ICO_API __declspec(dllexport)
#	else
#	define ICO_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define ICO_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* ͼ�����Ͷ��� */
#define ICONTYPE	1


/* ������ͼ�����Ͷ��� */
enum ICO_DATA_TYPE
{
	ICO_PRI_IMAGE,
	ICO_SUB_IMAGE
};


/* ͼ�����Ϣͷ�ṹ */
typedef struct
{
	unsigned char	width;			/* ͼ���� */
	unsigned char	height;			/* ͼ��߶� */
	unsigned char	color_count;	/* ��ɫ����(���λ��ȴ���8ʱΪ0) */
	unsigned char	reserved;
	unsigned short	planes;			/* λ���� */
	unsigned short	bit_count;		/* ÿ����λ�� */
	unsigned long	bytes_in_res;	/* ͼ����������Դ����ռ���ֽ��� */
	unsigned long	image_offset;	/* ͼ�����ݵ�ƫ�� */
} ICONDIRENTRY, *LPICONDIRENTRY;


/* ͼ���ļ���Ϣͷ�ṹ */
typedef struct 
{
	unsigned short	reserved;		/* ���� */
	unsigned short	type;			/* ��Դ����(ICONTYPEΪͼ��) */
	unsigned short	count;			/* ͼ����� */
	ICONDIRENTRY	entries[1];		/* ÿһ��ͼ������ */
} ICONDIR, *LPICONDIR;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern ICO_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern ICO_API LPIRWP_INFO	CALLAGREEMENT ico_get_plugin_info(void);
extern ICO_API int			CALLAGREEMENT ico_init_plugin(void);
extern ICO_API int			CALLAGREEMENT ico_detach_plugin(void);
#endif	/* WIN32 */

extern ICO_API int	CALLAGREEMENT ico_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern ICO_API int	CALLAGREEMENT ico_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern ICO_API int	CALLAGREEMENT ico_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ICO_MODULE_INC__ */
