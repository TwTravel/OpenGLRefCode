/********************************************************************

	pcd.h

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
	���ļ���;��	ISeeͼ���������PCDͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ�ȡ24λPCDͼ��ǰ������
							  
					���湦�ܣ���֧��
							   
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20604
	����޸��ڣ�	2002-6-4
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
		��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-6		��һ�������棨�°棩

********************************************************************/



#ifndef __PCD_MODULE_INC__
#define __PCD_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PCD���")
#define MODULE_FUNC_PREFIX				("pcd_")
#define MODULE_FILE_POSTFIX				("pcd")


/* �ļ��б��־��PCD����ϸ�ĸ�ʽ�ĵ����´�����û�з�����Щ
 * �ĵ�������ֻ��ͨ���ж�ͼ����С���������ȷ�����Ƿ���һ��
 * PCDͼ�󡣵�Ȼ�����ַ�ʽ�Ǻܲ���ȷ�ģ�
 */
#define PCD_MIN_SIZE					(1048576UL)
/* PCDͼ������Ϣλ�� */
#define PCD_VERTICAL_LOC				72
#define PCD_VERTICAL_MASK				(0x3f)
/* PCD��ͼ��ߴ磨PCDͼ���ǹ̶��ߴ�ģ�*/
#define PCD_1_W							192
#define PCD_1_H							128
#define PCD_2_W							384
#define PCD_2_H							256
#define PCD_3_W							768
#define PCD_3_H							512
#define PCD_4_W							1536
#define PCD_4_H							1024
#define PCD_5_W							3072
#define PCD_5_H							2048
/* PCDȱʡͼ��λ��� */
#define PCD_DEF_BITCOUNT				24
/* PCD��ͼ��������ͼ���е�ƫ�ƣ�������ͼ���ƫ�ƿ´﹫˾û�й����� */
#define PCD_1_OFF						0x2000
#define PCD_2_OFF						0xb800
#define PCD_3_OFF						0x30000


#ifdef WIN32	/* Windows */

#	ifdef  PCD_EXPORTS
#	define PCD_API __declspec(dllexport)
#	else
#	define PCD_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PCD_API
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
extern PCD_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PCD_API LPIRWP_INFO	CALLAGREEMENT pcd_get_plugin_info(void);
extern PCD_API int			CALLAGREEMENT pcd_init_plugin(void);
extern PCD_API int			CALLAGREEMENT pcd_detach_plugin(void);
#endif	/* WIN32 */

extern PCD_API int	CALLAGREEMENT pcd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PCD_API int	CALLAGREEMENT pcd_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PCD_API int	CALLAGREEMENT pcd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PCD_MODULE_INC__ */
