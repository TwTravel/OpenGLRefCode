/********************************************************************

	rle.h

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
	���ļ���;��	ISeeͼ���������Utah RLEͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ�ȡ8λ��24λ��32λRLEͼ�����
								��������ɫ��ͨ����һ��ALPHAͨ����
					���湦�ܣ��ɽ�8λ��24λͼ�󱣴�ΪRLEͼ�󣨲�
								ѹ����ʽ��
	  
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
	
	���ļ��汾��	20522
	����޸��ڣ�	2002-5-22
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2002-5		��һ�������棨�°棩

********************************************************************/



#ifndef __RLE_MODULE_INC__
#define __RLE_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("RLE���")
#define MODULE_FUNC_PREFIX				("rle_")
#define MODULE_FILE_POSTFIX				("rle")

/* �ļ��б��־ */
#define RLE_MAGIC						((unsigned short)0xcc52)


#ifdef WIN32	/* Windows */

#	ifdef  RLE_EXPORTS
#	define RLE_API __declspec(dllexport)
#	else
#	define RLE_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define RLE_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* ���������б�λ */
#define     LONGOPCODE			0x40

/* RLE�������붨�� */
#define	    RLE_SKIPLINE_OP		1
#define	    RLE_SETCOLOR_OP		2
#define	    RLE_SKIPPIXELS_OP	3
#define	    RLE_BYTEDATA_OP		5
#define	    RLE_RUNDATA_OP		6
#define	    RLE_EOF_OP			7

#define     RLE_CLEARFIRST_F	0x1		/* ������õĻ�����ѹǰ�������������ʹ�ñ���ɫ�� */
#define	    RLE_NO_BACKGROUND_F	0x2		/* ������õĻ���������δ�ṩ����ɫ��Ϣ */
#define	    RLE_ALPHA_F			0x4		/* ������õĻ����������д���ALPHAͨ����ͨ����-1�� */
#define	    RLE_COMMENT_F		0x8		/* ������õĻ����������д���ע����Ϣ */

#define		RLE_OPCODE(s)		(s[0] & ~(unsigned char)LONGOPCODE)
#define		RLE_LONGP(s)		(s[0] & (unsigned char)LONGOPCODE)
#define		RLE_DATUM(s)		(s[1] & (unsigned char)0xff)


/* RLEͷ�ṹ���� */
typedef struct
{
	unsigned short	magic;				/* RLE�б��־��0xcc52�� */
	unsigned short	xpos;				/* ͼ���x��yԭ�����꣨���½ǣ���һ���ޣ�*/
	unsigned short	ypos;
	unsigned short	width;				/* ͼ��Ŀ�Ⱥ͸߶ȣ����ص�λ��*/
	unsigned short	height;
	unsigned char	flags;				/* ��־λ */
	unsigned char	channels;			/* ��ɫͨ������������ALPHAͨ�� */
	unsigned char	chan_bits;			/* ÿ����ɫͨ����λ��ȣ���ǰ��Чֵֻ��8��*/
	unsigned char	pal_chan_count;		/* ��ɫ�����ͨ���� */
	unsigned char	pal_chan_bits;		/* ��ɫ��ͨ����λ��� */
} RLE_HEADER, *LPRLE_HEADER;




#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern RLE_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern RLE_API LPIRWP_INFO	CALLAGREEMENT rle_get_plugin_info(void);
extern RLE_API int			CALLAGREEMENT rle_init_plugin(void);
extern RLE_API int			CALLAGREEMENT rle_detach_plugin(void);
#endif	/* WIN32 */

extern RLE_API int	CALLAGREEMENT rle_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern RLE_API int	CALLAGREEMENT rle_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern RLE_API int	CALLAGREEMENT rle_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __RLE_MODULE_INC__ */
