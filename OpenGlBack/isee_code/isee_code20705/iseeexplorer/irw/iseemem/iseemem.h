/********************************************************************

	iseemem.h

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
	���ļ���;��	ISeeͼ����������ڴ����ϵͳ�����ļ�
	
	  
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20701
	����޸��ڣ�	2002-7-1
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��
			
			2002-7		�Ľ���������Ϣ�洢����
			2002-6		���Ӷ������������LOG�ļ�����(2.1)
			2002-6		�ڶ��淢��(2.0)
			2002-5		��һ�淢��(1.0)

********************************************************************/



#ifndef __ISEEMEM_MODULE_INC__
#define __ISEEMEM_MODULE_INC__


/* ����ģ��汾 */
#define	MODULE_BUILDID					20


#ifdef WIN32	/* Windows */

#	ifdef  ISEEMEM_EXPORTS
#	define ISEEMEM_API __declspec(dllexport)
#	else
#	define ISEEMEM_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	ifndef NDEBUG
#	define NDEBUG
#	endif
#	endif

#else			/* Linux */

#	define ISEEMEM_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/




#define IM_MAX_REVSIZE				(1024*1024*256)	/* ϵͳ�������ʹ�õ��ڴ��� */

#define IM_GUARD_PAGE_NUM			1				/* ����ҳ����ȱʡΪǰ�÷�����ʽ��*/
#define IM_GUARD_MAGIC				(0x4d455349)	/* �����ṹ��ǩ����ISEM����*/
#define IM_GUARD_FILL				(0xfdfdfdfd)	/* ��������������� */
#define IM_SHIT_FILL				0xcd			/* �����ֽ� */

#define IM_DBGINFO_MAX_SIZE			2048			/* ������Ϣ��󳤶� */

#define IM_LOG_FN					("iseemem.txt")	/* ϵͳ������־�ļ��� */
#define IM_LOG_FN_MAX_SIZE			512				/* ϵͳ������־�ļ�ȫ·����󳤶� */




/* ǰ�����ṹ */
typedef struct _tagFGuardStruct
{
	unsigned long	magic;				/* �ṹ��ǩ */
	unsigned long	f_guard;			/* ����������� */
} FGUARDSTRUCT, *LPFGUARDSTRUCT;

/* ������ṹ */
typedef struct _tagBGuardStruct
{
	unsigned long	b_guard;			/* ����������� */
	unsigned long	magic;				/* �ṹ��ǩ */
} BGUARDSTRUCT, *LPBGUARDSTRUCT;


#define IM_FGUARDSTRUCT_SIZE		(sizeof(FGUARDSTRUCT))
#define IM_BGUARDSTRUCT_SIZE		(sizeof(BGUARDSTRUCT))

#define IM_MAX_PROTECT				8				/* ����ܱ����ڴ��������ò�Ҫ������ֵ��*/

/* �����Ϣ��ṹ */
typedef struct _tagIMWatchInfoStr
{
	int				prot_count;						/* �������ܱ����ڴ��ĸ��� */
	unsigned char * prot_addr[IM_MAX_PROTECT];		/* �������ܱ����ڴ��ĵ�ַ */
	unsigned char * fn;								/* ������������ļ� */
	long			linenum;						/* �����������Դ���� */
	char			rev[20];						/* ���������뵽64�ֽ� */
} IMWATCHINFOSTR, *LPIMWATCHINFOSTR;


#define IM_WATCHINFONUM				15	/* ����Ƕ�׵������� */

#define IM_EXCEPT_MARK_INIT			0	/* ��ʼ״̬ */
#define IM_EXCEPT_MARK_EXCED		1	/* ����ʾ���쳣ԭ�� */

/* ÿ���̵߳ķ���ϵͳ��Ϣ */
typedef struct _tagIMThreadInfo
{
	unsigned long	thread_id;			/* �̱߳�ʶ */
	unsigned long	except_mark;		/* �쳣״̬��־ */

	unsigned char	revbuf[52];			/* �����������ṹ�ߴ���뵽64�ֽ� */

	int				watch_curr;			/* ��ǰ���ڵ�� */
	IMWATCHINFOSTR	watch_info[IM_WATCHINFONUM];
										/* ������Ϣ�ڵ����� */
} IMTHREADINFO, *LPIMTHREADINFO;		/* 64��64��IM_WATCHINFONUM �ֽ� */



#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

ISEEMEM_API void *	CALLAGREEMENT im_malloc_act(size_t size, char *fn, int ln);
ISEEMEM_API void *	CALLAGREEMENT im_calloc_act(size_t num, size_t size, char *fn, int ln);
ISEEMEM_API void *	CALLAGREEMENT im_realloc_act(void *pold, size_t newsize, char *fn, int ln);
ISEEMEM_API void 	CALLAGREEMENT im_free_act(void *paddr, char *fn, int ln);

ISEEMEM_API void	CALLAGREEMENT im_output_debug_info(const char *pstr, ...);
ISEEMEM_API void	CALLAGREEMENT im_check_all(long bln, int mark);
ISEEMEM_API void	CALLAGREEMENT im_protect(unsigned char *puser);
ISEEMEM_API void	CALLAGREEMENT im_unprotect(unsigned char *puser);
ISEEMEM_API void	CALLAGREEMENT im_watch_push(char *fn, long line);
ISEEMEM_API void	CALLAGREEMENT im_watch_pop(void);
ISEEMEM_API void	CALLAGREEMENT im_watch_protect(void *p1, ...);
ISEEMEM_API int 	CALLAGREEMENT im_create(void);
ISEEMEM_API int 	CALLAGREEMENT im_destroy(void);

ISEEMEM_API unsigned long	CALLAGREEMENT im_execpt_fiter(LPEXCEPTION_POINTERS per, long);
ISEEMEM_API LPIMTHREADINFO	CALLAGREEMENT im_get_tls(void);




#ifdef _DEBUG

/* watch��constant��over��end ���չ��ʽ��
	do
	{
		im_watch_push(__FILE__, __LINE__); 
		im_check_all(__LINE__, 0); 
		
		__try 
		{
			constant p, p2 over; ��չ��ʽ���£�
			im_watch_protect(p, p2, 0);

  
			########################################################################
			#																	   #
			#	<--- �û����뽫����˴���										   #
			#																	   #
			########################################################################

		}
		__except(im_execpt_fiter(GetExceptionInformation(), __LINE__))
		{
			;
		};

		im_check_all(__LINE__, 1);

		im_watch_pop();

	} while (0)
*/


/* ���԰�״̬�µķ���ϵͳ�ӿ� */
#define im_malloc(size)				im_malloc_act(size, __FILE__, __LINE__)
#define im_calloc(num, size)		im_calloc_act(num, size, __FILE__, __LINE__)
#define im_realloc(p, ns)			im_realloc_act(p, ns, __FILE__, __LINE__)
#define im_free(p)					im_free_act(p, __FILE__, __LINE__)

#define watch		do { im_watch_push(__FILE__, __LINE__); im_check_all(__LINE__, 0); __try 
#define constant		im_watch_protect(
#define over			,0)
#define end				__except(im_execpt_fiter(GetExceptionInformation(), __LINE__)){;}; \
						im_check_all(__LINE__, 1); \
						im_watch_pop(); \
					} while (0)

#else


#include <malloc.h>

/* ������״̬�µķ���ϵͳ�ӿ� */
#define im_malloc(size)				malloc(size)
#define im_calloc(num, size)		calloc(num, size)
#define im_realloc(p, ns)			realloc(p, ns)
#define im_free(p)					free(p)

#define watch						do
#define constant					(void)
#define and							; (void)
#define over						;
#define end							while (0)

#endif /* _DEBUG */


#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ISEEMEM_MODULE_INC__ */
