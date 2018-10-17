/********************************************************************

	pix.c

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

			yzfree##sina.com
	----------------------------------------------------------------
	本文件用途：	ISee图像浏览器—Alias PIX图像读写模块实现文件

					读取功能：可读取8、24位RLE压缩Alias PIX图象
							  
					保存功能：24位RLE压缩格式

	本文件编写人：	
					YZ			yzfree##yeah.net

	本文件版本：	020507
	最后修改于：	2002-5-7

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
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

#include "pix.h"


IRWP_INFO			pix_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	pix_get_info_critical;	/* pix_get_image_info函数的关键段 */
CRITICAL_SECTION	pix_load_img_critical;	/* pix_load_image函数的关键段 */
CRITICAL_SECTION	pix_save_img_critical;	/* pix_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _pix_is_valid_img(LPINFOSTR pinfo_str);

static enum EXERESULT CALLAGREEMENT _read_line_alias24(ISFILE *, unsigned char *, int);
static enum EXERESULT CALLAGREEMENT _read_line_alias8(ISFILE *, unsigned char *, int);
static enum EXERESULT CALLAGREEMENT _write_line_alias24(ISFILE *, unsigned char *, int);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&pix_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&pix_get_info_critical);
			InitializeCriticalSection(&pix_load_img_critical);
			InitializeCriticalSection(&pix_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&pix_get_info_critical);
			DeleteCriticalSection(&pix_load_img_critical);
			DeleteCriticalSection(&pix_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PIX_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pix_irwp_info;
}

#else

PIX_API LPIRWP_INFO CALLAGREEMENT pix_get_plugin_info()
{
	_init_irwp_info(&pix_irwp_info);

	return (LPIRWP_INFO)&pix_irwp_info;
}

PIX_API void CALLAGREEMENT pix_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PIX_API void CALLAGREEMENT pix_detach_plugin()
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
	lpirwp_info->irwp_function = IRWP_READ_SUPP | IRWP_WRITE_SUPP;

	/* 设置模块支持的保存位深 */
	/* ################################################################# */
	/* 各位数减一，否则32位格式无法表示。此处请用PIX正确的位深填写！*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1－只能保存一副图像 */
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
				(const char *)"PIX.... 简单的一种格式，早期黑客风格:)");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 3;

	/* 下面这些格式都是8位深的图象 */
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"MATTE");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"ALPHA");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[2]), 
		(const char *)"MASK");
	
	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
PIX_API int CALLAGREEMENT pix_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PIX_HEADER		pixheader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&pix_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 读取文件头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&pixheader, sizeof(PIX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 转为Intel字序 */
			pixheader.width   = EXCHANGE_WORD((pixheader.width));
			pixheader.height  = EXCHANGE_WORD((pixheader.height));
			pixheader.bitcount= EXCHANGE_WORD((pixheader.bitcount));

			/* 判断是否是有效的Alias PIX图象（这种格式非常简单，所以
			   无法做更严格的判断，发生错判的可能性较其它格式稍大一些）*/
			if ((pixheader.width == 0) || (pixheader.height == 0) || \
				((pixheader.bitcount!=24)&&(pixheader.bitcount!=8)))
			{
				b_status = ER_NONIMAGE; __leave; 
			}

			/* 填写数据包 */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_PIX;			/* 图像文件格式（后缀名） */

			pinfo_str->compression  = ICS_RLE8;

			pinfo_str->width	= (unsigned long)pixheader.width;
			pinfo_str->height	= (unsigned long)pixheader.height;
			pinfo_str->order	= 0;					/* PIX 图都为正向图 */
			pinfo_str->bitcount	= (unsigned long)pixheader.bitcount;
		
			/* 填写掩码信息 */
			switch (pixheader.bitcount)
			{
			case	24:
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
				break;
			case	8:
				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}


			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pix_get_info_critical);
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
PIX_API int CALLAGREEMENT pix_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, linesize;
	unsigned char	*p;
	
	PIX_HEADER		pixheader;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pix_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 读文件头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&pixheader, sizeof(PIX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 转为Intel字序 */
			pixheader.width   = EXCHANGE_WORD((pixheader.width));
			pixheader.height  = EXCHANGE_WORD((pixheader.height));
			pixheader.bitcount= EXCHANGE_WORD((pixheader.bitcount));
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if ((pixheader.width == 0) || (pixheader.height == 0) || \
					((pixheader.bitcount!=24)&&(pixheader.bitcount!=8)))
				{
					b_status = ER_NONIMAGE; __leave; 
				}
				
				/* 填写数据包 */
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_PIX;			/* 图像文件格式（后缀名） */
				
				pinfo_str->compression  = ICS_RLE8;
				
				pinfo_str->width	= (unsigned long)pixheader.width;
				pinfo_str->height	= (unsigned long)pixheader.height;
				pinfo_str->order	= 0;					/* PIX 图都为正向图 */
				pinfo_str->bitcount	= (unsigned long)pixheader.bitcount;
				
				switch (pixheader.bitcount)
				{
				case	24:
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
					break;
				case	8:
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					pinfo_str->a_mask = 0x0;
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave; break;
				}
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}

			/* 人工合成灰度图调色板 */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

				for (i=0; i<(int)pinfo_str->pal_count; i++)
				{
					pinfo_str->palette[i] = (unsigned long)((i<<16)|(i<<8)|(i<<0));
				}
			}
			else
			{
				pinfo_str->pal_count = 0;
			}

			
			/* 取得扫描行尺寸 */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			
			/* 设置图像个数 */
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;
			
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

			/* 填写行首地址数组（正向）*/
			for (i=0; i<(int)(pinfo_str->height); i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}
			

			/* 读哇..... */
			for (i=0; i<(int)(pinfo_str->height); i++)
			{
				p = pinfo_str->p_bit_data+linesize*i;

				/* 逐行读取、解压缩 */
				if (pinfo_str->bitcount == 24)
				{
					if ((b_status=_read_line_alias24(pfile, p, (int)pinfo_str->width)) != ER_SUCCESS)
					{
						__leave;
					}
				}
				else
				{
					if ((b_status=_read_line_alias8(pfile, p, (int)pinfo_str->width)) != ER_SUCCESS)
					{
						__leave;
					}
				}

				pinfo_str->process_current = i;

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
			}

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pix_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
PIX_API int CALLAGREEMENT pix_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	int				i;
	unsigned char	*p;
	
	PIX_HEADER		pixheader;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pix_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_pix_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 填写流头结构 */
			pixheader.width   = (unsigned short)pinfo_str->width;
			pixheader.height  = (unsigned short)pinfo_str->height;
			pixheader.rev0    = 0;
			pixheader.rev1    = 0;
			assert(pinfo_str->bitcount == 24);
			pixheader.bitcount= 24;

			/* 转换为MOTO字序 */
			pixheader.width   = EXCHANGE_WORD((pixheader.width));
			pixheader.height  = EXCHANGE_WORD((pixheader.height));
			pixheader.bitcount= EXCHANGE_WORD((pixheader.bitcount));
			
			
			/* 写入流头结构 */
			if (isio_write((const void *)&pixheader, sizeof(PIX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 逐行编码（RLE8）并写入目标流 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				p = (unsigned char *)pinfo_str->pp_line_addr[i];

				if ((b_status=_write_line_alias24(pfile, p, (int)pinfo_str->width)) != ER_SUCCESS)
				{
					__leave;
				}

				pinfo_str->process_current = i;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}


			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pix_save_img_critical);
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


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _pix_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(pix_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (pix_irwp_info.irwp_save.img_num)
		if (pix_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */

	return 0;
}


/* 读取一行24位RLE压缩数据 */
static enum EXERESULT CALLAGREEMENT _read_line_alias24(ISFILE *pfile, unsigned char *pix, int size)
{
	int				i;
	int				count = 0;
	unsigned char	buffer[4];

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pix&&size);

	__try
	{
		while (count < size)
		{
			/* 读取一组压缩数据（每组4字节，第一个字节为重复计数值，后
			   三个字节为B、G、R数据 */
			if (isio_read((void*)buffer, 1, 4, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 重复计数值 */
			count += (int)(unsigned int)(unsigned char)(buffer[0]);

			if (count > size)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 向数据包填写重复数据 */
			for (i= 0; i<(int)(unsigned int)(unsigned char)buffer[0]; i++)
			{
				*pix++ = buffer[1]; *pix++ = buffer[2]; *pix++ = buffer[3];
			}
		}
	}
	__finally
	{
	  ;
	}

	return b_status;
}


/* 读取一行8位RLE压缩数据 */
static enum EXERESULT CALLAGREEMENT _read_line_alias8(ISFILE *pfile, unsigned char *pix, int size)
{
	int				i;
	int				count = 0;
	unsigned char	buffer[2];

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pix&&size);

	__try
	{
		while (count < size)
		{
			/* 读取一组压缩数据（每组2字节，第一个字节为重复计数值，后
			   一个字节为颜色索引值 */
			if (isio_read((void*)buffer, 1, 2, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 重复计数值 */
			count += (int)(unsigned int)(unsigned char)(buffer[0]);

			if (count > size)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 向数据包填写重复数据 */
			for (i= 0; i<(int)(unsigned int)(unsigned char)buffer[0]; i++)
			{
				*pix++ = buffer[1];
			}
		}
	}
	__finally
	{
	  ;
	}

	return b_status;
}


/* 压缩一行数据到24位RLE */
static enum EXERESULT CALLAGREEMENT _write_line_alias24(ISFILE *pfile, unsigned char *pix, int size)
{
	int				i;
	int				count;
	unsigned char	buffer[4];

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pix&&size);

	__try
	{
		for (count=0, i=0; i<size; i++)
		{
			if (count == 0)
			{
				buffer[0] = 1;
				buffer[1] = *pix++;	/* B */
				buffer[2] = *pix++;	/* G */
				buffer[3] = *pix++;	/* R */
				count = 1;
			}
			else
			{
				if ((*(pix+0) == buffer[1])&&(*(pix+1) == buffer[2])&&\
					(*(pix+2) == buffer[3])&&(count < 255))
				{
					count++;
					buffer[0] = (unsigned char)count;
					pix += 3;
				}
				else
				{
					/* 写入一组压缩数据 */
					if (isio_write((const void *)buffer, 1, 4, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					count = 0;

					/* 空操作，循环值不变 */
					i--;
				}
			}
		}

		/* 写入剩余的数据 */
		if (count != 0)
		{
			if (isio_write((const void *)buffer, 1, 4, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
		}
	}
	__finally
	{
	  ;
	}

	return b_status;
}
