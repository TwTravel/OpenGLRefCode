/********************************************************************

	freelink.c

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
	本文件用途：	ISee图像浏览器—内存防护系统空闲内存链管理实现文件
	
	  
	本文件编写人：	
					YZ			yzfree##yeah.net
		
	本文件版本：	20628
	最后修改于：	2002-6-28
		  
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
#include "freelink.h"



/******************************************************************************/
/* 全局变量区 																  */
/******************************************************************************/
static LPFREELINKNODE	_im_free_head = 0;				/* 链表头指针 */
static LPFREELINKNODE	_im_free_tail = 0;				/* 链表尾指针 */
static LPFREELINKNODE	_im_free_curr = 0;				/* 当前用于分配的节点指针 */

static long				_im_free_count= 0;				/* 链表节点计数 */

static int				_im_free_pgnum= 0;				/* 页尺寸 */
static unsigned char	*_im_base_addr= 0;				/* 防护空间基地址 */
static HANDLE			_im_info_heap = 0;				/* 链表节点区句柄副本 */






/******************************************************************************/
/* 助手函数声明																  */
/******************************************************************************/
static LPFREELINKNODE CALLAGREEMENT imf_alloc_node(void);
static void CALLAGREEMENT imf_free_node(LPFREELINKNODE pnode);
static void CALLAGREEMENT imf_insert_node(LPFREELINKNODE pnode);
static LPFREELINKNODE CALLAGREEMENT imf_unite_node(void);







/******************************************************************************/
/* 接口函数																	  */
/******************************************************************************/

/* 创建空闲链 */
void CALLAGREEMENT imf_create(unsigned char *pstart_addr, int page_size, HANDLE info_heap)
{
	LPFREELINKNODE	p;

	assert((_im_free_head == 0)&&(_im_free_tail == 0)&&(_im_free_count == 0));
	assert((_im_info_heap == 0)&&(_im_free_pgnum == 0)&&(_im_base_addr == 0));

	/* 设置页尺寸、信息区句柄副本、及系统基地址 */
	_im_free_pgnum = page_size;
	_im_info_heap  = info_heap;
	_im_base_addr  = pstart_addr;

	/* 申请第一个空闲块节点（所有的防护系统内存都包含在该节点中）*/
	p = imf_alloc_node();

	p->begin_addr = pstart_addr;
	p->len_in_byte= IM_MAX_REVSIZE;
	p->len_in_page= IM_MAX_REVSIZE/page_size;

	/* 插入空闲链 */
	imf_insert_node(p);
}


/* 查找指定尺寸的空闲区域（以页为单位）*/
unsigned char * CALLAGREEMENT imf_lookup(int page_count)
{
	LPFREELINKNODE	p;
	LPFREELINKNODE	patt = 0;
	unsigned char	*result = 0;

	assert(page_count);
	assert(_im_free_head&&_im_free_tail&&_im_free_curr&&_im_free_count);

	/* 如果当前分配块中的内存不够，则先进行合并操作 */
	if (_im_free_curr->len_in_page < (unsigned long)page_count)
	{
		_im_free_curr = 0;
		
		/* 合并空闲节点，并设定最大的空闲块为当前可分配块 */
		_im_free_curr = imf_unite_node();
		
		if (_im_free_curr->len_in_page < (unsigned long)page_count)
		{
			return 0;		/* 没有足够的内存可用于分配 */
		}
	}

	/* 分配内存 */
	if (_im_free_curr->len_in_page > (unsigned long)page_count)
	{
		/* 提出基地址 */
		result = _im_free_curr->begin_addr;
		
		/* 缩减节点尺寸 */
		_im_free_curr->begin_addr += (page_count*_im_free_pgnum);
		_im_free_curr->len_in_byte-= (page_count*_im_free_pgnum);
		_im_free_curr->len_in_page-= page_count;
	}
	else	/* == */
	{
		/* 如果_im_free_count等于1，则说明程序将用尽防护系统的所有内存，*/
		/* 已无法正常维系空闲链的生存。此时应适当扩大防护系统的总空间。 */
		assert(_im_free_count > 1);

		p = _im_free_curr;

		/* 抽取节点 */
		if (p->prev)
		{
			p->prev->next = p->next;
		}
		if (p->next)
		{
			p->next->prev = p->prev;
		}
		if (_im_free_head == p)
		{
			_im_free_head = p->next;
		}
		if (_im_free_tail == p)
		{
			_im_free_tail = p->prev;
		}
		
		_im_free_count--;
		
		result = p->begin_addr;
		
		/* 释放节点 */
		imf_free_node(p);

		_im_free_curr = 0;

		/* 合并空闲节点，并设定最大的空闲块为当前可分配块 */
		_im_free_curr = imf_unite_node();
	}

	return result;
}


/* 回收空闲区域 */
void CALLAGREEMENT imf_reclaim(unsigned char *paddr, int page_count)
{
	LPFREELINKNODE	p;

	/* 检验待回收的地址是否合法 */
	assert((paddr >= _im_base_addr)&&(paddr < (_im_base_addr+IM_MAX_REVSIZE)));

	/* 申请一个回收节点 */
	p = imf_alloc_node();

	p->begin_addr = paddr;
	p->len_in_byte= page_count*_im_free_pgnum;
	p->len_in_page= page_count;

	/* 插入空闲链 */
	imf_insert_node(p);
}

	
/* 销毁空闲链 */
void CALLAGREEMENT imf_destroy()
{
	_im_free_curr = 0;

	/* 合并空闲块节点（使之变为一个节点）*/
	imf_unite_node();

	/* 只能存在一个空闲块（整个防护空间）*/
	assert(_im_free_count == 1);
	assert(_im_free_head&&_im_free_tail);
	assert(_im_free_head->begin_addr == _im_base_addr);
	assert(_im_free_head->len_in_byte== IM_MAX_REVSIZE);

	/* 释放节点 */
	imf_free_node(_im_free_head);

	_im_free_head  = 0;
	_im_free_tail  = 0;
	_im_free_count = 0;
}



/* 申请一个节点 */
static LPFREELINKNODE CALLAGREEMENT imf_alloc_node()
{
	LPFREELINKNODE	p;

	/* 分配节点 */
	p = (LPFREELINKNODE)HeapAlloc(_im_info_heap, 0, sizeof(FREELINKNODE));

	/* 如果分配失败，则整个防护系统将无法继续运行，所以用断言提示开发员 */
	assert(p);

	p->magic = IM_FREELINK_MAGIC;

	p->begin_addr = 0;
	p->len_in_byte= 0;
	p->len_in_page= 0;

	p->rev0  = 0;
	p->rev1  = 0;

	p->prev  = 0;
	p->next  = 0;
	
	return p;	/* p 一定不是 0 */
}


/* 释放一个节点 */
static void CALLAGREEMENT imf_free_node(LPFREELINKNODE pnode)
{
	int rs;

	assert((pnode)&&(pnode->magic == IM_FREELINK_MAGIC));

	/* 填充垃圾信息 */
	memset((void*)pnode, IM_SHIT_FILL, sizeof(FREELINKNODE));

	/* 释放节点 */
	rs = HeapFree(_im_info_heap, 0, (void*)pnode);

	/* 如果释放失败，则整个防护系统将无法继续运行，所以用断言提示开发员 */
	assert(rs);
}


/* 在释放位置插入节点（具有相邻块合并功能） */
static void CALLAGREEMENT imf_insert_node(LPFREELINKNODE pnode)
{
	LPFREELINKNODE p;

	if (_im_free_head == 0)	/* 空链 */
	{
		assert((_im_free_tail == 0)&&(_im_free_count == 0));

		pnode->prev = pnode->next = 0;
		_im_free_curr = _im_free_head  = _im_free_tail = pnode;
		_im_free_count = 1;
	}
	else					/* 插入 */
	{
		assert(_im_free_tail&&_im_free_count);

		/* 寻找插入点（在该点之前插入）*/
		for (p=_im_free_head; p!=0; p=p->next)
		{
			if (pnode->begin_addr < p->begin_addr)
			{
				break;
			}
		}

		if (p == _im_free_head)		/* 插入链表头部 */
		{
			assert(p->magic == IM_FREELINK_MAGIC);
			assert(p->prev == 0);
			assert((p->begin_addr)&&(p->len_in_byte)&&(p->len_in_page));
			
			/* 融合操作（不在前面与当前分配节点融合）*/
			if ((_im_free_curr != p)&&((pnode->begin_addr+pnode->len_in_byte) == p->begin_addr))
			{
				p->begin_addr = pnode->begin_addr;
				p->len_in_byte += pnode->len_in_byte;
				p->len_in_page += pnode->len_in_page;

				imf_free_node(pnode);
			}
			else
			{
				pnode->prev = 0;
				pnode->next = p;
				p->prev     = pnode;
				_im_free_head = pnode;
				_im_free_count++;
			}
		}
		else if (p == 0)			/* 插入尾部 */
		{
			p = _im_free_tail;

			assert(p->magic == IM_FREELINK_MAGIC);
			assert(p->next == 0);
			assert((p->begin_addr)&&(p->len_in_byte)&&(p->len_in_page));

			/* 融合操作（允许在当前分配节点尾部进行融合）*/
			if ((p->begin_addr+p->len_in_byte) == pnode->begin_addr)
			{
				p->len_in_byte += pnode->len_in_byte;
				p->len_in_page += pnode->len_in_page;
				
				imf_free_node(pnode);
			}
			else
			{
				pnode->prev = p;
				pnode->next = 0;
				p->next = pnode;
				_im_free_tail = pnode;
				_im_free_count++;
			}
		}
		else						/* 中部 */
		{
			assert(p->magic == IM_FREELINK_MAGIC);
			assert(p->prev != 0);
			assert((p->begin_addr)&&(p->len_in_byte)&&(p->len_in_page));
			
			/* 前融合操作 */
			if ((p->prev->begin_addr+p->prev->len_in_byte) == pnode->begin_addr)
			{
				p->prev->len_in_byte += pnode->len_in_byte;
				p->prev->len_in_page += pnode->len_in_page;

				imf_free_node(pnode);

				/* 后融合（不与当前分配节点的前端融合） */
				if (((p->prev->begin_addr+p->prev->len_in_byte) == p->begin_addr)&&(_im_free_curr != p))
				{
					p->prev->len_in_byte += p->len_in_byte;
					p->prev->len_in_page += p->len_in_page;
					
					if (_im_free_tail == p)
					{
						_im_free_tail = p->prev;
					}

					assert(p->prev->next == p);

					p->prev->next = p->next;
					if (p->next)
					{
						p->next->prev = p->prev;
					}

					imf_free_node(p);

					_im_free_count--;
				}
			}
			else
			{
				/* 后融合 */
				if (((pnode->begin_addr+pnode->len_in_byte) == p->begin_addr)&&(_im_free_curr != p))
				{
					p->begin_addr   = pnode->begin_addr;
					p->len_in_byte += pnode->len_in_byte;
					p->len_in_page += pnode->len_in_page;

					imf_free_node(pnode);
				}
				else	/* 找不到融合点或者p是当前分配节点，则插入节点 */
				{
					pnode->prev = p->prev;
					pnode->next = p;

					p->prev->next = pnode;
					p->prev       = pnode;

					_im_free_count++;
				}
			}
		}
	}
}


/* 合并相邻块并返回最大尺寸的空闲块节点地址 */
static LPFREELINKNODE CALLAGREEMENT imf_unite_node()
{
	LPFREELINKNODE p = _im_free_head;
	LPFREELINKNODE pt;

	assert(_im_free_curr == 0);		/* 合并前必须先将_im_free_curr设为0 */
	assert(_im_free_head&&_im_free_tail&&_im_free_count);

	/* 合并相邻块 */
	while (p->next)
	{
		if ((p->begin_addr+p->len_in_byte) == p->next->begin_addr)
		{	/* 相邻合并 */
			pt = p->next;

			p->len_in_byte += pt->len_in_byte;
			p->len_in_page += pt->len_in_page;

			p->next = pt->next;
			if (pt->next)
			{
				pt->next->prev = p;
			}
			else
			{
				assert(_im_free_tail == pt);
				_im_free_tail = p;
			}

			imf_free_node(pt);
			
			_im_free_count--;
		}
		else
		{	/* 不相邻继续 */
			p = p->next;
		}
	}

	p  = _im_free_head;
	pt = p;

	/* 寻找最大的空闲节点 */
	while (p)
	{
		if (p->len_in_byte > pt->len_in_byte)
		{
			pt = p;
		}

		p = p->next;
	}

	/* 返回最大空闲块节点地址 */
	return pt;
}


