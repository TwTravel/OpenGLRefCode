/********************************************************************

	pnm.h

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
	���ļ���;��	ISeeͼ���������PBM��PGM��PPMͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�PBM��PGM��PPM �ı���������ͼ��
							  
					���湦�ܣ��ɽ�1��8��24λͼ�񱣴�Ϊ��Ӧ��
							  PBM��PGM��PPM �ı��������ͼ���ʽ 
	  
	���ļ���д�ˣ�	YZ			yzfree##yeah.net
		
	���ļ��汾��	20207
	����޸��ڣ�	2002-2-7
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
			2002-2	��һ�������棨�°�ӿڣ�������汾2.0

********************************************************************/



#ifndef __PNM_MODULE_INC__
#define __PNM_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PNM���")
#define MODULE_FUNC_PREFIX				("pnm_")
#define MODULE_FILE_POSTFIX				("pbm")


#ifdef WIN32	/* Windows */

#	ifdef  PNM_EXPORTS
#	define PNM_API __declspec(dllexport)
#	else
#	define PNM_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PNM_API
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
extern PNM_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PNM_API LPIRWP_INFO	CALLAGREEMENT pnm_get_plugin_info(void);
extern PNM_API int			CALLAGREEMENT pnm_init_plugin(void);
extern PNM_API int			CALLAGREEMENT pnm_detach_plugin(void);
#endif	/* WIN32 */

extern PNM_API int	CALLAGREEMENT pnm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PNM_API int	CALLAGREEMENT pnm_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PNM_API int	CALLAGREEMENT pnm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PNM_MODULE_INC__ */
