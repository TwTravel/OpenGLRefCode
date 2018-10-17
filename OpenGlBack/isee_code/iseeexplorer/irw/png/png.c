/********************************************************************

	png.c

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
	本文件用途：	ISee图像浏览器—PNG图像读写模块实现文件

					读取功能：1、2、4、8位灰度图，8位带alpha通道的
								灰度图，24位RGB图、24位带alpha通道
								的RGB图，1、2、4、8位调色板图。
								注：16位灰度图及48位RGB图将在本模块
									内部被转换成8位及24位图。
									8位带alpha通道的灰度图将被转换为
									32位RGBA图。
									以上转换主要是为了使输出数据符
									合ISee位流格式。
					保存功能：1、2、4、8位调色板图，24、32位RGB图。
								注：32位图携带8位alpha通道。
									不能保存灰度图。
	  
	本文件编写人：	Aurora		aurorazzf##sina.com
					YZ			yzfree##sina.com （改版工作）
		
	本文件版本：	10809
	最后修改于：	2001-8-9
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-8		发布新版代码
		2001-5		重新定义模块的接口及输出功能
		2001-3		修正了一些BUG
		2000-7		修正了一些BUG
		2000-6		第一个测试版发布


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

#include "png.h"
#include "libpng\lpng106\png.h"


IRWP_INFO			png_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	png_get_info_critical;	/* png_get_image_info函数的关键段 */
CRITICAL_SECTION	png_load_img_critical;	/* png_load_image函数的关键段 */
CRITICAL_SECTION	png_save_img_critical;	/* png_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _png_is_valid_img(LPINFOSTR pinfo_str);

void CALLAGREEMENT png_error_handler(png_structp, png_const_charp);
void CALLAGREEMENT png_isio_read(png_structp, png_bytep, png_size_t);
void CALLAGREEMENT png_isio_write(png_structp, png_bytep, png_size_t);
void CALLAGREEMENT png_isio_flush(png_structp);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&png_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&png_get_info_critical);
			InitializeCriticalSection(&png_load_img_critical);
			InitializeCriticalSection(&png_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&png_get_info_critical);
			DeleteCriticalSection(&png_load_img_critical);
			DeleteCriticalSection(&png_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PNG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&png_irwp_info;
}

#else

PNG_API LPIRWP_INFO CALLAGREEMENT png_get_plugin_info()
{
	_init_irwp_info(&png_irwp_info);

	return (LPIRWP_INFO)&png_irwp_info;
}

PNG_API void CALLAGREEMENT png_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PNG_API void CALLAGREEMENT png_detach_plugin()
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
	lpirwp_info->irwp_save.bitcount = \
		1UL|(1UL<<(2-1))|(1UL<<(4-1))|(1UL<<(8-1))|(1UL<<(24-1))|(1UL<<(32-1));
	lpirwp_info->irwp_save.img_num = 1;	/* 1－只能保存一副图像 */
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 2;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"Aurora");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"aurorazzf##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"第一版");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
		(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
		(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
		(const char *)"现在是第二版了:)");
	/* ---------------------------------[2] － 第三组 -------------- */
	/* 后续开发者信息可加在此处。
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_message), 
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
PNG_API int CALLAGREEMENT png_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	png_structp		p_png = 0;
	png_infop		p_png_info = 0;
	PNGINFOHEADER	png_info_header;
	int				channels;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	
	
	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&png_get_info_critical);
			
			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 分配PNG结构，并初始化 */
			if ((p_png=png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)pfile, png_error_handler, 0)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 建立PNG信息头结构 */
			if ((p_png_info=png_create_info_struct(p_png)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			if (setjmp(png_jmpbuf(p_png)))
			{
				/* PNG库出错后将返回到这里 */
				b_status = ER_FILERWERR; __leave;
			}

			/* 设置自定义读函数 */
			png_set_read_fn(p_png, (PVOID)pfile, png_isio_read);
			
			/* 读文件信息头（直到IDAT）*/
			png_read_info(p_png, p_png_info);
			
			/* 获取图象信息 */
			png_get_IHDR(p_png, p_png_info, &png_info_header.Width, 
				&png_info_header.Height, &png_info_header.bit_depth, 
				&png_info_header.color_type, &png_info_header.interlace_type,
				&png_info_header.compression_type, &png_info_header.filter_type);
			

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_PNG;			/* 图像文件格式（后缀名） */

			if (png_info_header.color_type == PNG_COLOR_TYPE_PALETTE)
			{
				pinfo_str->compression = ICS_PNG_PALETTE; channels = 1;
			}
			else if (png_info_header.color_type & PNG_COLOR_MASK_COLOR)
			{
				pinfo_str->compression = ICS_PNG_RGB; channels = 3;
			}
			else
			{
				pinfo_str->compression = ICS_PNG_GRAYSCALE; channels = 1;
			}

			/* 如果图像带有alpha通道，则将灰度图全部转换为24位RGB图像 */
			if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
			{
				channels = 4;
				if (pinfo_str->compression == ICS_PNG_GRAYSCALE)
				{
					png_info_header.bit_depth = 8;
					pinfo_str->compression = ICS_PNG_RGB;
				}
			}

			/* 当PNG图像象素的采样点位深大于8位时，插件将其削减为8位
				（因ISee的插件输出格式规定每象素最大为32位位深）。
			*/
			if (png_info_header.bit_depth > 8)
				png_info_header.bit_depth = 8;

			/* 这个位深值可能会与实际位深不同（因上面的转换）*/
			png_info_header.bit_depth *= channels;


			pinfo_str->width	= (unsigned long)png_info_header.Width;
			pinfo_str->height	= (unsigned long)png_info_header.Height;
			pinfo_str->order	= 0;		/* PNG全部为自顶向下的扫描行顺序 */
			pinfo_str->bitcount	= png_info_header.bit_depth;
			
			switch (pinfo_str->compression)
			{
			case	ICS_PNG_PALETTE:
			case	ICS_PNG_GRAYSCALE:		/* 灰度图将被作为索引图对待 */
				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				break;
			case	ICS_PNG_RGB:			/* RGB图将只有24位一种（因48位图像已被削减为24位）*/
				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				break;
			default:
				assert(0); 
				break;
			}

			/* 计算alpha掩码数据 */
			if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
				pinfo_str->a_mask = ((1UL << (png_info_header.bit_depth / channels)) - 1)<<(32 - (png_info_header.bit_depth / channels));
			else
				pinfo_str->a_mask = 0x0;


			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if(p_png)
				png_destroy_read_struct(&p_png, &p_png_info, (png_infopp)0);
			if (pfile)
				isio_close(pfile);
		
			LeaveCriticalSection(&png_get_info_critical);
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
PNG_API int CALLAGREEMENT png_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	png_structp		p_png = 0;
	png_infop		p_png_info = 0;
	PNGINFOHEADER	png_info_header;
	int				channels, i, linesize, intent, pass, row;
	double			screen_gamma, image_gamma;
	char			*gamma_str;

	unsigned char	buf, tmp, *pbit;
	unsigned short	us;
	int				bit, mask;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	
	
	__try
	{
		__try
		{
			EnterCriticalSection(&png_load_img_critical);
			
			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 分配PNG结构，并初始化 */
			if ((p_png=png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp)pfile, png_error_handler, 0)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 建立PNG信息头结构 */
			if ((p_png_info=png_create_info_struct(p_png)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			if (setjmp(png_jmpbuf(p_png)))
			{
				/* PNG库出错后将返回到这里 */
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 设置自定义读函数 */
			png_set_read_fn(p_png, (PVOID)pfile, png_isio_read);
			
			/* 读文件信息头（直到IDAT）*/
			png_read_info(p_png, p_png_info);
			
			/* 获取图象信息 */
			png_get_IHDR(p_png, p_png_info, &png_info_header.Width, 
				&png_info_header.Height, &png_info_header.bit_depth, 
				&png_info_header.color_type, &png_info_header.interlace_type,
				&png_info_header.compression_type, &png_info_header.filter_type);
			

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_PNG;			/* 图像文件格式（后缀名） */

				if (png_info_header.color_type == PNG_COLOR_TYPE_PALETTE)
				{
					pinfo_str->compression = ICS_PNG_PALETTE; channels = 1;
				}
				else if (png_info_header.color_type & PNG_COLOR_MASK_COLOR)
				{
					pinfo_str->compression = ICS_PNG_RGB; channels = 3;
				}
				else
				{
					pinfo_str->compression = ICS_PNG_GRAYSCALE; channels = 1;
				}

				/* 如果图像带有alpha通道，则将灰度图全部转换为24位RGB图像 */
				if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
				{
					channels = 4;
					if (pinfo_str->compression == ICS_PNG_GRAYSCALE)
					{
						png_info_header.bit_depth = 8;
						pinfo_str->compression = ICS_PNG_RGB;
					}
				}
				
				/* 当PNG图像象素的采样点位深大于8位时，插件将其削减为8位
					（因ISee的插件输出格式规定每象素最大为32位位深）。
				*/
				if (png_info_header.bit_depth > 8)
					png_info_header.bit_depth = 8;

				/* 这个位深值可能会与实际位深不同（因上面的转换）*/
				png_info_header.bit_depth *= channels;


				pinfo_str->width	= (unsigned long)png_info_header.Width;
				pinfo_str->height	= (unsigned long)png_info_header.Height;
				pinfo_str->order	= 0;		/* PNG全部为自顶向下的扫描行顺序 */
				pinfo_str->bitcount	= png_info_header.bit_depth;
				
				switch (pinfo_str->compression)
				{
				case	ICS_PNG_PALETTE:
				case	ICS_PNG_GRAYSCALE:		/* 灰度图将被作为索引图对待 */
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					break;
				case	ICS_PNG_RGB:			/* RGB图将只有24位一种（因48位图像已被削减为24位）*/
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					break;
				default:
					assert(0); 
					break;
				}

				/* 计算alpha掩码数据 */
				if (png_info_header.color_type & PNG_COLOR_MASK_ALPHA)
					pinfo_str->a_mask = ((1UL << (png_info_header.bit_depth / channels)) - 1)<<(32 - (png_info_header.bit_depth / channels));
				else
					pinfo_str->a_mask = 0x0;
			}


			/* 初始化调色板数据 */
			if (pinfo_str->compression == ICS_PNG_PALETTE)
			{
				pinfo_str->pal_count = p_png_info->num_palette;

				if (pinfo_str->pal_count > 256)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* 复制调色板数据 */
				for (i=0;i<(int)pinfo_str->pal_count;i++)
				{
					((LPPALITEM)(&pinfo_str->palette[i]))->blue  = p_png_info->palette[i].blue;
					((LPPALITEM)(&pinfo_str->palette[i]))->green = p_png_info->palette[i].green;
					((LPPALITEM)(&pinfo_str->palette[i]))->red   = p_png_info->palette[i].red;
					((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
				}
			}
			else if (pinfo_str->compression == ICS_PNG_GRAYSCALE)
			{
				assert(pinfo_str->bitcount <= 8);

				pinfo_str->pal_count = 1 << pinfo_str->bitcount;
				
				/* 模拟调色板 */
				switch (pinfo_str->bitcount)
				{
				case	1:
					pinfo_str->palette[0] = 0x0;
					pinfo_str->palette[1] = 0xffffff;
					break;
				case	2:
					pinfo_str->palette[0] = 0x0;
					pinfo_str->palette[1] = 0x3f3f3f;
					pinfo_str->palette[2] = 0x7f7f7f;
					pinfo_str->palette[3] = 0xffffff;
					break;
				case	4:
					pinfo_str->palette[0] = 0x0;
					pinfo_str->palette[1] = 0x1f1f1f;
					pinfo_str->palette[2] = 0x2f2f2f;
					pinfo_str->palette[3] = 0x3f3f3f;
					pinfo_str->palette[4] = 0x4f4f4f;
					pinfo_str->palette[5] = 0x5f5f5f;
					pinfo_str->palette[6] = 0x6f6f6f;
					pinfo_str->palette[7] = 0x7f7f7f;
					pinfo_str->palette[8] = 0x8f8f8f;
					pinfo_str->palette[9] = 0x9f9f9f;
					pinfo_str->palette[10] = 0xafafaf;
					pinfo_str->palette[11] = 0xbfbfbf;
					pinfo_str->palette[12] = 0xcfcfcf;
					pinfo_str->palette[13] = 0xdfdfdf;
					pinfo_str->palette[14] = 0xefefef;
					pinfo_str->palette[15] = 0xffffff;
					break;
				case	8:
					for (i=0;i<(int)pinfo_str->pal_count;i++)
						pinfo_str->palette[i] = ((unsigned long)i<<16)|(i<<8)|i;
					break;
				default:
					assert(0);
					break;
				}
			}
			else
				pinfo_str->pal_count = 0;


			/* 取得扫描行尺寸 */
			linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);
			
			/* 设置图像个数 */
			pinfo_str->imgnumbers = 1;		/* 暂设为静态图像 */
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

			/* 初始化行首地址数据 */
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			
			/* 将tRNS通道转换为alpha通道 */
			if (png_get_valid(p_png, p_png_info,PNG_INFO_tRNS)) 
				png_set_tRNS_to_alpha(p_png);
			
			/* 将带alpha通道的灰度图转换为RGBA图 */
			if (p_png_info->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(p_png);

			/* 将图象数据由RGB转换为BGR（ISee位流格式） */
			png_set_bgr(p_png);
			
			/* 通知PNGLIB将16位图形削减为8位 */
			png_set_strip_16(p_png);

			

			/* 调整屏幕显示的GAMMA值
			*  screen gamma由CRT_gamma和环境矫正gamma组成 
			*/

			/* 暂时不实现用户指定屏幕gamma */
			if ((gamma_str = getenv("SCREEN_GAMMA")) != 0)
				screen_gamma = atof(gamma_str);
			else
				screen_gamma = 2.2;

			/* 让LIBPNG处理gamma矫正 
			*  此处应支持用户输入图象gamma值 
			*/
			if (png_get_sRGB(p_png, p_png_info, &intent))
				png_set_gamma(p_png, screen_gamma, 0.45455);/* 对图象gamma的猜测 */
			else
			{
				if (png_get_gAMA(p_png, p_png_info, &image_gamma))
					png_set_gamma(p_png, screen_gamma, image_gamma);
				else
					png_set_gamma(p_png, screen_gamma, 0.45455);
			}

			/* 打开INTERLACE处理器(使用png_read_row读图象时必须) */
			png_info_header.number_passes = png_set_interlace_handling(p_png);

			/* 调用上述变换函数后必须，更新信息结构 */
			png_read_update_info(p_png, p_png_info);
			
			/* 初始化读取进度标志 */
			pinfo_str->process_total   = png_info_header.number_passes*pinfo_str->height;
			pinfo_str->process_current = 0;

			/* 准备读取图像数据 */
			png_start_read_image(p_png);



			/* 逐行读取图像数据 */
			for(pass = 0; pass < png_info_header.number_passes; pass++)
			{
				/* 每次读取一行图象 */
				for (row = 0; row < (int)pinfo_str->height; row++)
				{
					/* 读啊 .....  :) */
					png_read_rows(p_png, (unsigned char **)&pinfo_str->pp_line_addr[row], 0, 1);

					/* 如果是1位位图，则颠倒字节内象素排列 */
					if (pinfo_str->bitcount == 1)
					{
						pbit = (unsigned char *) pinfo_str->pp_line_addr[row];

						for (i=0;i<linesize;i++)
						{
							buf = *(unsigned char *)(pbit+i);
							us  = ((unsigned short)buf)<<8;
							tmp = 0;
							for (bit=1, mask=7;bit<16;bit+=2, mask--)
								tmp |= (us>>bit)&(1<<mask);
							
							*(unsigned char *)(pbit+i) = tmp;
						}
					}

					/* 步进 */
					pinfo_str->process_current++;

					/* 支持用户中断 */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
			}
			

			/* 读取图象附加信息，包括zTxt、sTxt、tIME通道的信息
			*  对这些信息暂不处理，以后可以加到pinfo_str中传递给客户模块
			*/
			png_read_end(p_png, p_png_info);
			

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
			
			if(p_png)
				png_destroy_read_struct(&p_png, &p_png_info, (png_infopp)0);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&png_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}



/* 保存图像 */
PNG_API int CALLAGREEMENT png_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	png_structp		p_png = 0;
	png_infop		p_png_info = 0;
	png_colorp		p_palette = 0;
	unsigned int	color_type, bit_depth, i;
	unsigned long	number_passes, pass, row, height;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&png_save_img_critical);
			
			/* 判断是否是受支持的图像数据 */
			if (_png_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}
			
			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			

			/* 分配PNG结构，并初始化 */
			if ((p_png=png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)pfile, png_error_handler, 0)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 建立PNG信息头结构 */
			if ((p_png_info=png_create_info_struct(p_png)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 分配调色板类图像所需的内存块 */
			if ((p_palette=(png_colorp)malloc(sizeof(png_color)*256)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 设置自定义写函数 */
			png_set_write_fn(p_png, (PVOID)pfile, png_isio_write, (png_flush_ptr)png_isio_flush);
			
			if (setjmp(png_jmpbuf(p_png)))
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 确定PNG色彩模式 */
			if (pinfo_str->bitcount <= 8)		/* 调色板图。（不能保存灰度图）*/
			{
				assert(pinfo_str->pal_count > 0);

				color_type = PNG_COLOR_TYPE_PALETTE;
				bit_depth  = pinfo_str->bitcount;

				for (i=0;i<pinfo_str->pal_count;i++)
				{
					p_palette[i].red   = (png_byte)((pinfo_str->palette[i] & 0xff0000)>>16);
					p_palette[i].green = (png_byte)((pinfo_str->palette[i] & 0xff00)>>8);
					p_palette[i].blue  = (png_byte)(pinfo_str->palette[i] & 0xff);
				}
			}
			else								/* RGB图。（允许有alpha通道）*/
			{
				assert(pinfo_str->pal_count == 0);

				bit_depth  = 8;

				if (pinfo_str->a_mask)
					color_type = PNG_COLOR_TYPE_RGB_ALPHA;
				else
					color_type = PNG_COLOR_TYPE_RGB;
			}
				
			/* 设置PNG文件信息头 */
			png_set_IHDR(p_png, p_png_info, 
				pinfo_str->width, pinfo_str->height, 
				bit_depth, 
				color_type,
				PNG_INTERLACE_ADAM7, 
				PNG_COMPRESSION_TYPE_DEFAULT, 
				PNG_FILTER_TYPE_DEFAULT);
			
			/* 设置调色板数据 */
			if (color_type == PNG_COLOR_TYPE_PALETTE)
				png_set_PLTE(p_png, p_png_info, p_palette, pinfo_str->pal_count);
			else
				png_set_PLTE(p_png, p_png_info, (png_colorp)0, 0);


			/* 将信息写入信息头 */
			png_write_info(p_png, p_png_info);

			/* 这里用交错格式（interlaced）存储 */
			number_passes = png_set_interlace_handling(p_png);

			height = pinfo_str->height;

			/* 初始化读取进度标志 */
			pinfo_str->process_total   = number_passes*height;
			pinfo_str->process_current = 0;
			
			for (pass=0;pass<number_passes;pass++)
			{ 
				/* 一次只写入一行 */
				for (row=0;row<height;row++)
				{ 
					/* 写呀.......:) */
					png_write_rows(p_png, &((unsigned char *)(pinfo_str->pp_line_addr[row])), 1);

					/* 步进 */
					pinfo_str->process_current++;
					
					/* 支持用户中断 */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				} 
			} 
			
			/* 这里可以写入tEXt, zTXt, and tIME.......... */
			
			/* 写入IEND通道，必须 */
			png_write_end(p_png, p_png_info);

			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (p_palette)
				free(p_palette);

			if(p_png)
			{
				if (p_png_info)
					png_destroy_write_struct(&p_png, &p_png_info);
				else
					png_destroy_write_struct(&p_png, (png_infopp)0);
			}
			
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&png_save_img_critical);
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


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _png_is_valid_img(LPINFOSTR pinfo_str)
{
	if (!(png_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	
	assert(pinfo_str->imgnumbers);
	
	if (png_irwp_info.irwp_save.img_num)
		if (png_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */
			
	return 0;
}



/* PNG库自定义错误处理函数 */
void CALLAGREEMENT png_error_handler(png_structp png_ptr, png_const_charp message)
{
#ifdef PNG_SETJMP_SUPPORTED

#	ifdef USE_FAR_KEYWORD
	{
		jmp_buf jmpbuf;
		png_memcpy(jmpbuf,png_ptr->jmpbuf,sizeof(jmp_buf));
		longjmp(jmpbuf, 1);
	}
#	else
	longjmp(png_ptr->jmpbuf, 1);
#	endif

#else
	if (png_ptr == 0)
		PNG_ABORT();
#endif
}


/* PNG库自定义读函数
 * 参数：
 *		data — 返回数据缓冲区
 *		length — 读数据长度
 */
void CALLAGREEMENT png_isio_read(png_structp png_ptr, png_bytep data, png_size_t length)
{
   ISFILE	*file;

   assert(png_ptr);
   assert(png_ptr->io_ptr);

   file = (ISFILE*)CVT_PTR(png_ptr->io_ptr);

   if (isio_read((void*)data, length, 1, file) == 0)
   {
	   png_error(png_ptr, "reading png image error");
   }
}


/* PNG库自定义写函数
 * 参数：
 *		data — 数据缓冲区
 *		length — 数据长度
 */
void CALLAGREEMENT png_isio_write(png_structp png_ptr, png_bytep data, png_size_t length)
{
	ISFILE	*file;
	
	assert(png_ptr);
	assert(png_ptr->io_ptr);
	
	file = (ISFILE*)CVT_PTR(png_ptr->io_ptr);
	
	if (isio_write((void*)data, length, 1, file) == 0)
	{
		png_error(png_ptr, "write png image error");
	}
}


/* PNG库自定义FLUSH函数 */
void CALLAGREEMENT png_isio_flush(png_structp png_ptr)
{
	ISFILE	*file;
	
	assert(png_ptr);
	assert(png_ptr->io_ptr);
	
	file = (ISFILE*)CVT_PTR(png_ptr->io_ptr);
	
	if (isio_flush(file)) 
	{
		png_error(png_ptr, "fulsh file error");
	}
}

