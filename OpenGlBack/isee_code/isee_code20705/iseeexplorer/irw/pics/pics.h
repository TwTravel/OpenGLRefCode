/********************************************************************

	pics.h

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
	���ļ���;��	ISeeͼ���������SoftImage PICͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ�ȡ24λ��32λ����һ��ALPHAͨ������
								ѹ����δѹ�� SoftImage PIC ͼ��
	  
					���湦�ܣ��ɽ�24λͼ���Բ�ѹ���ķ�ʽ����Ϊ3ͨ��
								SoftImage PIC ͼ��
								
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20505
	����޸��ڣ�	2002-5-5
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-5		������һ���汾���°棩
			

********************************************************************/



#ifndef __PICS_MODULE_INC__
#define __PICS_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PICS���")
#define MODULE_FUNC_PREFIX				("pics_")
#define MODULE_FILE_POSTFIX				("pic")


/* �ļ��б��־ */
#define PICS_MAGIC						0x5380F634
#define PICS_PICT_STR					0x54434950		/* 'PICT' �ĸ��ַ� */


#ifdef WIN32	/* Windows */

#	ifdef  PICS_EXPORTS
#	define PICS_API __declspec(dllexport)
#	else
#	define PICS_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PICS_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* �ļ�ͷ�ṹ���� */
typedef struct
{
	unsigned int	magic;           /* PICͼ���ǩ             */
	float			version;         /* ʹ�õĸ�ʽ�汾��        */
	char			comment[80];     /* ͼ�������ִ�            */
	char			id[4];           /* "PICT"�ĸ��ַ�          */
	unsigned short	width;           /* ͼ���ȣ����ص�λ      */
	unsigned short	height;          /* ͼ��߶ȣ����ص�λ      */
	float			ratio;           /* ͼ��Ŀ�߱�            */
	unsigned short	fields;          /* ͼ���־��              */
	unsigned short	padding;         /* ����                    */
} PICS_HEADER, *LPPICS_HEADER;


/* ͨ����Ϣ�ṹ */
typedef struct
{
	unsigned char	chained;
	unsigned char	size;			/* ÿͨ��λ�� */
	unsigned char	type;			/* ѹ����ʽ */
	unsigned char	channel;		/* ������Щͨ��(R��G��B��A...) */
} PICS_CHANNEL_INFO, *LPPICS_CHANNEL_INFO;



/* �������� */
#define PIC_UNSIGNED_INTEGER	0x00
#define PIC_SIGNED_INTEGER		0x10	/* δʵ�� */
#define PIC_SIGNED_FLOAT		0x20	/* δʵ�� */


/* ѹ������ */
#define PIC_UNCOMPRESSED		0x00
#define PIC_PURE_RUN_LENGTH		0x01
#define PIC_MIXED_RUN_LENGTH	0x02

/* ͨ������ */
#define PIC_RED_CHANNEL			0x80
#define PIC_GREEN_CHANNEL		0x40
#define PIC_BLUE_CHANNEL		0x20
#define PIC_ALPHA_CHANNEL		0x10
#define PIC_SHADOW_CHANNEL		0x08	/* δʵ�� */
#define PIC_DEPTH_CHANNEL		0x04	/* δʵ�� */
#define PIC_AUXILIARY_1_CHANNEL	0x02	/* δʵ�� */
#define PIC_AUXILIARY_2_CHANNEL	0x01	/* δʵ�� */



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PICS_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PICS_API LPIRWP_INFO	CALLAGREEMENT pics_get_plugin_info(void);
extern PICS_API int			CALLAGREEMENT pics_init_plugin(void);
extern PICS_API int			CALLAGREEMENT pics_detach_plugin(void);
#endif	/* WIN32 */

extern PICS_API int	CALLAGREEMENT pics_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PICS_API int	CALLAGREEMENT pics_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PICS_API int	CALLAGREEMENT pics_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PICS_MODULE_INC__ */
