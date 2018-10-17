/********************************************************************

	pnm.c

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
	本文件用途：	ISee图像浏览器—PBM、PGM、PPM图像读写模块实现文件

					读取功能：PBM、PGM、PPM 文本及二进制图像
							  
					保存功能：可将1、8、24位图像保存为对应的
							  PBM、PGM、PPM 文本或二进制图像格式
							   

	本文件编写人：	YZ			yzfree##yeah.net

	本文件版本：	20207
	最后修改于：	2002-2-7

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

			2002-2	第一个发布版（新版接口），插件版本2.0


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
#include <ctype.h>
#include <assert.h>

#include "pnm.h"


IRWP_INFO			pnm_irwp_info;			/* 插件信息表 */

static enum pnm_type						/* PNM 类型定义 */
{
	PBM = 1,
	PGM,
	PPM
};

#ifdef WIN32
CRITICAL_SECTION	pnm_get_info_critical;	/* pnm_get_image_info函数的关键段 */
CRITICAL_SECTION	pnm_load_img_critical;	/* pnm_load_image函数的关键段 */
CRITICAL_SECTION	pnm_save_img_critical;	/* pnm_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _pnm_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave);
int CALLAGREEMENT _read_number(ISFILE *fp);
int CALLAGREEMENT _write_number(ISFILE *fp, unsigned int num, int mark);
int CALLAGREEMENT _is_pnm_hahaha(char *pmc);


#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&pnm_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&pnm_get_info_critical);
			InitializeCriticalSection(&pnm_load_img_critical);
			InitializeCriticalSection(&pnm_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&pnm_get_info_critical);
			DeleteCriticalSection(&pnm_load_img_critical);
			DeleteCriticalSection(&pnm_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PNM_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pnm_irwp_info;
}

#else

PNM_API LPIRWP_INFO CALLAGREEMENT pnm_get_plugin_info()
{
	_init_irwp_info(&pnm_irwp_info);

	return (LPIRWP_INFO)&pnm_irwp_info;
}

PNM_API void CALLAGREEMENT pnm_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PNM_API void CALLAGREEMENT pnm_detach_plugin()
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
	/* 各位数减一，否则32位格式无法表示。此处请用PNM正确的位深填写！*/
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(8-1)) | (1UL<<(24-1));
	/* ################################################################# */

	lpirwp_info->irwp_save.img_num = 1;	/* 1－只能保存一副图像 */

	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 1;
	lpirwp_info->irwp_save.para[0].count = 2;		/* 共有两种存储方式：文本、二进制 */
	strcpy((char*)lpirwp_info->irwp_save.para[0].desc, (const char *)"存储方式");
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[0], (const char *)"文本");
	strcpy((char*)lpirwp_info->irwp_save.para[0].value_desc[1], (const char *)"二进制");
	lpirwp_info->irwp_save.para[0].value[0] = 0;	/* 0 代表文本，1 代表二进制 */
	lpirwp_info->irwp_save.para[0].value[1] = 1;

	
	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 1;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"yzfree##yeah.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"呵呵，我都想不出要说什么了:-D");
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
	lpirwp_info->irwp_desc_info.idi_synonym_count = 2;

	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[0]), 
		(const char *)"PGM");
	strcpy((char*)(lpirwp_info->irwp_desc_info.idi_synonym[1]), 
		(const char *)"PPM");

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
PNM_API int CALLAGREEMENT pnm_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	char			buff[4];
	int				kind, ascii_mark;
	int				width, height, maxval;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&pnm_get_info_critical);

			/* 打开指定流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{ 
				b_status = ER_FILERWERR; __leave;	
			}
			
			/* 读取流头结构 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			memset((void*)buff, 0, 4);

			if (isio_read((void*)buff, 2, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断流是否是一个PNM图像 */
			if ((kind=_is_pnm_hahaha(buff)) == -1)
			{
				b_status = ER_NONIMAGE; __leave;
			}

			if (kind > 3)
			{
				ascii_mark = 0;			/* 二进制格式 */
				kind -= 3;
			}
			else
			{
				ascii_mark = 1;			/* ASCII 格式 */
			}
			
			/* 提取图像宽、高数据 */
			if ((width=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			if ((height=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}

			/* 宽、高值检测 */
			if ((width <= 0)||(height <= 0))
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* PGM和PPM都存在一个最大颜色分量变量值 */
			if (kind != PBM)
			{
				if ((maxval=_read_number(pfile)) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				/* ISee系统不支持单个颜色分量大于8位的图像 */
				if ((maxval <= 0)||(maxval > 255))
				{
					b_status = ER_BADIMAGE; __leave;
				}
			}
			else
			{
				maxval = 255;
			}

			
			/* 填写图像信息 */
			pinfo_str->imgtype		 = IMT_RESSTATIC;	/* 图像流类型 */
			if (kind == PBM)							/* 图像流格式（后缀名） */
				pinfo_str->imgformat = IMF_PBM;
			else if (kind == PGM)
				pinfo_str->imgformat = IMF_PGM;
			else if (kind == PPM)
				pinfo_str->imgformat = IMF_PPM;
			else
				assert(0);
			if (ascii_mark == 1)						/* 图像数据压缩方式 */
				pinfo_str->compression  = ICS_PNM_TEXT;
			else if (ascii_mark == 0)
				pinfo_str->compression  = ICS_PNM_BIN;
			else
				assert(0);
			
			pinfo_str->width		= width;
			pinfo_str->height		= height;
			pinfo_str->order		= 0;				/* 正向 */

			pinfo_str->b_mask = 0x0;
			pinfo_str->g_mask = 0x0;
			pinfo_str->r_mask = 0x0;
			pinfo_str->a_mask = 0x0;
													
			if (kind == PBM)							/* 图像位深 */
				pinfo_str->bitcount = 1;
			else if (kind == PGM)
				pinfo_str->bitcount = 8;				/* 调色板图 */
			else if (kind == PPM)
			{
				pinfo_str->bitcount = 24;

				pinfo_str->b_mask = 0xff;
				pinfo_str->g_mask = 0xff00;
				pinfo_str->r_mask = 0xff0000;
				pinfo_str->a_mask = 0xff000000;
			}
			else
				assert(0);
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (AbnormalTermination())
				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
				
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pnm_get_info_critical);
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
PNM_API int CALLAGREEMENT pnm_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	char			buff[4], *p, ch;
	int				kind, ascii_mark, i, y, x, r, linesize, cr, cb, cg;
	int				width, height, maxval;
	
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pnm_load_img_critical);

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

			/* 读流定位 */
			if (isio_seek(pfile, 0, SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 读PNM图像首部标志信息 */
			if (isio_read((void*)buff, 2, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断流是否是一个PNM图像 */
			if ((kind=_is_pnm_hahaha(buff)) == -1)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			if (kind > 3)
			{
				ascii_mark = 0;			/* 二进制格式 */
				kind -= 3;
			}
			else
			{
				ascii_mark = 1;			/* ASCII 格式 */
			}

			/* 提取图像宽、高数据 */
			if ((width=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			if ((height=_read_number(pfile)) == -1)
			{
				b_status = ER_BADIMAGE; __leave;
			}
			
			/* PGM和PPM都存在一个最大颜色分量变量值 */
			if (kind != PBM)
			{
				if ((maxval=_read_number(pfile)) == -1)
				{
					b_status = ER_BADIMAGE; __leave;
				}
				/* ISee系统不支持单个颜色分量大于8位的图像 */
				if ((maxval <= 0)||(maxval > 255))
				{
					b_status = ER_BADIMAGE; __leave;
				}
			}
			else
			{
				maxval = 255;
			}

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				/* 宽、高值检测 */
				if ((width <= 0)||(height <= 0))
				{
					b_status = ER_BADIMAGE; __leave;
				}
				
				/* 填写图像信息 */
				pinfo_str->imgtype		 = IMT_RESSTATIC;	/* 图像流类型 */
				if (kind == PBM)							/* 图像流格式（后缀名） */
					pinfo_str->imgformat = IMF_PBM;
				else if (kind == PGM)
					pinfo_str->imgformat = IMF_PGM;
				else if (kind == PPM)
					pinfo_str->imgformat = IMF_PPM;
				else
					assert(0);
				if (ascii_mark == 1)						/* 图像数据压缩方式 */
					pinfo_str->compression  = ICS_PNM_TEXT;
				else if (ascii_mark == 0)
					pinfo_str->compression  = ICS_PNM_BIN;
				else
					assert(0);
				
				pinfo_str->width		= width;
				pinfo_str->height		= height;
				pinfo_str->order		= 0;				/* 正向 */

				pinfo_str->b_mask = 0x0;
				pinfo_str->g_mask = 0x0;
				pinfo_str->r_mask = 0x0;
				pinfo_str->a_mask = 0x0;
														
				if (kind == PBM)							/* 图像位深 */
					pinfo_str->bitcount = 1;
				else if (kind == PGM)
					pinfo_str->bitcount = 8;				/* 调色板图 */
				else if (kind == PPM)
				{
					pinfo_str->bitcount = 24;

					pinfo_str->b_mask = 0xff;
					pinfo_str->g_mask = 0xff00;
					pinfo_str->r_mask = 0xff0000;
					pinfo_str->a_mask = 0xff000000;
				}
				else
					assert(0);
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}


			/* 设置调色板数据 */
			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->pal_count = 2;
				pinfo_str->palette[0] = 0xffffff;
				pinfo_str->palette[1] = 0x0;
				break;
			case	8:
				pinfo_str->pal_count = 256;
				for (i=0;i<256;i++)
				{
					pinfo_str->palette[i] = (i<<16)|(i<<8)|(i<<0);
				}
				break;
			case	24:
				pinfo_str->pal_count = 0;
				break;
			default:
				assert(0);
				pinfo_str->pal_count = 0;
				break;
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
			
			/* 初始化行首数组 */			
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));
			
			/* 计算颜色分量的缩放因子 */
			r = 255 / maxval;

			/* 逐行读入 */
			if (pinfo_str->compression == ICS_PNM_TEXT)
			{
				for (y=0; y<height; y++)
				{
					/* 取得行首地址 */
					p = (char*)pinfo_str->pp_line_addr[y];

					if (pinfo_str->imgformat == IMF_PBM)
					{
						memset((void*)p, 0, linesize);	/* 因为下面只有‘或’操作，所以需要先初始化为0 */

						for (x=0; x<width; x++)
						{
							do {
								if (isio_read((void*)buff, 1, 1, pfile) == 0)
								{
									b_status = ER_FILERWERR; __leave;
								}
							} while(isspace((int)buff[0]));

							/* 苛刻的源数据检验 :) */
							if ((buff[0] != '0')&&(buff[0] != '1'))
							{
								b_status = ER_BADIMAGE; __leave;
							}
							buff[0] -= '0';

							p[x/8] |= buff[0]<<(x%8);	/* 头象素在字节低端(ISee format) */
						}
					}
					else if (pinfo_str->imgformat == IMF_PGM)
					{
						for (x=0; x<width; x++)
						{
							if ((i=_read_number(pfile)) == -1)
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							if (i > maxval)
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							i *= r;	/* 缩放颜色分量范围至 0～255 */
							
							p[x] = (char)i;
						}
					}
					else if (pinfo_str->imgformat == IMF_PPM)
					{
						for (x=0; x<width; x++)
						{
							if (((cr=_read_number(pfile)) == -1)||
								((cg=_read_number(pfile)) == -1)||
							    ((cb=_read_number(pfile)) == -1))
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							if ((cr > maxval)||(cg > maxval)||(cb > maxval))
							{
								b_status = ER_BADIMAGE; __leave;
							}
							
							/* 缩放颜色分量范围至 0～255 */
							cr *= r;
							cg *= r;
							cb *= r;
							
							p[x*3]   = (char)cb;
							p[x*3+1] = (char)cg;
							p[x*3+2] = (char)cr;	/* BGR 顺序 (ISee format) */
						}
					}
					else
					{
						assert(0);
					}

					pinfo_str->process_current = y+1;
					
					/* 支持用户中断 */
					if (pinfo_str->break_mark)
					{
						b_status = ER_USERBREAK; __leave;
					}
				}
			}
			else	/* ICS_PNM_BIN */
			{
				for (y=0; y<height; y++)
				{
					/* 取得行首地址 */
					p = (char*)pinfo_str->pp_line_addr[y];
					
					if (pinfo_str->imgformat == IMF_PBM)
					{
						if (isio_read((void*)p, (size_t)((width+7)>>3), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						/* 颠倒象素排列，使之符合ISee格式（低序） */
						for (x=0; x<width; x+=8)
						{
							for (i=0, ch=0; i<8; i++)
							{
								ch |= ((p[x/8]&(1<<i))>>i)<<(7-i);
							}
							p[x/8] = ch;
						}
					}
					else if (pinfo_str->imgformat == IMF_PGM)
					{
						if (isio_read((void*)p, width, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						/* 与缩放因子相乘，得到0～255范围的颜色分量值 */
						for (x=0; x<width; x++)
						{
							p[x] *= (char)r;
						}
					}
					else if (pinfo_str->imgformat == IMF_PPM)
					{
						if (isio_read((void*)p, (size_t)(width*3), 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;
						}
						/* 交换R、B分量位置，使之符合ISee格式 */
						for (x=0; x<width; x++)
						{
							buff[0] = p[x*3];
							p[x*3]  = p[x*3+2];
							p[x*3+2]= buff[0];

							p[x*3]   *= (char)r;		/* 缩放分量 */
							p[x*3+1] *= (char)r;
							p[x*3+2] *= (char)r;
						}
					}
					else
					{
						assert(0);
					}

					pinfo_str->process_current = y+1;
					
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

				if (b_status == ER_SUCCESS)
					b_status = ER_FILERWERR;	/* I/O读写异常 */
			}

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pnm_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
PNM_API int CALLAGREEMENT pnm_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				ascii_mark;			/* 存储方式 */
	int				kind;				/* 保存格式（后缀名）*/
	char			buffer[64], *p, ch;
	int				i, x, y, mark = 0;
	unsigned int	num;
	
	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pnm_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_pnm_is_valid_img(pinfo_str, lpsave) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 定位到流首部 */
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
			
			
			/* 0 代表文本方式，1代表二进制方式 */
			ascii_mark = (lpsave->para_value[0] == 0) ? 1 : 0;
			/* 确定保存格式 */
			switch (pinfo_str->bitcount)
			{
			case	1:
				kind = PBM;
				break;
			case	8:
				kind = PGM;
				break;
			case	24:
				kind = PPM;
				break;
			default:
				assert(0);			/* 本模块不能保存以上三种格式以外的图像 */
				b_status = ER_NSIMGFOR;
				__leave;
				break;
			}

			/* 写入流头部信息结构 */
			sprintf((char*)buffer, "P%d\n#Created by ISeeExplorer PNM Plug-in 2.0\n%d %d", ((ascii_mark == 1) ? kind:(kind+3)), (int)pinfo_str->width, (int)pinfo_str->height);

			switch (kind)
			{
			case	PBM:
				strcat((char*)buffer, (const char *)"\n");
				break;
			case	PGM:
			case	PPM:
				strcat((char*)buffer, (const char *)" 255\n");
				break;
			default:
				assert(0);
				b_status = ER_NSIMGFOR;
				__leave;
				break;
			}

			if (isio_write((const void *)buffer, strlen(buffer), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入图像数据 */
			if (ascii_mark == 1)	/* 文本方式 */
			{
				if (kind == PBM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];

						for (x=0; x<(int)pinfo_str->width; x++)
						{
							mark++;
							if (_write_number(pfile, (unsigned int)(unsigned char)((p[x/8]&(1<<(x%8)))>>(x%8)), ((mark<32) ? 0:1)) == -1)
							{
								b_status = ER_FILERWERR; __leave;
							}
							/* 因为PNM说明书中规定每行数据的字符数不能超过70个字符所以设置mark变量 */
							if (mark >= 32)
							{
								mark = 0;
							}
						}

						/* 累加步进值 */
						pinfo_str->process_current = y+1;
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else if (kind == PGM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x++)
						{
							((unsigned long *)buffer)[0] = pinfo_str->palette[(unsigned int)(unsigned char)p[x]];
							/* 将RGB数据转换为灰度数据。灰度转换公式 (3*r+6*g+b)/10 */
							num = (((unsigned int)(unsigned char)buffer[2])*3+((unsigned int)(unsigned char)buffer[1])*6+((unsigned int)(unsigned char)buffer[0])*1)/(unsigned int)10;
							mark++;
							if (_write_number(pfile, num, ((mark<16) ? 0:1)) == -1)
							{
								b_status = ER_FILERWERR; __leave;
							}
							if (mark >= 16)
							{
								mark = 0;
							}
						}
						
						/* 累加步进值 */
						pinfo_str->process_current = y+1;
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else /* if (kind == PPM) */
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x++)
						{
							mark++;
							/* 写入R、G、B数据 */
							if ((_write_number(pfile, (unsigned int)(unsigned char)p[x*3+2], 0) == -1)||
								(_write_number(pfile, (unsigned int)(unsigned char)p[x*3+1], 0) == -1)||
								(_write_number(pfile, (unsigned int)(unsigned char)p[x*3+0], ((mark<5) ? 0:1)) == -1))
							{
								b_status = ER_FILERWERR; __leave;
							}
							if (mark >= 5)
							{
								mark = 0;
							}
						}
						
						/* 累加步进值 */
						pinfo_str->process_current = y+1;
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
			}
			else					/* 二进制方式 */
			{
				assert(ascii_mark == 0);

				/* 二进制方式对每行字符数无要求 */

				if (kind == PBM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x+=8)
						{
							/* 将ISee格式流转换PBM格式 */
							for (i=0, ch=0; i<8; i++)
							{
								ch |= ((p[x/8]&(1<<i))>>i)<<(7-i);
							}
							if (isio_write((const void *)&ch, 1, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave;/* 写文件失败 */
							}
						}
						
						/* 累加步进值 */
						pinfo_str->process_current = y+1;
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else if (kind == PGM)
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						if (isio_write((const void *)p, pinfo_str->width, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave;/* 写文件失败 */
						}
						
						/* 累加步进值 */
						pinfo_str->process_current = y+1;
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
				}
				else /* if (kind == PPM) */
				{
					for (y=0; y<(int)pinfo_str->height; y++)
					{
						p = (char*)pinfo_str->pp_line_addr[y];
						
						for (x=0; x<(int)pinfo_str->width; x++)
						{
							/* 写入R、G、B数据 */
							if ((isio_write((const void *)&(p[x*3+2]), 1, 1, pfile) == 0)||
								(isio_write((const void *)&(p[x*3+1]), 1, 1, pfile) == 0)||
								(isio_write((const void *)&(p[x*3+0]), 1, 1, pfile) == 0))
							{
								b_status = ER_FILERWERR; __leave;
							}
						}
						
						/* 累加步进值 */
						pinfo_str->process_current = y+1;
						
						/* 支持用户中断 */
						if (pinfo_str->break_mark)
						{
							b_status = ER_USERBREAK; __leave;
						}
					}
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
			
			LeaveCriticalSection(&pnm_save_img_critical);
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
int CALLAGREEMENT _pnm_is_valid_img(LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	int i;

	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(pnm_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (pnm_irwp_info.irwp_save.img_num)
	{
		if (pnm_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* 图像个数不正确 */
			}
		}
	}

	for (i=0;i<pnm_irwp_info.irwp_save.para[0].count;i++)
	{
		if (pnm_irwp_info.irwp_save.para[0].value[i] == lpsave->para_value[0])
		{
			break;
		}
	}
		
	return (i==pnm_irwp_info.irwp_save.para[0].count) ? -3:0;		/* -3:无效的存储方式 */
}


/* 从指定流中读取一个非负整数 */
int CALLAGREEMENT _read_number(ISFILE *fp)
{
	unsigned char buff[4];			/* 边界对齐，实际只使用第一个字节 */
	int number = 0;
	
	/* 跨过空白符号（包括ACSII码0x9~0xd和0x20）*/
	do {
		if (isio_read((void*)buff, 1, 1, fp) == 0)
			return -1;				/* 读错误或流结束 */
		
		/* 跨过注释信息（PNM中注释信息以#号开始，直到行尾）*/
		if (buff[0] == '#')
		{
			do
			{
				if (isio_read((void*)buff, 1, 1, fp) == 0)
				{
					return -1;
				}
			} while ((buff[0] != '\r')&&(buff[0] != '\n'));
		}

	} while (isspace((int)(buff[0])));
	
	/* 读取数值 */
	do {
		number *= 10;
		number += buff[0]-'0';
		
		if (isio_read((void*)buff, 1, 1, fp) == 0)
		{
			return -1;
		}

	} while (isdigit(buff[0]));
	
	return(number);		/* 返回负数（-1）表示函数读取失败 */
}


/* 写一个非负数整数（mark为后缀标志：0 后缀空格符，1后缀 \n） */
int CALLAGREEMENT _write_number(ISFILE *fp, unsigned int num, int mark)
{
	char buff[32];

	assert(num >= 0);

	_itoa(num, (char*)buff, 10);	/* 移植时应转换为UNIX对应函数 */

	if (mark == 1)
	{
		strcat((char*)buff, (const char *)"\n");
	}
	else
	{
		strcat((char*)buff, (const char *)" ");
	}

	if (isio_write((const void *)buff, strlen(buff), 1, fp) == 0)
	{
		return -1;					/* 写文件失败 */
	}
	
	return 0;						/* 成功 */
}


/* PNM 流标志判别 */
int CALLAGREEMENT _is_pnm_hahaha(char *pmc)
{
	char c;

	assert(pmc != 0);

	if (*pmc++ != 'P')
		return -1;		/* 非PNM图像 */

	c = (*pmc)-'0';

	/*
	 * P1	PBM, ascii
	 * P2	PGM, ascii
	 * P3	PPM, ascii
	 * P4	PBM, binary
	 * P5	PGM, binary
	 * P6	PPM, binary
	 */

	if ((c < 1)||(c > 6))
		return -1;		/* 非PNM图像 */

	return (int)c;		/* 返回PNM格式标识符 1~6 */
}


