/********************************************************************

	freelink.c

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
	���ļ���;��	ISeeͼ����������ڴ����ϵͳ�����ڴ�������ʵ���ļ�
	
	  
	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net
		
	���ļ��汾��	20628
	����޸��ڣ�	2002-6-28
		  
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
#include "freelink.h"



/******************************************************************************/
/* ȫ�ֱ����� 																  */
/******************************************************************************/
static LPFREELINKNODE	_im_free_head = 0;				/* ����ͷָ�� */
static LPFREELINKNODE	_im_free_tail = 0;				/* ����βָ�� */
static LPFREELINKNODE	_im_free_curr = 0;				/* ��ǰ���ڷ���Ľڵ�ָ�� */

static long				_im_free_count= 0;				/* ����ڵ���� */

static int				_im_free_pgnum= 0;				/* ҳ�ߴ� */
static unsigned char	*_im_base_addr= 0;				/* �����ռ����ַ */
static HANDLE			_im_info_heap = 0;				/* ����ڵ���������� */






/******************************************************************************/
/* ���ֺ�������																  */
/******************************************************************************/
static LPFREELINKNODE CALLAGREEMENT imf_alloc_node(void);
static void CALLAGREEMENT imf_free_node(LPFREELINKNODE pnode);
static void CALLAGREEMENT imf_insert_node(LPFREELINKNODE pnode);
static LPFREELINKNODE CALLAGREEMENT imf_unite_node(void);







/******************************************************************************/
/* �ӿں���																	  */
/******************************************************************************/

/* ���������� */
void CALLAGREEMENT imf_create(unsigned char *pstart_addr, int page_size, HANDLE info_heap)
{
	LPFREELINKNODE	p;

	assert((_im_free_head == 0)&&(_im_free_tail == 0)&&(_im_free_count == 0));
	assert((_im_info_heap == 0)&&(_im_free_pgnum == 0)&&(_im_base_addr == 0));

	/* ����ҳ�ߴ硢��Ϣ�������������ϵͳ����ַ */
	_im_free_pgnum = page_size;
	_im_info_heap  = info_heap;
	_im_base_addr  = pstart_addr;

	/* �����һ�����п�ڵ㣨���еķ���ϵͳ�ڴ涼�����ڸýڵ��У�*/
	p = imf_alloc_node();

	p->begin_addr = pstart_addr;
	p->len_in_byte= IM_MAX_REVSIZE;
	p->len_in_page= IM_MAX_REVSIZE/page_size;

	/* ��������� */
	imf_insert_node(p);
}


/* ����ָ���ߴ�Ŀ���������ҳΪ��λ��*/
unsigned char * CALLAGREEMENT imf_lookup(int page_count)
{
	LPFREELINKNODE	p;
	LPFREELINKNODE	patt = 0;
	unsigned char	*result = 0;

	assert(page_count);
	assert(_im_free_head&&_im_free_tail&&_im_free_curr&&_im_free_count);

	/* �����ǰ������е��ڴ治�������Ƚ��кϲ����� */
	if (_im_free_curr->len_in_page < (unsigned long)page_count)
	{
		_im_free_curr = 0;
		
		/* �ϲ����нڵ㣬���趨���Ŀ��п�Ϊ��ǰ�ɷ���� */
		_im_free_curr = imf_unite_node();
		
		if (_im_free_curr->len_in_page < (unsigned long)page_count)
		{
			return 0;		/* û���㹻���ڴ�����ڷ��� */
		}
	}

	/* �����ڴ� */
	if (_im_free_curr->len_in_page > (unsigned long)page_count)
	{
		/* �������ַ */
		result = _im_free_curr->begin_addr;
		
		/* �����ڵ�ߴ� */
		_im_free_curr->begin_addr += (page_count*_im_free_pgnum);
		_im_free_curr->len_in_byte-= (page_count*_im_free_pgnum);
		_im_free_curr->len_in_page-= page_count;
	}
	else	/* == */
	{
		/* ���_im_free_count����1����˵�������þ�����ϵͳ�������ڴ棬*/
		/* ���޷�����άϵ�����������档��ʱӦ�ʵ��������ϵͳ���ܿռ䡣 */
		assert(_im_free_count > 1);

		p = _im_free_curr;

		/* ��ȡ�ڵ� */
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
		
		/* �ͷŽڵ� */
		imf_free_node(p);

		_im_free_curr = 0;

		/* �ϲ����нڵ㣬���趨���Ŀ��п�Ϊ��ǰ�ɷ���� */
		_im_free_curr = imf_unite_node();
	}

	return result;
}


/* ���տ������� */
void CALLAGREEMENT imf_reclaim(unsigned char *paddr, int page_count)
{
	LPFREELINKNODE	p;

	/* ��������յĵ�ַ�Ƿ�Ϸ� */
	assert((paddr >= _im_base_addr)&&(paddr < (_im_base_addr+IM_MAX_REVSIZE)));

	/* ����һ�����սڵ� */
	p = imf_alloc_node();

	p->begin_addr = paddr;
	p->len_in_byte= page_count*_im_free_pgnum;
	p->len_in_page= page_count;

	/* ��������� */
	imf_insert_node(p);
}

	
/* ���ٿ����� */
void CALLAGREEMENT imf_destroy()
{
	_im_free_curr = 0;

	/* �ϲ����п�ڵ㣨ʹ֮��Ϊһ���ڵ㣩*/
	imf_unite_node();

	/* ֻ�ܴ���һ�����п飨���������ռ䣩*/
	assert(_im_free_count == 1);
	assert(_im_free_head&&_im_free_tail);
	assert(_im_free_head->begin_addr == _im_base_addr);
	assert(_im_free_head->len_in_byte== IM_MAX_REVSIZE);

	/* �ͷŽڵ� */
	imf_free_node(_im_free_head);

	_im_free_head  = 0;
	_im_free_tail  = 0;
	_im_free_count = 0;
}



/* ����һ���ڵ� */
static LPFREELINKNODE CALLAGREEMENT imf_alloc_node()
{
	LPFREELINKNODE	p;

	/* ����ڵ� */
	p = (LPFREELINKNODE)HeapAlloc(_im_info_heap, 0, sizeof(FREELINKNODE));

	/* �������ʧ�ܣ�����������ϵͳ���޷��������У������ö�����ʾ����Ա */
	assert(p);

	p->magic = IM_FREELINK_MAGIC;

	p->begin_addr = 0;
	p->len_in_byte= 0;
	p->len_in_page= 0;

	p->rev0  = 0;
	p->rev1  = 0;

	p->prev  = 0;
	p->next  = 0;
	
	return p;	/* p һ������ 0 */
}


/* �ͷ�һ���ڵ� */
static void CALLAGREEMENT imf_free_node(LPFREELINKNODE pnode)
{
	int rs;

	assert((pnode)&&(pnode->magic == IM_FREELINK_MAGIC));

	/* ���������Ϣ */
	memset((void*)pnode, IM_SHIT_FILL, sizeof(FREELINKNODE));

	/* �ͷŽڵ� */
	rs = HeapFree(_im_info_heap, 0, (void*)pnode);

	/* ����ͷ�ʧ�ܣ�����������ϵͳ���޷��������У������ö�����ʾ����Ա */
	assert(rs);
}


/* ���ͷ�λ�ò���ڵ㣨�������ڿ�ϲ����ܣ� */
static void CALLAGREEMENT imf_insert_node(LPFREELINKNODE pnode)
{
	LPFREELINKNODE p;

	if (_im_free_head == 0)	/* ���� */
	{
		assert((_im_free_tail == 0)&&(_im_free_count == 0));

		pnode->prev = pnode->next = 0;
		_im_free_curr = _im_free_head  = _im_free_tail = pnode;
		_im_free_count = 1;
	}
	else					/* ���� */
	{
		assert(_im_free_tail&&_im_free_count);

		/* Ѱ�Ҳ���㣨�ڸõ�֮ǰ���룩*/
		for (p=_im_free_head; p!=0; p=p->next)
		{
			if (pnode->begin_addr < p->begin_addr)
			{
				break;
			}
		}

		if (p == _im_free_head)		/* ��������ͷ�� */
		{
			assert(p->magic == IM_FREELINK_MAGIC);
			assert(p->prev == 0);
			assert((p->begin_addr)&&(p->len_in_byte)&&(p->len_in_page));
			
			/* �ںϲ���������ǰ���뵱ǰ����ڵ��ںϣ�*/
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
		else if (p == 0)			/* ����β�� */
		{
			p = _im_free_tail;

			assert(p->magic == IM_FREELINK_MAGIC);
			assert(p->next == 0);
			assert((p->begin_addr)&&(p->len_in_byte)&&(p->len_in_page));

			/* �ںϲ����������ڵ�ǰ����ڵ�β�������ںϣ�*/
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
		else						/* �в� */
		{
			assert(p->magic == IM_FREELINK_MAGIC);
			assert(p->prev != 0);
			assert((p->begin_addr)&&(p->len_in_byte)&&(p->len_in_page));
			
			/* ǰ�ںϲ��� */
			if ((p->prev->begin_addr+p->prev->len_in_byte) == pnode->begin_addr)
			{
				p->prev->len_in_byte += pnode->len_in_byte;
				p->prev->len_in_page += pnode->len_in_page;

				imf_free_node(pnode);

				/* ���ںϣ����뵱ǰ����ڵ��ǰ���ںϣ� */
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
				/* ���ں� */
				if (((pnode->begin_addr+pnode->len_in_byte) == p->begin_addr)&&(_im_free_curr != p))
				{
					p->begin_addr   = pnode->begin_addr;
					p->len_in_byte += pnode->len_in_byte;
					p->len_in_page += pnode->len_in_page;

					imf_free_node(pnode);
				}
				else	/* �Ҳ����ںϵ����p�ǵ�ǰ����ڵ㣬�����ڵ� */
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


/* �ϲ����ڿ鲢�������ߴ�Ŀ��п�ڵ��ַ */
static LPFREELINKNODE CALLAGREEMENT imf_unite_node()
{
	LPFREELINKNODE p = _im_free_head;
	LPFREELINKNODE pt;

	assert(_im_free_curr == 0);		/* �ϲ�ǰ�����Ƚ�_im_free_curr��Ϊ0 */
	assert(_im_free_head&&_im_free_tail&&_im_free_count);

	/* �ϲ����ڿ� */
	while (p->next)
	{
		if ((p->begin_addr+p->len_in_byte) == p->next->begin_addr)
		{	/* ���ںϲ� */
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
		{	/* �����ڼ��� */
			p = p->next;
		}
	}

	p  = _im_free_head;
	pt = p;

	/* Ѱ�����Ŀ��нڵ� */
	while (p)
	{
		if (p->len_in_byte > pt->len_in_byte)
		{
			pt = p;
		}

		p = p->next;
	}

	/* ���������п�ڵ��ַ */
	return pt;
}


