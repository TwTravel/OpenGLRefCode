/********************************************************************

	tiff.h

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
	���ļ���;��	ISeeͼ���������TIFFͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ��� ��ɫ�塢�Ҷȡ�RGB��YCbCr ��ʽ�洢��
								TIFFͼ�󣨰�����ҳͼ�󣩣�����֧��λ���
								����32λ��ͼ��
	  
					���湦�ܣ��ɽ�1��4��8��24λ�ĵ�ҳ���ҳͼ�󱣴�Ϊ��
								Ӧ��TIFFͼ��
								
	  
	���ļ���д�ˣ�	YZ				yzfree##yeah.net
					xuhappy			xuxuhappy##sina.com
					zjucypher		xubo.cy##263.net
		
	���ļ��汾��	20511
	����޸��ڣ�	2002-5-11
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------

	���ô����д�˼���Ȩ��Ϣ��
	----------------------------------------------------------------
	�������	TIFF�������(LibTiff)		Ŀ¼��libtiff
	
	Copyright (c) 1988-1997 Sam Leffler
	Copyright (c) 1991-1997 Silicon Graphics, Inc.

	Permission to use, copy, modify, distribute, and sell this software and 
	its documentation for any purpose is hereby granted without fee, provided
	that (i) the above copyright notices and this permission notice appear in
	all copies of the software and related documentation, and (ii) the names of
	Sam Leffler and Silicon Graphics may not be used in any advertising or
	publicity relating to the software without the specific, prior written
	permission of Sam Leffler and Silicon Graphics.

	THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
	EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
	WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  

	IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
	ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
	OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
	WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
	LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
	OF THIS SOFTWARE.
	----------------------------------------------------------------

	������ʷ��

		2002-5		������ȡJPEGѹ����ʽ��TIFFͼ��ʱR��B�����ߵ������⡣
					����������ȷ��ȡPhotoShop������TIFFͼ������⡣
					leye���֣�YZ������
		
		2002-4		�����°���롣֧��ISeeIO������JPEG��ʽ�洢��TIFF
					ͼ�󡣱��湦��֧��1��4��8��24λ��ȡ�֧�ֶ�ҳTIFF��

		2001-2		�����˱��湦��
		2001-1		����RGBAת�������д��ڵ����⣬���Ľ��˲������ƴ���
		2001-1		���������libtiff��
		2000-7		��һ��������


********************************************************************/



#ifndef __TIFF_MODULE_INC__
#define __TIFF_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("TIFF���")
#define MODULE_FUNC_PREFIX				("tiff_")
#define MODULE_FILE_POSTFIX				("tif")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  TIFF_EXPORTS
#	define TIFF_API __declspec(dllexport)
#	else
#	define TIFF_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define TIFF_API
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
extern TIFF_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern TIFF_API LPIRWP_INFO	CALLAGREEMENT tiff_get_plugin_info(void);
extern TIFF_API int			CALLAGREEMENT tiff_init_plugin(void);
extern TIFF_API int			CALLAGREEMENT tiff_detach_plugin(void);
#endif	/* WIN32 */

extern TIFF_API int	CALLAGREEMENT tiff_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern TIFF_API int	CALLAGREEMENT tiff_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern TIFF_API int	CALLAGREEMENT tiff_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __TIFF_MODULE_INC__ */
