/********************************************************************

	iseeio.h

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
			http://cosoft.org.cn/projects/iseeexplorer

	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ISee I/O �����ļ������ݱ�׼��ANSI C ��I/O��
	���ļ���д�ˣ�	YZ			yzfree##sina.com

	���ļ��汾��	20418
	����޸��ڣ�	2002-4-18

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
		��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-4		�����ļ�ȱʡ��ģʽ����Ϊ������ģʽ��
		2001-6		��һ�����԰淢��


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



/* isee I/O �����Ͷ��� */
typedef enum _iseeio_confer_type
{
	ISEEIO_CONFER_INVALID,						/* ��Ч��Э������			*/
	ISEEIO_CONFER_LOC_FILE,						/* �����ļ�����		@@		*/
	ISEEIO_CONFER_LOC_MEMORY,					/* �����ڴ�����		@@		*/
	ISEEIO_CONFER_ROT_INTERNET,					/* Զ��internet		@@		*/
	ISEEIO_CONFER_XXX,							/* ������ */
	ISEEIO_CONFER_MAX = 1024,					/* �߽�ֵ */
	ISEEIO_NON_CONFER_LOC_FILE					/* ��Э�飭�����ļ����� */
} ISEEIO_CONFER_TYPE, *PISEEIO_CONFER_TYPE;
/* ע������ö��ֵ�У�ֻ�д�@@ע�͵Ĳ��ֲſ��Ա�����isio_open_param������*/



/* isee I/O ���ĵ�ַ�ṹ */
typedef struct _tag_istream_address
{
	char				mark[4];				/* Э�������� */
	char				*name;					/* ������ */
	unsigned long		param1;					/* ���Ӳ���1 */
	unsigned long		param2;					/* ���Ӳ���2 */
} ISADDR, *PISADDR;



/* isee I/O Э�鴮��ʶ�� */
#define ISIO_STREAM_TYPE_INVALID		('X')	/* ��Ч��ʶ */
#define ISIO_STREAM_TYPE_LOC_FILE		('F')	/* �����ļ� */
#define ISIO_STREAM_TYPE_LOC_MEMORY		('M')	/* �����ڴ� */
#define ISIO_STREAM_TYPE_ROT_INTERNET	('I')	/* ���� */
/*#define ISIO_STREAM_TYPE_XXX_XXXX		('?')*/	/* ������ */


#if defined(_DEBUG)
#define ISFILE_DBG_ID	("+S+")					/* �������ʶ�ִ��������ã�*/
#endif



/* isee I/O ������ṹ */
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


/* �����ļ�����FILE �ṹ�Ѷ����� stdio.h �ļ��� */
typedef FILE	FILE_STREAM;



/* �����ڴ������ṹ���� */
#define MEMORY_STREAM_UP_SIZE		4096
#define MEMORY_STREAM_UP_STEP		1

typedef struct _tag_memory_stream
{
	char			*p_start_pos;	/* �ڴ����׵�ַ */
	unsigned long	len;			/* �����ݵĳ��� */
	unsigned long	cur_pos;		/* ��ǰ�Ķ�дλ�� */
	unsigned long	mode;			/* ���Ĵ�ģʽ */

	unsigned long	error;			/* �����־ */
	unsigned long	total_size;		/* �ڴ����ܳߴ� */
} MEMORY_STREAM;



/* �������������ṹ���� */
typedef struct _tag_internet_stream
{
	char			*p_address;
} INTERNET_STREAM;




/* ��ģʽ�֣�λ�����

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
#define ISIO_MODE_TXT		0x20000		/* ��ANSI��׼��ISIO������֧�� */



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

/* isee I/O �������� */
extern ISEEIO_API const char * CALLAGREEMENT isio_open_param(PISADDR psct, ISEEIO_CONFER_TYPE type, const char *name, unsigned long param1, unsigned long param2);
extern ISEEIO_API unsigned long CALLAGREEMENT isio_length(ISFILE *stream);

/* isee I/O ������ */
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
�µ�ISIO������ extern ISEEIO_API xxx CALLAGREEMENT isio_xxxx();
*/

#ifdef _DEBUG
extern ISEEIO_API int      CALLAGREEMENT __get_object_counts(void);
#endif /*_DEBUG*/

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __ISEEIO_INC__ */
