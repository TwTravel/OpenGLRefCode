/********************************************************************

	jbig.h

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
	���ļ���;��	ISeeͼ���������JBIGͼ���дģ�鶨���ļ�
	
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



#ifndef __JBIG_MODULE_INC__
#define __JBIG_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("JBIG���")
#define MODULE_FUNC_PREFIX				("jbig_")
#define MODULE_FILE_POSTFIX				("jbg")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  JBIG_EXPORTS
#	define JBIG_API __declspec(dllexport)
#	else
#	define JBIG_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define JBIG_API
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
extern JBIG_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern JBIG_API LPIRWP_INFO	CALLAGREEMENT jbig_get_plugin_info(void);
extern JBIG_API int			CALLAGREEMENT jbig_init_plugin(void);
extern JBIG_API int			CALLAGREEMENT jbig_detach_plugin(void);
#endif	/* WIN32 */

extern JBIG_API int	CALLAGREEMENT jbig_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern JBIG_API int	CALLAGREEMENT jbig_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern JBIG_API int	CALLAGREEMENT jbig_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __JBIG_MODULE_INC__ */
