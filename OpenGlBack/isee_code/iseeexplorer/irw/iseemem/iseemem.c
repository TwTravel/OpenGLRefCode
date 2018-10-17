/********************************************************************

	iseemem.c

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
	���ļ���;��	ISeeͼ����������ڴ����ϵͳ�����ļ�


	���ļ���д�ˣ�	
					YZ			yzfree##yeah.net

	���ļ��汾��	20701
	����޸��ڣ�	2002-7-1

	ע������E-Mail��ַ�е�##����@�滻����������Ϊ�˵��ƶ����E-Mail
	    ��ַ�ռ������
  	----------------------------------------------------------------
	������ʷ��

			2002-7		�Ľ���������Ϣ�洢����
			2002-6		���Ӷ������������LOG�ļ�����(2.1)
			2002-6		�ڶ��淢��(2.0)
			2002-5		��һ�淢��(1.0)

********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* ����windows.h�ļ��ı���ʱ�� */
#define _WIN32_WINNT 0x0400				/* IsDebuggerPresent()�������� */
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
/* ȫ�ֱ����� 																  */
/******************************************************************************/
static unsigned char	*im_start_addr    = 0;		/* �����ڴ�ռ�Ļ���ַ */
static int				im_page_size      = 0;		/* �����ڴ�ҳ�ĳߴ� */

static HANDLE			im_info_heap      = 0;		/* �ڵ�Ѿ�� */

static unsigned long	im_alloc_count    = 0;		/* ��������ƴ� */
static unsigned long	im_free_count     = 0;		/* �ͷŲ����ƴ� */
static unsigned long	im_max_mem_dosage = 0;		/* ˲������ڴ����� */
static unsigned long	im_cur_mem_dosage = 0;		/* ��ǰ�ڴ����� */

static unsigned long	im_tls_index      = 0;		/* �쳣�����־ */
static unsigned long	im_debuged        = 0;		/* ϵͳ�Ƿ񱻵��������� */
static FILE				*im_log_handle    = 0;		/* ϵͳ��־�ļ���� */

static unsigned char	im_log_filename[IM_LOG_FN_MAX_SIZE];	/* ϵͳ��־�ļ�ȫ·�� */




/******************************************************************************/
/* ���ֺ�������																  */
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
/* �ӿں���																	  */
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
			/* ���ϵͳ�����ڴ�ҳ�ߴ� */
			im_page_size  = _get_page_size();
			/* �ж�ϵͳ�Ƿ��Ǳ������������ģ���0Ϊ������������0Ϊ��������*/
			im_debuged    = IsDebuggerPresent();

			assert(im_page_size);
			assert(im_info_heap == 0);

			/* �����̱߳��ش洢��TLS�����������ڴ���쳣�����־ */
			if ((im_tls_index=TlsAlloc()) == 0xFFFFFFFF)
			{
				im_output_debug_info("\n--->IN : �޷�����TLS������ϵͳ����ʧ�ܣ�\n\n");
                return FALSE; 
			}
			
			/* ������Ϣ�ڵ������ѣ� */
			/* ((IM_MAX_REVSIZE/im_page_size/2)*sizeof(ALLOLINKNODE))/4 �ĺ��壺/*
			/*      (IM_MAX_REVSIZE/im_page_size/2) ��������ڵ���Ŀ����ҳΪһ��С�ڵ㣩*/
			/*      *sizeof(ALLOLINKNODE)           ����ÿ���ڵ�ṹ�ĳߴ� */
			/*      /4                              ��һ�����ֻ��ʹ��1/4�Ľڵ�ѿռ䣨��� */
			/*                                        �ѿռ䲻�㣬�ѹ��������Զ����ţ�*/
			if ((im_info_heap=HeapCreate(0, ((IM_MAX_REVSIZE/im_page_size/2)*sizeof(ALLOLINKNODE))/4, 0)) == 0)
			{
				im_output_debug_info("\n--->IN : ����ϵͳ�ڵ�ѷ���ʧ�ܣ�ϵͳ�޷����У�\n\n");
				TlsFree(im_tls_index);
				return FALSE;
			}

			/* �������ϵͳ�ռ� */
			if (!_alloc_reserves_area())
			{
				im_output_debug_info("\n--->IN : ����ϵͳ�ռ�����ʧ�ܣ�ϵͳ�޷����У�\n\n");
				TlsFree(im_tls_index);
				HeapDestroy(im_info_heap);
				return FALSE;
			}

			/* �������������������־�ļ� */
			if (im_debuged == 0)
			{
				/* ��ȡϵͳʵ���ļ�����·�����ļ��� */
				GetModuleFileName(hModule, im_log_filename, IM_LOG_FN_MAX_SIZE);

				/* ����ȫ·�������һ��'.'�������ڵ�λ�� */
				p = strrchr((const char *)im_log_filename, (int)(unsigned int)(unsigned char)'.');
				/* ϵͳ��ʵ���ļ���Ȼ����.dll��β������p��Ȼ����0 */
				assert(p);
				
				*p = 0;

				/* ����LOG�ļ�����ʵ���ļ�.txt��*/
				strcat((char*)im_log_filename, ".txt");

				/* ������־�ļ� */
				im_log_handle = fopen((const char *)im_log_filename, "w");

				if (im_log_handle == 0)
				{
					_free_reserves_area();
					TlsFree(im_tls_index);
					HeapDestroy(im_info_heap);
					return FALSE;
				}
			}

			/* ��ʼ�����ʹؼ��� */
			InitializeCriticalSection(&im_critical);
			
			/* �������п����� */
			imf_create(im_start_addr, im_page_size, im_info_heap);

			/* ������������� */
			ima_create(im_start_addr, im_page_size, im_info_heap);

			im_output_debug_info("\n--->IN : �ڴ����ϵͳ�����ɹ���\n\n");
			/* �˴����� break����Ϊ���̽�����ʱ���Զ�����һ��ȱʡ�߳� */
		case DLL_THREAD_ATTACH:
			/* Ϊ���߳�����һ���߳���Ϣ�ṹ */
			pti = (LPIMTHREADINFO)HeapAlloc(im_info_heap, 0, sizeof(IMTHREADINFO));

			/* ��Ϊһ�����̵��߳���������̫�࣬����IMTHREADINFO�ṹ�ĳߴ�Ҳ����
			   ���������pti���޷�����ɹ��������ϵͳ��ʱҲ����Σ�յ�״̬������
			   �˴��ö��Զ����Ƿ��ؼ١�*/
			assert(pti);

			/* ��д�ṹ��ʼֵ */
			pti->thread_id   = (unsigned long)GetCurrentThreadId();
			pti->except_mark = 0;
			pti->watch_curr  = 0;
			memset((void*)pti->watch_info, 0, sizeof(IMWATCHINFOSTR)*IM_WATCHINFONUM);

			/* ���߳̽ṹ��ַ���浽TLS������ */
			result = TlsSetValue(im_tls_index, (LPVOID)pti);

			/* ���Ա�������ɹ�������˵������ϵͳ�Ѳ����� */
			assert(result);

			break;
		case DLL_THREAD_DETACH:
			/* ȡ���߳��Լ�����Ϣ�ṹ */
			pti = (LPIMTHREADINFO)TlsGetValue(im_tls_index);

			/* ���Ա�����ȡ�ظýṹ������˵������ϵͳ�Ѳ����� */
			assert(pti);

			/* �ͷ��߳���Ϣ�ṹ */
			HeapFree(im_info_heap, 0, (LPVOID)pti);

			break;
		case DLL_PROCESS_DETACH:
			/* ����Ƿ�����ڴ�й© */
			if (ima_get_count())
			{
				LPALLOLINKNODE	p;

				im_output_debug_info("--->OUT: ����ϵͳ��⵽%d���ڴ�й©��\n", ima_get_count());

				/* �ͷ�й©���ڴ�� */
				while (p=ima_search(0))
				{
					assert((p->magic == IM_ALLOLINK_MAGIC)&&(p->guard_addr)&&(p->user_length));

					im_output_debug_info("         �ļ�%s�е�%d�д��������ڴ�û�б��ͷţ�\n", _short_filename(p->filename), p->linenum);

					im_free(p->user_addr);
				}
			}

			/* �ݻ����� */
			ima_destroy();
			imf_destroy();

			/* �ͷ�ϵͳ�ռ� */
			_free_reserves_area();

			/* ȡ��ȱʡ�̵߳���Ϣ�ṹ */
			pti = (LPIMTHREADINFO)TlsGetValue(im_tls_index);
			
			/* ���Ա�����ȡ�ظýṹ������˵������ϵͳ�Ѳ����� */
			assert(pti);
			
			/* �ͷ�ȱʡ�߳���Ϣ�ṹ */
			HeapFree(im_info_heap, 0, (LPVOID)pti);
			
			/* �ͷ��̱߳��ش洢���� */
            TlsFree(im_tls_index); 
			
			/* �ͷŽڵ�� */
			HeapDestroy(im_info_heap);

			/* ���ٷ��ʹؼ��� */
			DeleteCriticalSection(&im_critical);
			
			/* �����ڴ�ʹ����� */
			im_output_debug_info("\n--->OUT: �ڴ����ϵͳ�˳��ɹ���\n");
			im_output_debug_info("         ��������з��������%d��\n", im_alloc_count);
			im_output_debug_info("                   �ͷŲ�����%d��\n", im_free_count);

			if (im_max_mem_dosage < 1024)
			{
				im_output_debug_info("           ˲������ڴ�������%d�ֽ�\n\n", im_max_mem_dosage);
			}
			else if (im_max_mem_dosage < 1024*1024)
			{
				im_output_debug_info("           ˲������ڴ�������%d KB (%d byte)\n\n", im_max_mem_dosage/1024, im_max_mem_dosage);
			}
			else
			{
				im_output_debug_info("           ˲������ڴ�������%d MB (%d byte)\n\n", im_max_mem_dosage/(1024*1024), im_max_mem_dosage);
			}

			/* �ر�ϵͳ��־�ļ� */
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




/* malloc()��Ӧ�� */
ISEEMEM_API void * CALLAGREEMENT im_malloc_act(size_t size, char *fn, int ln)
{
	int				pages;
	unsigned char	*paddr;
	LPALLOLINKNODE	aln;

	EnterCriticalSection(&im_critical);

	/* ע��im_malloc���������0�ֽڵ��ڴ�飬��һ����ANSI��׼��ͬ������
	 *     ������׷��ֳ�������ϵ����� */
	assert(size);		

	/* ��������ҳ�� */
	pages = _cal_page_count(size);

	/* ǰ�÷���ҳ��������һҳ������ */
	assert(pages >= 2);
	
	/* ��ѯ�Ƿ����㹻�Ŀ����ڴ� */
	if ((paddr=imf_lookup(pages)) == 0)
	{
		LeaveCriticalSection(&im_critical);
		return (void*)0;
	}

	/* �����ڴ�ҳ */
	if (_alloc_pages(paddr, pages))
	{
		imf_reclaim(paddr, pages);
		LeaveCriticalSection(&im_critical);
		return (void*)0;
	}

	/* �����ѷ�������ȱʡ����Ϊ�ɶ�д��0�� */
	aln = ima_push(paddr, pages, size, 0, fn, ln);

	/* ������ */
	memset((void*)aln->fg_addr, IM_SHIT_FILL, (aln->guard_length-1)*im_page_size); 
	
	/* ��дǰ�����ṹ */
	aln->fg_addr->magic  = IM_GUARD_MAGIC;
	aln->fg_addr->f_guard= IM_GUARD_FILL;

	/* ��д������ṹ */
	aln->bg_addr->b_guard= IM_GUARD_FILL;
	aln->bg_addr->magic  = IM_GUARD_MAGIC;

	im_alloc_count++;
	im_cur_mem_dosage += aln->user_length;

	/* ˢ��˲������ڴ�����ֵ */
	if (im_cur_mem_dosage > im_max_mem_dosage)
	{
		im_max_mem_dosage = im_cur_mem_dosage;
	}

	LeaveCriticalSection(&im_critical);

	return (void*)(aln->user_addr);
}


/* calloc()��Ӧ�� */
ISEEMEM_API void * CALLAGREEMENT im_calloc_act(size_t num, size_t size, char *fn, int ln)
{
	return im_malloc_act(num*size, fn, ln);
}


/* realloc()��Ӧ�� */
ISEEMEM_API void * CALLAGREEMENT im_realloc_act(void *pold, size_t newsize, char *fn, int ln)
{
	LPALLOLINKNODE	aln;
	unsigned char	*pb;

	/* ���û��ָ���ڴ��ַ����malloc���� */
	if (pold == 0)
	{
		return im_malloc_act(newsize, fn, ln);
	}

	/* ����³ߴ�Ϊ0����free���� */
	if (newsize == 0)
	{
		im_free_act(pold, fn, ln);
		return (void*)0;
	}

	/* �������ڴ�飨�����������쳤�����������ڴ�飬����ֱ�������µ��ڴ�飩*/
	if ((pb=im_malloc_act(newsize, fn, ln)) == 0)
	{
		return (void*)0;	/* ����ʧ�ܣ������ڴ����Ȼ��Ч */
	}

	/* ����ؼ��� */
	EnterCriticalSection(&im_critical);

	/* �������ڴ�����Ϣ�ڵ��ַ */
	aln = ima_search((unsigned char *)pold);

	/* pold �������ܷ������ڴ�飨Ҳ����˵ima_search()�����������ҵ���Ӧ�ڵ㣩 */
	assert(aln);
	assert(aln->user_addr == (unsigned char *)pold);

	/* �������ڴ���е����ݵ����ڴ�� */
	memcpy((void*)pb, (const void *)pold, min((size_t)aln->user_length,newsize));

	LeaveCriticalSection(&im_critical);

	/* �ͷ����ڴ�� */
	im_free_act(pold, fn, ln);

	return (void*)pb;
}


/* free()��Ӧ�� */
ISEEMEM_API void CALLAGREEMENT im_free_act(void *paddr, char *fn, int ln)
{
	LPALLOLINKNODE		aln;
	unsigned char		*pbase;
	int					pages;

	EnterCriticalSection(&im_critical);

	/* ��ȡ��Ӧ�ڵ��ַ */
	aln = ima_search((unsigned char *)paddr);

	/* paddr �������ܷ������ڴ�飨Ҳ����˵ima_search()�����������ҵ���Ӧ�ڵ㣩 */
	/* ���߳�������ͼ�ͷ�һ���ѱ��ͷŹ����ڴ�� */
	assert(aln);

	pbase = aln->guard_addr;
	pages = aln->guard_length;

	/* д����״̬��� */
	if (aln->attrib != 0)
	{
		im_output_debug_info("--->INF: %s�е�%d�д��������ڴ���û�н��д����֮ǰ�ͱ��ͷ��ˣ���֪����������Ƿ���ȷ��\n", _short_filename(aln->filename), aln->linenum);
	}

	/* ���������� */
	switch (_check_overflow(aln))
	{
	case	0:			/* ��������� */
		break;
	case	1:			/* ǰ��� */
		im_output_debug_info("--->FRE: [ǰ���] �ļ�%s�е�%d�д��������ڴ淢����ǰд�����\n", _short_filename(aln->filename), aln->linenum);
		break;
	case	2:			/* ����� */
		im_output_debug_info("--->FRE: [�����] �ļ�%s�е�%d�д��������ڴ淢���˺�д�����\n", _short_filename(aln->filename), aln->linenum);
		break;
	case	3:			/* ǰ�������� */
		im_output_debug_info("--->FRE: [ǰ�������] �ļ�%s�е�%d�д��������ڴ淢����ǰ����д�����\n", _short_filename(aln->filename), aln->linenum);
		break;
	default:
		im_output_debug_info("--->FRE: [����ϵͳ�쳣] ����ϵͳ���ڲ��ȶ�״̬�������˳�����\n");
		assert(0);		/* ϵͳ�쳣 */
		break;
	}

	im_free_count++;
	im_cur_mem_dosage -= aln->user_length;
		
	/* ���ѷ���������ɾ���ڵ� */
	ima_pop(aln);

	/* �ͷ�ҳ�� */
	_free_pages(pbase, pages);

	/* ����������� */
	imf_reclaim(pbase, pages);

	LeaveCriticalSection(&im_critical);
}


/* ���������Ϣ */
ISEEMEM_API void CALLAGREEMENT im_output_debug_info(const char *pstr, ...)
{
	static char buf[IM_DBGINFO_MAX_SIZE];
	va_list		va;

	va_start(va, pstr);
	vsprintf(buf, pstr, va);
	va_end(va);

	if (im_debuged == 0)
	{
		/* ����Ϣд��ϵͳ��־�ļ� */
		fputs((const char *)buf, im_log_handle);
	}
	else
	{
		/* ����Ϣд����������� */
		OutputDebugString((const char *)buf);
	}
}


/* �ڴ�����쳣ȱʡ������ */
ISEEMEM_API unsigned long CALLAGREEMENT im_execpt_fiter(LPEXCEPTION_POINTERS per, long bln)
{
	LPIMTHREADINFO	pti;
	LPALLOLINKNODE	pa;
	unsigned char	*paddr, *fn;
	long			fln;
	LPIMWATCHINFOSTR	pwis;


	/* ���ڴ�����쳣���������� */
	if (per->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	/* ��ȡ�߳���Ϣ�ṹ */
	pti = im_get_tls();

	assert(pti);

	/* ������ѱ���������������쳣�����ٽ��з�������ʾ */
	if (pti->except_mark & IM_EXCEPT_MARK_EXCED)
	{
		im_watch_pop();
		return EXCEPTION_CONTINUE_SEARCH;
	}

	EnterCriticalSection(&im_critical);

	/* �ڴ�����쳣��¼��Ӧ�ô������������ĸ�����Ϣ��Ԫ */
	assert(per->ExceptionRecord->NumberParameters == 2);

	/* ��ȡ��ǰ������Ϣ�ڵ��ַ */
	pwis = &(pti->watch_info[pti->watch_curr-1]);

	fn  = pwis->fn;
	fln	= pwis->linenum;

	assert(fn);
	
	/* ��ȡ���������쳣�������ַ */
	paddr = (unsigned char *)(per->ExceptionRecord->ExceptionInformation[1]);

	im_output_debug_info("--->WAT: ��ַ%p������%s����\n", paddr, (per->ExceptionRecord->ExceptionInformation[0] == 0) ? "��" : "д");

	/* �ܿص�ַ��� */
	if ((paddr >= im_start_addr)&&(paddr < (im_start_addr+IM_MAX_REVSIZE)))
	{
		pa = ima_search(0);

		/* ���������ѷ������� */
		while (pa)
		{
			/* �쳣�����ڷ���ҳ�ĺ�벿�������п����ǵ�ǰ��Ԫǰ��� */
			if ((paddr >= (pa->guard_addr+im_page_size/2))&&(paddr < (unsigned char *)(pa->fg_addr)))
			{
				im_output_debug_info("         [ǰԽ��] �����������%s��%d��%d��֮���ĳЩ�ܿش��룬�����ļ�%s�е�%d��Դ�������ڴ���%s����ʱ��������ǰԽ��������ע��������������쳣��λ�㡣\n", 
					_short_filename(fn), fln, bln, 
					_short_filename(pa->filename), pa->linenum, 
					(per->ExceptionRecord->ExceptionInformation[0] == 0) ? "��ȡ" : "д��");
				break;
			}
			/* ���������һ������ҳ��ǰ�벿��������Ǻ���� */
			else if ((paddr >= (pa->guard_addr+pa->guard_length*im_page_size))&&(paddr <= (pa->guard_addr+pa->guard_length*im_page_size+im_page_size/2)))
			{
				im_output_debug_info("         [��Խ��] �����������%s��%d��%d��֮���ĳЩ�ܿش��룬�����ļ�%s�е�%d��Դ�������ڴ���%s����ʱ�������˺�Խ��������ע��������������쳣��λ�㡣\n", 
					_short_filename(fn), fln, bln, 
					_short_filename(pa->filename), pa->linenum, 
					(per->ExceptionRecord->ExceptionInformation[0] == 0) ? "��ȡ" : "д��");
				break;
			}
			/* �ܿش�����ͼд�ܱ������ڴ�� */
			else if ((paddr >= (unsigned char *)pa->fg_addr)&&(paddr < ((unsigned char *)pa->fg_addr+(pa->guard_length-1)*im_page_size)))
			{
				if (pa->attrib != 0)
				{
					im_output_debug_info("         [д����] �ļ�%s��%d��%d��֮���ĳЩ�ܿش��룬��ͼ���ܱ������ڴ��У�%s��%d��Դ�������ڴ棩д�����ݣ������ܿش��룬ͬʱ��ο��������������쳣��λ�㡣\n", 
						_short_filename(fn), fln, bln, 
						_short_filename(pa->filename), pa->linenum, 
						(per->ExceptionRecord->ExceptionInformation[0] == 0) ? "��ȡ" : "д��");
				}
				else
				{
					im_output_debug_info("         [δ֪ԭ����쳣] �ļ�%s��%d��%d��֮���ĳЩ�ܿش����ڲ���%s��%d��Դ�������ڴ�ʱ�������쳣���������ϵͳ��ʱ�޷��ж������������ԭ�򣬽�����ϸ�鿴�ܿش��룡\n", 
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
			/* �����Ϣ */
			im_output_debug_info("         [��д���ͷŵ��ڴ�] �������������%s��%d��%d��֮���ĳЩ�ܿش��룬��ͼ�����ͷŵ��ڴ���в���������д������ġ��������ܿش�����ʹ������Щ�ѱ��ͷŵ��ڴ档\n", _short_filename(fn), fln, bln);
		}
	}
	else
	{
		/* �����Ϣ */
		im_output_debug_info("         [��дϵͳ�ͳ����ڴ���] �����쳣�ĵ�ַ���ڷ���ϵͳ���ڴ��У���ע��鿴%s��%d��%d��֮����ܿش��룬ͬʱҲ��ο��������������쳣��λ�㡣\n", _short_filename(fn), fln, bln);
	}

	/* �ô����� */
	pti->except_mark |= IM_EXCEPT_MARK_EXCED;

	im_watch_pop();
		
	LeaveCriticalSection(&im_critical);

	/* �����쳣����ʹ��������λ�����쳣�����Ĵ����� */
	return EXCEPTION_CONTINUE_SEARCH;
}


/* ȫ�������� */
ISEEMEM_API void CALLAGREEMENT im_check_all(long bln, int mark)
{
	LPALLOLINKNODE	pa;
	LPIMTHREADINFO	pti;
	unsigned char	*fn;
	long			fln;
	LPIMWATCHINFOSTR	pwis;
	
	EnterCriticalSection(&im_critical);

	/* ��ȡ�߳���Ϣ�ṹ */
	pti = im_get_tls();
	
	assert(pti);

	/* ��ȡ��ǰ������Ϣ�ڵ��ַ */
	pwis = &(pti->watch_info[pti->watch_curr-1]);
	
	fn  = pwis->fn;
	fln	= pwis->linenum;
	
	assert(fn&&fln&&bln);

	pa = ima_search(0);	/* ����ѷ�������ͷ�ڵ��ַ */

	while (pa)
	{
		switch (_check_overflow(pa))
		{
		case	0:			/* ��������� */
			break;
		case	1:			/* ǰ��� */
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [ǰ���] �ڽ���%s��%d�д��봦�ķ�������ǰ������ϵͳ��⵽%s�е�%d�д��������ڴ淢����ǰд�������˲顣\n", _short_filename(fn), fln, _short_filename(pa->filename), pa->linenum);
			}
			else
			{
				im_output_debug_info("--->WAT: [ǰ���] �ļ�%s�е�%d�д��������ڴ淢����ǰд�������������Ĵ�����%s��%d��%d��֮�䣬����ϸ�˲顣\n", _short_filename(pa->filename), pa->linenum, _short_filename(fn), fln, bln);
			}
			break;
		case	2:			/* ����� */
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [�����] �ڽ���%s��%d�д��봦�ķ�������ǰ������ϵͳ��⵽%s�е�%d�д��������ڴ淢���˺�д�������˲顣\n", _short_filename(fn), fln, _short_filename(pa->filename), pa->linenum);
			}
			else
			{
				im_output_debug_info("--->WAT: [�����] �ļ�%s�е�%d�д��������ڴ淢���˺�д�������������Ĵ�����%s��%d��%d��֮�䣬����ϸ�˲顣\n", _short_filename(pa->filename), pa->linenum, _short_filename(fn), fln, bln);
			}
			break;
		case	3:			/* ǰ�������� */
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [ǰ�������] �ڽ���%s��%d�д��봦�ķ�������ǰ������ϵͳ��⵽%s�е�%d�д��������ڴ淢����ǰ����д�������˲顣\n", _short_filename(fn), fln, _short_filename(pa->filename), pa->linenum);
			}
			else
			{
				im_output_debug_info("--->WAT: [ǰ�������] �ļ�%s�е�%d�д��������ڴ淢����ǰ����д�������������Ĵ�����%s��%d��%d��֮�䣬����ϸ�˲顣\n", _short_filename(pa->filename), pa->linenum, _short_filename(fn), fln, bln);
			}
			break;
		default:
			if (mark == 0)
			{
				im_output_debug_info("--->WAT: [����ϵͳ�쳣] �ڽ���%s��%d�д��봦�ķ�������ǰ������ϵͳ�������ϵͳ�ƺ����ڲ��ȶ�״̬�������˳�����\n", _short_filename(fn), fln);
			}
			else
			{
				im_output_debug_info("--->WAT: [����ϵͳ�쳣] ����ϵͳ���ڲ��ȶ�״̬�������˳����򣡣�����λ��%s��%d��%d�У�\n", _short_filename(fn), fln, bln);
			}
			assert(0);		/* ϵͳ�쳣 */
			break;
		}
		pa = pa->next;
	}

	LeaveCriticalSection(&im_critical);
}


/* ����ָ�����ڴ�飨�����ڴ���������Ϊֻ����*/
ISEEMEM_API void CALLAGREEMENT im_protect(unsigned char *puser)
{
	LPALLOLINKNODE	pa;
	int				rs;
	unsigned long	oldprot;

	assert(puser);

	EnterCriticalSection(&im_critical);

	/* �����ڴ��Ľڵ���Ϣ */
	pa = ima_search(puser);

	assert(pa);				/* ָ�����ڴ�����Ƿ���ϵͳ������ڴ棬���������˶��� */

	if (pa->attrib != 0)	/* ���ڴ���Ѵ����ܱ���״̬ */
	{
		pa->attrib++;		/* ����������һ */
		LeaveCriticalSection(&im_critical);
		return;
	}

	/* ���ڴ�����������Ϊֻ�� */
	rs = (int)VirtualProtect((void*)(pa->fg_addr), pa->begin_length, PAGE_READONLY, &oldprot);

	assert(rs);				/* ��������ɹ�������˵��ϵͳ�Ѳ��ȶ� */

	pa->attrib = 1;

	LeaveCriticalSection(&im_critical);
}


/* �Ᵽ��ָ���ڴ�飨�����ڴ���������Ϊ��д��*/
ISEEMEM_API void CALLAGREEMENT im_unprotect(unsigned char *puser)
{
	LPALLOLINKNODE	pa;
	int				rs;
	unsigned long	oldprot;
	
	assert(puser);
	/* �û�ָ�����ܱ����ڴ�����Ƿ���ϵͳ������ڴ� */
	assert((puser>=im_start_addr)&&(puser<(im_start_addr+IM_MAX_REVSIZE)));

	EnterCriticalSection(&im_critical);
	
	/* �����ڴ��Ľڵ���Ϣ */
	pa = ima_search(puser);

	/* ����Ҳ������ڴ��Ӧ�Ľڵ㣬��˵���û�������ǰ�ͷ��˸��ڴ�� */
	if (!pa)
	{
		LeaveCriticalSection(&im_critical);
		return;
	}
	
	if (pa->attrib == 0)	/* ���ڴ�鲢δ�����ܱ���״̬ */
	{
		LeaveCriticalSection(&im_critical);
		return;
	}
	else if (pa->attrib > 1)
	{
		pa->attrib--;		/* ����������һ���ڴ���Դ����ܱ���״̬ */
		LeaveCriticalSection(&im_critical);
		return;
	}
	
	/* ���ڴ�����������Ϊ�ɶ�д */
	rs = (int)VirtualProtect((void*)(pa->fg_addr), pa->begin_length, PAGE_READWRITE, &oldprot);
	
	assert(rs);				/* ��������ɹ�������˵��ϵͳ�Ѳ��ȶ� */
	
	pa->attrib = 0;			/* �ڴ��Ϊ�ɶ�д״̬ */
	
	LeaveCriticalSection(&im_critical);
}


/* ��ȡ�̱߳��ش洢��TLS�������ַ��Ҳ���߳��������Ϣ�ṹ��ַ��*/
ISEEMEM_API LPIMTHREADINFO CALLAGREEMENT im_get_tls()
{
	LPIMTHREADINFO	pti;

	assert(im_tls_index);

	/* ȡ���߳��Լ�����Ϣ�ṹ */
	pti = (LPIMTHREADINFO)TlsGetValue(im_tls_index);
	
	/* ���Ա�����ȡ�ظýṹ������˵������ϵͳ�Ѳ����� */
	assert(pti);
	
	return pti;
}
	

/* ��ʼһ�����飨ѹ���������Ϣ��*/
ISEEMEM_API void CALLAGREEMENT im_watch_push(char *fn, long line)
{
	LPIMTHREADINFO pti = im_get_tls();

	/* ���û��ȡ���߳�˽�����ݣ���˵������ϵͳ�Ѳ��ȶ� */
	assert(pti);
	/* ��������˴˶��ԣ���˵���û���watch{}end��Ƕ�ײ�ι��� */
	assert(pti->watch_curr < IM_WATCHINFONUM);
	/* ��ʼ���ڵ���� */
	memset((void*)&(pti->watch_info[pti->watch_curr]), 0, sizeof(IMWATCHINFOSTR));

	pti->watch_info[pti->watch_curr].fn      = fn;
	pti->watch_info[pti->watch_curr].linenum = line;

	/* �����쳣�����־ */
	pti->except_mark = IM_EXCEPT_MARK_INIT;

	/* ��ǰָ���һ�����ƶ�ջָ�룩*/
	pti->watch_curr++;
}

	
/* ����һ�����飨���ܴ��쳣�������е��ã�Ҳ���ܴӼ���β���ã�*/
ISEEMEM_API void CALLAGREEMENT im_watch_pop()
{
	LPIMTHREADINFO pti = im_get_tls();

	/* ���û��ȡ���߳�˽�����ݣ���˵������ϵͳ�Ѳ��ȶ� */
	assert(pti);
	/* ��������˴˶��ԣ���˵���û���watch{}end���δ���׳��֣�ûдwatch��ֱ��д��end��*/
	assert(pti->watch_curr > 0);

	/* ��ǰָ���һ��ָ�������һ��������Ϣ�ڵ� */
	pti->watch_curr--;

	/* ������� */
	while (pti->watch_info[pti->watch_curr].prot_count--)
	{
		im_unprotect(pti->watch_info[pti->watch_curr].prot_addr[pti->watch_info[pti->watch_curr].prot_count]);
	}
}


/* ����ָ���ڴ�飨�����ڴ���������Ϊֻ����*/
ISEEMEM_API void CALLAGREEMENT im_watch_protect(void *p1, ...)
{
	char			*p = (char*)p1;
	va_list			va;
	int				watcurr, i;
	LPIMTHREADINFO	pti = im_get_tls();
	
	/* ���û��ȡ���߳�˽�����ݣ���˵������ϵͳ�Ѳ��ȶ� */
	assert(pti);
	/* ��������˴˶��ԣ���˵���û���watch{}end���δ���׳��֣�ûдwatch��*/
	assert(pti->watch_curr > 0);
	
	/* ��ȡ�����һ��������Ϣ�ڵ�� */
	watcurr = pti->watch_curr - 1;
	/* ��������˴˶��ԣ���˵���û���һ��watch{}end���б�����̫����ڴ�� */
	assert(pti->watch_info[watcurr].prot_count < IM_MAX_PROTECT);

	va_start(va, p1);
	while (p != 0)
	{
		/* �������ڴ���ڱ���watch{}end���Ƿ��ѱ����� */
		for (i=0; i<(int)(pti->watch_info[watcurr].prot_count); i++)
		{
			if (pti->watch_info[watcurr].prot_addr[i] == (unsigned char *)p)
			{
				break;
			}
		}

		/* ������ڴ���ѱ�����������Դ˴β������������ */
		if (i == (int)(pti->watch_info[watcurr].prot_count))
		{
			/* ��������˴˶��ԣ���˵���û���һ��watch{}end���б�����̫����ڴ�� */
			assert(i < IM_MAX_PROTECT);

			im_protect((unsigned char *)p);

			pti->watch_info[watcurr].prot_addr[i] = (unsigned char *)p;
			pti->watch_info[watcurr].prot_count++;
		}

		/* ��ȡ��һ��ָ����� */
		p = va_arg(va, char*);
	}
	va_end(va);
}


/* ���뱣���� */
static int CALLAGREEMENT _alloc_reserves_area()
{
	assert(im_start_addr == 0);		/* �����ظ����ñ����� */

	im_start_addr = (unsigned char *)VirtualAlloc(0, IM_MAX_REVSIZE, MEM_RESERVE, PAGE_NOACCESS);

	return (im_start_addr != 0);
}


/* �ͷű����� */
static void CALLAGREEMENT _free_reserves_area()
{
	assert(im_start_addr);

	VirtualFree(im_start_addr, IM_MAX_REVSIZE, MEM_DECOMMIT);
	VirtualFree(im_start_addr, 0, MEM_RELEASE);
}


/* ��������ڴ�ҳ�ĳߴ� */
static int CALLAGREEMENT _get_page_size()
{
	SYSTEM_INFO		si;

	GetSystemInfo(&si);

	return (int)(si.dwPageSize);
}


/* ����ָ���û��������ߴ��ҳ�� */
static int CALLAGREEMENT _cal_page_count(int user_len)
{
	int length;

	assert((user_len)&&(user_len < IM_MAX_REVSIZE));

	/* ǰ�����ṹ �� �û����� �� ������ṹ */
	length = sizeof(FGUARDSTRUCT) + user_len + sizeof(BGUARDSTRUCT);
	/* ����ҳ�� */
	length = (length+im_page_size-1)/im_page_size;
	/* ����ǰ�÷���ҳ */
	length += 1;

	assert(length);

	return length;
}


/* ����ָ���ߴ��ҳ������ */
static int CALLAGREEMENT _alloc_pages(unsigned char *paddr, int pages)
{
	MEMORY_BASIC_INFORMATION	mbi;
	unsigned char				*puser;

	assert(paddr&&(pages>=2));
	assert((paddr>=im_start_addr)&&(paddr<(im_start_addr+IM_MAX_REVSIZE)));

	/* ����ҳ������У�� */
	VirtualQuery(paddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));

	assert((mbi.State == MEM_RESERVE)&&((int)mbi.RegionSize >= pages*im_page_size));

	/* �ύҳ�棨ǰ����ҳ���⣩*/
	puser = (unsigned char *)VirtualAlloc((void*)(unsigned char *)(paddr+im_page_size), 
											(pages-1)*im_page_size, 
											MEM_COMMIT, 
											PAGE_READWRITE);
	if (puser == 0)
	{
		return -1;		/* �ڴ治�� */
	}

	assert((unsigned char *)(paddr+im_page_size) == puser);

	return 0;			/* �ɹ� */
}


/* �ͷ�ָ���ߴ��ҳ������ */
static void CALLAGREEMENT _free_pages(unsigned char *paddr, int pages)
{
	int							rs;
	MEMORY_BASIC_INFORMATION	mbi;
	
	assert(paddr&&(pages>=2));
	assert((paddr>=im_start_addr)&&(paddr<(im_start_addr+IM_MAX_REVSIZE)));
	
	/* ����ҳ������У�� */
	VirtualQuery(paddr, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	assert((mbi.State == MEM_RESERVE)&&((int)mbi.RegionSize == im_page_size));
	VirtualQuery(paddr+im_page_size, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
	assert((mbi.State == MEM_COMMIT)&&((int)mbi.RegionSize == (pages-1)*im_page_size));

	/* �ͷ�ҳ�� */
	rs = VirtualFree(paddr+im_page_size, (pages-1)*im_page_size, MEM_DECOMMIT);

	/* ����ͷ�ʧ�ܣ���˵������ϵͳ�������쳣������޷��������У������ö�����ʾ����Ա */
	assert(rs);
}


/* ������ */
static int CALLAGREEMENT _check_overflow(LPALLOLINKNODE p)
{
	int rs = 0;
	unsigned long	ult;
	unsigned char	*pt;

	assert(p&&(p->magic == IM_ALLOLINK_MAGIC));

	if ((p->fg_addr->magic != IM_GUARD_MAGIC)||(p->fg_addr->f_guard != IM_GUARD_FILL))
	{
		rs |= 1;		/* ǰ������� */
	}

	if ((p->bg_addr->magic != IM_GUARD_MAGIC)||(p->bg_addr->b_guard != IM_GUARD_FILL))
	{
		rs |= 2;		/* ��������� */
	}

	/* �����һҳʣ���ڴ��ֽ��� */
	ult = (p->guard_length-1)*im_page_size - p->begin_length;

	/* ���һҳʣ���ֽڱ�Ȼ��С��ҳ�ߴ� */
	assert(ult < (unsigned long)im_page_size);

	if (ult)
	{
		pt = (unsigned char *)(((unsigned char *)p->fg_addr)+p->begin_length);
		assert(pt == (((unsigned char *)p->bg_addr)+sizeof(BGUARDSTRUCT)));

		while (ult--)
		{
			if (*pt++ != IM_SHIT_FILL)
			{
				rs |= 2;	/* ��������� */
				break;
			}
		}
	}
	
	return rs;			/* 0 ��ʾû���쳣���� */
}


/* ȡ��ȫ·���еĸ��Դ���ַ */
static char * CALLAGREEMENT _short_filename(char *fnfull)
{
	char *p;

	assert(fnfull);
	assert(strlen(fnfull) < MAX_PATH);

	/* ����ȫ·�������һ��'\'�������ڵ�λ�� */
	p = strrchr((const char *)fnfull, (int)(unsigned int)(unsigned char)'\\');

	/* ȫ·���б�Ȼ��������һ��'\'���� */
	assert(p);

	/* ��������ļ����ڸ�Ŀ¼����ֻ�����ļ��� */
	if (*(p-1) == ':')
	{
		return (p+1);
	}
	else
	{
		/* ������һ����Ŀ¼��Ϣ */
		while (*(--p) != '\\');
		return (p+1);
	}
}


