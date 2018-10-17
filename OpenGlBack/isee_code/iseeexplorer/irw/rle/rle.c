/********************************************************************

	rle.c

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
	本文件用途：	ISee图像浏览器—Utah RLE图像读写模块实现文件

					读取功能：可读取8位、24位、32位RLE图象（最多
								允许三个色彩通道，一个ALPHA通道）
					保存功能：可将8位、24位图象保存为RLE图象（不
								压缩形式）
							   
	本文件编写人：	
					YZ			yzfree##yeah.net

	本文件版本：	20522
	最后修改于：	2002-5-22

	注：以上E-Mail地址中的##请用@替换，这样做是为了抵制恶意的E-Mail
	    地址收集软件。
  	----------------------------------------------------------------
	修正历史：

		2002-5		第一个发布版（新版）

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

#include "rle.h"


IRWP_INFO			rle_irwp_info;			/* 插件信息表 */

#ifdef WIN32
CRITICAL_SECTION	rle_get_info_critical;	/* rle_get_image_info函数的关键段 */
CRITICAL_SECTION	rle_load_img_critical;	/* rle_load_image函数的关键段 */
CRITICAL_SECTION	rle_save_img_critical;	/* rle_save_image函数的关键段 */
#else
/* Linux对应的语句 */
#endif


/* 内部助手函数 */
void CALLAGREEMENT _init_irwp_info(LPIRWP_INFO lpirwp_info);
int CALLAGREEMENT _calcu_scanline_size(int w/* 宽度 */, int bit/* 位深 */);
int CALLAGREEMENT _rle_is_valid_img(LPINFOSTR pinfo_str);

static int CALLAGREEMENT _rle_pack(unsigned char *psou, int len, unsigned char *pdec);




#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			/* 初始化插件信息表 */
			_init_irwp_info(&rle_irwp_info);

			/* 初始化访问关键段 */
			InitializeCriticalSection(&rle_get_info_critical);
			InitializeCriticalSection(&rle_load_img_critical);
			InitializeCriticalSection(&rle_save_img_critical);

			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			/* 销毁访问关键段 */
			DeleteCriticalSection(&rle_get_info_critical);
			DeleteCriticalSection(&rle_load_img_critical);
			DeleteCriticalSection(&rle_save_img_critical);
			break;
    }
    return TRUE;
}
#endif	/* WIN32 */



#ifdef WIN32

RLE_API LPIRWP_INFO CALLAGREEMENT is_irw_plugin()
{
	return (LPIRWP_INFO)&rle_irwp_info;
}

#else

RLE_API LPIRWP_INFO CALLAGREEMENT rle_get_plugin_info()
{
	_init_irwp_info(&rle_irwp_info);

	return (LPIRWP_INFO)&rle_irwp_info;
}

RLE_API void CALLAGREEMENT rle_init_plugin()
{
	/* 初始化多线程同步对象 */
}

RLE_API void CALLAGREEMENT rle_detach_plugin()
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
	/* 各位数减一，否则32位格式无法表示。此处请用RLE正确的位深填写！*/
	lpirwp_info->irwp_save.bitcount = (1UL<<(8-1)) | (1UL<<(24-1));
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
				(const char *)"教育界的格式，当然要大力支持了:)");
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

	/* 别名个数（##需手动修正，如RLE格式有别名请参考BMP插件中别名的设置） */
	lpirwp_info->irwp_desc_info.idi_synonym_count = 0;

	/* 设置初始化完毕标志 */
	lpirwp_info->init_tag = 1;

	return;
}



/* 获取图像信息 */
RLE_API int CALLAGREEMENT rle_get_image_info(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	RLE_HEADER		rle_header;

	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 如果数据包中已有了图像位数据，则不能再改变包中的图像信息 */	

	__try
	{
		__try
		{
			/* 进入关键段 */
			EnterCriticalSection(&rle_get_info_critical);

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

			if (isio_read((void*)&rle_header, sizeof(RLE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 判断指定的图象流是否是RLE格式（只支持8、24、32位1、3、4通道图象） */
			if ((rle_header.magic != RLE_MAGIC) || \
				((rle_header.channels != 1)&&(rle_header.channels != 3)) || \
				(rle_header.chan_bits != 8))
			{
				b_status = ER_NONIMAGE; __leave;
			}

			pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
			pinfo_str->imgformat	= IMF_RLE;			/* 图像文件格式（后缀名） */
			
			/* 填写图像信息 */
			pinfo_str->width	= rle_header.width;
			pinfo_str->height	= rle_header.height;
			pinfo_str->order	= 1;					/* RLE图象都是倒向图（第一象限）*/
			if (rle_header.flags & RLE_ALPHA_F)
			{											/* 此处未使用rle_header中的chan_bits，因为这个值总是8 */
				if (rle_header.channels == 1)
				{
					b_status = ER_NONIMAGE; __leave;	/* 不支持带ALPHA通道的8位索引图 */
				}
				else
				{
					assert(rle_header.channels == 3);
					pinfo_str->bitcount	= (rle_header.channels+1)*8;
				}
				assert(pinfo_str->bitcount == 32);
			}
			else
			{
				pinfo_str->bitcount	=  rle_header.channels*8;
			}

			pinfo_str->compression = ICS_RLE8;

			/* 计算图像掩码数据 */
			switch (pinfo_str->bitcount)
			{
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

			LeaveCriticalSection(&rle_get_info_critical);
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
RLE_API int CALLAGREEMENT rle_load_image(PISADDR psct, LPINFOSTR pinfo_str)
{
	ISFILE			*pfile = (ISFILE*)0;

	int				i, j, k, linesize, end_mark;

	RLE_HEADER		rle_header;
	char			bg[4], filler, bgmark;
	unsigned short	*pal_buf = 0, comm_len;
	unsigned char	*pcomm = 0, *p, oper[2], *pbuf = 0;
	unsigned int	xinc;
	unsigned short	skiplines, skipcol, nc, cor;
	unsigned char	currchann;
				
	enum EXERESULT	b_status = ER_SUCCESS;

	assert(psct&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state < 2);	/* 数据包中不能存在图像位数据 */	

	__try
	{
		__try
		{
			EnterCriticalSection(&rle_load_img_critical);

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

			if (isio_read((void*)&rle_header, sizeof(RLE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 如果该图像还未调用过提取信息函数，则填写图像信息 */
			if (pinfo_str->data_state == 0)
			{
				if ((rle_header.magic != RLE_MAGIC) || \
					((rle_header.channels != 1)&&(rle_header.channels != 3)) || \
					(rle_header.chan_bits != 8))
				{
					b_status = ER_NONIMAGE; __leave;
				}
				
				pinfo_str->imgtype		= IMT_RESSTATIC;	/* 图像文件类型 */
				pinfo_str->imgformat	= IMF_RLE;			/* 图像文件格式（后缀名） */
				
				/* 填写图像信息 */
				pinfo_str->width	= rle_header.width;
				pinfo_str->height	= rle_header.height;
				pinfo_str->order	= 1;
				if (rle_header.flags & RLE_ALPHA_F)
				{
					if (rle_header.channels == 1)
					{
						b_status = ER_NONIMAGE; __leave;
					}
					else
					{
						assert(rle_header.channels == 3);
						pinfo_str->bitcount	= (rle_header.channels+1)*8;
					}
					assert(pinfo_str->bitcount == 32);
				}
				else
				{
					pinfo_str->bitcount	=  rle_header.channels*8;
				}
				
				pinfo_str->compression = ICS_RLE8;
				
				/* 计算图像掩码数据 */
				switch (pinfo_str->bitcount)
				{
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
				
				pinfo_str->data_state = 1;
			}
				
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;

			/* 提取背景色信息 */
			if (rle_header.flags & RLE_NO_BACKGROUND_F)
			{
				/* 跨过填充字节 */
				if (isio_read((void*)&filler, 1, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
				bgmark = 0;
			}
			else
			{
				assert((rle_header.channels == 3)||(rle_header.channels == 1));

				memset((void*)bg, 0, 4);

				/* 读取背景色 */
				if (isio_read((void*)bg, rle_header.channels, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 确定是否需要事先填充背景色 */
				if (rle_header.flags & RLE_CLEARFIRST_F)
				{
					bgmark = 1;
				}
				else
				{
					bgmark = 0;
				}
			}

			/* 设置调色板数据 */
			if (pinfo_str->bitcount <= 8)
			{
				assert(rle_header.channels == 1);
				assert(pinfo_str->bitcount == 8);		/* 当前只支持8位索引图 */

				if ((rle_header.pal_chan_count != 3)||(rle_header.pal_chan_bits != 8))
				{
					b_status = ER_BADIMAGE; __leave;
				}

				/* 申请调色板数据缓冲区 */
				if ((pal_buf=(unsigned short *)malloc(256*3*2)) == 0)	/* 256项、3通道、2字节 */
				{
					b_status = ER_MEMORYERR; __leave;
				}

				/* 读取调色板数据 */
				if (isio_read((void*)pal_buf, (256*3*2), 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 转换至数据包 */
				for (i=0; i<256; i++)
				{
					pinfo_str->palette[i] = (((unsigned long)(pal_buf[i]>>8))<<16) | \
											(((unsigned long)(pal_buf[256+i]>>8))<<8) | \
											((unsigned long)(pal_buf[512+i]>>8));
				}

				pinfo_str->pal_count = 256;
			}
			else
			{
				if (rle_header.pal_chan_count != 0)
				{
					b_status = ER_BADIMAGE; __leave;	/* 非索引图不应存在调色板数据 */
				}

				pinfo_str->pal_count = 0;
			}
			
			
			/* 读取注释信息 */
			if (rle_header.flags & RLE_COMMENT_F)
			{
				/* 读取注释信息长度值（2字节）*/
				if (isio_read((void*)&comm_len, 2, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}

				/* 偶数对齐 */
				comm_len = (comm_len+1)&(unsigned short)0xfffe;

				if (comm_len)
				{
					/* 申请注释信息缓冲区 */
					if ((pcomm=(unsigned char *)malloc(comm_len)) == 0)
					{
						b_status = ER_MEMORYERR; __leave;
					}
					
					/* 读注释信息 */
					if (isio_read((void*)pcomm, comm_len, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
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

			/* 填写行首地址数组（倒向图）*/
			for (i=0;i<(int)(pinfo_str->height);i++)
			{
				pinfo_str->pp_line_addr[i] = (void *)(pinfo_str->p_bit_data+((pinfo_str->height-i-1)*linesize));
			}
			

			/* 填充背景色 */
			if (bgmark)
			{
				k = (int)pinfo_str->bitcount / 8;

				for (i=0; i<(int)pinfo_str->height; i++)
				{
					for (j=0; j<(int)pinfo_str->width; j++)
					{
						memcpy((void*)((unsigned char *)pinfo_str->pp_line_addr[i]+j*k), (const void *)bg, k);
					}
				}
			}

			/* 申请扫描行缓冲区 */
			if ((pbuf=(unsigned char *)malloc(pinfo_str->width*3+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}

			currchann = 0; nc = 0; k = pinfo_str->bitcount/8; end_mark = 0;

			/* 读入并解压象素数据至数据包 */
			for (i=0; i<(int)pinfo_str->height; i+=skiplines)
			{
				if (end_mark)
				{
					b_status = ER_BADIMAGE; __leave;			/* 源图象缺少数据 */
				}

				skiplines = 0; xinc = 0;

				p = pinfo_str->p_bit_data + i*linesize;

				while (!skiplines)
				{
					/* 检查源数据是否合法 */
					if (xinc > pinfo_str->width)
					{
						b_status = ER_BADIMAGE; __leave;
					}

					/* 读取操作命令（包括操作码和单字节操作数）*/
					if (isio_read((void*)oper, 2, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
					
					/* 分解并执行命令 */
					switch (RLE_OPCODE(oper))
					{
					case	RLE_SKIPLINE_OP:		/* 1 － 跳过指定行 */
						if (RLE_LONGP(oper))
						{
							if (isio_read((void*)&skiplines, 2, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave; break;
							}
						}
						else
						{
							skiplines = (unsigned short)RLE_DATUM(oper);
						}
						break;
					case	RLE_SETCOLOR_OP:		/* 2 － 设置当前通道 */
						currchann = RLE_DATUM(oper);

						switch (k)
						{
						case	1:
							if (currchann != 0)		/* 只能有0号通道数据 */
							{
								b_status = ER_BADIMAGE; __leave; break;
							}
							break;
						case	3:
							if (currchann == 0)		/* 将当前通道值转为象素写入位置 */
							{
								currchann = 2;
							}
							else if (currchann == 1)
							{
								currchann = 1;
							}
							else if (currchann == 2)
							{
								currchann = 0;
							}
							else					/* 不允许有其他通道号 */
							{
								b_status = ER_BADIMAGE; __leave; break;
							}
							break;
						case	4:
							if (currchann == 0)
							{
								currchann = 2;
							}
							else if (currchann == 1)
							{
								currchann = 1;
							}
							else if (currchann == 2)
							{
								currchann = 0;
							}
							else if (currchann == (unsigned char)255)	/* ALPHA通道转为象素写入位置 */
							{
								currchann = 3;
							}
							else
							{
								b_status = ER_BADIMAGE; __leave; break;
							}
							break;
						default:
							assert(0); b_status = ER_SYSERR; __leave; break;
						}

						assert(currchann <= 3);

						xinc = 0;					/* 象素索引归0 */

						break;
					case	RLE_SKIPPIXELS_OP:		/* 3 － 跳过指定个数的象素 */
						if (RLE_LONGP(oper))
						{
							if (isio_read((void*)&skipcol, 2, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave; break;
							}
						}
						else
						{
							skipcol = (unsigned short)RLE_DATUM(oper);
						}

						/* 防止发生写溢出 */
						if ((xinc+(unsigned int)skipcol) > pinfo_str->width)
						{
							b_status = ER_BADIMAGE; __leave; break;
						}

						xinc += (unsigned int)skipcol;
						break;
					case	RLE_BYTEDATA_OP:		/* 5 － 未压缩数据 */
						if (RLE_LONGP(oper))
						{
							if (isio_read((void*)&nc, 2, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave; break;
							}
						}
						else
						{
							nc = RLE_DATUM(oper);
						}

						nc++;

						/* 读入未压缩数据（如果可能，也包括填充字节） */
						if (isio_read((void*)pbuf, ((nc+1)/2)*2, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave; break;
						}

						/* 防止发生写溢出 */
						if ((xinc+(unsigned int)nc) > pinfo_str->width)
						{
							b_status = ER_BADIMAGE; __leave; break;
						}

						/* 写象素至数据包 */
						for (j=0; j<(int)(unsigned int)nc; j++)
						{
							p[k*(xinc+j)+(int)(unsigned int)currchann] = pbuf[j];
						}

						xinc += (unsigned int)nc;
						break;
					case	RLE_RUNDATA_OP:			/* 6 － 压缩数据 */
						if (RLE_LONGP(oper))
						{
							if (isio_read((void*)&nc, 2, 1, pfile) == 0)
							{
								b_status = ER_FILERWERR; __leave; break;
							}
						}
						else
						{
							nc = RLE_DATUM(oper);
						}
						
						nc++;

						/* 读压缩字节（和一字节的填充码）*/
						if (isio_read((void*)&cor, 2, 1, pfile) == 0)
						{
							b_status = ER_FILERWERR; __leave; break;
						}

						/* 防止发生写溢出 */
						if ((xinc+(unsigned int)nc) > pinfo_str->width)
						{
							b_status = ER_BADIMAGE; __leave; break;
						}
						
						/* 写象素至数据包 */
						for (j=0; j<(int)(unsigned int)nc; j++)
						{
							p[k*(xinc+j)+(int)(unsigned int)currchann] = (unsigned char)cor;
						}

						xinc += (unsigned int)nc;
						break;
					case	RLE_EOF_OP:				/* 7 － 流终结符 */
						skiplines = 1;				/* 虚值，使while循环终止 */
						end_mark  = 1;
						break;
					default:
						b_status = ER_BADIMAGE; __leave; break;	/* 未定义的操作码 */
					}
				}

				pinfo_str->process_current = i;
				
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

			if (pbuf)
				free(pbuf);

			if (pcomm)
				free(pcomm);

			if (pal_buf)
				free(pal_buf);

			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&rle_load_img_critical);
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		b_status = ER_SYSERR;
	}
	
	return (int)b_status;
}


/* 保存图像 */
RLE_API int CALLAGREEMENT rle_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave)
{
	ISFILE			*pfile = (ISFILE*)0;
	
	RLE_HEADER		rle_header;
	unsigned char	filler = 0;
	unsigned short	commlen, skipline, chann;
	int				i, j, l, k, packlen;
	unsigned char	*psou = 0, *pdec = 0, *p;

	static char				*ptag = "manufacturer=ISee";	/* 注释串，17字节 */
	static unsigned short	pal[3][256];					/* 调色板数据 */

	enum EXERESULT	b_status = ER_SUCCESS;
	
	assert(psct&&lpsave&&pinfo_str);
	assert(pinfo_str->sct_mark == INFOSTR_DBG_MARK);
	assert(pinfo_str->data_state == 2);	/* 必须存在图像位数据 */
	assert(pinfo_str->p_bit_data);
	
	__try
	{
		__try
		{
			EnterCriticalSection(&rle_save_img_critical);
	
			/* 判断是否是受支持的图像数据 */
			if (_rle_is_valid_img(pinfo_str) != 0)
			{
				b_status = ER_NSIMGFOR; __leave;
			}

			if (pinfo_str->break_mark)
			{
				b_status = ER_USERBREAK; __leave;
			}
			
			/* 如果存在同名流，本函数将不进行确认提示 */
			if ((pfile = isio_open((const char *)psct, "wb")) == (ISFILE*)0)
			{
				b_status = ER_FILERWERR; __leave;
			}

			/* 设置进度值 */	
			pinfo_str->process_total = pinfo_str->height;
			pinfo_str->process_current = 0;
			
			/* 填写头结构 */
			rle_header.magic    = (unsigned short)RLE_MAGIC;
			rle_header.xpos     = 0;
			rle_header.ypos     = 0;
			rle_header.width    = (unsigned short)pinfo_str->width;
			rle_header.height   = (unsigned short)pinfo_str->height;
			rle_header.channels	= (unsigned char)pinfo_str->bitcount/8;
			assert((rle_header.channels == 1)||(rle_header.channels == 3));	/* 只允许8位和24位图象 */
			rle_header.chan_bits= 8;
			rle_header.flags    = RLE_NO_BACKGROUND_F | RLE_COMMENT_F;

			if (pinfo_str->bitcount == 8)
			{
				rle_header.pal_chan_count = 3;
				rle_header.pal_chan_bits  = 8;
			}
			else
			{
				rle_header.pal_chan_count = 0;
				rle_header.pal_chan_bits  = 8;
			}

			/* 写入头结构 */
			if (isio_write((const void *)&rle_header, sizeof(RLE_HEADER), 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入填充字节 */
			if (isio_write((const void *)&filler, 1, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 写入调色板数据 */
			if (rle_header.pal_chan_count)
			{
				assert(pinfo_str->bitcount == 8);

				/* 分解原调色板数据 */
				for (i=0; i<3; i++)
				{
					for (j=0; j<256; j++)
					{
						pal[i][j] = (unsigned short)(((pinfo_str->palette[j]>>((2-i)*8))&0xff)<<8);
					}
				}

				/* 写入通道分离后的调色板数据 */
				for (i=0; i<3; i++)
				{
					if (isio_write((const void *)pal[i], 256*2, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
			}
			
			
			commlen = strlen(ptag)+1;		/* 计算注释串长度（包括结尾0符）*/

			/* 写入注释信息 */
			if (isio_write((const void *)&commlen, 2, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			if (isio_write((const void *)ptag, commlen, 1, pfile) == 0)
			{
				b_status = ER_FILERWERR; __leave;
			}
			
			/* 确保偶字节对齐 */
			if (commlen & 0x1)
			{
				/* 写入填充字节 */
				if (isio_write((const void *)&filler, 1, 1, pfile) == 0)
				{
					b_status = ER_FILERWERR; __leave;
				}
			}

			/* 分配压缩缓冲区 */
			if ((psou=(unsigned char *)malloc(pinfo_str->width+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			if ((pdec=(unsigned char *)malloc(pinfo_str->width*3+4)) == 0)
			{
				b_status = ER_MEMORYERR; __leave;
			}
			
			k = pinfo_str->bitcount/8;

			/* 逐行写入数据 */
			for (i=0; i<(int)pinfo_str->height; i++)
			{
				/* 以倒向图方式保存图象 */
				p = (unsigned char *)pinfo_str->pp_line_addr[pinfo_str->height-i-1];

				for (j=0; j<k; j++)
				{
					chann = ((j&0xff)<<8) | (RLE_SETCOLOR_OP&0xff);

					/* 写入通道值 */
					if (isio_write((const void *)&chann, 2, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}

					/* 提取通道数据 */
					for (l=0; l<(int)pinfo_str->width; l++)
					{
						psou[l] = p[k*l+(k-1-j)];
					}
					
					packlen = _rle_pack(psou, (int)pinfo_str->width, pdec);

					/* 写入通道数据 */
					if (isio_write((const void *)pdec, packlen, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				if ((i+1) == (int)pinfo_str->height)	/* 最后一行已经写入 */
				{
					skipline = RLE_EOF_OP;

					/* 写入EOF标志 */
					if (isio_write((const void *)&skipline, 2, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}
				else
				{
					skipline = (0x1<<8) | (RLE_SKIPLINE_OP&0xff);

					/* 写入通道值 */
					if (isio_write((const void *)&skipline, 2, 1, pfile) == 0)
					{
						b_status = ER_FILERWERR; __leave;
					}
				}

				pinfo_str->process_current = i;
				
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
			if (psou)
				free(psou);

			if (pdec)
				free(pdec);
				
			if (pfile)
				isio_close(pfile);
			
			LeaveCriticalSection(&rle_save_img_critical);
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
int CALLAGREEMENT _rle_is_valid_img(LPINFOSTR pinfo_str)
{
	/* ################################################################# */
	/* 位数减一，否则32位格式无法表示。（LONG为32位）*/
	if (!(rle_irwp_info.irwp_save.bitcount & (1UL<<(pinfo_str->bitcount-1))))
		return -1;			/* 不支持的位深图像 */
	/* ################################################################# */

	assert(pinfo_str->imgnumbers);

	if (rle_irwp_info.irwp_save.img_num)
	{
		if (rle_irwp_info.irwp_save.img_num == 1)
		{
			if (pinfo_str->imgnumbers != 1)
			{
				return -2;	/* 图像个数不正确 */
			}
		}
	}

	return 0;
}


#define RLE_UNKNOW		0
#define RLE_REPEAT		1
#define RLE_NONREPEAT	2


/* 压缩一行数据 */
/* 该函数实际上并未压缩数据，因为这种压缩法很原始，做不好的话就会产生反效果 */
static int CALLAGREEMENT _rle_pack(unsigned char *psou, int len, unsigned char *pdec)
{
	assert(psou&&len&&pdec);

	*pdec++ = RLE_BYTEDATA_OP | LONGOPCODE;
	*pdec++ = 0;
	*(unsigned short *)pdec = (unsigned short)(len-1);

	pdec += 2;

	memcpy((void*)pdec, (const void *)psou, len);

	return (len+2+2+1)&0xfffffffe;	/* 压缩后的数据长度，偶字节对齐 */
}



