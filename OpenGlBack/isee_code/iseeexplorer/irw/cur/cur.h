/********************************************************************

	cur.h

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
	���ļ���;��	ISeeͼ���������CURͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�1��4��8��16��24��32λ���ͼ��
							  
					���湦�ܣ���֧��
							   
	  
	���ļ���д�ˣ�	
					YZ		yzfree##yeah.net
		
	���ļ��汾��	20327
	����޸��ڣ�	2002-3-27
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
			2002-3		�����°�ģ�顣ȥ����������Windowsϵͳ�й�
							��API���ã�ʹ�����������ֲ��
			2000-8		��һ�������档


********************************************************************/



#ifndef __CUR_MODULE_INC__
#define __CUR_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("CUR���")
#define MODULE_FUNC_PREFIX				("cur_")
#define MODULE_FILE_POSTFIX				("cur")


#ifdef WIN32	/* Windows */

#	ifdef  CUR_EXPORTS
#	define CUR_API __declspec(dllexport)
#	else
#	define CUR_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define CUR_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* ������Ͷ��� */
#define CURTYPE		2


/* ������ͼ�����Ͷ��� */
enum CUR_DATA_TYPE
{
	CUR_PRI_IMAGE,
	CUR_SUB_IMAGE
};


/* ����ͼ�����Ϣͷ�ṹ */
typedef struct
{
	unsigned char	width;			/* ͼ���� */
	unsigned char	height;			/* ͼ��߶� */
	unsigned char	color_count;	/* ��ɫ����(���λ��ȴ���8ʱΪ0) */
	unsigned char	reserved;
	unsigned short	x_hotspot;		/* �ȵ�X���� */
	unsigned short	y_hotspot;		/* �ȵ�Y���� */
	unsigned long	bytes_in_res;	/* ͼ����������Դ����ռ���ֽ��� */
	unsigned long	image_offset;	/* ͼ�����ݵ�ƫ�� */
} CURDIRENTRY, *LPCURDIRENTRY;


/* �����Ϣͷ�ṹ */
typedef struct 
{
	unsigned short	reserved;		/* ���� */
	unsigned short	type;			/* ��Դ����(CURTYPEΪ���) */
	unsigned short	count;			/* ͼ����� */
	CURDIRENTRY		entries[1];		/* ÿһ��ͼ������ */
} CURDIR, *LPCURDIR;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"				/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern CUR_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern CUR_API LPIRWP_INFO	CALLAGREEMENT cur_get_plugin_info(void);
extern CUR_API int			CALLAGREEMENT cur_init_plugin(void);
extern CUR_API int			CALLAGREEMENT cur_detach_plugin(void);
#endif	/* WIN32 */

extern CUR_API int	CALLAGREEMENT cur_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern CUR_API int	CALLAGREEMENT cur_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern CUR_API int	CALLAGREEMENT cur_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __CUR_MODULE_INC__ */
