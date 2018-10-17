/********************************************************************

	iseeirw.c

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
			http://iseeexplorer.cosoft.org.cn
			
	���ŵ���

			yzfree##sina.com
	----------------------------------------------------------------
	���ļ���;��	ͼ���дģ��ȫ�ֶ����ļ�
	���ļ���д�ˣ�	YZ				yzfree##sina.com

	���ļ��汾��	20329
	����޸��ڣ�	2002-6-2

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	��ַ�ռ������
	----------------------------------------------------------------
	������ʷ��

		2002-3		���Ӳ���˳���������ص�INFOSTR���估�ͷź���
		2002-1		������̬ͼ���ļ����ܰ������ͼ��Ĵ���ע��
		2001-6		��һ�����԰淢��

********************************************************************/

#ifndef WIN32
#if defined(_WIN32) || defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef WIN32
#include <malloc.h>
#else
#include <mem.h>
#endif

#include "iseeirw.h"



/**********************************************************************************/
/* �ڲ������� */

#ifdef _DEBUG
static unsigned long sct_count = 0;		/* �ṹʵ����� */
static CRITICAL_SECTION  sct_access;	/* ������������ɱ�ʶ */
#endif

static char *IRWP_API_NAME_STRING[4] = { "", "get_image_info", "load_image", "save_image"};




/**********************************************************************************/
/* �ڲ��������� */

static void CALLAGREEMENT _free_subimg(LPINFOSTR pinfo);
static int  CALLAGREEMENT _defalt_process(PFUN_COMM, int, const void *, const int);
ISEEIRW_API IRWPFUN CALLAGREEMENT isirw_set_pfun(LPINFOSTR pinfo, IRWPFUN new_pfun);

ISEEIRW_API LPSUBIMGBLOCK CALLAGREEMENT isirw_alloc_SUBIMGBLOCK();
ISEEIRW_API void CALLAGREEMENT isirw_free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);




/**********************************************************************************/
/* �ӿں��� */


#ifdef WIN32

/*
  ���ܣ�����ָ��DLL�Ƿ���ISeeͼ���д���

  ��ڣ�
		hmode	�� DLL���

  ���أ�  0		�� ָ����DLL����ISee��ͼ���д���
		��0		�� ��

*/
ISEEIRW_API unsigned long CALLAGREEMENT isirw_test_plugin(HMODULE hmod)
{
	return (unsigned long)GetProcAddress(hmod, IRWP_TAG);
}



/*
  ���ܣ����ISeeͼ���д������ڲ���Ϣ

  ��ڣ�
		pfun_add�� �������isirw_test_plugin()�������ص�ֵ

  ���أ�  0		�� ����ִ��ʧ�ܣ������ò���������޷�����
		��0		�� ����ִ�гɹ�������ֵ��ָ��IRWP_INFO�ṹ�ĵ�ַ��
				   �ýṹ��ʵ���ڲ���ڲ���
*/
ISEEIRW_API LPIRWP_INFO CALLAGREEMENT isirw_get_info(unsigned long pfun_add)
{
	return (pfun_add == 0UL) ? (LPIRWP_INFO)0 : (*((IRWP_ISX)pfun_add))();
}



/*
  ���ܣ����ISeeͼ���д����ӿڵ�ַ

  ��ڣ�
		hmod	�� ���ģ����
		pinfo	�� ��isirw_get_info()�������صĲ����Ϣ�ṹ��ַ
		name	�� Ҫ��ȡ�Ľӿ����ƣ���IRWP_API_NAME�Ķ��壩

  ���أ�  0		�� ����ִ��ʧ�ܣ������ò���������޷�����
		��0		�� ����ִ�гɹ�������ֵ��ָ���ӿڵĺ�����ַ
*/
ISEEIRW_API void * CALLAGREEMENT isirw_get_api_addr(HMODULE hmod, LPIRWP_INFO pinfo, enum IRWP_API_NAME name)
{
	char buf[64];

	assert(hmod);
	assert(pinfo);

	strcpy((char*)buf, (const char *)(pinfo->irwp_func_prefix));
	strcat((char*)buf, (const char *)IRWP_API_NAME_STRING[name]);

	return (void*)GetProcAddress(hmod, (char*)buf);
}

#endif /* WIN32 */



/*
  ���ܣ�����һ���µ����ݰ�

  ��ڣ�

  ���أ�  0		�� ����ִ��ʧ�ܣ��ڴ治�㵼�µ�
		��0		�� ����ִ�гɹ�������ֵ�������ݰ����׵�ַ
				  ��ע���ɱ�������������ݰ�������isirw_free_INFOSTR()�ͷ�
*/
ISEEIRW_API LPINFOSTR CALLAGREEMENT isirw_alloc_INFOSTR()
{
	LPINFOSTR ptmp;

	/* ����ṹ�ռ� */
	if ((ptmp = (LPINFOSTR)malloc(sizeof(INFOSTR))) == (LPINFOSTR)0)
		return (LPINFOSTR)0;

#ifdef _DEBUG
	ptmp->sct_mark = INFOSTR_DBG_MARK;	/* �ṹ�ı�ʶ�����ڵ��� */

	if (sct_count == 0)
		InitializeCriticalSection(&(sct_access));

	EnterCriticalSection(&(sct_access));
	sct_count++;						/* �ṹʵ����� */
	LeaveCriticalSection(&(sct_access));
#endif
	
	ptmp->imgtype = IMT_NULL;			/* ͼ���ļ����� */
	ptmp->imgformat = IMF_NULL;			/* ͼ���ļ���ʽ����׺���� */
	ptmp->compression = ICS_RGB;		/* ͼ���ѹ����ʽ */

	ptmp->width = 0;					/* ͼ���� */
	ptmp->height = 0;					/* ͼ��߶� */
	ptmp->order = 0;					/* ͼ�������˳�� ��0������1������*/
	ptmp->bitcount = 0;					/* ÿ��������ռ��λ������Ӧ����ɫ���� */

	ptmp->r_mask = 0;					/* ����ɫ����������ֵ */
	ptmp->g_mask = 0;
	ptmp->b_mask = 0;
	ptmp->a_mask = 0;

	ptmp->pp_line_addr = (void **)0;
	ptmp->p_bit_data = (unsigned char *)0;
	ptmp->pal_count = 0;
	memset((void*)(ptmp->palette), 0, MAX_PALETTE_COUNT*sizeof(unsigned long));

	ptmp->psubimg = (LPSUBIMGBLOCK)0;	/* ��ͼ����������ַ */
	ptmp->imgnumbers = 0;				/* ���ļ���ͼ��ĸ��� */
	
	ptmp->data_state = 0;				/* ���ݰ���ǰ������״̬��
											0 �� �յ����ݰ���û���κα�����������Ч��
											1 �� ����ͼ���������Ϣ
											2 �� ����ͼ������ */
	InitializeCriticalSection(&(ptmp->access));
	
	/*ptmp->irwpfun = _defalt_process;	/* ʹ��ȱʡ�ص���������֮����Ϊ�գ� */
	/*ptmp->rev0    = 0;*/
	/*ptmp->rev1    = 0;*/
	ptmp->break_mark = 0;				/* �жϱ�־��0��������1���жϲ��� */
	ptmp->process_total = 0;			/* �ܵĲ�������ֵ */
	ptmp->process_current = 0;			/* ��ǰ�Ĳ������� */

	ptmp->play_number= 0UL;				/* ����˳���ߴ磨��Ԫ��Ϊ��λ�� */
	ptmp->play_order = 0UL;				/* ����˳��� */
	ptmp->time = 0;						/* ��֡ͣ��ʱ�� */
	ptmp->colorkey = -1;				/* ��ͼ��͸��ɫ */

	return ptmp;
}



/*
  ���ܣ��ͷ�һ�����ݰ�

  ��ڣ�
		pinfo	�� ���ݰ��׵�ַ

  ���أ�  0		�� �㶨����ֵ�����������ݰ�ָ������������
*/
ISEEIRW_API LPINFOSTR CALLAGREEMENT isirw_free_INFOSTR(LPINFOSTR pinfo)
{
	assert(pinfo);
	assert(pinfo->sct_mark == INFOSTR_DBG_MARK);

	assert(sct_count);

	EnterCriticalSection(&(pinfo->access));

#ifdef _DEBUG
	EnterCriticalSection(&(sct_access));
	sct_count--;						/* �ṹʵ����� */
	LeaveCriticalSection(&(sct_access));

	if (sct_count == 0)
		DeleteCriticalSection(&(sct_access));
#endif
	
	if (pinfo->pp_line_addr)
	{
		free(pinfo->pp_line_addr);
		pinfo->pp_line_addr = (void **)0;
	}
		
	if (pinfo->p_bit_data)
	{
		free(pinfo->p_bit_data);
		pinfo->p_bit_data = (unsigned char *)0;
	}

	if (pinfo->psubimg)
	{
		assert(pinfo->imgnumbers > 1);
		_free_subimg(pinfo);
	}
	
	if (pinfo->play_order)
	{
		free(pinfo->play_order);
		pinfo->play_order = (unsigned long *)0;
	}

	LeaveCriticalSection(&(pinfo->access));
	DeleteCriticalSection(&(pinfo->access));

	free(pinfo);

	return (LPINFOSTR)0;
}



/* �ڲ�������ֻ��isirw_free_INFOSTR����ʹ�ã��� �ͷ����ݰ��е���ͼ����������еĻ��� */
void CALLAGREEMENT _free_subimg(LPINFOSTR pinfo)
{
	int i;
	LPSUBIMGBLOCK	ptmp, pfree = pinfo->psubimg;

	/* �ͷ�������ͼ��ڵ� */
	for (i=0;i<(int)(pinfo->imgnumbers-1);i++)
	{
		ptmp = pfree->next;

		assert(pfree);

		isirw_free_SUBIMGBLOCK(pfree);

		pfree = ptmp;
	}

	pinfo->imgnumbers -= i;
	assert(pinfo->imgnumbers == 1);		/* ��Ϊֻʣ����֡ͼ���� */
	pinfo->psubimg = (LPSUBIMGBLOCK)0;
}



/*
  ���ܣ�����һ����ͼ��ڵ㣬����ʼ���ڵ�������

  ��ڣ�

  ���أ�  0		�� ����ִ��ʧ�ܣ��ڴ治�㵼�µ�
		��0		�� ����ִ�гɹ�������ֵ����ͼ��ڵ���׵�ַ
				  ��ע���ɱ������������ͼ��ڵ������isirw_free_SUBIMGBLOCK()�ͷ�
*/
ISEEIRW_API LPSUBIMGBLOCK CALLAGREEMENT isirw_alloc_SUBIMGBLOCK()
{
	LPSUBIMGBLOCK	p_subimg;
	
	/* ������ͼ����Ϣ�ڵ�(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* �ڴ治�� */
	
	/* ��ʼ��Ϊ 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	p_subimg->number   = 1;			/* ��ͼ����ʼ���Ϊ 1 */
	p_subimg->colorkey = -1;		/* ͸��ɫֵ��Ϊ�� */
	
	return p_subimg;
}



/*
  ���ܣ��ͷ�һ����ͼ��ڵ㣬�������е�λ�����������׵�ַ����

  ��ڣ�p_node	�� ���ͷŵ���ͼ��ڵ�

  ���أ�
*/
ISEEIRW_API void CALLAGREEMENT isirw_free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);
	assert(p_node->number > 0);
	
	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);
	
	if (p_node->p_bit_data)
		free(p_node->p_bit_data);
	
	free(p_node);
}



/*
  ���ܣ�Ϊָ�����ݰ������µĻص�����

  ��ڣ�pinfo	�����趨���ݰ�
		new_pfun���µĻص�����

  ���أ��ϵĻص�����
*/
ISEEIRW_API IRWPFUN CALLAGREEMENT isirw_set_pfun(LPINFOSTR pinfo, IRWPFUN new_pfun)
{
	IRWPFUN old;

	assert(pinfo);

	if (new_pfun == 0)
	{
		old = _defalt_process;
		new_pfun = _defalt_process;		/* �ص���������Ϊ�� */
	}
	else
	{
		/*old = pinfo->irwpfun;*/
	}

	/*pinfo->irwpfun = new_pfun;*/

	return old;
}



/* 
  ���ܣ�ȱʡ�Ľ��Ȼص�����

  ��ڣ�type	����������
		img_num	��ͼ����ţ�0Ϊ��ͼ������Ϊ��ͼ����ţ�
		pinfo	����ͼ�����ݰ���ַ����ͼ�����ݽṹ��ַ�������������;�����
		cur		����ǰ����ֵ����ͼ��ɨ����Ϊ��λ��

  ���أ�0��������дͼ��
*/
static int CALLAGREEMENT _defalt_process(PFUN_COMM type, int img_num, const void *pinfo, const int cur)
{
	int				result = 0;
	LPINFOSTR		pinfo_str = (LPINFOSTR)pinfo;
	LPSUBIMGBLOCK	psub_str  = (LPSUBIMGBLOCK)pinfo;

	(void)type, (void)img_num, (void)pinfo, (void)cur;

	switch (type)
	{
	case	PFC_NULL:						/* ������ */
		break;
	case	PFC_INTO_PLUG:					/* ���ƽ����� */
		break;
	case	PFC_BUILD_MAST_IMAGE:			/* �µ���ͼ�� */
		break;
	case	PFC_BUILD_SERT_IMAGE:			/* �µ���ͼ�� */
		break;
	case	PFC_PROCESS:					/* ���� */
		break;
	case	PFC_OVER_SIR:					/* ����һ��ͼ�� */
		break;
	case	PFC_OUT_PLUG:					/* ���Ƽ����뿪��� */
		break;
	default:
		assert(0); result = 2; break;		/* ϵͳ�����жϲ���Ķ�д���� */
	}
	
	return result;	/* 0��������д��1���û��жϣ�2��ϵͳ�쳣��ǿ���жϲ��� */
}

