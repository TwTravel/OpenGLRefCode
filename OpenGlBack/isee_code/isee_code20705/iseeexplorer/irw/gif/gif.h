/********************************************************************

	gif.h

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
	���ļ���;��	ISeeͼ���������GIFͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�1~8λ����̬��̬GIFͼ��
					���湦�ܣ���GIF-LZWר���ڵ�2003���ʧЧ������
							  �ݲ����ṩ���湦��
	  
	���ļ���д�ˣ�	YZ			yzfree##sina.com
		
	���ļ��汾��	11227
	����޸��ڣ�	2001-12-27
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		  2001-12		������GIF-LZW��ѹ�㷨�д��ڵ�һ�����󣬲�
						��ǿ��_get_imginfo()�������ݴ��ԡ�(B01)
		  2001-7		�����°����
		  2000-9		��һ�����԰淢��
		  2000-8		Sam��д��GIF��д��


	GIFͼ���ļ���ʽ��Ȩ������
	---------------------------------------------------------------
	The Graphics Interchange Format(c) is the Copyright property of
	CompuServe Incorporated. GIF(sm) is a Service Mark property of
	CompuServe Incorporated.
	---------------------------------------------------------------

********************************************************************/



#ifndef __GIF_MODULE_INC__
#define __GIF_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("GIF���")
#define MODULE_FUNC_PREFIX				("gif_")
#define MODULE_FILE_POSTFIX				("GIF")

/* �ļ��б��־ */
#define GIF_MARKER_87a					("GIF87a")
#define GIF_MARKER_89a					("GIF89a")


#ifdef WIN32	/* Windows */

#	ifdef  GIF_EXPORTS
#	define GIF_API __declspec(dllexport)
#	else
#	define GIF_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define GIF_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* GIF�汾��ʶ������ */
#define GIFVERSIZE					6

struct _tagGIFHEADER;
typedef struct _tagGIFHEADER		GIFHEADER;
typedef struct _tagGIFHEADER		*LPGIFHEADER;

struct _tagGIFINFO;
typedef struct _tagGIFINFO			GIFINFO;
typedef struct _tagGIFINFO			*LPGIFINFO;

struct _tagGIFRGB;
typedef struct _tagGIFRGB			GIFRGB;
typedef struct _tagGIFRGB			*LPGIFRGB;

struct _tagISEERGB;
typedef struct _tagISEERGB			ISEERGB;
typedef struct _tagISEERGB			*LPISEERGB;

struct _tagIMAGEDATAINFO;
typedef struct _tagIMAGEDATAINFO	IMAGEDATAINFO;
typedef struct _tagIMAGEDATAINFO	*LPIMAGEDATAINFO;

struct _tagGIFINSIDEINFO;
typedef struct _tagGIFINSIDEINFO	GIFINSIDEINFO;
typedef struct _tagGIFINSIDEINFO	*LPGIFINSIDEINFO;

struct _tagLZWTABLE;
typedef struct _tagLZWTABLE			LZWTABLE;
typedef struct _tagLZWTABLE			*LPLZWTABLE;

union _tagCODEWORD;
typedef union _tagCODEWORD			CODEWORD;
typedef union _tagCODEWORD			*LPCODEWORD;

struct _tagGRAPHCTRL;
typedef struct _tagGRAPHCTRL		GRAPHCTRL;
typedef struct _tagGRAPHCTRL		*LPGRAPHCTRL;

struct _tagNOTEHCTRL;
typedef struct _tagNOTEHCTRL		NOTEHCTRL;
typedef struct _tagNOTEHCTRL		*LPNOTEHCTRL;

struct _tagTEXTCTRL;
typedef struct _tagTEXTCTRL			TEXTCTRL;
typedef struct _tagTEXTCTRL			*LPTEXTCTRL;

struct _tagAPPCTRL;
typedef struct _tagAPPCTRL			APPCTRL;
typedef struct _tagAPPCTRL			*LPAPPCTRL;


/*@@@@@@@@@@@@@@@GIF87a �� GIF89a ���õĽṹ@@@@@@@@@@@@@@@@@@*/

struct _tagGIFINFO
{
	unsigned short FileType;		/* 89a or 87a */
	unsigned short ColorNum;		/* ��ɫ�� */
	unsigned short BitCount;		/* ͼ���λ��� */
	unsigned short Width;
	unsigned short Height;
	unsigned short FrameCount;		/* ���м���ͼ */
	unsigned char  bkindex;			/* ����ɫ���� */
	unsigned char  InitPixelBits;	/* ѹ��������ʼ����Bitsλ�� */
};


/* GIF��ɫ��Ԫ�ؽṹ */
struct _tagGIFRGB
{
	unsigned char bRed;
	unsigned char bGreen;
	unsigned char bBlue;
};

/* ISee��ɫ��Ԫ�ؽṹ */
struct _tagISEERGB
{
	unsigned char bBlue;
	unsigned char bGreen;
	unsigned char bRed;
	unsigned char rev;
};

/* GIFͼ��ͷ�ṹ */
struct _tagGIFHEADER
{
	unsigned char	gif_type[GIFVERSIZE];	/* ͼ��汾(87a or 89a) */
	unsigned short	scr_width;				/* �߼������ */
	unsigned short	scr_height;				/* �߼����߶� */
	unsigned char	global_flag;			/* ȫ�ֱ�� */
	unsigned char	bg_color;				/* ����ɫ���� */
	unsigned char	aspect_radio;			/* 89a��ĳ���� */
};


/* ͼ�������� */
struct _tagIMAGEDATAINFO
{
	unsigned char	label;					/* ��ǩֵ��0x2c */
	unsigned short	left; 					/* ��ͼ�����Ͻ����߼���Ļ�е�λ�� */
	unsigned short	top;
	unsigned short	width;
	unsigned short	height;
	unsigned char	local_flag;				/* ����λ�� */
};

#define	GIF_MAXCOLOR	256

/* GIF�ۺ���Ϣ�ṹ */
struct _tagGIFINSIDEINFO
{
	int				type;					/* GIF�汾��87a = 2 89a = 1 */

	unsigned long	scr_width;				/* �߼�����Ϣ */
	unsigned long	scr_height;
	unsigned long	scr_bitcount;
	GIFRGB			p_pal[GIF_MAXCOLOR];	/* ȫ�ֵ�ɫ������ */
	
	unsigned long	first_img_width;		/* �׷�ͼ����Ϣ */
	unsigned long	first_img_height;
	unsigned long	first_img_bitcount;
	GIFRGB			p_first_pal[GIF_MAXCOLOR];	
	
	int				img_count;				/* ���м���ͼ */
	int				bk_index;				/* ����ɫ���� */
};


#define LZWTABLESIZE	4096	/* GIF-LZW ����ߴ� */

/*	GIF-LZW �ֵ䵥Ԫ�ṹ */
struct _tagLZWTABLE	 
{
	short	prefix;			/* ǰ׺����������ǰ׺��ʱΪ-1�� */
	short	code;			/* β�� */
	int		used;			/* �Ƿ�ռ�ñ�־��0��δռ�ã�1����ռ�� */
};


/* GIF-LZW���뵥Ԫ */
union _tagCODEWORD
{
	unsigned char bcode[sizeof(unsigned long)];
	unsigned long lcode;
};


/*--------------------------------------------------------
  ע�����½ṹֻ������GIF89a��ʽ��ͼ���ļ� 
--------------------------------------------------------*/

/* ͼ�ο�����չ�� */
struct _tagGRAPHCTRL
{
	unsigned char	extintr;			/* 0x21	*/
	unsigned char	label;				/* 0xF9 */
	unsigned char	block_size;			/* 0x04 */
	unsigned char	field;
	unsigned short	delay_time;			/* �ӳ�ʱ�� */
	unsigned char	tran_color_index;	/* ͸��ɫ���� */
	unsigned char	block_terminator;	/* 0x00 */
};

/* ע����չ�� */
struct _tagNOTEHCTRL
{
	unsigned char extintr;				/* 0x21	*/
	unsigned char label;				/* 0xfe */
};

/* �ı���չ�� */
struct _tagTEXTCTRL
{
	unsigned char	extintr;			/* 0x21 */
	unsigned char	label;				/* 0x01 */
	unsigned char	block_size;			/* 0x0c */
	unsigned short	left;
	unsigned short	top;
	unsigned short	width;
	unsigned short	height; 
	unsigned char	fore_color_index;	/* ǰ��ɫ���� */
	unsigned char	bk_color_index;		/* ����ɫ���� */
};

/* Ӧ�ó�����չ�� */
struct _tagAPPCTRL
{
	unsigned char extintr;				/* 0x21	*/
	unsigned char label;				/* 0xff	*/
	unsigned char block_size;			/* 0xb	*/
	unsigned char identifier[8];
	unsigned char authentication[3];
};



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern GIF_API LPIRWP_INFO  CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern GIF_API LPIRWP_INFO  CALLAGREEMENT gif_get_plugin_info(void);
extern GIF_API int			CALLAGREEMENT gif_init_plugin(void);
extern GIF_API int			CALLAGREEMENT gif_detach_plugin(void);
#endif	/* WIN32 */

extern GIF_API int	CALLAGREEMENT gif_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern GIF_API int	CALLAGREEMENT gif_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern GIF_API int	CALLAGREEMENT gif_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __GIF_MODULE_INC__ */
