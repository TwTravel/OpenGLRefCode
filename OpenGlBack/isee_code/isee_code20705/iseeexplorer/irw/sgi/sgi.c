/********************************************************************

	sgi.c

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
	本文件用途：	ISee图像浏览器—SGI图像读写模块实现文件

					读取功能：8、24、32位SGI图象（包括压缩与未压缩）
							  
					保存功能：24位非压缩格式
							   

	本文件编写人：	YZ			yzfree##yeah.net

	本文件版本：	20428
	最后修改于：	2002-4-28

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-4		发布第一个新版本

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

#include "sgi.h"


IRWP_INFO			sgi_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	sgi_get_info_critical;	/* sgi_get_image_info函数的关键段 */
CRITICAL_SECTION	sgi_load_img_critical;	/* sgi_load_image函数的关键段 */
CRITICAL_SECTION	sgi_save_img_critical;	/* sgi_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif

/* SGI流头结构 */
static SGIHEADER	sgi_header;

/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _sgi_is_valid_img(LPINFOSTR pinfo_str);

static enum EXERESULT CALLAGREEMENT _read_uncom(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi);
static enum EXERESULT CALLAGREEMENT _read_rle8(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi);
static void _to_rgb(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *p, int width);
static void _to_rgba(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *pa, unsigned char *p, int width);
static int _read_long_tab(ISFILE *pfile, unsigned long *pl, int count);
static int _uncompression_rle8(unsigned char *prle, int rlesize, unsigned char *p, int bufsize);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&sgi_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&sgi_get_info_critical);
			InitializeCriticalSection(&sgi_load_img_critical);
			InitializeCriticalSection(&sgi_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&sgi_get_info_critical);
			DeleteCriticalSection(&sgi_load_img_critical);
			DeleteCriticalSection(&sgi_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

SGI_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&sgi_irwp_info;
}

#else

SGI_API LPIRWP_INFO CALLAGREEMENT sgi_get_plugin_info()
{
	_init_irwp_info(&sgi_irwp_info);

	return (LPIRWP_INFO)&sgi_irwp_info;
}

SGI_API void CALLAGREEMENT sgi_init_plugin()
{
	/* 初始化多线程同步对象 */
}

SGI_API void CALLAGREEMENT sgi_detach_plugin()
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
	/* 各位数减一，否则32位格式无法表示。此处请用SGI正确的位深填写！*/
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
				(const char *)":)");
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

	/* 别名个数（##需手动修正，如SGI格式有别名请参考BMP插件中别名的设置） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 3;
	
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"BW");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"RGB");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[2]), 
		(const char *)"RGBA");
	
	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
SGI_API int CALLAGREEMENT sgi_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE				*pfile = (ISFILE*)0;

	static SGIHEADER	sgi_header;

	enum EXERESULT		b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&sgi_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 读取流头结构 */
			if (isio_read((void*)&sgi_header, sizeof(SGIHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 转换字序 */
			sgi_header.magic  = EXCHANGE_WORD((sgi_header.magic));
			sgi_header.dim    = EXCHANGE_WORD((sgi_header.dim));
			sgi_header.width  = EXCHANGE_WORD((sgi_header.width));
			sgi_header.hight  = EXCHANGE_WORD((sgi_header.hight));
			sgi_header.channel= EXCHANGE_WORD((sgi_header.channel));

			sgi_header.pixmin = EXCHANGE_DWORD((sgi_header.pixmin));
			sgi_header.pixmax = EXCHANGE_DWORD((sgi_header.pixmax));
			sgi_header.palid  = EXCHANGE_DWORD((sgi_header.palid));


			/* 判断是否是有效（受支持）的SGI图象流 */
			if ((sgi_header.magic != SGI_MAGIC) || \
				(sgi_header.width == 0)		||		\
				(sgi_header.hight == 0)		||		\
				(sgi_header.channel == 0)	||		\
				(sgi_header.channel == 2)	||		\
				(sgi_header.channel > 4)	||		\
				(sgi_header.palid != 0)		||		\
				(sgi_header.dim == 0)		||		\
				(sgi_header.dim > 3)		||		\
				(sgi_header.bpc != 1))
			{
				b_status = ER_NONIMAGE; __leave;
			}


			/* 填写图象信息 */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像流类型 */
			pinfo_str->imgformat	= IMF_SGI;			/* 图像流格式（后缀名） */
			
			pinfo_str->compression  = (sgi_header.storage == 0) ? ICS_RGB : ICS_RLE8;

			pinfo_str->width		= (unsigned long)sgi_header.width;
			pinfo_str->height		= (unsigned long)sgi_header.hight;
			pinfo_str->order		= 1;				/* SGI 图象都为倒向图 */
			pinfo_str->bitcount		= (unsigned long)(sgi_header.bpc*sgi_header.channel*8);
			
			/* 设置掩码数据 */
			switch (pinfo_str->bitcount)
			{
			case	8:
				pinfo_str->r_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->b_mask = 0x0;
				pinfo_str->a_mask = 0x0;
				break;
			case	24:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0x0;
				break;
			case	32:
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->b_mask = 0xff;
				pinfo_str->a_mask = 0xff000000;
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

			LeaveCriticalSection(&sgi_get_info_critical);
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
SGI_API int CALLAGREEMENT sgi_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, linesize;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&sgi_load_img_critical);

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

			/* 读取流头结构 */
			if (isio_read((void*)&sgi_header, sizeof(SGIHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 转换字序 */
			sgi_header.magic  = EXCHANGE_WORD((sgi_header.magic));
			sgi_header.dim    = EXCHANGE_WORD((sgi_header.dim));
			sgi_header.width  = EXCHANGE_WORD((sgi_header.width));
			sgi_header.hight  = EXCHANGE_WORD((sgi_header.hight));
			sgi_header.channel= EXCHANGE_WORD((sgi_header.channel));
			
			sgi_header.pixmin = EXCHANGE_DWORD((sgi_header.pixmin));
			sgi_header.pixmax = EXCHANGE_DWORD((sgi_header.pixmax));
			sgi_header.palid  = EXCHANGE_DWORD((sgi_header.palid));

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 判断是否是有效（受支持）的SGI图象流 */
				if ((sgi_header.magic != SGI_MAGIC) || \
					(sgi_header.width == 0)		||		\
					(sgi_header.hight == 0)		||		\
					(sgi_header.channel == 0)	||		\
					(sgi_header.channel == 2)	||		\
					(sgi_header.channel > 4)	||		\
					(sgi_header.palid != 0)		||		\
					(sgi_header.dim == 0)		||		\
					(sgi_header.dim > 3)		||		\
					(sgi_header.bpc != 1))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* 填写图象信息 */
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像流类型 */
				pinfo_str->imgformat	= IMF_SGI;			/* 图像流格式（后缀名） */
				
				pinfo_str->compression  = (sgi_header.storage == 0) ? ICS_RGB : ICS_RLE8;
				
				pinfo_str->width		= (unsigned long)sgi_header.width;
				pinfo_str->height		= (unsigned long)sgi_header.hight;
				pinfo_str->order		= 1;				/* SGI 图象都为倒向图 */
				pinfo_str->bitcount		= (unsigned long)(sgi_header.bpc*sgi_header.channel*8);
				
				/* 设置掩码数据 */
				switch (pinfo_str->bitcount)
				{
				case	8:
					pinfo_str->r_mask = 0x0;
					pinfo_str->g_mask = 0x0;
					pinfo_str->b_mask = 0x0;
					pinfo_str->a_mask = 0x0;
					break;
				case	24:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0x0;
					break;
				case	32:
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->b_mask = 0xff;
					pinfo_str->a_mask = 0xff000000;
					break;
				default:
					assert(0); b_status = ER_SYSERR; __leave; break;
				}

				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}
			
			
			/* 合成灰度调色板 */
			if (pinfo_str->bitcount == 8)
			{
				pinfo_str->pal_count = 256;

				for (i=0; i<256; i++)
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
			
			/* 初始化行首地址数组（倒向） */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}

			/* 读取图象到数据包 */
			if (sgi_header.storage == 0)
			{
				b_status = _read_uncom(pfile, pinfo_str, &sgi_header);
			}
			else
			{
				b_status = _read_rle8(pfile, pinfo_str, &sgi_header);
			}
			
			if (b_status != ER_SUCCESS)
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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			}

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&sgi_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
SGI_API int CALLAGREEMENT sgi_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	*pbuf = 0;
	int				i, j;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&sgi_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_sgi_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 设置进度值 */	
			pinfo_str->process_total   = 3;
			pinfo_str->process_current = 0;
			
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 源数据必须是标准的24位图象 */
			assert((pinfo_str->r_mask == 0xff0000)&&(pinfo_str->g_mask == 0xff00)&&(pinfo_str->b_mask == 0xff));

			/* 填写流头结构并转换为MOTO字序 */
			sgi_header.magic  = EXCHANGE_WORD((SGI_MAGIC));
			sgi_header.dim    = EXCHANGE_WORD((3));
			sgi_header.width  = EXCHANGE_WORD(((unsigned short)pinfo_str->width));
			sgi_header.hight  = EXCHANGE_WORD(((unsigned short)pinfo_str->height));
			sgi_header.channel= EXCHANGE_WORD((3));
			
			sgi_header.pixmin = EXCHANGE_DWORD((0));
			sgi_header.pixmax = EXCHANGE_DWORD((0xff));
			sgi_header.palid  = EXCHANGE_DWORD((0));

			sgi_header.storage= 0;
			sgi_header.bpc    = 1;

			memset((void*)sgi_header.rev0, 0, SGI_REV0_SIZE);
			memset((void*)sgi_header.rev1, 0, SGI_REV1_SIZE);
			memcpy((void*)sgi_header.imgname, (const void *)"iseeexplorer image.", 20);
			
			/* 写入文件头结构 */
			if (isio_write((const void *)&sgi_header, sizeof(SGIHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 分配颜色分量缓冲区 */
			if ((pbuf=(unsigned char *)malloc(pinfo_str->width+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			/* 写入R分量数据 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j] = ((unsigned char *)(pinfo_str->pp_line_addr[((int)pinfo_str->height)-i-1]))[j*3+2];
				}
				if (isio_write((const void *)pbuf, pinfo_str->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			pinfo_str->process_current = 1;
			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}


			/* 写入G分量数据 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j] = ((unsigned char *)(pinfo_str->pp_line_addr[((int)pinfo_str->height)-i-1]))[j*3+1];
				}
				if (isio_write((const void *)pbuf, pinfo_str->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			pinfo_str->process_current = 2;
			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}


			/* 写入B分量数据 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				for (j=0; j<(int)pinfo_str->width; j++)
				{
					pbuf[j] = ((unsigned char *)(pinfo_str->pp_line_addr[((int)pinfo_str->height)-i-1]))[j*3+0];
				}
				if (isio_write((const void *)pbuf, pinfo_str->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
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
			
			LeaveCriticalSection(&sgi_save_img_critical);
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
int CALLAGREEMENT _sgi_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(sgi_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (sgi_irwp_info.irwp_save.img_num)
	{
		if (sgi_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* 图像个数不正确 */
			}
		}
	}

	return 0;
}


/* 读未压缩图象数据到数据包中 */
static enum EXERESULT CALLAGREEMENT _read_uncom(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi)
{
	int				i, linesize;
	unsigned char	*p;
	long			fr, fg, fb, fa;
	unsigned char	*pr = 0, *pg = 0, *pb = 0, *pa = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(pfile&&pinfo_str&&psgi);

	__try
	{
		if (isio_seek(pfile, sizeof(SGIHEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}
		
		assert(pinfo_str->pp_line_addr);

		linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
		p = pinfo_str->p_bit_data;
		
		switch (pinfo_str->bitcount)
		{
		case	8:
			assert((psgi->channel == 1)&&(psgi->bpc == 1));

			/* 逐行读取象素数据 */
			for (i=0; i<(int)psgi->hight; i++)
			{
				if (isio_read((void*)p, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				pinfo_str->process_current = i;
				p += linesize;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	24:
			assert((psgi->channel == 3)&&(psgi->bpc == 1));

			/* 申请R、G、B临时缓冲区 */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);

			if (!(pr&&pg&&pb))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}

			/* 计算R、G、B分量数据在流中的偏移 */
			fr = sizeof(SGIHEADER);
			fg = fr + psgi->width * psgi->hight;
			fb = fg + psgi->width * psgi->hight;

			/* 逐行读取象素数据 */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* 红色分量 */
				if (isio_seek(pfile, fr, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pr, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 绿色分量 */
				if (isio_seek(pfile, fg, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pg, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 蓝色分量 */
				if (isio_seek(pfile, fb, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pb, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				_to_rgb(pr, pg, pb, p, psgi->width);

				pinfo_str->process_current = i;
				p += linesize;

				fr += (long)psgi->width;
				fg += (long)psgi->width;
				fb += (long)psgi->width;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	32:
			assert((psgi->channel == 4)&&(psgi->bpc == 1));

			/* 申请R、G、B临时缓冲区 */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);
			pa = (unsigned char *)malloc(psgi->width);
			
			if (!(pr&&pg&&pb&&pa))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}
			
			/* 计算R、G、B分量数据在流中的偏移 */
			fr = sizeof(SGIHEADER);
			fg = fr + psgi->width * psgi->hight;
			fb = fg + psgi->width * psgi->hight;
			fa = fb + psgi->width * psgi->hight;
			
			/* 逐行读取象素数据 */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* 红色分量 */
				if (isio_seek(pfile, fr, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pr, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 绿色分量 */
				if (isio_seek(pfile, fg, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pg, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 蓝色分量 */
				if (isio_seek(pfile, fb, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pb, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* ALPHA通道数据 */
				if (isio_seek(pfile, fa, SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)pa, psgi->width, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				_to_rgba(pr, pg, pb, pa, p, psgi->width);
				
				pinfo_str->process_current = i;
				p += linesize;
				
				fr += (long)psgi->width;
				fg += (long)psgi->width;
				fb += (long)psgi->width;
				fa += (long)psgi->width;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		default:
			assert(0); b_status = ER_SYSERR; __leave; break;
		}

	}
	__finally
	{
		if (pr)
			free(pr);
		if (pg)
			free(pg);
		if (pb)
			free(pb);
		if (pa)
			free(pa);
	}
	
	return b_status;
}


/* 读RLE8压缩图象数据到数据包中 */
static enum EXERESULT CALLAGREEMENT _read_rle8(ISFILE *pfile, LPINFOSTR pinfo_str, LPSGIHEADER psgi)
{
	int				i, linesize;
	unsigned char	*p;
	unsigned char	*pr = 0, *pg = 0, *pb = 0, *pa = 0, *ptmp = 0;
	unsigned long	*pofftab = 0, *plentab = 0;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(pfile&&pinfo_str&&psgi);
	
	__try
	{
		if (isio_seek(pfile, sizeof(SGIHEADER), SEEK_SET) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* 分配RLE解压用临时缓冲区 */
		if ((ptmp=(unsigned char *)malloc(psgi->width*2+4)) == 0)
		{
			b_status = ER_MEMORYERR; __leave;
		}

		/* 分配用于存储RLE行偏移及行长度的缓冲区 */
		pofftab = (unsigned long *)malloc(psgi->hight*psgi->channel*sizeof(long));
		plentab = (unsigned long *)malloc(psgi->hight*psgi->channel*sizeof(long));

		if (!(pofftab&&plentab))
		{
			b_status = ER_MEMORYERR; __leave;
		}

		/* 读入RLE行偏移表 */
		if (_read_long_tab(pfile, pofftab, psgi->hight*psgi->channel) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		/* 读入RLE行长度表 */
		if (_read_long_tab(pfile, plentab, psgi->hight*psgi->channel) == -1)
		{
			b_status = ER_FILERWERR; __leave;
		}

		linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);
		p = pinfo_str->p_bit_data;

		switch (pinfo_str->bitcount)
		{
		case	8:
			assert((psgi->channel == 1)&&(psgi->bpc == 1));
			
			/* 逐行读取象素数据 */
			for (i=0; i<(int)psgi->hight; i++)
			{
				if (isio_seek(pfile, pofftab[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 解压缩RLE行 */
				if (_uncompression_rle8(ptmp, (int)plentab[i], p, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				pinfo_str->process_current = i;
				p += linesize;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	24:
			assert((psgi->channel == 3)&&(psgi->bpc == 1));
			
			/* 申请R、G、B临时缓冲区 */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);
			
			if (!(pr&&pg&&pb))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}
			
			/* 逐行读取象素数据 */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* 红色分量 */
				if (isio_seek(pfile, pofftab[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i], pr, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* 绿色分量 */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)], pg, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 蓝色分量 */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)*2], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)*2], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)*2], pb, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				_to_rgb(pr, pg, pb, p, psgi->width);
				
				pinfo_str->process_current = i;
				p += linesize;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		case	32:
			assert((psgi->channel == 4)&&(psgi->bpc == 1));
			
			/* 申请R、G、B临时缓冲区 */
			pr = (unsigned char *)malloc(psgi->width);
			pg = (unsigned char *)malloc(psgi->width);
			pb = (unsigned char *)malloc(psgi->width);
			pa = (unsigned char *)malloc(psgi->width);
			
			if (!(pr&&pg&&pb&&pa))
			{
				b_status = ER_MEMORYERR; __leave; break;
			}
			
			/* 逐行读取象素数据 */
			for (i=0; i<(int)psgi->hight; i++)
			{
				/* 红色分量 */
				if (isio_seek(pfile, pofftab[i], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i], pr, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 绿色分量 */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)], pg, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 蓝色分量 */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)*2], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)*2], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)*2], pb, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* ALPHA通道数据 */
				if (isio_seek(pfile, pofftab[i+(int)(psgi->hight)*3], SEEK_SET) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (isio_read((void*)ptmp, plentab[i+(int)(psgi->hight)*3], 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_uncompression_rle8(ptmp, (int)plentab[i+(int)(psgi->hight)*3], pa, (int)psgi->width) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}

				_to_rgba(pr, pg, pb, pa, p, psgi->width);
				
				pinfo_str->process_current = i;
				p += linesize;
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					b_status = ER_USERBREAK; __leave;
				}
			}
			break;
		default:
			assert(0); b_status = ER_SYSERR; __leave; break;
		}
		
	}
	__finally
	{
		if (pr)
			free(pr);
		if (pg)
			free(pg);
		if (pb)
			free(pb);
		if (pa)
			free(pa);
		if (ptmp)
			free(ptmp);
		if (pofftab)
			free(pofftab);
		if (plentab)
			free(plentab);
	}
	
	return b_status;
}


/* 组合R、G、B分量到RGB数组 */
static void _to_rgb(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *p, int width)
{
	int	i;

	assert(pr&&pg&&pb&&p&&width);

	for (i=0; i<width; i++)
	{
		*p++ = *pb++;
		*p++ = *pg++;
		*p++ = *pr++;
	}
}


/* 组合R、G、B、A分量到RGBA数组 */
static void _to_rgba(unsigned char *pr, unsigned char *pg, unsigned char *pb, unsigned char *pa, unsigned char *p, int width)
{
	int	i;
	
	assert(pr&&pg&&pb&&pa&&p&&width);
	
	for (i=0; i<width; i++)
	{
		*p++ = *pb++;
		*p++ = *pg++;
		*p++ = *pr++;
		*p++ = *pa++;
	}
}


/* 读取long型表（偏移表或行长度表） */
static int _read_long_tab(ISFILE *pfile, unsigned long *pl, int count)
{
	int i;

	assert(pfile&&pl);

	if (isio_read((void*)pl, count*sizeof(long), 1, pfile) == 0)
	{
		return -1;		/* 读流出错 */
	}

	/* 转换字序 */
	for (i=0; i<count; i++)
	{
		pl[i] = EXCHANGE_DWORD((pl[i]));
	}

	return 0;
}


/* 解压一行RLE数据 */
static int _uncompression_rle8(unsigned char *prle, int rlesize, unsigned char *p, int bufsize)
{
	int				bufcount = 0;
	unsigned char	pix, count;

	while (1)
	{
		/* 计数值 */
		pix = *prle++; rlesize--;

		/* 提取计数值（如果计数值为0，则说明RLE行结束） */
		if ((count=(pix&0x7f)) == 0)
		{
			if ((bufcount == bufsize)&&(rlesize == 0))
				return 0;
			else
				return -1;
		}

		bufcount += (int)count;

		if (bufcount > bufsize)
		{
			return -1;		/* 源数据受损 */
		}

		if(pix & 0x80)
		{
			if (rlesize < (int)count)
			{
				return -1;	/* 错误 */
			}
			else
			{
				rlesize -= (int)count;
			}

			/* 复制未压缩数据 */
			while(count--)
			{
				*p++ = *prle++;
			}
		}
		else
		{
			if (rlesize == (int)0)
			{
				return -1;	/* 错误 */
			}
			else
			{
				rlesize--;
			}
			
			pix = *prle++;

			/* 复制重复数据 */
			while(count--)
			{
				*p++ = pix;
			}
		}

		if (bufcount == bufsize)
		{
			if (rlesize == 0)
				return 0;	/* 成功 */
			else if ((rlesize = 1)&&(((*prle)&0x7f) == 0))
				return 0;	/* 成功 */
			else
				return -1;
		}
	}

	return -1;				/* 错误 */
}

