/********************************************************************

	xpm.c

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
	本文件用途：	ISee图像浏览器—XPM图像读写模块实现文件

					读取功能：XPM v3格式图像（对注释信息具有容错性）。
					保存功能：1、4、8位图像（注：不保存热点信息）

	本文件编写人：	YZ			yzfree##yeah.net

	本文件版本：	20125
	最后修改于：	2002-1-25

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

#include "xpm.h"
#include "rgbtab.h"

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


IRWP_INFO			 xpm_irwp_info;			/* 插件信息表 */
#define CTSIZE		 16						/* 颜色转换表，用于保存功能 */
static char			 ct1[17] = " *.o#+@O$81ugcms";
static unsigned short clbuff[256];			/* 颜色描述符表 */
static unsigned char  rwbuff[512];			/* 读写缓冲区 */


#ifdef WIN32
CRITICAL_SECTION	xpm_get_info_critical;	/* xpm_get_image_info函数的关键段 */
CRITICAL_SECTION	xpm_load_img_critical;	/* xpm_load_image函数的关键段 */
CRITICAL_SECTION	xpm_save_img_critical;	/* xpm_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _xpm_is_valid_img(LPINFOSTR pinfo_str);

int CALLAGREEMENT _read_until_data(ISFILE *fp);
int CALLAGREEMENT _read_line(ISFILE *fp, char *buff);
int CALLAGREEMENT _skip_spec(ISFILE *fp);
int CALLAGREEMENT _distill_comm(ISFILE *fp, char *comm, int commsize);
int CALLAGREEMENT _read_color_line(LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp);
int CALLAGREEMENT _parse_color_line(char *line, int cpp, char *colname, unsigned long *rgb);
int CALLAGREEMENT _read_pix_line(unsigned long *pixbuf, int width, int bitcount, LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&xpm_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&xpm_get_info_critical);
			InitializeCriticalSection(&xpm_load_img_critical);
			InitializeCriticalSection(&xpm_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&xpm_get_info_critical);
			DeleteCriticalSection(&xpm_load_img_critical);
			DeleteCriticalSection(&xpm_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

XPM_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&xpm_irwp_info;
}

#else

XPM_API LPIRWP_INFO CALLAGREEMENT xpm_get_plugin_info()
{
	_init_irwp_info(&xpm_irwp_info);

	return (LPIRWP_INFO)&xpm_irwp_info;
}

XPM_API void CALLAGREEMENT xpm_init_plugin()
{
	/* 初始化多线程同步对象 */
}

XPM_API void CALLAGREEMENT xpm_detach_plugin()
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
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(4-1)) | (1UL<<(8-1));
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
				(const char *)"XPM 插件做起来还挺麻烦:)");
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
XPM_API int CALLAGREEMENT xpm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256];
	int				w, h, cpp, ncol, i;
	char			*p;
	LPXPMCOLOR		pcol = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&xpm_get_info_critical);

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

			/* 去除文件首部多余的空格字符 */
			if (_skip_spec(pfile) != 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 提取注释信息 */
			if (_distill_comm(pfile, rwbuff, 512) < 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 检查XPM文件首部标志 */
			if (strcmp(rwbuff, XPM_MARK) != 0)
			{
				b_status = ER_NONIMAGE; __leave;
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

			/* 提取图像数据变量名 */
			if (rwbuff[0] == 's')
			{
				if (sscanf(rwbuff, "static char * %s[] = {", agb_name) != 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else if (rwbuff[0] == 'c')
			{
				if (sscanf(rwbuff, "char * %s[] = {", agb_name) != 1)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			else
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 清除可能携带的[]符号 */
			p = strrchr(agb_name, (int)'[');
			if (p)
				*p = '\0';

			/* 去除多余的空格、注释等字符 */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 读入图像宽、高等数据 */
			if (_read_line(pfile, rwbuff) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 提取图像宽、高、CPP及颜色值 */
			if (sscanf(rwbuff, "\"%d %d %d %d", &w, &h, &ncol, &cpp) != 4)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 图像数据合法性检查 */
			/* 注：当CPP值大于7时本模块认为它是非法参数，因为在现实世界中
					不应该会有这样的图像CPP值。*/
			if ((w == 0)||(h == 0)||(cpp == 0)||(cpp > 7)||(ncol == 0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 去除多余的空格、注释等字符 */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 申请图像颜色表 */
			pcol = (LPXPMCOLOR)malloc(sizeof(XPMCOLOR)*ncol);
			if (!pcol)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			/* 读取颜色信息 */
			if (_read_color_line(pcol, ncol, cpp, pfile) < 0)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 填写图像信息 */
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_XPM;			/* 图像文件格式（后缀名） */
			pinfo_str->compression  = ICS_XPM_TEXT;
			
			pinfo_str->width		= w;
			pinfo_str->height		= h;
			pinfo_str->order		= 0;	/* 正向 */


			/* 搜索是否有透明色 */
			for (i=0;i<ncol;i++)
				if (pcol[i].attrib == 1)
					break;

			if (i < ncol)						/* 有透明色则将图像转为32位，以容纳透明色 */
			{
				pinfo_str->bitcount = 32;

				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0xff000000;
			}
			else
			{
				if (ncol <= 2)
					pinfo_str->bitcount	= 1;
				else if (ncol <= 16)
					pinfo_str->bitcount	= 4;
				else if (ncol <= 256)
					pinfo_str->bitcount	= 8;
				else
					pinfo_str->bitcount	= 32;

				if (pinfo_str->bitcount == 32)	/* 无透明色32位图像 */
				{
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0x0;
				}
				else							/* 调色板图像 */
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
			if (AbnormalTermination())
				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			if (pcol)
				free(pcol);
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&xpm_get_info_critical);
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
XPM_API int CALLAGREEMENT xpm_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	unsigned char	agb_name[256];
	int				w, h, cpp, ncol;
	int				i, linesize, result;
	char			*p;
	LPXPMCOLOR		pcol = 0;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&xpm_load_img_critical);

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
				/* 去除文件首部多余的空格字符 */
				if (_skip_spec(pfile) != 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 提取注释信息 */
				if (_distill_comm(pfile, rwbuff, 512) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				/* 检查XPM文件首部标志 */
				if (strcmp(rwbuff, XPM_MARK) != 0)
				{
					b_status = ER_NONIMAGE; __leave;
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

				/* 提取图像数据变量名 */
				if (rwbuff[0] == 's')
				{
					if (sscanf(rwbuff, "static char * %s[] = {", agb_name) != 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else if (rwbuff[0] == 'c')
				{
					if (sscanf(rwbuff, "char * %s[] = {", agb_name) != 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
				}
				else
				{
					b_status = ER_NONIMAGE; __leave;
				}

				/* 清除可能携带的[]符号 */
				p = strrchr(agb_name, (int)'[');
				if (p)
					*p = '\0';

				/* 去除多余的空格、注释等字符 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 读入一行数据 */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 提取图像宽、高、CPP及颜色值 */
				if (sscanf(rwbuff, "\"%d %d %d %d", &w, &h, &ncol, &cpp) != 4)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				/* 图像数据合法性检查 */
				/* 注：当CPP值大于7时本模块认为它是非法参数，因为在现实世界中
						没有这样的图像CPP值。*/
				if ((w == 0)||(h == 0)||(cpp == 0)||(cpp > 7)||(ncol == 0))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				/* 去除多余的空格、注释等字符 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				
				/* 申请图像颜色表 */
				pcol = (LPXPMCOLOR)malloc(sizeof(XPMCOLOR)*ncol);
				if (!pcol)
				{
					b_status = ER_MEMORYERR; __leave;
				}
				
				/* 读取颜色信息 */
				if (_read_color_line(pcol, ncol, cpp, pfile) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}

				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_XPM;			/* 图像文件格式（后缀名） */
				pinfo_str->compression  = ICS_XPM_TEXT;
				
				pinfo_str->width		= w;
				pinfo_str->height		= h;
				pinfo_str->order		= 0;	/* 正向 */
				
				/* 搜索是否有透明色 */
				for (i=0;i<ncol;i++)
					if (pcol[i].attrib == 1)
						break;
					
				if (i < ncol)						/* 有透明色则将图像转为32位，以容纳透明色 */
				{
					pinfo_str->bitcount = 32;
					
					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0xff000000;
				}
				else
				{
					if (ncol <= 2)
						pinfo_str->bitcount	= 1;
					else if (ncol <= 16)
						pinfo_str->bitcount	= 4;
					else if (ncol <= 256)
						pinfo_str->bitcount	= 8;
					else
						pinfo_str->bitcount	= 32;
					
					if (pinfo_str->bitcount == 32)	/* 无透明色32位图像 */
					{
						pinfo_str->b_mask = 0xff;
						pinfo_str->g_mask = 0xff00;
						pinfo_str->r_mask = 0xff0000;
						pinfo_str->a_mask = 0x0;
					}
					else							/* 调色板图像 */
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
			else
			{
				/* 去除文件首部多余的空格字符 */
				if (_skip_spec(pfile) != 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 提取注释信息 */
				if (_distill_comm(pfile, rwbuff, 512) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				/* 去除文件首部多余的空格、注释等字符 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 读入变量名定义行 */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 去除多余的空格、注释等字符 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 读入图像宽、高、CPP等数据 */
				if (_read_line(pfile, rwbuff) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 提取图像宽、高、CPP及颜色值 */
				if (sscanf(rwbuff, "\"%d %d %d %d", &w, &h, &ncol, &cpp) != 4)
				{
					b_status = ER_NONIMAGE; __leave;
				}
				/* 宽、高等数据的合法性已经判断过了，所以此处将不再判断 */

				/* 去除多余的空格、注释等字符 */
				if (_read_until_data(pfile) == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				/* 申请图像颜色表 */
				pcol = (LPXPMCOLOR)malloc(sizeof(XPMCOLOR)*ncol);
				if (!pcol)
				{
					b_status = ER_MEMORYERR; __leave;
				}
				/* 读取颜色信息 */
				if (_read_color_line(pcol, ncol, cpp, pfile) < 0)
				{
					b_status = ER_NONIMAGE; __leave;
				}
			}
			
			/* 去除多余的空格、注释等字符，定位到象素数据处 */
			if (_read_until_data(pfile) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 设置调色板数据 */
			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->pal_count = 2;
				break;
			case	4:
				pinfo_str->pal_count = 16;
				break;
			case	8:
				pinfo_str->pal_count = 256;
				break;
			default:
				pinfo_str->pal_count = 0;
				break;
			}

			for (i=0;i<ncol;i++)
				pinfo_str->palette[i] = pcol[i].rgb;


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
			
			
			/* 逐行读入 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				result = _read_pix_line((unsigned long *)pinfo_str->pp_line_addr[i], (int)pinfo_str->width, (int)pinfo_str->bitcount, pcol, ncol, cpp, pfile);

				if (result == -1)
				{
					b_status = ER_FILERWERR; __leave;
				}
				else if ((result == -2)||(result == -3))
				{
					b_status = ER_BADIMAGE; __leave;
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

			if (pcol)
				free(pcol);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xpm_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
XPM_API int CALLAGREEMENT xpm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	static char		path_buffer[MAX_PATH_LEN];
	static char		drive[MAX_DRIVE_LEN];
	static char		fname[MAX_FNAME_LEN];
	static char		ext[MAX_EXT_LEN];
	static char		dir[MAX_DIR_LEN];

	static char		buffer[MAX_PATH_LEN];

	int				i, j, k, linesize, ncol, cpp, ctloc0, ctloc1;
	unsigned char	*p, tmpc;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&xpm_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_xpm_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 分解出文件名，以确定XPM中宽、高变量的名字 */
			if (psct->mark[1] == 'F')
				strcpy(path_buffer, (const char *)psct->name);	/* 文件 */
			else
				strcpy(path_buffer, (const char *)"image");	/* 其它介质 */

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

			/* 写入文件首部标志串 */
			sprintf(buffer, "%s\n", XPM_MARK);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 写入变量名描述 */
			sprintf(buffer, "static char * %s_xpm[] = {\n", fname);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 只保存调色板图像，因为颜色数目过多的图像不适合用XPM格式保存 */
			assert(pinfo_str->pal_count <= 256);

			ncol = pinfo_str->pal_count;		/* 取得颜色数 */
			cpp  = (ncol <= CTSIZE) ? 1 : 2;	/* 确定描述符宽度 */

			/* 写入宽、高等变量的描述 */
			sprintf(buffer, "\"%d %d %d %d\",\n", pinfo_str->width, pinfo_str->height, ncol, cpp);
			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 写入颜色数据 */
			for (i=0;i<ncol;i++)
			{
				ctloc0 = i/CTSIZE;
				ctloc1 = i%CTSIZE;

				if (cpp == 1)
				{
					sprintf(buffer, "\"%c c #%06X\",\n", ct1[ctloc1], pinfo_str->palette[i]);
					if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				else	/* cpp == 2 */
				{
					sprintf(buffer, "\"%c%c c #%06X\",\n", ct1[ctloc0], ct1[ctloc1], pinfo_str->palette[i]);
					if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
			}

			/* 取得扫描行尺寸 */
			linesize = _calcu_scanline_size(pinfo_str->width, pinfo_str->bitcount);

			/* 写入图像数据 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				p = (unsigned char *)(pinfo_str->pp_line_addr[i]);

				/* 写入前导引号 */
				buffer[0] = '\"'; buffer[1] = '\0';
				if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 写入图像数据行 */
				switch (pinfo_str->bitcount)
				{
				case	1:
					assert(cpp == 1);		/* 单色图CPP值必定为1 */
					for (j=0;j<(int)pinfo_str->width;j+=8)
					{
						tmpc = *p++;
						for (k=0;((k<8)&&((j+k)<(int)pinfo_str->width));k++)
						{
							if ((tmpc>>k)&1)
								buffer[0] = ct1[1];
							else
								buffer[0] = ct1[0];
							buffer[1] = 0;
							if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;
							}
						}
					}
					break;
				case	4:
					assert(cpp == 1);
					for (j=0;j<(int)pinfo_str->width;j+=2)
					{
						tmpc = *p++;
						for (k=0;((k<2)&&((j+k)<(int)pinfo_str->width));k++)
						{
							buffer[0] = ct1[((tmpc>>(k*4))&0xf)];
							buffer[1] = 0;
							if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;
							}
						}
					}
					break;
				case	8:					/* 8位深图像CPP值为2 */
					assert(cpp == 2);
					for (j=0;j<(int)pinfo_str->width;j++)
					{
						tmpc = *p++;
						buffer[0] = ct1[tmpc/CTSIZE];
						buffer[1] = ct1[tmpc%CTSIZE];
						buffer[2] = 0;
						if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
					}
					break;
				default:
					assert(0);		/* 系统不稳定 */
					b_status = ER_SYSERR; 
					__leave;
					break;
				}

				/* 结尾数据判断 */
				if ((i+1) == (int)(pinfo_str->height))
				{
					buffer[0] = '\"'; buffer[1] = '}'; buffer[2] = ';'; buffer[3] = '\n'; buffer[4] = 0;
				}
				else
				{
					buffer[0] = '\"'; buffer[1] = ','; buffer[2] = '\n'; buffer[3] = 0;
				}
				/* 写入后缀引号 */
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
			if (AbnormalTermination())
				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&xpm_save_img_critical);
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
int CALLAGREEMENT _xpm_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(xpm_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (xpm_irwp_info.irwp_save.img_num)
		if (xpm_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */

	return 0;
}


/* 跨过空白字符（包括空格、TAB符、回车换行符） */
int CALLAGREEMENT _skip_spec(ISFILE *fp)
{
	char buff[4];
	long ioloc;
	
	while(1)
	{
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;					/* 读错误或文件结束 */
		
		if (buff[0] == ' ')
			continue;
		else if (buff[0] == '\t')
			continue;
		else if (buff[0] == '\n')
			continue;
		else if (buff[0] == '\r')
			continue;
		else
		{
			ioloc = isio_tell(fp);		/* 流读写位置后退一字节 */
			ioloc --;
			if (isio_seek(fp, ioloc, SEEK_SET) == -1)
				return -1;
			break;
		}
	}

	return 0;	/* 成功 */
}


/* 提取注释信息 */
int CALLAGREEMENT _distill_comm(ISFILE *fp, char *comm, int commsize)
{
	char buff[4];
	long ioloc;
	int  wanlev, count;
	
	if (commsize < 5)
		return -3;						/* 给定缓冲区过小，起码需要5各字节 */

	if (isio_read((void*)buff, 2, 1, fp) == 0)
		return -1;					/* 读错误或文件结束 */
	
	buff[2] = 0;

	if (strcmp(buff, "/*"))
	{
		ioloc = isio_tell(fp);		/* 流读写位置后退2字节 */
		ioloc -= 2;
		if (isio_seek(fp, ioloc, SEEK_SET) == -1)
			return -1;
		
		return -2;					/* 当前位置不是注释信息 */
	}
	
	strcpy(comm, buff);

	wanlev = 0; count = 2;

	while (1)						/* 跨过注释信息 */
	{
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;

		comm[count++] = buff[0];
		
		if (count == commsize)
			return -4;				/* 注释信息过长，已超出给定缓冲区尺寸 */

		if (buff[0] == '*')
			wanlev = 1;
		else
		{
			if (buff[0] != '/')
				wanlev = 0;
			else if (wanlev == 1)
				break;
			continue;
		}
	}

	comm[count] = 0;
	
	return count;	/* 成功 */
}


/* 跨过无用字符（包括注释信息），直到有效数据位置 */
int CALLAGREEMENT _read_until_data(ISFILE *fp)
{
	char buff[4];
	long ioloc;
	int  wanlev;

	while(1)
	{
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;						/* 读错误或文件结束 */

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
			if (isio_read((void*)buff, 1, 1, fp) == 0)
				return -1;
			if (buff[0] == '*')				/* 注释信息开始 */
			{
				wanlev = 0;
				while (1)					/* 跨过注释信息 */
				{
					if (isio_read((void*)buff, 1, 1, fp) == 0)
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
int CALLAGREEMENT _read_line(ISFILE *fp, char *buff)
{
	int i = 0;

	while(1)
	{
		if (isio_read((void*)&(buff[i]), 1, 1, fp) == 0)
			return -1;								/* 读错误或文件结束 */
		
		if (buff[i] == '\r')
			continue;
		else if (buff[i] == '\n')					/* 去掉/r/n符号 */
		{
			buff[i++] = (char)0;
			break;
		}
		else
			i++;
	}

	return i;
}


/* 读取图像文件中的所有颜色数据 */
int CALLAGREEMENT _read_color_line(LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp)
{
	static char buff[256];
	int	i, result;

	for (i=0;i<ncol;i++)
	{
		if (_read_line(fp, (char *)buff) == -1)
			return -1;						/* 文件读写错 */

		result = _parse_color_line((char *)buff, cpp, pcol[i].pix, &(pcol[i].rgb));

		switch (result)
		{
		case	-1:
			return -2;						/* 文件数据错误 */
		case	0:
			pcol[i].attrib = 0;
			break;
		case	1:
			pcol[i].attrib = 1;
			break;
		case	-2:
		default:
			assert(0);
			return -3;						/* 系统不稳定 */
		}
	}

	return i;
}


#define XPM_MONO_MASK	1
#define XPM_COLOR_MASK	2
#define XPM_GRAY_MASK	4

/* 解析颜色数据 */
int CALLAGREEMENT _parse_color_line(char *line, int cpp, char *colname, unsigned long *rgb)
{
	static char mbuf[128], cbuf[128], gbuf[128];
	unsigned long mrgb, crgb, grgb;
	int  matt, catt, gatt;
	char *p, *coldat;
	char nametype;
	int  result, bmask = 0;
	
	assert((line)&&(strlen(line)));

	if ((p=strchr(line, (int)'\"')) == 0)
		return -1;						/* 源数据格式不正确 */

	p++;								/* 跨过前导引号 */

	if (strlen(p) <= (size_t)cpp)
		return -1;

	memcpy((void*)colname, (const void *)p, cpp);	/* 复制象素描述符 */

	p += cpp;							/* 跨过象素描述符 */
	
	matt = catt = gatt = 0;

	while (*p)
	{
		SKIPSPACE(p);					/* 跳过空白符 */
		if (*p == '\"')
			break;

		nametype = *p;
		SKIPNONSPACE(p);
		SKIPSPACE(p);
		coldat = p;
		SKIPNONSPACE(p);

		switch (nametype)
		{
		case	'S':
		case	's':					/* 跳过符号名称串 */
			continue;
		case	'M':
		case	'm':
			matt = result = _color_to_rgb(coldat, p - coldat, &mrgb);
			if ((matt == 0)||(matt == 1))
				bmask |= XPM_MONO_MASK;
			break;
		case	'C':
		case	'c':
			catt = result = _color_to_rgb(coldat, p - coldat, &crgb);
			if ((catt == 0)||(catt == 1))
				bmask |= XPM_COLOR_MASK;
			break;
		case	'G':
		case	'g':
			gatt = result = _color_to_rgb(coldat, p - coldat, &grgb);
			if ((gatt == 0)||(gatt == 1))
				bmask |= XPM_GRAY_MASK;
			break;
		default:
			return -1;					/* 源数据格式不正确 */
		}

		if ((result != 1)&&(result != 0)&&(result != -1))
		{
			assert(0);					/* 系统不稳定 :) */
			return -2;
		}
	}

	/* 此处采用颜色的原则是：有彩色数据就不用灰度数据，有灰度
		数据就不用黑白数据，如果连黑白数据都没有则返回错误。*/
	if (bmask & XPM_COLOR_MASK)
	{ *rgb = crgb; return catt; }
	else if (bmask & XPM_GRAY_MASK)
	{ *rgb = grgb; return gatt; }
	else if (bmask &XPM_MONO_MASK)
	{ *rgb = mrgb; return matt; }
	else
		return -1;						/* 源图像数据有缺损 */
}


/* 读取一行象素数据 */
int CALLAGREEMENT _read_pix_line(unsigned long *pixbuf, int width, int bitcount, LPXPMCOLOR pcol, int ncol, int cpp, ISFILE *fp)
{
	static char filebuf[256], buf[8];
	char *p;
	int i, index;

	/* 去除多余的空格、注释字符 */
	if (_read_until_data(fp) == -1)
		return -1;				/* 文件读写错 */

	if (_read_line(fp, filebuf) == -1)
		return -1;

	p = strchr(filebuf, '\"');	/* 定位到前导引号处 */
	if (p == 0)
		return -2;				/* 非法的象素格式 */

	if (strlen(p) < 3)
		return -3;				/* 两个引号加一个象素符是最短行长度 */
	
	p++;						/* 跨过前导引号 */

	switch (bitcount)
	{
	case	1:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* 非法象素数据 */
			assert(index < 2);
			if (i%8)
				((unsigned char *)pixbuf)[i/8] |= (unsigned char)index<<(i%8);
			else
				((unsigned char *)pixbuf)[i/8] = (unsigned char)index&0x1;
			p += cpp;
		}
		break;
	case	4:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* 非法象素数据 */
			assert(index < 16);
			if (i%2)
				((unsigned char *)pixbuf)[i/2] |= (unsigned char)index<<4;
			else
				((unsigned char *)pixbuf)[i/2] = (unsigned char)index&0xf;
			p += cpp;
		}
		break;
	case	8:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* 非法象素数据 */
			assert(index < 256);
			((unsigned char *)pixbuf)[i] = (unsigned char)index;
			p += cpp;
		}
		break;
	case	32:
		for (i=0;i<width;i++)
		{
			index = _search_pix(pcol, ncol, p, cpp);
			if (index == -1)
				return -2;			/* 非法象素数据 */
			pixbuf[i] = pcol[index].rgb;
			p += cpp;
		}
		break;
	default:
		assert(0);
		return -3;				/* 系统不稳定 */
	}

	return 0;					/* 成功 */
}
