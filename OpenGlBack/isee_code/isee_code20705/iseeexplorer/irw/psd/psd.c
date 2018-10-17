/********************************************************************

	psd.c

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
	本文件用途：	ISee图像浏览器—PSD图像读写模块实现文件
	
					读取功能：
								BITMAP色,1位,RLE压缩与无压缩; 
								RGB色,8位,RLE压缩与无压缩; 
								灰度,8位,RLE压缩与无压缩; 
								Duotone, 8位,RLE压缩与无压缩;
								CMYK色,8位,RLE压缩与无压缩; 
								Lab色,8位,RLE压缩与无压缩;
								* 不支持16位/通道，不支持多通道图象

					保存功能：不支持
	  
	本文件编写人：	Janhail		janhail##sina.com
					YZ			yzfree##yeah.net
		
	本文件版本：	20423
	最后修改于：	2002-4-23
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：

		2002-4		发布新版代码。支持ISeeIO，改变读取方法减少内存占用
					量，增加对单色图RLE压缩法支持，改进CMYK->RGB算法。
					去除对16位/通道图象的支持，去除对多通道图象的支持。
		
		2001-1		修正了一些BUG
		2000-10		第一个测试版发布

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

#include "psd.h"


IRWP_INFO			psd_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	psd_get_info_critical;	/* psd_get_image_info函数的关键段 */
CRITICAL_SECTION	psd_load_img_critical;	/* psd_load_image函数的关键段 */
CRITICAL_SECTION	psd_save_img_critical;	/* psd_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);

static int  CALLAGREEMENT _is_support_format( unsigned short, unsigned short );
static int	CALLAGREEMENT _skip_block( ISFILE *pfile );
static int  CALLAGREEMENT _get_rle_header(ISFILE *pfile, LPPSDFILEHEADER psdheader, unsigned short **prle_header);
static int  CALLAGREEMENT _read_line(ISFILE *pfile, long pos, unsigned short size, signed char *pdata);
static int  CALLAGREEMENT _decode_rle_line(signed char *psou, long ssize, unsigned char *pdes, long dsize);

static enum EXERESULT CALLAGREEMENT read_psd(ISFILE *pfile, LPINFOSTR pInfo, LPPSDFILEHEADER psdheader);
static enum EXERESULT CALLAGREEMENT _read_bitmap(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_grayscale_index_duotone(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_rgb(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_cmyk(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);
static enum EXERESULT CALLAGREEMENT _read_lab(ISFILE *, LPINFOSTR, LPPSDFILEHEADER, unsigned long);


										  

#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, unsigned long ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&psd_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&psd_get_info_critical);
			InitializeCriticalSection(&psd_load_img_critical);
			InitializeCriticalSection(&psd_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&psd_get_info_critical);
			DeleteCriticalSection(&psd_load_img_critical);
			DeleteCriticalSection(&psd_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PSD_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&psd_irwp_info;
}

#else

PSD_API LPIRWP_INFO CALLAGREEMENT psd_get_plugin_info()
{
	_init_irwp_info(&psd_irwp_info);

	return (LPIRWP_INFO)&psd_irwp_info;
}

PSD_API void CALLAGREEMENT psd_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PSD_API void CALLAGREEMENT psd_detach_plugin()
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

	/* 设置模块保存代码支持的位深 */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 2;


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"Janhail");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"janhail##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"请不要写作者留言!! ^_^");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"改版把我累得够呛:)");
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
PSD_API int CALLAGREEMENT psd_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	PSDFILEHEADER	psd_header;
	unsigned short	compression;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&psd_get_info_critical);

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

			if (isio_read((void*)&psd_header, sizeof(PSDFILEHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断文件头部的PSD文件标志('8BPS')和Version标志(1)
			   PSD 的 Version 一定是 1，如果是其他的就错了 */
			if ( (psd_header.Signature != PSD_SIGNATURE_MARKER) ||\
				(psd_header.Version != PSD_VERSION_MARKER) )
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			psd_header.Channels	= EXCHANGE_WORD( psd_header.Channels ) ;
			psd_header.Rows		= EXCHANGE_DWORD( psd_header.Rows ) ;
			psd_header.Columns	= EXCHANGE_DWORD( psd_header.Columns ) ;
			psd_header.Depth	= EXCHANGE_WORD( psd_header.Depth ) ;
			psd_header.Mode		= EXCHANGE_WORD( psd_header.Mode ) ;
			
			/* 通道数必须在 1 到 24 之间，长宽的范围在 1 到 30000 之间 */
			if( psd_header.Channels<1 || psd_header.Channels>24 \
				|| psd_header.Rows<1 || psd_header.Rows>30000 \
				|| psd_header.Columns<1 || psd_header.Columns>30000 )
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* 判断文件格式（位深度）是否支持 */
			if( _is_support_format( psd_header.Mode, psd_header.Depth ) )
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_PSD;			/* 图像文件格式（后缀名） */

			/* 填写图像信息 */
			pinfo_str->width	= psd_header.Columns ;
			pinfo_str->height	= psd_header.Rows ;
			pinfo_str->order	= 0 ;

			/* 检查各色系的通道数是否被ISee插件接受 */
			switch( psd_header.Mode )
			{
			case 0:	/* Bitmap */			/* 这三种色系只允许有一个通道 */
			case 1:	/* Grayscale */
			case 2:	/* Indexed */
			case 8:	/* Duotone (按灰度读取) */
				if (psd_header.Channels != 1)
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth;
				break;
			case 3:	/* RGB */				/* RGB色系通常为3通道，允许附加一个ALPHA通道 */
				if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
				break;
			case 4:	/* CMYK */				/* CMYK色系不允许再附加ALPHA通道 */
				if (psd_header.Channels != 4)
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth * 3;
				break;
			case 7:	/* Multichannel */		/* 暂不支持多通道图象 */
				b_status = ER_BADIMAGE; __leave; break;
			case 9:	/* Lab */
				if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
				{
					b_status = ER_BADIMAGE; __leave; break;
				}
				pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
				break;
			default:/* Error */
				assert(0); b_status = ER_SYSERR; __leave; break;
			}
			

			/* 提取图像的压缩方式 */

			/* 跳过调色板段(只有索引图和双色调图（duotone）才有调色板信息) */
			if (_skip_block( pfile ))
			{
				b_status = ER_FILERWERR; __leave;
			}
			/* 跳过图象资源段 */
			if (_skip_block( pfile ))
			{
				b_status = ER_FILERWERR; __leave;
			}
			/* 跳过图象层和蒙板段 */
			if (_skip_block( pfile ))
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 提取压缩方法信息 */
			if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			compression = EXCHANGE_WORD( compression ) ;

			switch (compression)
			{
			case	1:
				pinfo_str->compression = ICS_RLE8 ;
				break;
			case	0:
				/* 这里也有分 888 的RGB 和只有8位的灰度等 */
				if( psd_header.Depth == 1 )
					pinfo_str->compression = ICS_RGB;			/* 简单了一些，做不得准的 */
				else
					pinfo_str->compression = ICS_BITFIELDS ;	/* 分位的 */
				break;
			default:
				b_status = ER_BADIMAGE; __leave; break;			/* 插件暂不支持ZIP压缩 */
			}


			/* 计算图像掩码数据 */
			switch (pinfo_str->bitcount)
			{
			case	1:
			case	8:
				pinfo_str->r_mask = 0;
				pinfo_str->g_mask = 0;
				pinfo_str->b_mask = 0;
				pinfo_str->a_mask = 0;
				break;
			case	24:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0;
				break;
			case	32:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0xff000000;
				break;
			}

			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&psd_get_info_critical);
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
PSD_API int CALLAGREEMENT psd_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PSDFILEHEADER	psd_header;

	unsigned short	compression;
	int				linesize, i;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&psd_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 读文件头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			if (isio_read((void*)&psd_header, sizeof(PSDFILEHEADER), 1, pfile) == 0)
			{
				b_status = (pinfo_str->data_state == 1) ? ER_FILERWERR:ER_NONIMAGE; 
				__leave;
			}
			
			/* 判断文件头部的PSD文件标志('8BPS')和Version标志(1)
			   PSD 的 Version 一定是 1，如果是其他的就错了 */
			if ( (psd_header.Signature != PSD_SIGNATURE_MARKER) ||\
				(psd_header.Version != PSD_VERSION_MARKER) )
			{
				b_status = ER_NONIMAGE; __leave;
			}

			psd_header.Channels	= EXCHANGE_WORD( psd_header.Channels ) ;
			psd_header.Rows		= EXCHANGE_DWORD( psd_header.Rows ) ;
			psd_header.Columns	= EXCHANGE_DWORD( psd_header.Columns ) ;
			psd_header.Depth	= EXCHANGE_WORD( psd_header.Depth ) ;
			psd_header.Mode		= EXCHANGE_WORD( psd_header.Mode ) ;
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 通道数必须在 1 到 24 之间
				   长宽的范围在 1 到 30000 之间 */
				if( psd_header.Channels<1 || psd_header.Channels>24 \
					|| psd_header.Rows<1 || psd_header.Rows>30000 \
					|| psd_header.Columns<1 || psd_header.Columns>30000 )
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				if( _is_support_format( psd_header.Mode, psd_header.Depth ) )
				{	/* 文件的格式不支持(也包括是PSD但不支持的文件) */
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_PSD;			/* 图像文件格式（后缀名） */

				/* 填写图像信息 */
				pinfo_str->width	= psd_header.Columns ;
				pinfo_str->height	= psd_header.Rows ;
				pinfo_str->order	= 0 ;

				/* 检查各色系的通道数是否被ISee插件接受 */
				switch( psd_header.Mode )
				{
				case 0:	/* Bitmap */			/* 这三种色系只允许有一个通道 */
				case 1:	/* Grayscale */
				case 2:	/* Indexed */
				case 8:	/* Duotone (按灰度读取) */
					if (psd_header.Channels != 1)
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth;
					break;
				case 3:	/* RGB */				/* RGB色系通常为3通道，允许附加一个ALPHA通道 */
					if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
					break;
				case 4:	/* CMYK */				/* CMYK色系不允许再附加ALPHA通道 */
					if (psd_header.Channels != 4)
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth * 3;
					break;
				case 7:	/* Multichannel */		/* 暂不支持多通道图象 */
					b_status = ER_BADIMAGE; __leave; break;
				case 9:	/* Lab */
					if (( psd_header.Channels != 3)&&(psd_header.Channels != 4))
					{
						b_status = ER_BADIMAGE; __leave; break;
					}
					pinfo_str->bitcount	= psd_header.Depth * psd_header.Channels;
					break;
				default:/* Error */
					assert(0); b_status = ER_SYSERR; __leave; break;
				}
				

				/* 提取图像的压缩方式 */

				/* 跳过调色板段、图象资源段、图象层和蒙板段 */
				if (_skip_block( pfile )||_skip_block( pfile )||_skip_block( pfile ))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 提取压缩方法信息 */
				if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				compression = EXCHANGE_WORD( compression ) ;

				switch (compression)
				{
				case	1:
					pinfo_str->compression = ICS_RLE8 ;
					break;
				case	0:
					/* 这里也有分 888 的RGB 和只有8位的灰度等 */
					if( psd_header.Depth == 1 )
						pinfo_str->compression = ICS_RGB;			/* 简单了一些，做不得准的 */
					else
						pinfo_str->compression = ICS_BITFIELDS ;	/* 分位的 */
					break;
				default:
					b_status = ER_BADIMAGE; __leave; break;			/* 插件暂不支持ZIP压缩 */
				}


				/* 计算图像掩码数据 */
				switch (pinfo_str->bitcount)
				{
				case	1:
				case	8:
					pinfo_str->r_mask = 0;
					pinfo_str->g_mask = 0;
					pinfo_str->b_mask = 0;
					pinfo_str->a_mask = 0;
					break;
				case	24:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0;
					break;
				case	32:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0xff000000;
					break;
				}

				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}


			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
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

			/* PSD图总是正向的 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			}

			/* 读取位数据到内存 */
			if ((b_status=read_psd(pfile, pinfo_str, &psd_header)) != ER_SUCCESS)
			{
				__leave;
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
			
			LeaveCriticalSection(&psd_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
PSD_API int CALLAGREEMENT psd_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&psd_save_img_critical);
	
			b_status = ER_NOTSUPPORT;
			/* ----------------------------------------------------------------->
			   Janhail:
			       1，我认为PSD的格式特殊，即使保存也只能保存成多通道(深度)的单张背景图
			       2，PhotoShop 能识别大部分的图像格式
			       3，除PhotoShop 外PSD 格式的应用很少
			     所以不要保存命令. &_* 我是不是有点懒，"是!!" 不要这样说我嘛！
			   ----------------------------------------------------------------->
			   YZ:
			       呵呵:)，我也认为暂时先不实现保存功能，也许涉及到使用许可的问题。
			*/

			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;
		}
		__finally
		{
			LeaveCriticalSection(&psd_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/***********************************************************************************\
$                                                                                   $
$    函数： enum EXERESULT read_psd( ISFILE *pfile, 								$
$										LPINFOSTR pInfo, 							$
$										LPPSDFILEHEADER psdheader)                  $
$           将指定文件读入目标缓冲区                                                $
$                                                                                   $
$    参数： file        指定文件                                                    $
$           pInfo       图像读写数据包结构指针                                      $
$			psdheader	PSD文件信息头												$
$                                                                                   $
$    返回：enum EXERESULT 型，参见iseeirw.h中相应定义                               $
$                                                                                   $
\***********************************************************************************/
static enum EXERESULT CALLAGREEMENT read_psd(ISFILE *pfile, LPINFOSTR pInfo, LPPSDFILEHEADER psdheader)
{
	unsigned long	temp_data;

	int				j;
	unsigned long	imgdata;

	static char		palbuf[768];
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pInfo&&psdheader);


	__try
	{
		if (isio_seek(pfile, sizeof(PSDFILEHEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		/* 跳过调色板段、图象资源段、图象层和蒙板段 */
		if (_skip_block( pfile )||_skip_block( pfile )||_skip_block( pfile ))
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* 获得象素数据偏移 */
		imgdata = isio_tell(pfile);

		
		/* 分类读取象素数据 */
		switch (psdheader->Mode)
		{
		case	0:		/* 单色黑白图 */
			pInfo->pal_count = 2;
			
			pInfo->palette[0] = 0xffffff;
			pInfo->palette[1] = 0x0;

			if ((b_status=_read_bitmap(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	1:		/* 灰度图 */
			pInfo->pal_count = 256;
			
			/* 人工合成灰度调色板 */
			for (j=0;j<(int)pInfo->pal_count;j++)
			{
				pInfo->palette[j] = (unsigned long)((j<<16)|(j<<8)|(j));
			}

			if ((b_status=_read_grayscale_index_duotone(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	2:		/* 调色板图 */
			pInfo->pal_count = 256;
			
			if (isio_seek(pfile, sizeof(PSDFILEHEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 读取调色板尺寸值 */
			if (isio_read((void*)&temp_data, sizeof(unsigned long), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave; break;
			}
			
			temp_data = EXCHANGE_DWORD( temp_data ) ;
			
			/* 如果调色板尺寸不是768（256×3），则认为是坏的图象 */
			if (temp_data != 768)
			{
				b_status = ER_BADIMAGE; __leave; break;
			}
			
			/* 读取调色板信息 */
			if (isio_read((void*)palbuf, 256*3, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave; break;
			}
			
			/* 导入数据包 */
			for (j=0;j<(int)pInfo->pal_count;j++)
			{
				pInfo->palette[j] = (unsigned long)(((unsigned long)(unsigned char)palbuf[j]<<16)|((unsigned long)(unsigned char)palbuf[j+256]<<8)|((unsigned long)(unsigned char)palbuf[j+512]));
			}

			if ((b_status=_read_grayscale_index_duotone(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	3:		/* RGB图 */
			pInfo->pal_count = 0;

			if ((b_status=_read_rgb(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	4:		/* CMYK图 */
			pInfo->pal_count = 0;

			if ((b_status=_read_cmyk(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	8:		/* 双色调图 */
			pInfo->pal_count = 256;
			
			/* 人工合成灰度调色板 */
			for (j=0;j<(int)pInfo->pal_count;j++)
			{
				pInfo->palette[j] = (unsigned long)((j<<16)|(j<<8)|(j));
			}

			/* 双色调图将视为灰度图像读取 */
			if ((b_status=_read_grayscale_index_duotone(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		case	9:		/* Lab图 */
			pInfo->pal_count = 0;

			/* Lab还未经过公式转换 */
			if ((b_status=_read_lab(pfile, pInfo, psdheader, imgdata)) != ER_SUCCESS)
			{
				__leave; break;
			}
			break;
		default:
			b_status = ER_BADIMAGE; __leave; break;
		}
	}
	__finally
	{
	}

	return b_status;
}


/* 读BITMAP型图到数据包 */
static enum EXERESULT CALLAGREEMENT _read_bitmap(ISFILE *pfile, 
										  LPINFOSTR pInfo, 
										  LPPSDFILEHEADER psdheader,
										  unsigned long imgdata)
{
	int				i, j, w, h, linesize;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	__try
	{
		/* 定位到象素数据起始点 */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 跳过压缩算法字 */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* 翻转字序 */
		compression = EXCHANGE_WORD(compression);

		w = (int)pInfo->width;
		h = (int)pInfo->height;

		/* 取得PSD扫描行尺寸（一字节对齐：Janhail）*/
		linesize = (w+7)/8;
		
		if (compression == 0)			/* NONE */
		{
			for (i=0; i<h; i++)
			{
				/* 读取一行 */
				if (isio_read((void*)pInfo->pp_line_addr[i], linesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 转换象素字节内顺序以符合ISee格式 */
				for (j=0; j<linesize; j++)
				{
					CVT_BITS1((((unsigned char *)pInfo->pp_line_addr[i])[j]));
				}

				/* 累加步进值 */
				pInfo->process_current = i;

				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* 提取RLE行长度数据 */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}
			
			assert(prle_header);
			
			/* 分配RLE行数据缓冲区 */
			if ((buf=(signed char *)malloc(linesize)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			assert(pInfo->p_bit_data);
			
			/* 读取RLE数据并将其解压至数据包 */
			for (i=0; i<(int)(pInfo->height); i++)
			{
				if (_read_line(pfile, -1, prle_header[i], buf))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 解压一行的RLE数据（忽略错误数据的情况）*/
				_decode_rle_line(buf, (long)prle_header[i], (unsigned char *)(pInfo->pp_line_addr[i]), (long)linesize);
				
				for (j=0; j<(int)linesize; j++)
				{
					CVT_BITS1((((unsigned char *)pInfo->pp_line_addr[i])[j]));
				}

				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf)
			free(buf);
		
		if (prle_header)
			free(prle_header);
	}

	return b_status;
}


/* 读灰度、索引、双色调图到数据包 */
static enum EXERESULT CALLAGREEMENT _read_grayscale_index_duotone(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, w, h, linesize;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	__try
	{
		/* 定位到象素数据起始点 */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 跳过压缩算法字 */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 翻转字序 */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		/* 取得PSD扫描行尺寸 */
		linesize = w;
		
		if (compression == 0)			/* NONE */
		{
			for (i=0; i<h; i++)
			{
				/* 读取一行 */
				if (isio_read((void*)pInfo->pp_line_addr[i], linesize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* 提取RLE行长度数据 */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);

			/* 分配RLE行数据缓冲区 */
			if ((buf=(signed char *)malloc(psdheader->Columns*2+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			assert(pInfo->p_bit_data);

			/* 读取RLE数据并将其解压至数据包 */
			for (i=0; i<(int)(pInfo->height); i++)
			{
				if (_read_line(pfile, -1, prle_header[i], buf))
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 解压一行的RLE数据（忽略错误数据的情况）*/
				_decode_rle_line(buf, (long)prle_header[i], (unsigned char *)(pInfo->pp_line_addr[i]), (long)psdheader->Columns);

				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}



/* 读RGB图到数据包 */
static enum EXERESULT CALLAGREEMENT _read_rgb(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, j, w, h;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0, *buf2 = 0;
	unsigned long	chan_imgsize, rpos, gpos, bpos, apos;
	unsigned char	*pr, *pg, *pb, *pa, *pdesc;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	__try
	{
		/* 定位到象素数据起始点 */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 跳过压缩算法字 */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 翻转字序 */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		if (compression == 0)			/* NONE */
		{
			/* 一个通道所有数据的尺寸 */
			chan_imgsize = (unsigned long)w * (unsigned long)h;
			
			/* 计算各通道在流中的偏移 */
			rpos = (unsigned long)isio_tell(pfile);
			gpos = rpos + chan_imgsize;
			bpos = gpos + chan_imgsize;
			apos = bpos + chan_imgsize;
			
			/* 分配行数据缓冲区 */
			if ((buf=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 各通道在缓冲区中的起始位置 */
			pr = buf;
			pg = pr+(psdheader->Columns+4);
			pb = pg+(psdheader->Columns+4);
			pa = pb+(psdheader->Columns+4);

			for (i=0; i<h; i++)
			{
				/* 读取一行的RGB数据 */
				if (_read_line(pfile, rpos, (unsigned short)psdheader->Columns, (signed char *)pr))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, gpos, (unsigned short)psdheader->Columns, (signed char *)pg))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, (unsigned short)psdheader->Columns, (signed char *)pb))
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 如果有一个附加通道，则读取 */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, apos, (unsigned short)psdheader->Columns, (signed char *)pa))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				rpos += (unsigned long)psdheader->Columns;
				gpos += (unsigned long)psdheader->Columns;
				bpos += (unsigned long)psdheader->Columns;
				apos += (unsigned long)psdheader->Columns;

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);

				/* 合成RGB象素数据 */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					*pdesc++ = pb[j];
					*pdesc++ = pg[j];
					*pdesc++ = pr[j];

					if (psdheader->Channels == 4)
					{
						*pdesc++ = pa[j];
					}
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* 提取RLE行长度数据 */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);


			/* 一个通道所有数据的尺寸 */
			chan_imgsize = 0UL;
			
			/* 计算各通道在流中的偏移 */
			rpos = (unsigned long)isio_tell(pfile);

			for (i=(int)psdheader->Rows*0; i<(int)psdheader->Rows*1; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			gpos = rpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*1; i<(int)psdheader->Rows*2; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			bpos = gpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*2; i<(int)psdheader->Rows*3; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			apos = bpos + chan_imgsize;
			

			/* 分配RLE行数据缓冲区 */
			if ((buf=(signed char *)malloc((psdheader->Columns*2+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 分配解压后的行数据缓冲区 */
			if ((buf2=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 各通道在缓冲区中的起始位置 */
			pr = buf2;
			pg = pr+(psdheader->Columns+4);
			pb = pg+(psdheader->Columns+4);
			pa = pb+(psdheader->Columns+4);

			assert(pInfo->p_bit_data);

			/* 读取RLE数据并将其解压至数据包 */
			for (i=0; i<(int)(pInfo->height); i++)
			{
#ifdef _DEBUG
				memset((void*)buf, 0, (psdheader->Columns*2+4)*4);
				memset((void*)buf2, 0, (psdheader->Columns+4)*4);
#endif
				if (_read_line(pfile, rpos, prle_header[psdheader->Rows*0+i], (signed char *)(buf+psdheader->Columns*2*0)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, gpos, prle_header[psdheader->Rows*1+i], (signed char *)(buf+psdheader->Columns*2*1)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, prle_header[psdheader->Rows*2+i], (signed char *)(buf+psdheader->Columns*2*2)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 如果有一个附加通道，则读取 */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, apos, prle_header[psdheader->Rows*3+i], (signed char *)(buf+psdheader->Columns*2*3)))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				rpos += (unsigned long)prle_header[psdheader->Rows*0+i];
				gpos += (unsigned long)prle_header[psdheader->Rows*1+i];
				bpos += (unsigned long)prle_header[psdheader->Rows*2+i];

				if (psdheader->Channels == 4)
				{
					apos += (unsigned long)prle_header[psdheader->Rows*3+i];
				}
				
				/* 解压RLE数据（忽略错误数据的情况）*/
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*0), (long)prle_header[psdheader->Rows*0+i], pr, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*1), (long)prle_header[psdheader->Rows*1+i], pg, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*2), (long)prle_header[psdheader->Rows*2+i], pb, (long)psdheader->Columns);

				if (psdheader->Channels == 4)
				{
					_decode_rle_line((signed char *)(buf+psdheader->Columns*2*3), (long)prle_header[psdheader->Rows*3+i], pa, (long)psdheader->Columns);
				}

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);
				
				/* 合成RGB象素数据 */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					*pdesc++ = pb[j];
					*pdesc++ = pg[j];
					*pdesc++ = pr[j];
					
					if (psdheader->Channels == 4)
					{
						*pdesc++ = pa[j];
					}
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf2)
			free(buf2);

		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}


/* 读CMYK图到数据包 */
static enum EXERESULT CALLAGREEMENT _read_cmyk(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, j, w, h;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0, *buf2 = 0;
	unsigned long	chan_imgsize, cpos, mpos, ypos, kpos;
	unsigned char	*pc, *pm, *py, *pk, *pdesc, r, g, b;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psdheader->Channels == 4);		/* 只能处理4通道数据 */

	__try
	{
		/* 定位到象素数据起始点 */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 跳过压缩算法字 */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 翻转字序 */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		if (compression == 0)			/* NONE */
		{
			/* 一个通道所有数据的尺寸 */
			chan_imgsize = (unsigned long)w * (unsigned long)h;
			
			/* 计算各通道在流中的偏移 */
			cpos = (unsigned long)isio_tell(pfile);
			mpos = cpos + chan_imgsize;
			ypos = mpos + chan_imgsize;
			kpos = ypos + chan_imgsize;
			
			/* 分配行数据缓冲区 */
			if ((buf=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 各通道在缓冲区中的起始位置 */
			pc = buf;
			pm = pc+(psdheader->Columns+4);
			py = pm+(psdheader->Columns+4);
			pk = py+(psdheader->Columns+4);

			for (i=0; i<h; i++)
			{
				/* 读取一行的RGB数据 */
				if (_read_line(pfile, cpos, (unsigned short)psdheader->Columns, (signed char *)pc))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, mpos, (unsigned short)psdheader->Columns, (signed char *)pm))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, ypos, (unsigned short)psdheader->Columns, (signed char *)py))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, kpos, (unsigned short)psdheader->Columns, (signed char *)pk))
				{
					b_status = ER_FILERWERR; __leave;
				}

				cpos += (unsigned long)psdheader->Columns;
				mpos += (unsigned long)psdheader->Columns;
				ypos += (unsigned long)psdheader->Columns;
				kpos += (unsigned long)psdheader->Columns;
				
				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);

				/* 合成RGB象素数据 */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					/* 这是一个很粗糙的转换公式 */
					CVT_CMYK_TO_RGB(((unsigned char)pc[j]), ((unsigned char)pm[j]), ((unsigned char)py[j]), ((signed char)pk[j]), r, g, b);

					*pdesc++ = b;
					*pdesc++ = g;
					*pdesc++ = r;
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* 提取RLE行长度数据 */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);


			/* 一个通道所有数据的尺寸 */
			chan_imgsize = 0UL;
			
			/* 计算各通道在流中的偏移 */
			cpos = (unsigned long)isio_tell(pfile);

			for (i=(int)psdheader->Rows*0; i<(int)psdheader->Rows*1; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			mpos = cpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*1; i<(int)psdheader->Rows*2; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			ypos = mpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*2; i<(int)psdheader->Rows*3; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			kpos = ypos + chan_imgsize;
			

			/* 分配RLE行数据缓冲区 */
			if ((buf=(signed char *)malloc((psdheader->Columns*2+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 分配解压后的行数据缓冲区 */
			if ((buf2=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 各通道在缓冲区中的起始位置 */
			pc = buf2;
			pm = pc+(psdheader->Columns+4);
			py = pm+(psdheader->Columns+4);
			pk = py+(psdheader->Columns+4);

			assert(pInfo->p_bit_data);

			/* 读取RLE数据并将其解压至数据包 */
			for (i=0; i<(int)(pInfo->height); i++)
			{
#ifdef _DEBUG
				memset((void*)buf, 0, (psdheader->Columns*2+4)*4);
				memset((void*)buf2, 0, (psdheader->Columns+4)*4);
#endif
				if (_read_line(pfile, cpos, prle_header[psdheader->Rows*0+i], (signed char *)(buf+psdheader->Columns*2*0)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, mpos, prle_header[psdheader->Rows*1+i], (signed char *)(buf+psdheader->Columns*2*1)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, ypos, prle_header[psdheader->Rows*2+i], (signed char *)(buf+psdheader->Columns*2*2)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, kpos, prle_header[psdheader->Rows*3+i], (signed char *)(buf+psdheader->Columns*2*3)))
				{
					b_status = ER_FILERWERR; __leave;
				}

				cpos += (unsigned long)prle_header[psdheader->Rows*0+i];
				mpos += (unsigned long)prle_header[psdheader->Rows*1+i];
				ypos += (unsigned long)prle_header[psdheader->Rows*2+i];
				kpos += (unsigned long)prle_header[psdheader->Rows*3+i];
				
				/* 解压RLE数据（忽略错误数据的情况）*/
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*0), (long)prle_header[psdheader->Rows*0+i], pc, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*1), (long)prle_header[psdheader->Rows*1+i], pm, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*2), (long)prle_header[psdheader->Rows*2+i], py, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*3), (long)prle_header[psdheader->Rows*3+i], pk, (long)psdheader->Columns);

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);
				
				/* 合成RGB象素数据 */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					CVT_CMYK_TO_RGB(((unsigned char)pc[j]), ((unsigned char)pm[j]), ((unsigned char)py[j]), ((signed char)pk[j]), r, g, b);
					
					*pdesc++ = b;
					*pdesc++ = g;
					*pdesc++ = r;
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf2)
			free(buf2);

		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}


/* 读Lab图到数据包 */
static enum EXERESULT CALLAGREEMENT _read_lab(ISFILE *pfile, 
											 LPINFOSTR pInfo, 
											 LPPSDFILEHEADER psdheader,
											 unsigned long imgdata)
{
	int				i, j, w, h;
	unsigned short	compression, *prle_header = 0;
	signed char		*buf = 0, *buf2 = 0;
	unsigned long	chan_imgsize, lpos, apos, bpos, hpos;
	unsigned char	*pl, *pa, *pb, *ph, *pdesc;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert((psdheader->Channels == 3)||(psdheader->Channels == 4));

	__try
	{
		/* 定位到象素数据起始点 */
		if (isio_seek(pfile, imgdata, SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 跳过压缩算法字 */
		if (isio_read((void*)&compression, sizeof(unsigned short), 1, pfile) == 0)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		/* 翻转字序 */
		compression = EXCHANGE_WORD(compression);
		
		w = (int)pInfo->width;
		h = (int)pInfo->height;
		
		if (compression == 0)			/* NONE */
		{
			/* 一个通道所有数据的尺寸 */
			chan_imgsize = (unsigned long)w * (unsigned long)h;
			
			/* 计算各通道在流中的偏移 */
			lpos = (unsigned long)isio_tell(pfile);
			apos = lpos + chan_imgsize;
			bpos = apos + chan_imgsize;
			hpos = bpos + chan_imgsize;
			
			/* 分配行数据缓冲区 */
			if ((buf=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 各通道在缓冲区中的起始位置 */
			pl = buf;
			pa = pl+(psdheader->Columns+4);
			pb = pa+(psdheader->Columns+4);
			ph = pb+(psdheader->Columns+4);

			for (i=0; i<h; i++)
			{
				/* 读取一行的RGB数据 */
				if (_read_line(pfile, lpos, (unsigned short)psdheader->Columns, (signed char *)pl))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, apos, (unsigned short)psdheader->Columns, (signed char *)pa))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, (unsigned short)psdheader->Columns, (signed char *)pb))
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 如果有一个附加通道，则读取 */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, hpos, (unsigned short)psdheader->Columns, (signed char *)ph))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				lpos += (unsigned long)psdheader->Columns;
				apos += (unsigned long)psdheader->Columns;
				bpos += (unsigned long)psdheader->Columns;
				hpos += (unsigned long)psdheader->Columns;

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);

				/* 合成RGB象素数据 */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					/* Lab to RGB（此处没有做Lab to RGB的转换，资料缺乏....） */
					*pdesc++ = pb[j];
					*pdesc++ = pa[j];
					*pdesc++ = pl[j];

					if (psdheader->Channels == 4)
					{
						*pdesc++ = ph[j];
					}
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else if (compression == 1)		/* RLE8 */
		{
			/* 提取RLE行长度数据 */
			switch (_get_rle_header(pfile, psdheader, &prle_header))
			{
			case	0:
				break;
			case	-1:
				b_status = ER_MEMORYERR; __leave; break;
			case	-2:
				b_status = ER_FILERWERR; __leave; break;
			default:
				assert(0); b_status = ER_SYSERR; __leave; break;
			}

			assert(prle_header);


			/* 一个通道所有数据的尺寸 */
			chan_imgsize = 0UL;
			
			/* 计算各通道在流中的偏移 */
			lpos = (unsigned long)isio_tell(pfile);

			for (i=(int)psdheader->Rows*0; i<(int)psdheader->Rows*1; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			apos = lpos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*1; i<(int)psdheader->Rows*2; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			bpos = apos + chan_imgsize;

			chan_imgsize = 0UL;
			for (i=(int)psdheader->Rows*2; i<(int)psdheader->Rows*3; i++)
			{
				chan_imgsize += (unsigned long)prle_header[i];
			}
			hpos = bpos + chan_imgsize;
			

			/* 分配RLE行数据缓冲区 */
			if ((buf=(signed char *)malloc((psdheader->Columns*2+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 分配解压后的行数据缓冲区 */
			if ((buf2=(unsigned char *)malloc((psdheader->Columns+4)*4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 各通道在缓冲区中的起始位置 */
			pl = buf2;
			pa = pl+(psdheader->Columns+4);
			pb = pa+(psdheader->Columns+4);
			ph = pb+(psdheader->Columns+4);

			assert(pInfo->p_bit_data);

			/* 读取RLE数据并将其解压至数据包 */
			for (i=0; i<(int)(pInfo->height); i++)
			{
#ifdef _DEBUG
				memset((void*)buf, 0, (psdheader->Columns*2+4)*4);
				memset((void*)buf2, 0, (psdheader->Columns+4)*4);
#endif
				if (_read_line(pfile, lpos, prle_header[psdheader->Rows*0+i], (signed char *)(buf+psdheader->Columns*2*0)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, apos, prle_header[psdheader->Rows*1+i], (signed char *)(buf+psdheader->Columns*2*1)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, bpos, prle_header[psdheader->Rows*2+i], (signed char *)(buf+psdheader->Columns*2*2)))
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 如果有一个附加通道，则读取 */
				if (psdheader->Channels == 4)
				{
					if (_read_line(pfile, hpos, prle_header[psdheader->Rows*3+i], (signed char *)(buf+psdheader->Columns*2*3)))
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				lpos += (unsigned long)prle_header[psdheader->Rows*0+i];
				apos += (unsigned long)prle_header[psdheader->Rows*1+i];
				bpos += (unsigned long)prle_header[psdheader->Rows*2+i];

				if (psdheader->Channels == 4)
				{
					hpos += (unsigned long)prle_header[psdheader->Rows*3+i];
				}
				
				/* 解压RLE数据（忽略错误数据的情况）*/
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*0), (long)prle_header[psdheader->Rows*0+i], pl, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*1), (long)prle_header[psdheader->Rows*1+i], pa, (long)psdheader->Columns);
				_decode_rle_line((signed char *)(buf+psdheader->Columns*2*2), (long)prle_header[psdheader->Rows*2+i], pb, (long)psdheader->Columns);

				if (psdheader->Channels == 4)
				{
					_decode_rle_line((signed char *)(buf+psdheader->Columns*2*3), (long)prle_header[psdheader->Rows*3+i], ph, (long)psdheader->Columns);
				}

				pdesc = (unsigned char *)(pInfo->pp_line_addr[i]);
				
				/* 合成RGB象素数据 */
				for (j=0; j<(int)psdheader->Columns; j++)
				{
					/* Lab to RGB（此处没有做Lab to RGB的转换，资料缺乏....） */
					*pdesc++ = pb[j];
					*pdesc++ = pa[j];
					*pdesc++ = pl[j];
					
					if (psdheader->Channels == 4)
					{
						*pdesc++ = ph[j];
					}
				}
				
				/* 累加步进值 */
				pInfo->process_current = i;
				
				/* 支持用户中断 */
				if (pInfo->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
		}
		else
		{
			assert(0); b_status = ER_SYSERR; __leave;
		}
	}
	__finally
	{
		if (buf2)
			free(buf2);

		if (buf)
			free(buf);

		if (prle_header)
			free(prle_header);
	}
	
	return b_status;
}


/* 获取RLE行长度数组 */
static int CALLAGREEMENT _get_rle_header(ISFILE *pfile, LPPSDFILEHEADER psdheader, unsigned short **prle_header)
{
	int					header_size, i;
	unsigned short		*ps;

	assert(pfile&&psdheader&&prle_header);

	*prle_header = (unsigned short *)0;

	/* 数组长度（字节单位） */
	header_size = psdheader->Rows * psdheader->Channels * sizeof(unsigned short);

	/* 本历程假设当前流读写位置在压缩方式域之后！*/
	if ((ps=(unsigned short *)malloc(header_size)) == 0)
	{
		return -1;		/* 内存不足 */
	}

	/* 读取RLE行长度数据 */
	if (isio_read((void*)ps, header_size, 1, pfile) == 0)
	{
		free(ps);
		return -2;		/* 读流错 */
	}

	/* 翻转字序 */
	for (i=0; i<(int)(header_size/sizeof(unsigned short)); i++)
	{
		ps[i] = EXCHANGE_WORD(ps[i]);
	}

	*prle_header = ps;

	return 0;			/* 成功 */
}


/* 读取一行RLE压缩数据 */
static int CALLAGREEMENT _read_line(ISFILE *pfile, long pos, unsigned short size, signed char *pdata)
{
	assert(pdata);

	if (pos >= 0)
	{
		if (isio_seek(pfile, pos, SEEK_SET) == -1)
		{
			return -1;		/* 读流错 */
		}
	}

	/* 读取RLE数据 */
	if (isio_read((void*)pdata, size, 1, pfile) == 0)
	{
		return -1;		/* 读流错 */
	}
	
	return 0;
}


/* 解压一行RLE数据 */
static int CALLAGREEMENT _decode_rle_line(signed char *psou, long ssize, unsigned char *pdes, long dsize)
{
	signed char		n;
	unsigned char	b;
	int				mark = 0;

	assert(psou&&pdes);

	while ((ssize>0) && (dsize>0))
	{
		/* 提取一字节的源数据 */
		n = *psou++, ssize--;
		
		if (n < 0)
		{
			if (n == -128)
			{
				continue;
			}

			/* 计算出后续字节的重复次数 */
            n = -n + 1;

			/* 这的数据类型转换很棘手，稍有不慎就会掉到冰窟隆里！*/
            if( dsize < (long)(unsigned int)(unsigned char)n )
            {
                /* 如果目标缓冲区无法容纳 n 字节的数据，则抛弃一些数据。（容错处理） */
                n = (signed char)(unsigned char)dsize, dsize = 0, mark = 1;
            }
			else
			{
				dsize -= (long)(unsigned int)(unsigned char)n;
			}

			b = *psou++, ssize--;

			/* 写入重复数据 */
			while ((int)(unsigned int)(unsigned char)n-- > 0)
			{
				*pdes++ = b;
			}
		}
		else
		{
			if (dsize < ((long)(unsigned int)(unsigned char)n+1))
            {
                /* 如果目标缓冲区无法容纳解压数据，则抛弃部分数据 */
                n = (signed char)((unsigned char)dsize - 1), mark = 1;
            }
			/* 直接拷贝未经压缩的数据 */
            memcpy(pdes, psou, (size_t)(unsigned int)(unsigned char)(++n));

			pdes += (unsigned int)(unsigned char)n; dsize -= (unsigned int)(unsigned char)n;
			psou += (unsigned int)(unsigned char)n; ssize -= (unsigned int)(unsigned char)n;
		}
	}

	if (dsize > 0)
	{
		mark = 1;
	}

	/* 返回值：0－成功，1－源数据有错误，但已经过容错处理 */
	return mark;
}


/***********************************************************************************\
$                                                                                   $
$    函数：int _is_support_format( unsigned short, unsigned short ) ;               $
$          检查颜色模式和色深是否被支持                                             $
$                                                                                   $
$    参数：color_mode   颜色的模式                                                  $
$                 Bitmap=0, Grayscale=1, Indexed=2, RGB=3,                          $
$                 CMYK=4, Multichannel=7, Duotone=8, Lab=9                          $
$          color_pixel  颜色的位深                                                  $
$                 PSD 格式只有 1, 8, 16                                             $
$                                                                                   $
$    返回：int 型                                                                   $
$                 成功支持=0, 颜色模式不支持=1, 位深不支持=2                        $
$                                                                                   $
\***********************************************************************************/
int	CALLAGREEMENT _is_support_format( unsigned short color_mode, unsigned short color_pixel )
{
	/* 下面是测试 color_mode ,color_pixel 的正确性 */
	switch( color_mode )
	{
	case 0:	/* Bitmap */
		if( color_pixel == 1)	break ;
		return	2 ;
	case 1:	/* Grayscale */
	case 2:	/* Indexed */
	case 3:	/* RGB */
	case 4:	/* CMYK */
	case 8:	/* Duotone (按灰度的方法读取) */
	case 9:	/* Lab */
		if( color_pixel == 8 )	break ;
		return	2 ;
	case 7:	/* Multichannel（暂不支持多通道图象） */
	default:/* Unknow -> Error */
		return	1 ;
	}
	return 0 ;
}


/***********************************************************************************\
$                                                                                   $
$    函数：int _skip_block( ISFILE *pfile ) ;                                       $
$          跳过下一个块，并移动流的读写指针                                         $
$                                                                                   $
$    参数：pfile   图象流指针                                                       $
$                                                                                   $
$    返回：int 型                                                                   $
$                 成功=0, 读失败=-1                                                 $
$                                                                                   $
\***********************************************************************************/
int	CALLAGREEMENT _skip_block( ISFILE *pfile )
{
	unsigned long	temp_data;

	if (isio_read((void*)&temp_data, sizeof(unsigned long), 1, pfile) == 0)
	{
		return -1;
	}

	temp_data = EXCHANGE_DWORD( temp_data ) ;

	if( temp_data )
	{
		if (isio_seek(pfile, temp_data, SEEK_CUR) == -1)
		{
			return -1;
		}
	}

	return 0;
}


