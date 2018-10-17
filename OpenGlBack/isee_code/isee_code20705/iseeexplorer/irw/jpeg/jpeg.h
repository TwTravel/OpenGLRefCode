/********************************************************************

	jpeg.h

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
	���ļ���;��	ISeeͼ���������JPEGͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�8λ�Ҷ�ͼ��24λ���ͼ��
					���湦�ܣ�8λ�Ҷ�ͼ��24λ���ͼ��
						    �����趨Ʒ��ϵ����
	  
	���ļ���д�ˣ�	orbit		Inte2000##263.net
					YZ			yzfree##sina.com
		
	���ļ��汾��	10715
	����޸��ڣ�	2001-7-15
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-7		�����°����
		2000-12		�Ľ��˽��ȿ��ƴ���
		2000-9		��һ�����԰淢��


	���ô���������
	---------------------------------------------------------------
	�����ʹ����IJG��jpeglib6b�����⣬��л���������Ĺ�����

	"this software is based in part on the work of the Independent 
	JPEG Group"
  
********************************************************************/



#ifndef __JPEG_MODULE_INC__
#define __JPEG_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("JPEG���")
#define MODULE_FUNC_PREFIX				("jpeg_")
#define MODULE_FILE_POSTFIX				("JPG")


#ifdef WIN32	/* Windows */

#	ifdef  JPEG_EXPORTS
#	define JPEG_API __declspec(dllexport)
#	else
#	define JPEG_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define JPEG_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* JPEGͼ���ʶ�� */
#define JPG_SOI_MARK            0xD8FF


/* ��������Ϣ�ṹ */
struct my_error_mgr
{
	struct jpeg_error_mgr		pub;	
	jmp_buf						setjmp_buffer;
};

typedef struct my_error_mgr		*my_error_ptr;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern JPEG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern JPEG_API LPIRWP_INFO CALLAGREEMENT jpeg_get_plugin_info(void);
extern JPEG_API int			CALLAGREEMENT jpeg_init_plugin(void);
extern JPEG_API int			CALLAGREEMENT jpeg_detach_plugin(void);
#endif	/* WIN32 */

extern JPEG_API int CALLAGREEMENT jpeg_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern JPEG_API int	CALLAGREEMENT jpeg_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern JPEG_API int	CALLAGREEMENT jpeg_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __JPEG_MODULE_INC__ */
