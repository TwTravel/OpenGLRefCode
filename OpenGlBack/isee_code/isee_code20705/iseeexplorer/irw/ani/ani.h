/********************************************************************

	ani.h

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
	���ļ���;��	ISeeͼ���������ANIͼ���дģ�鶨���ļ�
	
					��ȡ���ܣ�1��4��8��16��24��32λANIͼ��
							  
					���湦�ܣ���֧��
							   
	  
	���ļ���д�ˣ�	
					YZ		yzfree##yeah.net
		
	���ļ��汾��	20330
	����޸��ڣ�	2002-3-30
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

			2002-3		������һ���汾���°棬֧��ISeeIOϵͳ��		


********************************************************************/



#ifndef __ANI_MODULE_INC__
#define __ANI_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* ���ݴ洢�豸�ӿڶ����ļ� */
#endif


/* ����ģ��汾�����ơ�����ǰ׺��Ĭ�ϵ��ļ�����׺ */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("ANI���")
#define MODULE_FUNC_PREFIX				("ani_")
#define MODULE_FILE_POSTFIX				("ani")

/* �ļ��б��־ */


#ifdef WIN32	/* Windows */

#	ifdef  ANI_EXPORTS
#	define ANI_API __declspec(dllexport)
#	else
#	define ANI_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define ANI_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* ������Ͷ��� */
#define ANI_CURTYPE		2


/* ������ͼ�����Ͷ��� */
enum CUR_DATA_TYPE
{
	ANI_CUR_PRI_IMAGE,
	ANI_CUR_SUB_IMAGE
};


/* ����ͼ�����Ϣͷ�ṹ */
typedef struct
{
	unsigned char	width;			/* ͼ���� */
	unsigned char	height;			/* ͼ��߶� */
	unsigned char	color_count;	/* ��ɫ����(���λ��ȴ���8ʱΪ0) */
	unsigned char	reserved;
	unsigned short	x_hotspot;		/* �ȵ�X���� */
	unsigned short	y_hotspot;		/* �ȵ�Y���� */
	unsigned long	bytes_in_res;	/* ͼ����������Դ����ռ���ֽ��� */
	unsigned long	image_offset;	/* ͼ�����ݵ�ƫ�� */
} ANI_CURDIRENTRY, *LPANI_CURDIRENTRY;


/* �����Ϣͷ�ṹ */
typedef struct 
{
	unsigned short	reserved;		/* ���� */
	unsigned short	type;			/* ��Դ����(CURTYPEΪ���) */
	unsigned short	count;			/* ͼ����� */
	ANI_CURDIRENTRY	entries[1];		/* ÿһ��ͼ������ */
} ANI_CURDIR, *LPANI_CURDIR;



/* RIFF ������ID���� */
#define	RIFF_STR	"RIFF"
#define LIST_STR	"LIST"

#define ACON_STR	"ACON"
#define INFO_STR	"INFO"
#define INAM_STR	"INAM"
#define IART_STR	"IART"

#define FRAM_STR	"fram"
#define ANIH_STR	"anih"
#define RATE_STR	"rate"
#define SEQ_STR		"seq "
#define ICON_STR	"icon"

#define NULL_STR	"\0\0\0\0"


typedef enum 
{
	ANICHUNK_OUT = 0,
	ANICHUNK_RIFF,
	ANICHUNK_LIST,
	ANICHUNK_INAM,
	ANICHUNK_IART,
	ANICHUNK_ANIH,
	ANICHUNK_RATE,
	ANICHUNK_SEQ,
	ANICHUNK_ICON,
	ANICHUNK_UNKONW				/* δ֪���� */
} ANICHUNK, *LPANICHUNK;

/* RIFF ���ʹ����� */
#define FORMTYPESEIZE	4


/* ANI�ļ�ͷ�ṹ */
typedef struct _tagAnihead
{
	unsigned char	riff[4];
	unsigned long	imgsize;
	unsigned char	acon[4];
} ANIHEAD, PANIHEAD;


/* ANIͨ�ÿ�ͷ���ṹ�����+���С��*/
typedef  struct  _tagAnitag{
	char			ck_id[4];	/* ���� */
	unsigned long	ck_size;	/* ���С���ֽڼƣ� */
} ANITAG, *PANITAG;


/* ANI�ļ���Ϣ��������anih����ṹ */
typedef  struct  _tagAniheader{
	unsigned long	cbSizeof;		/* ���С����36�ֽ� */
	unsigned long	cFrames;		/* �����ͼ������ */
	unsigned long	cSteps;			/* ��ʾ��ͼ������ */
	unsigned long	cx;				/* ͼ���� */
	unsigned long	cy;				/* ͼ��߶� */
	unsigned long	cBitCount;		/* ��ɫλ�� */
	unsigned long	cPlanes;
	unsigned long	jifRate;		/* JIF���� */
	unsigned long	fl;				/* AF_ICON/AF_SEQUENCE���ñ�� */
} ANIHEADER, *PANIHEADER;


/* ��60��֮һ��ĵ�λ����תΪǧ��֮һ������� */
#define		FPS_TO_MS(r)		(r*(1000/60))



#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* ���������ݽӿڶ����ļ� */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern ANI_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern ANI_API LPIRWP_INFO	CALLAGREEMENT ani_get_plugin_info(void);
extern ANI_API int			CALLAGREEMENT ani_init_plugin(void);
extern ANI_API int			CALLAGREEMENT ani_detach_plugin(void);
#endif	/* WIN32 */

extern ANI_API int	CALLAGREEMENT ani_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern ANI_API int	CALLAGREEMENT ani_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern ANI_API int	CALLAGREEMENT ani_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ANI_MODULE_INC__ */
