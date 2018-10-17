/********************************************************************

	iseeio.c

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
	���ļ���;��	ISee I/O ʵ���ļ������ݱ�׼��ANSI C ��I/O��
	���ļ���д�ˣ�	YZ			yzfree##sina.com

	���ļ��汾��	20418
	����޸��ڣ�	2002-4-18

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
		��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

		2002-4		�����ļ�ȱʡ��ģʽ����Ϊ������ģʽ��
		2001-6		������һ�����԰�

********************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "ISeeIO.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#ifdef _DEBUG
static int __isfile_object_counts = 0;
#endif

#if defined(WIN32) && defined(_MSC_VER)
#include <fcntl.h>	/* �趨�ļ���ģʽʱ��Ҫ��һЩ�� */
static int old_fmode;
#endif



/* �ڲ����� */
ISEEIO_CONFER_TYPE	CALLAGREEMENT _get_confer_type(PISADDR confer_str);
unsigned long		CALLAGREEMENT _analyst_for_mode(const char *mode);
int					CALLAGREEMENT _add_memory_block(MEMORY_STREAM *memstream, int add_size);
int					CALLAGREEMENT _dec_memory_block(MEMORY_STREAM *memstream);
ISFILE *			CALLAGREEMENT _alloc_ISFILE(void);
ISFILE *			CALLAGREEMENT _free_ISFILE(ISFILE *pfile);
MEMORY_STREAM *		CALLAGREEMENT _alloc_MEMORY_STREAM(void);
MEMORY_STREAM *		CALLAGREEMENT _free_MEMORY_STREAM(MEMORY_STREAM *pmem);



/* ################################################################

	ע��* ��ǰ�汾ֻʵ�����ļ������ڴ����Ĵ��룬������δʵ�֡�
		* ISIO���������趨ԭ���Ǽ���ANSI C�����������ڱ����������Լ�
		  ��չ�����������ڿ��Ƿ�Χ֮�ڡ�

   ################################################################*/



#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if defined(_MSC_VER)
		old_fmode = _fmode;		/* ������ǰ���ļ���ģʽ */
		_fmode = _O_BINARY;		/* ȱʡ�ļ���ģʽ��Ϊ������ģʽ */
#endif
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
#if defined(_MSC_VER)
		_fmode = old_fmode;		/* �ָ���ǰ���ļ���ģʽ */
#endif
		break;
    }
	
    return TRUE;
}
#endif /* WIN32 */



/*
  ���ܣ�׼������isio_open������ַ����

  ��ڣ�
	PISADDR psct				- ��������ṹ

	ISEEIO_CONFER_TYPE type		- �������͡��μ�ö������ISEEIO_CONFER_TYPE��

	const char *name			- �����ơ�  �ļ����ͣ�ָ��·������ָ��
										    �ڴ����ͣ�ָ���ڴ���׵�ַ��ָ�루���ڴ������Ƕ����ģ���������ĳһ�ڴ���м�ĵ�ַ��
										    �������ͣ���

	unsigned long param1		- ���Ӳ������ļ����ͣ���
											�ڴ����ͣ��ڴ��ĳߴ�
											�������ͣ���

	unsigned long param2		- ���Ӳ������ļ����ͣ���
											�ڴ����ͣ���
											�������ͣ���


	���أ���ǿ��ת����psct��ַ��
*/
ISEEIO_API const char * CALLAGREEMENT isio_open_param(
	PISADDR psct, 
	ISEEIO_CONFER_TYPE type, 
	const char *name, 
	unsigned long param1, 
	unsigned long param2
	)
{
	assert(psct);

	/* Э���־ "+X+" */
	psct->mark[0] = psct->mark[2] = '+';
	psct->mark[3] = '\0';

	switch (type)
	{
	case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
		psct->mark[1] = ISIO_STREAM_TYPE_LOC_FILE;
		break;
	case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
		psct->mark[1] = ISIO_STREAM_TYPE_LOC_MEMORY;
		break;
	case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
		psct->mark[1] = ISIO_STREAM_TYPE_ROT_INTERNET;
		break;
	default:									
		assert(0);								/* �ݲ�֧���������� */
		psct->mark[1] = ISIO_STREAM_TYPE_INVALID;
		break;
	}

	psct->name = (char*)name;
	psct->param1 = param1;
	psct->param2 = param2;

	return (const char *)psct;
}




/*
  ���ܣ���ָ����ISIO��

  ��ڣ�
	pstream_name	�� �����ƽṹ��ַ��

	�ò����������������ͣ�һ����ISADDR�ṹ��ַ��ǿ��ת��Ϊchar*����������
	�������£�

				�ļ�����	�ڴ�����	��������
	---------------------------------------------------------------------
	mark[4]		+F+			+M+			+I+

	*name		�ļ�·��	�ڴ����	��
				��			��ַ��
							д����ʱ
							�ɲ�����

	param1		��			�ڴ����	��
							����ֵ��
							д����ʱ
							�ɲ�����
							
	param2		��			��			��
	---------------------------------------------------------------------
													ע��"��"��ʾδ��ʹ��

	��һ����char*�͵��ļ�·�����ĵ�ַ���磺"a:\a.txt"�ȵȡ�
	
	mode			�� �����ķ�ʽ��

	��Ч��������£�r,r+,rb,r+b,w,w+,wb,w+b,a,a+,ab,a+b�����ǵĺ���ɲμ�
	ANSI C��fopen��ע�͡�

	
	ע������isio_open��һ���ڴ�������ڲ���pstream_name��ָ���ISADDR
		�ṹ������ɾ������Ϊ��������ʱ��ϵͳ����������д������ݡ���ϸ˵��
		�μ�isio_close������

  ���أ�  0 �� ��ʧ��
		��0 �� �򿪳ɹ�������ֵ������������ISIO����
*/
ISEEIO_API ISFILE * CALLAGREEMENT isio_open(const char *pstream_name, const char *mode)
{
	PISADDR				pname = (PISADDR)pstream_name;
	ISFILE				*pfile = (ISFILE*)0;
	void				*pstream = (void*)0;
	MEMORY_STREAM		*pmemory;

	ISEEIO_CONFER_TYPE	stream_type;

	assert((pname)&&(mode));						/* ���ϸ����ڲ������ */

	if ((!pname)||(!mode))
		return (ISFILE*)0;

	/* �ж�ָ������Э������ */
	if ((stream_type = _get_confer_type(pname)) == ISEEIO_CONFER_INVALID)
		return (ISFILE*)0;

	/* ����ISFILE�ṹ�� */
	if ((pfile = _alloc_ISFILE()) == (ISFILE*)0)
		return (ISFILE*)0;							/* �ڴ治�� */


	/* ����Э�����͵Ĳ�ͬ������Ӧ���� */
	switch (stream_type)
	{
		case	ISEEIO_NON_CONFER_LOC_FILE:			/* ��Э�飭�����ļ� */
			if ((pstream = (void*)fopen(pstream_name, mode)) == (void*)0)
				return _free_ISFILE(pfile);

			pfile->stream = pstream;
			pfile->stream_type = ISEEIO_CONFER_LOC_FILE;
			pfile->org_name = (char*)pstream_name;
			return pfile;
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			if ((pstream = (void*)fopen((const char *)pname->name, mode)) == (void*)0)
				return _free_ISFILE(pfile);

			pfile->stream = pstream;
			pfile->stream_type = ISEEIO_CONFER_LOC_FILE;
			pfile->org_name = pname->name;
			return pfile;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			/* �����ڴ����ṹ */
			if ((pmemory = _alloc_MEMORY_STREAM()) == (void*)0)
				return _free_ISFILE(pfile);

			pfile->stream = (void*)pmemory;

			pmemory->mode = _analyst_for_mode(mode);
			
			if (pname->name)	/* �Ѿ����ڵ��ڴ��� */
			{
				pmemory->p_start_pos = pname->name;
				/* �����"w"��"w+"��ʽ�򿪵��ڴ������򳤶ȱ���Ϊ0 */
				pmemory->len         = ((pmemory->mode & (ISIO_MODE_WRITE|ISIO_MODE_CREATE)) == (ISIO_MODE_WRITE|ISIO_MODE_CREATE)) ? 0 : pname->param1;
				pmemory->total_size  = pname->param1;
				pmemory->cur_pos     = 0;
				pmemory->error       = (pmemory->len == pmemory->cur_pos) ? ISIO_ERROR_EOF:0;
			}
			else				/* �����ڵ��ڴ��� */
			{
				if (!(pmemory->mode & ISIO_MODE_CREATE))
				{
					_free_MEMORY_STREAM((MEMORY_STREAM *)(pfile->stream));
					return _free_ISFILE(pfile);		/* û�д������� */
				}

				/* �����ڴ�����һ�����䵥λ��������*/
				if (_add_memory_block(pmemory, 0) == -1)
				{
					_free_MEMORY_STREAM((MEMORY_STREAM *)(pfile->stream));
					return _free_ISFILE(pfile);		/* �ڴ治�� */
				}

				pmemory->len = 0UL;
				pmemory->cur_pos = 0UL;
				pmemory->error = ISIO_ERROR_EOF;
			}

			pfile->stream_type = ISEEIO_CONFER_LOC_MEMORY;
			pfile->org_name = (char*)pname;			/* ��д�ṹ��ַ */

			pname->name = ((MEMORY_STREAM*)pfile->stream)->p_start_pos;

			return pfile;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return (ISFILE*)0;
}




/*
  ���ܣ��ر�ָ����ISIO��

  ��ڣ�
		stream	�� ��isio_open()�����򿪵�ISIO�����

	��ע��

	isee I/O �����رպ�isio_close����������isio_open���������PISADDR�ṹ
	��д��Ҫ�����ݣ������������£�

				�ļ�����	�ڴ�����	��������
	---------------------------------------------------------------------
	mark[4]		��			��			��
	*name		��			������	��
							����׵�ַ
	param1		��			�ڴ������	��
							����ֵ
	param2		��			�ڴ���ܳ�	��
							��
	---------------------------------------------------------------------
													ע��"��"��ʾδ���ı�
	
	�ڴ����ͱ�ע������һ���ڴ�����ԭ�����ڴ�飨���ָ���˵Ļ���������
		��ʹ�ã�����ڴ��ܿ��ܻᱻISIO�����ı�����λ�ã�������������д��
		�ݵ��������ֻ�йر�����ڴ����������������ṹ�з��صĵ�ַ��name
		���ſ��Ա���ȫ��ʹ�á�


  ���أ�  0 �� �رճɹ�
		EOF �� �ر�ʧ��
*/
ISEEIO_API int CALLAGREEMENT isio_close(ISFILE *stream)
{
	int result;

	assert(stream);									/* ���ϸ����ڲ������ */

	if (!stream)
		return EOF;

	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	/* �ͷ���Ӧ���� */
	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			assert(stream->stream);
			result = fclose((FILE*)(stream->stream));
			_free_ISFILE(stream);
			return result;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			/* ѹ���ڴ�β���Ŀ����ڴ��Խ�ʡ�ռ� */
			_dec_memory_block((MEMORY_STREAM*)stream->stream);
			
			assert(((MEMORY_STREAM*)(stream->stream))->p_start_pos);

			/* ��д�ڴ�����Ϣ 
			   
			   ע�⣺* �������ر���ǰ������Open���ĵ�ַ�ṹ���ܱ�ɾ����
			         * �ڴ��������ݿ鲻�ᱻClose�����������ͷţ��������ص�����	
			*/
			((PISADDR)(stream->org_name))->name   = ((MEMORY_STREAM*)(stream->stream))->p_start_pos;	/* �ڴ���׵�ַ */
			((PISADDR)(stream->org_name))->param1 = ((MEMORY_STREAM*)(stream->stream))->len;			/* �ڴ�����ݵĳߴ� */
			((PISADDR)(stream->org_name))->param2 = ((MEMORY_STREAM*)(stream->stream))->total_size;		/* �ڴ���ʵ�ʳߴ� */

			/* �ͷ��ڴ����ṹ */
			_free_MEMORY_STREAM((MEMORY_STREAM *)(stream->stream));
			/* �ͷ����ṹ */
			_free_ISFILE(stream);

			return 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			/* �ͷ��ڲ����ݡ����� */
			_free_ISFILE(stream);
			return 0;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}




/* 
  ���ܣ���ISIO��������

  ��ڣ�
		buffer	�� ���ڴ�Ŷ������ݵĻ������׵�ַ
		size	�� ������ĳߴ�
		count	�� ������ĸ���
		stream	�� ISIO�����

  ���أ��ɹ���������������
*/
ISEEIO_API size_t CALLAGREEMENT isio_read(void *buffer, size_t size, size_t count, ISFILE *stream)
{
	MEMORY_STREAM *pmemory;
	char *buf = (char*)buffer;
	int i, step;

	assert(buffer);								/* ���ϸ����ڲ������ */
	assert(stream);

	if ((!stream)||(!buffer)||(!size)||(!count))
		return 0;

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));
	

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return fread(buffer, size, count, (FILE*)(stream->stream));
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)stream->stream;

			if (pmemory->error & ISIO_ERROR_EOF)
			{
				assert(pmemory->len == pmemory->cur_pos);
				return 0;							/* �ѵ����ļ�β�� */
			}

			assert(pmemory->cur_pos < pmemory->len);

			/* �������ݵ�ָ���Ļ����� */
			for (i=0;i<(int)count;i++)
			{
				if ((pmemory->cur_pos+size) > pmemory->len)
					step = (int)(pmemory->len - pmemory->cur_pos);
				else
					step = size;

				memcpy((void*)buf, (const void *)(pmemory->p_start_pos+pmemory->cur_pos), step);

				/* ˢ������ǰָ�� */
				pmemory->cur_pos += (unsigned long)step;
				buf += step;

				if (pmemory->cur_pos == pmemory->len)
				{
					pmemory->error |= ISIO_ERROR_EOF;
					if (step == (int)size) i++;
					break;
				}
			}
			return i;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}




/* 
  ���ܣ���ISIO��д����

  ��ڣ�
		buffer	�� ��д�����ݻ������׵�ַ
		size	�� ������ĳߴ�
		count	�� ������ĸ���
		stream	�� ISIO�����

  ���أ��ɹ�д������������
*/
ISEEIO_API size_t CALLAGREEMENT isio_write(const void *buffer, size_t size, size_t count, ISFILE *stream)
{
	MEMORY_STREAM *pmemory;
	char *buf = (char*)buffer;
	int i, addsize;

	assert(buffer);								/* ���ϸ����ڲ������ */
	assert(stream);

	if ((!stream)||(!buffer)||(!size)||(!count))
		return 0;

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return fwrite(buffer, size, count, (FILE*)stream->stream);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)stream->stream;

			/* �ж�ָ���ڴ���Ƿ���д��׷�ӣ�Ȩ�� */
			if (!(pmemory->mode & (ISIO_MODE_WRITE|ISIO_MODE_APPEND)))
			{
				pmemory->error |= ISIO_ERROR_ERR;
				return 0;							/* û��дȨ�� */
			}

			/* ׷������������д���������ݶ�׷�ӵ��ļ�β�� */
			if (pmemory->mode & ISIO_MODE_APPEND)
			{
				pmemory->cur_pos = pmemory->len;
				pmemory->error |= ISIO_ERROR_EOF;
			}

			/* �ж��Ƿ���Ҫ�����ڴ�� */
			if ((pmemory->cur_pos+(unsigned long)(size*count)) > pmemory->total_size)
			{
				/* Ϊ����ϵͳ�ط���������˴������ȡ������ */
				addsize = (pmemory->cur_pos+size*count)-pmemory->total_size;
				addsize = ((addsize + (MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP-1))/(MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP))*(MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP);
				if (_add_memory_block(pmemory, addsize) == -1)/* �����ڴ��ߴ� */
				{
					pmemory->error |= ISIO_ERROR_ERR;
					return 0;			/* �ڴ治�� */
				}
				assert((pmemory->cur_pos+(unsigned long)(size*count)) <= pmemory->total_size);
			}

			/* �������ݵ�ָ���Ļ����� */
			for (i=0;i<(int)count;i++)
			{
				memcpy((void*)(pmemory->p_start_pos+pmemory->cur_pos), (const void *)buf, size);

				pmemory->cur_pos += (unsigned long)size;
				buf += (int)size;

				if (pmemory->cur_pos >= pmemory->len)
				{
					pmemory->len = pmemory->cur_pos;
					pmemory->error |= ISIO_ERROR_EOF;
				}

				assert(pmemory->cur_pos <= pmemory->len);
				assert(pmemory->len <= pmemory->total_size);
			}
			return i;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}



/* 
  ���ܣ�ˢ��ISIO����ע���ڴ���û��ˢ�����ԣ�

  ��ڣ�
		stream	�� ISIO�����

  ���أ� 0  �� ˢ�³ɹ�
		EOF �� ʧ��
*/
ISEEIO_API int CALLAGREEMENT isio_flush(ISFILE *stream)
{
	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return fflush((FILE*)(stream->stream));
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			return 0;	/* �޻������������ˢ�� */
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}




/* 
  ���ܣ�ISIO��ָ�붨λ��ע������λ�ã�

  ��ڣ�
		stream	�� ISIO�����
		offset	�� �����origin��ƫ���������ֽ�Ϊ��λ��
		origin	�� ��λ�õĲο��㣨SEEK_SET��SEEK_CUR��SEEK_END��

  ���أ� 0 �� ��λ�ɹ�
		-1 �� ʧ��
*/
ISEEIO_API int CALLAGREEMENT isio_seek(ISFILE *stream, long offset, int origin)
{
	MEMORY_STREAM *pmemory;
	unsigned long new_off = 0UL;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return fseek((FILE*)stream->stream, offset, origin);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)stream->stream;

			assert(pmemory->p_start_pos);

			switch (origin)
			{
			case	SEEK_SET:
				new_off = (unsigned long)offset;
				break;
			case	SEEK_CUR:
				new_off = (unsigned long)((long)(pmemory->cur_pos)+offset);
				break;
			case	SEEK_END:
				new_off = (unsigned long)((long)(pmemory->len)+offset);
				break;
			default:
				assert(0);							/* �����ƴ��� */
				break;
			}

			if (new_off > pmemory->len)				/* �Ƿ�λ�ã���ǰλ�ò��ı� */
				return -1;

			if (new_off == pmemory->len)
				pmemory->error |= ISIO_ERROR_EOF;
			else
				pmemory->error &= ~(ISIO_ERROR_EOF);

			pmemory->cur_pos = new_off;

			return 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}




/* 
  ���ܣ�ISIO��ָ�붨λ��ע����λ����ͷ����

  ��ڣ�
		stream	�� ISIO�����

  ���أ���
*/

ISEEIO_API void CALLAGREEMENT isio_rewind(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			rewind((FILE*)(stream->stream));
			return;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)stream->stream;

			assert(pmemory->p_start_pos);

			pmemory->cur_pos = 0UL;

			if (pmemory->cur_pos == pmemory->len)
				pmemory->error |= ISIO_ERROR_EOF;
			else
				pmemory->error &= ~(ISIO_ERROR_EOF);
			
			return;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return;
}




/* 
  ���ܣ���ȡISIO����ǰָ��λ��

  ��ڣ�
		stream	�� ISIO�����

  ���أ�  -1 �� ����ִ��ʧ��
		��-1 �� λ��ֵ
*/
ISEEIO_API long CALLAGREEMENT isio_tell(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return ftell((FILE*)(stream->stream));
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)stream->stream;

			assert(pmemory->p_start_pos);

			return (long)pmemory->cur_pos;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return (long)-1;
}




/* 
  ���ܣ��ж�ISIO����ǰָ���Ƿ�������β��

  ��ڣ�
		stream	�� ISIO�����

  ���أ�  0 �� ����β��
		��0 �� ��β��
*/
ISEEIO_API int CALLAGREEMENT isio_eof(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return feof((FILE*)stream->stream);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)(stream->stream);

			assert(pmemory->p_start_pos);			/* �ڴ�������� */

			return (pmemory->error & ISIO_ERROR_EOF) ? ISIO_ERROR_EOF : 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}




/* 
  ���ܣ��ж�ISIO���Ƿ����˴���

  ��ڣ�
		stream	�� ISIO�����

  ���أ�  0 �� û�д���
		��0 �� �д���
*/
ISEEIO_API int CALLAGREEMENT isio_error(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			return ferror((FILE*)stream->stream);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)(stream->stream);

			assert(pmemory->p_start_pos);			/* �ڴ�������� */

			return (pmemory->error & ISIO_ERROR_ERR) ? ISIO_ERROR_ERR : 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return 0;
}




/* 
  ���ܣ����ISIO���Ĵ����־��EOF��־

  ��ڣ�
		stream	�� ISIO�����

  ���أ���
*/
ISEEIO_API void CALLAGREEMENT isio_clearerr(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* �����ļ����� */
			clearerr((FILE*)(stream->stream));
			return;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* �����ڴ����� */
			pmemory = (MEMORY_STREAM *)(stream->stream);

			assert(pmemory->p_start_pos);			/* �ڴ�������� */

			pmemory->error &= ~(ISIO_ERROR_ERR);	/* �������ָʾ�� */

			if (pmemory->cur_pos == pmemory->len)	/* ���ô����־ */
				pmemory->error |= ISIO_ERROR_EOF;
			else
				pmemory->error &= ~ISIO_ERROR_EOF;

			return;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* Զ��internet */
			break;
		default:
			assert(0);								/* ��Ч��Э������ */
			break;
	}

	return;
}




/* 
  ���ܣ�ȡ��ISIO���ĳ��� 

  ��ڣ�
		stream	�� ISIO�����

  ���أ������ȣ����ֽ�Ϊ��λ��
*/
ISEEIO_API unsigned long CALLAGREEMENT isio_length(ISFILE *stream)
{
	long pos;
	unsigned long len;
	
	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));
	
	/* ȡ�������Ⱥ����ĵ�ǰָ�벻�ı� */
	pos = isio_tell(stream);

	isio_seek(stream, 0, SEEK_END);
	len = (unsigned long)isio_tell(stream);

	isio_seek(stream, pos, SEEK_SET);

	return len;
}





/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/* ϵͳ�ڲ����� */


/* �ж�Э������ */
ISEEIO_CONFER_TYPE CALLAGREEMENT _get_confer_type(PISADDR confer_str)
{
	assert(confer_str);	/* ���������ڲ����� */

	/* �ж��Ƿ���Э�鴮 */
	if ((confer_str->mark[0] == '+')&&
		(confer_str->mark[2] == '+')&&
		(confer_str->mark[3] == '\0'))
	{
		switch(confer_str->mark[1])
		{
		case	ISIO_STREAM_TYPE_LOC_FILE:		/* file ���� */
			return ISEEIO_CONFER_LOC_FILE;
		case	ISIO_STREAM_TYPE_LOC_MEMORY:	/* memory ���� */
			return ISEEIO_CONFER_LOC_MEMORY;
		case	ISIO_STREAM_TYPE_ROT_INTERNET:	/* internet ���� */
			return ISEEIO_CONFER_ROT_INTERNET;
		/* case	ISIO_STREAM_TYPE_XXX_XXXX:
			return ISEEIO_CONFER_XXXX; */
		default:
			assert(0);	/* �Ƿ����� */
			return ISEEIO_CONFER_INVALID;
		}
	}

	if (confer_str->mark[0] == '\0')
		return ISEEIO_CONFER_INVALID;
	
	return ISEEIO_NON_CONFER_LOC_FILE;		/* ��Э�飭�����ļ����� */
}


/* ����ISIO��Openģʽ */
unsigned long CALLAGREEMENT _analyst_for_mode(const char *mode)
{
	unsigned long result = 0UL;
	int count = 0;

	assert(mode);

    switch (*mode)
	{
	case 'r':
		result = ISIO_MODE_READ;	break;
    case 'w':
		result = ISIO_MODE_CREATE | ISIO_MODE_WRITE | ISIO_MODE_READ;	break;
    case 'a':
		result = ISIO_MODE_CREATE | ISIO_MODE_APPEND | ISIO_MODE_READ;	break;
    default:
		assert(0);						/* �Ƿ���ģʽ��ʶ�� ANSI (r/w/a)*/
		return 0UL;
    }

	result |= ISIO_MODE_BIN;			/* ANSI ��ģʽΪ������ */

	while(*++mode)
	{
		switch(*mode) 
		{
		case '+':
			if (result & (ISIO_MODE_WRITE|ISIO_MODE_APPEND))
				;
			else
				result |= ISIO_MODE_WRITE;
			break;
		case 'b':
			break;
		default:
			assert(0);					/* �Ƿ���ģʽ��ʶ�� */
			count = 2;
			break;
		}

		/* ģʽ����󳤶�Ϊ3���ַ����磺"r+b"�ȵ� */
		if (++count >= 2)
			break;
	}

	return result;
}


/* Ϊ�ڴ������ӿռ䣨���ӵĳߴ�add_size��*/
int CALLAGREEMENT _add_memory_block(MEMORY_STREAM *memstream, int add_size)
{
	char *ptmp;
	int new_size;

	assert(memstream);
	assert(memstream->mode & ISIO_MODE_CREATE);

	/* ���add_size��ָ��Ϊ0����������ϵͳȱʡ���ڴ�ߴ磺MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP */
	new_size = memstream->total_size + ((add_size == 0) ? (MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP) : add_size);

	ptmp = (char*)realloc((void*)(memstream->p_start_pos), new_size);

	if (!ptmp)
		return -1;			/* �ڴ治�� */

	memstream->p_start_pos = ptmp;
	memstream->total_size  = new_size;

	return 0;				/* �ɹ� */
}


/* �ͷ��ڴ��β���Ŀ����ڴ� */
int CALLAGREEMENT _dec_memory_block(MEMORY_STREAM *memstream)
{
	char *ptmp;

	assert(memstream);
	assert(memstream->total_size >= memstream->len);

	if ((memstream->p_start_pos)&&(memstream->total_size > memstream->len)) 
	{
		ptmp = (char*)realloc((void*)(memstream->p_start_pos), memstream->len);
		if (!ptmp)
			return -1;		/* realloc����ʧ�ܣ��ƺ�������:�� */
		memstream->p_start_pos = ptmp;
		memstream->total_size  = memstream->len;
		return 0;			/* �ɹ� */
	}

	return 1;				/* û�п��е��ڴ����� */
}


/* ����һ���µ��ڴ����ṹ */
MEMORY_STREAM * CALLAGREEMENT _alloc_MEMORY_STREAM()
{
	MEMORY_STREAM *pmem = (MEMORY_STREAM *)malloc(sizeof(MEMORY_STREAM));

	if (!pmem)
		return (MEMORY_STREAM *)0;

	pmem->p_start_pos = (char *)0;

	pmem->len = pmem->cur_pos = pmem->total_size = 0UL;

	pmem->mode = 0UL;

	pmem->error = 0UL;

	return pmem;
}


/* �ͷ�һ���ڴ����ṹ���ú��������ͷ�����Ӧ���ڴ�飩 */
MEMORY_STREAM * CALLAGREEMENT _free_MEMORY_STREAM(MEMORY_STREAM *pmem)
{
	assert(pmem);				/* ���ϸ����ڲ������ */

	free(pmem);

	return (MEMORY_STREAM*)0;
}


/* ����һ���µ�ISFILE�ṹ */
ISFILE * CALLAGREEMENT _alloc_ISFILE()
{
	ISFILE *pfile = (ISFILE*)malloc(sizeof(ISFILE));

	if (!pfile)
		return (ISFILE*)0;

#ifdef _DEBUG
	strcpy((char *)pfile->dbg_iseeio_stream_id, ISFILE_DBG_ID);
#endif
	
	pfile->org_name = (char*)0;
	pfile->stream   = (void*)0;
	pfile->stream_type = ISEEIO_CONFER_INVALID;

#ifdef _DEBUG
	__isfile_object_counts++;
#endif

	return pfile;
}


/* �ͷ�һ��ISFILE�ṹ */
ISFILE * CALLAGREEMENT _free_ISFILE(ISFILE *pfile)
{
	assert(pfile);				/* ���ϸ����ڲ������ */

	if (!pfile)
		return (ISFILE*)0;

	assert(!strcmp((const char *)pfile->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	free(pfile);

#ifdef _DEBUG
	__isfile_object_counts--;
#endif
	
	return (ISFILE*)0;
}


/* ����ר�ã���ȡISFILEʵ��ĸ��� */
#ifdef _DEBUG
ISEEIO_API int CALLAGREEMENT __get_object_counts(void)
{
	return __isfile_object_counts;
}
#endif /*_DEBUG*/


