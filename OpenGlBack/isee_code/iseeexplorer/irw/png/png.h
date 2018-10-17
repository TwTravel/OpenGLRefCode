/********************************************************************

	png.h

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
	���ļ���;��	ISeeͼ���������PNGͼ���дģ�鶨���ļ�

					��ȡ���ܣ�1��2��4��8λ�Ҷ�ͼ��8λ��alphaͨ����
								�Ҷ�ͼ��24λRGBͼ��24λ��alphaͨ��
								��RGBͼ��1��2��4��8λ��ɫ��ͼ��
								ע��16λ�Ҷ�ͼ��48λRGBͼ���ڱ�ģ��
								�ڲ���ת����8λ��24λͼ��
								8λ��alphaͨ���ĻҶ�ͼ����ת��Ϊ
								32λRGBAͼ��
								����ת����Ҫ��Ϊ��ʹ������ݷ�
								��ISeeλ����ʽ��
					���湦�ܣ�1��2��4��8λ��ɫ��ͼ��24��32λRGBͼ��
								ע��32λͼЯ��8λalphaͨ����
									���ܱ���Ҷ�ͼ��
								
	���ļ���д�ˣ�	Aurora		aurorazzf##sina.com
					YZ			yzfree##sina.com ���İ湤����
		
	���ļ��汾��	10809
	����޸��ڣ�	2001-8-9
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-8		�����°����
		2001-5		���¶���ģ��Ľӿڼ��������
		2001-3		������һЩBUG
		2000-7		������һЩBUG
		2000-6		��һ�����԰淢��


********************************************************************/



#ifndef __PNG_MODULE_INC__
#define __PNG_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PNG���")
#define MODULE_FUNC_PREFIX				("png_")
#define MODULE_FILE_POSTFIX				("PNG")

/* �ļ��б��־ (windows - 'BM', OS/2 - 'BA')*/


#ifdef WIN32	/* Windows */

#	ifdef  PNG_EXPORTS
#	define PNG_API __declspec(dllexport)
#	else
#	define PNG_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PNG_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* PNGͼ����Ϣ�ṹ */
typedef struct _tagPNGINFOHEADER
{
	unsigned long Width;
	unsigned long Height;
	int color_type; 
	int bit_depth;
	int interlace_type;
	int filter_type;
	int compression_type;
	int number_passes;
}PNGINFOHEADER, *LPPNGINFOHEADER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PNG_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PNG_API LPIRWP_INFO	CALLAGREEMENT png_get_plugin_info(void);
extern PNG_API int			CALLAGREEMENT png_init_plugin(void);
extern PNG_API int			CALLAGREEMENT png_detach_plugin(void);
#endif	/* WIN32 */

extern PNG_API int	CALLAGREEMENT png_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PNG_API int	CALLAGREEMENT png_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PNG_API int	CALLAGREEMENT png_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PNG_MODULE_INC__ */
