/********************************************************************

	iseemem.h

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2002 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本（本目录
	GPL.txt文件）。如果还没有，写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	如果你在使用本软件时有什么问题或建议，请用以下地址与我们取得联系：

			http://isee.126.com
			http://cosoft.org.cn/projects/iseeexplorer

	或发信到：

			isee##vip.163.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—内存防护系统定义文件
	
	  
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20701
	最后修改于：	2002-7-1
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
			2002-7		改进防护宏信息存储机制
			2002-6		增加对自启动程序的LOG文件功能(2.1)
			2002-6		第二版发布(2.0)
			2002-5		第一版发布(1.0)

********************************************************************/



#ifndef __ISEEMEM_MODULE_INC__
#define __ISEEMEM_MODULE_INC__


/* 定义模块版本 */
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




#define IM_MAX_REVSIZE				(1024*1024*256)	/* 系统最大允许使用的内存量 */

#define IM_GUARD_PAGE_NUM			1				/* 防护页数（缺省为前置防护方式）*/
#define IM_GUARD_MAGIC				(0x4d455349)	/* 防护结构标签（“ISEM”）*/
#define IM_GUARD_FILL				(0xfdfdfdfd)	/* 防溢出缓冲区内容 */
#define IM_SHIT_FILL				0xcd			/* 垃圾字节 */

#define IM_DBGINFO_MAX_SIZE			2048			/* 调试信息最大长度 */

#define IM_LOG_FN					("iseemem.txt")	/* 系统运行日志文件名 */
#define IM_LOG_FN_MAX_SIZE			512				/* 系统运行日志文件全路径最大长度 */




/* 前防护结构 */
typedef struct _tagFGuardStruct
{
	unsigned long	magic;				/* 结构标签 */
	unsigned long	f_guard;			/* 防溢出缓冲区 */
} FGUARDSTRUCT, *LPFGUARDSTRUCT;

/* 后防护结构 */
typedef struct _tagBGuardStruct
{
	unsigned long	b_guard;			/* 防溢出缓冲区 */
	unsigned long	magic;				/* 结构标签 */
} BGUARDSTRUCT, *LPBGUARDSTRUCT;


#define IM_FGUARDSTRUCT_SIZE		(sizeof(FGUARDSTRUCT))
#define IM_BGUARDSTRUCT_SIZE		(sizeof(BGUARDSTRUCT))

#define IM_MAX_PROTECT				8				/* 最大受保护内存个数（最好不要调整该值）*/

/* 监测信息块结构 */
typedef struct _tagIMWatchInfoStr
{
	int				prot_count;						/* 监测块中受保护内存块的个数 */
	unsigned char * prot_addr[IM_MAX_PROTECT];		/* 监测块中受保护内存块的地址 */
	unsigned char * fn;								/* 监测块代码所在文件 */
	long			linenum;						/* 监测块代码所在源码行 */
	char			rev[20];						/* 保留，对齐到64字节 */
} IMWATCHINFOSTR, *LPIMWATCHINFOSTR;


#define IM_WATCHINFONUM				15	/* 监测块嵌套的最大深度 */

#define IM_EXCEPT_MARK_INIT			0	/* 初始状态 */
#define IM_EXCEPT_MARK_EXCED		1	/* 已显示过异常原因 */

/* 每个线程的防护系统信息 */
typedef struct _tagIMThreadInfo
{
	unsigned long	thread_id;			/* 线程标识 */
	unsigned long	except_mark;		/* 异常状态标志 */

	unsigned char	revbuf[52];			/* 保留，并将结构尺寸对齐到64字节 */

	int				watch_curr;			/* 当前监测节点号 */
	IMWATCHINFOSTR	watch_info[IM_WATCHINFONUM];
										/* 监测块信息节点数组 */
} IMTHREADINFO, *LPIMTHREADINFO;		/* 64＋64×IM_WATCHINFONUM 字节 */



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

/* watch、constant、over、end 宏的展开式：
	do
	{
		im_watch_push(__FILE__, __LINE__); 
		im_check_all(__LINE__, 0); 
		
		__try 
		{
			constant p, p2 over; 的展开式如下：
			im_watch_protect(p, p2, 0);

  
			########################################################################
			#																	   #
			#	<--- 用户代码将插入此处！										   #
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


/* 调试版状态下的防护系统接口 */
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

/* 发布版状态下的防护系统接口 */
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
