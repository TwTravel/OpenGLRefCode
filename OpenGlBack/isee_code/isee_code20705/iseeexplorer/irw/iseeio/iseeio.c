/********************************************************************

	iseeio.c

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
	本文件用途：	ISee I/O 实现文件（兼容标准：ANSI C 流I/O）
	本文件编写人：	YZ			yzfree##sina.com

	本文件版本：	20418
	最后修改于：	2002-4-18

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
		地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2002-4		修正文件缺省打开模式（定为二进制模式）
		2001-6		发布第一个测试版

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
#include <fcntl.h>	/* 设定文件打开模式时需要的一些宏 */
static int old_fmode;
#endif



/* 内部函数 */
ISEEIO_CONFER_TYPE	CALLAGREEMENT _get_confer_type(PISADDR confer_str);
unsigned long		CALLAGREEMENT _analyst_for_mode(const char *mode);
int					CALLAGREEMENT _add_memory_block(MEMORY_STREAM *memstream, int add_size);
int					CALLAGREEMENT _dec_memory_block(MEMORY_STREAM *memstream);
ISFILE *			CALLAGREEMENT _alloc_ISFILE(void);
ISFILE *			CALLAGREEMENT _free_ISFILE(ISFILE *pfile);
MEMORY_STREAM *		CALLAGREEMENT _alloc_MEMORY_STREAM(void);
MEMORY_STREAM *		CALLAGREEMENT _free_MEMORY_STREAM(MEMORY_STREAM *pmem);



/* ################################################################

	注：* 当前版本只实现了文件流和内存流的代码，网络流未实现。
		* ISIO流函数的设定原则是兼容ANSI C流函数，对于编译器厂商自己
		  扩展的流函数不在考虑范围之内。

   ################################################################*/



#ifdef WIN32
BOOL APIENTRY DllMain( HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
#if defined(_MSC_VER)
		old_fmode = _fmode;		/* 保存以前的文件打开模式 */
		_fmode = _O_BINARY;		/* 缺省文件打开模式设为二进制模式 */
#endif
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
#if defined(_MSC_VER)
		_fmode = old_fmode;		/* 恢复以前的文件打开模式 */
#endif
		break;
    }
	
    return TRUE;
}
#endif /* WIN32 */



/*
  功能：准备用于isio_open的流地址参数

  入口：
	PISADDR psct				- 待设的流结构

	ISEEIO_CONFER_TYPE type		- 流的类型。参见枚举类型ISEEIO_CONFER_TYPE。

	const char *name			- 流名称。  文件类型：指向路径串的指针
										    内存类型：指向内存块首地址的指针（该内存块必须是独立的，而不能是某一内存块中间的地址）
										    网络类型：－

	unsigned long param1		- 附加参数。文件类型：－
											内存类型：内存块的尺寸
											网络类型：－

	unsigned long param2		- 附加参数。文件类型：－
											内存类型：－
											网络类型：－


	返回：经强制转换的psct地址。
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

	/* 协议标志 "+X+" */
	psct->mark[0] = psct->mark[2] = '+';
	psct->mark[3] = '\0';

	switch (type)
	{
	case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
		psct->mark[1] = ISIO_STREAM_TYPE_LOC_FILE;
		break;
	case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
		psct->mark[1] = ISIO_STREAM_TYPE_LOC_MEMORY;
		break;
	case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
		psct->mark[1] = ISIO_STREAM_TYPE_ROT_INTERNET;
		break;
	default:									
		assert(0);								/* 暂不支持其它类型 */
		psct->mark[1] = ISIO_STREAM_TYPE_INVALID;
		break;
	}

	psct->name = (char*)name;
	psct->param1 = param1;
	psct->param2 = param2;

	return (const char *)psct;
}




/*
  功能：打开指定的ISIO流

  入口：
	pstream_name	－ 流名称结构地址：

	该参数可以是两种类型，一种是ISADDR结构地址（强制转换为char*），其内容
	含义如下：

				文件类型	内存类型	网络类型
	---------------------------------------------------------------------
	mark[4]		+F+			+M+			+I+

	*name		文件路径	内存块首	－
				串			地址。
							写属性时
							可不设置

	param1		－			内存块总	－
							长度值。
							写属性时
							可不设置
							
	param2		－			－			－
	---------------------------------------------------------------------
													注："－"表示未被使用

	另一种是char*型的文件路径串的地址，如："a:\a.txt"等等。
	
	mode			－ 打开流的方式：

	有效的组合如下：r,r+,rb,r+b,w,w+,wb,w+b,a,a+,ab,a+b，它们的含义可参见
	ANSI C中fopen的注释。

	
	注：当用isio_open打开一个内存流后，入口参数pstream_name所指向的ISADDR
		结构不允许删除，因为当流结束时，系统将向其中填写结果数据。详细说明
		参见isio_close函数。

  返回：  0 － 打开失败
		非0 － 打开成功，返回值可用于其它的ISIO函数
*/
ISEEIO_API ISFILE * CALLAGREEMENT isio_open(const char *pstream_name, const char *mode)
{
	PISADDR				pname = (PISADDR)pstream_name;
	ISFILE				*pfile = (ISFILE*)0;
	void				*pstream = (void*)0;
	MEMORY_STREAM		*pmemory;

	ISEEIO_CONFER_TYPE	stream_type;

	assert((pname)&&(mode));						/* 更严格的入口参数检查 */

	if ((!pname)||(!mode))
		return (ISFILE*)0;

	/* 判断指定流的协议类型 */
	if ((stream_type = _get_confer_type(pname)) == ISEEIO_CONFER_INVALID)
		return (ISFILE*)0;

	/* 申请ISFILE结构块 */
	if ((pfile = _alloc_ISFILE()) == (ISFILE*)0)
		return (ISFILE*)0;							/* 内存不足 */


	/* 根据协议类型的不同而打开相应的流 */
	switch (stream_type)
	{
		case	ISEEIO_NON_CONFER_LOC_FILE:			/* 非协议－本地文件 */
			if ((pstream = (void*)fopen(pstream_name, mode)) == (void*)0)
				return _free_ISFILE(pfile);

			pfile->stream = pstream;
			pfile->stream_type = ISEEIO_CONFER_LOC_FILE;
			pfile->org_name = (char*)pstream_name;
			return pfile;
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			if ((pstream = (void*)fopen((const char *)pname->name, mode)) == (void*)0)
				return _free_ISFILE(pfile);

			pfile->stream = pstream;
			pfile->stream_type = ISEEIO_CONFER_LOC_FILE;
			pfile->org_name = pname->name;
			return pfile;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			/* 申请内存流结构 */
			if ((pmemory = _alloc_MEMORY_STREAM()) == (void*)0)
				return _free_ISFILE(pfile);

			pfile->stream = (void*)pmemory;

			pmemory->mode = _analyst_for_mode(mode);
			
			if (pname->name)	/* 已经存在的内存流 */
			{
				pmemory->p_start_pos = pname->name;
				/* 如果以"w"或"w+"方式打开的内存流，则长度被截为0 */
				pmemory->len         = ((pmemory->mode & (ISIO_MODE_WRITE|ISIO_MODE_CREATE)) == (ISIO_MODE_WRITE|ISIO_MODE_CREATE)) ? 0 : pname->param1;
				pmemory->total_size  = pname->param1;
				pmemory->cur_pos     = 0;
				pmemory->error       = (pmemory->len == pmemory->cur_pos) ? ISIO_ERROR_EOF:0;
			}
			else				/* 不存在的内存流 */
			{
				if (!(pmemory->mode & ISIO_MODE_CREATE))
				{
					_free_MEMORY_STREAM((MEMORY_STREAM *)(pfile->stream));
					return _free_ISFILE(pfile);		/* 没有创建属性 */
				}

				/* 创建内存流（一个分配单位的容量）*/
				if (_add_memory_block(pmemory, 0) == -1)
				{
					_free_MEMORY_STREAM((MEMORY_STREAM *)(pfile->stream));
					return _free_ISFILE(pfile);		/* 内存不足 */
				}

				pmemory->len = 0UL;
				pmemory->cur_pos = 0UL;
				pmemory->error = ISIO_ERROR_EOF;
			}

			pfile->stream_type = ISEEIO_CONFER_LOC_MEMORY;
			pfile->org_name = (char*)pname;			/* 回写结构地址 */

			pname->name = ((MEMORY_STREAM*)pfile->stream)->p_start_pos;

			return pfile;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return (ISFILE*)0;
}




/*
  功能：关闭指定的ISIO流

  入口：
		stream	－ 由isio_open()函数打开的ISIO流句柄

	备注：

	isee I/O 流被关闭后，isio_close函数将向由isio_open函数传入的PISADDR结构
	填写必要的数据，具体数据如下：

				文件类型	内存类型	网络类型
	---------------------------------------------------------------------
	mark[4]		－			－			－
	*name		－			最后的内	－
							存块首地址
	param1		－			内存块内容	－
							长度值
	param2		－			内存块总长	－
							度
	---------------------------------------------------------------------
													注："－"表示未被改变
	
	内存类型备注：当打开一个内存流后，原来的内存块（如果指定了的话）将不能
		再使用，这个内存块很可能会被ISIO函数改变所在位置（尤其是向流中写数
		据的情况），只有关闭这个内存流后，在它的流名结构中返回的地址（name
		）才可以被安全的使用。


  返回：  0 － 关闭成功
		EOF － 关闭失败
*/
ISEEIO_API int CALLAGREEMENT isio_close(ISFILE *stream)
{
	int result;

	assert(stream);									/* 更严格的入口参数检查 */

	if (!stream)
		return EOF;

	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	/* 释放相应的流 */
	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			assert(stream->stream);
			result = fclose((FILE*)(stream->stream));
			_free_ISFILE(stream);
			return result;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			/* 压缩内存尾部的空余内存以节省空间 */
			_dec_memory_block((MEMORY_STREAM*)stream->stream);
			
			assert(((MEMORY_STREAM*)(stream->stream))->p_start_pos);

			/* 回写内存流信息 
			   
			   注意：* 在流被关闭以前，用于Open流的地址结构不能被删除。
			         * 内存流的内容块不会被Close（本函数）释放，它被传回调用者	
			*/
			((PISADDR)(stream->org_name))->name   = ((MEMORY_STREAM*)(stream->stream))->p_start_pos;	/* 内存块首地址 */
			((PISADDR)(stream->org_name))->param1 = ((MEMORY_STREAM*)(stream->stream))->len;			/* 内存块内容的尺寸 */
			((PISADDR)(stream->org_name))->param2 = ((MEMORY_STREAM*)(stream->stream))->total_size;		/* 内存块的实际尺寸 */

			/* 释放内存流结构 */
			_free_MEMORY_STREAM((MEMORY_STREAM *)(stream->stream));
			/* 释放流结构 */
			_free_ISFILE(stream);

			return 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			/* 释放内部数据。。。 */
			_free_ISFILE(stream);
			return 0;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}




/* 
  功能：从ISIO流读数据

  入口：
		buffer	－ 用于存放读入数据的缓冲区首地址
		size	－ 数据项的尺寸
		count	－ 数据项的个数
		stream	－ ISIO流句柄

  返回：成功读入的数据项个数
*/
ISEEIO_API size_t CALLAGREEMENT isio_read(void *buffer, size_t size, size_t count, ISFILE *stream)
{
	MEMORY_STREAM *pmemory;
	char *buf = (char*)buffer;
	int i, step;

	assert(buffer);								/* 更严格的入口参数检查 */
	assert(stream);

	if ((!stream)||(!buffer)||(!size)||(!count))
		return 0;

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));
	

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return fread(buffer, size, count, (FILE*)(stream->stream));
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)stream->stream;

			if (pmemory->error & ISIO_ERROR_EOF)
			{
				assert(pmemory->len == pmemory->cur_pos);
				return 0;							/* 已到达文件尾部 */
			}

			assert(pmemory->cur_pos < pmemory->len);

			/* 拷贝数据到指定的缓冲区 */
			for (i=0;i<(int)count;i++)
			{
				if ((pmemory->cur_pos+size) > pmemory->len)
					step = (int)(pmemory->len - pmemory->cur_pos);
				else
					step = size;

				memcpy((void*)buf, (const void *)(pmemory->p_start_pos+pmemory->cur_pos), step);

				/* 刷新流当前指针 */
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
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}




/* 
  功能：向ISIO流写数据

  入口：
		buffer	－ 待写的数据缓冲区首地址
		size	－ 数据项的尺寸
		count	－ 数据项的个数
		stream	－ ISIO流句柄

  返回：成功写入的数据项个数
*/
ISEEIO_API size_t CALLAGREEMENT isio_write(const void *buffer, size_t size, size_t count, ISFILE *stream)
{
	MEMORY_STREAM *pmemory;
	char *buf = (char*)buffer;
	int i, addsize;

	assert(buffer);								/* 更严格的入口参数检查 */
	assert(stream);

	if ((!stream)||(!buffer)||(!size)||(!count))
		return 0;

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return fwrite(buffer, size, count, (FILE*)stream->stream);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)stream->stream;

			/* 判断指定内存块是否有写（追加）权限 */
			if (!(pmemory->mode & (ISIO_MODE_WRITE|ISIO_MODE_APPEND)))
			{
				pmemory->error |= ISIO_ERROR_ERR;
				return 0;							/* 没有写权限 */
			}

			/* 追加型流的所有写操作的数据都追加到文件尾部 */
			if (pmemory->mode & ISIO_MODE_APPEND)
			{
				pmemory->cur_pos = pmemory->len;
				pmemory->error |= ISIO_ERROR_EOF;
			}

			/* 判断是否需要扩大内存块 */
			if ((pmemory->cur_pos+(unsigned long)(size*count)) > pmemory->total_size)
			{
				/* 为减轻系统重分配次数，此处作向大取整运算 */
				addsize = (pmemory->cur_pos+size*count)-pmemory->total_size;
				addsize = ((addsize + (MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP-1))/(MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP))*(MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP);
				if (_add_memory_block(pmemory, addsize) == -1)/* 增加内存块尺寸 */
				{
					pmemory->error |= ISIO_ERROR_ERR;
					return 0;			/* 内存不足 */
				}
				assert((pmemory->cur_pos+(unsigned long)(size*count)) <= pmemory->total_size);
			}

			/* 拷贝数据到指定的缓冲区 */
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
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}



/* 
  功能：刷新ISIO流（注：内存流没有刷新特性）

  入口：
		stream	－ ISIO流句柄

  返回： 0  － 刷新成功
		EOF － 失败
*/
ISEEIO_API int CALLAGREEMENT isio_flush(ISFILE *stream)
{
	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return fflush((FILE*)(stream->stream));
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			return 0;	/* 无缓冲操作，不用刷新 */
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}




/* 
  功能：ISIO流指针定位（注：任意位置）

  入口：
		stream	－ ISIO流句柄
		offset	－ 相对于origin的偏移量（以字节为单位）
		origin	－ 定位用的参考点（SEEK_SET、SEEK_CUR、SEEK_END）

  返回： 0 － 定位成功
		-1 － 失败
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
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return fseek((FILE*)stream->stream, offset, origin);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
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
				assert(0);							/* 软件设计错误 */
				break;
			}

			if (new_off > pmemory->len)				/* 非法位置，当前位置不改变 */
				return -1;

			if (new_off == pmemory->len)
				pmemory->error |= ISIO_ERROR_EOF;
			else
				pmemory->error &= ~(ISIO_ERROR_EOF);

			pmemory->cur_pos = new_off;

			return 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}




/* 
  功能：ISIO流指针定位（注：定位在流头部）

  入口：
		stream	－ ISIO流句柄

  返回：－
*/

ISEEIO_API void CALLAGREEMENT isio_rewind(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			rewind((FILE*)(stream->stream));
			return;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)stream->stream;

			assert(pmemory->p_start_pos);

			pmemory->cur_pos = 0UL;

			if (pmemory->cur_pos == pmemory->len)
				pmemory->error |= ISIO_ERROR_EOF;
			else
				pmemory->error &= ~(ISIO_ERROR_EOF);
			
			return;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return;
}




/* 
  功能：获取ISIO流当前指针位置

  入口：
		stream	－ ISIO流句柄

  返回：  -1 － 函数执行失败
		非-1 － 位置值
*/
ISEEIO_API long CALLAGREEMENT isio_tell(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return ftell((FILE*)(stream->stream));
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)stream->stream;

			assert(pmemory->p_start_pos);

			return (long)pmemory->cur_pos;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return (long)-1;
}




/* 
  功能：判断ISIO流当前指针是否在流的尾部

  入口：
		stream	－ ISIO流句柄

  返回：  0 － 不在尾部
		非0 － 在尾部
*/
ISEEIO_API int CALLAGREEMENT isio_eof(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return feof((FILE*)stream->stream);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)(stream->stream);

			assert(pmemory->p_start_pos);			/* 内存块必须存在 */

			return (pmemory->error & ISIO_ERROR_EOF) ? ISIO_ERROR_EOF : 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}




/* 
  功能：判断ISIO流是否发生了错误

  入口：
		stream	－ ISIO流句柄

  返回：  0 － 没有错误
		非0 － 有错误
*/
ISEEIO_API int CALLAGREEMENT isio_error(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			return ferror((FILE*)stream->stream);
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)(stream->stream);

			assert(pmemory->p_start_pos);			/* 内存块必须存在 */

			return (pmemory->error & ISIO_ERROR_ERR) ? ISIO_ERROR_ERR : 0;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return 0;
}




/* 
  功能：清除ISIO流的错误标志及EOF标志

  入口：
		stream	－ ISIO流句柄

  返回：－
*/
ISEEIO_API void CALLAGREEMENT isio_clearerr(ISFILE *stream)
{
	MEMORY_STREAM *pmemory;

	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	switch (stream->stream_type)
	{
		case	ISEEIO_CONFER_LOC_FILE:				/* 本地文件类型 */
			clearerr((FILE*)(stream->stream));
			return;
		case	ISEEIO_CONFER_LOC_MEMORY:			/* 本地内存类型 */
			pmemory = (MEMORY_STREAM *)(stream->stream);

			assert(pmemory->p_start_pos);			/* 内存块必须存在 */

			pmemory->error &= ~(ISIO_ERROR_ERR);	/* 清除错误指示符 */

			if (pmemory->cur_pos == pmemory->len)	/* 重置错误标志 */
				pmemory->error |= ISIO_ERROR_EOF;
			else
				pmemory->error &= ~ISIO_ERROR_EOF;

			return;
		case	ISEEIO_CONFER_ROT_INTERNET:			/* 远程internet */
			break;
		default:
			assert(0);								/* 无效的协议类型 */
			break;
	}

	return;
}




/* 
  功能：取得ISIO流的长度 

  入口：
		stream	－ ISIO流句柄

  返回：流长度（以字节为单位）
*/
ISEEIO_API unsigned long CALLAGREEMENT isio_length(ISFILE *stream)
{
	long pos;
	unsigned long len;
	
	assert(stream);

	assert(stream->stream);
	assert(!strcmp((const char *)stream->dbg_iseeio_stream_id, ISFILE_DBG_ID));
	
	/* 取得流长度后，流的当前指针不改变 */
	pos = isio_tell(stream);

	isio_seek(stream, 0, SEEK_END);
	len = (unsigned long)isio_tell(stream);

	isio_seek(stream, pos, SEEK_SET);

	return len;
}





/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/* 系统内部函数 */


/* 判断协议类型 */
ISEEIO_CONFER_TYPE CALLAGREEMENT _get_confer_type(PISADDR confer_str)
{
	assert(confer_str);	/* 必须存在入口参数串 */

	/* 判断是否是协议串 */
	if ((confer_str->mark[0] == '+')&&
		(confer_str->mark[2] == '+')&&
		(confer_str->mark[3] == '\0'))
	{
		switch(confer_str->mark[1])
		{
		case	ISIO_STREAM_TYPE_LOC_FILE:		/* file 类型 */
			return ISEEIO_CONFER_LOC_FILE;
		case	ISIO_STREAM_TYPE_LOC_MEMORY:	/* memory 类型 */
			return ISEEIO_CONFER_LOC_MEMORY;
		case	ISIO_STREAM_TYPE_ROT_INTERNET:	/* internet 类型 */
			return ISEEIO_CONFER_ROT_INTERNET;
		/* case	ISIO_STREAM_TYPE_XXX_XXXX:
			return ISEEIO_CONFER_XXXX; */
		default:
			assert(0);	/* 非法类型 */
			return ISEEIO_CONFER_INVALID;
		}
	}

	if (confer_str->mark[0] == '\0')
		return ISEEIO_CONFER_INVALID;
	
	return ISEEIO_NON_CONFER_LOC_FILE;		/* 非协议－本地文件名串 */
}


/* 解析ISIO流Open模式 */
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
		assert(0);						/* 非法的模式标识符 ANSI (r/w/a)*/
		return 0UL;
    }

	result |= ISIO_MODE_BIN;			/* ANSI 流模式为二进制 */

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
			assert(0);					/* 非法的模式标识符 */
			count = 2;
			break;
		}

		/* 模式串最大长度为3个字符，如："r+b"等等 */
		if (++count >= 2)
			break;
	}

	return result;
}


/* 为内存流增加空间（增加的尺寸add_size）*/
int CALLAGREEMENT _add_memory_block(MEMORY_STREAM *memstream, int add_size)
{
	char *ptmp;
	int new_size;

	assert(memstream);
	assert(memstream->mode & ISIO_MODE_CREATE);

	/* 如果add_size被指定为0，则函数增加系统缺省的内存尺寸：MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP */
	new_size = memstream->total_size + ((add_size == 0) ? (MEMORY_STREAM_UP_SIZE*MEMORY_STREAM_UP_STEP) : add_size);

	ptmp = (char*)realloc((void*)(memstream->p_start_pos), new_size);

	if (!ptmp)
		return -1;			/* 内存不足 */

	memstream->p_start_pos = ptmp;
	memstream->total_size  = new_size;

	return 0;				/* 成功 */
}


/* 释放内存块尾部的空闲内存 */
int CALLAGREEMENT _dec_memory_block(MEMORY_STREAM *memstream)
{
	char *ptmp;

	assert(memstream);
	assert(memstream->total_size >= memstream->len);

	if ((memstream->p_start_pos)&&(memstream->total_size > memstream->len)) 
	{
		ptmp = (char*)realloc((void*)(memstream->p_start_pos), memstream->len);
		if (!ptmp)
			return -1;		/* realloc函数失败（似乎不可能:） */
		memstream->p_start_pos = ptmp;
		memstream->total_size  = memstream->len;
		return 0;			/* 成功 */
	}

	return 1;				/* 没有空闲的内存区域 */
}


/* 申请一个新的内存流结构 */
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


/* 释放一个内存流结构（该函数并不释放流对应当内存块） */
MEMORY_STREAM * CALLAGREEMENT _free_MEMORY_STREAM(MEMORY_STREAM *pmem)
{
	assert(pmem);				/* 更严格的入口参数检查 */

	free(pmem);

	return (MEMORY_STREAM*)0;
}


/* 申请一个新的ISFILE结构 */
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


/* 释放一个ISFILE结构 */
ISFILE * CALLAGREEMENT _free_ISFILE(ISFILE *pfile)
{
	assert(pfile);				/* 更严格的入口参数检查 */

	if (!pfile)
		return (ISFILE*)0;

	assert(!strcmp((const char *)pfile->dbg_iseeio_stream_id, ISFILE_DBG_ID));

	free(pfile);

#ifdef _DEBUG
	__isfile_object_counts--;
#endif
	
	return (ISFILE*)0;
}


/* 调试专用：获取ISFILE实体的个数 */
#ifdef _DEBUG
ISEEIO_API int CALLAGREEMENT __get_object_counts(void)
{
	return __isfile_object_counts;
}
#endif /*_DEBUG*/


