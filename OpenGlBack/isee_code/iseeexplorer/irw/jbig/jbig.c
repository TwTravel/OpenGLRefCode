/********************************************************************

	jbig.c

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
	本文件用途：	ISee图像浏览器—JBIG图像读写模块实现文件

					读取功能：可读取单色JBG图象（绝大部分的扫描或传真图象）
							  
					保存功能：不支持
							   
	本文件编写人：	
					YZ			yzfree##yeah.net

	本文件版本：	20530
	最后修改于：	2002-5-30

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------

    引用代码编写人及版权信息：
	----------------------------------------------------------------
	代码类别：	JBIG编解代码库(jbig kit)
	相关文件：  libjbig.h、libjbig.c、libjbigtab.c、libjbig.txt
	使用许可：  GPL
	----------------------------------------------------------------

	修正历史：

		2002-5		第一个发布版（新版）


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

#include "jbig.h"
#include "libjbig.h"


IRWP_INFO			jbig_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	jbig_get_info_critical;	/* jbig_get_image_info函数的关键段 */
CRITICAL_SECTION	jbig_load_img_critical;	/* jbig_load_image函数的关键段 */
CRITICAL_SECTION	jbig_save_img_critical;	/* jbig_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


#define	BUFFER_SIZE		(32*1024)			/* 读数据缓冲区尺寸（32KB）*/


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&jbig_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&jbig_get_info_critical);
			InitializeCriticalSection(&jbig_load_img_critical);
			InitializeCriticalSection(&jbig_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&jbig_get_info_critical);
			DeleteCriticalSection(&jbig_load_img_critical);
			DeleteCriticalSection(&jbig_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

JBIG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&jbig_irwp_info;
}

#else

JBIG_API LPIRWP_INFO CALLAGREEMENT jbig_get_plugin_info()
{
	_init_irwp_info(&jbig_irwp_info);

	return (LPIRWP_INFO)&jbig_irwp_info;
}

JBIG_API void CALLAGREEMENT jbig_init_plugin()
{
	/* 初始化多线程同步对象 */
}

JBIG_API void CALLAGREEMENT jbig_detach_plugin()
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


	/* 功能标识 （##需手动修正，如不支持保存功能请将IRWP_WRITE_SUPP标识去掉） */
	lpirwp_info->irwp_function = IRWP_READ_SUPP;

	/* 设置模块支持的保存位深 */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num  = 0;
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
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
				(const char *)"向Markus Kuhn致敬！");
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

	/* 别名个数（##需手动修正，如JBIG格式有别名请参考BMP插件中别名的设置） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"JBIG");
	
	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
JBIG_API int CALLAGREEMENT jbig_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	JBG_DEC_STATE	jds;
	unsigned char	*buf = 0, *p;
	int				len, cnt, result;
	unsigned long	xmax = 4294967295UL;
	unsigned long	ymax = 4294967295UL;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&jbig_get_info_critical);

			jbg_dec_init(&jds);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 申请读数据缓冲区 */
			if ((buf=(unsigned char *)malloc(BUFFER_SIZE)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 设定图象最大限长 */
			jbg_dec_maxsize(&jds, xmax, ymax);

			result = JBG_EAGAIN;
			
			do
			{
				/* 分段读取压缩数据 */
				if ((len=isio_read((void*)buf, 1, BUFFER_SIZE, pfile)) == 0)
				{
					if ((result == JBG_EOK_INTR)||(result == JBG_EOK))
					{
						break;
					}
					else
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				
				cnt = 0;
				
				p = buf;
				
				while (len > 0 && (result == JBG_EAGAIN || result == JBG_EOK))
				{
					/* 将数据倒入解码器 */
					result = jbg_dec_in(&jds, p, len, &cnt);
					
					p   += cnt;
					len -= cnt;
				}
			} while (result == JBG_EAGAIN || result == JBG_EOK);
			

			/* 判断指定图象是否是JBIG图象 */
			switch (result)
			{
			case	JBG_EOK:
			case	JBG_EOK_INTR:
				break;
			case	JBG_ENOMEM:
				b_status = ER_MEMORYERR; __leave; break;
			case	JBG_EAGAIN:
			case	JBG_EABORT:
			case	JBG_EMARKER:
			case	JBG_ENOCONT:
			case	JBG_EINVAL:
			case	JBG_EIMPL:
				b_status = ER_NONIMAGE;  __leave; break;
			case	JBG_ESYSERR:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* 只支持单色图象 */
			if (jbg_dec_getplanes(&jds) != 1)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_JBIG;			/* 图像文件格式（后缀名） */
			
			pinfo_str->compression  = ICS_JBIG;			/* 压缩方式 */

			pinfo_str->width	= (unsigned long)jbg_dec_getwidth(&jds);
			pinfo_str->height	= (unsigned long)jbg_dec_getheight(&jds);
			pinfo_str->order	= 0;					/* 都为正向图 */
			pinfo_str->bitcount	= (unsigned long)jbg_dec_getplanes(&jds);
			
			SETMASK_8(pinfo_str);						/* 设置掩码为全0 */
			

			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (buf)
				free(buf);

			jbg_dec_free(&jds);

			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&jbig_get_info_critical);
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
JBIG_API int CALLAGREEMENT jbig_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	JBG_DEC_STATE	jds;
	unsigned char	*buf = 0, *p, *bits;
	int				len, srclinesize, cnt, result, i, j, linesize, step;
	unsigned long	xmax = 4294967295UL;
	unsigned long	ymax = 4294967295UL;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&jbig_load_img_critical);

			jbg_dec_init(&jds);
			
			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total   = pinfo_str->height;
			pinfo_str->process_current = 0;

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 申请读数据缓冲区 */
			if ((buf=(unsigned char *)malloc(BUFFER_SIZE)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 设定图象最大限长 */
			jbg_dec_maxsize(&jds, xmax, ymax);
			
			result = JBG_EAGAIN;
			
			do
			{
				/* 分段读取压缩数据 */
				if ((len=isio_read((void*)buf, 1, BUFFER_SIZE, pfile)) == 0)
				{
					if ((result == JBG_EOK_INTR)||(result == JBG_EOK))
					{
						break;
					}
					else
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				
				cnt = 0;
				
				p = buf;
				
				while (len > 0 && (result == JBG_EAGAIN || result == JBG_EOK))
				{
					/* 将数据倒入解码器 */
					result = jbg_dec_in(&jds, p, len, &cnt);
					
					p   += cnt;
					len -= cnt;
				}
			} while (result == JBG_EAGAIN || result == JBG_EOK);
			
			
			/* 判断指定图象是否是JBIG图象 */
			switch (result)
			{
			case	JBG_EOK:
			case	JBG_EOK_INTR:
				break;
			case	JBG_ENOMEM:
				b_status = ER_MEMORYERR; __leave; break;
			case	JBG_EAGAIN:
			case	JBG_EABORT:
			case	JBG_EMARKER:
			case	JBG_ENOCONT:
			case	JBG_EINVAL:
			case	JBG_EIMPL:
				b_status = ER_NONIMAGE;  __leave; break;
			case	JBG_ESYSERR:
				b_status = ER_SYSERR;    __leave; break;
			}

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if (jbg_dec_getplanes(&jds) != 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_JBIG;			/* 图像文件格式（后缀名） */
				
				pinfo_str->compression  = ICS_JBIG;			/* 压缩方式 */
				
				pinfo_str->width	= (unsigned long)jbg_dec_getwidth(&jds);
				pinfo_str->height	= (unsigned long)jbg_dec_getheight(&jds);
				pinfo_str->order	= 0;					/* 都为正向图 */
				pinfo_str->bitcount	= (unsigned long)jbg_dec_getplanes(&jds);
				
				SETMASK_8(pinfo_str);						/* 设置掩码为全0 */
				
				pinfo_str->data_state = 1;
			}			
			
			/* 设置调色板数据 */
			assert(pinfo_str->bitcount == 1);

			pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->palette[0] = 0xffffff;
				pinfo_str->palette[1] = 0x0;
				break;
			case	4:
			case	8:
				assert(0);		/* 不应该执行到此处，下面的代码只是为将来扩展之用 */

				step = 256/(int)pinfo_str->pal_count;

				for (i=0; i<(int)pinfo_str->pal_count; i++)
				{
					pinfo_str->palette[i] = step*(pinfo_str->pal_count-i)-1;
				}
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}


			/* 取得扫描行尺寸 */
			linesize    = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			/* JBIG解压后的扫描行长度（字节边界） */
			srclinesize	= (int)((pinfo_str->bitcount*pinfo_str->width+7)/8);
			
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
			
			/* 填写行首地址数组（正向图）*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}

			/* 获取解压后的图象数据地址 */
			bits = jbg_dec_getimage(&jds, 0);
			
			/* 逐行读入 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				p = (unsigned char *)pinfo_str->pp_line_addr[i];

				memcpy((void*)p, (const void *)bits, srclinesize);

				/* 转为ISee位流格式 */
				for (j=0; j<srclinesize; j++)
				{
					CVT_BITS1(p[j]);
				}

				bits += srclinesize;

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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			}

			if (buf)
				free(buf);

			jbg_dec_free(&jds);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&jbig_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
JBIG_API int CALLAGREEMENT jbig_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&jbig_save_img_critical);
	
			/* ################ 请在下面编写保存图像的代码 ################ */
			
			b_status = ER_NOTSUPPORT;
			
			/* 暂不支持JBIG的保存功能。这种图片一般都是由扫描仪、传真机产生
			   的，所以只支持读功能即可。－YZ
			*/

			/* ############################################################ */
			
			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&jbig_save_img_critical);
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

