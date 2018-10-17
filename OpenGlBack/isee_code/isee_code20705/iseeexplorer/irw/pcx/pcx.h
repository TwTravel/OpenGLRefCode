/********************************************************************

	pcx.h

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
	���ļ���;��	ISeeͼ���������PCXͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�1��4��8��24λͼ��
					���湦�ܣ�1��4��8��24λͼ��
	  
	���ļ���д�ˣ�	freedvlp	feelfree##263.net���������д�ˣ�
					ChK			ChK##163.net��д�����д�ˣ�
					YZ			yzfree##sina.com���İ湤����
		
	���ļ��汾��	10818
	����޸��ڣ�	2001-8-18
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-8		�����°���롣����չ�˸�ģ��ı��湦�ܣ�
					��ǿ��ģ���ݴ��ԣ������ģ��Ķ�д�ٶȡ�
			  
		2001-4		�����ͼ��ı��湦�ܣ�24λ��
		2000-8		����˶�ȡͼ�񲿷ֵ�ȫ������
		2000-6		��һ�����԰淢��


********************************************************************/



#ifndef __PCX_MODULE_INC__
#define __PCX_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PCX���")
#define MODULE_FUNC_PREFIX				("pcx_")
#define MODULE_FILE_POSTFIX				("PCX")

/* �ļ��б��־ */
#define PCX_FLAG						0xa


#ifdef WIN32	/* Windows */

#	ifdef  PCX_EXPORTS
#	define PCX_API __declspec(dllexport)
#	else
#	define PCX_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PCX_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* PCX��ɫ����ṹ */
typedef struct _tag_pcx_palette_item
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} PCX_PALITEM, *LPPCX_PALITEM;


/* PCX��Ϣ�ṹ */
typedef struct _tag_pcx_header
{
	unsigned char  byManufacturer;
	unsigned char  byVersion;
	unsigned char  byEncoding;
	unsigned char  byBits;
	unsigned short wLeft;
	unsigned short wTop;
	unsigned short wRight;
	unsigned short wBottom;
	unsigned short wXResolution;
	unsigned short wYResolution;
	PCX_PALITEM    ptPalette[16];
	unsigned char  byReserved;
	unsigned char  byPlanes;
	unsigned short wLineBytes;
	unsigned short wPaletteType;
	unsigned short wScrWidth;
	unsigned short wScrDepth;
	unsigned char  byFiller[54];
}  PCX_HEADER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PCX_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PCX_API LPIRWP_INFO	CALLAGREEMENT pcx_get_plugin_info(void);
extern PCX_API int			CALLAGREEMENT pcx_init_plugin(void);
extern PCX_API int			CALLAGREEMENT pcx_detach_plugin(void);
#endif	/* WIN32 */

extern PCX_API int	CALLAGREEMENT pcx_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PCX_API int	CALLAGREEMENT pcx_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PCX_API int	CALLAGREEMENT pcx_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PCX_MODULE_INC__ */
