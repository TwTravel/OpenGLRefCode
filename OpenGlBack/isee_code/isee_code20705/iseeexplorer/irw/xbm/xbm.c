/********************************************************************

	xbm.c

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
	本文件用途：	ISee图像浏览器—XBM图像读写模块实现文件

			读取功能：X10、X11格式XBM图像（对注释信息具有
						容错性）。
							  
			保存功能：X11格式XBM
							   

	本文件编写人：	YZ			yzfree##yeah.net

	本文件版本：	20113
	最后修改于：	2002-1-13

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-1		第一次发布（新版本，起始版本号2.0）


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

#include "xbm.h"


#ifdef WIN32
#define MAX_PATH_LEN	_MAX_PATH
#define MAX_DRIVE_LEN	_MAX_DRIVE
#define MAX_DIR_LEN		_MAX_DIR
#define MAX_FNAME_LEN	_MAX_FNAME
#define MAX_EXT_LEN		_MAX_EXT
#else
#define MAX_PATH_LEN	512				/* 应修改为Linux中的相应值 */
#define MAX_DRIVE_LEN	32
#define MAX_DIR_LEN		512
#define MAX_FNAME_LEN	512
#define MAX_EXT_LEN		512
#endif


IRWP_INFO			 xbm_irwp_info;			/* 插件信息表 */

static int			 hex_table[256];		/* 字符转数字表 */
static unsigned char rwbuff[512];			/* 读写缓冲区 */


#ifdef WIN32
CRITICAL_SECTION	xbm_get_info_critical;	/* xbm_get_image_info函数的关键段 */
CRITICAL_SECTION	xbm_load_img_critical;	/* xbm_load_image函数的关键段 */
CRITICAL_SECTION	xbm_save_img_critical;	/* xbm_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _xbm_is_valid_img(LPINFOSTR pinfo_str);

void CALLAGREEMENT _init_hex_table(void);
int CALLAGREEMENT _read_integer(ISFILE	*fp);
int CALLAGREEMENT _read_until_data(ISFILE *fp);
int CALLAGREEMENT _read_line(ISFILE *fp, unsigned char *buff);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&xbm_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&xbm_get_info_critical);
			InitializeCriticalSection(&xbm_load_img_critical);
			InitializeCriticalSection(&xbm_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&xbm_get_info_critical);
			DeleteCriticalSection(&xbm_load_img_critical);
			DeleteCriticalSection(&xbm_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

XBM_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&xbm_irwp_info;
}

#else

XBM_API LPIRWP_INFO CALLAGREEMENT xbm_get_plugin_info()
{
	_init_irwp_info(&xbm_irwp_info);

	return (LPIRWP_INFO)&xbm_irwp_info;
}

XBM_API void CALLAGREEMENT xbm_init_plugin()
{
	/* 初始化多线程同步对象 */
}

XBM_API void CALLAGREEMENT xbm_detach_plugin()
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
	/* 各位数减一，否则32位格式无法表示。*/
	lpirwp_info->irwp_save.bitcount = 1UL;
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
				(const char *)"呵呵，XBM这个小东西还满有意思的:)");
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

	/* 别名个数（##需手动修正） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
XBM_API int CALLAGREEMENT xbm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256];
	unsigned int	w, h;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&xbm_get_info_critical);

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

			/* 去除文件首部多余的空格、注释等字符 */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 读入一行数据 */
			if (_read_line(pfile, rwbuff) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 提取图像宽度值 */
			if ((sscanf(rwbuff, "#define %s %u", agb_name, &w) == 2) && 
				(strlen(agb_name) >= 6) &&
				(strcmp(agb_name+strlen(agb_name)-6, "_width") == 0))
			{
				pinfo_str->width = (unsigned long)w;
			}
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}


			/* 读入一行数据 */
			if (_read_line(pfile, rwbuff) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 提取图像高度值 */
			if ((sscanf(rwbuff, "#define %s %u", agb_name, &h) == 2) && 
				(strlen(agb_name) >= 7) &&
				(strcmp(agb_name+strlen(agb_name)-7, "_height") == 0))
			{
				pinfo_str->height = (unsigned long)h;
			}
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}

			if ((w == 0)||(h == 0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_XBM;			/* 图像文件格式（后缀名） */
			pinfo_str->compression  = ICS_XBM_TEXT;
			
			pinfo_str->order		= 0;	/* 正向 */
			pinfo_str->bitcount		= 1;	/* XBM为单色图，位深为 1 */
			
			pinfo_str->b_mask = 0;			/* 索引图，无掩码数据 */
			pinfo_str->g_mask = 0;
			pinfo_str->r_mask = 0;
			pinfo_str->a_mask = 0;
				
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&xbm_get_info_critical);
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
XBM_API int CALLAGREEMENT xbm_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256], *p;
	unsigned int	w, h, version;
	int				i, linesize, result, j;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&xbm_load_img_critical);

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

			/* 读文件定位 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 去除文件首部多余的空格、注释等字符 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 读入一行数据 */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 提取图像宽度值 */
				if ((sscanf(rwbuff, "#define %s %u", agb_name, &w) == 2) && 
					(strlen(agb_name) >= 6) &&
					(strcmp(agb_name+strlen(agb_name)-6, "_width") == 0))
				{
					pinfo_str->width = (unsigned long)w;
				}
				else
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				
				/* 读入一行数据 */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 提取图像高度值 */
				if ((sscanf(rwbuff, "#define %s %u", agb_name, &h) == 2) && 
					(strlen(agb_name) >= 7) &&
					(strcmp(agb_name+strlen(agb_name)-7, "_height") == 0))
				{
					pinfo_str->height = (unsigned long)h;
				}
				else
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* 检查图像宽、高数据是否合法 */
				if ((w == 0)||(h == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_XBM;			/* 图像文件格式（后缀名） */
				pinfo_str->compression  = ICS_XBM_TEXT;
				
				pinfo_str->order		= 0;	/* 正向 */
				pinfo_str->bitcount		= 1;	/* XBM为单色图，位深为 1 */
				
				pinfo_str->b_mask = 0;			/* 索引图，无掩码数据 */
				pinfo_str->g_mask = 0;
				pinfo_str->r_mask = 0;
				pinfo_str->a_mask = 0;

				pinfo_str->data_state = 1;		/* 设置标记 */
			}
			else
			{
				/* 跨过文件宽高信息 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			/* 跨过热点信息行 */
			while(1)
			{
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				if (sscanf(rwbuff, "#define %s %u", agb_name, &w) == 2)
					continue;
				else
					break;
			}
					
			/* 设置调色板数据 */
			pinfo_str->pal_count = 2;
			/* 实际位数据为1，索引为0，所以0索引为白色，1索引为黑色 */
			pinfo_str->palette[0] = 0xffffffUL;
			pinfo_str->palette[1] = 0UL;

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
			
			/* 初始化行首数组 */			
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			
			
			version = 11;		/* 缺省：X11 XBM 格式 */

			/* 判断XBM版本 */
			while (1)
			{
				if (sscanf(rwbuff, "static short %s = {", agb_name) == 1)
					version = 10;
				else
				{
					if (sscanf(rwbuff, "static unsigned char %s = {", agb_name) == 1)
						version = 11;
					else
						if (sscanf(rwbuff, "static char %s = {", agb_name) == 1)
							version = 11;
						else
							continue;
				}

				p = (unsigned char *)strrchr(agb_name, '_');

				/* 检查_bits[]标记 */
				if (p == (unsigned char *)0)
					p = (unsigned char *)agb_name;
				else
					p++;

				if (strcmp((const char *)"bits[]",(char *)p) == 0)
					break;

				/* 读入一行数据 */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}
			
			/* 初始化转换值表 */
			_init_hex_table();

			if (version == 11)
			{
				/* 逐行读入 */
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					for (j=0;j<(int)(pinfo_str->width);j+=8)
					{
						result = _read_integer(pfile);
						if (result == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}
						*(unsigned char *)(pinfo_str->p_bit_data+(i*linesize)+j/8) = (unsigned char)result;
					}

					pinfo_str->process_current = i+1;
					
					/* 支持用户中断 */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
			}
			else
			{
				assert(version == 10);
				/* 逐行读入 */
				for (i=0;i<(int)(pinfo_str->height);i++)
				{
					for (j=0;j<(int)(pinfo_str->width);j+=16)
					{
						result = _read_integer(pfile);
						if (result == -1)
						{
							b_status = ER_FILERWERR; __leave;
						}
						*(unsigned short *)(pinfo_str->p_bit_data+(i*linesize)+j/8) = (unsigned short)result;
					}
					
					pinfo_str->process_current = i+1;
					
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

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xbm_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
XBM_API int CALLAGREEMENT xbm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	static char		path_buffer[MAX_PATH_LEN];
	static char		drive[MAX_DRIVE_LEN];
	static char		fname[MAX_FNAME_LEN];
	static char		ext[MAX_EXT_LEN];
	static char		dir[MAX_DIR_LEN];

	static char		buffer[MAX_PATH_LEN];

	int				i, j, linesize;
	unsigned char	*p;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&xbm_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_xbm_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 分解出文件名，以确定XBM中宽、高变量的名字 */
			if (psct->mark[1] == 'F')
				strcpy(path_buffer, (const char *)psct->name);	/* 文件 */
			else
				strcpy(path_buffer, (const char *)"temp_name");	/* 其它介质 */

			_splitpath(path_buffer, drive, dir, fname, ext);
			
			/* 排除文件名中多余的'.'字符（该字符不能用于变量名）*/
			p = (unsigned char *)strchr((const char *)fname, (int)'.');
			if (p)
				*p = 0;


			/* 定位到文件首部 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
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

			/* 写入宽、高变量描述 */
			sprintf(buffer, "#define %s_width %u\n", fname, pinfo_str->width);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			sprintf(buffer, "#define %s_height %u\n", fname, pinfo_str->height);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 注：本模块将不保存热点信息 */

			/* 保存格式为X11 */
			sprintf(buffer, "static unsigned char %s_bits[] = {\n", fname);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			strcpy(buffer, (const char *)"   ");
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			assert(pinfo_str->bitcount == 1);

			linesize = (pinfo_str->width+7)/8;

			/* 写入图像数据 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				p = (unsigned char *)(pinfo_str->pp_line_addr[i]);

				for (j=0;j<linesize;j++)
				{
					/* 结尾数据判断 */
					if (((i+1) == (int)(pinfo_str->height))&&((j+1) == linesize))
					{
						sprintf(buffer, "0x%02x };", (unsigned char)(*p++));
						if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
					}
					else
					{
						sprintf(buffer, "0x%02x, ", (unsigned char)(*p++));
						if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
					}
				}

				strcpy(buffer, "\n   ");
				if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 累加步进值 */
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
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xbm_save_img_critical);
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


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _xbm_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(xbm_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (xbm_irwp_info.irwp_save.img_num)
		if (xbm_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */

	return 0;
}


/* 初始化字符值表 */
void CALLAGREEMENT _init_hex_table()
{
	int i;

	for (i=(int)'0';i<=(int)'9';i++)
		hex_table[i] = i - (int)'0';

	for (i=(int)'a';i<=(int)'f';i++)
		hex_table[i] = i - (int)'a' + 10;

	for (i=(int)'A';i<=(int)'F';i++)
		hex_table[i] = i - (int)'A' + 10;

	hex_table['x'] = 0;
	hex_table['X'] = 0;

	hex_table[' '] = -1;
	hex_table[','] = -1;
	hex_table['}'] = -1;
	hex_table['\n'] = -1;
	hex_table['\t'] = -1;
}


/* 读取一个整数 */
int CALLAGREEMENT _read_integer(ISFILE *fp)
{
	int	c, flag = 0, value = 0;
	
	while (1)
	{
		if (isio_read((void*)&c, 1, 1, fp) == -1)		
			return -1;								/* 读错误或文件结束 */
		
		c &= 0xff;

		if (isxdigit(c))
		{
			value = (value << 4) + hex_table[c];
			flag++;
			continue;
		}

		if ((hex_table[c]<0) && flag)		/* ','和空格的表值小于0 */
			break;
	}

	return value;
}


/* 跨过无用字符（包括注释信息），直到有效数据位置 */
int CALLAGREEMENT _read_until_data(ISFILE *fp)
{
	char buff[4];
	long ioloc;
	int  wanlev;

	while(1)
	{
		if (isio_read((void*)buff, 1, 1, fp) == -1)		
			return -1;								/* 读错误或文件结束 */

		if (buff[0] == ' ')
			continue;
		else if (buff[0] == '\t')
			continue;
		else if (buff[0] == '\n')
			continue;
		else if (buff[0] == '\r')
			continue;
		else if (buff[0] == '/')
		{
			if (isio_read((void*)buff, 1, 1, fp) == -1)		
				return -1;
			if (buff[0] == '*')				/* 注释信息开始 */
			{
				wanlev = 0;
				while (1)					/* 跨过注释信息 */
				{
					if (isio_read((void*)buff, 1, 1, fp) == -1)		
						return -1;
					if (buff[0] == '*')
						wanlev = 1;
					else
					{
						if (buff[0] != '/')
							wanlev = 0;
						else if ((buff[0] == '/')&&(wanlev == 1))
							break;
						continue;
					}
				}
			}
			else
			{
				ioloc = isio_tell(fp);		/* 流读写位置后退一字节 */
				ioloc --;
				if (isio_seek(fp, ioloc, SEEK_SET) == -1)
					return -1;
			}
			continue;
		}
		else
		{
			ioloc = isio_tell(fp);		/* 流读写位置后退一字节 */
			ioloc --;
			if (isio_seek(fp, ioloc, SEEK_SET) == -1)
				return -1;
			break;
		}
	}

	return 0;
}


/* 读一行数据 */
int CALLAGREEMENT _read_line(ISFILE *fp, unsigned char *buff)
{
	int i = 0;

	while(1)
	{
		if (isio_read((void*)&(buff[i]), 1, 1, fp) == -1)		
			return -1;								/* 读错误或文件结束 */
		
		if (buff[i] == '\r')
			continue;
		else if (buff[i] == '\n')					/* 去掉/r/n符号 */
		{
			buff[i++] = (unsigned char)0;
			break;
		}
		else
			i++;
	}

	return i;
}
