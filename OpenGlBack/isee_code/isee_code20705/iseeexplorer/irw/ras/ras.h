/********************************************************************

	ras.h

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
	���ļ���;��	ISeeͼ���������Sun RASͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ�ȡ1��8��24��32λѹ����δѹ��RASͼ��
							  
					���湦�ܣ��ṩ24λ��ѹ����ʽ�ı��湦��
	  
	���ļ���д�ˣ�	
					netmap		beahuang##hotmail.com
					YZ			yzfree##yeah.net
	
	���ļ��汾��	20610
	����޸��ڣ�	2002-6-10
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	ע�����ļ��Ĳ��ֺ����ο���FreeImageͼ����Դ���룬�ڴ���
			Floris van den Berg (flvdberg@wxs.nl)
			Herv Drolon (drolon@iut.univ-lehavre.fr)
		��ʾ��л������������˽�FreeImageͼ������������
		�����Ĺٷ���վ��
			http://www.6ixsoft.com/
	----------------------------------------------------------------
	������ʷ��
			
		2002-6		��һ���汾�������°棩

********************************************************************/



#ifndef __RAS_MODULE_INC__
#define __RAS_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("RAS���")
#define MODULE_FUNC_PREFIX				("ras_")
#define MODULE_FILE_POSTFIX				("ras")

/* �ļ��б��־ */
#define RAS_MAGIC						0x59a66a95


#ifdef WIN32	/* Windows */

#	ifdef  RAS_EXPORTS
#	define RAS_API __declspec(dllexport)
#	else
#	define RAS_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define RAS_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* �ļ��ṹ���� */
#define RT_OLD					0			/* δѹ����ʽ */
#define RT_STANDARD				1			/* δѹ����ʽ */
#define RT_BYTE_ENCODED			2			/* RLEѹ����ʽ */
#define RT_FORMAT_RGB			3			/* RGB��ʽ */
#define RT_FORMAT_TIFF			4
#define RT_FORMAT_IFF			5

#define RT_EXPERIMENTAL			0xFFFF

#define RMT_NONE				0			/* �޵�ɫ������ */
#define RMT_EQUAL_RGB			1
#define RMT_RAW					2

#define RESC					0x80		/* RLEѹ����ʶ�� */
#define RAS_ROUNDROW			16			/* ɨ���ж���λ�� */


/* RASͼ��ͷ�ṹ */
typedef struct _SUNHeaderinfo
{
    unsigned long	ras_magic;				/* �б��־ */
	unsigned long	ras_width;				/* ��� */
	unsigned long	ras_height;				/* �߶� */
	unsigned long	ras_depth;				/* λ��� */
	unsigned long	ras_length;				/* ͼ�����ݳ��ȣ�������ͷ�ṹ�����ܵĵ�ɫ�壩*/
	unsigned long	ras_type;				/* ��ʽ���� */
	unsigned long	ras_maptype;			/* ��ɫ�����ݴ�ŷ�ʽ */
	unsigned long	ras_maplength;			/* ��ɫ�峤�ȣ��ֽڼƣ�*/
} SUNHeaderinfo, *LPSUNHeaderinfo;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"						/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern RAS_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern RAS_API LPIRWP_INFO	CALLAGREEMENT ras_get_plugin_info(void);
extern RAS_API int			CALLAGREEMENT ras_init_plugin(void);
extern RAS_API int			CALLAGREEMENT ras_detach_plugin(void);
#endif	/* WIN32 */

extern RAS_API int	CALLAGREEMENT ras_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern RAS_API int	CALLAGREEMENT ras_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern RAS_API int	CALLAGREEMENT ras_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __RAS_MODULE_INC__ */
