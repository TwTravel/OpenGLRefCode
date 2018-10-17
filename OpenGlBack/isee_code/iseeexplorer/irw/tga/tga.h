/********************************************************************

	tga.h

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
	���ļ���;��	ISeeͼ���������TGAͼ���дģ��ʵ���ļ�
	
					��ȡ���ܣ�δѹ����8��15��16��24��32λͼ�񣬼�
							����RLEѹ����8��15��16��24��32λͼ��
					���湦�ܣ�δѹ����8��15��16��24��32λͼ��
	  
	  
	���ļ���д�ˣ�	���¶�			xiaoyueer##263.net
					YZ				yzfree##sina.com
		
	���ļ��汾��	11225
	����޸��ڣ�	2001-12-25
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
		��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-12		�����°���루�ӿ���ͼ���ȡ�ٶȣ�������
					�˱���ͼ��Ĺ��ܣ�
			  
		2001-3		Ϊ���ģ���ͼ���ٶȶ��޸Ĵ���
		2000-8		��һ�����԰淢��
				
********************************************************************/



#ifndef __TGA_MODULE_INC__
#define __TGA_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("TGA���")
#define MODULE_FUNC_PREFIX				("tga_")
#define MODULE_FILE_POSTFIX				("TGA")


/**************TGA ͼ������ *************/
typedef enum{
	TGA_NULL		= 0,
	TGA_UNCPSPAL	= 1,
	TGA_UNCPSCOLOR	= 2,
	TGA_UNCPSGRAY	= 3,
	TGA_RLEPAL		= 9,
	TGA_RLECOLOR	= 10,
	TGA_RLEGRAY		= 11
}TGATYPE;

/**********TGA ͼ�����ݴ洢����(��bDescriptor��Ӧλ��) ***********/
typedef enum{
	TGA_HORZMIRROR = 0x10,
	TGA_VERTMIRROR = 0x20
}TGAMIRROR;


#ifdef WIN32	/* Windows */

#	ifdef  TGA_EXPORTS
#	define TGA_API __declspec(dllexport)
#	else
#	define TGA_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define TGA_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/**************TGA �ļ�ͷ���� ***************/
typedef struct {
	unsigned char  bID_Length;		/* ������Ϣ���� */
	unsigned char  bPalType;		/* ��ɫ����Ϣ */
	unsigned char  bImageType;		/* ͼ������(0,1,2,3,9,10,11) */
	unsigned short wPalFirstNdx;	/* ��ɫ���һ������ֵ */
	unsigned short wPalLength;		/* ��ɫ��������(�Ե�ɫ�嵥ԪΪ��λ) */
	unsigned char  bPalBits;		/* һ����ɫ�嵥λλ��(15,16,24,32) */
	unsigned short wLeft;			/* ͼ���������(��������) */
	unsigned short wBottom;			/* ͼ��׶�����(��������) */
	unsigned short wWidth;			/* ͼ���� */
	unsigned short wDepth;			/* ͼ�󳤶� */
	unsigned char  bBits;			/* һ������λ�� */
	unsigned char  bDescriptor;		/* ������������  */
}TGAHEADER, *LPTGAHEADER;

/**************TGA �ļ�β���� ***************/
typedef struct {
	unsigned long  eao;				/* ��չ��ƫ�� */
	unsigned long  ddo;				/* ��������ƫ�� */
	unsigned char  info[16];		/* TRUEVISION-XFILE �̱��ַ��� */
	unsigned char  period;			/* �ַ�"." */
	unsigned char  zero;			/* 0 */
} TGAFOOTER, *LPTGAFOOTER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern TGA_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern TGA_API LPIRWP_INFO	CALLAGREEMENT tga_get_plugin_info(void);
extern TGA_API int			CALLAGREEMENT tga_init_plugin(void);
extern TGA_API int			CALLAGREEMENT tga_detach_plugin(void);
#endif	/* WIN32 */

extern TGA_API int	CALLAGREEMENT tga_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern TGA_API int	CALLAGREEMENT tga_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern TGA_API int	CALLAGREEMENT tga_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __TGA_MODULE_INC__ */
