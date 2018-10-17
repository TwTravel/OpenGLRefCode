/********************************************************************

	iseemem.c

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


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* 缩短windows.h文件的编译时间 */
#define _WIN32_WINNT 0x0400				/* IsDebuggerPresent()函数声明 */
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "iseemem.h"
#include "allolink.h"
#include "freelink.h"


#ifdef WIN32
CRITICAL_SECTION	im_critical;
#endif



/******************************************************************************/
/* 全局变量区 																  */
/******************************************************************************/
static unsigned char	*im_start_addr    = 0;		/* 防护内存空间的基地址 */
static int				im_page_size      = 0;		/* 虚拟内存页的尺寸 */

static HANDLE			im_info_heap      = 0;		/* 节点堆句柄 */

static unsigned long	im_alloc_count    = 0;		/* 分配操作计次 */
static unsigned long	im_free_count     = 0;		/* 释放操作计次 */
static unsigned long	im_max_mem_dosage = 0;		/* 瞬间最大内存用量 */
static unsigned long	im_cur_mem_dosage = 0;		/* 当前内存用量 */

static unsigned long	im_tls_index      = 0;		/* 异常报告标志 */
static unsigned long	im_debuged        = 0;		/* 系统是否被调试器启动 */
static FILE				*im_log_handle    = 0;		/* 系统日志文件句柄 */

static unsigned char	im_log_filename[IM_LOG_FN_MAX_SIZE];	/* 系统日志文件全路径 */




/******************************************************************************/
/* 助手函数声明																  */
/******************************************************************************/
static int  CALLAGREEMENT _get_page_size();

static int  CALLAGREEMENT _alloc_reserves_area();
static void CALLAGREEMENT _free_reserves_area();

static int  CALLAGREEMENT _cal_page_count(int user_len);
static int  CALLAGREEMENT _alloc_pages(unsigned char *paddr, int pages);
static void CALLAGREEMENT _free_pages(unsigned char *paddr, int pages);

static int  CALLAGREEMENT _check_overflow(LPALLOLINKNODE p);
static char*CALLAGREEMENT _short_filename(char *fnfull);






/******************************************************************************/
/* 接口函数																	  */
/******************************************************************************/
#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	LPIMTHREADINFO	pti;
	int				result;
	char			*p;

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 获得系统虚拟内存页尺寸 */
			im_page_size  = _get_page_size();
			/* 判断系统是否是被调试器启动的（非0为调试器启动，0为自启动）*/
			im_debuged    = IsDebuggerPresent();

			assert(im_page_size);
			assert(im_info_heap == 0);

			/* 创建线程本地存储（TLS）索引，用于存放异常报告标志 */
			if ((im_tls_index=TlsAlloc()) == 0xFFFFFFFF)
			{
				im_output_debug_info("\n--->IN : 无法创建TLS，防护系统构建失败！\n\n");
                return FALSE; 
			}
			
			/* 分配信息节点区（堆） */
			/* ((IM_MAX_REVSIZE/im_page_size/2)*sizeof(ALLOLINKNODE))/4 的含义：/*
			/*      (IM_MAX_REVSIZE/im_page_size/2) ：最大分配节点数目（两页为一最小节点）*/
			/*      *sizeof(ALLOLINKNODE)           ：乘每个节点结构的尺寸 */
			/*      /4                              ：一般程序只能使用1/4的节点堆空间（如果 */
			/*                                        堆空间不足，堆管理器会自动扩张）*/
			if ((im_info_heap=HeapCreate(0, ((IM_MAX_REVSIZE/im_page_size/2)*sizeof(ALLOLINKNODE))/4, 0)) == 0)
			{
				im_output_debug_info("\n--->IN : 防护系统节点堆分配失败，系统无法运行！\n\n");
				TlsFree(im_tls_index);
				return FALSE;
			}

			/* 分配防护系统空间 */
			if (!_alloc_reserves_area())
			{
				im_output_debug_info("\n--->IN : 防护系统空间申请失败，系统无法运行！\n\n");
				TlsFree(im_tls_index);
				HeapDestroy(im_info_heap);
				return FALSE;
			}

			/* 如果是自启动，则建立日志文件 */
			if (im_debuged == 0)
			{
				/* 获取系统实体文件所在路径及文件名 */
				GetModuleFileName(hModule, im_log_filename, IM_LOG_FN_MAX_SIZE);

				/* 搜索全路径中最后一个'.'符号所在的位置 */
				p = strrchr((const char *)im_log_filename, (int)(unsigned int)(unsigned char)'.');
				/* 系统的实体文件必然会以.dll结尾，所以p必然不是0 */
				assert(p);
				
				*p = 0;

				/* 生成LOG文件名（实体文件.txt）*/
				strcat((char*)im_log_filename, ".txt");

				/* 创建日志文件 */
				im_log_handle = fopen((const char *)im_log_filename, "w");

				if (im_log_handle == 0)
				{
					_free_reserves_area();
					TlsFree(im_tls_index);
					HeapDestroy(im_info_heap);
					return FALSE;
				}
			}

			/* 初始化访问关键段 */
			InitializeCriticalSection(&im_critical);
			
			/* 建立空闲块链表 */
			imf_create(im_start_addr, im_page_size, im_info_heap);

			/* 建立分配块链表 */
			ima_create(im_start_addr, im_page_size, im_info_heap);

			im_output_debug_info("\n--->IN : 内存防护系统创建成功！\n\n");
			/* 此处不设 break，因为进程建立的时候将自动创建一个缺省线程 */
		case DLL_THREAD_ATTACH:
			/* 为新线程申请一个线程信息结构 */
			pti = (LPIMTHREADINFO)HeapAlloc(im_info_heap, 0, sizeof(IMTHREADINFO));

			/* 因为一个进程的线程数不可能太多，而且IMTHREADINFO结构的尺寸也不大，
			   所以如果连pti都无法申请成功，则防护系统此时也处于危险的状态。所以
			   此处用断言而不是返回假。*/
			assert(pti);

			/* 填写结构初始值 */
			pti->thread_id   = (unsigned long)GetCurrentThreadId();
			pti->except_mark = 0;
			pti->watch_curr  = 0;
			memset((void*)pti->watch_info, 0, sizeof(IMWATCHINFOSTR)*IM_WATCHINFONUM);

			/* 将线程结构地址保存到TLS索引中 */
			result = TlsSetValue(im_tls_index, (LPVOID)pti);

			/* 断言保存操作成功，否则说明操作系统已不正常 */
			assert(result);

			break;
		case DLL_THREAD_DETACH:
			/* 取回线程自己的信息结构 */
			pti = (LPIMTHREADINFO)TlsGetValue(im_tls_index);

			/* 断言必须能取回该结构，否则说明操作系统已不正常 */
			assert(pti);

			/* 释放线程信息结构 */
			HeapFree(im_info_heap, 0, (LPVOID)pti);

			break;
		case DLL_PROCESS_DETACH:
			/* 检查是否存在内存泄漏 */
			if (ima_get_count())
			{
				LPALLOLINKNODE	p;

				im_output_debug_info("--->OUT: 防护系统检测到%d处内存泄漏！\n", ima_get_count());

				/* 释放泄漏的内存块 */
				while (p=ima_search(0))
				{
					assert((p->magic == IM_ALLOLINK_MAGIC)&&(p->guard_addr)&&(p->user_length));

					im_output_debug_info("         文件%s中第%d行代码分配的内存没有被释放！\n", _short_filename(p->filename), p->linenum);

					im_free(p->user_addr);
				}
			}

			/* 摧毁链表 */
			ima_destroy();
			imf_destroy();

			/* 释放系统空间 */
			_free_reserves_area();

			/* 取回缺省线程的信息结构 */
			pti = (LPIMTHREADINFO)TlsGetValue(im_tls_index);
			
			/* 断言必须能取回该结构，否则说明操作系统已不正常 */
			assert(pti);
			
			/* 释放缺省线程信息结构 */
			HeapFree(im_info_heap, 0, (LPVOID)pti);
			
			/* 释放线程本地存储索引 */
            TlsFree(im_tls_index); 
			
			/* 释放节点堆 */
			HeapDestroy(im_info_heap);

			/* 销毁访问关键段 */
			DeleteCriticalSection(&im_critical);
			
			/* 报告内存使用情况 */
			im_output_debug_info("\n--->OUT: 内存防护系统退出成功！\n");
			im_output_debug_info("         软件共进行分配操作：%d次\n", im_alloc_count);
			im_output_debug_info("                   释放操作：%d次\n", im_free_count);

			if (im_max_mem_dosage < 1024)
			{
				im_output_debug_info("           瞬间最大内存用量：%d字节\n\n", im_max_mem_dosage);
			}
			else if (im_max_mem_dosage < 1024*1024)
			{
				im_output_debug_info("           瞬间最大内存用量：%d KB (%d byte)\n\n", im_max_mem_dosage/1024, im_max_mem_dosage);
			}
			else
			{
				im_output_debug_info("           瞬间最大内存用量：%d MB (%d byte)\n\n", im_max_mem_dosage/(1024*1024), im_max_mem_dosage);
			}

			/* 关闭系统日志文件 */
			if (im_debuged == 0)
			{
				assert(im_log_handle);
				fclose(im_log_handle);
			}

			break;
    }

    return TRUE;

	UNREFERENCED_PARAMETER(lpReserved);
}
#endif	/* WIN32 */




/* malloc()对应体 */
ISEEMEM_API void * CALLAGREEMENT im_malloc_act(size_t size, char *fn, int ln)
{
	int				pages;
	unsigned char	*paddr;
	LPALLOLINKNODE	aln;

	EnterCriticalSection(&im_critical);

	/* 注：im_malloc不允许分配0字节的内存块，这一点与ANSI标准不同，不过
	 *     这更容易发现程序设计上的问题 */
	assert(size);		

	/* 计算所需页数 */
	pages = _cal_page_count(size);

	/* 前置防护页加上至少一页的数据 */
	assert(pages >= 2);
	
	/* 查询是否有足够的可用内存 */
	if ((paddr=imf_lookup(pages)) == 0)
	{
		LeaveCriticalSection(&im_critical);
		return (void*)0;
	}

	/* 分配内存页 */
	if (_alloc_pages(paddr, pages))
	{
		imf_reclaim(paddr, pages);
		LeaveCriticalSection(&im_critical);
		return (void*)0;
	}

	/* 加入已分配链表（缺省属性为可读写－0） */
	aln = ima_push(paddr, pages, size, 0, fn, ln);

	/* 填充废料 */
	memset((void*)aln->fg_addr, IM_SHIT_FILL, (aln->guard_length-1)*im_page_size); 
	
	/* 填写前防护结构 */
	aln->fg_addr->magic  = IM_GUARD_MAGIC;
	aln->fg_addr->f_guard= IM_GUARD_FILL;

	/* 填写后防护结构 */
	aln->bg_addr->b_guard= IM_GUARD_FILL;
	aln->bg_addr->magic  = IM_GUARD_MAGIC;

	im_alloc_count++;
	im_cur_mem_dosage += aln->user_length;

	/* 刷新瞬间最大内存用量值 */
	if (im_cur_mem_dosage > im_max_mem_dosage)
	{
		im_max_mem_dosage = im_cur_mem_dosage;
	}

	LeaveCriticalSection(&im_critical);

	return (void*)(aln->user_addr);
}


/* calloc()对应体 */
ISEEMEM_API void * CALLAGREEMENT im_calloc_act(size_t num, size_t size, char *fn, int ln)
{
	return im_malloc_act(num*size, fn, ln);
}


/* realloc()对应体 */
ISEEMEM_API void * CALLAGREEMENT im_realloc_act(void *pold, size_t newsize, char *fn, int ln)
{
	LPALLOLINKNODE	aln;
	unsigned char	*pb;

	/* 如果没有指定内存地址，则按malloc处理 */
	if (pold == 0)
	{
		return im_malloc_act(newsize, fn, ln);
	}

	/* 如果新尺寸为0，则按free处理 */
	if (newsize == 0)
	{
		im_free_act(pold, fn, ln);
		return (void*)0;
	}

	/* 分配新内存块（本函数将不伸长或缩短已有内存块，而是直接申请新的内存块）*/
	if ((pb=im_malloc_act(newsize, fn, ln)) == 0)
	{
		return (void*)0;	/* 分配失败，但老内存块仍然有效 */
	}

	/* 进入关键段 */
	EnterCriticalSection(&im_critical);

	/* 搜索老内存块的信息节点地址 */
	aln = ima_search((unsigned char *)pold);

	/* pold 必须是受防护的内存块（也就是说ima_search()函数必须能找到对应节点） */
	assert(aln);
	assert(aln->user_addr == (unsigned char *)pold);

	/* 复制老内存块中的数据到新内存块 */
	memcpy((void*)pb, (const void *)pold, min((size_t)aln->user_length,newsize));

	LeaveCriticalSection(&im_critical);

	/* 释放老内存块 */
	im_free_act(pold, fn, ln);

	return (void*)pb;
}


/* free()对应体 */
ISEEMEM_API void CALLAGREEMENT im_free_act(void *paddr, char *fn, int ln)
{
	LPALLOLINKNODE		aln;
	unsigned char		*pbase;
	int					pages;

	EnterCriticalSection(&im_critical);

	/* 获取对应节点地址 */
	aln = ima_search((unsigned char *)paddr);

	/* paddr 必须是受防护的内存块（也就是说ima_search()函数必须能找到对应节点） */
	/* 或者程序在试图释放一个已被释放过的内存块 */
	assert(aln);

	pbase = aln->guard_addr;
	pages = aln->guard_length;

	/* 写保护状态检测 */
	if (aln->attrib != 0)
	{
		im_output_debug_info("--->INF: %s中第%d行代码分配的内存在没有解除写保护之前就被释放了，不知程序设计上是否正确？\n", _short_filename(aln->filename), aln->linenum);
	}

	/* 进行溢出检测 */
	switch (_check_overflow(aln))
	{
	case	0:			/* 无溢出现象 */
		break;
	case	1:			/* 前溢出 */
		im_output_debug_info("--->FRE: [前溢出] 文件%s中第%d行代码分配的内存发生了前写溢出！\n", _short_filename(aln->filename), aln->linenum);
		break;
	case	2:			/* 后溢出 */
		im_output_debug_info("--->FRE: [后溢出] 文件%s中第%d行代码分配的内存发生了后写溢出！\n", _short_filename(aln->filename), aln->linenum);
		break;
	case	3:			/* 前、后均溢出 */
		im_output_debug_info("--->FRE: [前、后溢出] 文件%s中第%d行代码分配的内存发生了前、后写溢出！\n", _short_filename(aln->filename), aln->linenum);
		break;
	default:
		im_output_debug_info("--->FRE: [操作系统异常] 操作系统处于不稳定状态，建议退出程序！\n");
		assert(0);		/* 系统异常 */
		break;
	}

	im_free_count++;
	im_cur_mem_dosage -= aln->user_length;
		
	/* 从已分配链表中删除节点 */
	ima_pop(aln);

	/* 释放页面 */
	_free_pages(pbase, pages);

	/* 加入空闲链表 */
	imf_reclaim(pbase, pages);

	LeaveCriticalSection(&im_critical);
}


/* 输出调试信息 */
ISEEMEM_API void CALLAGREEMENT im_output_debug_info(const char *pstr, ...)
{
	static char buf[IM_DBGINFO_MAX_SIZE];
	va_list		va;

	va_start(va, pstr);
	vsprintf(buf, pstr, va);
	va_end(va);

	if (im_debuged == 0)
	{
		/* 将信息写入系统日志文件 */
		fputs((const char *)buf, im_log_handle);
	}
	else
	{
		/* 将信息写入调试器窗口 */
		OutputDebugString((const char *)buf);
	}
}


/* 内存访问异常缺省处理函数 */
ISEEMEM_API unsigned long CALLAGREEMENT im_execpt_fiter(LPEXCEPTION_POINTERS per, long bln)
{
	LPIMTHREADINFO	pti;
	LPALLOLINKNODE	pa;
	unsigned char	*paddr, *fn;
	long			fln;
	LPIMWATCHINFOSTR	pwis;


	/* 非内存访问异常将不被处理 */
	if (per->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/* 获取线程信息结构 */
	pti = im_get_tls();

	assert(pti);

	/* 如果是已被本函数处理过的异常，则不再进行分析、显示 */
	if (pti->except_mark & IM_EXCEPT_MARK_EXCED)
	{
		im_watch_pop();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	EnterCriticalSection(&im_critical);

	/* 内存访问异常记录中应该存在两个参量的附加信息单元 */
	assert(per->ExceptionRecord->NumberParameters == 2);

	/* 提取当前监测块信息节点地址 */
	pwis = &(pti->watch_info[pti->watch_curr-1]);

	fn  = pwis->fn;
	fln	= pwis->linenum;

	assert(fn);
	
	/* 提取发生访问异常的虚拟地址 */
	paddr = (unsigned char *)(per->ExceptionRecord->ExceptionInformation[1]);

	im_output_debug_info("--->WAT: 地址%p发生了%s错误！\n", paddr, (per->ExceptionRecord->ExceptionInformation[0] == 0) ? "读" : "写");

	/* 受控地址检测 */
	if ((paddr >= im_start_addr)&&(paddr < (im_start_addr+IM_MAX_REVSIZE)))
	{
		pa = ima_search(0);

		/* 遍历整个已分配链表 */
		while (pa)
		{
			/* 异常发生在防护页的后半部，则最有可能是当前单元前溢出 */
			if ((paddr >= (pa->guard_addr+im_page_size/2))&&(paddr < (unsigned char *)(pa->fg_addr)))
			{
				im_output_debug_info("         [前越界] 这可能是由于%s中%d～%d行之间的某些受控代码，在向文件%s中第%d行源码分配的内存中%s数据时，发生了前越界现象，请注意调试器给出的异常定位点。\n", 
					_short_filename(fn), fln, bln, 
					_short_filename(pa->filename), pa->linenum, 
					(per->ExceptionRecord->ExceptionInformation[0] == 0) ? "读取" : "写入");
				break;
			}
			/* 如果是在下一个防护页的前半部，则可能是后溢出 */
			else if ((paddr >= (pa->guard_addr+pa->guard_length*im_page_size))&&(paddr <= (pa->guard_addr+pa->guard_length*im_page_size+im_page_size/2)))
			{
				im_output_debug_info("         [后越界] 这可能是由于%s中%d～%d行之间的某些受控代码，在向文件%s中第%d行源码分配的内存中%s数据时，发生了后越界现象，请注意调试器给出的异常定位点。\n", 
					_short_filename(fn), fln, bln, 
					_short_filename(pa->filename), pa->linenum, 
					(per->ExceptionRecord->ExceptionInformation[0] == 0) ? "读取" : "写入");
				break;
			}
			/* 受控代码试图写受保护的内存块 */
			else if ((paddr >= (unsigned char *)pa->fg_addr)&&(paddr < ((unsigned char *)pa->fg_addr+(pa->guard_length-1)*im_page_size)))
			{
				if (pa->attrib != 0)
				{
					im_output_debug_info("         [写保护] 文件%s中%d～%d行之间的某些受控代码，试图向受保护的内存中（%s第%d行源码分配的内存）写入数据，请检查受控代码，同时请参考调试器给出的异常定位点。\n", 
						_short_filename(fn), fln, bln, 
						_short_filename(pa->filename), pa->linenum, 
						(per->ExceptionRecord->ExceptionInformation[0] == 0) ? "读取" : "写入");
				}
				else
				{
					im_output_debug_info("         [未知原因的异常] 文件%s中%d～%d行之间的某些受控代码在操作%s第%d行源码分配的内存时出现了异常情况，防护系统暂时无法判断引发该问题的原因，建议仔细查看受控代码！\n", 
						_short_filename(fn), fln, bln, 
						_short_filename(pa->filename), pa->linenum);
				}
				break;
			}
			else
			{
				pa = pa->next;
			}
		}

		if (!pa)
		{
			/* 输出信息 */
			im_output_debug_info("         [读写已释放的内存] 该问题可能是由%s中%d～%d行之间的某些受控代码，试图对已释放的内存进行操作（读或写）引起的。建议检查受控代码中使用了哪些已被释放的内存。\n", _short_filename(fn), fln, bln);
		}
	}
	else
	{
		/* 输出信息 */
		im_output_debug_info("         [读写系统和程序内存区] 发生异常的地址不在防护系统的内存中，请注意查看%s中%d～%d行之间的受控代码，同时也请参考调试器给出的异常定位点。\n", _short_filename(fn), fln, bln);
	}

	/* 置处理标记 */
	pti->except_mark |= IM_EXCEPT_MARK_EXCED;

	im_watch_pop();
		
	LeaveCriticalSection(&im_critical);

	/* 继续异常，以使调试器定位具体异常发生的代码行 */
	return EXCEPTION_CONTINUE_SEARCH;
}


/* 全面溢出检测 */
ISEEMEM_API void CALLAGREEMENT im_check_all(long bln, int mark)
{
	LPALLOLINKNODE	pa;
	LPIMTHREADINFO	pti;
	unsigned char	*fn;
	long			fln;
	LPIMWATCHINFOSTR	pwis;
	
	EnterCriticalSection(&im_critical);

	/* 获取线程信息结构 */
	pti = im_get_tls();
	
	assert(pti);

	/* 提取当前监测块信息节点地址 */
	pwis = &(pti->watch_info[pti->watch_curr-1]);
	
	fn  = pwis->fn;
	fln	= pwis->linenum;
	
	assert(fn&&fln&&bln);

	pa = ima_search(0);	/* 获得已分配链表头节点地址 */

	while (pa)
	{
		switch (_check_overflow(pa))
		{
		case	0:			/* 无溢出现象 */
			break;
		case	1:			/* 前溢出 */
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [前溢出] 在进入%s第%d行代码处的防护语句块前，防护系统侦测到%s中第%d行代码分配的内存发生了前写溢出！请核查。\n", _short_filename(fn), fln, _short_filename(pa->filename), pa->linenum);
			}
			else
			{
				im_output_debug_info("--->WAT: [前溢出] 文件%s中第%d行代码分配的内存发生了前写溢出！引发溢出的代码在%s第%d～%d行之间，请仔细核查。\n", _short_filename(pa->filename), pa->linenum, _short_filename(fn), fln, bln);
			}
			break;
		case	2:			/* 后溢出 */
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [后溢出] 在进入%s第%d行代码处的防护语句块前，防护系统侦测到%s中第%d行代码分配的内存发生了后写溢出！请核查。\n", _short_filename(fn), fln, _short_filename(pa->filename), pa->linenum);
			}
			else
			{
				im_output_debug_info("--->WAT: [后溢出] 文件%s中第%d行代码分配的内存发生了后写溢出！引发溢出的代码在%s第%d～%d行之间，请仔细核查。\n", _short_filename(pa->filename), pa->linenum, _short_filename(fn), fln, bln);
			}
			break;
		case	3:			/* 前、后均溢出 */
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [前、后溢出] 在进入%s第%d行代码处的防护语句块前，防护系统侦测到%s中第%d行代码分配的内存发生了前、后写溢出！请核查。\n", _short_filename(fn), fln, _short_filename(pa->filename), pa->linenum);
			}
			else
			{
				im_output_debug_info("--->WAT: [前、后溢出] 文件%s中第%d行代码分配的内存发生了前、后写溢出！引发溢出的代码在%s第%d～%d行之间，请仔细核查。\n", _short_filename(pa->filename), pa->linenum, _short_filename(fn), fln, bln);
			}
			break;
		default:
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [操作系统异常] 在进入%s第%d行代码处的防护语句块前，防护系统察觉操作系统似乎处于不稳定状态，建议退出程序！\n", _short_filename(fn), fln);
			}
			else
			{
				im_output_debug_info("--->WAT: [操作系统异常] 操作系统处于不稳定状态，建议退出程序！（检测点位于%s第%d～%d行）\n", _short_filename(fn), fln, bln);
			}
			assert(0);		/* 系统异常 */
			break;
		}
		pa = pa->next;
	}

	LeaveCriticalSection(&im_critical);
}


/* 保护指定的内存块（将该内存块的属性设为只读）*/
ISEEMEM_API void CALLAGREEMENT im_protect(unsigned char *puser)
{
	LPALLOLINKNODE	pa;
	int				rs;
	unsigned long	oldprot;

	assert(puser);

	EnterCriticalSection(&im_critical);

	/* 搜索内存块的节点信息 */
	pa = ima_search(puser);

	assert(pa);				/* 指定的内存必须是防护系统分配的内存，否则将引发此断言 */

	if (pa->attrib != 0)	/* 该内存块已处于受保护状态 */
	{
		pa->attrib++;		/* 防护计数加一 */
		LeaveCriticalSection(&im_critical);
		return;
	}

	/* 将内存块访问属性置为只读 */
	rs = (int)VirtualProtect((void*)(pa->fg_addr), pa->begin_length, PAGE_READONLY, &oldprot);

	assert(rs);				/* 函数必须成功，否则说明系统已不稳定 */

	pa->attrib = 1;

	LeaveCriticalSection(&im_critical);
}


/* 解保护指定内存块（将该内存块的属性设为读写）*/
ISEEMEM_API void CALLAGREEMENT im_unprotect(unsigned char *puser)
{
	LPALLOLINKNODE	pa;
	int				rs;
	unsigned long	oldprot;
	
	assert(puser);
	/* 用户指定的受保护内存必须是防护系统分配的内存 */
	assert((puser>=im_start_addr)&&(puser<(im_start_addr+IM_MAX_REVSIZE)));

	EnterCriticalSection(&im_critical);
	
	/* 搜索内存块的节点信息 */
	pa = ima_search(puser);

	/* 如果找不到该内存对应的节点，则说明用户可能提前释放了该内存块 */
	if (!pa)
	{
		LeaveCriticalSection(&im_critical);
		return;
	}
	
	if (pa->attrib == 0)	/* 该内存块并未处于受保护状态 */
	{
		LeaveCriticalSection(&im_critical);
		return;
	}
	else if (pa->attrib > 1)
	{
		pa->attrib--;		/* 防护计数减一，内存块仍处于受保护状态 */
		LeaveCriticalSection(&im_critical);
		return;
	}
	
	/* 将内存块访问属性置为可读写 */
	rs = (int)VirtualProtect((void*)(pa->fg_addr), pa->begin_length, PAGE_READWRITE, &oldprot);
	
	assert(rs);				/* 函数必须成功，否则说明系统已不稳定 */
	
	pa->attrib = 0;			/* 内存块为可读写状态 */
	
	LeaveCriticalSection(&im_critical);
}


/* 获取线程本地存储（TLS）对象地址（也即线程自身的信息结构地址）*/
ISEEMEM_API LPIMTHREADINFO CALLAGREEMENT im_get_tls()
{
	LPIMTHREADINFO	pti;

	assert(im_tls_index);

	/* 取回线程自己的信息结构 */
	pti = (LPIMTHREADINFO)TlsGetValue(im_tls_index);
	
	/* 断言必须能取回该结构，否则说明操作系统已不正常 */
	assert(pti);
	
	return pti;
}
	

/* 开始一个监测块（压入块的相关信息）*/
ISEEMEM_API void CALLAGREEMENT im_watch_push(char *fn, long line)
{
	LPIMTHREADINFO pti = im_get_tls();

	/* 如果没有取出线程私有数据，则说明操作系统已不稳定 */
	assert(pti);
	/* 如果触发了此断言，则说明用户的watch{}end块嵌套层次过多 */
	assert(pti->watch_curr < IM_WATCHINFONUM);
	/* 初始化节点变量 */
	memset((void*)&(pti->watch_info[pti->watch_curr]), 0, sizeof(IMWATCHINFOSTR));

	pti->watch_info[pti->watch_curr].fn      = fn;
	pti->watch_info[pti->watch_curr].linenum = line;

	/* 重置异常报告标志 */
	pti->except_mark = IM_EXCEPT_MARK_INIT;

	/* 当前指针加一（类似堆栈指针）*/
	pti->watch_curr++;
}

	
/* 结束一个监测块（可能从异常过滤器中调用，也可能从监测块尾调用）*/
ISEEMEM_API void CALLAGREEMENT im_watch_pop()
{
	LPIMTHREADINFO pti = im_get_tls();

	/* 如果没有取出线程私有数据，则说明操作系统已不稳定 */
	assert(pti);
	/* 如果触发了此断言，则说明用户的watch{}end语句未配套出现（没写watch而直接写了end）*/
	assert(pti->watch_curr > 0);

	/* 当前指针减一，指向最近的一个监测块信息节点 */
	pti->watch_curr--;

	/* 解除保护 */
	while (pti->watch_info[pti->watch_curr].prot_count--)
	{
		im_unprotect(pti->watch_info[pti->watch_curr].prot_addr[pti->watch_info[pti->watch_curr].prot_count]);
	}
}


/* 保护指定内存块（将该内存块的属性设为只读）*/
ISEEMEM_API void CALLAGREEMENT im_watch_protect(void *p1, ...)
{
	char			*p = (char*)p1;
	va_list			va;
	int				watcurr, i;
	LPIMTHREADINFO	pti = im_get_tls();
	
	/* 如果没有取出线程私有数据，则说明操作系统已不稳定 */
	assert(pti);
	/* 如果触发了此断言，则说明用户的watch{}end语句未配套出现（没写watch）*/
	assert(pti->watch_curr > 0);
	
	/* 获取最近的一个监测块信息节点号 */
	watcurr = pti->watch_curr - 1;
	/* 如果触发了此断言，则说明用户在一个watch{}end块中保护了太多的内存块 */
	assert(pti->watch_info[watcurr].prot_count < IM_MAX_PROTECT);

	va_start(va, p1);
	while (p != 0)
	{
		/* 搜索该内存块在本次watch{}end块是否已被保护 */
		for (i=0; i<(int)(pti->watch_info[watcurr].prot_count); i++)
		{
			if (pti->watch_info[watcurr].prot_addr[i] == (unsigned char *)p)
			{
				break;
			}
		}

		/* 如果该内存块已被保护，则忽略此次操作，否则加入 */
		if (i == (int)(pti->watch_info[watcurr].prot_count))
		{
			/* 如果触发了此断言，则说明用户在一个watch{}end块中保护了太多的内存块 */
			assert(i < IM_MAX_PROTECT);

			im_protect((unsigned char *)p);

			pti->watch_info[watcurr].prot_addr[i] = (unsigned char *)p;
			pti->watch_info[watcurr].prot_count++;
		}

		/* 获取下一个指针参数 */
		p = va_arg(va, char*);
	}
	va_end(va);
}


/* 申请保留地 */
static int CALLAGREEMENT _alloc_reserves_area()
{
	assert(im_start_addr == 0);		/* 不能重复调用本函数 */

	im_start_addr = (unsigned char *)VirtualAlloc(0, IM_MAX_REVSIZE, MEM_RESERVE, PAGE_NOACCESS);

	return (im_start_addr != 0);
}


/* 释放保留地 */
static void CALLAGREEMENT _free_reserves_area()
{
	assert(im_start_addr);

	VirtualFree(im_start_addr, IM_MAX_REVSIZE, MEM_DECOMMIT);
	VirtualFree(im_start_addr, 0, MEM_RELEASE);
}


/* 获得虚拟内存页的尺寸 */
static int CALLAGREEMENT _get_page_size()
{
	SYSTEM_INFO		si;

	GetSystemInfo(&si);

	return (int)(si.dwPageSize);
}


/* 计算指定用户缓冲区尺寸的页数 */
static int CALLAGREEMENT _cal_page_count(int user_len)
{
	int length;

	assert((user_len)&&(user_len < IM_MAX_REVSIZE));

	/* 前防护结构 ＋ 用户数据 ＋ 后防护结构 */
	length = sizeof(FGUARDSTRUCT) + user_len + sizeof(BGUARDSTRUCT);
	/* 所需页数 */
	length = (length+im_page_size-1)/im_page_size;
	/* 加上前置防护页 */
	length += 1;

	assert(length);

	return length;
}


/* 分配指定尺寸的页缓冲区 */
static int CALLAGREEMENT _alloc_pages(unsigned char *paddr, int pages)
{
	MEMORY_BASIC_INFORMATION	mbi;
	unsigned char				*puser;

	assert(paddr&&(pages>=2));
	assert((paddr>=im_start_addr)&&(paddr<(im_start_addr+IM_MAX_REVSIZE)));

	/* 进行页面属性校验 */
	VirtualQuery(paddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	assert((mbi.State == MEM_RESERVE)&&((int)mbi.RegionSize >= pages*im_page_size));

	/* 提交页面（前防护页除外）*/
	puser = (unsigned char *)VirtualAlloc((void*)(unsigned char *)(paddr+im_page_size), 
											(pages-1)*im_page_size, 
											MEM_COMMIT, 
											PAGE_READWRITE);
	if (puser == 0)
	{
		return -1;		/* 内存不足 */
	}

	assert((unsigned char *)(paddr+im_page_size) == puser);

	return 0;			/* 成功 */
}


/* 释放指定尺寸的页缓冲区 */
static void CALLAGREEMENT _free_pages(unsigned char *paddr, int pages)
{
	int							rs;
	MEMORY_BASIC_INFORMATION	mbi;
	
	assert(paddr&&(pages>=2));
	assert((paddr>=im_start_addr)&&(paddr<(im_start_addr+IM_MAX_REVSIZE)));
	
	/* 进行页面属性校验 */
	VirtualQuery(paddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	assert((mbi.State == MEM_RESERVE)&&((int)mbi.RegionSize == im_page_size));
	VirtualQuery(paddr+im_page_size, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	assert((mbi.State == MEM_COMMIT)&&((int)mbi.RegionSize == (pages-1)*im_page_size));

	/* 释放页面 */
	rs = VirtualFree(paddr+im_page_size, (pages-1)*im_page_size, MEM_DECOMMIT);

	/* 如果释放失败，则说明操作系统发生了异常情况，无法继续运行，所以用断言提示开发员 */
	assert(rs);
}


/* 溢出检测 */
static int CALLAGREEMENT _check_overflow(LPALLOLINKNODE p)
{
	int rs = 0;
	unsigned long	ult;
	unsigned char	*pt;

	assert(p&&(p->magic == IM_ALLOLINK_MAGIC));

	if ((p->fg_addr->magic != IM_GUARD_MAGIC)||(p->fg_addr->f_guard != IM_GUARD_FILL))
	{
		rs |= 1;		/* 前溢出发生 */
	}

	if ((p->bg_addr->magic != IM_GUARD_MAGIC)||(p->bg_addr->b_guard != IM_GUARD_FILL))
	{
		rs |= 2;		/* 后溢出发生 */
	}

	/* 求最后一页剩余内存字节数 */
	ult = (p->guard_length-1)*im_page_size - p->begin_length;

	/* 最后一页剩余字节必然会小于页尺寸 */
	assert(ult < (unsigned long)im_page_size);

	if (ult)
	{
		pt = (unsigned char *)(((unsigned char *)p->fg_addr)+p->begin_length);
		assert(pt == (((unsigned char *)p->bg_addr)+sizeof(BGUARDSTRUCT)));

		while (ult--)
		{
			if (*pt++ != IM_SHIT_FILL)
			{
				rs |= 2;	/* 后溢出发生 */
				break;
			}
		}
	}
	
	return rs;			/* 0 表示没有异常发生 */
}


/* 取得全路径中的概略串地址 */
static char * CALLAGREEMENT _short_filename(char *fnfull)
{
	char *p;

	assert(fnfull);
	assert(strlen(fnfull) < MAX_PATH);

	/* 搜索全路径中最后一个'\'符号所在的位置 */
	p = strrchr((const char *)fnfull, (int)(unsigned int)(unsigned char)'\\');

	/* 全路径中必然会有至少一个'\'符号 */
	assert(p);

	/* 如果代码文件处于根目录，则只返回文件名 */
	if (*(p-1) == ':')
	{
		return (p+1);
	}
	else
	{
		/* 保留上一级的目录信息 */
		while (*(--p) != '\\');
		return (p+1);
	}
}


