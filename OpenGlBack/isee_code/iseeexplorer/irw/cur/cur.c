/********************************************************************

	cur.c

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
			http://www.vchelp.net
			http://www.chinafcu.com

	或发信到：

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—CUR图像读写模块实现文件

					读取功能：1、4、8、16、24、32位光标图象
							  
					保存功能：不支持
							   

	本文件编写人：	
					YZ		yzfree##yeah.net

	本文件版本：	20327
	最后修改于：	2002-3-27

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-3		发布新版模块。去除了所有与Windows系统有关
							的API调用，使代码更容易移植。
			2000-8		第一个发布版。


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

#include "cur.h"


IRWP_INFO			cur_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	cur_get_info_critical;	/* cur_get_image_info函数的关键段 */
CRITICAL_SECTION	cur_load_img_critical;	/* cur_load_image函数的关键段 */
CRITICAL_SECTION	cur_save_img_critical;	/* cur_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _read_cur_header(ISFILE *file, CURDIR **cur);
int CALLAGREEMENT _get_mask(int bitcount, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
int CALLAGREEMENT _read_image_block(ISFILE *file, unsigned long offset, unsigned long len, unsigned char **image);
int CALLAGREEMENT _compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand);
int CALLAGREEMENT _conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark);

static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&cur_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&cur_get_info_critical);
			InitializeCriticalSection(&cur_load_img_critical);
			InitializeCriticalSection(&cur_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&cur_get_info_critical);
			DeleteCriticalSection(&cur_load_img_critical);
			DeleteCriticalSection(&cur_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

CUR_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&cur_irwp_info;
}

#else

CUR_API LPIRWP_INFO CALLAGREEMENT cur_get_plugin_info()
{
	_init_irwp_info(&cur_irwp_info);

	return (LPIRWP_INFO)&cur_irwp_info;
}

CUR_API void CALLAGREEMENT cur_init_plugin()
{
	/* 初始化多线程同步对象 */
}

CUR_API void CALLAGREEMENT cur_detach_plugin()
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
	lpirwp_info->irwp_save.img_num = 0;
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 1;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"Hello!!! 这里光标的给^_^");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
CUR_API int CALLAGREEMENT cur_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	LPCURDIR		lpir = 0;
	LPBITMAPINFOHEADER lpbmi = 0;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&cur_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 读取文件头信息 */
			switch (_read_cur_header(pfile, (CURDIR **)&lpir))
			{
			case	-1:		/* 文件读写失败 */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* 光标文件受损 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* 内存不足 */
				b_status = ER_MEMORYERR; __leave; break;
			case	-4:		/* 不是一个光标文件 */
				b_status = ER_NONIMAGE;  __leave; break;
			case	0:		/* 成功 */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_CUR;			/* 图像文件格式（后缀名） */
			pinfo_str->compression	= ICS_RGB;			/* 图像的压缩方式 */
			
			/* 读取主图象数据块 */
			switch (_read_image_block(pfile, (unsigned long)lpir->entries[0].image_offset, (unsigned long)lpir->entries[0].bytes_in_res, (unsigned char **)&lpbmi))
			{
			case	-1:		/* 文件读写失败 */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* 光标文件受损 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* 内存不足 */
				b_status = ER_MEMORYERR; __leave; break;
			case	0:		/* 成功 */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			/* 填写图像信息 */
			pinfo_str->width	= (unsigned long)lpbmi->biWidth;
			pinfo_str->height	= (unsigned long)lpbmi->biHeight/2;	/* XOR和AND 图总高度 */
			pinfo_str->order	= 1;	/* 总是倒向图 */
			pinfo_str->bitcount = (unsigned long)lpbmi->biBitCount;
			
			if (_get_mask(pinfo_str->bitcount, 
				(unsigned long *)(&pinfo_str->b_mask), 
				(unsigned long *)(&pinfo_str->g_mask), 
				(unsigned long *)(&pinfo_str->r_mask),
				(unsigned long *)(&pinfo_str->a_mask)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (lpbmi)
				free(lpbmi);
			
			if (lpir)
				free(lpir);
			
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&cur_get_info_critical);
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
CUR_API int CALLAGREEMENT cur_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	LPCURDIR		lpir = 0;
	unsigned char	*lpimg = 0;
	int				i;
	
	LPBITMAPINFOHEADER	lpbmi;
	LPSUBIMGBLOCK		lpsub = 0, lplast;
	LPSUBIMGBLOCK		subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&cur_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* ################ 下面请编写读取图像的代码 ################ */
			/* 读取文件头信息 */
			switch (_read_cur_header(pfile, (CURDIR **)&lpir))
			{
			case	-1:		/* 文件读写失败 */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* 光标文件受损 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* 内存不足 */
				b_status = ER_MEMORYERR; __leave; break;
			case	-4:		/* 不是一个光标文件 */
				b_status = ER_NONIMAGE;  __leave; break;
			case	0:		/* 成功 */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* 读取主图象数据块 */
			switch (_read_image_block(pfile, (unsigned long)lpir->entries[0].image_offset, (unsigned long)lpir->entries[0].bytes_in_res, &lpimg))
			{
			case	-1:		/* 文件读写失败 */
				b_status = ER_FILERWERR; __leave; break;
			case	-2:		/* 光标文件受损 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* 内存不足 */
				b_status = ER_MEMORYERR; __leave; break;
			case	0:		/* 成功 */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			lpbmi = (LPBITMAPINFOHEADER)lpimg;
			assert(lpbmi);
			
			pinfo_str->process_total = (int)lpir->count;
			assert(lpir->count > 0);
			pinfo_str->process_current = 0;
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_CUR;			/* 图像文件格式（后缀名） */
				pinfo_str->compression	= ICS_RGB;			/* 图像的压缩方式 */
				
				/* 填写图像信息 */
				pinfo_str->width	= (unsigned long)lpbmi->biWidth;
				pinfo_str->height	= (unsigned long)lpbmi->biHeight/2;	/* XOR和AND 图总高度 */
				pinfo_str->order	= 1;	/* 总是倒向图 */
				pinfo_str->bitcount = (unsigned long)lpbmi->biBitCount;
				
				if (_get_mask(pinfo_str->bitcount, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}
	
			/* 设置图像个数 */
			pinfo_str->imgnumbers = (unsigned long)lpir->count;
			assert(pinfo_str->psubimg == 0);
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;
			
			/* 读首帧图象到数据包中 */
			switch (_conv_image_block(lpbmi, (void *)pinfo_str, CUR_PRI_IMAGE))
			{
			case	-2:		/* 光标文件受损 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* 内存不足 */
				b_status = ER_MEMORYERR; __leave; break;
			case	0:		/* 成功 */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* 读取的首帧图象数据已无用，释放 */
			free(lpimg);
			lpimg = (unsigned char *)lpbmi = (unsigned char *)0;

			pinfo_str->process_current = 1;
			
			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			lplast = 0;

			/* 读入子图象 */
			for (i=0;i<(int)(pinfo_str->imgnumbers-1)/*主图象不算*/;i++)
			{
				if ((lpsub=_alloc_SUBIMGBLOCK()) == 0)
				{
					b_status = ER_MEMORYERR; __leave;
				}

				lpsub->number = i+1;	/* 子图像序号由 1 开始 */
				lpsub->parents = pinfo_str;

				/* 加入子图象链表尾部 */
				if (lplast == 0)
				{
					pinfo_str->psubimg = lpsub;
					lplast = lpsub;
					
					lpsub->prev = 0;
					lpsub->next = 0;
				}
				else
				{
					lpsub->prev = lplast;
					lpsub->next = 0;
					lplast->next = lpsub;
					lplast = lpsub;
				}

				/* 读取子图象数据块 */
				switch (_read_image_block(pfile, (unsigned long)lpir->entries[i+1].image_offset, (unsigned long)lpir->entries[i+1].bytes_in_res, &lpimg))
				{
				case	-1:		/* 文件读写失败 */
					b_status = ER_FILERWERR; __leave; break;
				case	-2:		/* 光标文件受损 */
					b_status = ER_BADIMAGE;  __leave; break;
				case	-3:		/* 内存不足 */
					b_status = ER_MEMORYERR; __leave; break;
				case	0:		/* 成功 */
					break;
				default:
					b_status = ER_SYSERR;    __leave; break;
				}

				lpbmi = (LPBITMAPINFOHEADER)lpimg;

				/* 转换子图象到数据包中 */
				switch (_conv_image_block(lpbmi, (void *)lpsub, CUR_SUB_IMAGE))
				{
				case	-2:		/* 光标文件受损 */
					b_status = ER_BADIMAGE;  __leave; break;
				case	-3:		/* 内存不足 */
					b_status = ER_MEMORYERR; __leave; break;
				case	0:		/* 成功 */
					break;
				default:
					b_status = ER_SYSERR;    __leave; break;
				}

				free(lpimg);
				lpimg = (unsigned char *)lpbmi = (unsigned char *)0;
				
				pinfo_str->process_current++;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
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

			if (lpimg)
				free(lpimg);
			
			if (lpir)
				free(lpir);
			
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&cur_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
CUR_API int CALLAGREEMENT cur_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&cur_save_img_critical);
	
			b_status = ER_NOTSUPPORT;

		}
		__finally
		{
			LeaveCriticalSection(&cur_save_img_critical);
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


/* 获取CUR头结构（包括图像个数、图像尺寸等），并进行简单的鉴别 */
int CALLAGREEMENT _read_cur_header(ISFILE *file, CURDIR **cur)
{
	CURDIR		curdir;
	int			infosize, result = 0;
	LPCURDIR	lpir = 0;

	assert((file!=0)&&(cur != 0));

	__try
	{
		/* 定位于文件首部 */
		if (isio_seek(file, 0, SEEK_SET) == -1)
		{
			result = -1; __leave;		/* 文件读写失败 */
		}
		
		/* 读取光标文件信息头结构 */
		if (isio_read((void*)&curdir, sizeof(CURDIR), 1, file) == 0)
		{
			result = -4; __leave;		/* 不是一个光标文件 */
		}

		/* 判断文件标志 */
		if ((curdir.reserved != 0)||(curdir.type != CURTYPE))
		{
			result = -4; __leave;		/* 不是一个光标文件 */
		}

		/* 至少会存在一个图象 */
		if (curdir.count < 1)
		{
			result = -2; __leave;		/* 光标文件受损 */
		}


		/* 计算光标信息块的尺寸 */
		infosize = sizeof(CURDIR)+(curdir.count-1)*sizeof(CURDIRENTRY);

		/* 分配用于存放光标信息的缓冲区内存块 */
		if ((lpir=(LPCURDIR)malloc(infosize)) == 0)
		{
			result = -3; __leave;		/* 内存不足 */
		}

		/* 读取光标信息 */
		if (isio_seek(file, 0, SEEK_SET) == -1)
		{
			result = -1; __leave;
		}

		if (isio_read((void*)lpir, infosize, 1, file) == 0)
		{
			result = -2; __leave;		/* 光标文件受损（也有可能是文件读写错，但图象受损的可能性更大） */
		}
	}
	__finally
	{
		if ((result != 0)||(AbnormalTermination()))
		{
			if (lpir)
			{
				free(lpir);	lpir = 0;
			}
			if (result == 0)
			{
				result = -1;	/* 大多数异常可能都是由文件读写失败引起的 */
			}
		}
	}

	*cur = lpir;	/* 当发生错误或有异常时，*cur将被置 0 */

	return result;	/* 返回 0 成功，非 0 值失败。*cur指向的内存块在不需要时应释放。 */
}


/* 获取各颜色分量的值 */
int CALLAGREEMENT _get_mask(int bitcount, 
	unsigned long * pb_mask, 
	unsigned long * pg_mask, 
	unsigned long * pr_mask, 
	unsigned long * pa_mask)
{
	int result = 0;

	switch(bitcount)
	{
	case	1:
	case	4:
	case	8:
		/* 调色板位图无掩码 */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:		/* 555 格式 */
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:		/* 888 格式 */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	default:		/* 非法格式 */
		result = -1;
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}

	return result;	/* 0-成功， -1－非法的图像格式 */
}


/* 读取一个光标的象素数据块（包含BITMAPINFOHEADER结构、XOR数据和AND数据）*/
int CALLAGREEMENT _read_image_block(ISFILE *file, unsigned long offset, unsigned long len, unsigned char **image)
{
	unsigned char *	tmp = 0;
	int				result = 0;

	assert((file != 0)&&(image != 0));

	/* 数据合法性检查 */
	if ((offset == 0)||(len == 0))
	{
		return -2;		/* 光标文件受损（数据不合法）*/
	}
	
	__try
	{
		/* 定位读写位置 */
		if (isio_seek(file, offset, SEEK_SET) == -1)
		{
			result = -1; __leave;		/* 文件读写失败 */
		}
		
		if ((tmp=(unsigned char *)malloc(len)) == 0)
		{
			result = -3; __leave;		/* 内存不足 */
		}

		/* 读取图象数据 */
		if (isio_read((void*)tmp, len, 1, file) == 0)
		{
			result = -2; __leave;		/* 光标文件受损 */
		}
		
		if (((LPBITMAPINFOHEADER)tmp)->biSize != sizeof(BITMAPINFOHEADER))
		{
			result = -2; __leave;		/* 光标文件受损 */
		}
	}
	__finally
	{
		if ((result != 0)||(AbnormalTermination()))
		{
			if (tmp)
			{
				free(tmp);	tmp = 0;
			}
			if (result == 0)
			{
				result = -1;			/* 文件读写失败 */
			}
		}
	}

	*image = tmp;
	
	return result;	/* 返回 0 成功，非 0 值失败。*tmp指向的内存块在不需要时应释放。 */
}


/* 将光标数据块转为数据包块 */
int CALLAGREEMENT _conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark)
{
	LPINFOSTR		pinfo = (LPINFOSTR)0;
	LPSUBIMGBLOCK	psubinfo = (LPSUBIMGBLOCK)0;
	LPBITMAPINFO	pbi = (LPBITMAPINFO)lpbmi;
	
	unsigned char	*p, *pxor, *pand;
	unsigned long	pal_num = 0;
	int				i, linesize, linesize2;

	assert((lpbmi)&&(lpdest));
	assert((mark == CUR_PRI_IMAGE)||(mark == CUR_SUB_IMAGE));

	if (mark == CUR_PRI_IMAGE)
	{
		pinfo = (LPINFOSTR)lpdest;

		/* 设置调色板数据 */
		if (lpbmi->biBitCount <= 8)
		{
			pinfo->pal_count = 1UL << pinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? pinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > pinfo->pal_count)
			{
				return -2;		/* 图象受损 */
			}
			memmove((void*)(pinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			pinfo->pal_count = 0;
		}

		/* XOR象素扫描行尺寸 */
		linesize  = _calcu_scanline_size(pinfo->width, pinfo->bitcount);
		/* AND象素扫描行尺寸 */
		linesize2 = _calcu_scanline_size(pinfo->width, 1);
		
		assert(pinfo->p_bit_data == (unsigned char *)0);
		
		/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
		pinfo->p_bit_data = (unsigned char *)malloc(linesize * pinfo->height + 4);
		if (!pinfo->p_bit_data)
		{						
			return -3;			/* 内存不足 */
		}
		
		assert(pinfo->pp_line_addr == (void**)0);
		/* 分配行首地址数组 */
		pinfo->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo->height);
		if (!pinfo->pp_line_addr)
		{
			free(pinfo->p_bit_data); 
			pinfo->p_bit_data = 0; 
			return -3;			/* 内存不足 */
		}

		/* 初始化行首地址数组（倒向） */
		for (i=0;i<(int)(pinfo->height);i++)
		{
			pinfo->pp_line_addr[i] = (void *)(pinfo->p_bit_data+((pinfo->height-i-1)*linesize));
		}
		
		/* 计算XOR象素数据首地址 */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);

		/* 复制XOR象素数据 */
		memmove((void*)pinfo->p_bit_data, (const void *)p, (linesize*pinfo->height));

		/* 取得 XOR 象素首地址 */
		pxor = (unsigned char *)pinfo->p_bit_data;
		/* 计算 AND 象素首地址 */
		pand = p + linesize * pinfo->height;

		/* 合成图象 */
		for (i=0;i<(int)pinfo->height;i++)
		{
			if (_compose_img(pinfo->bitcount, pinfo->width, pxor, pand) != 0)
			{
				free(pinfo->p_bit_data); 
				pinfo->p_bit_data = 0; 
				free(pinfo->pp_line_addr);
				pinfo->pp_line_addr = 0;

				return -2;		/* 受损的图象 */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}
	else
	{
		psubinfo = (LPSUBIMGBLOCK)lpdest;

		/* 填写图像信息 */
		psubinfo->width		= (int)lpbmi->biWidth;
		psubinfo->height	= (int)lpbmi->biHeight/2;	/* XOR和AND 图总高度 */
		psubinfo->order		= 1;	/* 总是倒向图 */
		psubinfo->bitcount	= (int)lpbmi->biBitCount;

		if (_get_mask(psubinfo->bitcount, 
			(unsigned long *)(&psubinfo->b_mask), 
			(unsigned long *)(&psubinfo->g_mask), 
			(unsigned long *)(&psubinfo->r_mask),
			(unsigned long *)(&psubinfo->a_mask)) == -1)
		{
			return -2;		/* 图象受损 */
		}

		psubinfo->left = psubinfo->top = 0;
		psubinfo->dowith =psubinfo->userinput = psubinfo->time = 0;
		psubinfo->colorkey = -1;	/* 定为无关键色 */

		/* 设置调色板数据 */
		if (lpbmi->biBitCount <= 8)
		{
			psubinfo->pal_count = 1UL << psubinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? psubinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > (unsigned long)psubinfo->pal_count)
			{
				return -2;		/* 图象受损 */
			}
			memmove((void*)(psubinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			psubinfo->pal_count = 0;
		}

		
		/* 取得XOR象素扫描行尺寸 */
		linesize  = _calcu_scanline_size(psubinfo->width, psubinfo->bitcount);
		/* 计算AND象素扫描行尺寸 */
		linesize2 = _calcu_scanline_size(psubinfo->width, 1);
		
		assert(psubinfo->p_bit_data == (unsigned char *)0);
		
		/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
		psubinfo->p_bit_data = (unsigned char *)malloc(linesize * psubinfo->height + 4);
		if (!psubinfo->p_bit_data)
		{						
			return -3;			/* 内存不足 */
		}
		
		assert(psubinfo->pp_line_addr == (void**)0);
		/* 分配行首地址数组 */
		psubinfo->pp_line_addr = (void **)malloc(sizeof(void*) * psubinfo->height);
		if (!psubinfo->pp_line_addr)
		{
			free(psubinfo->p_bit_data); 
			psubinfo->p_bit_data = 0; 
			return -3;			/* 内存不足 */
		}
		
		/* 初始化行首地址数组（倒向） */
		for (i=0;i<(int)(psubinfo->height);i++)
		{
			psubinfo->pp_line_addr[i] = (void *)(psubinfo->p_bit_data+((psubinfo->height-i-1)*linesize));
		}
		
		/* 计算象素数据首地址 */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);
		
		/* 复制象素数据 */
		memmove((void*)psubinfo->p_bit_data, (const void *)p, (linesize*psubinfo->height));
		
		/* 计算 XOR 象素首地址 */
		pxor = (unsigned char *)psubinfo->p_bit_data;
		/* 计算 AND 象素首地址 */
		pand = p + linesize * psubinfo->height;
		
		/* 合成图象（使用XOR数据与AND数据）*/
		for (i=0;i<psubinfo->height;i++)
		{
			if (_compose_img(psubinfo->bitcount, psubinfo->width, pxor, pand) != 0)
			{
				free(psubinfo->p_bit_data); 
				psubinfo->p_bit_data = 0;
				free(psubinfo->pp_line_addr);
				psubinfo->pp_line_addr = 0;

				return -2;		/* 受损的图象 */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}

	return 0;		/* 返回 0 成功，非 0 失败 */
}


/* 合成一个扫描行的XOR图和AND图，并进行ISee位流格式转换，结果保存在XOR图中 */
int CALLAGREEMENT _compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand)
{
	unsigned char buf[32], tmp, *p = pxor, isc = 0;
	int i, j, k, l, a, bytesize;

	assert(bitcount > 0);
	assert(pxor&&pand);

	/* 一次处理 8 个象素 */
	for (i=0, k=0;i<width;i+=8, k++)
	{
		/* 取得 8 个象素的AND数据（共 1 个字节） */
		tmp = *(pand+k);

		/* 将AND数据转为XOR象素格式 */
		switch (bitcount)
		{
		case	1:
			buf[0] = tmp;
			bytesize = 1;
			break;
		case	4:
			for (l=0;l<8;l+=2)
			{
				buf[l/2]  = ((tmp>>(7-l))&0x1) ? 0xf0 : 0x0;
				buf[l/2] |= ((tmp>>(7-l-1))&0x1) ? 0xf : 0x0;
			}
			bytesize = 4;
			break;
		case	8:
			for (l=0;l<8;l++)
			{
				buf[l] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
			}
			bytesize = ((width-i) < 8) ? (width-i) : 8;
			break;
		case	16:
			for (l=0;l<8;l++)
			{
				buf[l*2] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*2+1] = buf[l*2]&0x7f;		/* 555格式 */
			}
			bytesize = ((width-i) < 8) ? (width-i)*2 : 16;
			break;
		case	24:
			for (l=0;l<8;l++)
			{
				buf[l*3] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*3+1] = buf[l*3];
				buf[l*3+2] = buf[l*3];
			}
			bytesize = ((width-i) < 8) ? (width-i)*3 : 24;
			break;
		case	32:
			for (l=0;l<8;l++)
			{
				buf[l*4] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*4+1] = buf[l*4];
				buf[l*4+2] = buf[l*4];
				buf[l*4+3] = 0;					/* 888格式 */
			}
			bytesize = ((width-i) < 8) ? (width-i)*4 : 32;
			break;
		default:
			return -1;							/* 不支持的位深度 */
			break;
		}

		/* 合成图象（一次合成 8 个象素）*/
		for (j=0;j<bytesize;j++)
		{
			*p++ ^= buf[j];
		}

		isc = 0;

		/* 将 1 位深图象转为ISee位流格式 */
		if (bitcount == 1)
		{
			for (a=0;a<8;a++)
			{
				isc |= (((*(p-1))>>(7-a))&0x1)<<a;
			}
			*(p-1) = isc;
		}

		/* 将 4 位深图象转为ISee位流格式 */
		if (bitcount == 4)
		{
			for (a=0;a<4;a++)
			{
				isc  = (*(p-4+a))>>4;
				isc |= (*(p-4+a))<<4;
				*(p-4+a) = isc;
			}
		}
	}

	return 0;	/* 返回 0 表示成功，返回非 0 值表示失败 */
}


/* 分配一个子图像节点，并初始化节点内数据 */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void)
{
	LPSUBIMGBLOCK	p_subimg = 0;

	/* 申请子图像信息节点(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* 内存不足 */
	
	/* 初始化为 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	return p_subimg;
}


/* 释放一个子图像节点，包括其中的位缓冲器及行首地址数组 */
void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);

	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);

	if (p_node->p_bit_data)
		free(p_node->p_bit_data);

	free(p_node);
}

