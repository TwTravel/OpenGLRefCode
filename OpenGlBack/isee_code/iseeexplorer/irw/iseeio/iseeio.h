/********************************************************************

	iseeio.h

	----------------------------------------------------------------
    软件许可证 － GPL
	版权所有 (C) 2001 VCHelp coPathway ISee workgroup.
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

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee I/O 定义文件（兼容标准：ANSI C 流I/O）
	本文件编写人：	YZ			yzfree##sina.com

	本文件版本：	20418
	最后修改于：	2002-4-18

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
		地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-4		修正文件缺省打开模式（定为二进制模式）
		2001-6		第一个测试版发布


********************************************************************/



#ifndef __ISEEIO_INC__
#define __ISEEIO_INC__


#ifndef WIN32
#if defined(_WIN32) || defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */


#ifdef WIN32	/* Windows OS */

#	ifdef ISEEIO_EXPORTS
#	define ISEEIO_API __declspec(dllexport)
#	else
#	define ISEEIO_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	ifndef NDEBUG
#	define NDEBUG
#	endif
#	endif

#else			/* Linux OS */

#	define ISEEIO_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/



/* isee I/O 流类型定义 */
typedef enum _iseeio_confer_type
{
	ISEEIO_CONFER_INVALID,						/* 无效的协议类型			*/
	ISEEIO_CONFER_LOC_FILE,						/* 本地文件类型		@@		*/
	ISEEIO_CONFER_LOC_MEMORY,					/* 本地内存类型		@@		*/
	ISEEIO_CONFER_ROT_INTERNET,					/* 远程internet		@@		*/
	ISEEIO_CONFER_XXX,							/* 新类型 */
	ISEEIO_CONFER_MAX = 1024,					/* 边界值 */
	ISEEIO_NON_CONFER_LOC_FILE					/* 非协议－本地文件类型 */
} ISEEIO_CONFER_TYPE, *PISEEIO_CONFER_TYPE;
/* 注：以上枚举值中，只有带@@注释的部分才可以被用于isio_open_param函数。*/



/* isee I/O 流的地址结构 */
typedef struct _tag_istream_address
{
	char				mark[4];				/* 协议描述字 */
	char				*name;					/* 流名称 */
	unsigned long		param1;					/* 附加参数1 */
	unsigned long		param2;					/* 附加参数2 */
} ISADDR, *PISADDR;



/* isee I/O 协议串标识符 */
#define ISIO_STREAM_TYPE_INVALID		('X')	/* 无效标识 */
#define ISIO_STREAM_TYPE_LOC_FILE		('F')	/* 本地文件 */
#define ISIO_STREAM_TYPE_LOC_MEMORY		('M')	/* 本地内存 */
#define ISIO_STREAM_TYPE_ROT_INTERNET	('I')	/* 网络 */
/*#define ISIO_STREAM_TYPE_XXX_XXXX		('?')*/	/* 新类型 */


#if defined(_DEBUG)
#define ISFILE_DBG_ID	("+S+")					/* 流句柄标识字串（调试用）*/
#endif



/* isee I/O 流句柄结构 */
typedef	struct _tag_iseeio_stream
{
	void				*stream;
	ISEEIO_CONFER_TYPE	stream_type;
	char				*org_name;
#if defined(_DEBUG)
	char				dbg_iseeio_stream_id[4];
#endif
} ISFILE;


#ifndef _FILE_DEFINED
#include <stdio.h>
#endif


/* 本地文件流－FILE 结构已定义在 stdio.h 文件中 */
typedef FILE	FILE_STREAM;



/* 本地内存流－结构定义 */
#define MEMORY_STREAM_UP_SIZE		4096
#define MEMORY_STREAM_UP_STEP		1

typedef struct _tag_memory_stream
{
	char			*p_start_pos;	/* 内存块的首地址 */
	unsigned long	len;			/* 有内容的长度 */
	unsigned long	cur_pos;		/* 当前的读写位置 */
	unsigned long	mode;			/* 流的打开模式 */

	unsigned long	error;			/* 错误标志 */
	unsigned long	total_size;		/* 内存块的总尺寸 */
} MEMORY_STREAM;



/* 网络数据流－结构定义 */
typedef struct _tag_internet_stream
{
	char			*p_address;
} INTERNET_STREAM;




/* 流模式字－位域含义表：

						32 bit (unsigned long)
	_________________________________________________________________->low bit
	|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|
                                 | |                         | | | |
                                 | bin                       | | | |
                                 text                        | | | read
                                                             | | write
                                                             | append
															 create
*/
#define ISIO_MODE_READ		0x1
#define ISIO_MODE_WRITE		0x2
#define ISIO_MODE_RDWR		0x3
#define ISIO_MODE_APPEND	0x4
#define ISIO_MODE_CREATE	0x8

#define ISIO_MODE_BIN		0x10000
#define ISIO_MODE_TXT		0x20000		/* 非ANSI标准，ISIO将不予支持 */



#ifdef	WIN32
#define ISIO_ERROR_EOF		_IOEOF
#define ISIO_ERROR_ERR		_IOERR
#else
#define ISIO_ERROR_EOF		0x10
#define ISIO_ERROR_ERR		0x20
#endif /* WIN32 */


#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

/* isee I/O 辅助函数 */
extern ISEEIO_API const char * CALLAGREEMENT isio_open_param(PISADDR psct, ISEEIO_CONFER_TYPE type, const char *name, unsigned long param1, unsigned long param2);
extern ISEEIO_API unsigned long CALLAGREEMENT isio_length(ISFILE *stream);

/* isee I/O 流函数 */
extern ISEEIO_API ISFILE * CALLAGREEMENT isio_open(const char *pstream_name, const char *mode);
extern ISEEIO_API int      CALLAGREEMENT isio_close(ISFILE *stream);

extern ISEEIO_API size_t   CALLAGREEMENT isio_read(void *buffer, size_t size, size_t count, ISFILE *stream);
extern ISEEIO_API size_t   CALLAGREEMENT isio_write(const void *buffer, size_t size, size_t count, ISFILE *stream);
extern ISEEIO_API int      CALLAGREEMENT isio_flush(ISFILE *stream);

extern ISEEIO_API int      CALLAGREEMENT isio_seek(ISFILE *stream, long offset, int origin);
extern ISEEIO_API void     CALLAGREEMENT isio_rewind(ISFILE *stream);
extern ISEEIO_API long     CALLAGREEMENT isio_tell(ISFILE *stream);

extern ISEEIO_API int      CALLAGREEMENT isio_eof(ISFILE *stream);
extern ISEEIO_API int      CALLAGREEMENT isio_error(ISFILE *stream);
extern ISEEIO_API void     CALLAGREEMENT isio_clearerr(ISFILE *stream);

/*
新的ISIO流函数 extern ISEEIO_API xxx CALLAGREEMENT isio_xxxx();
*/

#ifdef _DEBUG
extern ISEEIO_API int      CALLAGREEMENT __get_object_counts(void);
#endif /*_DEBUG*/

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ISEEIO_INC__ */
