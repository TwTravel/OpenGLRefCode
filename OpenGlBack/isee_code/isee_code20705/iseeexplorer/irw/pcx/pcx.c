/********************************************************************

	pcx.c

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
	本文件用途：	ISee图像浏览器—PCX图像读写模块实现文件
	
					读取功能：1、4、8、24位图像
					保存功能：1、4、8、24位图像
	  
	本文件编写人：	freedvlp	feelfree##263.net（读代码编写人）
					ChK			ChK##163.net（写代码编写人）
					YZ			yzfree##sina.com（改版工作）
		
	本文件版本：	10818
	最后修改于：	2001-8-18
		  
	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	地址收集软件。
	----------------------------------------------------------------
	修正历史：
			
		2001-8		发布新版代码。并扩展了该模块的保存功能，
					增强了模块容错性，提高了模块的读写速度。
			  
		2001-4		完成了图像的保存功能（24位）
		2000-8		完成了读取图像部分的全部代码
		2000-6		第一个测试版发布


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

#include "pcx.h"


IRWP_INFO			pcx_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	pcx_get_info_critical;	/* pcx_get_image_info函数的关键段 */
CRITICAL_SECTION	pcx_load_img_critical;	/* pcx_load_image函数的关键段 */
CRITICAL_SECTION	pcx_save_img_critical;	/* pcx_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _pcx_is_valid_img(LPINFOSTR pinfo_str);

char * CALLAGREEMENT _read_line(char *p_buf, char *p_sou, int linesize, char *p_base, int data_size);
int	   CALLAGREEMENT _write_line(char *p_buf, char *p_sou, int linesize);
void   CALLAGREEMENT _get_palette(LPINFOSTR pinfo_str, PCX_HEADER *p_header, ISFILE *pfile);



#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&pcx_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&pcx_get_info_critical);
			InitializeCriticalSection(&pcx_load_img_critical);
			InitializeCriticalSection(&pcx_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&pcx_get_info_critical);
			DeleteCriticalSection(&pcx_load_img_critical);
			DeleteCriticalSection(&pcx_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

PCX_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&pcx_irwp_info;
}

#else

PCX_API LPIRWP_INFO CALLAGREEMENT pcx_get_plugin_info()
{
	_init_irwp_info(&pcx_irwp_info);

	return (LPIRWP_INFO)&pcx_irwp_info;
}

PCX_API void CALLAGREEMENT pcx_init_plugin()
{
	/* 初始化多线程同步对象 */
}

PCX_API void CALLAGREEMENT pcx_detach_plugin()
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
	lpirwp_info->irwp_save.bitcount = 1UL | (1UL<<(4-1)) | (1UL<<(8-1)) | (1UL<<(24-1));

	lpirwp_info->irwp_save.img_num = 1;	/* 1－只能保存一副图像 */
	/* 如需更多的设定参数，可修改此值（##需手动修正） */
	lpirwp_info->irwp_save.count = 0;

	/* 开发者人数（即开发者信息中有效项的个数）（##需手动修正）*/
	lpirwp_info->irwp_author_count = 3;	


	/* 开发者信息（##需手动修正） */
	/* ---------------------------------[0] － 第一组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_name), 
				(const char *)"freedvlp");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_email), 
				(const char *)"feelfree##263.net");
	strcpy((char*)(lpirwp_info->irwp_author[0].ai_message), 
				(const char *)"^_^");
	/* ---------------------------------[1] － 第二组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_name), 
				(const char *)"ChK");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_email), 
				(const char *)"ChK##163.net");
	strcpy((char*)(lpirwp_info->irwp_author[1].ai_message), 
				(const char *)"^_^");
	/* ---------------------------------[2] － 第三组 -------------- */
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_name), 
				(const char *)"YZ");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_email), 
		(const char *)"yzfree##sina.com");
	strcpy((char*)(lpirwp_info->irwp_author[2].ai_message), 
		(const char *)"^_^");
	/* ---------------------------------[3] － 第四组 -------------- */
	/* 后续开发者信息可加在此处。
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_name), 
				(const char *)"");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_email), 
				(const char *)"@");
	strcpy((char*)(lpirwp_info->irwp_author[3].ai_message), 
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
PCX_API int CALLAGREEMENT pcx_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCX_HEADER		phr;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&pcx_get_info_critical);

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

			if (isio_read((void*)&phr, sizeof(PCX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			

			/* 检测文件标志 */
			if (phr.byManufacturer != (unsigned char)PCX_FLAG)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 数据有效性检验 */
			if (((phr.wRight-phr.wLeft)==0)||((phr.wBottom-phr.wTop)==0))
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			
			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_PCX;			/* 图像文件格式（后缀名） */
			pinfo_str->compression  = ICS_PCXRLE;		/* 图像的压缩方式 */
			
			/* 填写图像信息 */
			pinfo_str->width	= (unsigned long)((phr.wRight-phr.wLeft)+1);
			pinfo_str->height	= (unsigned long)((phr.wBottom-phr.wTop)+1);
			pinfo_str->order	= 0;
			pinfo_str->bitcount	= phr.byBits*phr.byPlanes;
			
			/* 填写掩码数据 */
			if (pinfo_str->bitcount > 8)	/* 24位 */
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
			
			/* 设定数据包状态 */
			pinfo_str->data_state = 1;
		}
		__finally
		{
			if (pfile)
				isio_close(pfile);

			LeaveCriticalSection(&pcx_get_info_critical);
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
PCX_API int CALLAGREEMENT pcx_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCX_HEADER		phr;
	int				i, j, k, linesize, bufsize;
	unsigned long	data_len;
	
	unsigned char	buf, tmp;
	unsigned short	us;
	int				cnv_count, bit, bitmask;
	
	unsigned char	*pbit = (unsigned char *)0;
	unsigned char	*ptmp = (unsigned char *)0;
	unsigned char	*plinebuf = (unsigned char *)0;

	unsigned char	*psou, *pdesline;
	unsigned char	*pla3, *pla2, *pla1, *pla0;
	unsigned char	pb3, pb2, pb1, pb0, tbuf, tofs;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&pcx_load_img_critical);

			/* 打开流 */
			if ((pfile = isio_open((const char *)psct, "rb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total = 1;
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

			if (isio_read((void*)&phr, sizeof(PCX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			
			/* 检测文件标志 */
			if (phr.byManufacturer != (unsigned char)PCX_FLAG)
			{
				b_status = ER_NONIMAGE; __leave;
			}
			
			/* 数据有效性检验 */
			if (((phr.wRight-phr.wLeft)==0)||((phr.wBottom-phr.wTop)==0))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_PCX;			/* 图像文件格式（后缀名） */
				pinfo_str->compression  = ICS_PCXRLE;		/* 图像的压缩方式 */
				
				/* 填写图像信息 */
				pinfo_str->width	= (unsigned long)((phr.wRight-phr.wLeft)+1);
				pinfo_str->height	= (unsigned long)((phr.wBottom-phr.wTop)+1);
				pinfo_str->order	= 0;
				pinfo_str->bitcount	= phr.byBits*phr.byPlanes;
				
				/* 填写掩码数据 */
				if (pinfo_str->bitcount > 8)	/* 24位 */
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
				
				/* 设定数据包状态 */
				pinfo_str->data_state = 1;
			}


			/* 设置调色板数据 */
			_get_palette(pinfo_str, &phr, pfile);


			/* 取得图像数据长度 */
			data_len = isio_length(pfile) - sizeof(PCX_HEADER);

			if ((pinfo_str->bitcount == 8)&&(phr.wPaletteType == 1))
				data_len -= 256*3+1;		/* +1 : 0xc0

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

			/* 初始化行首地址数组（PCX图像都为正向） */
			for (i=0;i<(int)(pinfo_str->height);i++)
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+(i*linesize));


			bufsize = (int)phr.wLineBytes * phr.byPlanes;

			/* 申请用于存放临时解压数据的缓冲区 */
			plinebuf = (unsigned char *)malloc(bufsize);
			if (plinebuf == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}


			/* 读取位数据到内存 */
			if (isio_seek(pfile, sizeof(PCX_HEADER), SEEK_SET) == -1)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 申请临时内存块，用于存放整个PCX图像数据 */
			ptmp = (unsigned char *)malloc(data_len);
			if (ptmp == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			/* 读原始图像数据 */
			if (isio_read((void*)ptmp, data_len, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			pinfo_str->process_total = pinfo_str->height;
			psou = ptmp;
			pbit = pinfo_str->p_bit_data;

			/* 逐行读入 */
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				if ((psou=_read_line((char *)plinebuf, (char *)psou, bufsize, (char *)ptmp, data_len)) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 目标缓冲区地址 */
				pdesline = (unsigned char *)pinfo_str->pp_line_addr[i];

				switch (pinfo_str->bitcount)
				{
				case	1:
					/* 1位图像数据翻转拷贝，字节低端对应左边图像象素 */
					for (cnv_count=0;cnv_count<bufsize;cnv_count++)
					{
						buf = *(unsigned char *)(plinebuf+cnv_count);
						us  = ((unsigned short)buf)<<8;
						tmp = 0;
						for (bit=1, bitmask=7;bit<16;bit+=2, bitmask--)
							tmp |= (us>>bit)&(1<<bitmask);
						
						*pdesline++ = tmp;
					}
					break;
				case	2:
				case	3:
					/* 待修改。（未找到相关的面板设置资料，假设为1个面板）*/
					memcpy((void*)pdesline, (const void *)plinebuf, bufsize);
					break;
				case	4:
					/* 1位、4面板 */
					pla0 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*0));
					pla1 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*1));
					pla2 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*2));
					pla3 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*3));

					for (k=0;k<(int)pinfo_str->width;k+=8)
					{
						pb0 = *pla0++; pb1 = *pla1++; 
						pb2 = *pla2++; pb3 = *pla3++;

						/* 高位为左边的象素 */
						for (j=7;(j>=0)&&((k+(7-j))<(int)pinfo_str->width);j--)
						{
							tofs = 1 << j; tbuf = 0;

							tbuf |= (pb0&tofs) ? 0x1 : 0;
							tbuf |= (pb1&tofs) ? 0x2 : 0;
							tbuf |= (pb2&tofs) ? 0x4 : 0;
							tbuf |= (pb3&tofs) ? 0x8 : 0;

							/* ISee位流格式－左象素在字节低位 */
							if (j%2)
								*pdesline = tbuf;
							else
							{
								tbuf <<= 4;
								*pdesline++ |= tbuf;
							}
						}
					}
					break;
				case	8:
					/* 8位图像数据直接拷贝 */
					memcpy((void*)pdesline, (const void *)plinebuf, bufsize);
					break;
				case	24:
					/* 8位、3面板 */
					pla0 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*0));
					pla1 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*1));
					pla2 = (unsigned char *)(plinebuf+((int)phr.wLineBytes*2));
					
					/* BGR顺序 */
					for (k=0;k<(int)pinfo_str->width;k++)
					{
						*pdesline++ = *pla2++;	/* B */
						*pdesline++ = *pla1++;	/* G */
						*pdesline++ = *pla0++;	/* R */
					}
					break;
				default:
					b_status = ER_BADIMAGE; __leave;
				}

				/* 步进 */
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
			}

			if (plinebuf)
				free(plinebuf);
			if (ptmp)
				free(ptmp);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pcx_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
PCX_API int CALLAGREEMENT pcx_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;

	PCX_HEADER		phr;

	int				i, j, k, expsize, commsize;

	unsigned char	*psou, *pdes;

	unsigned char   *pexpend = (unsigned char *)0;
	unsigned char   *pexpend2= (unsigned char *)0;
	unsigned char   *pdesbuf = (unsigned char *)0;

	LPPCX_PALITEM	p8pal = (LPPCX_PALITEM)0;

	unsigned char	*pla3, *pla2, *pla1, *pla0;
	unsigned char	pb3, pb2, pb1, pb0, tbuf;
	
	unsigned char	buf, tmp;
	unsigned short	us;
	int				cnv_count, bit, bitmask;

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&pcx_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_pcx_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 设置PCX图像信息结构 */
			phr.byManufacturer = (unsigned char)PCX_FLAG;

			phr.byVersion  = 5;
			phr.byEncoding = 1;
			phr.wLeft      = 0;
			phr.wTop       = 0;
			assert(pinfo_str->width); 
			assert(pinfo_str->height);
			phr.wRight     = (unsigned short)(pinfo_str->width - 1);
			phr.wBottom    = (unsigned short)(pinfo_str->height - 1);

			phr.byReserved   = 0;

			/* 缺省设置值，可修改 */
			phr.wXResolution = 300;	/* DPI */
			phr.wYResolution = 300;
			phr.wScrWidth    = 800; /* PIXEL */
			phr.wScrDepth    = 600;

			memset((void*)phr.byFiller, 0, 54);
			
			switch (pinfo_str->bitcount)
			{
			case	1:
				phr.byBits = 1;
				phr.byPlanes = 1;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+15)/16)*2);
				phr.wPaletteType = 1;	/* 单色 */

				for (i=0;i<2;i++)
				{
					phr.ptPalette[i].blue  = ((LPPALITEM)(&pinfo_str->palette[i]))->blue;
					phr.ptPalette[i].green = ((LPPALITEM)(&pinfo_str->palette[i]))->green;
					phr.ptPalette[i].red   = ((LPPALITEM)(&pinfo_str->palette[i]))->red;
				}

				expsize = phr.wLineBytes*1;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)0;
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*1*2);	/* 按最糟的情况来分配内存 */
				if ((pexpend==0)||(pdesbuf==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				break;
			case	4:
				phr.byBits = 1;
				phr.byPlanes = 4;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+15)/16)*2);
				phr.wPaletteType = 1;	/* 彩色 */
				
				for (i=0;i<16;i++)
				{
					phr.ptPalette[i].blue  = ((LPPALITEM)(&pinfo_str->palette[i]))->blue;
					phr.ptPalette[i].green = ((LPPALITEM)(&pinfo_str->palette[i]))->green;
					phr.ptPalette[i].red   = ((LPPALITEM)(&pinfo_str->palette[i]))->red;
				}

				expsize = phr.wLineBytes*4;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)malloc(DIBSCANLINE_WIDTHBYTES(pinfo_str->width*4));
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*4*2);
				if ((pexpend==0)||(pdesbuf==0)||(pexpend2==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				break;
			case	8:
				phr.byBits = 8;
				phr.byPlanes = 1;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+1)/2)*2);
				phr.wPaletteType = 0;	/* 彩色 */
				
				expsize = phr.wLineBytes*1;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)0;
				p8pal   = (LPPCX_PALITEM)malloc(sizeof(PCX_PALITEM)*256);
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*1*2);
				if ((pexpend==0)||(pdesbuf==0)||(p8pal==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				/* 调色板数据将在写图像数据后合成 */
				break;
			case	24:
				phr.byBits = 8;
				phr.byPlanes = 3;
				phr.wLineBytes = (unsigned short)(((pinfo_str->width+1)/2)*2);
				phr.wPaletteType = 0;	/* 彩色 */
				
				expsize = phr.wLineBytes*3;
				pexpend = (unsigned char *)malloc(expsize);
				pexpend2= (unsigned char *)malloc(DIBSCANLINE_WIDTHBYTES(pinfo_str->width*3*8));
				pdesbuf = (unsigned char *)malloc(phr.wLineBytes*3*2);
				if ((pexpend==0)||(pdesbuf==0)||(pexpend2==0))
				{
					b_status = ER_MEMORYERR; __leave;
				}
				/* 无调色板数据 */
				break;
			default:
				assert(0);
				break;
			}


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
			
			/* 写入信息结构 */
			if (isio_write((const void *)&phr, sizeof(PCX_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入图像数据 
				注：传入的图像数据必须是ISee位流格式，否则可能会产生失真
			*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				psou = (unsigned char *)pinfo_str->pp_line_addr[i];
				pdes = pexpend;

				/* 展开为面板格式 */
				switch (pinfo_str->bitcount)
				{
				case	1:
					for (cnv_count=0;cnv_count<phr.wLineBytes;cnv_count++)
					{
						buf = *(unsigned char *)(psou+cnv_count);
						us  = ((unsigned short)buf)<<8;
						tmp = 0;
						for (bit=1, bitmask=7;bit<16;bit+=2, bitmask--)
							tmp |= (us>>bit)&(1<<bitmask);
						
						*pdes++ = tmp;
					}
					break;
				case	4:
					/* 字节内顺序翻转 */
					for (cnv_count=0;cnv_count<(int)((pinfo_str->width+1)/2);cnv_count++)
					{
						buf = *(unsigned char *)(psou+cnv_count);
						tmp = (buf>>4)|(buf<<4);
						*(unsigned char *)(pexpend2+cnv_count) = tmp;
					}
					
					/* 初始化展开缓冲区 */
					memset((void*)pexpend, 0, phr.wLineBytes*4);

					pla0 = (unsigned char *)(pexpend+(phr.wLineBytes*0));
					pla1 = (unsigned char *)(pexpend+(phr.wLineBytes*1));
					pla2 = (unsigned char *)(pexpend+(phr.wLineBytes*2));
					pla3 = (unsigned char *)(pexpend+(phr.wLineBytes*3));

					/* 将图像数据展开为4个面板 */
					for (k=0, j=7;k<(int)pinfo_str->width;k++, j--)
					{
						tbuf = *(unsigned char *)(pexpend2+k/2);

						if (k%2)
							tbuf &= 0xf;
						else
							tbuf >>= 4;

						pb0 = ((tbuf&0x1)>>0)<<j;
						pb1 = ((tbuf&0x2)>>1)<<j;
						pb2 = ((tbuf&0x4)>>2)<<j;
						pb3 = ((tbuf&0x8)>>3)<<j;

						*pla0 |= pb0;
						*pla1 |= pb1;
						*pla2 |= pb2;
						*pla3 |= pb3;

						if (j == 0)
						{
							j = 8; pla0++; pla1++; pla2++; pla3++;
						}
					}
					break;
				case	8:
					/* 8位图像直接拷贝 */
					memcpy((void*)pdes, (const void *)psou, phr.wLineBytes);
					break;
				case	24:
					/* 字节内顺序翻转 */
					for (cnv_count=0;cnv_count<(int)pinfo_str->width;cnv_count++)
					{
						*(unsigned char *)(pexpend2+cnv_count*3+2) = *(unsigned char *)(psou+cnv_count*3+0);
						*(unsigned char *)(pexpend2+cnv_count*3+1) = *(unsigned char *)(psou+cnv_count*3+1);
						*(unsigned char *)(pexpend2+cnv_count*3+0) = *(unsigned char *)(psou+cnv_count*3+2);
					}

					/* 初始化展开缓冲区 */
					memset((void*)pexpend, 0, phr.wLineBytes*3);
					
					pla0 = (unsigned char *)(pexpend+(phr.wLineBytes*0)); /*R*/
					pla1 = (unsigned char *)(pexpend+(phr.wLineBytes*1)); /*G*/
					pla2 = (unsigned char *)(pexpend+(phr.wLineBytes*2)); /*B*/
					
					/* 将图像数据展开为3个面板--RGB */
					for (k=0;k<(int)pinfo_str->width;k++)
					{
						*pla0++ = *(unsigned char *)(pexpend2+k*3+0);
						*pla1++ = *(unsigned char *)(pexpend2+k*3+1);
						*pla2++ = *(unsigned char *)(pexpend2+k*3+2);
					}
					break;
				}

				/* 使用RLE算法对展开后的行数据进行压缩 */
				commsize = _write_line((char*)pdesbuf, (char*)pexpend, expsize);

				/* 写入目标流 */
				if (isio_write((const void*)pdesbuf, commsize, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 如果是8位图像，则在流的最后加入256色调色板数据 */
				if ((pinfo_str->bitcount == 8)&&((i+1)==(int)(pinfo_str->height)))
				{
					pdesbuf[0] = (unsigned char)0xc;	/* 0xc 为标志符 */
					if (isio_write((const void*)pdesbuf, 1, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					for (k=0;k<256;k++)
					{
						p8pal[k].blue  = ((LPPALITEM)(&pinfo_str->palette[k]))->blue;
						p8pal[k].green = ((LPPALITEM)(&pinfo_str->palette[k]))->green;
						p8pal[k].red   = ((LPPALITEM)(&pinfo_str->palette[k]))->red;
					}

					if (isio_write((const void*)p8pal, sizeof(PCX_PALITEM)*256, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
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
			if (p8pal)
				free(p8pal);
			if (pexpend2)
				free(pexpend2);
			if (pexpend)
				free(pexpend);
			if (pdesbuf)
				free(pdesbuf);
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&pcx_save_img_critical);
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

/* 读入一行PCX_RLE数据，并解码到指定的缓冲区中 */
char * CALLAGREEMENT _read_line(char *p_buf, char *p_sou, int linesize, char *p_base, int data_size)
{
	int cur_offset = 0;
	unsigned char count, conv;
	
	while(cur_offset < linesize)
	{
		if((p_sou-p_base) >= data_size)
		{
			p_sou = (char *)0; break;
		}

		count = (unsigned char)*p_sou++;

		if((count&0xc0) == 0xc0)			/* 计数值 */
		{
			count &= 0x3f;
			conv = (unsigned char)*p_sou++;	/* 内容 */

			while(count--)
				p_buf[cur_offset++] = conv;	/* 重复写入数据 */
		}
		else								/* 内容 */
			p_buf[cur_offset++] = count;
	}
	
	return p_sou;	/* 返回下一行的读取地址，如果发生数据错，返回0 */
}


/* 写一行数据到PCX_RLE缓冲区 */
int CALLAGREEMENT _write_line(char *p_buf, char *p_sou, int linesize)
{
	int				m = 0;
	int				commsize = 0;
	unsigned char	v, n;
	
	while(m < linesize)
	{
		v = p_sou[m];
		
		/* 取得相连且相同的字节数n */
		n = 1;
		while (((m+n)<linesize) && (n<64) && (v==(unsigned char)(p_sou[m+n])))
			n++;

		if (n > 63 )
			n = 63;
		
		if (n == 1)		/* 未能压缩 */
		{
			if (v < 0xc0)
				p_buf[commsize++] = v;
			else
			{
				p_buf[commsize++] = (char)0xc1;
				p_buf[commsize++] = v;
			}
		}
		else			/* 压缩 */
		{
			p_buf[commsize++] = (char)(n+0xc0);
			p_buf[commsize++] = v;
		}
		m += n;
	}

	return commsize;	/* 压缩后的数据行尺寸 */
}
	

/* 读取调色板数据 */
void CALLAGREEMENT _get_palette(LPINFOSTR pinfo_str, PCX_HEADER *p_header, ISFILE *pfile)
{
	int i;

	PCX_PALITEM		tmp_palbuf[256];
#ifdef WIN32
	PALETTEENTRY	palcolor[256];
	HDC				hdc;
#endif

	assert(pfile);

	if (pinfo_str->bitcount <= 8)
	{
		pinfo_str->pal_count = 1UL << pinfo_str->bitcount;

		if (p_header->wPaletteType <= 1)		/* 彩色或单色（YZ: 0将被视为彩色） */
		{
			if (p_header->byVersion == 3)		/* 使用系统调色板 */
			{
#ifdef WIN32
				hdc = GetDC((HWND)0);
				GetSystemPaletteEntries(hdc, 0, pinfo_str->pal_count, (LPPALETTEENTRY)palcolor);
				ReleaseDC((HWND)0, hdc);

				for (i=0;i<(int)pinfo_str->pal_count;i++)
				{
					((LPPALITEM)(&pinfo_str->palette[i]))->blue = palcolor[i].peBlue;
					((LPPALITEM)(&pinfo_str->palette[i]))->green= palcolor[i].peGreen;
					((LPPALITEM)(&pinfo_str->palette[i]))->red  = palcolor[i].peRed;
					((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
				}
#else
				/* 缺省为灰度调色板，可更改为Linux GUI系统的缺省调色板 */
				switch (pinfo_str->bitcount)
				{
				case	1:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0xffffff;
					break;
				case	2:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0x555555;
					pinfo_str->palette[2] = 0xaaaaaa;
					pinfo_str->palette[3] = 0xffffff;
					break;
				case	3:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0x242424;
					pinfo_str->palette[2] = 0x484848;
					pinfo_str->palette[3] = 0x6c6c6c;
					pinfo_str->palette[4] = 0x909090;
					pinfo_str->palette[5] = 0xb4b4b4;
					pinfo_str->palette[6] = 0xd8d8d8;
					pinfo_str->palette[7] = 0xffffff;
					break;
				case	4:
					pinfo_str->palette[0] = 0;
					pinfo_str->palette[1] = 0x111111;
					pinfo_str->palette[2] = 0x222222;
					pinfo_str->palette[3] = 0x333333;
					pinfo_str->palette[4] = 0x444444;
					pinfo_str->palette[5] = 0x555555;
					pinfo_str->palette[6] = 0x666666;
					pinfo_str->palette[7] = 0x777777;
					pinfo_str->palette[8] = 0x888888;
					pinfo_str->palette[9] = 0x999999;
					pinfo_str->palette[10] = 0xaaaaaa;
					pinfo_str->palette[11] = 0xbbbbbb;
					pinfo_str->palette[12] = 0xcccccc;
					pinfo_str->palette[13] = 0xdddddd;
					pinfo_str->palette[14] = 0xeeeeee;
					pinfo_str->palette[15] = 0xffffff;
					break;
				case	8:
					for (i=0;i<(int)256;i++)
						pinfo_str->palette[i] = i|i<<8|i<<16;
					break;
				}
#endif
			}
			else							/* 自带调色板 */
			{
				if (pinfo_str->bitcount == 8)
				{
					__try
					{
						/* 读取256色调色板数据（流尾部）*/
						isio_seek(pfile, -256L*3, SEEK_END);
						isio_read((void*)tmp_palbuf, 256*3, 1, pfile);
					}
					__except(EXCEPTION_CONTINUE_SEARCH)
					{
						;
					}

					/* 复制调色板数据（8位） */
					for (i=0;i<(int)pinfo_str->pal_count;i++)
					{
						((LPPALITEM)(&pinfo_str->palette[i]))->blue = tmp_palbuf[i].blue;
						((LPPALITEM)(&pinfo_str->palette[i]))->green= tmp_palbuf[i].green;
						((LPPALITEM)(&pinfo_str->palette[i]))->red  = tmp_palbuf[i].red;
						((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
					}
				}
				else
				{	/* 复制调色板数据（1、2、3、4位） */
					for (i=0;i<(int)pinfo_str->pal_count;i++)
					{
						((LPPALITEM)(&pinfo_str->palette[i]))->blue  = p_header->ptPalette[i].blue;
						((LPPALITEM)(&pinfo_str->palette[i]))->green = p_header->ptPalette[i].green;
						((LPPALITEM)(&pinfo_str->palette[i]))->red   = p_header->ptPalette[i].red;
						((LPPALITEM)(&pinfo_str->palette[i]))->reserved = 0;
					}
				}
			}
		}
		else								/* 灰度（2） */
		{
			switch (pinfo_str->bitcount)
			{
			case	1:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0xffffff;
				break;
			case	2:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0x555555;
				pinfo_str->palette[2] = 0xaaaaaa;
				pinfo_str->palette[3] = 0xffffff;
				break;
			case	3:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0x242424;
				pinfo_str->palette[2] = 0x484848;
				pinfo_str->palette[3] = 0x6c6c6c;
				pinfo_str->palette[4] = 0x909090;
				pinfo_str->palette[5] = 0xb4b4b4;
				pinfo_str->palette[6] = 0xd8d8d8;
				pinfo_str->palette[7] = 0xffffff;
				break;
			case	4:
				pinfo_str->palette[0] = 0;
				pinfo_str->palette[1] = 0x111111;
				pinfo_str->palette[2] = 0x222222;
				pinfo_str->palette[3] = 0x333333;
				pinfo_str->palette[4] = 0x444444;
				pinfo_str->palette[5] = 0x555555;
				pinfo_str->palette[6] = 0x666666;
				pinfo_str->palette[7] = 0x777777;
				pinfo_str->palette[8] = 0x888888;
				pinfo_str->palette[9] = 0x999999;
				pinfo_str->palette[10] = 0xaaaaaa;
				pinfo_str->palette[11] = 0xbbbbbb;
				pinfo_str->palette[12] = 0xcccccc;
				pinfo_str->palette[13] = 0xdddddd;
				pinfo_str->palette[14] = 0xeeeeee;
				pinfo_str->palette[15] = 0xffffff;
				break;
			case	8:
				for (i=0;i<(int)256;i++)
					pinfo_str->palette[i] = i|i<<8|i<<16;
				break;
			}
		}
	}
	else
	{
		pinfo_str->pal_count = 0;		/* 无调色板数据 */
	}
}


/* 判断传入的图像是否可以被保存 */
int CALLAGREEMENT _pcx_is_valid_img(LPINFOSTR pinfo_str)
{
	if (!(pcx_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	
	assert(pinfo_str->imgnumbers);
	
	if (pcx_irwp_info.irwp_save.img_num)
		if (pcx_irwp_info.irwp_save.img_num == 1)
			if (pinfo_str->imgnumbers != 1)
				return -2;	/* 图像个数不正确 */
			
	return 0;
}
