/********************************************************************

	ani.c

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
	本文件用途：	ISee图像浏览器—ANI图像读写模块实现文件

					读取功能：1、4、8、16、24、32位ANI图象
							  
					保存功能：不支持
							   

	本文件编写人：	
					YZ		yzfree##yeah.net

	本文件版本：	20330
	最后修改于：	2002-3-30

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-3		发布第一个版本（新版，支持ISeeIO系统）


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

#include "ani.h"


IRWP_INFO			ani_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	ani_get_info_critical;	/* ani_get_image_info函数的关键段 */
CRITICAL_SECTION	ani_load_img_critical;	/* ani_load_image函数的关键段 */
CRITICAL_SECTION	ani_save_img_critical;	/* ani_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);

int CALLAGREEMENT _is_ani(unsigned char *pani);
int CALLAGREEMENT _get_info(unsigned char *pani, LPINFOSTR pinfo_str);
int CALLAGREEMENT _get_image(unsigned char *pani, LPINFOSTR pinfo_str);
int CALLAGREEMENT _str_to_value(unsigned char *pstr);
unsigned char * CALLAGREEMENT _jump_and_jump_and_jump(unsigned char *ptag);
int CALLAGREEMENT _ani_compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand);
int CALLAGREEMENT _ani_conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark);

static LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void);
static void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node);

static int CALLAGREEMENT _get_mask(int bitcount, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
static int CALLAGREEMENT _set_rate(int index, unsigned long rate, LPINFOSTR pinfo_str);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&ani_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&ani_get_info_critical);
			InitializeCriticalSection(&ani_load_img_critical);
			InitializeCriticalSection(&ani_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&ani_get_info_critical);
			DeleteCriticalSection(&ani_load_img_critical);
			DeleteCriticalSection(&ani_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

ANI_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&ani_irwp_info;
}

#else

ANI_API LPIRWP_INFO CALLAGREEMENT ani_get_plugin_info()
{
	_init_irwp_info(&ani_irwp_info);

	return (LPIRWP_INFO)&ani_irwp_info;
}

ANI_API void CALLAGREEMENT ani_init_plugin()
{
	/* 初始化多线程同步对象 */
}

ANI_API void CALLAGREEMENT ani_detach_plugin()
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


	/* 功能标识 */
	lpirwp_info->irwp_function = IRWP_READ_SUPP;

	/* 设置模块支持的保存位深 */
	lpirwp_info->irwp_save.bitcount = 0;
	lpirwp_info->irwp_save.img_num = 0;
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
				(const char *)"he he and he he ...");
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

	/* 别名个数（##需手动修正，如ANI格式有别名请参考BMP插件中别名的设置） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
ANI_API int CALLAGREEMENT ani_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned char	*pani  = 0, *p;

	unsigned long	ani_len;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&ani_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 获得ANI数据流长度 */
			if ((ani_len=isio_length(pfile)) < (FORMTYPESEIZE*4))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 定位于文件头 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 申请内存以存放完整的ANI图象数据（+8是为了防止解码函数读操作越界） */
			if ((pani=(unsigned char *)malloc(ani_len+8)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;	/* 内存不足 */
			}

			memset((void*)(pani+ani_len), 0, 8);

			/* 读取完整的ANI数据到内存中 */
			if (isio_read((void*)pani, ani_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 判断是否是ANI图象 */
			if (_is_ani(pani) != 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 定位于数据区（跨过RIFF标志首部） */
			p = (unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)+FORMTYPESEIZE);
			
			/* 提取第一幅光标的概要信息 */
			switch (_get_info(p, pinfo_str))
			{
			case	-1:		/* 光标文件受损 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	0:		/* 成功 */
				break;
			default:
				b_status = ER_SYSERR;    __leave; break;
			}
			
			pinfo_str->imgtype		= IMT_RESDYN;		/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_ANI;			/* 图像文件格式（后缀名） */
			pinfo_str->compression	= ICS_RGB;			/* 图像的压缩方式 */

			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pani)
				free(pani);

			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&ani_get_info_critical);
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
ANI_API int CALLAGREEMENT ani_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;
	unsigned char	*pani  = 0, *p;
	unsigned long	ani_len;

	LPSUBIMGBLOCK	subimg_tmp;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&ani_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 允许用户中断 */
			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}

			/* 获得ANI数据流长度 */
			if ((ani_len=isio_length(pfile)) < (FORMTYPESEIZE*4))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 定位于文件头 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 申请内存以存放完整的ANI图象数据（+8是为了防止解码函数读操作越界） */
			if ((pani=(unsigned char *)malloc(ani_len+8)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;	/* 内存不足 */
			}
			
			memset((void*)(pani+ani_len), 0, 8);
			
			/* 读取完整的ANI数据到内存中 */
			if (isio_read((void*)pani, ani_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断是否是ANI图象 */
			if (_is_ani(pani) != 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 定位于数据区（跨过RIFF标志首部） */
			p = (unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)+FORMTYPESEIZE);
			
			pinfo_str->process_total   = 1;
			pinfo_str->process_current = 0;
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 提取第一幅光标的概要信息 */
				switch (_get_info(p, pinfo_str))
				{
				case	-1:		/* 光标文件受损 */
					b_status = ER_BADIMAGE;  __leave; break;
				case	0:		/* 成功 */
					break;
				default:
					b_status = ER_SYSERR;    __leave; break;
				}
				
				pinfo_str->imgtype		= IMT_RESDYN;		/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_ANI;			/* 图像文件格式（后缀名） */
				pinfo_str->compression	= ICS_RGB;			/* 图像的压缩方式 */
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}
			
			/* 读取图象数据 */
			switch (_get_image(p, pinfo_str))
			{
			case	0:		/* 成功 */
				break;
			case	-1:		/* 受损的图象 */
				b_status = ER_BADIMAGE;  __leave; break;
			case	-3:		/* 内存不足 */
				b_status = ER_MEMORYERR; __leave; break;
			case	-4:		/* 用户中断 */
				b_status = ER_USERBREAK; __leave; break;
			case	-2:		/* 系统异常 */
			default:
				assert(0);
				b_status = ER_SYSERR;    __leave; break;
			}
			

			/* 结束操作 */
			pinfo_str->process_current = pinfo_str->process_total;

			pinfo_str->data_state = 2;
		}
		__finally
		{
			if ((b_status != ER_SUCCESS)||(AbnormalTermination()))
			{
				/* 释放子图像 */
				if (pinfo_str->psubimg != 0)
				{
					while(subimg_tmp=pinfo_str->psubimg->next)
					{
						_free_SUBIMGBLOCK(pinfo_str->psubimg);
						pinfo_str->psubimg = subimg_tmp;
					}
					_free_SUBIMGBLOCK(pinfo_str->psubimg);
					pinfo_str->psubimg = 0;
				}

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
				if (pinfo_str->play_order)
				{
					free(pinfo_str->play_order);
					pinfo_str->play_order = (unsigned long *)0;
				}

				if (pinfo_str->data_state == 2)
					pinfo_str->data_state =1;	/* 自动降级 */

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			}

			if (pani)
				free(pani);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&ani_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
ANI_API int CALLAGREEMENT ani_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&ani_save_img_critical);

			/* ################ 请在下面编写保存图像的代码 ################ */
			
			
			b_status = ER_NOTSUPPORT;	/* 不支持保存功能 */			
			
			
			/* ############################################################ */
		}
		__finally
		{
			LeaveCriticalSection(&ani_save_img_critical);
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


/* 判断指定的数据流是否是有效的ANI数据 */
int CALLAGREEMENT _is_ani(unsigned char *pani)
{
	assert(pani);

	if ( (memcmp((const char *)pani, (const char *)RIFF_STR, FORMTYPESEIZE)==0) && \
		 (memcmp((const char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), (const char *)ACON_STR, FORMTYPESEIZE)==0) )
	{
		return 0;	/* 是ANI数据流 */
	}
	else
	{
		return -1;	/* 非ANI数据 */
	}
}


/* 提取首幅光标的概要信息 */
int CALLAGREEMENT _get_info(unsigned char *pani, LPINFOSTR pinfo_str)
{
	int		mark = 0;

	ANICHUNK			type;
	ANIHEADER			aniheader;
	LPANI_CURDIR		pdirect;
	LPBITMAPINFOHEADER	pbmi;


	assert((pani!=0)&&(pinfo_str!=0));

	while ((type=_str_to_value(pani)) != ANICHUNK_OUT)
	{
		switch (type)
		{
		case	ANICHUNK_RIFF:
			return -1;	/* 非法数据。pani已指向RIFF块内部，所以不应再出现RIFF块标志 */
		case	ANICHUNK_LIST:
			pani += FORMTYPESEIZE + sizeof(unsigned long) + FORMTYPESEIZE;
			break;
		case	ANICHUNK_INAM:
		case	ANICHUNK_IART:
		case	ANICHUNK_RATE:
		case	ANICHUNK_SEQ:
		case	ANICHUNK_UNKONW:
			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ANIH:
			if ((mark & 0x2) != 0)
				return -1;			/* 只能存在一个ANIH块 */
			else
				mark |= 0x2;

			memcpy((void*)&aniheader, (const void *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), sizeof(ANIHEADER));

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ICON:
			if ((mark & 0x1) == 0)	/* 只提取第一幅光标图象信息 */
			{
				/* 这里可能是光标数据，也可能是图标数据，但我们不使用这些数据，而是 */
				/* 直接读取后面BMPINFOHEADER中的参数。所以不做区分 */
				pdirect = (LPANI_CURDIR)(unsigned char *)(pani + FORMTYPESEIZE + sizeof(unsigned long));
				pbmi = (LPBITMAPINFOHEADER)(unsigned char *)(((unsigned char *)pdirect) + pdirect->entries[0].image_offset);
				
				if (pbmi->biSize != sizeof(BITMAPINFOHEADER))
					return -1;		/* 非法数据 */

				/* 提取第一幅图像的信息填入数据包主帧 */
				pinfo_str->width	= (unsigned long)pbmi->biWidth;
				pinfo_str->height	= (unsigned long)pbmi->biHeight/2;	/* XOR和AND 图总高度 */
				pinfo_str->order	= 1;	/* 总是倒向图 */
				pinfo_str->bitcount = (unsigned long)pbmi->biBitCount;
				
				if ((pinfo_str->width == 0)||(pinfo_str->height == 0))
					return -1;

				if (_get_mask(pinfo_str->bitcount, 
					(unsigned long *)(&pinfo_str->b_mask), 
					(unsigned long *)(&pinfo_str->g_mask), 
					(unsigned long *)(&pinfo_str->r_mask),
					(unsigned long *)(&pinfo_str->a_mask)) == -1)
				{
					return -1;
				}

				mark |= 0x1;	/* 不再提取后续图象信息 */
			}
			pani = _jump_and_jump_and_jump(pani);
			break;
		default:
			assert(0);
			return -2;		/* 系统异常 */
			break;
		}
	}

	if (mark == 3)
		return 0;	/* 成功 */
	else
		return -1;	/* 图象数据受损 */
}


/* 提取光标数据 */
int CALLAGREEMENT _get_image(unsigned char *pani, LPINFOSTR pinfo_str)
{
	int				mark = 0, imgcount = 0, i, imgnum = 0;
	unsigned long	seqcount, ratecount;
	unsigned long	*rate = 0;		/* 速率表 */

	ANICHUNK			type;
	ANIHEADER			aniheader;
	LPANI_CURDIR		pdirect;
	LPBITMAPINFOHEADER	pbmi;
	LPSUBIMGBLOCK		lpsub = 0, lplast = 0;
	
	
	assert((pani!=0)&&(pinfo_str!=0));
	
	while ((type=_str_to_value(pani)) != ANICHUNK_OUT)
	{
		switch (type)
		{
		case	ANICHUNK_RIFF:
			if (rate)
				free(rate);
			return -1;	/* 非法数据。pani已指向RIFF块内部，所以不应再出现RIFF块标志 */
		case	ANICHUNK_LIST:
			pani += FORMTYPESEIZE + sizeof(unsigned long) + FORMTYPESEIZE;
			break;
		case	ANICHUNK_RATE:
			ratecount = *(unsigned long *)(unsigned char *)(pani+FORMTYPESEIZE);
			/* 分配播放速率表 */
			rate = (unsigned long *)malloc(ratecount);

			if (rate == 0)
			{
				return -3;
			}

			/* 复制表内容 */
			memcpy((void*)rate, (const void *)(unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), ratecount);

			/* 转换为元素单位 */
			ratecount /= sizeof(unsigned long);

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_SEQ:
			seqcount = *(unsigned long *)(unsigned char *)(pani+FORMTYPESEIZE);
			/* 分配播放顺序表 */
			pinfo_str->play_order = (unsigned long *)malloc(seqcount);

			if (pinfo_str->play_order == 0)
			{
				if (rate)
					free(rate);
				return -3;			/* 内存不足 */
			}

			/* 复制表内容 */
			memcpy((void*)pinfo_str->play_order, (const void *)(unsigned char *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), seqcount);

			/* 转换为元素单位 */
			seqcount /= sizeof(unsigned long);
			/* 播放表中元素个数 */
			pinfo_str->play_number = seqcount;

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_INAM:		/* 暂时忽略图象描述信息 */
		case	ANICHUNK_IART:
		case	ANICHUNK_UNKONW:
			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ANIH:
			if ((mark & 0x2) != 0)
			{
				if (rate)
					free(rate);
				return -1;			/* 只能存在一个ANIH块 */
			}
			else
			{
				mark |= 0x2;
			}

			memcpy((void*)&aniheader, (const void *)(pani+FORMTYPESEIZE+sizeof(unsigned long)), sizeof(ANIHEADER));

			if ((aniheader.fl & 0x1) == 0)
			{
				if (rate)
					free(rate);
				return -1;			/* 本插件将不处理以裸数据（RAW）方式存储图象的ANI */
			}

			/* 设定步进参数 */
			pinfo_str->process_total   = aniheader.cFrames;
			pinfo_str->process_current = 0;

			pani = _jump_and_jump_and_jump(pani);
			break;
		case	ANICHUNK_ICON:
			if ((mark & 0x2) == 0)
			{
				if (rate)
					free(rate);
				
				return -1;			/* anih 块必须在ICON块的前面，否则视为非法数据 */
			}

			pdirect = (LPANI_CURDIR)(unsigned char *)(pani + FORMTYPESEIZE + sizeof(unsigned long));
			pbmi = (LPBITMAPINFOHEADER)(unsigned char *)(((unsigned char *)pdirect) + pdirect->entries[0].image_offset);
			
			if (pbmi->biSize != sizeof(BITMAPINFOHEADER))
			{
				if (rate)
					free(rate);
				
				return -1;		/* 非法数据 */
			}

			imgcount = pdirect->count;

			if (imgcount < 1)	/* 至少应有一幅图象 */
			{
				if (rate)
					free(rate);
				
				return -1;
			}

			if ((mark & 0x1) == 0)	/* 提取首帧光标图象 */
			{
				switch (_ani_conv_image_block(pbmi, (void *)pinfo_str, ANI_CUR_PRI_IMAGE))
				{
				case	-2:		/* 光标文件受损 */
					if (rate)
						free(rate);
					return -1;
				case	-3:		/* 内存不足 */
					if (rate)
						free(rate);
					return -3;
				case	0:		/* 成功 */
					break;
				default:
					if (rate)
						free(rate);
					return -2;	/* 系统异常 */
				}

				imgcount--;		/* 单个光标块中图象个数计数 */
				imgnum = 0;		/* 总图象个数计数 */
				lplast = 0;
				mark |= 0x1;
			}
				
			/* 支持用户中断 */
			if (pinfo_str->break_mark)
			{
				if (rate)
					free(rate);
				return -4;	/* 用户中断 */
			}

			/* 读入子图象 */
			while (imgcount)
			{
				if ((lpsub=_alloc_SUBIMGBLOCK()) == 0)
				{
					if (rate)
						free(rate);
					return -3;
				}
				
				lpsub->number = ++imgnum;	/* 子图像序号由 1 开始 */
				lpsub->parents = pinfo_str;
				
				/* 加入子图象链表尾部 */
				if (lplast == 0)
				{
					pinfo_str->psubimg = lpsub;	/* 首幅子图象 */
					lplast = lpsub;
					
					lpsub->prev = 0;
					lpsub->next = 0;
				}
				else
				{
					lpsub->prev = lplast;		/* 后续子图象 */
					lpsub->next = 0;
					lplast->next = lpsub;
					lplast = lpsub;
				}

				/* 计算图象数据地址 */
				pbmi = (LPBITMAPINFOHEADER)(unsigned char *)(((unsigned char *)pdirect) + pdirect->entries[pdirect->count-imgcount].image_offset);
				
				/* 转换子图象到数据包中 */
				switch (_ani_conv_image_block(pbmi, (void *)lpsub, ANI_CUR_SUB_IMAGE))
				{
				case	-2:		/* 光标文件受损 */
					if (rate)
						free(rate);
					return -1;
				case	-3:		/* 内存不足 */
					if (rate)
						free(rate);
					return -3;
				case	0:		/* 成功 */
					break;
				default:
					if (rate)
						free(rate);
					return -2;
				}
				
				/* 支持用户中断 */
				if (pinfo_str->break_mark)
				{
					if (rate)
						free(rate);
					return -4;
				}

				imgcount--;
			}

			/* 累加步进值。（保留最后一步给SEQ和RATE块的读取）*/
			if ((pinfo_str->process_current+1) != pinfo_str->process_total)
				pinfo_str->process_current++;
			
			pani = _jump_and_jump_and_jump(pani);
			break;
		default:
			assert(0);
			if (rate)
				free(rate);
			return -2;		/* 系统异常 */
			break;
		}
	}
	
	if (mark == 3)
	{
		pinfo_str->imgnumbers = imgnum+1;	/* 图象总个数（包括主帧图象）*/

		/* 设置每帧图象的显示延迟时间 */
		if (rate)
		{
			if (pinfo_str->play_order)
			{
				for (i=0;i<(int)pinfo_str->play_number;i++)
				{
					if (_set_rate((int)pinfo_str->play_order[i], rate[i], pinfo_str) != 0)
					{
						free(rate);
						return -1;
					}
				}
			}
			else
			{
				for (i=0;i<(int)pinfo_str->imgnumbers;i++)
				{
					if (_set_rate((int)i, rate[i], pinfo_str) != 0)
					{
						free(rate);
						return -1;
					}
				}
			}
		}
		else
		{
			for (i=0;i<(int)pinfo_str->imgnumbers;i++)
			{
				if (_set_rate((int)i, aniheader.jifRate, pinfo_str) != 0)
				{
					return -1;
				}
			}
		}

		pinfo_str->colorkey = -1;

		if (rate)
			free(rate);

		return 0;	/* 成功 */
	}
	else
	{
		if (rate)
			free(rate);
		return -1;	/* 图象数据受损 */
	}
}


/* 转换块标示串为标示符 */
int CALLAGREEMENT _str_to_value(unsigned char *pstr)
{
	assert(pstr);

	if (memcmp((const char *)pstr, (const char *)ICON_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_ICON;
	else if (memcmp((const char *)pstr, (const char *)LIST_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_LIST;
	else if (memcmp((const char *)pstr, (const char *)ANIH_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_ANIH;
	else if (memcmp((const char *)pstr, (const char *)RATE_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_RATE;
	else if (memcmp((const char *)pstr, (const char *)SEQ_STR , FORMTYPESEIZE) == 0)
		return ANICHUNK_SEQ;
	else if (memcmp((const char *)pstr, (const char *)RIFF_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_RIFF;
	else if (memcmp((const char *)pstr, (const char *)INAM_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_INAM;
	else if (memcmp((const char *)pstr, (const char *)IART_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_IART;
	else if (memcmp((const char *)pstr, (const char *)NULL_STR, FORMTYPESEIZE) == 0)
		return ANICHUNK_OUT;	/* 超出有效数据范围（到达数据尾部） */
	else
		return ANICHUNK_UNKONW;	/* 未知的RIFF块标志 */
}


/* 跳过一个块 */
unsigned char * CALLAGREEMENT _jump_and_jump_and_jump(unsigned char *ptag)
{
	int size;
	
	assert(ptag);
	
	size = (int)*(unsigned long *)(unsigned char *)(ptag+FORMTYPESEIZE);
	
	size += size%2;		/* 块尺寸如果是奇数，其后追加一个字节（RIFF格式规定） */
	
	return (unsigned char *)(ptag + FORMTYPESEIZE + sizeof(unsigned long) + size);
}


/* 获取各颜色分量的值 */
int CALLAGREEMENT _get_mask(int bitcount, 
							unsigned long * pb_mask, 
							unsigned long * pg_mask, 
							unsigned long * pr_mask, 
							unsigned long * pa_mask)
{
	int result = 0;
	
	switch(bitcount)
	{
	case	1:
	case	4:
	case	8:
		/* 调色板位图无掩码 */
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	case	16:		/* 555 格式 */
		*pr_mask = 0x7c00;		*pg_mask = 0x3e0;
		*pb_mask = 0x1f;		*pa_mask = 0;
		break;
	case	24:
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	case	32:		/* 888 格式 */
		*pr_mask  = 0xff0000;		*pg_mask  = 0xff00;
		*pb_mask  = 0xff;	 		*pa_mask  = 0x0;
		break;
	default:		/* 非法格式 */
		result = -1;
		*pr_mask = 0;		*pg_mask = 0;
		*pb_mask = 0;		*pa_mask = 0;
		break;
	}
	
	return result;	/* 0-成功， -1－非法的图像格式 */
}


/* 设置图象显示延迟参数（只供_get_image函数调用） */
int CALLAGREEMENT _set_rate(int index, unsigned long rate, LPINFOSTR pinfo_str)
{
	LPSUBIMGBLOCK	lpsub;
	
	if (index >= (int)pinfo_str->imgnumbers)
	{
		return -1;		/* 非法的数据 */
	}
	
	if (index == 0)		/* 首帧图象 */
	{
		pinfo_str->time = FPS_TO_MS(rate);
	}
	else				/* 子图象 */
	{
		for (lpsub=pinfo_str->psubimg; lpsub != 0; lpsub=lpsub->next)
		{
			if (lpsub->number != index)
				continue;
			else
			{
				lpsub->time = FPS_TO_MS(rate);
				break;
			}
		}
		assert(lpsub);	/* 不可能出现找不到对应子图象的情况 */
	}
	
	return 0;
}


/* 分配一个子图像节点，并初始化节点内数据 */
LPSUBIMGBLOCK CALLAGREEMENT _alloc_SUBIMGBLOCK(void)
{
	LPSUBIMGBLOCK	p_subimg = 0;
	
	/* 申请子图像信息节点(SUBIMGBLOCK) */
	if ((p_subimg=(LPSUBIMGBLOCK)malloc(sizeof(SUBIMGBLOCK))) == 0)
		return 0;					/* 内存不足 */
	
	/* 初始化为 0 */
	memset((void*)p_subimg, 0, sizeof(SUBIMGBLOCK));

	p_subimg->number = 1;
	p_subimg->colorkey = -1;
	
	return p_subimg;
}


/* 释放一个子图像节点，包括其中的位缓冲区及行首地址数组 */
void CALLAGREEMENT _free_SUBIMGBLOCK(LPSUBIMGBLOCK p_node)
{
	assert(p_node);
	assert(p_node->number > 0);
	
	if (p_node->pp_line_addr)
		free(p_node->pp_line_addr);
	
	if (p_node->p_bit_data)
		free(p_node->p_bit_data);
	
	free(p_node);
}


/* 将光标数据块转为数据包块 */
int CALLAGREEMENT _ani_conv_image_block(LPBITMAPINFOHEADER lpbmi, void *lpdest, enum CUR_DATA_TYPE mark)
{
	LPINFOSTR		pinfo = (LPINFOSTR)0;
	LPSUBIMGBLOCK	psubinfo = (LPSUBIMGBLOCK)0;
	LPBITMAPINFO	pbi = (LPBITMAPINFO)lpbmi;
	
	unsigned char	*p, *pxor, *pand;
	unsigned long	pal_num = 0;
	int				i, linesize, linesize2;

	assert((lpbmi)&&(lpdest));
	assert((mark == ANI_CUR_PRI_IMAGE)||(mark == ANI_CUR_SUB_IMAGE));

	if (mark == ANI_CUR_PRI_IMAGE)
	{
		pinfo = (LPINFOSTR)lpdest;

		/* 设置调色板数据 */
		if (lpbmi->biBitCount <= 8)
		{
			pinfo->pal_count = 1UL << pinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? pinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > pinfo->pal_count)
			{
				return -2;		/* 图象受损 */
			}
			memmove((void*)(pinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			pinfo->pal_count = 0;
		}

		/* XOR象素扫描行尺寸 */
		linesize  = _calcu_scanline_size(pinfo->width, pinfo->bitcount);
		/* AND象素扫描行尺寸 */
		linesize2 = _calcu_scanline_size(pinfo->width, 1);
		
		assert(pinfo->p_bit_data == (unsigned char *)0);
		
		/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
		pinfo->p_bit_data = (unsigned char *)malloc(linesize * pinfo->height + 4);
		if (!pinfo->p_bit_data)
		{						
			return -3;			/* 内存不足 */
		}
		
		assert(pinfo->pp_line_addr == (void**)0);
		/* 分配行首地址数组 */
		pinfo->pp_line_addr = (void **)malloc(sizeof(void*) * pinfo->height);
		if (!pinfo->pp_line_addr)
		{
			free(pinfo->p_bit_data); 
			pinfo->p_bit_data = 0; 
			return -3;			/* 内存不足 */
		}

		/* 初始化行首地址数组（倒向） */
		for (i=0;i<(int)(pinfo->height);i++)
		{
			pinfo->pp_line_addr[i] = (void *)(pinfo->p_bit_data+((pinfo->height-i-1)*linesize));
		}
		
		/* 计算XOR象素数据首地址 */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);

		/* 复制XOR象素数据 */
		memmove((void*)pinfo->p_bit_data, (const void *)p, (linesize*pinfo->height));

		/* 取得 XOR 象素首地址 */
		pxor = (unsigned char *)pinfo->p_bit_data;
		/* 计算 AND 象素首地址 */
		pand = p + linesize * pinfo->height;

		/* 合成图象 */
		for (i=0;i<(int)pinfo->height;i++)
		{
			if (_ani_compose_img(pinfo->bitcount, pinfo->width, pxor, pand) != 0)
			{
				free(pinfo->p_bit_data); 
				pinfo->p_bit_data = 0; 
				free(pinfo->pp_line_addr);
				pinfo->pp_line_addr = 0;

				return -2;		/* 受损的图象 */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}
	else
	{
		psubinfo = (LPSUBIMGBLOCK)lpdest;

		/* 填写图像信息 */
		psubinfo->width		= (int)lpbmi->biWidth;
		psubinfo->height	= (int)lpbmi->biHeight/2;	/* XOR和AND 图总高度 */
		psubinfo->order		= 1;	/* 总是倒向图 */
		psubinfo->bitcount	= (int)lpbmi->biBitCount;

		if (_get_mask(psubinfo->bitcount, 
			(unsigned long *)(&psubinfo->b_mask), 
			(unsigned long *)(&psubinfo->g_mask), 
			(unsigned long *)(&psubinfo->r_mask),
			(unsigned long *)(&psubinfo->a_mask)) == -1)
		{
			return -2;		/* 图象受损 */
		}

		psubinfo->left = psubinfo->top = 0;
		psubinfo->dowith =psubinfo->userinput = psubinfo->time = 0;
		psubinfo->colorkey = -1;	/* 定为无关键色 */

		/* 设置调色板数据 */
		if (lpbmi->biBitCount <= 8)
		{
			psubinfo->pal_count = 1UL << psubinfo->bitcount;
			
			pal_num = (lpbmi->biClrUsed == 0) ? psubinfo->pal_count : lpbmi->biClrUsed;
			if (pal_num > (unsigned long)psubinfo->pal_count)
			{
				return -2;		/* 图象受损 */
			}
			memmove((void*)(psubinfo->palette), (const void *)(pbi->bmiColors), sizeof(RGBQUAD)*pal_num);
		}
		else
		{
			psubinfo->pal_count = 0;
		}

		
		/* 取得XOR象素扫描行尺寸 */
		linesize  = _calcu_scanline_size(psubinfo->width, psubinfo->bitcount);
		/* 计算AND象素扫描行尺寸 */
		linesize2 = _calcu_scanline_size(psubinfo->width, 1);
		
		assert(psubinfo->p_bit_data == (unsigned char *)0);
		
		/* 分配目标图像内存块（+4 － 尾部附加4字节缓冲区） */
		psubinfo->p_bit_data = (unsigned char *)malloc(linesize * psubinfo->height + 4);
		if (!psubinfo->p_bit_data)
		{						
			return -3;			/* 内存不足 */
		}
		
		assert(psubinfo->pp_line_addr == (void**)0);
		/* 分配行首地址数组 */
		psubinfo->pp_line_addr = (void **)malloc(sizeof(void*) * psubinfo->height);
		if (!psubinfo->pp_line_addr)
		{
			free(psubinfo->p_bit_data); 
			psubinfo->p_bit_data = 0; 
			return -3;			/* 内存不足 */
		}
		
		/* 初始化行首地址数组（倒向） */
		for (i=0;i<(int)(psubinfo->height);i++)
		{
			psubinfo->pp_line_addr[i] = (void *)(psubinfo->p_bit_data+((psubinfo->height-i-1)*linesize));
		}
		
		/* 计算象素数据首地址 */
		p = (unsigned char *)lpbmi;
		p += lpbmi->biSize;
		p += pal_num*sizeof(RGBQUAD);
		
		/* 复制象素数据 */
		memmove((void*)psubinfo->p_bit_data, (const void *)p, (linesize*psubinfo->height));
		
		/* 计算 XOR 象素首地址 */
		pxor = (unsigned char *)psubinfo->p_bit_data;
		/* 计算 AND 象素首地址 */
		pand = p + linesize * psubinfo->height;
		
		/* 合成图象（使用XOR数据与AND数据）*/
		for (i=0;i<psubinfo->height;i++)
		{
			if (_ani_compose_img(psubinfo->bitcount, psubinfo->width, pxor, pand) != 0)
			{
				free(psubinfo->p_bit_data); 
				psubinfo->p_bit_data = 0;
				free(psubinfo->pp_line_addr);
				psubinfo->pp_line_addr = 0;

				return -2;		/* 受损的图象 */
			}
			pxor += linesize;
			pand += linesize2;
		}
	}

	return 0;		/* 返回 0 成功，非 0 失败 */
}


/* 合成一个扫描行的XOR图和AND图，并进行ISee位流格式转换，结果保存在XOR图中 */
int CALLAGREEMENT _ani_compose_img(int bitcount, int width, unsigned char *pxor, unsigned char *pand)
{
	unsigned char buf[32], tmp, *p = pxor, isc = 0;
	int i, j, k, l, a, bytesize;

	assert(bitcount > 0);
	assert(pxor&&pand);

	/* 一次处理 8 个象素 */
	for (i=0, k=0;i<width;i+=8, k++)
	{
		/* 取得 8 个象素的AND数据（共 1 个字节） */
		tmp = *(pand+k);

		/* 将AND数据转为XOR象素格式 */
		switch (bitcount)
		{
		case	1:
			buf[0] = tmp;
			bytesize = 1;	/* 实际要操作的字节数 */
			break;
		case	4:
			for (l=0;l<8;l+=2)
			{
				buf[l/2]  = ((tmp>>(7-l))&0x1) ? 0xf0 : 0x0;
				buf[l/2] |= ((tmp>>(7-l-1))&0x1) ? 0xf : 0x0;
			}
			bytesize = 4;
			break;
		case	8:
			for (l=0;l<8;l++)
			{
				buf[l] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
			}
			bytesize = ((width-i) < 8) ? (width-i) : 8;
			break;
		case	16:
			for (l=0;l<8;l++)
			{
				buf[l*2] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*2+1] = buf[l*2]&0x7f;		/* 555格式 */
			}
			bytesize = ((width-i) < 8) ? (width-i)*2 : 16;
			break;
		case	24:
			for (l=0;l<8;l++)
			{
				buf[l*3] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*3+1] = buf[l*3];
				buf[l*3+2] = buf[l*3];
			}
			bytesize = ((width-i) < 8) ? (width-i)*3 : 24;
			break;
		case	32:
			for (l=0;l<8;l++)
			{
				buf[l*4] = ((tmp>>(7-l))&0x1) ? 0xff : 0x0;
				buf[l*4+1] = buf[l*4];
				buf[l*4+2] = buf[l*4];
				buf[l*4+3] = 0;					/* 888格式 */
			}
			bytesize = ((width-i) < 8) ? (width-i)*4 : 32;
			break;
		default:
			return -1;							/* 不支持的位深度 */
			break;
		}

		/* 合成图象（一次合成 8 个象素）*/
		for (j=0;j<bytesize;j++)
		{
			*p++ ^= buf[j];
		}

		isc = 0;

		/* 将 1 位深图象转为ISee位流格式 */
		if (bitcount == 1)
		{
			for (a=0;a<8;a++)
			{
				isc |= (((*(p-1))>>(7-a))&0x1)<<a;
			}
			*(p-1) = isc;
		}

		/* 将 4 位深图象转为ISee位流格式 */
		if (bitcount == 4)
		{
			for (a=0;a<4;a++)
			{
				isc  = (*(p-4+a))>>4;
				isc |= (*(p-4+a))<<4;
				*(p-4+a) = isc;
			}
		}
	}

	return 0;	/* 返回 0 表示成功，返回非 0 值表示失败 */
}


