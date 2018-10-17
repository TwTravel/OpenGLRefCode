/********************************************************************

	tga.c

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
	本文件用途：	ISee图像浏览器—TGA图像读写模块实现文件

					读取功能：未压缩的8、15、16、24、32位图像，及
							采用RLE压缩的8、15、16、24、32位图像
					保存功能：未压缩的8、15、16、24、32位图像

	本文件编写人：	晓月儿			xiaoyueer##263.net
					YZ				yzfree##sina.com

	本文件版本：	11225
	最后修改于：	2001-12-25

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2001-12		发布新版代码（加快了图像读取速度，并增加
					了保存图像的功能）

		2001-3		为提高模块读图像速度而修改代码
		2000-8		第一个测试版发布

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

#include "tga.h"


IRWP_INFO			tga_irwp_info;			/* 插件信息表 */
unsigned char		*tga_save_idinfo = "ISeeExplorer";
											/* 保存图像时注入的标志性信息 */

#ifdef WIN32
CRITICAL_SECTION	tga_get_info_critical;	/* tga_get_image_info函数的关键段 */
CRITICAL_SECTION	tga_load_img_critical;	/* tga_load_image函数的关键段 */
CRITICAL_SECTION	tga_save_img_critical;	/* tga_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);

int	CALLAGREEMENT _decoding_line(unsigned char *pdest,unsigned char *psrc,int rleunit,int destlen);
int CALLAGREEMENT _get_mask(LPTGAHEADER pInfo, unsigned long * pb_mask, unsigned long * pg_mask, unsigned long * pr_mask, unsigned long * pa_mask);
int CALLAGREEMENT _calcu_scanline_size(int w, int bit);
int CALLAGREEMENT _tga_is_valid_img(LPINFOSTR pinfo_str);
							

#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&tga_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&tga_get_info_critical);
			InitializeCriticalSection(&tga_load_img_critical);
			InitializeCriticalSection(&tga_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&tga_get_info_critical);
			DeleteCriticalSection(&tga_load_img_critical);
			DeleteCriticalSection(&tga_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

TGA_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&tga_irwp_info;
}

#else

TGA_API LPIRWP_INFO CALLAGREEMENT tga_get_plugin_info()
{
	_init_irwp_info(&tga_irwp_info);

	return (LPIRWP_INFO)&tga_irwp_info;
}

TGA_API void CALLAGREEMENT tga_init_plugin()
{
	/* 初始化多线程同步对象 */
}

TGA_API void CALLAGREEMENT tga_detach_plugin()
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
	/* ## 各位数减一，否则32位格式无法表示。*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(8-1)) | \
						(1UL<<(16-1)) | (1UL<<(24-1)) | (1UL<<(32-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1－只能保存一副图像 */
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 2;


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"晓月儿");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"xiaoyueer##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"我爱你，我一直在努力！");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"我主要负责改版工作，跟着晓月儿后面跑两趟:)");
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
TGA_API int CALLAGREEMENT tga_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	TGAHEADER		tgaHeader;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&tga_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}

			/* 获取流长度 */
			stream_length = isio_length(pfile);
			
			/* 读取文件头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&tgaHeader, sizeof(TGAHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断文件头的图象类型标志 */
			switch(tgaHeader.bImageType)
			{
			case	TGA_RLEPAL:
			case	TGA_RLECOLOR:
			case	TGA_RLEGRAY:
				/* 有效的图像流长度应大于头结构尺寸（粗略判断） */
				if (stream_length < sizeof(TGAHEADER))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				break;		
			case	TGA_UNCPSPAL:
			case	TGA_UNCPSCOLOR:
			case	TGA_UNCPSGRAY:
				/* 未压缩图像可计算出图像数据的尺寸，所以判断的能更精确一些 */
				if (stream_length < (sizeof(TGAHEADER)+tgaHeader.bID_Length+((tgaHeader.bBits+7)>>3)*tgaHeader.wDepth*tgaHeader.wWidth+tgaHeader.wPalLength*((tgaHeader.bPalBits+7)>>3)))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				break;
			case	TGA_NULL:	/* 未包含图像数据的文件将视为无效图像 */
			default:			/* 未知文件类型 */
				b_status = ER_NONIMAGE; __leave;
				break;
			}

			/* 检验象素位数是否合法 */
			if ((tgaHeader.bBits != 8)&&(tgaHeader.bBits != 15)&& \
				(tgaHeader.bBits != 16)&&(tgaHeader.bBits != 24)&&\
				(tgaHeader.bBits != 32))
			{
				b_status = ER_BADIMAGE; __leave;
			}
			

			/* 填写图像信息结构 */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_TGA;			/* 图像文件格式（后缀名） */
			
			switch(tgaHeader.bImageType)				/* 图像的压缩方式 */
			{
			case	TGA_UNCPSPAL:
			case	TGA_UNCPSCOLOR:
			case	TGA_UNCPSGRAY:
				pinfo_str->compression = ICS_RGB; break;
			case	TGA_RLEPAL:
			case	TGA_RLECOLOR:
			case	TGA_RLEGRAY:
				switch(tgaHeader.bBits)
				{
				case	8:
					pinfo_str->compression = ICS_RLE8;  break;
				case	15:
				case	16:
					pinfo_str->compression = ICS_RLE16;	break;
				case	24:
					pinfo_str->compression = ICS_RLE24;	break;
				case	32:
					pinfo_str->compression = ICS_RLE32;	break;
				}
				break;
			default:
				assert(0);		/* 系统崩溃 */
				b_status = ER_SYSERR; 
				__leave;
				break;
			}
			
			
			/* 填写图像信息 */
			pinfo_str->width	= (unsigned long)tgaHeader.wWidth;
			pinfo_str->height	= (unsigned long)tgaHeader.wDepth;
			pinfo_str->order	= (tgaHeader.bDescriptor&TGA_VERTMIRROR) ? 0 : 1;
			pinfo_str->bitcount	= (unsigned long)(tgaHeader.bBits == 15) ? 16:tgaHeader.bBits;
			
			if (_get_mask(&tgaHeader, 
				(unsigned long *)(&pinfo_str->b_mask), 
				(unsigned long *)(&pinfo_str->g_mask), 
				(unsigned long *)(&pinfo_str->r_mask),
				(unsigned long *)(&pinfo_str->a_mask)) == 1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&tga_get_info_critical);
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
TGA_API int CALLAGREEMENT tga_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	TGAHEADER		tgaHeader;
	
	int				i, k, j, linesize, decoded;
	int				pallen = 0, firstindex = 0, newlinepos = 0, ile = 0;
	unsigned short	palentry;
	unsigned char	*pbuf = 0;
	unsigned char	*pbits;

	unsigned char	bpix;
	unsigned short	wpix;
	unsigned long	lpix;

	static unsigned char tmp_pal[1024];	/* 临时调色板缓冲区 */

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&tga_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 初始化步进值 */
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;

			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 获取流长度 */
			stream_length = isio_length(pfile);
			
			/* 读文件头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&tgaHeader, sizeof(TGAHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 判断文件头的图象类型标志 */
				switch(tgaHeader.bImageType)
				{
				case	TGA_RLEPAL:
				case	TGA_RLECOLOR:
				case	TGA_RLEGRAY:
					/* 有效的图像流长度应大于头结构尺寸（粗略判断） */
					if (stream_length < sizeof(TGAHEADER))
					{
						b_status = ER_NONIMAGE; __leave;
					}
					break;		
				case	TGA_UNCPSPAL:
				case	TGA_UNCPSCOLOR:
				case	TGA_UNCPSGRAY:
					/* 未压缩图像可计算出图像数据的尺寸，所以判断的能更精确一些 */
					if (stream_length < (sizeof(TGAHEADER)+tgaHeader.bID_Length+((tgaHeader.bBits+7)>>3)*tgaHeader.wDepth*tgaHeader.wWidth+tgaHeader.wPalLength*((tgaHeader.bPalBits+7)>>3)))
					{
						b_status = ER_NONIMAGE; __leave;
					}
					break;
				case	TGA_NULL:	/* 未包含图像数据的文件将视为无效图像 */
				default:			/* 未知文件类型 */
					b_status = ER_NONIMAGE; __leave;
					break;
				}
				
				/* 检验象素位数是否合法 */
				if ((tgaHeader.bBits != 8)&&(tgaHeader.bBits != 15)&& \
					(tgaHeader.bBits != 16)&&(tgaHeader.bBits != 24)&&\
					(tgaHeader.bBits != 32))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				
				/* 填写图像信息结构 */
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_TGA;			/* 图像文件格式（后缀名） */
				
				switch(tgaHeader.bImageType)				/* 图像的压缩方式 */
				{
				case	TGA_UNCPSPAL:
				case	TGA_UNCPSCOLOR:
				case	TGA_UNCPSGRAY:
					pinfo_str->compression = ICS_RGB; break;
				case	TGA_RLEPAL:
				case	TGA_RLECOLOR:
				case	TGA_RLEGRAY:
					switch(tgaHeader.bBits)
					{
					case	8:
						pinfo_str->compression = ICS_RLE8;  break;
					case	15:
					case	16:
						pinfo_str->compression = ICS_RLE16;	break;
					case	24:
						pinfo_str->compression = ICS_RLE24;	break;
					case	32:
						pinfo_str->compression = ICS_RLE32;	break;
					}
					break;
				default:
					assert(0);		/* 系统崩溃 */
					b_status = ER_SYSERR; 
					__leave;
					break;
				}
				
				
				/* 填写图像信息 */
				pinfo_str->width	= (unsigned long)tgaHeader.wWidth;
				pinfo_str->height	= (unsigned long)tgaHeader.wDepth;
				pinfo_str->order	= (tgaHeader.bDescriptor&TGA_VERTMIRROR) ? 0 : 1;
				pinfo_str->bitcount	= (unsigned long)(tgaHeader.bBits == 15) ? 16:tgaHeader.bBits;
				
				/* 填写掩码信息 */
				if (_get_mask(&tgaHeader, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == 1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}


			/* 设置调色板数据 */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

						/* 生成灰度图调色板 */
				if ((tgaHeader.bImageType == TGA_UNCPSGRAY)||(tgaHeader.bImageType == TGA_RLEGRAY))
				{
					for (i=0;i<(int)pinfo_str->pal_count;i++)
						pinfo_str->palette[i] = (i<<16)|(i<<8)|(i);
				}
				else	/* 拷贝并转换调色板数据从文件 */
				{
					if (isio_seek(pfile, sizeof(TGAHEADER)+tgaHeader.bID_Length, SEEK_SET) == -1)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* TGA调色板项并不是普通的ARGB格式，所以需要下面的判断过程 */
					pallen = tgaHeader.wPalLength * ((tgaHeader.bPalBits+7) >> 3);
					firstindex = tgaHeader.wPalFirstNdx;

					if (isio_read((void*)tmp_pal, pallen, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* 转换调色板数据 */
					switch(tgaHeader.bPalBits)
					{
					case	15:
					case	16:		/* 转换2字节调色板数据 */
						for (i=0;i<pallen;i+=2)
						{
							if (firstindex >= 256)
							{
								b_status = ER_BADIMAGE; __leave;
							}

							palentry = *((unsigned short*)(tmp_pal+i));
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->blue  = (palentry&0x1f)<<3;
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->green = ((palentry>>5)&0x1f)<<3;
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->red   = ((palentry>>10)&0x1f)<<3;
							pinfo_str->palette[firstindex++] &= 0xffffff;	/* 最高字节置零 */

						}	
						break;
					case	24:		/* 转换3字节调色板数据 */
						for (i=0;i<pallen;i+=3)
						{
							if (firstindex >= 256)
							{
								b_status = ER_BADIMAGE; __leave;
							}

							((LPPALITEM)(&pinfo_str->palette[firstindex]))->blue  = tmp_pal[i];
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->green = tmp_pal[i+1];
							((LPPALITEM)(&pinfo_str->palette[firstindex]))->red   = tmp_pal[i+2];
							pinfo_str->palette[firstindex++] &= 0xffffff;
						}
						break;
					case	32:		/* 转换4字节调色板数据 */
						for (i=0;i<pallen;i+=4)
						{
							if (firstindex >= 256)
							{
								b_status = ER_BADIMAGE; __leave;
							}

							pinfo_str->palette[firstindex] = *(unsigned long *)(tmp_pal+i);
							pinfo_str->palette[firstindex++] &= 0xffffff;
						}
						break;
					default:
						assert(0);		/* 系统崩溃 */
						b_status = ER_SYSERR; 
						__leave;
						break;
					}	
				}
			}
			else
			{
				pinfo_str->pal_count = 0;	/* 无调色板数据 */
			}
				
			/* 取得扫描行尺寸 */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
			
			/* 设置图像个数 */
			pinfo_str->imgnumbers = 1;		/* TGA文件中只有一个图像，*/
											/* 应当还有原始图象缩图(Postage Stamp)，*/
											/* 但晓月儿还未遇见实例	*/
			pinfo_str->psubimg	= (LPSUBIMGBLOCK)0;		/* 子图像链置空 */
			
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
			
			/* 分配图像扫描行缓冲区 */
			pbuf = (unsigned char *)malloc(pinfo_str->width*((pinfo_str->bitcount+7)>>3)+pinfo_str->width);	/* 最坏的情况是RLE压缩中没有一个重复字组 */
			if (!pbuf)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 初始化行首地址数组 */
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
			
			/* 读取图像数据 */
			/*     根据图像文件的位深度的不同而采用不同的读取代码 */
			switch(tgaHeader.bImageType)
			{
			case	TGA_UNCPSPAL:
			case	TGA_UNCPSGRAY:
			case	TGA_UNCPSCOLOR:
				/* 下一行图像数据在文件中的起始位置 */
				newlinepos = sizeof(TGAHEADER)+tgaHeader.bID_Length+pallen;
				
				if (isio_seek(pfile, newlinepos, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 文件扫描行长度 */
				ile = pinfo_str->width*((pinfo_str->bitcount+7)>>3);
				
				pbits = pinfo_str->p_bit_data;

				/* 以行为单位读取图像数据 */
				if (tgaHeader.bDescriptor&TGA_HORZMIRROR)	/* 左右反置 */
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_read((void*)pbuf, ile, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						for (k=0;k<(int)pinfo_str->width;k++)
						{
							switch (pinfo_str->bitcount)
							{
							case	8:
								((unsigned char *)pbits)[k] = pbuf[pinfo_str->width-1-k];
								break;
							case	15:
							case	16:
								((unsigned short *)pbits)[k] = ((unsigned short *)pbuf)[pinfo_str->width-1-k];
								break;
							case	24:
								for (j=0;j<3;j++)
									((unsigned char *)pbits)[k*3+j] = pbuf[(pinfo_str->width-1-k)*3+j];
								break;
							case	32:
								((unsigned long *)pbits)[k] = ((unsigned long *)pbuf)[pinfo_str->width-1-k];
								break;
							default:
								assert(0);	/* 系统崩溃 */
								b_status = ER_SYSERR; 
								__leave;
								break;								
							}
						}
						
						/* 步进值累加 */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_read((void*)pbits, ile, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						/* 步进值累加 */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				break;
			case	TGA_RLEGRAY:
			case	TGA_RLEPAL:
			case	TGA_RLECOLOR:
				/* 下一行图像数据在文件中的起始位置 */
				newlinepos = sizeof(TGAHEADER)+tgaHeader.bID_Length+pallen;

				/* 文件扫描行长度（以最坏的情况计算） */
				ile = pinfo_str->width*((pinfo_str->bitcount+7)>>3)+pinfo_str->width;
				
				pbits = pinfo_str->p_bit_data;

				/* 以行为单位读取图像数据 */
				if (tgaHeader.bDescriptor&TGA_HORZMIRROR)	/* 左右反置 */
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_seek(pfile, newlinepos, SEEK_SET) == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						if (isio_read((void*)pbuf, 1, ile, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						
						/* 解压一RLE图像行 */
						decoded = _decoding_line(pbits, pbuf, (pinfo_str->bitcount+7)>>3, pinfo_str->width*((pinfo_str->bitcount+7)>>3));
						
						if (decoded == -1)
						{
							b_status = ER_BADIMAGE; __leave;
						}
						
						assert(decoded);
						
						/* 下一行图像数据在文件中的起始位置 */
						newlinepos += decoded;

						/* 左右颠倒图像数据 */
						for (k=0;k<(int)pinfo_str->width;k++)
						{
							switch (pinfo_str->bitcount)
							{
							case	8:
								bpix = ((unsigned char *)pbits)[k];
								((unsigned char *)pbits)[k] = ((unsigned char *)pbits)[pinfo_str->width-1-k];
								((unsigned char *)pbits)[pinfo_str->width-1-k] = bpix;
								break;
							case	15:
							case	16:
								wpix = ((unsigned short *)pbits)[k];
								((unsigned short *)pbits)[k] = ((unsigned short *)pbits)[pinfo_str->width-1-k];
								((unsigned short *)pbits)[pinfo_str->width-1-k] = wpix;
								break;
							case	24:
								for (j=0;j<3;j++)
								{
									bpix = ((unsigned char *)pbits)[k*3+j];
									((unsigned char *)pbits)[k*3+j] = ((unsigned char *)pbits)[(pinfo_str->width-1-k)*3+j];
									((unsigned char *)pbits)[(pinfo_str->width-1-k)*3+j] = bpix;
								}
								break;
							case	32:
								lpix = ((unsigned long *)pbits)[k];
								((unsigned long *)pbits)[k] = ((unsigned long *)pbits)[pinfo_str->width-1-k];
								((unsigned long *)pbits)[pinfo_str->width-1-k] = lpix;
								break;
							default:
								assert(0);	/* 系统崩溃 */
								b_status = ER_SYSERR; 
								__leave;
								break;								
							}
						}
						
						/* 步进值累加 */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else
				{
					for (i=0;i<(int)pinfo_str->height;i++)
					{
						if (isio_seek(pfile, newlinepos, SEEK_SET) == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}

						if (isio_read((void*)pbuf, 1, ile, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}

						/* 解压一RLE图像行 */
						decoded = _decoding_line(pbits, pbuf, (pinfo_str->bitcount+7)>>3, pinfo_str->width*((pinfo_str->bitcount+7)>>3));
						
						if (decoded == -1)
						{
							b_status = ER_BADIMAGE; __leave;
						}

						assert(decoded);

						/* 下一行图像数据在文件中的起始位置 */
						newlinepos += decoded;
						
						/* 步进值累加 */
						pinfo_str->process_current = i+1;
						pbits = (unsigned char *)(pbits + linesize);
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				break;
			default:
				assert(0);	/* 系统崩溃 */
				b_status = ER_SYSERR; 
				__leave;
				break;
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

			if (pbuf)
				free(pbuf);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&tga_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
TGA_API int CALLAGREEMENT tga_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	TGAHEADER		tgaHeader;
	TGAFOOTER		tgaFooter;
	int				i, wrlinesize;
	unsigned char   *pbits;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&tga_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_tga_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}


			/* 附加信息长度，此处将增加一个"ISeeExplorer"字符串 */
			tgaHeader.bID_Length   = (unsigned char)strlen((const char *)tga_save_idinfo);
			/* 调色板信息（灰度图在这里也将被看作调色板图） */
			tgaHeader.bPalType     = (unsigned char) (pinfo_str->bitcount == 8) ? 1 : 0;
			/* 图象类型(只有1，2两种类型) */
			tgaHeader.bImageType   = (unsigned char) (pinfo_str->bitcount == 8) ? TGA_UNCPSPAL : TGA_UNCPSCOLOR;
			/* 调色板第一个索引值 */
			tgaHeader.wPalFirstNdx = 0;
			/* 调色板索引数(以调色板单元为单位) */
			tgaHeader.wPalLength   = (unsigned short)(pinfo_str->bitcount == 8) ? 256 : 0;
			/* 一个调色板单位位数(ISee调色板项是32位的) */
			tgaHeader.bPalBits     = 32;
			/* 图象左下端坐标 */
			tgaHeader.wLeft        = 0;
			tgaHeader.wBottom      = 0;
			/* 图象宽、高度 */
			tgaHeader.wWidth       = (unsigned short)pinfo_str->width;
			tgaHeader.wDepth       = (unsigned short)pinfo_str->height;
			/* 象素位数 */
			tgaHeader.bBits        = (unsigned char)pinfo_str->bitcount;
			/* 附加特性描述(不包含A通道，一律为倒向图) */
			tgaHeader.bDescriptor  = 0;

			
			/* 设置进度值 */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入文件头结构 */
			if (isio_write((const void *)&tgaHeader, sizeof(TGAHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 写入信息 */
			if (isio_write((const void *)tga_save_idinfo, strlen(tga_save_idinfo), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入调色板数据 */
			if (tgaHeader.bPalType == 1)
			{
				if (isio_write((const void *)pinfo_str->palette, pinfo_str->pal_count*4, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			/* TGA未压缩扫描行尺寸并不是4字节对齐的，所以需要按实际象素计算行尺寸 */
			wrlinesize = pinfo_str->width * ((pinfo_str->bitcount+7)>>3);

			/* 写入图像位数据 
				注：传入的图像数据必须是ISee位流格式，否则可能会产生失真
			*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				/* 一律保存为倒向图 */
				pbits = (unsigned char *)(pinfo_str->pp_line_addr[pinfo_str->height-1-i]);

				if (isio_write((const void*)pbits, wrlinesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}

			/* 填写文件尾部信息（新版TGA格式）*/
			tgaFooter.eao = 0;
			tgaFooter.ddo = 0;
			memcpy((void*)tgaFooter.info, (const void *)"TRUEVISION-XFILE", 16);
			tgaFooter.period = '.';
			tgaFooter.zero = 0;

			if (isio_write((const void*)&tgaFooter, sizeof(TGAFOOTER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&tga_save_img_critical);
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
int CALLAGREEMENT _get_mask(LPTGAHEADER pInfo, 
	unsigned long * pb_mask, 
	unsigned long * pg_mask, 
	unsigned long * pr_mask, 
	unsigned long * pa_mask
	)
{
	int result = 0;

	assert(pInfo->bBits);

	switch(pInfo->bBits)
	{
	case	8:
		/* 调色板位图无掩码 */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	15:
		/*******************************************
				15、16位掩码示意图 - TGA

			  高字节      低字节
			0000 0000   0000 0000 
						   1 1111--B	15位格式
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	16位格式
				   11   111 -------G
			 111 11  --------------R
			1  --------------------A
		*******************************************/
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	16:
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0x8000;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:
		/******************************************************
						32位掩码示意图 - TGA
			    高               ->                 低
		0000 0000   0000 0000   0000 0000   0000 0000 	888格式
		1111 1111  ------------------------------------A
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		******************************************************/
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	default:
		result = 1;

		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}

	return result;	/* 0-成功， 1－非法的图像格式 */
}


/* 计算扫描行尺寸(四字节对齐) */
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */)
{
	return DIBSCANLINE_WIDTHBYTES(w*bit);
}


/* 行解压，返回psrc实际解压字节数，如源数据有误，返回-1 */
int	CALLAGREEMENT _decoding_line(
		unsigned char *pdest,			/* 目标缓冲区 */
		unsigned char * psrc,			/* RLE源数据  */
		int rleunit,					/* rleunit为解压单位(in Bytes) */
		int destlen						/* destlen为解压目标长度 */
		)
{
	int i, j, nCount;
	unsigned char *pdestEnd = pdest+destlen, *psrcOrg=psrc, *pdestOrg=pdest;
	unsigned char *ptmp;

	assert(psrc);
	assert(pdest);
	
	while(pdest<pdestEnd)
	{
		if(psrc[0]&0x80)						/* 重复字组 */
		{
			nCount=(psrc[0]&0x7F)+1;
			psrc++;
			for(i=0;i<nCount;i++)
			{
				ptmp = psrc;
				for (j=0;j<rleunit;j++)			/* 解压一个象素(length equ rleunit) */
					*pdest++ = *ptmp++;
			}
			psrc+=rleunit;
		}
		else									/* 非重复字组 */
		{
			nCount=(psrc[0]&0x7F)+1;
			psrc++;
			memcpy(pdest,psrc,nCount*rleunit);	/* 非重复字组的尺寸可能会比较大，所以memcpy的效率可能更高 */
			pdest+=nCount*rleunit;
			psrc+=nCount*rleunit;
		}
	}

	if(pdest != pdestEnd)						/* 图像数据非法，返回 -1 */
		return -1;

	return psrc-psrcOrg;						/* 解压成功，返回被解压字节数 */
}


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _tga_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* ## 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(tga_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */
	
	assert(pinfo_str->imgnumbers);
	
	if (tga_irwp_info.irwp_save.img_num)
		if (tga_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */
			
	return 0;
}
