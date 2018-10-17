/********************************************************************

	ras.c

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
	本文件用途：	ISee图像浏览器—Sun RAS图像读写模块实现文件

					读取功能：可读取1、8、24、32位压缩与未压缩RAS图象
							  
					保存功能：提供24位非压缩格式的保存功能

	本文件编写人：	
					netmap		beahuang##hotmail.com
					YZ			yzfree##yeah.net

	本文件版本：	20610
	最后修改于：	2002-6-10

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	注：本文件的部分函数参考了FreeImage图象库的源代码，在此向
			Floris van den Berg (flvdberg@wxs.nl)
			Herv Drolon (drolon@iut.univ-lehavre.fr)
		表示感谢！如果想更多的了解FreeImage图象库的情况，请拜
		访它的官方网站：
			http://www.6ixsoft.com/
	----------------------------------------------------------------
	修正历史：

		2002-6		第一个版本发布（新版）

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

#include "ras.h"


IRWP_INFO			ras_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	ras_get_info_critical;	/* ras_get_image_info函数的关键段 */
CRITICAL_SECTION	ras_load_img_critical;	/* ras_load_image函数的关键段 */
CRITICAL_SECTION	ras_save_img_critical;	/* ras_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _ras_is_valid_img(LPINFOSTR pinfo_str);

enum EXERESULT CALLAGREEMENT _read_data(ISFILE *, unsigned char *, long, int, int);





#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&ras_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&ras_get_info_critical);
			InitializeCriticalSection(&ras_load_img_critical);
			InitializeCriticalSection(&ras_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&ras_get_info_critical);
			DeleteCriticalSection(&ras_load_img_critical);
			DeleteCriticalSection(&ras_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

RAS_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&ras_irwp_info;
}

#else

RAS_API LPIRWP_INFO CALLAGREEMENT ras_get_plugin_info()
{
	_init_irwp_info(&ras_irwp_info);

	return (LPIRWP_INFO)&ras_irwp_info;
}

RAS_API void CALLAGREEMENT ras_init_plugin()
{
	/* 初始化多线程同步对象 */
}

RAS_API void CALLAGREEMENT ras_detach_plugin()
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
	/* 各位数减一，否则32位格式无法表示。此处请用RAS正确的位深填写！*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1－只能保存一副图像 */
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 2;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"netmap");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"beahuang##hotmail.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"How do you do!");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"一只蚂蚁？！我踩、我踩、我踩。。。佛家说“扫地不伤蝼蚁命”，唉！");
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

	/* 别名个数（##需手动修正，如RAS格式有别名请参考BMP插件中别名的设置） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
RAS_API int CALLAGREEMENT ras_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	SUNHeaderinfo	header;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&ras_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 读取头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&header, sizeof(SUNHeaderinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 转至X86字序 */
			header.ras_magic     = EXCHANGE_DWORD((header.ras_magic));
			header.ras_width     = EXCHANGE_DWORD((header.ras_width));
			header.ras_height    = EXCHANGE_DWORD((header.ras_height));
			header.ras_depth     = EXCHANGE_DWORD((header.ras_depth));
			header.ras_length    = EXCHANGE_DWORD((header.ras_length));
			header.ras_type      = EXCHANGE_DWORD((header.ras_type));
			header.ras_maptype   = EXCHANGE_DWORD((header.ras_maptype));
			header.ras_maplength = EXCHANGE_DWORD((header.ras_maplength));
			
			/* 判断是否是RAS图象 */
			if (header.ras_magic != RAS_MAGIC)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 图象类型数据是否合法（RT_FORMAT_TIFF、RT_FORMAT_IFF、
			   RT_EXPERIMENTAL格式将不被支持）*/
			if ((header.ras_type != RT_OLD) && 
				(header.ras_type != RT_STANDARD) && 
				(header.ras_type != RT_BYTE_ENCODED) && 
				(header.ras_type != RT_FORMAT_RGB))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 判断尺寸值是否合法 */
			if ((header.ras_width == 0)||(header.ras_height == 0)||(header.ras_depth == 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}

			switch (header.ras_depth)
			{
			case	1:
			case	8:
			case	24:
			case	32:
				break;
			default:
				b_status = ER_BADIMAGE; __leave;	/* 插件将不支持以上四种格式之外的图象*/
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_RAS;			/* 图像文件格式（后缀名） */

			switch (header.ras_type)					/* 图像的压缩方式 */
			{
			case RT_OLD:
			case RT_STANDARD:
			case RT_FORMAT_RGB:
				pinfo_str->compression = ICS_RGB;
				break;
			case RT_BYTE_ENCODED:
				pinfo_str->compression = ICS_RLE8;
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave;
			}
			
			/* 填写图像信息 */
			pinfo_str->width	= header.ras_width;
			pinfo_str->height	= header.ras_height;
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= header.ras_depth;
			
			/* 填写掩码数据 */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	8:
				SETMASK_8(pinfo_str);
				break;
			case	24:
				SETMASK_24(pinfo_str);
				break;
			case	32:
				SETMASK_32(pinfo_str);
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave;
			}
		
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&ras_get_info_critical);
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
RAS_API int CALLAGREEMENT ras_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				linesize, linelength, fill, i, j, numcolors, isrle, isrgb, fillchar;
	unsigned char	c, *r = 0, *g, *b, *colormap = 0, *buf = 0, *bp, *bits;
	SUNHeaderinfo	header;
	LPPALITEM		pal;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&ras_load_img_critical);

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

			/* 读文件头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&header, sizeof(SUNHeaderinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 转至X86字序 */
			header.ras_magic     = EXCHANGE_DWORD((header.ras_magic));
			header.ras_width     = EXCHANGE_DWORD((header.ras_width));
			header.ras_height    = EXCHANGE_DWORD((header.ras_height));
			header.ras_depth     = EXCHANGE_DWORD((header.ras_depth));
			header.ras_length    = EXCHANGE_DWORD((header.ras_length));
			header.ras_type      = EXCHANGE_DWORD((header.ras_type));
			header.ras_maptype   = EXCHANGE_DWORD((header.ras_maptype));
			header.ras_maplength = EXCHANGE_DWORD((header.ras_maplength));
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if (header.ras_magic != RAS_MAGIC)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				if ((header.ras_type != RT_OLD) && 
					(header.ras_type != RT_STANDARD) && 
					(header.ras_type != RT_BYTE_ENCODED) && 
					(header.ras_type != RT_FORMAT_RGB))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				if ((header.ras_width == 0)||(header.ras_height == 0)||(header.ras_depth == 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				switch (header.ras_depth)
				{
				case	1:
				case	8:
				case	24:
				case	32:
					break;
				default:
					b_status = ER_BADIMAGE; __leave;	/* 插件将不支持以上四种格式之外的图象*/
				}

				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_RAS;			/* 图像文件格式（后缀名） */

				switch (header.ras_type)					/* 图像的压缩方式 */
				{
				case RT_OLD:
				case RT_STANDARD:
				case RT_FORMAT_RGB:
					pinfo_str->compression = ICS_RGB;
					break;
				case RT_BYTE_ENCODED:
					pinfo_str->compression = ICS_RLE8;
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave;
				}
				
				pinfo_str->width	= header.ras_width;
				pinfo_str->height	= header.ras_height;
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= header.ras_depth;
				
				switch (pinfo_str->bitcount)
				{
				case	1:
				case	8:
					SETMASK_8(pinfo_str);
					break;
				case	24:
					SETMASK_24(pinfo_str);
					break;
				case	32:
					SETMASK_32(pinfo_str);
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave;
				}

				pinfo_str->data_state = 1;
			}
			
			pinfo_str->process_total   = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			isrle = 0;
			isrgb = 0;
			
			/* 设定象素数据存放方式（是否压缩、及RGB排放顺序）*/
			switch (header.ras_type)
			{
			case RT_OLD:
			case RT_STANDARD:		/* 非压缩、BGR排列 */
				break;
			case RT_BYTE_ENCODED:	/* 压缩 */
				isrle = 1;
				break;
			case RT_FORMAT_RGB:		/* 非压缩、RGB排列 */
				isrgb = 1;
				break;
			case RT_FORMAT_TIFF:	/* 不支持的存储格式 */
			case RT_FORMAT_IFF:
			default:
				b_status = ER_BADIMAGE; __leave; break;
			}
	

			/* 设置调色板数据 */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	8:
				pinfo_str->pal_count = numcolors = 1 << header.ras_depth;
				
				switch (header.ras_maptype)
				{
				case RMT_NONE:			/* 人工合成灰度调色板 */
					pal = (LPPALITEM)pinfo_str->palette;
					
					for (i = 0; i < numcolors; i++)
					{
						c = (unsigned char)((255*i)/(numcolors-1));

						pal[i].red	 = c;
						pal[i].green = c;
						pal[i].blue	 = c;
						pal[i].reserved = 0;
					}
					break;
				case RMT_EQUAL_RGB:		/* 读取调色板数据 */
					pal = (LPPALITEM)pinfo_str->palette;
					
					/* 判断图象调色板长度域的值是否合法 */
					if (header.ras_maplength > (3*numcolors*sizeof(unsigned char)))
					{
						b_status = ER_BADIMAGE; __leave; break;
					}

					/* 按最大可能分配调色板缓冲区 */
					if ((r=(unsigned char *)malloc(3*numcolors*sizeof(unsigned char))) == 0)
					{
						b_status = ER_MEMORYERR; __leave; break;
					}

					/* 计算实际调色板项个数 */
					numcolors = header.ras_maplength/3;
					
					g = r + numcolors;
					b = g + numcolors;
					
					/* 读入调色板数据 */
					if (isio_read((void*)r, header.ras_maplength, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave; break;
					}
					
					for (i = 0; i < numcolors; i++)
					{
						pal[i].red	 = r[i];
						pal[i].green = g[i];
						pal[i].blue	 = b[i];
						pal[i].reserved = 0;
					}
					break;
				case RMT_RAW:			/* 跳过裸调色板数据 */
					/* 未能搞清这种调色板的格式，暂时跳过 */
					if ((colormap=(unsigned char *)malloc(header.ras_maplength * sizeof(unsigned char))) == 0)
					{
						b_status = ER_MEMORYERR; __leave; break;
					}
					
					if (isio_read((void*)colormap, header.ras_maplength, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave; break;
					}
					break;
				default:				/* 非法的调色板标签值 */
					b_status = ER_BADIMAGE; __leave; break;
				}
				break;
			case	24:
			case	32:
				/* 如果图象中存在调色板数据，则说明是一个不正常的图象 */
				if (header.ras_maptype != RMT_NONE)
				{
					b_status = ER_BADIMAGE; __leave; break;
				}

				pinfo_str->pal_count = 0;

				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}
			
			
			/* 取得扫描行尺寸 */
			linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);
			
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
			
			/* 初始化行首地址数组（RAS图像都为正向） */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}
			
			if (header.ras_depth == 1)
				linelength = (int)((header.ras_width/8)+(header.ras_width%8 ? 1 : 0));
			else
				linelength = (int)header.ras_width;

			/* RAS扫描行为双字节对齐 */
			fill = (linelength % 2) ? 1 : 0;

			
			/* 初始化读取函数 */
			_read_data(pfile, 0, 0, 0, 1);

			/* 逐行读入 */
			switch (pinfo_str->bitcount)
			{
			case 1:
			case 8:
				for (i=0; i<(int)pinfo_str->height; i++)
				{
					bits = pinfo_str->p_bit_data+i*linesize;

					/* 读入一行的数据 */
					if ((b_status=_read_data(pfile, bits, linelength, isrle, 0)) != ER_SUCCESS)
					{
						__leave; break;
					}

					/* 如果有的话，读取行尾填充字节 */
					if (fill)
					{
						if ((b_status=_read_data(pfile, (unsigned char *)&fillchar, fill, isrle, 0)) != ER_SUCCESS)
						{
							__leave; break;
						}
					}

					pinfo_str->process_current = i;
					
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
				break;
			case 24:
				/* 分配缓冲区 */
				if ((buf=(unsigned char *)malloc(pinfo_str->width*3)) == 0)
				{
					b_status = ER_MEMORYERR; __leave; break;
				}

				for (i=0; i<(int)pinfo_str->height; i++)
				{
					bits = pinfo_str->p_bit_data+i*linesize;

					if ((b_status=_read_data(pfile, buf, pinfo_str->width*3, isrle, 0)) != ER_SUCCESS)
					{
						__leave; break;
					}

					bp = buf;

					for (j=0; j<(int)pinfo_str->width; j++)
					{
						bits[2] = *bp++; /* red */
						bits[1] = *bp++; /* green */
						bits[0] = *bp++; /* blue */
						
						bits += 3;
					}

#if 0
					/* 下面这段代码与实测图象格式冲突，isrgb为假时象素仍为RGB排列！*/
					/* 暂时废弃这段代码 */
					if (isrgb)		/* RGB排列 */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[2] = *bp++; /* red */
							bits[1] = *bp++; /* green */
							bits[0] = *bp++; /* blue */
							
							bits += 3;
						}
					}
					else			/* BGR排列 */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[0] = *bp++; /* blue */
							bits[1] = *bp++; /* green */
							bits[2] = *bp++; /* red */
							
							bits += 3;
						}
					}
#endif

					if (fill)
					{
						if ((b_status=_read_data(pfile, (unsigned char *)&fillchar, fill, isrle, 0)) != ER_SUCCESS)
						{
							__leave; break;
						}
					}

					pinfo_str->process_current = i;
					
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
				break;
			case 32:
				/* 分配缓冲区 */
				if ((buf=(unsigned char *)malloc(pinfo_str->width*4)) == 0)
				{
					b_status = ER_MEMORYERR; __leave; break;
				}
				
				for (i=0; i<(int)pinfo_str->height; i++)
				{
					bits = pinfo_str->p_bit_data+i*linesize;
					
					if ((b_status=_read_data(pfile, buf, pinfo_str->width*4, isrle, 0)) != ER_SUCCESS)
					{
						__leave; break;
					}
					
					bp = buf;
					
					for (j=0; j<(int)pinfo_str->width; j++)
					{
						bits[3] = *bp++; /* empty */
						bits[2] = *bp++; /* red */
						bits[1] = *bp++; /* green */
						bits[0] = *bp++; /* blue */
						
						bits += 4;
					}

#if 0
					/* 注释原因见上 */
					if (isrgb)		/* RGB排列 */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[3] = *bp++; /* empty */
							bits[2] = *bp++; /* red */
							bits[1] = *bp++; /* green */
							bits[0] = *bp++; /* blue */
							
							bits += 4;
						}
					}
					else			/* BGR排列 */
					{
						for (j=0; j<(int)pinfo_str->width; j++)
						{
							bits[3] = *bp++; /* empty */
							bits[0] = *bp++; /* blue */
							bits[1] = *bp++; /* green */
							bits[2] = *bp++; /* red */
							
							bits += 4;
						}
					}
#endif
					
					if (fill)
					{
						if ((b_status=_read_data(pfile, (unsigned char *)&fillchar, fill, isrle, 0)) != ER_SUCCESS)
						{
							__leave; break;
						}
					}

					pinfo_str->process_current = i;
					
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
				break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
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

			if (colormap)
				free(colormap);

			if (r)
				free(r);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&ras_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
RAS_API int CALLAGREEMENT ras_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, j, wlen;
	unsigned char	*bits, *pbuf = 0;
	SUNHeaderinfo	header;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&ras_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_ras_is_valid_img(pinfo_str) != 0)
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

			/* 填写头结构 */
			header.ras_magic     = RAS_MAGIC;
			header.ras_width     = (int)pinfo_str->width;
			header.ras_height    = (int)pinfo_str->height;
			header.ras_depth     = (int)pinfo_str->bitcount;
			header.ras_length    = (header.ras_width*3+((header.ras_width*3)%2))*header.ras_height;
			header.ras_type      = RT_STANDARD;
			header.ras_maptype   = RMT_NONE;
			header.ras_maplength = 0;

			/* 转至68K字序 */
			header.ras_magic     = EXCHANGE_DWORD((header.ras_magic));
			header.ras_width     = EXCHANGE_DWORD((header.ras_width));
			header.ras_height    = EXCHANGE_DWORD((header.ras_height));
			header.ras_depth     = EXCHANGE_DWORD((header.ras_depth));
			header.ras_length    = EXCHANGE_DWORD((header.ras_length));
			header.ras_type      = EXCHANGE_DWORD((header.ras_type));
			header.ras_maptype   = EXCHANGE_DWORD((header.ras_maptype));
			header.ras_maplength = EXCHANGE_DWORD((header.ras_maplength));
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入头结构到目标流 */
			if (isio_write((const void*)&header, sizeof(SUNHeaderinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			wlen = pinfo_str->width*3+((pinfo_str->width*3)%2);
			
			/* 申请写缓冲区（扫描行）*/
			if ((pbuf=(unsigned char *)malloc(wlen+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 写入图像数据（传入的图像数据必须是ISee位流格式，否则可能会产生失真）*/
			for (i=0; i<(int)(pinfo_str->height); i++)
			{
				bits = (unsigned char *)pinfo_str->pp_line_addr[i];

				/* 颠倒R、B分量。注：文档描述与实测图象格式不符，暂以ACDSee显示效果为准 */
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j*3+0] = bits[j*3+2];
					pbuf[j*3+1] = bits[j*3+1];
					pbuf[j*3+2] = bits[j*3+0];
				}

				/* 写入目标流 */
				if (isio_write((const void*)pbuf, wlen, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i;
				
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
			if (pbuf)
				free(pbuf);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&ras_save_img_critical);
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
int CALLAGREEMENT _ras_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(ras_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (ras_irwp_info.irwp_save.img_num)
	{
		if (ras_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* 图像个数不正确 */
			}
		}
	}

	return 0;
}


/* 读取指定数目的象素数据至缓冲区*/
static enum EXERESULT CALLAGREEMENT _read_data(ISFILE *pfile, 
											   unsigned char *buf, 
											   long length, 
											   int rle, 
											   int init)
{
	static unsigned char repchar, remaining;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	/* 初始化变量值 */
	if (init)
	{
		repchar = remaining = 0;	/* 静态变量，重复调用时需要适时的初始化 */

		return b_status;
	}

	__try
	{
		__try
		{
			/* 如果是RLE压缩数据，则先解码 */
			if (rle)
			{
				while(length--)
				{
					if (remaining)
					{
						remaining--;
						*(buf++)= repchar;
					}
					else
					{
						/* 读取标识字节 */
						if (isio_read((void*)&repchar, 1, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						/* 判断是否是有压缩可能的字段 */
						if (repchar == RESC)
						{
							if (isio_read((void*)&remaining, 1, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;
							}
							/* 压缩内容为1字节的0x80 */
							if (remaining == 0)
							{
								*(buf++)= RESC;
							}
							else	/* 重复内容 */
							{
								/* 提取重复字节 */
								if (isio_read((void*)&repchar, 1, 1, pfile) == 0)
								{
									b_status = ER_FILERWERR; __leave;
								}
								
								*(buf++)= repchar;
							}
						}
						else	/* 未压缩单字节内容 */
						{
							*(buf++)= repchar;
						}
					}
				}
			}
			else	/* 未压缩图象 */
			{
				if (isio_read((void*)buf, length, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
		}
		__finally
		{
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_FILERWERR;
	}

	return b_status;
}
