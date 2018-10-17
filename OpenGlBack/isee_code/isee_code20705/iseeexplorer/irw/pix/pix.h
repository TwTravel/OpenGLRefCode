/********************************************************************

	pix.h

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
	���ļ���;��	ISeeͼ���������Alias PIXͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ�ȡ8��24λRLEѹ��Alias PIXͼ��
							  
					���湦�ܣ�24λRLEѹ����ʽ
							   
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	020507
	����޸��ڣ�	2002-5-7
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-5		��һ�������棨�°棩
			

********************************************************************/



#ifndef __PIX_MODULE_INC__
#define __PIX_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PIX���")
#define MODULE_FUNC_PREFIX				("pix_")
#define MODULE_FILE_POSTFIX				("pix")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  PIX_EXPORTS
#	define PIX_API __declspec(dllexport)
#	else
#	define PIX_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PIX_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* ��ͷ�ṹ���� */
typedef struct
{
	unsigned short	width;			/* ͼ����߶ȣ����ص�λ��*/
	unsigned short	height;
	unsigned short	rev0;			/* �ѷ����������֣���ֵ������ */
	unsigned short	rev1;
	unsigned short	bitcount;		/* ͼ��λ��ȣ�ֻ����24��8��*/
} PIX_HEADER, *LPPIX_HEADER;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PIX_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PIX_API LPIRWP_INFO	CALLAGREEMENT pix_get_plugin_info(void);
extern PIX_API int			CALLAGREEMENT pix_init_plugin(void);
extern PIX_API int			CALLAGREEMENT pix_detach_plugin(void);
#endif	/* WIN32 */

extern PIX_API int	CALLAGREEMENT pix_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PIX_API int	CALLAGREEMENT pix_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PIX_API int	CALLAGREEMENT pix_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PIX_MODULE_INC__ */
