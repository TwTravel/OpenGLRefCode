/********************************************************************

	iseeirw.c

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
			http://iseeexplorer.cosoft.org.cn
			
	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	图像读写模块全局定义文件
	本文件编写人：	YZ				yzfree##sina.com

	本文件版本：	20329
	最后修改于：	2002-6-2

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-3		增加播放顺序表。修正相关的INFOSTR分配及释放函数
		2002-1		修正静态图像文件不能包含多幅图像的错误注释
		2001-6		第一个测试版发布

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
/* 内部用数据 */

#ifdef _DEBUG
static unsigned long sct_count = 0;		/* 结构实体计数 */
static CRITICAL_SECTION  sct_access;	/* 计数器访问许可标识 */
#endif

static char *IRWP_API_NAME_STRING[4] = { "", "get_image_info", "load_image", "save_image"};




/**********************************************************************************/
/* 内部辅助函数 */

static void CALLAGREEMENT _free_subimg(LPINFOSTR pinfo);
static int  CALLAGREEMENT _defalt_process(PFUN_COMM, int, const void *, const int);
ISEEIRW_API IRWPFUN CALLAGREEMENT isirw_set_pfun(LPINFOSTR pinfo, IRWPFUN new_pfun);

ISEEIRW_API LPSUBIMGBLOCK CALLAGREEMENT isirw_alloc_SUBIMGBLOCK();
ISEEIRW_API void CALLAGREEMENT isirw_free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);




/**********************************************************************************/
/* 接口函数 */


#ifdef WIN32

/*
  功能：测试指定DLL是否是ISee图像读写插件

  入口：
		hmode	－ DLL句柄

  返回：  0		－ 指定的DLL不是ISee的图像读写插件
		非0		－ 是

*/
ISEEIRW_API unsigned long CALLAGREEMENT isirw_test_plugin(HMODULE hmod)
{
	return (unsigned long)GetProcAddress(hmod, IRWP_TAG);
}



/*
  功能：获得ISee图像读写插件的内部信息

  入口：
		pfun_add－ 由上面的isirw_test_plugin()函数返回的值

  返回：  0		－ 函数执行失败，表明该插件已受损，无法利用
		非0		－ 函数执行成功，返回值是指向IRWP_INFO结构的地址，
				   该结构的实体在插件内部。
*/
ISEEIRW_API LPIRWP_INFO CALLAGREEMENT isirw_get_info(unsigned long pfun_add)
{
	return (pfun_add == 0UL) ? (LPIRWP_INFO)0 : (*((IRWP_ISX)pfun_add))();
}



/*
  功能：获得ISee图像读写插件接口地址

  入口：
		hmod	－ 插件模块句柄
		pinfo	－ 由isirw_get_info()函数返回的插件信息结构地址
		name	－ 要获取的接口名称（见IRWP_API_NAME的定义）

  返回：  0		－ 函数执行失败，表明该插件已受损，无法利用
		非0		－ 函数执行成功，返回值是指定接口的函数地址
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
  功能：分配一个新的数据包

  入口：

  返回：  0		－ 函数执行失败，内存不足导致的
		非0		－ 函数执行成功，返回值是新数据包的首地址
				  （注：由本函数申请的数据包必须由isirw_free_INFOSTR()释放
*/
ISEEIRW_API LPINFOSTR CALLAGREEMENT isirw_alloc_INFOSTR()
{
	LPINFOSTR ptmp;

	/* 分配结构空间 */
	if ((ptmp = (LPINFOSTR)malloc(sizeof(INFOSTR))) == (LPINFOSTR)0)
		return (LPINFOSTR)0;

#ifdef _DEBUG
	ptmp->sct_mark = INFOSTR_DBG_MARK;	/* 结构的标识，用于调试 */

	if (sct_count == 0)
		InitializeCriticalSection(&(sct_access));

	EnterCriticalSection(&(sct_access));
	sct_count++;						/* 结构实体个数 */
	LeaveCriticalSection(&(sct_access));
#endif
	
	ptmp->imgtype = IMT_NULL;			/* 图像文件类型 */
	ptmp->imgformat = IMF_NULL;			/* 图像文件格式（后缀名） */
	ptmp->compression = ICS_RGB;		/* 图像的压缩方式 */

	ptmp->width = 0;					/* 图像宽度 */
	ptmp->height = 0;					/* 图像高度 */
	ptmp->order = 0;					/* 图像的上下顺序 （0－正向，1－倒向）*/
	ptmp->bitcount = 0;					/* 每个像素所占的位数（对应于颜色数） */

	ptmp->r_mask = 0;					/* 各颜色分量的掩码值 */
	ptmp->g_mask = 0;
	ptmp->b_mask = 0;
	ptmp->a_mask = 0;

	ptmp->pp_line_addr = (void **)0;
	ptmp->p_bit_data = (unsigned char *)0;
	ptmp->pal_count = 0;
	memset((void*)(ptmp->palette), 0, MAX_PALETTE_COUNT*sizeof(unsigned long));

	ptmp->psubimg = (LPSUBIMGBLOCK)0;	/* 子图像数据链地址 */
	ptmp->imgnumbers = 0;				/* 该文件中图像的个数 */
	
	ptmp->data_state = 0;				/* 数据包当前的数据状态：
											0 － 空的数据包，没有任何变量数据是有效的
											1 － 存在图像的描述信息
											2 － 存在图像数据 */
	InitializeCriticalSection(&(ptmp->access));
	
	/*ptmp->irwpfun = _defalt_process;	/* 使用缺省回调函数（总之不能为空） */
	/*ptmp->rev0    = 0;*/
	/*ptmp->rev1    = 0;*/
	ptmp->break_mark = 0;				/* 中断标志。0－继续，1－中断操作 */
	ptmp->process_total = 0;			/* 总的操作进度值 */
	ptmp->process_current = 0;			/* 当前的操作进度 */

	ptmp->play_number= 0UL;				/* 播放顺序表尺寸（以元素为单位） */
	ptmp->play_order = 0UL;				/* 播放顺序表 */
	ptmp->time = 0;						/* 首帧停留时间 */
	ptmp->colorkey = -1;				/* 主图象透明色 */

	return ptmp;
}



/*
  功能：释放一个数据包

  入口：
		pinfo	－ 数据包首地址

  返回：  0		－ 恒定返回值（可用于数据包指针的置零操作）
*/
ISEEIRW_API LPINFOSTR CALLAGREEMENT isirw_free_INFOSTR(LPINFOSTR pinfo)
{
	assert(pinfo);
	assert(pinfo->sct_mark == INFOSTR_DBG_MARK);

	assert(sct_count);

	EnterCriticalSection(&(pinfo->access));

#ifdef _DEBUG
	EnterCriticalSection(&(sct_access));
	sct_count--;						/* 结构实体个数 */
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



/* 内部函数（只供isirw_free_INFOSTR函数使用）－ 释放数据包中的子图像链表（如果有的话） */
void CALLAGREEMENT _free_subimg(LPINFOSTR pinfo)
{
	int i;
	LPSUBIMGBLOCK	ptmp, pfree = pinfo->psubimg;

	/* 释放所有子图象节点 */
	for (i=0;i<(int)(pinfo->imgnumbers-1);i++)
	{
		ptmp = pfree->next;

		assert(pfree);

		isirw_free_SUBIMGBLOCK(pfree);

		pfree = ptmp;
	}

	pinfo->imgnumbers -= i;
	assert(pinfo->imgnumbers == 1);		/* 因为只剩下主帧图象了 */
	pinfo->psubimg = (LPSUBIMGBLOCK)0;
}



/*
  功能：分配一个子图像节点，并初始化节点内数据

  入口：

  返回：  0		－ 函数执行失败，内存不足导致的
		非0		－ 函数执行成功，返回值是子图象节点的首地址
				  （注：由本函数申请的子图象节点必须由isirw_free_SUBIMGBLOCK()释放
*/
ISEEIRW_API LPSUBIMGBLOCK CALLAGREEMENT isirw_alloc_SUBIMGBLOCK()
{
	LPSUBIMGBLOCK	p_subimg;
	
	/* 申请子图像信息节点(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* 内存不足 */
	
	/* 初始化为 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	p_subimg->number   = 1;			/* 子图象起始序号为 1 */
	p_subimg->colorkey = -1;		/* 透明色值设为无 */
	
	return p_subimg;
}



/*
  功能：释放一个子图像节点，包括其中的位缓冲区及行首地址数组

  入口：p_node	－ 待释放的子图象节点

  返回：
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
  功能：为指定数据包设置新的回调函数

  入口：pinfo	－待设定数据包
		new_pfun－新的回调函数

  返回：老的回调函数
*/
ISEEIRW_API IRWPFUN CALLAGREEMENT isirw_set_pfun(LPINFOSTR pinfo, IRWPFUN new_pfun)
{
	IRWPFUN old;

	assert(pinfo);

	if (new_pfun == 0)
	{
		old = _defalt_process;
		new_pfun = _defalt_process;		/* 回调函数不能为空 */
	}
	else
	{
		/*old = pinfo->irwpfun;*/
	}

	/*pinfo->irwpfun = new_pfun;*/

	return old;
}



/* 
  功能：缺省的进度回调函数

  入口：type	－命令类型
		img_num	－图象序号（0为主图象，其他为子图象序号）
		pinfo	－主图象数据包地址或子图象数据结构地址（根据命令类型决定）
		cur		－当前进度值（以图象扫描行为单位）

  返回：0－继续读写图象
*/
static int CALLAGREEMENT _defalt_process(PFUN_COMM type, int img_num, const void *pinfo, const int cur)
{
	int				result = 0;
	LPINFOSTR		pinfo_str = (LPINFOSTR)pinfo;
	LPSUBIMGBLOCK	psub_str  = (LPSUBIMGBLOCK)pinfo;

	(void)type, (void)img_num, (void)pinfo, (void)cur;

	switch (type)
	{
	case	PFC_NULL:						/* 空命令 */
		break;
	case	PFC_INTO_PLUG:					/* 控制进入插件 */
		break;
	case	PFC_BUILD_MAST_IMAGE:			/* 新的主图象 */
		break;
	case	PFC_BUILD_SERT_IMAGE:			/* 新的子图象 */
		break;
	case	PFC_PROCESS:					/* 步进 */
		break;
	case	PFC_OVER_SIR:					/* 结束一副图象 */
		break;
	case	PFC_OUT_PLUG:					/* 控制即将离开插件 */
		break;
	default:
		assert(0); result = 2; break;		/* 系统错误，中断插件的读写操作 */
	}
	
	return result;	/* 0－继续读写，1－用户中断，2－系统异常，强行中断操作 */
}

