/********************************************************************

	gif.c

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
	本文件用途：	ISee图像浏览器—GIF图像读写模块定义文件
	
					读取功能：1~8位、静态或动态GIF图像
					保存功能：因GIF-LZW专利期到2003年才失效，所以
							  暂不能提供保存功能
	  
	本文件编写人：	YZ			yzfree##sina.com
		
	本文件版本：	11227
	最后修改于：	2001-12-27
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		  2001-12		修正了GIF-LZW解压算法中存在的一处错误，并
						加强了_get_imginfo()函数的容错性。(B01)
		  2001-7		发布新版代码
		  2000-9		第一个测试版发布
		  2000-8		Sam编写了GIF读写库


	GIF图像文件格式版权声明：
	---------------------------------------------------------------
	The Graphics Interchange Format(c) is the Copyright property of
	CompuServe Incorporated. GIF(sm) is a Service Mark property of
	CompuServe Incorporated.
	---------------------------------------------------------------

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

#include "gif.h"


/* 插件信息表 */
IRWP_INFO			gif_irwp_info;


#ifdef WIN32
CRITICAL_SECTION	gif_get_info_critical;	/* gif_get_image_info函数的关键段 */
CRITICAL_SECTION	gif_load_img_critical;	/* gif_load_image函数的关键段 */
CRITICAL_SECTION	gif_save_img_critical;	/* gif_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void			CALLAGREEMENT _init_irwp_info(LPIRWP_INFO);
int				CALLAGREEMENT _get_imginfo(unsigned char *, unsigned long, LPGIFINSIDEINFO);
void			CALLAGREEMENT _get_imgdata_size(unsigned char *, unsigned long *, unsigned long *);

void			CALLAGREEMENT _init_lzw_table(LZWTABLE *, int, int);
unsigned char * CALLAGREEMENT _get_lzw_datablock(unsigned char *, unsigned long);
unsigned short	CALLAGREEMENT _get_next_codeword(unsigned char *, unsigned long, unsigned long, unsigned long, unsigned long);

LPSUBIMGBLOCK	CALLAGREEMENT _alloc_SUBIMGBLOCK(unsigned char *, unsigned char *, LPGRAPHCTRL);
void			CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK);

int				CALLAGREEMENT _load_static_gif(LPINFOSTR, unsigned char *, unsigned long, LPGIFINSIDEINFO);
int				CALLAGREEMENT _load_dyn_gif(LPINFOSTR, unsigned char *, unsigned long, LPGIFINSIDEINFO);
int				CALLAGREEMENT _build_bkimg(LPINFOSTR, LPGIFINSIDEINFO);
int				CALLAGREEMENT _load_subimg_serial(LPINFOSTR, unsigned char *, unsigned long);
LPSUBIMGBLOCK	CALLAGREEMENT _decomp_LZW_to_BMP(unsigned char *, unsigned char *, LPGRAPHCTRL, unsigned long *);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&gif_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&gif_get_info_critical);
			InitializeCriticalSection(&gif_load_img_critical);
			InitializeCriticalSection(&gif_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&gif_get_info_critical);
			DeleteCriticalSection(&gif_load_img_critical);
			DeleteCriticalSection(&gif_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

GIF_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&gif_irwp_info;
}

#else

GIF_API LPIRWP_INFO CALLAGREEMENT bmp_get_plugin_info()
{
	_init_irwp_info(&gif_irwp_info);

	return (LPIRWP_INFO)&gif_irwp_info;
}

GIF_API void CALLAGREEMENT gif_init_plugin()
{
	/* 初始化多线程同步对象 */
}

GIF_API void CALLAGREEMENT gif_detach_plugin()
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

	/* 设置模块支持的保存位深 */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 2;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"向GIF格式的制订者开火！:)");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"Sam");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)":)");
	/* ---------------------------------[2] － 第三组 -------------- */
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
GIF_API int CALLAGREEMENT gif_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	void			*ptmp = (void*)0;

	GIFHEADER		header;
	GIFINSIDEINFO	inside;

	int				type = 0;
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&gif_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 获取流长度 */
			stream_length = isio_length(pfile);

			/* 有效的图像流长度应大于头结构尺寸+终结码 */
			if (stream_length < (sizeof(GIFHEADER)+1))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 读取流头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			if (isio_read((void*)&header, sizeof(GIFHEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 判断是否是有效的图像流 */
			if (!memcmp((const void *)header.gif_type, (const char *)GIF_MARKER_89a, strlen((const char *)GIF_MARKER_89a)))
				type = 1;
			else if (!memcmp((const void *)header.gif_type, (const char *)GIF_MARKER_87a, strlen((const char *)GIF_MARKER_87a)))
				type = 2;
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}

			
			/* 申请内存，用于保存整个GIF流 */
			ptmp = malloc(stream_length);
			if (!ptmp)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 读取整个GIF流到内存 */
			if (isio_read(ptmp, stream_length, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 获取图像信息 */
			if (_get_imginfo((unsigned char *)ptmp, stream_length, &inside) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 如果只有一副图像，则视为静态图像，如果有多幅图像，则
			   为动态，并以逻辑屏作为背景图像(图像个数+1)。
			*/
			assert(inside.img_count);

			inside.img_count = (inside.img_count > 1) ? inside.img_count+1 : 1;

													/* 图像流类型 */
			pinfo_str->imgtype	= (inside.img_count>1) ? IMT_RESDYN : IMT_RESSTATIC;
			pinfo_str->imgformat= IMF_GIF;			/* 图像流格式（后缀名） */
			pinfo_str->compression = ICS_GIFLZW;	/* 图像压缩方式 */

			/* 如果是动态GIF，则取用逻辑屏的数据 */
			if (pinfo_str->imgtype == IMT_RESDYN)
			{
				pinfo_str->width	= inside.scr_width;
				pinfo_str->height	= inside.scr_height;
				pinfo_str->order	= 0;			/* GIF图像为正向 */
				/* 如果没有全局调色板数据，则取首幅图像对应数据 */
				if (inside.scr_bitcount == 0)
					pinfo_str->bitcount = inside.first_img_bitcount;
				else
					pinfo_str->bitcount	= inside.scr_bitcount;
			}
			else
			{
				pinfo_str->width	= inside.first_img_width;
				pinfo_str->height	= inside.first_img_height;
				pinfo_str->order	= 0;			/* GIF图像为正向 */
				pinfo_str->bitcount	= inside.first_img_bitcount;
			}

			/* GIF都为索引图，所以没有掩码数据 */
			pinfo_str->b_mask = 0x0;
			pinfo_str->g_mask = 0x0;
			pinfo_str->r_mask = 0x0;
			pinfo_str->a_mask = 0x0;

			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (ptmp)
				free(ptmp);
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&gif_get_info_critical);
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
GIF_API int CALLAGREEMENT gif_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned long	stream_length;
	void			*ptmp = (void*)0;
	int				result;
	
	GIFINSIDEINFO	inside;
	LPSUBIMGBLOCK	subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&gif_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 初始化进度指示变量（本模块因一次性需要读入所有数据，
			   所以对用户中断支持的不好）*/
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;

			/* 判断用户是否要终止操作 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 获取流长度 */
			stream_length = isio_length(pfile);

			/* 申请内存，用于保存整个GIF流 */
			ptmp = malloc(stream_length);
			if (!ptmp)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 读取整个GIF流到内存 */
			if (isio_read(ptmp, stream_length, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 获取图像信息 */
			if (_get_imginfo((unsigned char *)ptmp, stream_length, &inside) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			inside.img_count = (inside.img_count > 1) ? inside.img_count+1 : 1;
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 图像流类型 */
				pinfo_str->imgtype	= (inside.img_count>1) ? IMT_RESDYN : IMT_RESSTATIC;
				pinfo_str->imgformat= IMF_GIF;			/* 图像流格式（后缀名） */
				pinfo_str->compression = ICS_GIFLZW;	/* 图像压缩方式 */
				
				/* 如果是动态GIF，则取用逻辑屏的数据 */
				if (pinfo_str->imgtype == IMT_RESDYN)
				{
					pinfo_str->width	= inside.scr_width;
					pinfo_str->height	= inside.scr_height;
					pinfo_str->order	= 0;			/* GIF图像为正向 */
					/* 如果没有全局调色板数据，则取首幅图像对应数据 */
					if (inside.scr_bitcount == 0)
						pinfo_str->bitcount = inside.first_img_bitcount;
					else
						pinfo_str->bitcount	= inside.scr_bitcount;
				}
				else
				{
					pinfo_str->width	= inside.first_img_width;
					pinfo_str->height	= inside.first_img_height;
					pinfo_str->order	= 0;			/* GIF图像为正向 */
					pinfo_str->bitcount	= inside.first_img_bitcount;
				}
				
				/* GIF都为索引图，所以没有掩码数据 */
				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}			


			/* 读入GIF图像 */
			if (pinfo_str->imgtype == IMT_RESSTATIC)	/* 静态 */
			{
				result = _load_static_gif(pinfo_str, ptmp, stream_length, &inside);
			}
			else if (pinfo_str->imgtype == IMT_RESDYN)	/* 动态 */
			{
				result = _load_dyn_gif(pinfo_str, ptmp, stream_length, &inside);
			}
			else
				assert(0);		/* 设计错误 */

			if (result == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* 释放子图像链表 */
				if (pinfo_str->psubimg != 0)
				{
					while(subimg_tmp=pinfo_str->psubimg)
					{
						_free_SUBIMGBLOCK(pinfo_str->psubimg);
						pinfo_str->psubimg = subimg_tmp;
					}
					_free_SUBIMGBLOCK(pinfo_str->psubimg);
					pinfo_str->psubimg = 0;
				}
				
				/* 释放主图像 */
				if (pinfo_str->pp_line_addr != 0)
					free(pinfo_str->pp_line_addr);
				if (pinfo_str->p_bit_data != 0)
					free(pinfo_str->p_bit_data);

				pinfo_str->pp_line_addr = 0;
				pinfo_str->p_bit_data   = 0;
				pinfo_str->pal_count    = 0;

				pinfo_str->imgnumbers = 0;

				if (pinfo_str->data_state == 2)
					pinfo_str->data_state =1;	/* 自动降级 */
			}

			if (ptmp)
				free(ptmp);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&gif_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}




/* 保存图像（暂不会实现） */
GIF_API int CALLAGREEMENT gif_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
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
			EnterCriticalSection(&gif_save_img_critical);
	
			/* ############################################################# */
			/* 在此处插入保存代码 */
			/* ############################################################# */

			b_status = ER_NOTSUPPORT;	/* 现在暂时不支持写功能 */
		}
		__finally
		{
			LeaveCriticalSection(&gif_save_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}




/*==================================================================*/
/* 内部助手函数 */
/*==================================================================*/


/* 读入静态GIF图像数据 */
int CALLAGREEMENT _load_static_gif(
	LPINFOSTR pinfo_str, 
	unsigned char *stream, 
	unsigned long stream_len,
	LPGIFINSIDEINFO p_info)
{
	assert(pinfo_str->imgtype == IMT_RESSTATIC);
	
	if (_load_subimg_serial(pinfo_str, stream, stream_len) == -1)
		return -1;
	
	assert(pinfo_str->imgnumbers == 1);
	
	memcpy((void*)pinfo_str->palette, (const void *)pinfo_str->psubimg->palette, sizeof(unsigned long)*MAX_PALETTE_COUNT);
	pinfo_str->pal_count = pinfo_str->psubimg->pal_count;
	pinfo_str->pp_line_addr = pinfo_str->psubimg->pp_line_addr;
	pinfo_str->p_bit_data = pinfo_str->psubimg->p_bit_data;
	
	memset((void*)pinfo_str->psubimg, 0, sizeof(SUBIMGBLOCK));
	
	_free_SUBIMGBLOCK(pinfo_str->psubimg);
	
	pinfo_str->psubimg = 0;
	
	return 0;
}



/* 读入动态GIF图像数据 */
int CALLAGREEMENT _load_dyn_gif(
	LPINFOSTR pinfo_str, 
	unsigned char *stream, 
	unsigned long stream_len, 
	LPGIFINSIDEINFO p_info)
{
	LPGIFHEADER		pgif_header = (LPGIFHEADER)stream;
	LPSUBIMGBLOCK	subimg_tmp;
	
	assert(pinfo_str->imgtype == IMT_RESDYN);
	
	/* 读入子图像序列 */
	if (_load_subimg_serial(pinfo_str, stream, stream_len) == -1)
		return -1;
	
	assert(pinfo_str->imgnumbers > 1);
	
	/* 申请背景图 */
	if (_build_bkimg(pinfo_str, p_info) == -1)
	{
		/* 释放子图像链表 */
		if (pinfo_str->psubimg != 0)
		{
			while(subimg_tmp=pinfo_str->psubimg)
			{
				_free_SUBIMGBLOCK(pinfo_str->psubimg);
				pinfo_str->psubimg = subimg_tmp;
			}
			_free_SUBIMGBLOCK(pinfo_str->psubimg);
			pinfo_str->psubimg = 0;
		}
		
		pinfo_str->imgnumbers = 0;
		
		return -1;
	}
	
	/* 背景图也将计入图像个数中 */
	pinfo_str->imgnumbers++;
	
	return 0;
}


/* 获取GIF图像信息
/
/	参数p_filecontent指向的缓冲区必须包括完整的GIF流内容
/	参数filesize是指明p_filecontent所指向的缓冲区的尺寸（即GIF流的尺寸）
/	成功返回0，失败返回-1，图像数据有些不正规，但已成功读取，返回1
/	注：如果是一个动态GIF（图像个数大于1），图像的宽、高数据将以
/		逻辑屏的宽、高为准，如果是静态GIF（单幅图像），则将以图像
/		的宽、高数据为准。
*/
int CALLAGREEMENT _get_imginfo(unsigned char *p_filecontent, unsigned long filesize, LPGIFINSIDEINFO p_info)
{
	unsigned char *	p = p_filecontent;
	int				type = 0, i;
	unsigned char	data_len;
	int				color_num;
	int				first_img_mark = 0;
	unsigned long	len = 0;

	GIFHEADER		gifHeader;
	IMAGEDATAINFO	image_data;	/* 图像数据块 */
	
	
	assert(p_filecontent);
	assert(p_info);
	
	memcpy(&gifHeader,p,sizeof(GIFHEADER));

	if (!memcmp((const void *)gifHeader.gif_type, (const char *)GIF_MARKER_89a, strlen((const char *)GIF_MARKER_89a)))
		type = 1;
	else if (!memcmp((const void *)gifHeader.gif_type, (const char *)GIF_MARKER_87a, strlen((const char *)GIF_MARKER_87a)))
		type = 2;
	else
		return -1;			/* 非法图像数据 */

	/* 填写返回数据 */
	p_info->type = type;
	p_info->scr_width = (unsigned long)gifHeader.scr_width;
	p_info->scr_height= (unsigned long)gifHeader.scr_height;
	p_info->bk_index  = (int)gifHeader.bg_color;
	
	if ((p_info->scr_width==0)||(p_info->scr_height==0))
		return -1;			/* 非法图像数据 */

	if (gifHeader.global_flag&0x80)
	{
		p_info->scr_bitcount = (unsigned long)((gifHeader.global_flag&0x7)+1);

		/* 测试背景色索引的合法性 */
		if (p_info->bk_index >= (1<<p_info->scr_bitcount))
			p_info->bk_index = (1<<p_info->scr_bitcount) - 1;	/* 非法，容错处理－等于最高索引 */
			/* return -1; 如果对源图像数据要求严格的话，可将以上 */
			/* 一行的代码用本行return -1;语句替换。*/
	}
	else
	{
		p_info->scr_bitcount = (unsigned long)0;
		p_info->bk_index = 0;
	}


	/* 跨过GIF流头及逻辑屏幕描述块 */
	p += sizeof(GIFHEADER);
	len += sizeof(GIFHEADER);
	
	/* 判断是否存在全局调色板，如果存在则提取数据并跨过该调色板 */
	if (gifHeader.global_flag&0x80)
	{
		color_num = 1 << p_info->scr_bitcount;

		if (color_num > GIF_MAXCOLOR)
			return -1;			/* 受损的图像 */

		if ((sizeof(GIFHEADER)+color_num*sizeof(GIFRGB)) >= (int)filesize)
			return -1;			/* 受损的图像 */

		len += color_num*sizeof(GIFRGB);
		
		for (i=0;i<color_num;i++)
		{
			p_info->p_pal[i].bRed	= *p++;
			p_info->p_pal[i].bGreen = *p++;
			p_info->p_pal[i].bBlue	= *p++;
		}
	}
	else
	{
		/* 如果没有全局调色板数据，则用灰度图数据初始化调色板数组 */
		for (i=0;i<GIF_MAXCOLOR;i++)
		{
			p_info->p_pal[i].bRed   = (unsigned char)i;
			p_info->p_pal[i].bGreen = (unsigned char)i;
			p_info->p_pal[i].bBlue  = (unsigned char)i;
		}
	}
	
	/* 分解数据区 */
	while (1)
	{
		if (p[0] == 0x2c)	/* 图像描述符＋[局部调色板]＋图像数据块 */
		{
			if ((len+sizeof(IMAGEDATAINFO)) >= filesize)
				return -1;				/* 受损的图像 */

			/* 跨过图像描述符结构 */
			memcpy(&image_data, p, sizeof(IMAGEDATAINFO));
			p += sizeof(IMAGEDATAINFO);
			len += sizeof(IMAGEDATAINFO);

			if (image_data.local_flag&0x80)
			{
				if ((len+(1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB)) >= filesize)
					return -1;
				else
					len += (1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB);
			}

			/* 填写首幅图像信息 */
			if (first_img_mark == 0)
			{
				p_info->first_img_width = (unsigned long)image_data.width;
				p_info->first_img_height= (unsigned long)image_data.height;
				if (image_data.local_flag&0x80)
					p_info->first_img_bitcount = (unsigned long)((image_data.local_flag&0x7)+1);
				else
					p_info->first_img_bitcount = p_info->scr_bitcount;

				if ((p_info->first_img_width==0)||(p_info->first_img_height==0))
					return -1;			/* 受损的图像 */

				p_info->img_count = 0;

				/* 提取局部调色板数据 */
				if (image_data.local_flag&0x80) 
				{
					color_num = 1 << p_info->first_img_bitcount;

					if (color_num > GIF_MAXCOLOR)
						return -1;		/* 受损的图像 */
					
					for (i=0;i<color_num;i++)
					{
						p_info->p_first_pal[i].bRed   = *p++;
						p_info->p_first_pal[i].bGreen = *p++;
						p_info->p_first_pal[i].bBlue  = *p++;
					}
				}
				else
					memmove((void*)p_info->p_first_pal, (const void *)p_info->p_pal, sizeof(GIFRGB)*GIF_MAXCOLOR);

				first_img_mark = 1;
			}
			else
			{
				/* 非首幅图像：如果拥有局部调色板数据，则跨过 */
				if (image_data.local_flag&0x80)
					p += (1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB);
			}

			if ((len+1) >= filesize)
				return -1;

			p++;				/* 越过GIF-LZW最小编码值 */
			len++;
			data_len = 1;
			
			/* 跨过经压缩的图像数据子块 */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* 受损的图像流 */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
			/* 图像个数加1 */
			p_info->img_count++;
		}
		else if (p[0] == 0x21 && p[1] == 0xf9 && p[2] == 0x4)	/* 图形控制扩展块 */
		{
			if ((len+sizeof(GRAPHCTRL)) >= filesize)
				return -1;	/* 受损的图像流 */

			p += sizeof(GRAPHCTRL);
			len += sizeof(GRAPHCTRL);
		}
		else if (p[0] == 0x21 && p[1] == 0x1 && p[2] == 0xc)	/* 文本扩展块 */
		{
			if ((len+sizeof(TEXTCTRL)) >= filesize)
				return -1;	/* 受损的图像流 */

			p += sizeof(TEXTCTRL);
			len += sizeof(TEXTCTRL);

			data_len = 1;
			
			/* 跨过文本子块序列  */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* 受损的图像流 */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xff && p[2] == 0xb)	/* 应用程序扩展块 */
		{
			if ((len+sizeof(APPCTRL)) >= filesize)
				return -1;	/* 受损的图像流 */

			p += sizeof(APPCTRL);
			len += sizeof(APPCTRL);

			data_len = 1;
			
			/* 跨过文本子块序列  */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* 受损的图像流 */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xfe)					/* 注释扩展块 */
		{
			if ((len+sizeof(NOTEHCTRL)) >= filesize)
				return -1;	/* 受损的图像流 */

			p += sizeof(NOTEHCTRL);
			len += sizeof(NOTEHCTRL);

			data_len = 1;
			
			/* 跨过文本子块序列  */
			while (data_len)
			{
				if ((len+1) >= filesize)
					return -1;	/* 受损的图像流 */

				data_len = *p++;
				p += data_len;
				len += data_len+1;
			}
		}
		else if (p[0] == 0x0)									/* 滤除多余的子块结尾符，在正规的文档中没有说明多余字块结尾符，此处为经验代码 */
		{
			p++;
			len++;
		}
		else if (p[0] == 0x3b)									/* 结束 */
			break;
		else 
			return -1;	/* 受损的图像流 */
		
		if (len >= filesize)
			return -1;	/* 受损的图像流 */
	}

	if ((len+1) != filesize)
		return 1;		/* 此图像流尾部还有数据，但并不是GIF的数据，可能 */
						/* 此图像被附加了另外的信息。不过这不影响图像的  */
						/* 正常读取，属良性错误:)，所以返回1而不是-1     */
	/* 成功返回 */
	return 0;
}


/* 将给定的LZW压缩数据解压为BMP数据，并将这些数据保存在一个新申请的子数据节点中 */
LPSUBIMGBLOCK CALLAGREEMENT _decomp_LZW_to_BMP(
	unsigned char *plzw, 			/* 图像块首地址 */
	unsigned char *stream,			/* GIF流首地址 */ 
	LPGRAPHCTRL p_imgctrl, 			/* 最近有效的图像控制扩展块 */
	unsigned long *block_size)		/* 返回图像数据块的实际尺寸 */ 
{
	LZWTABLE		str_table[LZWTABLESIZE];				/* GIF-LZW 串表 */
	unsigned char	cw_buf[LZWTABLESIZE];					/* 输出字串时使用的缓冲区 */

	LPGIFHEADER		p_header = (LPGIFHEADER)stream;			/* GIF流头结构 */
	LPIMAGEDATAINFO p_imginfo = (LPIMAGEDATAINFO)plzw;
	unsigned char   *p_data, *p;
	
	unsigned char	first_char;
	int				cw_buf_len, percw;
	int				linesize, init_bit_len;
	unsigned long	imgdata_size, imgblock_size;

	int				lzw_clear_code, lzw_eoi_code, cur_bit_len;
	int				cur_insert_pos, cur_code, old_code, old_code_mark;
	/* 目标缓冲区当前写位置相关参数 */
	int				cur_row, cur_x, cur_row_bit_count, intr, pass;
	unsigned short	code_buf;
	unsigned long	bit_count;
	
	int				warninglevel = 0;
	unsigned char   *p_lzwdata = 0;
	LPSUBIMGBLOCK	p_subimg   = 0;
	
	assert(plzw&&((*plzw) == 0x2c));
	assert(stream&&((*stream) == 'G'));
	
	__try
	{
		/* 申请一个子图像节点 */
		if ((p_subimg=_alloc_SUBIMGBLOCK(plzw, stream, p_imgctrl)) == 0)
			return 0;					/* 内存不足或图像数据非法 */
		
		assert((p_subimg->p_bit_data)&&(p_subimg->pp_line_addr));

		/* 计算图像数据偏移 */
		if (p_imginfo->local_flag&0x80)
			p_data = plzw+sizeof(IMAGEDATAINFO)+(1<<p_subimg->bitcount)*sizeof(GIFRGB);
		else
			p_data = plzw+sizeof(IMAGEDATAINFO);
		
		/* DIB行尺寸 */
		linesize = DIBSCANLINE_WIDTHBYTES(p_subimg->width*p_subimg->bitcount);

		p = p_data;

		/* 提取初始位长度值 */
		init_bit_len = (int)*p++;

		/* 获取纯LZW数据长度和LZW数据块长度 */
		_get_imgdata_size(p, &imgdata_size, &imgblock_size);

		/* 返回实际图像块（LZW块）的长度 */
		*block_size = imgblock_size;

		/* 将LZW数据提取到一个新的、连续的内存块中 */
		if ((p_lzwdata=_get_lzw_datablock(p, imgdata_size)) == 0)
		{
			_free_SUBIMGBLOCK(p_subimg);
			return 0;					/* 内存不足 */
		}


		/* 解码前的初始化工作 */
		p = p_lzwdata;

		intr = (p_imginfo->local_flag&0x40) ? 1:0;	/* 交叉标志 */
		cur_row = cur_x = cur_row_bit_count = 0;	/* 目标图像缓冲区行、列位置 */
		pass = 1;									/* 交叉通道值 */

		lzw_clear_code = 1<<init_bit_len;			/* 清除码 */
		lzw_eoi_code   = lzw_clear_code+1;			/* 终结码 */
		cur_bit_len    = init_bit_len+1;			/* 当前码长度 */
		cur_insert_pos = lzw_eoi_code+1;			/* 当前串表插入位置 */
		bit_count      = 0;							/* 从流中读取的数据位总数 */
		old_code       = 0;							/* 前缀码 */
		old_code_mark  = 0;							/* 首标志 */


		/* 开始解码 */
		while((cur_code=(int)_get_next_codeword(p, imgdata_size, bit_count/8, bit_count%8, cur_bit_len)) != lzw_eoi_code)
		{
			if (cur_code == lzw_clear_code)				/* 清除码 */
			{
				bit_count      += cur_bit_len;
				cur_bit_len    = init_bit_len+1;
				cur_insert_pos = lzw_eoi_code+1;

				old_code       = 0;
				old_code_mark  = 0;

				/* 初始化串表 */
				_init_lzw_table((LZWTABLE*)str_table, LZWTABLESIZE, init_bit_len);

				warninglevel = 0;	/* 解除警戒 */
				continue;
			}
			else if (cur_code < cur_insert_pos)			/* 存在于串表中 */
			{
				if (warninglevel)	/* 防止写串表溢出 */
				{
					_free_SUBIMGBLOCK(p_subimg); 
					p_subimg = 0;
					__leave;
				}

				/* 输出码字 */
				cw_buf_len = 0;
				percw = cur_code;

				/* 将码字对应的图像数据串存入缓冲区 */
				while(percw != -1)
				{
					/* assert(str_table[percw].used == 1); */
					cw_buf[cw_buf_len++] = (unsigned char)str_table[percw].code;
					percw = (int)str_table[percw].prefix;
				}

				/* 写入目标图像缓冲区 */
				while(cw_buf_len > 0)
				{
					if (intr)	/* 是否是交错模式 */
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0; cur_row_bit_count = 0;

							/* 交错跳进 */
							if(pass == 1)
								cur_row += 8;
							if(pass == 2)
								cur_row += 8;
							if(pass == 3)
								cur_row += 4;
							if(pass == 4)
								cur_row += 2;

							if(cur_row >= p_subimg->height)
							{
								pass++;
								cur_row = 0x10 >> pass;
							}

							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
							
						}
					}
					else
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0;
							cur_row_bit_count = 0;
							cur_row++;
							
							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
						}
					}

					/* 提出图像数据并移动到预定位置 */
					code_buf = (unsigned short)(unsigned char)(cw_buf[cw_buf_len-1]);
					code_buf &= (unsigned short)((1<<(p_subimg->bitcount))-1);
					code_buf <<= cur_row_bit_count%8;

					/* 将象素数据按紧凑位写入图像缓冲区 */
					*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8)) |= (unsigned char)code_buf;
					if (((cur_row_bit_count%8)+p_subimg->bitcount) > 8)
						*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8+1)) |= (unsigned char)(code_buf>>8);

					cur_x++;
					cur_row_bit_count += p_subimg->bitcount;

					cw_buf_len--;
				}
					
				/* 是否是第一次读入 */
				if (old_code_mark)
				{
					/* 向串表中添加新码字（存在码字情况下） */
					percw = cur_code;

					/* 取得当前码字第一个字符 */
					while(percw != -1)
					{
						/* assert(str_table[percw].used == 1); */
						first_char = (unsigned char)str_table[percw].code;
						percw = (int)str_table[percw].prefix;
					}

					/* 输出新码字到LZW串表中 */
					str_table[cur_insert_pos].used = 1;
					str_table[cur_insert_pos].code = (short)first_char;
					str_table[cur_insert_pos].prefix = old_code;

					/* 插入位置后移 */
					cur_insert_pos++;
				}
				else
					old_code_mark = 1;					/* 第一次进入 */

				old_code = cur_code;
				bit_count += cur_bit_len;
			}
			else										/* 不存在的编码 */
			{
				if (warninglevel)	/* 防止写串表溢出 */
				{
					_free_SUBIMGBLOCK(p_subimg); 
					p_subimg = 0;
					__leave;
				}

				/* 向串表中添加新码字（不存在码字情况下） */
				percw = old_code;

				/* 取得前缀码字第一个字符 */
				while(percw != -1)
				{
					/* assert(str_table[percw].used == 1); */
					first_char = (unsigned char)str_table[percw].code;
					percw = (int)str_table[percw].prefix;
				}

				/* 输出新码字 */
				str_table[cur_insert_pos].used = 1;
				str_table[cur_insert_pos].code = (short)first_char;
				str_table[cur_insert_pos].prefix = old_code;

				cur_insert_pos++;

				/* 输出码字 */
				cw_buf_len = 0;
				/* 上面刚刚生成的新码字 */
				percw = cur_insert_pos-1;
				
				/* 将码字对应的图像数据串存入缓冲区 */
				while(percw != -1)
				{
					/* assert(str_table[percw].used == 1); */
					cw_buf[cw_buf_len++] = (unsigned char)str_table[percw].code;
					percw = (int)str_table[percw].prefix;
				}
				
				while(cw_buf_len > 0)
				{
					if (intr)	/* 是否是交错模式 */
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0; cur_row_bit_count = 0;
							
							if(pass == 1)
								cur_row += 8;
							if(pass == 2)
								cur_row += 8;
							if(pass == 3)
								cur_row += 4;
							if(pass == 4)
								cur_row += 2;
							
							if(cur_row >= p_subimg->height)
							{
								pass++;
								cur_row = 0x10 >> pass;
							}

							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
						}
					}
					else
					{
						if (cur_x == p_subimg->width)
						{
							cur_x = 0;
							cur_row_bit_count = 0;
							cur_row++;

							if (cur_row >= p_subimg->height)
							{
								_free_SUBIMGBLOCK(p_subimg); 
								p_subimg = 0;
								__leave;
							}
						}
					}
					
					code_buf = (unsigned short)(cw_buf[cw_buf_len-1]);
					code_buf &= (unsigned short)((1<<(p_subimg->bitcount))-1);
					code_buf <<= cur_row_bit_count%8;
					
					/* 将象素数据按紧凑位写入图像缓冲区 */
					*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8)) |= (unsigned char)code_buf;
					if (((cur_row_bit_count%8)+p_subimg->bitcount) > 8)
						*((unsigned char *)(((unsigned char *)(p_subimg->pp_line_addr[cur_row]))+cur_row_bit_count/8+1)) |= (unsigned char)(code_buf>>8);
					
					cur_x++;
					cur_row_bit_count += p_subimg->bitcount;
					
					cw_buf_len--;
				}
				
				old_code = cur_code;
				bit_count += cur_bit_len;
			}

			if ((cur_insert_pos>>cur_bit_len) > 0)
				cur_bit_len++;

			if (cur_bit_len > 12)
			{
				cur_bit_len = 12;
				warninglevel = 1;	/* 进入警戒状态 */
			}
		}
	}
	__finally
	{
		if (AbnormalTermination())
		{
			if (p_subimg)
				_free_SUBIMGBLOCK(p_subimg);
			p_subimg = 0;
		}
		if (p_lzwdata)
			free(p_lzwdata);
		p_lzwdata = 0;
	}
	
	return p_subimg;
}


int CALLAGREEMENT _load_subimg_serial(LPINFOSTR pinfo_str, unsigned char *stream, unsigned long stream_len)
{
	unsigned char *p = stream;
	unsigned char *pimg;
	int type = 0;
	unsigned char data_len;
	unsigned long block_size;
	int			  first_img_mark = 0;

	GIFHEADER		gifHeader;
	IMAGEDATAINFO	image_data;			/* 图像数据块 */
	GRAPHCTRL		cur_craph_ctrl;		/* 当前的图像控制数据 */

	LPSUBIMGBLOCK	subimg_list = 0;	/* 子图像链表 */
	LPSUBIMGBLOCK	subimg_tail = 0;
	LPSUBIMGBLOCK	subimg_tmp  = 0;
	
	
	assert(p);
	assert(pinfo_str);
	
	memset((void*)&cur_craph_ctrl, 0, sizeof(GRAPHCTRL));
	memcpy(&gifHeader,p,sizeof(GIFHEADER));

	/* 跨过GIF流头及逻辑屏幕描述块 */
	p += sizeof(GIFHEADER);

	/* 跨过全局调色板 */
	if (gifHeader.global_flag&0x80)
		p += (1<<((gifHeader.global_flag&0x7)+1))*sizeof(GIFRGB);
	
	/* 初始化图像控制数据 */
	cur_craph_ctrl.extintr			= 0x21;
	cur_craph_ctrl.label			= 0xf9;
	cur_craph_ctrl.block_size		= 0x4;
	cur_craph_ctrl.delay_time		= 0;
	cur_craph_ctrl.field			= 0;
	cur_craph_ctrl.tran_color_index = -1;
	cur_craph_ctrl.block_terminator = 0;
	

	/* 分解数据区 */
	while (1)
	{
		if (p[0] == 0x2c)	/* 图像描述符＋[局部调色板]＋图像数据块 */
		{
			/* 保留图像描述块首地址 */
			pimg = p;

			/* 跨过图像描述符结构 */
			memcpy(&image_data,p,sizeof(IMAGEDATAINFO));
			p += sizeof(IMAGEDATAINFO);
			
			/* 如果拥有局部调色板数据，则跨过 */
			if (image_data.local_flag&0x80)
				p += (1<<((image_data.local_flag&0x7)+1))*sizeof(GIFRGB);

			/* 对子图像进行解码 */
			subimg_tmp = _decomp_LZW_to_BMP(pimg, stream, &cur_craph_ctrl, &block_size);

			/* 如果解码失败，释放子图像链表，返回错误 */
			if (subimg_tmp == 0)
			{
				if (subimg_list != 0)
				{
					while(subimg_tmp=subimg_list->next)
					{
						_free_SUBIMGBLOCK(subimg_list);
						subimg_list = subimg_tmp;
					}
					_free_SUBIMGBLOCK(subimg_list);
					subimg_list = 0;
				}
				return -1;
			}
			
			/* 将子图像加入子图像链表尾部 */
			if (subimg_list == 0)
			{				  
				subimg_list = subimg_tmp;	/* 链表头 */
				subimg_tail = subimg_tmp;	/* 链表尾 */

				subimg_tmp->prev = 0;
				subimg_tmp->next = 0;
				subimg_tmp->number = 1;
				subimg_tmp->parents = pinfo_str;
			}
			else
			{
				subimg_tmp->prev = subimg_tail;
				subimg_tmp->next = 0;
				subimg_tmp->number = subimg_tail->number+1;
				subimg_tmp->parents = pinfo_str;

				assert(subimg_tail->next == 0);

				subimg_tail->next = subimg_tmp;
				subimg_tail = subimg_tmp;
			}

			/* 图像个数加1 */
			pinfo_str->imgnumbers++;

			/* 跨过图像数据 */
			p += block_size;
		}
		else if (p[0] == 0x21 && p[1] == 0xf9 && p[2] == 0x4)	/* 图形控制扩展块 */
		{
			memcpy((void*)&cur_craph_ctrl, (const void *)p, sizeof(GRAPHCTRL));
			p += sizeof(GRAPHCTRL);
		}
		else if (p[0] == 0x21 && p[1] == 0x1 && p[2] == 0xc)	/* 文本扩展块 */
		{
			p += sizeof(TEXTCTRL);
			data_len = 1;
			
			/* 跨过文本子块序列  */
			while (data_len)
			{
				data_len = *p++;
				p += data_len;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xff && p[2] == 0xb)	/* 应用程序扩展块 */
		{
			p += sizeof(APPCTRL);
			data_len = 1;
			
			/* 跨过文本子块序列  */
			while (data_len)
			{
				data_len = *p++;
				p += data_len;
			}
		}
		else if (p[0] == 0x21 && p[1] == 0xfe)					/* 注释扩展块 */
		{
			p += sizeof(NOTEHCTRL);
			data_len = 1;
			
			/* 跨过文本子块序列  */
			while (data_len)
			{
				data_len = *p++;
				p += data_len;
			}
		}
		else if (p[0] == 0x0)									/* 滤除多余的子块结尾符，在正规的文档中没有说明多余字块结尾符，此处为经验代码 */
		{
			p++;
		}
		else if (p[0] == 0x3b)									/* 结束 */
			break;
		else 
		{
			/* 释放子图像链表 */
			if (subimg_list != 0)
			{
				while(subimg_tmp=subimg_list->next)
				{
					_free_SUBIMGBLOCK(subimg_list);
					subimg_list = subimg_tmp;
				}
				_free_SUBIMGBLOCK(subimg_list);
				subimg_list = 0;
			}

			pinfo_str->imgnumbers = 0;

			return -1;	/* 受损的图像流 */
		}
	}

	pinfo_str->psubimg = subimg_list;

	/* 成功返回 */
	return 0;
}



/* 初始化LZW串表 */
void CALLAGREEMENT _init_lzw_table(LZWTABLE *pt, int table_size, int init_bit_len)
{
	int i, len;

	assert(init_bit_len <= 12);

	memset((void*)pt, 0, sizeof(LZWTABLE)*table_size);

	len = 1<<init_bit_len;

	/* 根码 */
	for (i=0;i<len;i++)
	{
		pt[i].prefix = -1;
		pt[i].code   = (short)i;
		pt[i].used   = 1;
	}

	/* 清除码 */
	pt[len].prefix = -1;
	pt[len].code   = -1;
	pt[len].used   = 1;

	/* 结束码 */
	pt[len+1].prefix = -1;
	pt[len+1].code   = -1;
	pt[len+1].used   = 1;
}


/* 分配一个子图像节点，并初始化节点内数据 */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(
	 unsigned char *plzw, 			/* 图像块首地址 */
	 unsigned char *stream,			/* GIF流首地址 */ 
	 LPGRAPHCTRL p_imgctrl)			/* 最近有效的图像控制扩展块 */
{
	LPGIFHEADER		p_header = (LPGIFHEADER)stream;
	LPIMAGEDATAINFO p_imginfo = (LPIMAGEDATAINFO)plzw;
	LPGIFRGB		p_pal;

	int				linesize, i, colorkey;
	
	LPSUBIMGBLOCK	p_subimg = 0;


	/* 申请子图像信息节点(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* 内存不足 */
	
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	/* 设置子图像节点数据 */
	p_subimg->left = (int)p_imginfo->left;
	p_subimg->top = (int)p_imginfo->top;
	p_subimg->width = (int)p_imginfo->width;
	p_subimg->height= (int)p_imginfo->height;

	if ((p_subimg->width==0)||(p_subimg->height==0))
	{
		free(p_subimg);				/* 非法图像数据 */
		return 0;
	}

	/* 提取位计数值 */
	if (p_imginfo->local_flag&0x80)
	{
		p_subimg->bitcount = (p_imginfo->local_flag&0x7)+1;
		p_pal = (LPGIFRGB)(unsigned char *)(plzw+sizeof(IMAGEDATAINFO));
	}
	else
	{
		if (!(p_header->global_flag&0x80))
		{
			free(p_subimg);
			return 0;				/* 非法图像（既没有子图像位数也没有全局位数值）*/
		}
		else
		{
			p_subimg->bitcount = (p_header->global_flag&0x7)+1;
			p_pal = (LPGIFRGB)(unsigned char *)(p_header+1);
		}
	}
	
	/* DIB行尺寸 */
	linesize = DIBSCANLINE_WIDTHBYTES(p_subimg->width*p_subimg->bitcount);

	/* 申请目标图像缓冲区（+4 － 附加的4字节缓冲区） */
	if ((p_subimg->p_bit_data = (unsigned char *)malloc(linesize*p_subimg->height+4)) == 0)
	{
		free(p_subimg);
		return 0;					/* 内存不足 */
	}

	/* 申请行地址数组 */
	if ((p_subimg->pp_line_addr = (void**)malloc(sizeof(void*)*p_subimg->height)) == 0)
	{
		free(p_subimg->p_bit_data);
		free(p_subimg);
		return 0;					/* 内存不足 */
	}

	/* 初始化图像缓冲区 */
	memset((void*)p_subimg->p_bit_data, 0, linesize*p_subimg->height);

	/* GIF为正向图像 */
	p_subimg->order   = 0;

	for (i=0;i<p_subimg->height;i++)
		p_subimg->pp_line_addr[i] = (void *)(p_subimg->p_bit_data+(i*linesize));
	
	/* GIF为索引图，无掩码数据 */
	p_subimg->r_mask = 0;
	p_subimg->g_mask = 0;
	p_subimg->b_mask = 0;
	p_subimg->a_mask = 0;


	/* 处置方式：0 - 未指定处置方式
				 1 - 不必做任何处理动作
				 2 - 以背景色擦除
				 3 - 回复原来的背景图像 */
	p_subimg->dowith= (int)((p_imgctrl->field&0x1c)>>2);
	/* 在显示下一幅子图像前是否需要等待某一事件 */
	p_subimg->userinput = (int)((p_imgctrl->field&0x2)>>1);
	/* 延迟时间（1/1000 s）*/
	p_subimg->time = (int)p_imgctrl->delay_time;
	/* 设置透明色索引，如果没有透明色，则设为-1 */
	colorkey = (p_imgctrl->field&0x1) ? p_imgctrl->tran_color_index : -1;


	/* 复制调色板数据 */
	p_subimg->pal_count = 1 << p_subimg->bitcount;

	for (i=0;i<p_subimg->pal_count;i++)
	{
		((LPISEERGB)(&(p_subimg->palette[i])))->bRed	= p_pal[i].bRed;
		((LPISEERGB)(&(p_subimg->palette[i])))->bGreen	= p_pal[i].bGreen;
		((LPISEERGB)(&(p_subimg->palette[i])))->bBlue	= p_pal[i].bBlue;
		((LPISEERGB)(&(p_subimg->palette[i])))->rev		= 0;
	}

	if (colorkey != -1)
		p_subimg->colorkey = p_subimg->palette[colorkey];
	else
		p_subimg->colorkey = (unsigned long)-1;

	
	p_subimg->number  = 0;			/* 子图像块的序列号暂设为0 */
	
	p_subimg->parents = 0;
	p_subimg->prev    = 0;
	p_subimg->next    = 0;
	
	return p_subimg;
}


/* 释放一个子图像节点，包括其中的位缓冲器及行首地址数组 */
void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);

	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);

	if (p_node->p_bit_data)
		free(p_node->p_bit_data);

	free(p_node);
}


/* 建立背景图（针对动态图像） */
int CALLAGREEMENT _build_bkimg(LPINFOSTR pinfo_str, LPGIFINSIDEINFO p_info)
{
	int linesize, i;
	unsigned char *pline = 0;
	unsigned char *pimg  = 0;
	void **ppaddr = 0;
	unsigned char bkindex = p_info->bk_index;
	
	int byte_pos, bit_pos, bit_count;
	unsigned char c;
	
	assert((pinfo_str->width > 0)&&(pinfo_str->height > 0));
	
	linesize = DIBSCANLINE_WIDTHBYTES(pinfo_str->width*pinfo_str->bitcount);
	
	/* 分配样本行缓冲区 */
	if ((pline = (unsigned char *)malloc(linesize)) == 0)
		return -1;

	/* 初始化为0 */
	memset((void*)pline, 0, linesize);
	
	/* 分配背景图像缓冲区（+4 － 附加的4字节缓冲区） */
	if ((pimg = (unsigned char *)malloc(linesize*pinfo_str->height+4)) == 0)
	{
		free(pline);
		return -1;
	}
	
	/* 分配行首地址数组 */
	if ((ppaddr = (void**)malloc(sizeof(void*)*pinfo_str->height)) == 0)
	{
		free(pimg);
		free(pline);
		return -1;
	}
	
	/* 初始化行首地址（GIF正向） */
	for (i=0;i<(int)pinfo_str->height;i++)
		ppaddr[i] = (void*)(pimg+(i*linesize));
	
	assert(pinfo_str->bitcount <= 8);
	assert(bkindex < (1<<pinfo_str->bitcount));
	
	/* 填写样本行 */
	byte_pos = bit_pos = bit_count = 0;
	for (i=0;i<(int)pinfo_str->width;i++)
	{
		byte_pos = bit_count/8;
		bit_pos  = bit_count%8;
		
		c = (unsigned char)bkindex;
		c <<= bit_pos;
		pline[byte_pos] |= c;
		
		if ((bit_pos+pinfo_str->bitcount) > 8)
		{
			c = (unsigned char)bkindex;
			c >>= 8-bit_pos;
			pline[byte_pos+1] |= c;
		}
		
		bit_count += pinfo_str->bitcount;
	}
	
	/* 将背景图初始化为背景色的单色图 */
	for (i=0;i<(int)pinfo_str->height;i++)
		memmove((void*)ppaddr[i], (const void *)pline, linesize);
	
	
	pinfo_str->pp_line_addr = ppaddr;
	pinfo_str->p_bit_data   = pimg;
	pinfo_str->pal_count = 1<<pinfo_str->bitcount;
	
	/* 复制调色板数据 */
	for (i=0;i<(int)pinfo_str->pal_count;i++)
	{
		((LPISEERGB)(&(pinfo_str->palette[i])))->bRed   = p_info->p_pal[i].bRed;
		((LPISEERGB)(&(pinfo_str->palette[i])))->bGreen = p_info->p_pal[i].bGreen;
		((LPISEERGB)(&(pinfo_str->palette[i])))->bBlue  = p_info->p_pal[i].bBlue;
		((LPISEERGB)(&(pinfo_str->palette[i])))->rev    = 0;
	}
	
	free(pline);
	
	return 0;
}


/* 计算图像位数据的尺寸（包括纯图像数据尺寸和图像数据块的整体尺寸）
/		
/	注：块尺寸将包括首部的初始位置的字节。
*/
void CALLAGREEMENT _get_imgdata_size(
	unsigned char *stream,			/* 指向图像数据块首地址的指针（已跨过了初始位长度字节）*/
	unsigned long *imgdata_size, 
	unsigned long *imgblock_size)
{
	unsigned char *p = stream;
	unsigned long data_count, block_count;
	unsigned char data_len;

	assert(p);

	data_count = block_count = 0UL;

	while(1)
	{
		data_len = *p++;				/* 提取长度描述字节值，并后移指针 */
		p += data_len;
		data_count += (unsigned long)data_len;
		block_count += ((unsigned long)data_len+1);
										/* 加1是附带了数据子块的长度字节 */
		if (data_len == 0) 
			break;
	}

	block_count++;						/* 跨过LZW初始位值的一个字节 */
	
	*imgdata_size  = data_count;
	*imgblock_size = block_count;
}


/* 将GIF中的纯LZW图像数据提取到新的内存块中 
/
/	如果内存分配失败，返回0
*/
unsigned char * CALLAGREEMENT _get_lzw_datablock(
	unsigned char *stream,				/* 图像块首地址（已跨过初始位长度字节）*/
	unsigned long lzw_len)
{
	unsigned char *p1, *p2, *pdata, data_len;
	unsigned long count = 0UL;

	assert(stream);
	
	/* 分配内存块，用于存放纯LZW数据 */
	if ((pdata = (unsigned char *)malloc(lzw_len+1)) == (unsigned char *)0)
		return (unsigned char *)0;
	
	p1 = stream;
	p2 = pdata;
	
	while (1)
	{
		data_len = *p1++;
		if (data_len == 0x00) 
			break;

		/* 防止错误数据干扰程序 */
		count += (unsigned long)data_len;
		if (count > lzw_len)
		{
			free(pdata);
			return (unsigned char *)0;
		}

		/* 将LZW数据复制到新分配的内存块中 */
		memcpy(p2, p1, data_len);

		p1 += data_len;
		p2 += data_len;
	}

	assert(count == lzw_len);
	
	return pdata;
}


/* 取得LZW中下一个编码字  */
unsigned short CALLAGREEMENT _get_next_codeword(
	unsigned char *code_stream,			/* LZW数据流首地址 */
	unsigned long code_stream_len,		/* 流长度 */
	unsigned long offset_byte,			/* 当前码所在字节偏移 */
	unsigned long offset_bits,			/* 当前码所在位偏移 */
	unsigned long code_word_bit_size)	/* 码字位长度 */
{
	CODEWORD	cw;
	LPCODEWORD	p_code_word;

	assert(code_word_bit_size <= 12);
	
	p_code_word = (LPCODEWORD)(unsigned char *)(code_stream + offset_byte);

	/* 12位，取3个字节即可 */
	if ((offset_byte+0) < code_stream_len)
		cw.bcode[0] = p_code_word->bcode[0];
	if ((offset_byte+1) < code_stream_len)
		cw.bcode[1] = p_code_word->bcode[1];
	if ((offset_byte+2) < code_stream_len)
		cw.bcode[2] = p_code_word->bcode[2];
	
	/* 将数据移至字低端 */
	cw.lcode >>= offset_bits;
	cw.lcode  &= ((1<<code_word_bit_size)-1);
	
	return (unsigned short)cw.lcode;
}


