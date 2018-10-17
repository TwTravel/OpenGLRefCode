/********************************************************************

	jpeg.c

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
	本文件用途：	ISee图像浏览器—JPEG图像读写模块实现文件
	
					读取功能：8位灰度图像、24位真彩图像
					保存功能：8位灰度图像、24位真彩图像
							  （可设定品质系数）
	  
	本文件编写人：	orbit		Inte2000##263.net
					YZ			yzfree##sina.com
		
	本文件版本：	10715
	最后修改于：	2001-7-15
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-7		发布新版代码
		2000-12		改进了进度控制代码
		2000-9		第一个测试版发布


	引用代码声明：
	---------------------------------------------------------------
	本插件使用了IJG的jpeglib6b函数库，感谢他们所做的工作！

	"this software is based in part on the work of the Independent 
	JPEG Group"
  
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
#include <setjmp.h>
#include <assert.h>

#include "jpeglib.h"
#include "jpeg.h"


IRWP_INFO			jpeg_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	jpeg_get_info_critical;	/* jpeg_get_image_info函数的关键段 */
CRITICAL_SECTION	jpeg_load_img_critical;	/* jpeg_load_image函数的关键段 */
CRITICAL_SECTION	jpeg_save_img_critical;	/* jpeg_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
METHODDEF(void)  CALLAGREEMENT	my_error_exit (j_common_ptr cinfo);
enum IMGCOMPRESS CALLAGREEMENT 	compress_type(J_COLOR_SPACE jcs);
void			 CALLAGREEMENT	_init_irwp_info(LPIRWP_INFO lpirwp_info);
int				 CALLAGREEMENT _jpeg_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&jpeg_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&jpeg_get_info_critical);
			InitializeCriticalSection(&jpeg_load_img_critical);
			InitializeCriticalSection(&jpeg_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&jpeg_get_info_critical);
			DeleteCriticalSection(&jpeg_load_img_critical);
			DeleteCriticalSection(&jpeg_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

JPEG_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&jpeg_irwp_info;
}

#else

JPEG_API LPIRWP_INFO CALLAGREEMENT jpeg_get_plugin_info()
{
	_init_irwp_info(&jpeg_irwp_info);

	return (LPIRWP_INFO)&jpeg_irwp_info;
}

JPEG_API void CALLAGREEMENT jpeg_init_plugin()
{
	/* 初始化多线程同步对象 */
}

JPEG_API void CALLAGREEMENT jpeg_detach_plugin()
{
	/* 销毁多线程同步对象 */
}

#endif /* WIN32 */


/* 初始化插件信息结构 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info)
{
	int		i, j;

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
	/* ## 2001-8-9  YZ 修改：各位数减一，否则32位格式无法表示。*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(8-1)) | (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;			/* 1－只能保存一副图像 */
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 1;
	lpirwp_info->irwp_save.para[0].count = 15;	/* 共设15组品质系数（均匀分布于1－100）*/
	strcpy((char*)lpirwp_info->irwp_save.para[0].desc, (const char *)"品质系数");
	j = lpirwp_info->irwp_save.para[0].count;
	/* ################################################################# */
	/* ## 2002-6-29  yygw 修改：原保存参数计算不正确。                   */
	for (i=0;i<j;i++)
	{
		// lpirwp_info->irwp_save.para[0].value[i] = i*(100/j)+1;
        lpirwp_info->irwp_save.para[0].value[i] = i*99/(j-1)+1;
		sprintf((char*)lpirwp_info->irwp_save.para[0].value_desc[i], (const char *)"%d%%", lpirwp_info->irwp_save.para[0].value[i]);
	}
	// i--;
	// lpirwp_info->irwp_save.para[0].value[i] = 100;/* 最高品质 */
	// sprintf((char*)lpirwp_info->irwp_save.para[0].value_desc[i], (const char *)"%d", lpirwp_info->irwp_save.para[0].value[i]);
	/* ################################################################# */


	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 2;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
		(const char *)"orbit");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
		(const char *)"Inte2000##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
		(const char *)"死不了人的！");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"娃哈哈呀～娃哈哈！咦呀咦呀呦！");
	/* ---------------------------------[2] － 第三组 -------------- */
	/* 后续开发者信息可加在此处。*/
	/* ------------------------------------------------------------- */


	/* 插件描述信息（扩展名信息）*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* 别名个数（##需手动修正） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 1;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
				(const char *)"JPEG");

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
JPEG_API int CALLAGREEMENT jpeg_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned short	fmark;

	struct jpeg_decompress_struct	cinfo;
	struct my_error_mgr				jerr;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&jpeg_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 读取文件标志 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&fmark, sizeof(unsigned short), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断是否是有效的文件 */
			if (fmark != JPG_SOI_MARK)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			cinfo.err = jpeg_std_error(&jerr.pub);
			jerr.pub.error_exit = my_error_exit;
	
			/* 设置错误处理点 */
			if (setjmp(jerr.setjmp_buffer)) 
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* 创建解码对象 */
			jpeg_create_decompress(&cinfo);
			/* 指定源数据流 */
			jpeg_stdio_src(&cinfo, (FILE*)pfile);
			/* 读入图像信息 */
			jpeg_read_header(&cinfo, 1);
			/* 销毁解码对象 */			
			jpeg_destroy_decompress(&cinfo);
			
			
			/* 数据有效性检验 */
			if ((cinfo.image_width == 0)||(cinfo.image_height == 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_JPEG;			/* 图像文件格式（后缀名） */
			pinfo_str->compression	= compress_type(cinfo.jpeg_color_space);
														/* 图像的压缩方式 */
			if (pinfo_str->compression == ICS_UNKONW)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			/* 填写图像信息 */
			pinfo_str->width	= (unsigned long)cinfo.image_width;
			pinfo_str->height	= (unsigned long)cinfo.image_height;
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= (cinfo.num_components == 3) ? 24UL:8UL;
			
			if (pinfo_str->bitcount == 24)
			{
				if (pinfo_str->compression == ICS_JPEGYCbCr)
				{
					/* 注：此类图像RGB顺序已在jmorecfg.h文件中修改，与BMP相同 */
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0;
				}
				else
				{
					/* 注：JPEG的RGB排列顺序与BMP图像相反 */
					pinfo_str->b_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff;
					pinfo_str->a_mask = 0;
				}
			}
			else
			{
				pinfo_str->b_mask = pinfo_str->g_mask = pinfo_str->r_mask = pinfo_str->a_mask = 0;
			}
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&jpeg_get_info_critical);
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
JPEG_API int CALLAGREEMENT jpeg_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned short	fmark;

	struct jpeg_decompress_struct	cinfo;
	struct my_error_mgr				jerr;
	unsigned char	*pdata;
	JSAMPARRAY		buffer;	
	int				row_stride, i, linesize;		
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&jpeg_load_img_critical);

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

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 判断是否已获取过图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if (isio_read((void*)&fmark, sizeof(unsigned short), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 判断是否是有效的文件 */
				if (fmark != JPG_SOI_MARK)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				if (isio_seek(pfile, 0, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			cinfo.err = jpeg_std_error(&jerr.pub);
			jerr.pub.error_exit = my_error_exit;

			/* 设置错误返回地址 */
			if (setjmp(jerr.setjmp_buffer)) 
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* 创建解码对象 */
			jpeg_create_decompress(&cinfo);
			/* 指定源数据流 */
			jpeg_stdio_src(&cinfo, (FILE*)pfile);
			/* 读入图像信息 */
			jpeg_read_header(&cinfo, 1);

			/* 如果没有提取过图像信息，则提取之 */
			if (pinfo_str->data_state == 0)
			{
				/* 数据有效性检验 */
				if ((cinfo.image_width == 0)||(cinfo.image_height == 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_JPEG;			/* 图像文件格式（后缀名） */
				pinfo_str->compression	= compress_type(cinfo.jpeg_color_space);
				/* 图像的压缩方式 */
				if (pinfo_str->compression == ICS_UNKONW)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				/* 填写图像信息 */
				pinfo_str->width	= (unsigned long)cinfo.image_width;
				pinfo_str->height	= (unsigned long)cinfo.image_height;
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= (cinfo.num_components == 3) ? 24UL:8UL;
				
				if (pinfo_str->bitcount == 24)
				{
					if (pinfo_str->compression == ICS_JPEGYCbCr)
					{
						/* 注：此类图像RGB顺序已在jmorecfg.h文件中修改，与BMP相同 */
						pinfo_str->r_mask = 0xff0000;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->b_mask = 0xff;
						pinfo_str->a_mask = 0;
					}
					else
					{
						pinfo_str->b_mask = 0xff0000;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->r_mask = 0xff;
						pinfo_str->a_mask = 0;
					}
				}
				else
				{
					pinfo_str->b_mask = pinfo_str->g_mask = pinfo_str->r_mask = pinfo_str->a_mask = 0;
				}
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}
			
			/* 设置调色板数据 */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

				for (i=0;i<256;i++)
					pinfo_str->palette[i] = (unsigned long)((i<<16)|(i<<8)|i);
			}
			else
				pinfo_str->pal_count = 0;
			
			/* 计算扫描行尺寸(四字节对齐) */
			linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);

			/* 设置图像个数 */
			pinfo_str->imgnumbers = 1;
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;
			
			assert(pinfo_str->p_bit_data == (unsigned char *)0);
			/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
			pinfo_str->p_bit_data = (unsigned char *)malloc(linesize * pinfo_str->height + 4);
			if (!pinfo_str->p_bit_data)
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_MEMORYERR; __leave;
			}
			
			assert(pinfo_str->pp_line_addr == (void**)0);
			/* 分配行首地址数组 */
			pinfo_str->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo_str->height);
			if (!pinfo_str->pp_line_addr)
			{
				jpeg_destroy_decompress(&cinfo);
				b_status = ER_MEMORYERR; __leave;
			}

			/* 初始化行首地址数组（正向）*/
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			

			/* 开始解码 */
			jpeg_start_decompress(&cinfo);
			
			/* 计算扫描行缓冲区长度 */
			row_stride = cinfo.output_width * cinfo.output_components;
			
			/* 申请行指针缓冲区 */
			buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

			/* 进度变量初始化 */
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			
			/* 读啊。。。。:) */
			while (cinfo.output_scanline < cinfo.output_height) 
			{
				jpeg_read_scanlines(&cinfo, buffer, 1);
				pdata = (unsigned char *)(buffer[0]);

				memmove((void*)pinfo_str->pp_line_addr[cinfo.output_scanline-1], (const void*)pdata, cinfo.output_width*((cinfo.out_color_components == 3) ? 3:1));

				pinfo_str->process_current = cinfo.output_scanline;

				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					jpeg_destroy_decompress(&cinfo);
					b_status = ER_USERBREAK; __leave;
				}
			}
			
			/* 完成解码 */
			jpeg_finish_decompress(&cinfo);
			/* 销毁解码对象 */			
			jpeg_destroy_decompress(&cinfo);
			
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

			LeaveCriticalSection(&jpeg_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 （只能保存8、24位图像）*/
JPEG_API int CALLAGREEMENT jpeg_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr			jerr;
	unsigned char				*lp;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&jpeg_save_img_critical);
	
			/* 判断传入的图像是否被支持 */
			if (_jpeg_is_valid_img(pinfo_str, lpsave))
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 设置进度值 */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			

			/* 设置错误处理历程 */
			cinfo.err = jpeg_std_error(&jerr.pub);
			jerr.pub.error_exit = my_error_exit;
			if (setjmp(jerr.setjmp_buffer)) 
			{
				jpeg_destroy_compress(&cinfo);
				b_status = ER_FILERWERR; __leave;
			}

			/* 设置压缩对象及参数 */
			jpeg_create_compress(&cinfo);
			jpeg_stdio_dest(&cinfo, (FILE*)pfile);

			cinfo.image_width  = pinfo_str->width;
			cinfo.image_height = pinfo_str->height;

			/* 设置象素格式信息 */
			if (pinfo_str->bitcount == 24)
			{
				cinfo.input_components = 3;		
				cinfo.in_color_space = JCS_RGB; 	
			}
			else if (pinfo_str->bitcount == 8)
			{
				cinfo.input_components = 1;		
				cinfo.in_color_space = JCS_GRAYSCALE; 	
			}
			else
				assert(0);

			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, lpsave->para_value[0], 1);		/* 设置品质系数 */
			jpeg_start_compress(&cinfo, 1);
			
			/* 写入目标流 */
			while (cinfo.next_scanline < cinfo.image_height) 
			{
				lp = (unsigned char *)pinfo_str->pp_line_addr[cinfo.next_scanline];
				jpeg_write_scanlines(&cinfo, &lp, 1);

				pinfo_str->process_current = cinfo.next_scanline;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					jpeg_destroy_compress(&cinfo);
					b_status = ER_USERBREAK; __leave;
				}
			}				

			/* 完成压缩 */
			jpeg_finish_compress(&cinfo);
			jpeg_destroy_compress(&cinfo);
			
			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&jpeg_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 错误处理历程 */
METHODDEF(void) CALLAGREEMENT my_error_exit (j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	
	char buffer[JMSG_LENGTH_MAX];
	
	(*cinfo->err->format_message) (cinfo, buffer);
	
	longjmp(myerr->setjmp_buffer, 1);
}


/* 解析图像色彩系统 */
enum IMGCOMPRESS CALLAGREEMENT compress_type(J_COLOR_SPACE jcs)
{
	enum IMGCOMPRESS rtn = ICS_UNKONW;

	switch(jcs)
	{
	case JCS_UNKNOWN:
		rtn = ICS_UNKONW;
		break;
	case JCS_GRAYSCALE:	
		rtn = ICS_JPEGGRAYSCALE;
		break;
	case JCS_RGB:
		rtn = ICS_JPEGRGB;
		break;		
	case JCS_YCbCr:
		rtn = ICS_JPEGYCbCr;
		break;		
	case JCS_CMYK:
		rtn = ICS_JPEGCMYK;
		break;		
	case JCS_YCCK:
		rtn = ICS_JPEGYCCK;
		break;
	default:
		rtn = ICS_UNKONW;
		break;
	}
	return rtn;
}


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _jpeg_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	int i;

	/* ################################################################# */
	/* ## 2001-8-9  YZ 修改：位数减一，否则32位格式无法表示。*/
	if (!(jpeg_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */
	
	assert(pinfo_str->imgnumbers);
	
	if (jpeg_irwp_info.irwp_save.img_num != (int)pinfo_str->imgnumbers)
		return -2;			/* 图像个数不正确 */

	for (i=0;i<jpeg_irwp_info.irwp_save.para[0].count;i++)
		if (jpeg_irwp_info.irwp_save.para[0].value[i] == lpsave->para_value[0])
			break;

	return (i==jpeg_irwp_info.irwp_save.para[0].count) ? -3:0;		/* -3:无效的品质系数 */
}

