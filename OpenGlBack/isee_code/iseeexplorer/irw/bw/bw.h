/********************************************************************

	bw.h

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
	���ļ���;��	ISeeͼ���������BWͼ���дģ�鶨���ļ�
	
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



#ifndef __BW_MODULE_INC__
#define __BW_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("BW���")
#define MODULE_FUNC_PREFIX				("bw_")
#define MODULE_FILE_POSTFIX				("bw")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  BW_EXPORTS
#	define BW_API __declspec(dllexport)
#	else
#	define BW_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define BW_API
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
extern BW_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern BW_API LPIRWP_INFO	CALLAGREEMENT bw_get_plugin_info(void);
extern BW_API int			CALLAGREEMENT bw_init_plugin(void);
extern BW_API int			CALLAGREEMENT bw_detach_plugin(void);
#endif	/* WIN32 */

extern BW_API int	CALLAGREEMENT bw_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern BW_API int	CALLAGREEMENT bw_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern BW_API int	CALLAGREEMENT bw_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __BW_MODULE_INC__ */
