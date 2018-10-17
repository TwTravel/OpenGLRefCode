/********************************************************************

	bmp.h

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
			http://iseeexplorer.cosoft.org.cn

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISeeͼ���������BMPͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�Windows ��� ��1��32λλͼ��RLEλͼ
							  OS/2 ��� �� 1��4��8��24λλͼ
					���湦�ܣ�Windows ��� ��1��4��8��16(555)��24��
								32(888)λλͼ
	  
	���ļ���д�ˣ�	YZ			yzfree##sina.com
		
	���ļ��汾��	10709
	����޸��ڣ�	2001-7-9
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
		2001-7		����RLE�����е�һ������
					1��4λͼ���������ISeeλ����ʽ
					����Ŀ��ͼ�񻺳��������ֽ�
					��ǿ���ݴ���
			  
		2001-6		�����°����
		2001-5		���¶���ģ��Ľӿڼ��������
		2000-10		������һ����Ϊ�ش��BUG
		2000-7		��һ�����԰淢��


********************************************************************/



#ifndef __BMP_MODULE_INC__
#define __BMP_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("BMP���")
#define MODULE_FUNC_PREFIX				("bmp_")
#define MODULE_FILE_POSTFIX				("BMP")

/* �ļ��б��־ (windows - 'BM', OS/2 - 'BA')*/
#define DIB_HEADER_MARKER				((unsigned short)('M'<<8)|'B')
#define DIB_HEADER_MARKER_OS2			((unsigned short)('A'<<8)|'B')


#ifdef WIN32	/* Windows */

#	ifdef  BMP_EXPORTS
#	define BMP_API __declspec(dllexport)
#	else
#	define BMP_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define BMP_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* λͼ��ɫ����ṹ ��Windows ���*/
typedef struct _tag_rgb_quad{
    unsigned char	rgbBlue; 
    unsigned char	rgbGreen; 
    unsigned char	rgbRed; 
    unsigned char	rgbReserved; 
} RGB_QUAD, *LPRGB_QUAD; 

/* λͼ��ɫ����ṹ ��OS/2 ���*/
typedef struct _tag_rgb_triple{
    unsigned char	rgbtBlue; 
    unsigned char	rgbtGreen; 
    unsigned char	rgbtRed; 
} RGB_TRIPLE, *LPRGB_TRIPLE; 



/* λͼ��Ϣͷ�ṹ ��Windows ���*/
typedef struct _tag_bmp_info_header{
	unsigned long	biSize;
	long			biWidth;
	long			biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned long	biCompression;
	unsigned long	biSizeImage;
	long			biXPelsPerMeter;
	long			biYPelsPerMeter;
	unsigned long	biClrUsed;
	unsigned long	biClrImportant;
} BMP_INFO_HEADER, *LPBMP_INFO_HEADER;

/* λͼ��Ϣͷ�ṹ ��OS/2 ���*/
typedef struct _tag_bmp_core_header{
	unsigned long	bcSize; 
	unsigned short	bcWidth; 
	unsigned short	bcHeight; 
	unsigned short	bcPlanes; 
	unsigned short	bcBitCount; 
} BMP_CORE_HEADER, *LPBMP_CORE_HEADER; 



/* λͼ��Ϣ�ṹ ��Windows ��� */
typedef struct _tag_bmp_info{
	BMP_INFO_HEADER	bmiHeader;
	RGB_QUAD		bmiColors[1];
} BMP_INFO, *LPBMP_INFO;

/* λͼ��Ϣ�ṹ ��OS/2 ��� */
typedef struct _tag_bmp_core{
	BMP_CORE_HEADER bmciHeader; 
	RGB_TRIPLE      bmciColors[1]; 
} BMP_CORE, *LPBMP_CORE; 



/* λͼ�ļ�ͷ�ṹ ��Windows ��OS/2 ���ã�*/
typedef struct _tag_bmp_file_header{
	unsigned short	bfType;
	unsigned long	bfSize;
	unsigned short	bfReserved1;
	unsigned short	bfReserved2;
	unsigned long	bfOffBits;
} BMP_FILE_HEADER, *LPBMP_FILE_HEADER;


#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern BMP_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern BMP_API LPIRWP_INFO	CALLAGREEMENT bmp_get_plugin_info(void);
extern BMP_API int			CALLAGREEMENT bmp_init_plugin(void);
extern BMP_API int			CALLAGREEMENT bmp_detach_plugin(void);
#endif	/* WIN32 */

extern BMP_API int	CALLAGREEMENT bmp_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern BMP_API int	CALLAGREEMENT bmp_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern BMP_API int	CALLAGREEMENT bmp_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __BMP_MODULE_INC__ */
