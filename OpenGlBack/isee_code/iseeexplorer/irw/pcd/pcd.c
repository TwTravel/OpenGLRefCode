/********************************************************************

	pcd.c

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

			isee##vip.163.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—PCD图像读写模块实现文件

					读取功能：可读取24位PCD图象（前三幅）
							  
					保存功能：不支持

	本文件编写人：	
					YZ			yzfree##yeah.net

	本文件版本：	20604
	最后修改于：	2002-6-4

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2002-6		第一个发布版（新版）

********************************************************************/


#ifndef WIN32
#if defined(_WIN32)||defined(_WINDOWS)
#define WIN32
#endif
#endif /* WIN32 */

/*###################################################################

  移植注释：以下代码使用了WIN32系统的SEH(结构化异常处理)及多线程同步
			对象“关键段”，在移植时应转为Linux的相应语句。

  #################################################################*/


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN				/* 缩短windows.h文件的编译时间 */
#include <windows.h>
#endif /* WIN32 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "pcd.h"


IRWP_INFO			pcd_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	pcd_get_info_critical;	/* pcd_get_image_info函数的关键段 */
CRITICAL_SECTION	pcd_load_img_critical;	/* pcd_load_image函数的关键段 */
CRITICAL_SECTION	pcd_save_img_critical;	/* pcd_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);

static enum EXERESULT CALLAGREEMENT _read_img(void *pinfo, int img_num, ISFILE *pfile);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);
static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK();
static LPSUBIMGBLOCK CALLAGREEMENT _allsub(int num, LPINFOSTR pinfo_str);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&pcd_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&pcd_get_info_critical);
			InitializeCriticalSection(&pcd_load_img_critical);
			InitializeCriticalSection(&pcd_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&pcd_get_info_critical);
			DeleteCriticalSection(&pcd_load_img_critical);
			DeleteCriticalSection(&pcd_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PCD_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pcd_irwp_info;
}

#else

PCD_API LPIRWP_INFO CALLAGREEMENT pcd_get_plugin_info()
{
	_init_irwp_info(&pcd_irwp_info);

	return (LPIRWP_INFO)&pcd_irwp_info;
}

PCD_API void CALLAGREEMENT pcd_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PCD_API void CALLAGREEMENT pcd_detach_plugin()
{
	/* 销毁多线程同步对象 */
}

#endif /* WIN32 */


/* 初始化插件信息结构 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info)
{
	assert(lpirwp_info);

	/* 初始化结构变量 */
	memset((void*)lpirwp_info, 0, sizeof(IRWP_INFO));

	/* 版本号。（十进制值，十位为主版本号，个位为副版本，*/
	lpirwp_info->irwp_version = MODULE_BUILDID;
	/* 插件名称 */
	strcpy((char*)(lpirwp_info->irwp_name), MODULE_NAME);
	/* 本模块函数前缀 */
	strcpy((char*)(lpirwp_info->irwp_func_prefix), MODULE_FUNC_PREFIX);


	/* 插件的发布类型。0－调试版插件，1－发布版插件 */
#ifdef _DEBUG
	lpirwp_info->irwp_build_set = 0;
#else
	lpirwp_info->irwp_build_set = 1;
#endif


	/* 功能标识 （##需手动修正） */
	lpirwp_info->irwp_function = IRWP_READ_SUPP;

	/* 设置模块支持的保存位深 */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num  = 0;
	lpirwp_info->irwp_save.count    = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 1;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"ACDSee真的很有本事，不知道他们是从哪弄来的PCD格式资料:)");
	/* ---------------------------------[1] － 第二组 -------------- */
	/* 后续开发者信息可加在此处。
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	*/
	/* ------------------------------------------------------------- */


	/* 插件描述信息（扩展名信息）*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* 别名个数（##需手动修正） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"pcds");
	
	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
PCD_API int CALLAGREEMENT pcd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	ord;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&pcd_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 确定PCD图象长度 */
			if (isio_seek(pfile, 0, SEEK_END) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 简单的通过图象长度来确定是否是PCD图象 */
			if (isio_tell(pfile) < PCD_MIN_SIZE)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 读取图象方向信息 */
			if (isio_seek(pfile, PCD_VERTICAL_LOC, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (isio_read((void*)&ord, sizeof(unsigned char), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_PCD;			/* 图像文件格式（后缀名） */
			pinfo_str->compression  = ICS_JPEGYCbCr;	/* 图像的压缩方式 */
			
			/* 填写图像信息 */
			pinfo_str->width	= PCD_1_W;				/* 取第一幅图象的尺寸数据 */
			pinfo_str->height	= PCD_1_H;
			pinfo_str->order	= ((ord&PCD_VERTICAL_MASK) != 8);
			pinfo_str->bitcount	= PCD_DEF_BITCOUNT;		/* 不支持8位深PCD */
			
			/* 填写掩码数据（24位）*/
			SETMASK_24(pinfo_str);


			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pcd_get_info_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		pinfo_str->data_state = 0;
		b_status = ER_SYSERR;
	}

	return (int)b_status;
}


/* 读取图像位数据 */
PCD_API int CALLAGREEMENT pcd_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				linesize, i;
	unsigned char	ord;
	LPSUBIMGBLOCK	subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pcd_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if (isio_seek(pfile, 0, SEEK_END) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 简单的通过图象长度来确定是否是PCD图象 */
				if (isio_tell(pfile) < PCD_MIN_SIZE)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* 读取图象方向信息 */
				if (isio_seek(pfile, PCD_VERTICAL_LOC, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)&ord, sizeof(unsigned char), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_PCD;			/* 图像文件格式（后缀名） */
				pinfo_str->compression  = ICS_JPEGYCbCr;	/* 图像的压缩方式 */
				
				pinfo_str->width	= PCD_1_W;				/* 取第一幅图象的尺寸数据 */
				pinfo_str->height	= PCD_1_H;
				pinfo_str->order	= ((ord&PCD_VERTICAL_MASK) != 8);
				pinfo_str->bitcount	= PCD_DEF_BITCOUNT;		/* 不支持8位深PCD */
				
				SETMASK_24(pinfo_str);

				pinfo_str->data_state = 1;
			}

			pinfo_str->process_total   = 3;		/* 插件暂时只能读取前三幅图象 */
			pinfo_str->process_current = 0;
			
			pinfo_str->pal_count = 0;

			/* 取得扫描行尺寸 */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			
			/* 设置图像个数 */
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg	  = (LPSUBIMGBLOCK)0;
			
			assert(pinfo_str->p_bit_data == (unsigned char *)0);
			/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
			pinfo_str->p_bit_data = (unsigned char *)malloc(linesize * pinfo_str->height + 4);
			if (!pinfo_str->p_bit_data)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			assert(pinfo_str->pp_line_addr == (void**)0);
			/* 分配行首地址数组 */
			pinfo_str->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo_str->height);
			if (!pinfo_str->pp_line_addr)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 填写行首地址数组 */
			if (pinfo_str->order == 0)		/* 正向 */
			{
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
				}
			}
			else							/* 倒向 */
			{
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
				}
			}
			

			/* 读入主图象 */
			if ((b_status=_read_img((void*)pinfo_str, 0, pfile)) != ER_SUCCESS)
			{
				__leave;
			}

			pinfo_str->process_current = 1;

			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			

			/* 申请第二幅图象的子节点 */
			if ((subimg_tmp=_allsub(1, pinfo_str)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 读入第二幅图象 */
			if ((b_status=_read_img((void*)subimg_tmp, 1, pfile)) != ER_SUCCESS)
			{
				__leave;
			}
			
			pinfo_str->process_current = 2;

			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			
			/* 申请第三幅图象的子节点 */
			if ((subimg_tmp=_allsub(2, pinfo_str)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 读入第三幅图象 */
			if ((b_status=_read_img((void*)subimg_tmp, 2, pfile)) != ER_SUCCESS)
			{
				__leave;
			}
			
			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}


			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* 释放子图像 */
				if (pinfo_str->psubimg != 0)
				{
					while(subimg_tmp=pinfo_str->psubimg->next)
					{
						_free_SUBIMGBLOCK(pinfo_str->psubimg);
						pinfo_str->psubimg = subimg_tmp;
					}
					_free_SUBIMGBLOCK(pinfo_str->psubimg);
					pinfo_str->psubimg = 0;
				}

				if (pinfo_str->p_bit_data)
				{
					free(pinfo_str->p_bit_data);
					pinfo_str->p_bit_data = (unsigned char *)0;
				}
				if (pinfo_str->pp_line_addr)
				{
					free(pinfo_str->pp_line_addr);
					pinfo_str->pp_line_addr = (void**)0;
				}
				if (pinfo_str->data_state == 2)
					pinfo_str->data_state =1;	/* 自动降级 */
			}

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pcd_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
PCD_API int CALLAGREEMENT pcd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pcd_save_img_critical);
			b_status = ER_NOTSUPPORT;	/* 现在暂时不支持写功能 */			
		}
		__finally
		{
			LeaveCriticalSection(&pcd_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}





/*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*/
/* 内部辅助函数 */

/* 计算扫描行尺寸(四字节对齐) */
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* 将指定主（或子）图象读入数据包 */
static enum EXERESULT CALLAGREEMENT _read_img(void *pinfo, int img_num, ISFILE *pfile)
{
	LPINFOSTR		pinfo_str = (LPINFOSTR)pinfo;
	LPSUBIMGBLOCK	psub_str  = (LPSUBIMGBLOCK)pinfo;

	int				w, h, bitcount, linesize, i, j;
	long			pos;
	unsigned char	*y1 = 0, *y2 = 0, *cbcr = 0, *pbits, *p;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pinfo&&pfile);
	assert((img_num < 3)&&(img_num >= 0));		/* 插件暂时只能读取前三幅图象 */

	__try
	{
		__try
		{
			/* 获取图象尺寸数据 */
			if (img_num == 0)					/* 第一幅图象（主图象）*/
			{
				w = (int)(pinfo_str->width);
				h = (int)(pinfo_str->height);
				bitcount = (int)(pinfo_str->bitcount);
				pbits = (unsigned char *)(pinfo_str->p_bit_data);
			}
			else								/* 后续图象（子图象）*/
			{
				w = (int)(psub_str->width);
				h = (int)(psub_str->height);
				bitcount = (int)(psub_str->bitcount);
				pbits = (unsigned char *)(psub_str->p_bit_data);
			}
			
			/* 计算扫描行尺寸 */
			linesize  = _calcu_scanline_size(w, bitcount);

			/* 分配合成缓冲区 */
			y1   = (unsigned char *)malloc(w);
			y2   = (unsigned char *)malloc(w);
			cbcr = (unsigned char *)malloc(w);

			if ((y1==0)||(y2==0)||(cbcr==0))
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 计算图象数据偏移 */
			switch (img_num)
			{
			case	0:
				pos = PCD_1_OFF; break;
			case	1:
				pos = PCD_2_OFF; break;
			case	2:
				pos = PCD_3_OFF; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			/* 设定读取位置 */
			if (isio_seek(pfile, pos, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* PCD图象高度一定是偶数 */
			assert((h%2) == 0);

			/* 逐行读入数据 */
			for (i=0; i<h; i+=2)
			{
				unsigned char *r, *g, *b;

				/* 读取两行的YCbCr数据（CbCr数据合用） */
				if (isio_read((void*)y1, w, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)y2, w, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)cbcr, w, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 确定象素写入位置 */
				p = pbits+(i+0)*linesize;

				/* 转换YUV数据至RGB数据 */
				for (j=0; j<w; j++)
				{
					b = p+j*3+0;
					g = p+j*3+1;
					r = p+j*3+2;
					YUV2RGB(y1[j], cbcr[j/2], cbcr[(w/2)+(j/2)], r, g, b);
				}

				/* 指向下一行地址 */
				p = pbits+(i+1)*linesize;

				/* 转换YUV数据至RGB数据 */
				for (j=0; j<w; j++)
				{
					b = p+j*3+0;
					g = p+j*3+1;
					r = p+j*3+2;
					YUV2RGB(y2[j], cbcr[j/2], cbcr[(w/2)+(j/2)], r, g, b);
				}
			}

		}
		__finally
		{
			if (y1)
				free(y1);
			if (y2)
				free(y2);
			if (cbcr)
				free(cbcr);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}

	return b_status;
}


/* 分配一个子图像节点，并初始化节点内数据 */
static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK()
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



/* 释放一个子图像节点，包括其中的位缓冲区及行首地址数组 */
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);
	assert(p_node->number > 0);
	
	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);
	
	if (p_node->p_bit_data)
		free(p_node->p_bit_data);
	
	free(p_node);
}


/* 申请并初始化子图象节点 */
static LPSUBIMGBLOCK CALLAGREEMENT _allsub(int num, LPINFOSTR pinfo_str)
{
	LPSUBIMGBLOCK	subimg_tmp;
	int				i, linesize;

	/* 申请节点 */
	if ((subimg_tmp=_alloc_SUBIMGBLOCK()) == 0)
	{
		return 0;
	}
	
	subimg_tmp->number  = num;			/* 子图像序号由 1 开始 */
	subimg_tmp->parents = pinfo_str;	/* 主图象地址 */

	if (num == 1)
	{
		subimg_tmp->prev    = 0;
		subimg_tmp->next    = 0;
		pinfo_str->psubimg  = subimg_tmp;
		pinfo_str->imgnumbers = 2;

		subimg_tmp->width	= PCD_2_W;
		subimg_tmp->height	= PCD_2_H;
	}
	else if (num == 2)
	{
		subimg_tmp->prev    = pinfo_str->psubimg;
		subimg_tmp->next    = 0;
		pinfo_str->psubimg->next  = subimg_tmp;
		pinfo_str->imgnumbers = 3;

		subimg_tmp->width	= PCD_3_W;
		subimg_tmp->height	= PCD_3_H;
	}
	else
	{
		assert(0);									/* 只能处理第二幅和第三幅图象 */
		_free_SUBIMGBLOCK(subimg_tmp);
		return 0;
	}
	
	subimg_tmp->order	= pinfo_str->order;			/* 等同与主图象方向 */
	subimg_tmp->bitcount= PCD_DEF_BITCOUNT;
	
	SETMASK_24(subimg_tmp);
	
	subimg_tmp->left = subimg_tmp->top = 0;
	subimg_tmp->dowith =subimg_tmp->userinput = subimg_tmp->time = 0;
	subimg_tmp->colorkey = -1;						/* 定为无关键色 */
	subimg_tmp->pal_count = 0;
	
	linesize = _calcu_scanline_size(subimg_tmp->width, subimg_tmp->bitcount);
	
	assert(subimg_tmp->p_bit_data == (unsigned char *)0);
	
	/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
	subimg_tmp->p_bit_data = (unsigned char *)malloc(linesize * subimg_tmp->height + 4);
	if (!subimg_tmp->p_bit_data)
	{						
		_free_SUBIMGBLOCK(subimg_tmp); return 0;
	}
	
	assert(subimg_tmp->pp_line_addr == (void**)0);
	/* 分配行首地址数组 */
	subimg_tmp->pp_line_addr = (void **)malloc(sizeof(void*) * subimg_tmp->height);
	if (!subimg_tmp->pp_line_addr)
	{
		_free_SUBIMGBLOCK(subimg_tmp); return 0;
	}
	
	/* 初始化行首地址数组（倒向） */
	if (subimg_tmp->order == 0)		/* 正向 */
	{
		for (i=0;i<(int)(subimg_tmp->height);i++)
		{
			subimg_tmp->pp_line_addr[i] = (void *)(subimg_tmp->p_bit_data+(i*linesize));
		}
	}
	else							/* 倒向 */
	{
		for (i=0;i<(int)(subimg_tmp->height);i++)
		{
			subimg_tmp->pp_line_addr[i] = (void *)(subimg_tmp->p_bit_data+((subimg_tmp->height-i-1)*linesize));
		}
	}
	
	return subimg_tmp;
}

