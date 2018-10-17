/********************************************************************

	emf.h

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

			isee##vip.163.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������EMFͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�	�ɶ�ȡ��ǿ��Ԫ�ļ���EMF���������ļ����ı���ʾ������
							  
					���湦�ܣ�	��֧�ֱ��湦��
							   

	���ļ���д�ˣ�	YZ			yzfree##yeah.net
					swstudio	swstudio##sohu.com

	���ļ��汾��	20615
	����޸��ڣ�	2002-6-15

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-6		�ڶ������������°棩
		2001-1		��������ע����Ϣ
		2000-7		�����ദBUG������ǿ��ģ����ݴ���
		2000-6		��һ���汾����

			

********************************************************************/



#ifndef __EMF_MODULE_INC__
#define __EMF_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("EMF���")
#define MODULE_FUNC_PREFIX				("emf_")
#define MODULE_FILE_POSTFIX				("emf")


#ifdef WIN32	/* Windows */

#	ifdef  EMF_EXPORTS
#	define EMF_API __declspec(dllexport)
#	else
#	define EMF_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define EMF_API
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
extern EMF_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern EMF_API LPIRWP_INFO	CALLAGREEMENT emf_get_plugin_info(void);
extern EMF_API int			CALLAGREEMENT emf_init_plugin(void);
extern EMF_API int			CALLAGREEMENT emf_detach_plugin(void);
#endif	/* WIN32 */

extern EMF_API int	CALLAGREEMENT emf_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern EMF_API int	CALLAGREEMENT emf_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern EMF_API int	CALLAGREEMENT emf_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __EMF_MODULE_INC__ */
