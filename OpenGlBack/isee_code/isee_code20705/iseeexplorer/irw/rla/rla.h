/********************************************************************

	rla.h

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
	���ļ���;��	ISeeͼ���������Alias/Wavefront RLAͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ��ɶ�ȡ3ͨ��24λѹ��rgbɫϵ��ͼ�󣬲�֧��matte��
								auxͨ��
							  
					���湦�ܣ��ɽ�24λͼ�󱣴�Ϊ3ͨ��RLE8ѹ����rgbɫϵͼ��
							   
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
					
	���ļ��汾��	20509
	����޸��ڣ�	2002-5-9
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-5		��һ�������棨�°棩


********************************************************************/



#ifndef __RLA_MODULE_INC__
#define __RLA_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("RLA���")
#define MODULE_FUNC_PREFIX				("rla_")
#define MODULE_FILE_POSTFIX				("rla")


/* �ļ��б��־ */
#define RLA_REVISION					0xfffe


#ifdef WIN32	/* Windows */

#	ifdef  RLA_EXPORTS
#	define RLA_API __declspec(dllexport)
#	else
#	define RLA_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define RLA_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* ��������ṹ */
typedef struct
{
	short left;
	short right;
	short bottom;
	short top;
} RLA_WINDOW_S, *LPRLA_WINDOW_S;


/* RLA��ͷ�ṹ���� */
typedef struct
{
	RLA_WINDOW_S window;			/* ������������ */
	RLA_WINDOW_S active_window;		/* ͼ�󴰿����� */
	short frame;					/* ͼ��֡�� */
	short storage_type;				/* ͼ������ѹ������ */
	short num_chan;					/* ͼ��ͨ������������matte��auxͨ������һ��Ϊ3 */
	short num_matte;				/* matteͨ������һ��Ϊ1 */
	short num_aux;					/* ����ͨ���� */
	short revision;					/* ��ʽ�޶��ţ���ǰΪ0xfffe */
	char  gamma[16];				/* gammaֵ��һ��Ϊ2.2 */
	char  red_pri[24];
	char  green_pri[24];
	char  blue_pri[24];
	char  white_pt[24];
	long  job_num;
	char  name[128];
	char  desc[128];				/* ͼ���������� */
	char  program[64];				/* ����ͼ���������� */
	char  machine[32];				/* ����ͼ��Ļ������� */
	char  user[32];
	char  date[20];					/* ����ͼ������ڡ�ʱ�� */
	char  aspect[24];
	char  aspect_ratio[8];
	char  chan[32];					/* ����ɫϵ��һ��Ϊ"rgb" */
	short field;
	char  time[12];
	char  filter[32];
	short chan_bits;				/* ÿͨ����λ��� */
	short matte_type;
	short matte_bits;				/* matteͨ��λ��� */
	short aux_type;
	short aux_bits;					/* ����ͨ��λ��� */
	char  aux[32];
	char  space[36];
	long  next;						/* ��һ����ͼ�������ƫ�� */
} RLA_HEADER, *LPRLA_HEADER;



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern RLA_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern RLA_API LPIRWP_INFO	CALLAGREEMENT rla_get_plugin_info(void);
extern RLA_API int			CALLAGREEMENT rla_init_plugin(void);
extern RLA_API int			CALLAGREEMENT rla_detach_plugin(void);
#endif	/* WIN32 */

extern RLA_API int	CALLAGREEMENT rla_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern RLA_API int	CALLAGREEMENT rla_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern RLA_API int	CALLAGREEMENT rla_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __RLA_MODULE_INC__ */
