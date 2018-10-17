/********************************************************************

	pics.c

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
	本文件用途：	ISee图像浏览器—SoftImage PIC图像读写模块实现文件

					读取功能：可读取24位、32位（带一个ALPHA通道）的
								压缩与未压缩 SoftImage PIC 图象
							  
					保存功能：可将24位图象以不压缩的方式保存为3通道
								SoftImage PIC 图象

	本文件编写人：	
					YZ			yzfree##yeah.net

	本文件版本：	20505
	最后修改于：	2002-5-5

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2002-5		发布第一个版本（新版）


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

#include "pics.h"


IRWP_INFO			pics_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	pics_get_info_critical;	/* pics_get_image_info函数的关键段 */
CRITICAL_SECTION	pics_load_img_critical;	/* pics_load_image函数的关键段 */
CRITICAL_SECTION	pics_save_img_critical;	/* pics_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _pics_is_valid_img(LPINFOSTR pinfo_str);

static enum EXERESULT CALLAGREEMENT _read_channel_info(ISFILE *pfile, LPPICS_CHANNEL_INFO *ppchannel, int *channel_count);

static unsigned long read_scanline(ISFILE *, unsigned char *, unsigned long, LPPICS_CHANNEL_INFO, unsigned long);
static unsigned long channel_read_mixed(ISFILE *, unsigned char *, unsigned long, unsigned long, unsigned long *, unsigned long);
static unsigned long channel_read_pure(ISFILE *, unsigned char *, unsigned long, unsigned long, unsigned long *, unsigned long);
static unsigned long channel_read_raw(ISFILE *, unsigned char *, unsigned long, unsigned long, unsigned long *, unsigned long);

									  


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&pics_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&pics_get_info_critical);
			InitializeCriticalSection(&pics_load_img_critical);
			InitializeCriticalSection(&pics_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&pics_get_info_critical);
			DeleteCriticalSection(&pics_load_img_critical);
			DeleteCriticalSection(&pics_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PICS_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pics_irwp_info;
}

#else

PICS_API LPIRWP_INFO CALLAGREEMENT pics_get_plugin_info()
{
	_init_irwp_info(&pics_irwp_info);

	return (LPIRWP_INFO)&pics_irwp_info;
}

PICS_API void CALLAGREEMENT pics_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PICS_API void CALLAGREEMENT pics_detach_plugin()
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
	/* 各位数减一，否则32位格式无法表示。此处请用PICS正确的位深填写！*/
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
				(const char *)"SoftImage，又一个新领域...");
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

	/* 别名个数（##需手动修正，如PICS格式有别名请参考BMP插件中别名的设置） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
PICS_API int CALLAGREEMENT pics_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE				*pfile = (ISFILE*)0;

	int					channel_count, i;

	PICS_HEADER			picheader;
	LPPICS_CHANNEL_INFO	pchannel = 0;

	enum EXERESULT		b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&pics_get_info_critical);

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

			if (isio_read((void*)&picheader, sizeof(PICS_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 转为X86字序 */
			picheader.fields = EXCHANGE_WORD(picheader.fields);
			picheader.height = EXCHANGE_WORD(picheader.height);
			picheader.magic  = EXCHANGE_DWORD(picheader.magic);
			picheader.ratio  = (float)EXCHANGE_DWORD(((unsigned long)picheader.ratio));
			picheader.version= (float)EXCHANGE_DWORD(((unsigned long)picheader.version));
			picheader.width  = EXCHANGE_WORD(picheader.width);
			
			/* 判断图象流是否是SoftImage PIC格式 */
			if ((picheader.magic != PICS_MAGIC) || \
				(*((unsigned long *)(&(picheader.id))) != PICS_PICT_STR) || \
				(picheader.width == 0) || \
				(picheader.height == 0))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 读取通道数据 */
			if ((b_status=_read_channel_info(pfile, &pchannel, &channel_count)) != ER_SUCCESS)
			{
				__leave;
			}

			/* 只支持3或4通道图象（当前的SoftImage PIC图象绝大部分是这两种通道格式）*/
			/* RGB三个通道合并在一起，ALPHA通道单独计算 */
			if ((channel_count != 1)&&(channel_count != 2))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_PIC;			/* 图像文件格式（后缀名） */
			
			pinfo_str->compression	= ICS_RGB;			/* 象素数据压缩方法 */

			for (i=0; i<channel_count; i++)				/* 如果有一个通道采用了RLE8，则视为RLE8压缩 */
			{
				if (pchannel[i].type != PIC_UNCOMPRESSED)
				{
					pinfo_str->compression	= ICS_RLE8; break;
				}
			}

			pinfo_str->width	= (unsigned long)picheader.width;
			pinfo_str->height	= (unsigned long)picheader.height;
			pinfo_str->order	= 0;					/* 正向图 */
			pinfo_str->bitcount	= (unsigned long)(channel_count == 1) ? 24:32;
			
			if (pinfo_str->bitcount == 24)				/* 设置掩码数据 */
			{
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0x0;
			}
			else if (pinfo_str->bitcount == 32)
			{
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0xff000000;
			}
			else
			{
				assert(0);		/* 系统异常（只能有3通道或4通道） */
			}

			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pchannel)
				free(pchannel);

			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pics_get_info_critical);
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
PICS_API int CALLAGREEMENT pics_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE				*pfile = (ISFILE*)0;

	int					i, linesize;
	int					channel_count;
	unsigned char		*pb;
	
	PICS_HEADER			picheader;
	LPPICS_CHANNEL_INFO	pchannel = 0;
	
	enum EXERESULT		b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pics_load_img_critical);

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

			if (isio_read((void*)&picheader, sizeof(PICS_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 转为X86字序 */
			picheader.fields = EXCHANGE_WORD(picheader.fields);
			picheader.height = EXCHANGE_WORD(picheader.height);
			picheader.magic  = EXCHANGE_DWORD(picheader.magic);
			picheader.ratio  = (float)EXCHANGE_DWORD(((unsigned long)picheader.ratio));
			picheader.version= (float)EXCHANGE_DWORD(((unsigned long)picheader.version));
			picheader.width  = EXCHANGE_WORD(picheader.width);

			/* 判断图象流是否是SoftImage PIC格式 */
			if ((picheader.magic != PICS_MAGIC) || \
				(*((unsigned long *)(&(picheader.id))) != PICS_PICT_STR) || \
				(picheader.width == 0) || \
				(picheader.height == 0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 读取通道数据 */
			if ((b_status=_read_channel_info(pfile, &pchannel, &channel_count)) != ER_SUCCESS)
			{
				__leave;
			}
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if ((channel_count != 1)&&(channel_count != 2))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_PIC;			/* 图像文件格式（后缀名） */
				
				pinfo_str->compression	= ICS_RGB;			/* 象素数据压缩方法 */
				
				for (i=0; i<channel_count; i++)				/* 如果有一个通道采用了RLE8，则视为RLE8压缩 */
				{
					if (pchannel[i].type != PIC_UNCOMPRESSED)
					{
						pinfo_str->compression	= ICS_RLE8; break;
					}
				}
				
				pinfo_str->width	= (unsigned long)picheader.width;
				pinfo_str->height	= (unsigned long)picheader.height;
				pinfo_str->order	= 0;					/* 正向图 */
				pinfo_str->bitcount	= (unsigned long)(channel_count == 1) ? 24:32;
				
				if (pinfo_str->bitcount == 24)				/* 设置掩码数据 */
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else if (pinfo_str->bitcount == 32)
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0xff000000;
				}
				else
				{
					assert(0);
				}
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}
				
			/* 设置调色板项的个数为0 */
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
			
			/* 初始化行首地址数组（PIC为正向图）*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}
			
			
			/* 逐行读入象素数据 */
			for (pb=(unsigned char*)pinfo_str->p_bit_data, i=0;i<(int)(pinfo_str->height);i++)
			{
				switch (read_scanline(pfile, pb, pinfo_str->width, pchannel, channel_count))
				{
				case	0:
					break;
				case	1:
				case	3:
					b_status = ER_BADIMAGE; __leave; break;
				case	2:
					b_status = ER_FILERWERR; __leave; break;
				default:
					assert(0); b_status = ER_SYSERR; __leave; break;
				}

				pb += linesize;
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

			if (pchannel)
				free(pchannel);
			
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pics_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
PICS_API int CALLAGREEMENT pics_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE				*pfile = (ISFILE*)0;

	int					i, j, linesize;
	unsigned char		*pb;
	unsigned char		buf[4];
	
	PICS_HEADER			picheader;
	PICS_CHANNEL_INFO	channel_info;
	
	enum EXERESULT		b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pics_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_pics_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 设置进度值 */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 填写流头结构 */
			strcpy((char*)picheader.comment, (const char*)"Saved by ISee PICS plug-in.");
			*((unsigned long *)&(picheader.id[0])) = PICS_PICT_STR;
			picheader.padding= 0;
			
			picheader.magic  = PICS_MAGIC;
			picheader.width  = (unsigned short)pinfo_str->width;
			picheader.height = (unsigned short)pinfo_str->height;
			picheader.fields = 0x3;
			picheader.ratio  = (float)pinfo_str->width/(float)pinfo_str->height;
			picheader.version= (float)0.0;
			
			/* 转换至MOTO字序 */
			picheader.magic  = EXCHANGE_DWORD((picheader.magic));
			picheader.width  = EXCHANGE_WORD((picheader.width));
			picheader.height = EXCHANGE_WORD((picheader.height));
			picheader.fields = EXCHANGE_WORD((picheader.fields));
			picheader.ratio  = (float)EXCHANGE_DWORD(((unsigned long)picheader.ratio));
			picheader.version= (float)EXCHANGE_DWORD(((unsigned long)picheader.version));
			
			/* 取得扫描行尺寸 */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入文件头结构 */
			if (isio_write((const void *)&picheader, sizeof(PICS_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			assert(pinfo_str->bitcount == 24);

			/* 填写通道信息（只有一个通道）*/
			channel_info.chained = 0;
			channel_info.channel = 0xe0;
			channel_info.size    = 8;
			channel_info.type    = PIC_UNCOMPRESSED;

			/* 写入通道信息 */
			if (isio_write((const void *)&channel_info, sizeof(PICS_CHANNEL_INFO), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 以不压缩的方式逐行保存象素数据 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				pb = (unsigned char *)pinfo_str->pp_line_addr[i];
				
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					/* 提取出R、G、B分量值（PIC顺序与ISee流格式相反）*/
					buf[2] = *pb++; buf[1] = *pb++; buf[0] = *pb++;

					if (isio_write((const void *)buf, 3, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
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
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pics_save_img_critical);
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
int CALLAGREEMENT _pics_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(pics_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (pics_irwp_info.irwp_save.img_num)
	{
		if (pics_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* 图像个数不正确 */
			}
		}
	}

	return 0;
}


/* 读取流中的通道信息（超过4个通道的流将被视为4通道图象）*/
static enum EXERESULT CALLAGREEMENT _read_channel_info(ISFILE *pfile, LPPICS_CHANNEL_INFO *ppchannel, int *channel_count)
{
	int					i, j;
	LPPICS_CHANNEL_INFO	p = 0;
	LPPICS_CHANNEL_INFO	ptmp;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&ppchannel&&channel_count);

	__try
	{
		*ppchannel = p;
		*channel_count = 0;

		if (isio_seek(pfile, sizeof(PICS_HEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		if ((p=ptmp=(LPPICS_CHANNEL_INFO)malloc(sizeof(PICS_CHANNEL_INFO)*3)) == 0)
		{
			b_status = ER_MEMORYERR; __leave;
		}

		j = i = 0;

		do
		{
			/* 读一个通道数据 */
			if (isio_read((void*)ptmp, sizeof(PICS_CHANNEL_INFO), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			i++; j++;

			/* 检测通道数据是否合法 */
			if ((ptmp->size != 8)||(ptmp->type > 0x2)||((ptmp->channel!=0xe0)&&(ptmp->channel!=0x10)))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			if (i > 2)			/* 只保存前4个通道的数据（RGB三个通道放在一起） */
			{
				i--; ptmp--;	/* 继续读取下一个通道的数据，但不保留 */
				if (j > 6)		/* 超过8个通道就有可能是非法的PIC图象 */
				{
					b_status = ER_BADIMAGE; __leave;
				}
			}
		} while((ptmp->chained != 0)&&(ptmp++));

		*ppchannel		= p;	/* 传回通道数据 */
		*channel_count	= i;
	}
	__finally
	{
		if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
		{
			if (p)
			{
				free(p);
			}
		}
	}

	return b_status;
}


/* 读取一个扫描行的数据（包括这个扫描行的所有通道）*/ 
static unsigned long read_scanline(ISFILE *pfile, 
							unsigned char *scan, 
							unsigned long width, 
							LPPICS_CHANNEL_INFO channel, 
							unsigned long chan_count)
{
	int				i;
	unsigned long	noCol, status = 0;
	unsigned long	off[4];
	unsigned long	bytes;

	assert(pfile&&scan&&width&&channel&&chan_count);
	assert((chan_count == 1)||(chan_count == 2));

	bytes = (chan_count == 1) ? 3:4;

	for (i=0; i<(int)chan_count; i++)
	{
		noCol = 0;

		/* 确定R、G、B各分量在ISee流中单个象素中的位置 */
		if (channel[i].channel & PIC_RED_CHANNEL)
		{
			off[noCol] = 2;
			noCol++;
		}
		if (channel[i].channel & PIC_GREEN_CHANNEL)
		{
			off[noCol] = 1;
			noCol++;
		}
		if (channel[i].channel & PIC_BLUE_CHANNEL)
		{
			off[noCol] = 0;
			noCol++;
		}
		if (channel[i].channel & PIC_ALPHA_CHANNEL)
		{
			off[noCol] = 3;
			noCol++;
		}
		
		/* 依据压缩方式调用相应的读取函数 */
		switch(channel[i].type & 0xf)
		{
			case PIC_UNCOMPRESSED:		/* 未压缩 */
				status = channel_read_raw(pfile, scan, width, noCol, off, bytes);
				break;
			case PIC_PURE_RUN_LENGTH:	/* 标准RLE8压缩 */
				status = channel_read_pure(pfile, scan, width, noCol, off, bytes);
				break;
			case PIC_MIXED_RUN_LENGTH:	/* 混合RLE8压缩法 */
				status = channel_read_mixed(pfile, scan, width, noCol, off, bytes);
				break;
			default:					/* 系统异常 */
				assert(0); return 0;
		}

		if (status != 0)				/* 如果出错则停止读取操作 */
			break;
	}

	return status;
}


/* 读一未压缩行象素数据 */
static unsigned long channel_read_raw(ISFILE *file, 
									  unsigned char *scan, 
									  unsigned long width, 
									  unsigned long noCol, 
									  unsigned long *off, 
									  unsigned long bytes)
{
	int			i, j;
	
	for (i=0; i<(int)width; i++)
	{
		if (isio_eof(file))
		{
			return 1;		/* 源数据缺损 */
		}

		for (j=0; j<(int)noCol; j++)
		{
			/* 按ISee流格式排列读入的象素分量 */
			if (isio_read((void*)&(scan[off[j]]), 1, 1, file) == 0)
			{
				return 2;	/* 流读写异常 */
			}
		}

		scan += bytes;
	}

	return 0;
}


/* 读一标准RLE压缩行象素数据 */
static unsigned long channel_read_pure(ISFILE *file, 
									   unsigned char *scan, 
									   unsigned long width, 
									   unsigned long noCol, 
									   unsigned long *off, 
									   unsigned long bytes)
{
	unsigned char	col[4];
	unsigned long	count = 0;
	int				i, j, k;
	
	for (i=(int)width; i>0; )
	{
		/* 读入计数值 */
		if (isio_read((void*)&count, 1, 1, file) == 0)
		{
			return 1;		/* 源数据缺损 */
		}
		
		/* 容错处理 */
		if (count > width)
		{
			count = width;
		}

		i -= (int)count;
		
		if (isio_eof(file))
		{
			return 1;
		}
		
		/* 读入重复数据 */
		for (j=0; j<(int)noCol; j++)
		{
			if (isio_read((void*)&(col[j]), 1, 1, file) == 0)
			{
				return 2;	/* 流读写异常 */
			}
		}
		
		/* 展开至数据包 */
		for (k=0; k<(int)count; k++, scan += bytes)
		{
			for (j=0; j<(int)noCol; j++)
			{
				scan[off[j]+k] = col[j];
			}
		}
	}

	return 0;
}


/* 读一混合RLE压缩象素数据（非理论压缩法）*/
static unsigned long channel_read_mixed(ISFILE *file, 
										unsigned char *scan, 
										unsigned long width, 
										unsigned long noCol, 
										unsigned long *off, 
										unsigned long bytes)
{
	unsigned long	count = 0;
	int				i, j, k;
	unsigned char	col[4];
	
	for (i=0; i<(int)width; i+=count)
	{
		if (isio_eof(file))
		{
			return 1;			/* 源数据缺损 */
		}
		
		/* 读入计数值 */
		if (isio_read((void*)&count, 1, 1, file) == 0)
		{
			return 2;			/* 流读写异常 */
		}
		
		if (count >= 128)		/* 重复（被压缩）数据 */
		{
			if (count == 128)
			{
				if (isio_read((void*)&count, 2, 1, file) == 0)
				{
					return 2;
				}
				count = (unsigned long)EXCHANGE_WORD(((unsigned short)count));
			}
			else
			{
				count -= 127;
			}
			
			if ((i+(int)count) > (int)width)
			{
				return 3;		/* 源数据非法 */
			}

			/* 读入压缩数据 */
			for (j=0; j<(int)noCol; j++)
			{
				if (isio_read((void*)&(col[j]), 1, 1, file) == 0)
				{
					return 2;
				}
			}
			
			for (k=0; k<(int)count; k++, scan += bytes)
			{
				for (j=0; j<(int)noCol; j++)
				{
					scan[off[j]] = col[j];
				}
			}
		}
		else
		{						/* 未能压缩的数据 */
			count++;

			if ((i+(int)count) > (int)width)
			{
				return 3;		/* 源数据非法 */
			}
			
			/* 直接复制源数据至数据包 */
			for (k=(int)count; k>0; k--, scan+=bytes)
			{
				for (j=0; j<(int)noCol; j++)
				{
					if (isio_read((void*)&(scan[off[j]]), 1, 1, file) == 0)
					{
						return 2;
					}
				}
			}
		}
	}

	return 0;
}


