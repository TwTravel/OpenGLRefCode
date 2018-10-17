/********************************************************************

	bmp.c

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
	本文件用途：	ISee图像浏览器—BMP图像读写模块实现文件

					读取功能：Windows 风格 －1～32位位图、RLE位图
							  OS/2 风格 － 1、4、8、24位位图
					保存功能：Windows 风格 －1、4、8、16(555)、24、
								32(888)位位图

	本文件编写人：	YZ			yzfree##sina.com

	本文件版本：	10709
	最后修改于：	2001-7-9

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2001-7		修正RLE解码中的一处错误。
					1、4位图像输出符合ISee位流格式
					增加目标图像缓冲区附加字节
					增强了容错性

		2001-6		发布新版代码
		2001-5		重新定义模块的接口及输出功能
		2000-10		修正了一个较为重大的BUG
		2000-7		第一个测试版发布


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

#include "bmp.h"


IRWP_INFO			bmp_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	bmp_get_info_critical;	/* bmp_get_image_info函数的关键段 */
CRITICAL_SECTION	bmp_load_img_critical;	/* bmp_load_image函数的关键段 */
CRITICAL_SECTION	bmp_save_img_critical;	/* bmp_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _get_mask(LPBMP_INFO pInfo, unsigned long * pb_mask, 
			  unsigned long * pg_mask, unsigned long * pr_mask, unsigned long * pa_mask);
unsigned char * CALLAGREEMENT _write2buff4RLE(int, int, unsigned char, unsigned char *);
unsigned char * CALLAGREEMENT _write2buff8RLE(int, int, unsigned char, unsigned char *);
int CALLAGREEMENT _calcu_scanline_size(int w, int bit);
int CALLAGREEMENT _bmp_is_valid_img(LPINFOSTR pinfo_str);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&bmp_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&bmp_get_info_critical);
			InitializeCriticalSection(&bmp_load_img_critical);
			InitializeCriticalSection(&bmp_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&bmp_get_info_critical);
			DeleteCriticalSection(&bmp_load_img_critical);
			DeleteCriticalSection(&bmp_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

BMP_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&bmp_irwp_info;
}

#else

BMP_API LPIRWP_INFO CALLAGREEMENT bmp_get_plugin_info()
{
	_init_irwp_info(&bmp_irwp_info);

	return (LPIRWP_INFO)&bmp_irwp_info;
}

BMP_API void CALLAGREEMENT bmp_init_plugin()
{
	/* 初始化多线程同步对象 */
}

BMP_API void CALLAGREEMENT bmp_detach_plugin()
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

	/* 设置bmp模块支持的保存位深 */
	/* ################################################################# */
	/* ## 2001-8-9  YZ 修改：各位数减一，否则32位格式无法表示。*/
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(4-1)) | (1UL<<(8-1)) | \
		(1UL<<(16-1)) | (1UL<<(24-1)) | (1UL<<(32-1));
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
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"做好BMP其实也挺复杂的:)");
	/* ---------------------------------[1] － 第二组 -------------- */
	/* 后续开发者信息可加在此处。
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"##");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	*/
	/* ------------------------------------------------------------- */


	/* 插件描述信息（扩展名信息）*/
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_currency_name), 
				(const char *)MODULE_FILE_POSTFIX);

	lpirwp_info->irwp_desc_info.idi_rev = 0;

	/* 别名个数（##需手动修正） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 2;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
				(const char *)"DIB");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
				(const char *)"RLE");

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
BMP_API int CALLAGREEMENT bmp_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	BMP_FILE_HEADER bfh;
	LPBMP_INFO		pdibinfo = (LPBMP_INFO)0;
	unsigned long	dib_info_len = 0;
	int				style;				/* 位图风格，0－WINDOWS，1－OS/2 */

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&bmp_get_info_critical);

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

			if (isio_read((void*)&bfh, sizeof(BMP_FILE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断是否是有效的位图文件 */
			if (bfh.bfType == DIB_HEADER_MARKER)
				style = 0;
			else if (bfh.bfType == DIB_HEADER_MARKER_OS2)
				style = 1;
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 信息头＋颜色表（包括OS/2风格） */
			dib_info_len = bfh.bfOffBits - sizeof(BMP_FILE_HEADER);

			/* 有一些图像保存的不是很正规，图像标签是Windows风格，而内容却是OS/2风格 */
			if (style == 0)
				if (dib_info_len == sizeof(BMP_CORE_HEADER))
					style = 1;

			if ((pdibinfo = (LPBMP_INFO)malloc(dib_info_len)) == (LPBMP_INFO)0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, sizeof(BMP_FILE_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)pdibinfo, dib_info_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 数据有效性检验 */
			if (style == 0)
			{
				if ((pdibinfo->bmiHeader.biHeight == 0)||(pdibinfo->bmiHeader.biWidth == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
			{
				assert(style == 1);
				if ((((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight == 0)||(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_BMP;			/* 图像文件格式（后缀名） */

			if (style == 0)
			{
				switch(pdibinfo->bmiHeader.biCompression)	/* 图像的压缩方式 */
				{
				case	0:
					pinfo_str->compression = ICS_RGB; break;
				case	1:
					pinfo_str->compression = ICS_RLE8; break;
				case	2:
					pinfo_str->compression = ICS_RLE4; break;
				case	3:
					pinfo_str->compression = ICS_BITFIELDS; break;
				default:
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
				pinfo_str->compression = ICS_RGB;	/* OS/2 无压缩位图格式 */
			

			/* 填写图像信息 */
			if (style == 0)
			{
				pinfo_str->width	= pdibinfo->bmiHeader.biWidth;
				pinfo_str->height	= (unsigned long)abs(pdibinfo->bmiHeader.biHeight);
				pinfo_str->order	= (pdibinfo->bmiHeader.biHeight < 0) ? 0 : 1;
				pinfo_str->bitcount	= pdibinfo->bmiHeader.biBitCount;
				
				if (_get_mask(pdibinfo, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
			{
				pinfo_str->width	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth);
				pinfo_str->height	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight);
				pinfo_str->order	= 1;	/* OS/2位图都是倒向的 */
				pinfo_str->bitcount	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcBitCount);

				/* OS/2 位图的有效位深只有：1、4、8、24 */
				if (pinfo_str->bitcount == 24)
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else
				{
					pinfo_str->b_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->r_mask = 0x0;
					pinfo_str->a_mask = 0x0;
				}
			}
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pdibinfo)
				free(pdibinfo);
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&bmp_get_info_critical);
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
BMP_API int CALLAGREEMENT bmp_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	BMP_FILE_HEADER bfh;
	LPBMP_INFO		pdibinfo = (LPBMP_INFO)0;

	unsigned long	dib_info_len;		/* DIB信息结构尺寸 */
	unsigned long	dib_data_len;		/* DIB位数据尺寸 */
	unsigned int	pal_num;			/* 调色板项个数 */
	int				i, linesize;
	int				style;				/* 位图风格，0－WINDOWS，1－OS/2 */
	
	int				bEOF = 0;			/* 图像结尾标志 */
	int				line = 0;			/* 当前行 */
	unsigned char	pix, pix2;			/* 当前值 */

	unsigned char	buf, tmp, *pc;
	unsigned short	us;
	int				cnv_count, bit, mask;
	
	unsigned char	*pbit = (unsigned char *)0;
	unsigned char	*prle = (unsigned char *)0;
	unsigned char	*prlesou = (unsigned char *)0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&bmp_load_img_critical);

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

			if (isio_read((void*)&bfh, sizeof(BMP_FILE_HEADER), 1, pfile) == 0)
			{
				b_status = (pinfo_str->data_state == 1) ? ER_FILERWERR:ER_NONIMAGE; 
				__leave;
			}
			
			/* 判断是否是有效的位图文件 */
			if (bfh.bfType == DIB_HEADER_MARKER)
				style = 0;
			else if (bfh.bfType == DIB_HEADER_MARKER_OS2)
				style = 1;
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}


			/* 计算 BMP_INFO 的尺寸 */
			dib_info_len = bfh.bfOffBits - sizeof(BMP_FILE_HEADER);
			
			/* 有一些图像保存的不是很正规，图像标签是Windows风格，而内容却是OS/2风格 */
			if (style == 0)
				if (dib_info_len == sizeof(BMP_CORE_HEADER))
					style = 1;
				
			/* 分配存放 BMP_INFO 信息的内存块 */
			if ((pdibinfo = (LPBMP_INFO)malloc(dib_info_len)) == (LPBMP_INFO)0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 读信息头结构 */
			if (isio_seek(pfile, sizeof(BMP_FILE_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)pdibinfo, dib_info_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 数据有效性检验 */
				if (style == 0)
				{
					if ((pdibinfo->bmiHeader.biHeight == 0)||(pdibinfo->bmiHeader.biWidth == 0))
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
				{
					assert(style == 1);
					if ((((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight == 0)||(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth == 0))
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}

				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_BMP;			/* 图像文件格式（后缀名） */
				
				if (style == 0)
				{
					switch(pdibinfo->bmiHeader.biCompression)	/* 图像的压缩方式 */
					{
					case	0:
						pinfo_str->compression = ICS_RGB; break;
					case	1:
						pinfo_str->compression = ICS_RLE8; break;
					case	2:
						pinfo_str->compression = ICS_RLE4; break;
					case	3:
						pinfo_str->compression = ICS_BITFIELDS; break;
					default:
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
					pinfo_str->compression = ICS_RGB;

				if (style == 0)
				{
					pinfo_str->width	= pdibinfo->bmiHeader.biWidth;
					pinfo_str->height	= (unsigned long)abs(pdibinfo->bmiHeader.biHeight);
					pinfo_str->order	= (pdibinfo->bmiHeader.biHeight < 0) ? 0 : 1;
					pinfo_str->bitcount	= pdibinfo->bmiHeader.biBitCount;
					
					if (_get_mask(pdibinfo, 
						(unsigned long *)(&pinfo_str->b_mask), 
						(unsigned long *)(&pinfo_str->g_mask), 
						(unsigned long *)(&pinfo_str->r_mask),
						(unsigned long *)(&pinfo_str->a_mask)) == 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
				{
					pinfo_str->width	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcWidth);
					pinfo_str->height	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcHeight);
					pinfo_str->order	= 1;	/* OS/2位图都是倒向的 */
					pinfo_str->bitcount	= (unsigned long)(((LPBMP_CORE)pdibinfo)->bmciHeader.bcBitCount);
					
					/* OS/2 位图的有效位深只有：1、4、8、24 */
					if (pinfo_str->bitcount == 24)
					{
						pinfo_str->b_mask = 0xff;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->r_mask = 0xff0000;
						pinfo_str->a_mask = 0x0;
					}
					else
					{
						pinfo_str->b_mask = 0x0;
						pinfo_str->g_mask = 0x0;
						pinfo_str->r_mask = 0x0;
						pinfo_str->a_mask = 0x0;
					}
				}
				
				pinfo_str->data_state = 1;
			}


			/* 设置调色板数据 */
			if (pinfo_str->bitcount <= 8)
			{
				pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

				if (style == 0)
				{
					pal_num = (pdibinfo->bmiHeader.biClrUsed == 0) ? pinfo_str->pal_count : pdibinfo->bmiHeader.biClrUsed;
					if (pal_num > pinfo_str->pal_count)
					{
						b_status = ER_NONIMAGE; __leave;
					}
					memmove((void*)(pinfo_str->palette), (const void *)(pdibinfo->bmiColors), sizeof(RGB_QUAD)*pal_num);
				}
				else
				{
					for (i=0;i<(int)pinfo_str->pal_count;i++)
					{
						((LPRGB_TRIPLE)(&pinfo_str->palette[i]))->rgbtBlue = ((LPBMP_CORE)pdibinfo)->bmciColors[i].rgbtBlue;
						((LPRGB_TRIPLE)(&pinfo_str->palette[i]))->rgbtGreen = ((LPBMP_CORE)pdibinfo)->bmciColors[i].rgbtGreen;
						((LPRGB_TRIPLE)(&pinfo_str->palette[i]))->rgbtRed = ((LPBMP_CORE)pdibinfo)->bmciColors[i].rgbtRed;
						pinfo_str->palette[i] &= 0xffffff;	/* 最高字节置零 */
					}
				}
			}
			else
			{
				pinfo_str->pal_count = 0;
			}


			/* 取得位数据长度 */
			dib_data_len = isio_length(pfile) - bfh.bfOffBits;
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


			/* 是否是RLE */
			if ((pinfo_str->compression == ICS_RLE4)||(pinfo_str->compression == ICS_RLE8))
			{
				assert(style == 0);

				/* 分配内存，用于存放RLE原始数据 */
				prlesou = prle = (unsigned char *)malloc(dib_data_len);
				if (!prle)
				{
					b_status = ER_MEMORYERR; __leave;
				}

				/* 读取RLE原始数据 */
				if (isio_seek(pfile, bfh.bfOffBits, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				if (isio_read((void*)prle, dib_data_len, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 为RLE解压做准备（暂时作为写指针数组） */
				for (i=0;i<(int)(pinfo_str->height);i++)
					pinfo_str->pp_line_addr[i] = (void*)(pinfo_str->p_bit_data+(i*linesize));
				
				if (pinfo_str->compression == ICS_RLE4)
				{
					_write2buff4RLE(0, 0, 0, (unsigned char*)pinfo_str->pp_line_addr[line]);	/* 初始化写函数 */

					/* 先将RLE的数据解压成标准的BMP数据 */
					while(!bEOF)
					{
						pix = *prle++;
						
						if (pix == 0)	/* 绝对方式 */
						{
							pix2 = *prle++;
							switch(pix2)
							{
							case	0:		/* 行结尾 */
								line++;
								_write2buff4RLE(2, 0, 0, (unsigned char *)pinfo_str->pp_line_addr[line]);/* 指定新的偏移地址 */
								break;
							case	1:		/* 图像结尾 */
								bEOF = 1;
								break;
							case	2:		/* 当前地址偏移 */
								pix = *prle++; pix2 = *prle++;
								line += pix2;	/* 下移pix2行 */
								_write2buff4RLE(2, pix%2, 0, (unsigned char *)((unsigned char *)pinfo_str->pp_line_addr[line]+(unsigned long)pix/2));
								break;
							default:		/* 不能压缩的像素 */
								for (i=0;i<pix2/2;i++)
								{
									pix = *(prle+i);
									_write2buff4RLE(1, 2, pix, (unsigned char *)0);
								}
								if (pix2%2)
								{	/* 补充遗漏的单个像素 */
									pix = *(prle+i);
									_write2buff4RLE(1, 1, pix, (unsigned char *)0);
								}
								/* (pix2+1)/2是将像素对齐到字节边界上，再加1、除2是对齐到字边界上 */
								prle = (unsigned char *)(prle + ((pix2+1)/2+1)/2*2);	/* 字对齐 */
								break;
							}
						}
						else			/* 编码方式 */
						{
							/* pix == 相同像素的个数 */
							pix2 = *prle++;	/* pix2 == 像素 */
							_write2buff4RLE(1, (int)pix, pix2, (unsigned char *)0);
						}
					}
					
					_write2buff4RLE(3, 0, 0, (unsigned char *)0);	/* 结束写函数 */

					/* 4位位图，颠倒字节内象素排列 */
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						pc = (unsigned char *)(pinfo_str->pp_line_addr[i]);
						for (cnv_count=0;cnv_count<linesize;cnv_count++)
						{
							buf = *(unsigned char *)(pc+cnv_count);
							tmp = (buf>>4)|(buf<<4);
							*(unsigned char *)(pc+cnv_count) = tmp;
						}
					}
				}
				else
				{
					_write2buff8RLE(0, 0, 0, (unsigned char *)pinfo_str->pp_line_addr[line]);	/* 初始化写函数 */
					
					while(!bEOF)
					{
						pix = *prle++;
						
						if (pix == 0)	/* 绝对方式 */
						{
							pix2 = *prle++;
							switch(pix2)
							{
							case	0:		/* 行结尾 */
								line++;
								_write2buff8RLE(2, 0, 0, (unsigned char *)pinfo_str->pp_line_addr[line]);/* 指定新的偏移地址 */
								break;
							case	1:		/* 图像结尾 */
								bEOF = 1;
								break;
							case	2:		/* 当前地址偏移 */
								pix = *prle++; pix2 = *prle++;
								line += pix2;	/* 下移pix2行 */
								_write2buff8RLE(2, 0, 0, (unsigned char *)((unsigned char *)pinfo_str->pp_line_addr[line]+(unsigned long)pix));
								break;
							default:		/* 不能压缩的像素 */
								for (i=0;i<pix2;i++)
								{
									pix = *(prle+i);
									_write2buff8RLE(1, 1, pix, (unsigned char *)0);
								}
								/* (pix2+1)/2是将像素对齐到字节边界上，再加1、除2是对齐到字边界上 */
								prle = (unsigned char *)(prle + ((pix2+1)/2)*2);	/* 字对齐 */
								break;
							}
						}
						else			/* 编码方式 */
						{
							/* pix == 相同像素的个数 */
							pix2 = *prle++;	/* pix2 == 像素 */
							_write2buff8RLE(1, (int)pix, pix2, (unsigned char *)0);
						}
					}
					
					_write2buff8RLE(3, 0, 0, (unsigned char *)0);	/* 结束写函数 */
				}

				/* 初始化行首地址（RLE位图都是倒向的） */
				for (i=0;i<(int)(pinfo_str->height);i++)
					pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
			else
			{
				if (pinfo_str->order == 0)		/* 正向 */
				{
					for (i=0;i<(int)(pinfo_str->height);i++)
						pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
				}
				else							/* 倒向 */
				{
					for (i=0;i<(int)(pinfo_str->height);i++)
						pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
				}

				/* 读取位数据到内存 */
				if (isio_seek(pfile, bfh.bfOffBits, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pbit = pinfo_str->p_bit_data;

				/* 逐行读入 */
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					if (isio_read((void*)pbit, linesize, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* 如果是1位位图，则颠倒字节内象素排列 */
					if (pinfo_str->bitcount == 1)
					{
						for (cnv_count=0;cnv_count<linesize;cnv_count++)
						{
							buf = *(unsigned char *)(pbit+cnv_count);
							us  = ((unsigned short)buf)<<8;
							tmp = 0;
							for (bit=1, mask=7;bit<16;bit+=2, mask--)
								tmp |= (us>>bit)&(1<<mask);

							*(unsigned char *)(pbit+cnv_count) = tmp;
						}
					}

					/* 如果是4位位图，则颠倒字节内象素排列 */
					if (pinfo_str->bitcount == 4)
					{
						for (cnv_count=0;cnv_count<linesize;cnv_count++)
						{
							buf = *(unsigned char *)(pbit+cnv_count);
							tmp = (buf>>4)|(buf<<4);
							*(unsigned char *)(pbit+cnv_count) = tmp;
						}
					}

					pinfo_str->process_current = i+1;
					pbit = (unsigned char *)(pbit + linesize);

					/* 支持用户中断 */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
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

			if (prlesou)
				free(prlesou);
			if (pdibinfo)
				free(pdibinfo);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&bmp_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 （只能保存Windows风格的位图）*/
BMP_API int CALLAGREEMENT bmp_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	BMP_FILE_HEADER bfh;
	LPBMP_INFO		pdibinfo = (LPBMP_INFO)0;
	
	unsigned long	dib_info_len;		/* DIB信息结构尺寸 */
	unsigned char	*plinebuf = (unsigned char *)0;
	int				i, linesize;
	
	unsigned char	buf, tmp;
	unsigned short	us;
	int				cnv_count, bit, mask;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&bmp_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_bmp_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}


			/* 计算位图信息头结构的尺寸（包括调色板） */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	4:
			case	8:
				dib_info_len = sizeof(BMP_INFO_HEADER) + pinfo_str->pal_count*sizeof(RGB_QUAD);
				break;
			case	24:
				dib_info_len = sizeof(BMP_INFO_HEADER);
				break;
			case	16:
			case	32:
				dib_info_len = sizeof(BMP_INFO_HEADER) + 3 * sizeof(unsigned long);
				break;
			default:
				assert(0);
				b_status = ER_SYSERR; __leave;
			}

			/* 分配存放 BMP_INFO 信息的内存块 */
			if ((pdibinfo = (LPBMP_INFO)malloc(dib_info_len)) == (LPBMP_INFO)0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			pdibinfo->bmiHeader.biSize			= sizeof(BMP_INFO_HEADER);
			pdibinfo->bmiHeader.biPlanes		= 1;
			pdibinfo->bmiHeader.biBitCount		= (unsigned short)pinfo_str->bitcount;
			pdibinfo->bmiHeader.biCompression	= ((pinfo_str->bitcount == 16)||(pinfo_str->bitcount == 32)) ? 3 : 0;
			pdibinfo->bmiHeader.biWidth			= pinfo_str->width;
			/* 全部保存为倒向图 */
			pdibinfo->bmiHeader.biHeight		= (long)pinfo_str->height;
			pdibinfo->bmiHeader.biSizeImage		= 0;
			pdibinfo->bmiHeader.biClrImportant	= 0;
			pdibinfo->bmiHeader.biClrUsed		= 0;
			pdibinfo->bmiHeader.biXPelsPerMeter = 0;
			pdibinfo->bmiHeader.biYPelsPerMeter = 0;

			if (pdibinfo->bmiHeader.biCompression == 0)
				memmove((void*)pdibinfo->bmiColors, (const void *)pinfo_str->palette, pinfo_str->pal_count * sizeof(RGB_QUAD));
			else
			{
				*((unsigned long *)&(pdibinfo->bmiColors[0])) = pinfo_str->r_mask;
				*((unsigned long *)&(pdibinfo->bmiColors[1])) = pinfo_str->g_mask;
				*((unsigned long *)&(pdibinfo->bmiColors[2])) = pinfo_str->b_mask;
			}

			/* 填写位图文件头结构 */
			bfh.bfSize		= sizeof(BMP_FILE_HEADER);
			bfh.bfType		= DIB_HEADER_MARKER;
			bfh.bfOffBits	= bfh.bfSize+dib_info_len;
			bfh.bfReserved1 = bfh.bfReserved2 = 0;
				
			/* 设置进度值 */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 取得扫描行尺寸 */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			
			/* 申请写数据缓冲区 */
			if ((plinebuf=(unsigned char *)malloc(linesize)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入文件头结构 */
			if (isio_write((const void *)&bfh, sizeof(BMP_FILE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 写入信息头结构 */
			if (isio_write((const void *)pdibinfo, dib_info_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入图像位数据 
				注：传入的图像数据必须是ISee位流格式，否则可能会产生失真
			*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				memcpy((void*)plinebuf, (const void *)(unsigned char *) \
					(pinfo_str->pp_line_addr[pinfo_str->height-1-i]), linesize);

				/* 如果是1位位图，则颠倒字节内象素排列 */
				if (pinfo_str->bitcount == 1)
				{
					for (cnv_count=0;cnv_count<linesize;cnv_count++)
					{
						buf = *(unsigned char *)(plinebuf+cnv_count);
						us  = ((unsigned short)buf)<<8;
						tmp = 0;
						for (bit=1, mask=7;bit<16;bit+=2, mask--)
							tmp |= (us>>bit)&(1<<mask);
						
						*(unsigned char *)(plinebuf+cnv_count) = tmp;
					}
				}
				
				/* 如果是4位位图，则颠倒字节内象素排列 */
				if (pinfo_str->bitcount == 4)
				{
					for (cnv_count=0;cnv_count<linesize;cnv_count++)
					{
						buf = *(unsigned char *)(plinebuf+cnv_count);
						tmp = (buf>>4)|(buf<<4);
						*(unsigned char *)(plinebuf+cnv_count) = tmp;
					}
				}
				
				/* 一律保存为倒向图 */
				if (isio_write((const void*)plinebuf, linesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i+1;
				
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
			if (plinebuf)
				free(plinebuf);
			if (pdibinfo)
				free(pdibinfo);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&bmp_save_img_critical);
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


/* 获取各颜色分量的值 */
int CALLAGREEMENT _get_mask(LPBMP_INFO pInfo, 
	unsigned long * pb_mask, 
	unsigned long * pg_mask, 
	unsigned long * pr_mask, 
	unsigned long * pa_mask)
{
	int result = 0;
	unsigned long *pMark;

	assert(pInfo->bmiHeader.biSize);

	switch(pInfo->bmiHeader.biBitCount)
	{
	case	1:
	case	4:
	case	8:
		/* 调色板位图无掩码 */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:
		/*******************************************
				16位掩码示意图

			  高字节      低字节
			0000 0000   0000 0000 
						   1 1111--B	555格式
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	565格式
				  111   111 -------G
			1111 1   --------------R

		* Win95 系统只支持以上两种格式（NT支持多种格式）
		*******************************************/
		if (pInfo->bmiHeader.biCompression == 0)
		{
			*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
			*pb_mask = 0x1f;		*pa_mask = 0;
			break;
		}
		else
		{
			pMark = (unsigned long *)((LPBMP_INFO_HEADER)pInfo+1);

			*pr_mask = pMark[0];		*pg_mask = pMark[1];
			*pb_mask = pMark[2];		*pa_mask = 0;
			break;
		}
		break;
	case	24:
		/* assert(pInfo->bmiHeader.biCompression == 0); */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:
		/******************************************************
						32位掩码示意图
			    高               ->                 低
		0000 0000   0000 0000   0000 0000   0000 0000 	888格式
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		* Win95 系统只支持这一种格式
		******************************************************/
		if (pInfo->bmiHeader.biCompression == 0)
		{
			*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
			*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		}
		else
		{
			pMark = (unsigned long *)((LPBMP_INFO_HEADER)pInfo+1);
			
			*pr_mask = pMark[0];		*pg_mask = pMark[1];
			*pb_mask = pMark[2];		*pa_mask = 0;
		}
		break;
	default:
		result = 1;

		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}

	return result;	/* 0-成功， 1－非法的图像格式 */
}


/* 向4位缓冲区写像素
   形参mark含意：0-初始化变量  1-写像素  2-移动到指定点  3-结束操作  4-获取当前地址
   注：当mark等于2时，参数num将作为像素除2余数被代入，来对iCurOff赋值
*/
unsigned char * CALLAGREEMENT _write2buff4RLE(int mark, int num, unsigned char pix, unsigned char *lpDesData)
{
	static unsigned char *lpDes;		/* 缓冲区基地址 */
	static unsigned char *lpCur;		/* 缓冲区当前写地址 */
	static int			  iCurOff;		/* 在字节中的当前位置，0-高半字节  1-低半字节 */
	int i;
	unsigned char pixa[2];
	
	switch(mark)
	{
	case	0:		/* 初始化变量 */
		lpDes = lpDesData;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	1:		/* 写像素 */
		pixa[0] = pix>>4;
		pixa[1] = pix&0xf;
		for (i=0;i<num;i++)
		{
			if (iCurOff == 0)	/* 左像素 */
			{
				*lpCur = pixa[i%2]<<4;
				iCurOff = 1;
			}
			else				/* 右像素 */
			{
				assert(iCurOff == 1);
				*lpCur |= pixa[i%2];
				lpCur++;
				iCurOff = 0;
			}
		}
		break;
	case	2:		/* 移动到指定点 */
		lpCur = lpDesData;
		iCurOff = num;
		break;
	case	3:		/* 结束操作 */
		lpDes = (unsigned char *)0;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	4:		/* 获取当前地址 */
		break;
	default:
		assert(0);
		break;
	}
	
	return lpCur;
}



/* 向8位缓冲区写像素（只供_from_8_RLE_read()函数内部调用）
   形参mark含意：0-初始化变量  1-写像素  2-移动到指定点  3-结束操作  4-获取当前地址
*/
unsigned char * CALLAGREEMENT _write2buff8RLE(int mark, int num, unsigned char pix, unsigned char *lpDesData)
{
	static unsigned char *lpDes;		/* 缓冲区基地址 */
	static unsigned char *lpCur;		/* 缓冲区当前写地址 */
	int i;
	
	switch(mark)
	{
	case	0:		/* 初始化变量 */
		lpDes = lpDesData;
		lpCur = lpDes;
		break;
	case	1:		/* 写像素 */
		for (i=0;i<num;i++)
			*lpCur++ = pix;
		break;
	case	2:		/* 移动到指定点 */
		lpCur = lpDesData;
		break;
	case	3:		/* 结束操作 */
		lpDes = (unsigned char *)0;
		lpCur = lpDes;
		break;
	case	4:		/* 获取当前地址 */
		break;
	default:
		assert(0);
		break;
	}
	return lpCur;
}



/* 计算扫描行尺寸(四字节对齐) */
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _bmp_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* ## 2001-8-9  YZ 修改：位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(bmp_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (bmp_irwp_info.irwp_save.img_num)
		if (bmp_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */

	return 0;
}