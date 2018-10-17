/********************************************************************

	xbm.h

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
	���ļ���;��	ISeeͼ���������XBMͼ���дģ�鶨���ļ�
	
			��ȡ���ܣ�X10��X11��ʽXBMͼ�񣨶�ע����Ϣ����
						�ݴ��ԣ���
			���湦�ܣ�X11��ʽXBM
							   
	  
	���ļ���д�ˣ�	YZ			yzfree##yeah.net
		
	���ļ��汾��	20113
	����޸��ڣ�	2002-1-13
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
			2002-1		��һ�η������°汾����ʼ�汾��2.0��


********************************************************************/



#ifndef __XBM_MODULE_INC__
#define __XBM_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("XBM���")
#define MODULE_FUNC_PREFIX				("xbm_")
#define MODULE_FILE_POSTFIX				("xbm")


#ifdef WIN32	/* Windows */

#	ifdef  XBM_EXPORTS
#	define XBM_API __declspec(dllexport)
#	else
#	define XBM_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define XBM_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif


#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern XBM_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern XBM_API LPIRWP_INFO	CALLAGREEMENT xbm_get_plugin_info(void);
extern XBM_API int			CALLAGREEMENT xbm_init_plugin(void);
extern XBM_API int			CALLAGREEMENT xbm_detach_plugin(void);
#endif	/* WIN32 */

extern XBM_API int	CALLAGREEMENT xbm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern XBM_API int	CALLAGREEMENT xbm_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern XBM_API int	CALLAGREEMENT xbm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __XBM_MODULE_INC__ */
