/********************************************************************

	rla.c

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
	本文件用途：	ISee图像浏览器—Alias/Wavefront RLA图像读写模块实现文件

					读取功能：可读取3通道24位压缩rgb色系的图象，不支持matte、
								aux通道
							  
					保存功能：可将24位图象保存为3通道RLE8压缩的rgb色系图象

	本文件编写人：	
					YZ			yzfree##yeah.net

	本文件版本：	20509
	最后修改于：	2002-5-9

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

#include "rla.h"


IRWP_INFO			rla_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	rla_get_info_critical;	/* rla_get_image_info函数的关键段 */
CRITICAL_SECTION	rla_load_img_critical;	/* rla_load_image函数的关键段 */
CRITICAL_SECTION	rla_save_img_critical;	/* rla_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _rla_is_valid_img(LPINFOSTR pinfo_str);

static void CALLAGREEMENT _swap_byte(LPRLA_HEADER prla);
static int  CALLAGREEMENT _decode_scanline(unsigned char *, int, unsigned char *, int);
static int CALLAGREEMENT _encode_scanline(unsigned char *, int, unsigned char *);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&rla_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&rla_get_info_critical);
			InitializeCriticalSection(&rla_load_img_critical);
			InitializeCriticalSection(&rla_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&rla_get_info_critical);
			DeleteCriticalSection(&rla_load_img_critical);
			DeleteCriticalSection(&rla_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

RLA_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&rla_irwp_info;
}

#else

RLA_API LPIRWP_INFO CALLAGREEMENT rla_get_plugin_info()
{
	_init_irwp_info(&rla_irwp_info);

	return (LPIRWP_INFO)&rla_irwp_info;
}

RLA_API void CALLAGREEMENT rla_init_plugin()
{
	/* 初始化多线程同步对象 */
}

RLA_API void CALLAGREEMENT rla_detach_plugin()
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
	lpirwp_info->irwp_function = IRWP_READ_SUPP | IRWP_WRITE_SUPP;

	/* 设置模块支持的保存位深 */
	/* ################################################################# */
	/* 各位数减一，否则32位格式无法表示。此处请用RLA正确的位深填写！*/
	lpirwp_info->irwp_save.bitcount = 1UL<<(24-1);
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
				(const char *)"RLA的测试图片真难找啊！");
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

	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
RLA_API int CALLAGREEMENT rla_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	RLA_HEADER		rlaheader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&rla_get_info_critical);

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

			if (isio_read((void*)&rlaheader, sizeof(RLA_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 转换字序 */
			_swap_byte(&rlaheader);

			/* 判断是否是合法的RLA图象流 */
			if ((rlaheader.revision != (short)RLA_REVISION) || \
				((rlaheader.matte_type != 0)&&(rlaheader.matte_type != 4)) || \
				((rlaheader.storage_type != 0)&&(rlaheader.storage_type != 4)) || \
				(rlaheader.window.left > rlaheader.window.right) || \
				(rlaheader.window.bottom > rlaheader.window.top) || \
				(rlaheader.active_window.left > rlaheader.active_window.right) || \
				(rlaheader.active_window.bottom > rlaheader.active_window.top))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 只支持RGB色系的RLA图象 */
			if (strcmp((const char *)rlaheader.chan, "rgb"))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 只支持24位RLA图象，并且废弃matte通道数据（数据包暂时无法容纳matte数据）*/
			if ((rlaheader.num_chan != 3) || (rlaheader.chan_bits != 8))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 检查图象宽、高参数是否合法 */
			if (((rlaheader.active_window.right - rlaheader.active_window.left) < 0) || \
				((rlaheader.active_window.top - rlaheader.active_window.bottom) < 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}


			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_RLA;			/* 图像文件格式（后缀名） */
			pinfo_str->compression  = ICS_RLE8;

			pinfo_str->width	= (unsigned long)(rlaheader.active_window.right - rlaheader.active_window.left + 1);
			pinfo_str->height	= (unsigned long)(rlaheader.active_window.top - rlaheader.active_window.bottom + 1);
			pinfo_str->order	= 1;
			pinfo_str->bitcount	= 24;					/* 3通道、8位 */
			
			pinfo_str->b_mask = 0xff;
			pinfo_str->g_mask = 0xff00;
			pinfo_str->r_mask = 0xff0000;
			pinfo_str->a_mask = 0x0;
			

			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&rla_get_info_critical);
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
RLA_API int CALLAGREEMENT rla_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, linesize, j;
	unsigned int	*poffset = 0;
	unsigned char	*punpack = 0, *pspl = 0, *p;
	unsigned char	*pr, *pg, *pb;
	unsigned short	rl, gl, bl;
	
	RLA_HEADER		rlaheader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&rla_load_img_critical);

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

			if (isio_read((void*)&rlaheader, sizeof(RLA_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 转换字序 */
			_swap_byte(&rlaheader);
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if ( (rlaheader.revision != (short)RLA_REVISION) || \
					((rlaheader.matte_type != 0)&&(rlaheader.matte_type != 4)) || \
					((rlaheader.storage_type != 0)&&(rlaheader.storage_type != 4)) || \
					(rlaheader.window.left > rlaheader.window.right) || \
					(rlaheader.window.bottom > rlaheader.window.top) || \
					(rlaheader.active_window.left > rlaheader.active_window.right) || \
					(rlaheader.active_window.bottom > rlaheader.active_window.top) )
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				if (strcmp((const char *)rlaheader.chan, "rgb"))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				if ((rlaheader.num_chan != 3) || (rlaheader.chan_bits != 8))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				if (((rlaheader.active_window.right - rlaheader.active_window.left) < 0) || \
					((rlaheader.active_window.top - rlaheader.active_window.bottom) < 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_RLA;			/* 图像文件格式（后缀名） */
				pinfo_str->compression  = ICS_RLE8;
				
				pinfo_str->width	= (unsigned long)(rlaheader.active_window.right - rlaheader.active_window.left + 1);
				pinfo_str->height	= (unsigned long)(rlaheader.active_window.top - rlaheader.active_window.bottom + 1);
				pinfo_str->order	= 1;
				pinfo_str->bitcount	= 24;					/* 3通道、8位 */
				
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
				
				pinfo_str->data_state = 1;
			}				
			
			/* 置调色板数为 0 */
			pinfo_str->pal_count = 0;
			
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
			
			/* 填写行首地址数组 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
			
			/* 分配用于存储扫描行偏移表的内存 */
			if ((poffset=(int*)malloc(4*pinfo_str->height)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 读取偏移表 */
			if (isio_read((void*)poffset, 4*pinfo_str->height, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 转换偏移值字序 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				poffset[i] = EXCHANGE_DWORD(poffset[i]);
			}
			
			/* 分配压缩数据缓冲区 */
			if ((punpack=(unsigned char *)malloc(pinfo_str->width*(pinfo_str->bitcount/8)*2)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			pr = punpack;
			pg = pr+pinfo_str->width*2;
			pb = pg+pinfo_str->width*2;

			/* 分配解压缓冲区 */
			if ((pspl=(unsigned char *)malloc(pinfo_str->width*(pinfo_str->bitcount/8))) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			

			/* 逐行读入 */
			for (p=pinfo_str->p_bit_data, i=0; i<(int)(pinfo_str->height); i++)
			{
				/* 定位扫描行读取位置 */
				if (isio_seek(pfile, poffset[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 只读取前3个通道数据（R、G、B），matte通道暂不读取 */

				/* 读R */
				if (isio_read((void*)&rl, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				rl = EXCHANGE_WORD(rl);

				if (isio_read((void*)pr, rl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 读G */
				if (isio_read((void*)&gl, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				gl = EXCHANGE_WORD(gl);
				
				if (isio_read((void*)pg, gl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 读B */
				if (isio_read((void*)&bl, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				bl = EXCHANGE_WORD(bl);
				
				if (isio_read((void*)pb, bl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 查验数据合法性 */
				if ((rl==0)||(gl==0)||(bl==0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 解压缩 */
				if (_decode_scanline(pr, rl, pspl, (int)pinfo_str->width) != 0)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				if (_decode_scanline(pg, gl, pspl+pinfo_str->width, (int)pinfo_str->width) != 0)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				if (_decode_scanline(pb, bl, pspl+pinfo_str->width*2, (int)pinfo_str->width) != 0)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				p = pinfo_str->p_bit_data + (linesize*i);
				
				/* 合成目标象素，并写入数据包 */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					*p++ = ((unsigned char *)(pspl+pinfo_str->width*2))[j];
					*p++ = ((unsigned char *)(pspl+pinfo_str->width))[j];
					*p++ = ((unsigned char *)(pspl))[j];
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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			}

			if (pspl)
				free(pspl);

			if (punpack)
				free(punpack);

			if (poffset)
				free(poffset);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&rla_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
RLA_API int CALLAGREEMENT rla_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	int				i, j, off;
	unsigned int	*poffset = 0;
	unsigned char	*ppack = 0, *pspl = 0, *p;
	unsigned char	*pr, *pg, *pb;
	unsigned short	rl, gl, bl, l;
	
	RLA_HEADER		rlaheader;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&rla_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_rla_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			memset((void*)&rlaheader, 0, sizeof(RLA_HEADER));

			/* 填写流头结构 */
			rlaheader.window.left   = 0;
			rlaheader.window.bottom = 0;
			rlaheader.window.right  = (short)(pinfo_str->width-1);
			rlaheader.window.top    = (short)(pinfo_str->height-1);
			
			rlaheader.active_window.left   = 0;
			rlaheader.active_window.bottom = 0;
			rlaheader.active_window.right  = (short)(pinfo_str->width-1);
			rlaheader.active_window.top    = (short)(pinfo_str->height-1);

			rlaheader.revision  = (short)RLA_REVISION;

			/* 3通道，R、G、B */
			strcpy(rlaheader.chan, "rgb");
			rlaheader.num_chan  = 3;
			rlaheader.chan_bits	= 8;

			strcpy(rlaheader.desc, "Saved by ISee RLA plug-in." );
			strcpy(rlaheader.program, "ISee Explorer.");
			
			/* 图象个数 */
			rlaheader.frame = 1;

			strcpy( rlaheader.gamma, "2.2" );

			strcpy( rlaheader.red_pri  , "0.670 0.330" );
			strcpy( rlaheader.green_pri, "0.210 0.710" );
			strcpy( rlaheader.blue_pri , "0.140 0.080" );
			strcpy( rlaheader.white_pt , "0.310 0.316" );
			
			strcpy( rlaheader.aspect, "user defined" );
			
			/* 转为MOTO字序 */
			_swap_byte(&rlaheader);

			/* 设置进度值 */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			/* 写入头结构 */
			if (isio_write((const void *)&rlaheader, sizeof(RLA_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 分配偏移表 */
			if ((poffset=(unsigned int *)malloc(4*pinfo_str->height)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 分配压缩数据缓冲区 */
			if ((ppack=(unsigned char *)malloc(pinfo_str->width*(pinfo_str->bitcount/8)*2)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			pr = ppack;
			pg = pr+pinfo_str->width*2;
			pb = pg+pinfo_str->width*2;
			
			/* 分配单通道缓冲区 */
			if ((pspl=(unsigned char *)malloc(pinfo_str->width+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 定位通道数据写入的起始点（跨过头结构及偏移表） */
			if (isio_seek(pfile, sizeof(RLA_HEADER)+(4*pinfo_str->height), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			off = isio_tell(pfile);

			/* 逐行写入 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				p = (unsigned char *)(pinfo_str->pp_line_addr[i]);

				poffset[(int)pinfo_str->height-i-1] = EXCHANGE_DWORD(off);

				/* 萃取 R 通道数据 */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pspl[j] = p[j*3+2];
				}
				/* 压缩 R 通道数据 */
				rl = (unsigned short)_encode_scanline(pspl, (int)pinfo_str->width, pr);

				/* 萃取 G 通道数据 */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pspl[j] = p[j*3+1];
				}
				/* 压缩 G 通道数据 */
				gl = (unsigned short)_encode_scanline(pspl, (int)pinfo_str->width, pg);
				
				/* 萃取 B 通道数据 */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pspl[j] = p[j*3+0];
				}
				/* 压缩 B 通道数据 */
				bl = (unsigned short)_encode_scanline(pspl, (int)pinfo_str->width, pb);
				

				/* 写入压缩后的数据，顺序为：R、G、B */
				l = EXCHANGE_WORD(rl);
				if (isio_write((const void *)&l, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_write((const void *)pr, rl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				l = EXCHANGE_WORD(gl);
				if (isio_write((const void *)&l, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_write((const void *)pg, gl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				l = EXCHANGE_WORD(bl);
				if (isio_write((const void *)&l, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_write((const void *)pb, bl, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			
				pinfo_str->process_current = i;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
				
				/* 获取下一行的写入位置 */
				off = isio_tell(pfile);
			}
			
			/* 定位偏移表写入点 */
			if (isio_seek(pfile, sizeof(RLA_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 写入偏移表 */
			if (isio_write((const void *)poffset, 4*pinfo_str->height, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			
			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pspl)
				free(pspl);
			
			if (ppack)
				free(ppack);

			if (poffset)
					free(poffset);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&rla_save_img_critical);
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


/* 判断传入的图像是否可以被保存（如不支持保存功能，可将本函数去掉） */
int CALLAGREEMENT _rla_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(rla_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (rla_irwp_info.irwp_save.img_num)
	{
		if (rla_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* 图像个数不正确 */
			}
		}
	}

	return 0;
}


/* 对RLA_HEADER结构中的short域进行字序转换 */
static void CALLAGREEMENT _swap_byte(LPRLA_HEADER prla)
{
	assert(prla);
	
	prla->window.bottom = EXCHANGE_WORD(prla->window.bottom);
	prla->window.left   = EXCHANGE_WORD(prla->window.left);
	prla->window.right  = EXCHANGE_WORD(prla->window.right);
	prla->window.top    = EXCHANGE_WORD(prla->window.top);

	prla->active_window.bottom = EXCHANGE_WORD(prla->active_window.bottom);
	prla->active_window.left   = EXCHANGE_WORD(prla->active_window.left);
	prla->active_window.right  = EXCHANGE_WORD(prla->active_window.right);
	prla->active_window.top    = EXCHANGE_WORD(prla->active_window.top);

	prla->frame         = EXCHANGE_WORD(prla->frame);
	prla->storage_type  = EXCHANGE_WORD(prla->storage_type);
	prla->num_chan      = EXCHANGE_WORD(prla->num_chan);
	prla->num_matte     = EXCHANGE_WORD(prla->num_matte);
	prla->num_aux       = EXCHANGE_WORD(prla->num_aux);
	prla->revision      = EXCHANGE_WORD(prla->revision);
	prla->job_num       = EXCHANGE_DWORD(prla->job_num);
	prla->field         = EXCHANGE_WORD(prla->field);
	prla->chan_bits     = EXCHANGE_WORD(prla->chan_bits);
	prla->matte_type    = EXCHANGE_WORD(prla->matte_type);
	prla->matte_bits    = EXCHANGE_WORD(prla->matte_bits);
	prla->aux_type      = EXCHANGE_WORD(prla->aux_type);
	prla->aux_bits      = EXCHANGE_WORD(prla->aux_bits);
	prla->next          = EXCHANGE_DWORD(prla->next);
}


/* 解码一个RLE8的通道压缩数据（一个扫描行中的一个通道） */
static int CALLAGREEMENT _decode_scanline(unsigned char *src, int src_len, unsigned char *dec, int dec_len)
{
	unsigned char count;

	assert(src&&dec&&src_len&&dec_len);

	while (src_len > 0)
	{
		/* 取计数值 */
		count = *src++;
		src_len--;

		if (src_len == 0)
		{
			return 2;			/* 源数据不足 */
		}

		if (count < (unsigned char)128)
		{
			count++;

			/* 合法性检测 */
			if (((int)(unsigned int)count) > dec_len)
			{
				return 1;		/* 源数据非法 */
			}
			else
			{
				dec_len -= (int)(unsigned int)count;
			}

			/* 重复计数次 */
			while (count-- > 0)
			{
				*dec++ = *src;
			}
			src++;
			src_len--;
		}
		else
		{
			/* 拷贝未压缩的数据 */
			for (count=(unsigned char)(256-(int)(unsigned int)count); count > 0; src_len--, dec_len--, count--)
			{
				if (dec_len == 0)
				{
					return 1;
				}
				else
				{
					*dec++ = *src++;
				}
			}
		}
	}

	if (dec_len != 0)
	{
		return 3;			/* 源数据有缺损 */
	}
	else
	{
		return 0;			/* 成功解压缩 */
	}
}


/* 压缩一个通道的数据（一个扫描行中的一个通道） */
static int CALLAGREEMENT _encode_scanline(unsigned char *src, int src_len, unsigned char *dec)
{
	int				len = 0;
	unsigned char	count;
	
	assert(src&&src_len&&dec);

	while (src_len > 0)
	{
		if ((src_len>1)&&(src[0]==src[1]))
		{
			/* 累计重复字节个数 */
			for (count=2; (int)(unsigned int)count<src_len; count++)
			{
				if (src[(int)(unsigned int)count] != src[(int)(unsigned int)count-1])
				{
					break;
				}
				if ((int)(unsigned int)count >= 127)
				{
					break;
				}
			}
			
			/* 写入计数值 */
			*dec++ = count - (unsigned char)1;
			len++;
			
			/* 写入重复内容值 */
			*dec++ = *src;
			len++;

			src_len -= (int)(unsigned int)count;
			src += (int)(unsigned int)count;
		}
		else
		{
			/* 累计不重复字节个数 */
			for (count=1; (int)(unsigned int)count<src_len; count++)
			{
				if (((src_len-(int)(unsigned int)count)>1) && (src[(int)(unsigned int)count]==src[(int)(unsigned int)count+1]))
				{
					break;
				}
				if ((int)(unsigned int)count >= 127)
				{
					break;
				}
			}
			
			/* 写入计数值 */
			*dec++ = (unsigned char)((int)256 - (int)(unsigned int)count);
			len++;
			
			/* 拷贝未压缩数据 */
			for (; count-->0; len++, src_len--)
			{
				*dec++ = *src++;
			}
		}
	}
	
	return len;	/* 返回压缩后的通道数据长度 */
}




