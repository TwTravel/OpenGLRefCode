/********************************************************************

	allolink.c

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
	���ļ���;��	ISeeͼ����������ڴ����ϵͳ�ѷ����ڴ�������ʵ���ļ�
	
	  
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20618
	����޸��ڣ�	2002-6-18
		  
	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-6		�ڶ��淢��
		2002-5		��һ�淢��

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
/* ȫ�ֱ����� 																  */
/******************************************************************************/
static LPALLOLINKNODE	_im_allo_head = 0;				/* ����ͷָ�� */
static LPALLOLINKNODE	_im_allo_tail = 0;				/* ����βָ�� */

static long				_im_allo_count= 0;				/* ����ڵ���� */

static int				_im_allo_pgnum= 0;				/* ҳ�ߴ� */
static unsigned char	*_im_base_addr= 0;				/* �����ռ����ַ */
static HANDLE			_im_info_heap = 0;				/* ����ڵ���������� */






/******************************************************************************/
/* ���ֺ�������																  */
/******************************************************************************/
static LPALLOLINKNODE CALLAGREEMENT ima_alloc_node(void);
static void CALLAGREEMENT ima_free_node(LPALLOLINKNODE pnode);
static void CALLAGREEMENT ima_insert_node(LPALLOLINKNODE pnode);




/******************************************************************************/
/* �ӿں���																	  */
/******************************************************************************/

/* �������� */
void CALLAGREEMENT ima_create(unsigned char *pstart_addr, int page_size, HANDLE info_heap)
{
	/* ��ͬһ�����ڲ������ظ����ñ����� */
	assert((_im_allo_head == 0)&&(_im_allo_tail == 0)&&(_im_allo_count == 0));
	assert((_im_info_heap == 0)&&(_im_allo_pgnum == 0)&&(_im_base_addr == 0));
	
	/* ����ҳ�ߴ硢��Ϣ�����������������ϵͳ����ַ */
	_im_allo_pgnum = page_size;
	_im_info_heap  = info_heap;
	_im_base_addr  = pstart_addr;
}


/* �������� */
void CALLAGREEMENT ima_destroy()
{
	/* ���������ڴ�й© */
	assert((_im_allo_head == 0)&&(_im_allo_tail == 0)&&(_im_allo_count == 0));
}


/* ��¼�ѷ��䵥Ԫ��Ϣ */
LPALLOLINKNODE CALLAGREEMENT ima_push(unsigned char *paddr, 
										 int page_count, 
										 int user_data_len,
										 long attrib,
										 unsigned char *fn,
										 unsigned long linenum)
{
	LPALLOLINKNODE	p;

	assert(paddr&&page_count&&user_data_len);

	/* ������Ϣ�ڵ� */
	p = ima_alloc_node();

	/* ��д�ڵ���Ϣ */
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

	/* ���ڵ�������� */
	ima_insert_node(p);

	return p;
}


/* ��ȡָ��ָ����ڴ�ڵ���Ϣ */
LPALLOLINKNODE CALLAGREEMENT ima_search(unsigned char *puser_addr)
{
	LPALLOLINKNODE	p;

	/* ���ָ����0���򷵻�����ͷ��ĵ�ַ */
	if (puser_addr == 0)
	{
		return _im_allo_head;
	}

	/* ָ�����ڴ��ַ������ϵͳ����������Χ֮����ڴ� */
	assert((puser_addr >= (_im_base_addr+IM_GUARD_PAGE_NUM*_im_allo_pgnum+sizeof(FGUARDSTRUCT)))&&(puser_addr < (_im_base_addr+IM_MAX_REVSIZE-sizeof(BGUARDSTRUCT))));

	/* ���Ҹõ�ַ��Ӧ����Ϣ�ڵ� */
	for (p=_im_allo_head; p!=0; p=p->next)
	{
		if ((puser_addr >= p->user_addr)&&(puser_addr < (p->user_addr+p->user_length)))
		{
			break;
		}
	}

	return p;	/* ����ֵ�п�����0����˵��û���ҵ��ڵ���Ϣ�������ָ����һ���Ƿ�ָ�룩*/
}


/* ժ�����ͷ�һ����Ϣ�ڵ� */
void CALLAGREEMENT ima_pop(LPALLOLINKNODE pnode)
{
	assert(pnode&&(pnode->magic == IM_ALLOLINK_MAGIC));
	assert((pnode->guard_length)&&(pnode->guard_addr)&&(pnode->bg_addr)&&(pnode->begin_length));

	/* ժ���ڵ� */
	if (pnode->prev)
	{
		pnode->prev->next = pnode->next;
	}
	if (pnode->next)
	{
		pnode->next->prev = pnode->prev;
	}

	/* ����ͷβָ�� */
	if (_im_allo_head == pnode)
	{
		_im_allo_head = pnode->next;
	}
	if (_im_allo_tail == pnode)
	{
		_im_allo_tail = pnode->prev;
	}

	/* �ͷŽڵ� */
	ima_free_node(pnode);

	_im_allo_count--;
}


/* ��ȡ�ѷ������Ľڵ���� */
int CALLAGREEMENT ima_get_count()
{
	return _im_allo_count;
}


/* ����һ���ڵ� */
static LPALLOLINKNODE CALLAGREEMENT ima_alloc_node()
{
	LPALLOLINKNODE	p;
	
	assert(_im_info_heap);

	/* ����ڵ� */
	p = (LPALLOLINKNODE)HeapAlloc(_im_info_heap, 0, sizeof(ALLOLINKNODE));

	/* �������ʧ�ܣ�����������ϵͳ���޷��������У������ö�����ʾ����Ա */
	assert(p);

	memset((void*)p, 0, sizeof(ALLOLINKNODE));

	p->magic = IM_ALLOLINK_MAGIC;
	
	return p;
}


/* �ͷ�һ���ڵ� */
static void CALLAGREEMENT ima_free_node(LPALLOLINKNODE pnode)
{
	int rs;

	assert((pnode)&&(pnode->magic == IM_ALLOLINK_MAGIC));
	
	/* ���������Ϣ */
	memset((void*)pnode, IM_SHIT_FILL, sizeof(ALLOLINKNODE));

	/* �ͷŽڵ� */
	rs = HeapFree(_im_info_heap, 0, (void*)pnode);

	/* ����ͷ�ʧ�ܣ�����������ϵͳ���޷��������У������ö�����ʾ����Ա */
	assert(rs);
}


/* ������ͷ������ڵ� */
static void CALLAGREEMENT ima_insert_node(LPALLOLINKNODE pnode)
{
#if 0
	LPALLOLINKNODE	p;
#endif

	assert(pnode&&(pnode->magic==IM_ALLOLINK_MAGIC)&&(pnode->user_length>0));

	if (_im_allo_head == 0)		/* ���� */
	{
		assert((_im_allo_tail == 0)&&(_im_allo_count == 0));

		pnode->prev = pnode->next = 0;
		_im_allo_head = _im_allo_tail = pnode;
		_im_allo_count = 1;
	}
	else						/* ���������ײ� */
	{
		assert(_im_allo_head&&_im_allo_count);

		pnode->prev = 0;
		pnode->next = _im_allo_head;
		_im_allo_head->prev = pnode;
		_im_allo_head = pnode;

		_im_allo_count++;

#if 0
		/* ����Ĵ���Ϊ����ַ����Ĳ��뷽������ʱ�����ã�*/

		/* Ѱ�Ҳ���� */
		for (p=_im_allo_head; p!=0; p=p->next)
		{
			assert(p->guard_addr != pnode->guard_addr);

			if (p->guard_addr > pnode->guard_addr)
			{
				break;
			}
		}

		if (p)		/* �С�ͷ������ */
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
		else		/* β������ */
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


