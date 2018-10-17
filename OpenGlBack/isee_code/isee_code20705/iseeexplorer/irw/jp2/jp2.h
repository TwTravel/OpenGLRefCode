/********************************************************************

	jp2.h

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
	���ļ���;��	ISeeͼ���������JPEG2000ͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�
							  
					���湦�ܣ�
							   
	  
	���ļ���д�ˣ�	
		
	���ļ��汾��	
	����޸��ڣ�	
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			

********************************************************************/



#ifndef __JP2_MODULE_INC__
#define __JP2_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("JP2���")
#define MODULE_FUNC_PREFIX				("jp2_")
#define MODULE_FILE_POSTFIX				("jp2")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  JP2_EXPORTS
#	define JP2_API __declspec(dllexport)
#	else
#	define JP2_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define JP2_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* �ļ��ṹ���� */









#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern JP2_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern JP2_API LPIRWP_INFO	CALLAGREEMENT jp2_get_plugin_info(void);
extern JP2_API int			CALLAGREEMENT jp2_init_plugin(void);
extern JP2_API int			CALLAGREEMENT jp2_detach_plugin(void);
#endif	/* WIN32 */

extern JP2_API int	CALLAGREEMENT jp2_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern JP2_API int	CALLAGREEMENT jp2_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern JP2_API int	CALLAGREEMENT jp2_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __JP2_MODULE_INC__ */
