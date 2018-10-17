/********************************************************************

	allolink.c

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
	本文件用途：	ISee图像浏览器—内存防护系统已分配内存链管理实现文件
	
	  
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20618
	最后修改于：	2002-6-18
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-6		第二版发布
		2002-5		第一版发布

********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "iseemem.h"
#include "allolink.h"




/******************************************************************************/
/* 全局变量区 																  */
/******************************************************************************/
static LPALLOLINKNODE	_im_allo_head = 0;				/* 链表头指针 */
static LPALLOLINKNODE	_im_allo_tail = 0;				/* 链表尾指针 */

static long				_im_allo_count= 0;				/* 链表节点计数 */

static int				_im_allo_pgnum= 0;				/* 页尺寸 */
static unsigned char	*_im_base_addr= 0;				/* 防护空间基地址 */
static HANDLE			_im_info_heap = 0;				/* 链表节点区句柄副本 */






/******************************************************************************/
/* 助手函数声明																  */
/******************************************************************************/
static LPALLOLINKNODE CALLAGREEMENT ima_alloc_node(void);
static void CALLAGREEMENT ima_free_node(LPALLOLINKNODE pnode);
static void CALLAGREEMENT ima_insert_node(LPALLOLINKNODE pnode);




/******************************************************************************/
/* 接口函数																	  */
/******************************************************************************/

/* 创建链表 */
void CALLAGREEMENT ima_create(unsigned char *pstart_addr, int page_size, HANDLE info_heap)
{
	/* 在同一进程内不允许重复调用本函数 */
	assert((_im_allo_head == 0)&&(_im_allo_tail == 0)&&(_im_allo_count == 0));
	assert((_im_info_heap == 0)&&(_im_allo_pgnum == 0)&&(_im_base_addr == 0));
	
	/* 设置页尺寸、信息区句柄副本、及防护系统基地址 */
	_im_allo_pgnum = page_size;
	_im_info_heap  = info_heap;
	_im_base_addr  = pstart_addr;
}


/* 销毁链表 */
void CALLAGREEMENT ima_destroy()
{
	/* 不允许发生内存泄漏 */
	assert((_im_allo_head == 0)&&(_im_allo_tail == 0)&&(_im_allo_count == 0));
}


/* 记录已分配单元信息 */
LPALLOLINKNODE CALLAGREEMENT ima_push(unsigned char *paddr, 
										 int page_count, 
										 int user_data_len,
										 long attrib,
										 unsigned char *fn,
										 unsigned long linenum)
{
	LPALLOLINKNODE	p;

	assert(paddr&&page_count&&user_data_len);

	/* 申请信息节点 */
	p = ima_alloc_node();

	/* 填写节点信息 */
	p->guard_addr   = paddr;
	p->guard_length = page_count;

	p->fg_addr      = (LPFGUARDSTRUCT)(unsigned char *)(paddr+IM_GUARD_PAGE_NUM*_im_allo_pgnum);
	p->user_addr    = ((unsigned char *)p->fg_addr)+sizeof(FGUARDSTRUCT);
	p->user_length  = user_data_len;
	p->bg_addr      = (LPBGUARDSTRUCT)(unsigned char *)(p->user_addr+user_data_len);

	p->begin_length = sizeof(FGUARDSTRUCT)+user_data_len+sizeof(BGUARDSTRUCT);
	p->attrib       = attrib;
	p->filename     = fn;
	p->linenum      = linenum;

	/* 将节点插入链表 */
	ima_insert_node(p);

	return p;
}


/* 获取指定指针的内存节点信息 */
LPALLOLINKNODE CALLAGREEMENT ima_search(unsigned char *puser_addr)
{
	LPALLOLINKNODE	p;

	/* 如果指定了0，则返回链表头块的地址 */
	if (puser_addr == 0)
	{
		return _im_allo_head;
	}

	/* 指定的内存地址不能是系统防护保护范围之外的内存 */
	assert((puser_addr >= (_im_base_addr+IM_GUARD_PAGE_NUM*_im_allo_pgnum+sizeof(FGUARDSTRUCT)))&&(puser_addr < (_im_base_addr+IM_MAX_REVSIZE-sizeof(BGUARDSTRUCT))));

	/* 查找该地址对应的信息节点 */
	for (p=_im_allo_head; p!=0; p=p->next)
	{
		if ((puser_addr >= p->user_addr)&&(puser_addr < (p->user_addr+p->user_length)))
		{
			break;
		}
	}

	return p;	/* 返回值有可能是0，这说明没有找到节点信息（传入的指针是一个非法指针）*/
}


/* 摘除并释放一个信息节点 */
void CALLAGREEMENT ima_pop(LPALLOLINKNODE pnode)
{
	assert(pnode&&(pnode->magic == IM_ALLOLINK_MAGIC));
	assert((pnode->guard_length)&&(pnode->guard_addr)&&(pnode->bg_addr)&&(pnode->begin_length));

	/* 摘除节点 */
	if (pnode->prev)
	{
		pnode->prev->next = pnode->next;
	}
	if (pnode->next)
	{
		pnode->next->prev = pnode->prev;
	}

	/* 更新头尾指针 */
	if (_im_allo_head == pnode)
	{
		_im_allo_head = pnode->next;
	}
	if (_im_allo_tail == pnode)
	{
		_im_allo_tail = pnode->prev;
	}

	/* 释放节点 */
	ima_free_node(pnode);

	_im_allo_count--;
}


/* 获取已分配链的节点计数 */
int CALLAGREEMENT ima_get_count()
{
	return _im_allo_count;
}


/* 申请一个节点 */
static LPALLOLINKNODE CALLAGREEMENT ima_alloc_node()
{
	LPALLOLINKNODE	p;
	
	assert(_im_info_heap);

	/* 分配节点 */
	p = (LPALLOLINKNODE)HeapAlloc(_im_info_heap, 0, sizeof(ALLOLINKNODE));

	/* 如果分配失败，则整个防护系统将无法继续运行，所以用断言提示开发员 */
	assert(p);

	memset((void*)p, 0, sizeof(ALLOLINKNODE));

	p->magic = IM_ALLOLINK_MAGIC;
	
	return p;
}


/* 释放一个节点 */
static void CALLAGREEMENT ima_free_node(LPALLOLINKNODE pnode)
{
	int rs;

	assert((pnode)&&(pnode->magic == IM_ALLOLINK_MAGIC));
	
	/* 填充垃圾信息 */
	memset((void*)pnode, IM_SHIT_FILL, sizeof(ALLOLINKNODE));

	/* 释放节点 */
	rs = HeapFree(_im_info_heap, 0, (void*)pnode);

	/* 如果释放失败，则整个防护系统将无法继续运行，所以用断言提示开发员 */
	assert(rs);
}


/* 在链表头部插入节点 */
static void CALLAGREEMENT ima_insert_node(LPALLOLINKNODE pnode)
{
#if 0
	LPALLOLINKNODE	p;
#endif

	assert(pnode&&(pnode->magic==IM_ALLOLINK_MAGIC)&&(pnode->user_length>0));

	if (_im_allo_head == 0)		/* 空链 */
	{
		assert((_im_allo_tail == 0)&&(_im_allo_count == 0));

		pnode->prev = pnode->next = 0;
		_im_allo_head = _im_allo_tail = pnode;
		_im_allo_count = 1;
	}
	else						/* 插入链表首部 */
	{
		assert(_im_allo_head&&_im_allo_count);

		pnode->prev = 0;
		pnode->next = _im_allo_head;
		_im_allo_head->prev = pnode;
		_im_allo_head = pnode;

		_im_allo_count++;

#if 0
		/* 下面的代码为按地址排序的插入方法（暂时不采用）*/

		/* 寻找插入点 */
		for (p=_im_allo_head; p!=0; p=p->next)
		{
			assert(p->guard_addr != pnode->guard_addr);

			if (p->guard_addr > pnode->guard_addr)
			{
				break;
			}
		}

		if (p)		/* 中、头部插入 */
		{
			pnode->next = p;
			pnode->prev = p->prev;

			if (p->prev)
			{
				p->prev->next = pnode;
			}
			p->prev = pnode;

			if (_im_allo_head == p)
			{
				_im_allo_head = pnode;
			}

			_im_allo_count++;
		}
		else		/* 尾部插入 */
		{
			pnode->prev = _im_allo_tail;
			pnode->next = 0;

			_im_allo_tail->next = pnode;
			_im_allo_tail = pnode;

			_im_allo_count++;
		}
#endif	/* 0 */
	}
}


