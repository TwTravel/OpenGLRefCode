/********************************************************************

	wmf.h

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
	���ļ���;��	ISeeͼ���������WMFͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�	�ɶ�ȡWindows��׼Ԫ�ļ���Aldus�ɷ���Ԫ�ļ���
							  
					���湦�ܣ�	��֧�ֱ��湦�ܡ�
							   
	  
	���ļ���д�ˣ�	YZ			yzfree##sina.com����һ�棩
					swstudio	swstudio##sohu.com���ڶ��棩
		
	���ļ��汾��	20527
	����޸��ڣ�	2002-5-27
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-5		�ڶ������������°棩
		2001-1		��������ע����Ϣ
		2000-7		�����ദBUG������ǿ��ģ����ݴ���
		2000-6		��һ���汾����
			

********************************************************************/



#ifndef __WMF_MODULE_INC__
#define __WMF_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("WMF���")
#define MODULE_FUNC_PREFIX				("wmf_")
#define MODULE_FILE_POSTFIX				("wmf")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  WMF_EXPORTS
#	define WMF_API __declspec(dllexport)
#	else
#	define WMF_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define WMF_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* �ļ��ṹ���� */
#define	ALDUSKEY				0x9AC6CDD7
#define	ALDUSMETAHEADERSIZE		22

typedef struct 
{
	unsigned int	key;				/* �������ALDUSKEY */
	unsigned short	hmf;
	short			left;
	short			top;
	short			right;
	short			bottom;
    unsigned short	inch;
    unsigned int	reserved;
    unsigned short	checksum;
} ALDUSMETAHEADER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern WMF_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern WMF_API LPIRWP_INFO	CALLAGREEMENT wmf_get_plugin_info(void);
extern WMF_API int			CALLAGREEMENT wmf_init_plugin(void);
extern WMF_API int			CALLAGREEMENT wmf_detach_plugin(void);
#endif	/* WIN32 */

extern WMF_API int	CALLAGREEMENT wmf_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern WMF_API int	CALLAGREEMENT wmf_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern WMF_API int	CALLAGREEMENT wmf_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __WMF_MODULE_INC__ */
