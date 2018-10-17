/********************************************************************

	BmpModule.cpp - ISee图像浏览器—BMP图像读写模块实现文件
          
    版权所有(C) 2000 VCHelp-coPathway-ISee workgroup member

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用公共许
	可证条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根
	据你的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用公共许可证。

    你应该已经和程序一起收到一份GNU通用公共许可证的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	编写人：YZ
	E-Mail：yzfree@sina.com

	文件版本：
		Beta  1.5
		Build 01209
		Date  2000-12-9

********************************************************************/

#include "stdafx.h"
#include "..\public\gol_isee.h"	// 此文件定义了接口数据包
#include "BmpModule.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char ModuleProcessImgType[]="BMP,DIB,RLE";		// 本模块能处理的图像类型
static char WriterList[]="YZ";							// 本模块的作者列表
static char WriterMess[]="没想到做好BMP模块还真挺复杂^_^";	// 作者留言

static int  RWPROGRESSSIZE;
static const int RWPROGRESSSTEP = 10;

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

//		在图像读写模块中，如果想分配内存，请使用API函数GlobalAlloc()
//		，如果想释放内存请使用GlobalFree()函数。不要使用诸如：new
//		、malloc()等函数。这是为了使各模块之间可以异地释放内存。
//
//

/////////////////////////////////////////////////////////////////////////////
// CBmpModuleApp

BEGIN_MESSAGE_MAP(CBmpModuleApp, CWinApp)
	//{{AFX_MSG_MAP(CBmpModuleApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpModuleApp construction

CBmpModuleApp::CBmpModuleApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBmpModuleApp object

CBmpModuleApp theApp;


// 接口函数声明 — 第一层，唯一与外界联系的接口
int WINAPI AccessBMPModule(INFOSTR *pInfo)
{
	// 这个函数可以不作修改的使用，除非你的返回值多于两种。

	switch(pInfo->comm)
	{
	case	CMD_GETPROCTYPE:	// 获取本模块能处理的图像类型
		_fnCMD_GETPROCTYPE(pInfo);
		break;
	case	CMD_GETWRITERS:		// 获取本模块的作者列表，多人时用逗号分隔
		_fnCMD_GETWRITERS(pInfo);
		break;
	case	CMD_GETWRITERMESS:	// 获取作者们的留言
		_fnCMD_GETWRITERMESS(pInfo);
		break;
	case	CMD_GETBUILDID:		// 获取图像模块内部版本号
		_fnCMD_GETBUILDID(pInfo);
		break;
	case	CMD_IS_VALID_FILE:	// 判断指定文件是否是有效的BMP文件
		_fnCMD_IS_VALID_FILE(pInfo);
		break;
	case	CMD_GET_FILE_INFO:	// 获取指定文件的信息
		_fnCMD_GET_FILE_INFO(pInfo);
		break;
	case	CMD_LOAD_FROM_FILE:	// 从指定图像文件中读取数据
		_fnCMD_LOAD_FROM_FILE(pInfo);
		break;
	case	CMD_SAVE_TO_FILE:	// 将数据保存到指定文件中
		_fnCMD_SAVE_TO_FILE(pInfo);
		break;
	case	CMD_IS_SUPPORT:		// 查询某个命令是否被支持
		_fnCMD_IS_SUPPORT(pInfo);
		break;
	case	CMD_RESIZE:			// 从新获取指定尺寸的图像位数据（只适用于矢量图像）
		_fnCMD_RESIZE(pInfo);
		break;
	default:
		pInfo->result = ER_ILLCOMM; // 非法命令
		ASSERT(FALSE);			// 调用者的程序设计有问题 :-)
		break;
	}

	// 执行命令成功返回1, 失败返回0
	return (pInfo->result==ER_SUCCESS)? 1:0;
}





// 命令解释函数 — 第二层解释函数
//********************************************************************//

// 操作命令解释函数---解释：CMD_IS_SUPPORT命令
// 查询某个命令是否被支持
void _fnCMD_IS_SUPPORT(INFOSTR *pInfo)
{
	// 这个函数是为客户程序查询时使用，如果你实现了对某个命令的
	// 解释，可修改相应的case中的设置，使其返回ER_SUCCESS，这就
	// 表示你的模块已经支持该命令了。同时，现在的这个文件中已包
	// 含了对前四个命令的解释，你只需向还未支持的命令函数中添加
	// 代码即可。

	ASSERT(pInfo->result == ER_EMPTY);

	switch(pInfo->annexdata.cmAnnData)		 
	{
	case	CMD_GETPROCTYPE:	// 获取本模块能处理的图像类型
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GETWRITERS:		// 获取本模块的作者列表，多人时用逗号分隔
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GETWRITERMESS:	// 获取作者们的留言
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GETBUILDID:		// 获取图像模块内部版本号
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_IS_VALID_FILE:	// 判断指定文件是否是有效的WMF文件
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_GET_FILE_INFO:	// 获取指定文件的信息
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_LOAD_FROM_FILE:	// 从指定图像文件中读取数据
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_SAVE_TO_FILE:	// 将数据保存到指定文件中
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_IS_SUPPORT:		// 查询某个命令是否被支持
		pInfo->result = ER_SUCCESS;
		break;
	case	CMD_RESIZE:			// 获取指定尺寸的图像（只适用于矢量图像）
		pInfo->result = ER_NOTSUPPORT;
		break;
	default:
		pInfo->result = ER_NOTSUPPORT;
		break;
	}
}


// 操作命令解释函数---解释：CMD_GETPROCTYPE命令
// 获取本模块能处理的图像类型，如：BMP，PCX等等
void _fnCMD_GETPROCTYPE(INFOSTR *pInfo)
{
	// 这是预定义的函数代码，你可以不必修改的使用。

	// 根据接口定义，此时附加数据应被清空为0，所以下此断言
	ASSERT(pInfo->annexdata.scAnnData[0] == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// 复制能处理的类型字符串
	::CopyMemory((PVOID)pInfo->annexdata.scAnnData, (PVOID)ModuleProcessImgType, 
		sizeof(ModuleProcessImgType));

	pInfo->result = ER_SUCCESS;
}



// 操作命令解释函数---解释：CMD_GETWRITER命令
// 获取本模块的作者列表，多人时用逗号分隔
void _fnCMD_GETWRITERS(INFOSTR *pInfo)
{
	// 这是预定义的函数代码，你可以不必修改的使用。

	// 根据接口定义，此时附加数据应被清空为0，所以下此断言
	ASSERT(pInfo->annexdata.scAnnData[0] == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// 复制开发者名单串
	::CopyMemory((PVOID)pInfo->annexdata.scAnnData, (PVOID)WriterList, 
		sizeof(WriterList));

	pInfo->result = ER_SUCCESS;
}


// 操作命令解释函数---解释：CMD_GETWRITERMESS命令
// 获取作者们的留言
void _fnCMD_GETWRITERMESS(INFOSTR *pInfo)
{
	// 这是预定义的函数代码，你可以不必修改的使用。

	// 根据接口定义，此时附加数据应被清空为0，所以下此断言
	ASSERT(pInfo->annexdata.scAnnData[0] == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// 复制开发者们的留言字符串
	::CopyMemory((PVOID)pInfo->annexdata.scAnnData, (PVOID)WriterMess, 
		sizeof(WriterMess));

	pInfo->result = ER_SUCCESS;
}



// 操作命令解释函数---解释：CMD_GETBUILDID命令
// 获取图像模块内部版本号
void _fnCMD_GETBUILDID(INFOSTR *pInfo)
{
	// 这是预定义的函数代码，你可以不必修改的使用。

	// 根据接口定义，此时annexdata.dwAnnData应被设为0，所以下此断言
	ASSERT(pInfo->annexdata.dwAnnData == 0);
	ASSERT(pInfo->result == ER_EMPTY);
	// 填写内部版本号码
	pInfo->annexdata.dwAnnData = MODULE_BUILDID;

	pInfo->result = ER_SUCCESS;
}



// 操作命令解释函数---解释：CMD_IS_VALID_FILE命令
// 判断指定文件是否是有效的BMP文件
void _fnCMD_IS_VALID_FILE(INFOSTR *pInfo)
{
	CFile file;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;
	DWORD dwSize;
	UINT uRet, uRet2;

	ASSERT(pInfo);

	// 检验入口参数是否符合接口定义
	ASSERT(pInfo->result == ER_EMPTY);
	ASSERT(pInfo->annexdata.iAnnData == 0);
	ASSERT(::strlen(pInfo->filename));
	ASSERT(pInfo->state == PKST_NOTVER);

	// 设初值
	pInfo->result = ER_SUCCESS;
	pInfo->annexdata.iAnnData = 0;	

	// 先判断指定的文件是否存在
	if (!IsFileExist(pInfo->filename))
		pInfo->result = ER_COMMINFOERR;
	else
	{
		// 打开指定文件
		if (!file.Open(pInfo->filename, CFile::modeRead))
		{
			pInfo->result = ER_FILERWERR;	// 打开文件时出错
			return;
		}

		// 获取BMP文件的长度（以字节为单位）
		dwSize = file.GetLength();

		// 用长度判断
		if (dwSize < (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)))
		{
			// 这不是一个BMP文件，BMP文件的长度起码大于文件头
			// 加信息头结构的长度
			file.Close();					
			return;
		}

		// 读取BMP的文件头及信息头结构，并检查它们的有效性
		file.SeekToBegin();
		uRet  = file.Read((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));
		uRet2 = file.Read((LPSTR)&bmiHeader, sizeof(BITMAPINFOHEADER));	
		if ((uRet != sizeof(BITMAPFILEHEADER))||(uRet2 != sizeof(BITMAPINFOHEADER)))
		{
			pInfo->result = ER_FILERWERR;	// 读文件时出错
			file.Close();
			return;
		}

		// 判断文件头部的位图标志（‘BM’）
		if (bmfHeader.bfType != DIB_HEADER_MARKER)
		{
			file.Close();					
			return;
		}

		// 判断信息结构的尺寸
		switch(bmiHeader.biSize)
		{
		case	sizeof(BITMAPINFOHEADER):
			break;
		default:
		//case	sizeof(BITMAPCOREHEADER):	// 不支持OS/2格式的位图
		//case	sizeof(BITMAPV5HEADER):		// 不支持NT5以上系统产生的位图
		//case	sizeof(BITMAPV4HEADER):		// 不支持Windows4.0系统产生的位图，以后支持
			file.Close();					
			return;
		}
		
		// 检验位数据偏移地址
		if ((bmfHeader.bfOffBits > dwSize)||
			(bmfHeader.bfOffBits < (bmiHeader.biSize+sizeof(BITMAPFILEHEADER))))
		{
			file.Close();					
			return;
		}

		// 到此，大致可以表明该文件是一个有效的BMP文件，iAnnData变量设为1
		pInfo->annexdata.iAnnData = 1;
		pInfo->state = PKST_PASSVER;	// 表示通过校验

		file.Close();
	}
}




// 操作命令解释函数---解释：CMD_GET_FILE_INFO命令
// 获取指定文件的信息
void _fnCMD_GET_FILE_INFO(INFOSTR *pInfo)
{
	CFile file;
	CFileStatus	status;
	LPBITMAPINFO pDIB;
	BITMAPFILEHEADER bmfHeader;
	UINT uRet, uDibInfoSize;

	// 检验入口参数是否符合接口定义
	ASSERT(pInfo->result == ER_EMPTY);
	ASSERT(::strlen(pInfo->filename));
	// 此时，该文件必需是一个已存在的、并且是有效的BMP文件
	ASSERT(pInfo->state == PKST_PASSVER);
	// 客户模块必需要先将imginfo清空为0
	ASSERT(pInfo->imginfo.imgtype == IMT_NULL);

	// 打开指定文件
	if (!file.Open(pInfo->filename, CFile::modeRead))
	{
		pInfo->result = ER_FILERWERR;
		return;
	}

	// 读取BMP的文件头结构
	file.SeekToBegin();
	uRet  = file.Read((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));

	if (uRet != sizeof(BITMAPFILEHEADER))
	{
		file.Close();
		pInfo->result = ER_FILERWERR;
		return;
	}

	uDibInfoSize = bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER);

	// 分配内存，用于存放信息头及颜色表（如果有）
	pDIB = (LPBITMAPINFO)::GlobalAlloc(GPTR, uDibInfoSize);
	if (!pDIB)
	{
		file.Close();
		pInfo->result = ER_MEMORYERR;
		return;
	}

	// 读取图像信息数据
	file.Seek((LONG)sizeof(BITMAPFILEHEADER), CFile::begin);
	uRet = file.Read((void *)pDIB, uDibInfoSize);
	if (uRet != uDibInfoSize)
	{
		::GlobalFree((void*)pDIB);
		file.Close();
		pInfo->result = ER_FILERWERR;
		return;
	}

	LPIMAGEINFOSTR lpImgInfoStr = &pInfo->imginfo;

	// 获取文件的长度、图像的宽度、高度等信息
	lpImgInfoStr->imgtype = IMT_RESSTATIC;
	lpImgInfoStr->imgformat = IMF_BMP;
	lpImgInfoStr->filesize = file.GetLength();
	lpImgInfoStr->width = pDIB->bmiHeader.biWidth;
	// 图像的高度值有时可能是负值，所以使用了abs()函数
	lpImgInfoStr->height = (DWORD)::abs(pDIB->bmiHeader.biHeight);
	lpImgInfoStr->bitcount = (DWORD)pDIB->bmiHeader.biBitCount;
	switch(pDIB->bmiHeader.biCompression)
	{
	case	BI_RGB:		//0L
		lpImgInfoStr->compression = ICS_RGB; break;
	case	BI_RLE8:	//1L
		lpImgInfoStr->compression = ICS_RLE8; break;
	case	BI_RLE4:	//2L
		lpImgInfoStr->compression = ICS_RLE4; break;
	case	BI_BITFIELDS://3L
		lpImgInfoStr->compression = ICS_BITFIELDS; break;
	default:
		//case	BI_PNG:				// 以后的版本将支持这两种位格式
		//case	BI_JPEG:
		ASSERT(FALSE);
	}
	// 每一图像行所占的字节数（DWORD对齐，并且只对非压缩位图有效）
	lpImgInfoStr->linesize = DIBSCANLINE_WIDTHBYTES(pDIB->bmiHeader.biWidth*pDIB->bmiHeader.biBitCount);
	lpImgInfoStr->imgnumbers = 1;	// BMP文件中只有一个图像
	lpImgInfoStr->imgchang = 0;		// 表示可以被编辑

	// 获取文件最后的修改日期（月在高字节，日在低字节）
	file.GetStatus(status);
	lpImgInfoStr->year = (WORD)status.m_mtime.GetYear();
	lpImgInfoStr->monday = (WORD)status.m_mtime.GetMonth();
	lpImgInfoStr->monday <<= 8;
	lpImgInfoStr->monday |= (WORD)status.m_mtime.GetDay();

	// 获取文件最后的修改时间（字序：最高—0, 2—时，1—分，0—秒）
	lpImgInfoStr->time = status.m_mtime.GetHour();
	lpImgInfoStr->time <<= 8;
	lpImgInfoStr->time |= status.m_mtime.GetMinute();
	lpImgInfoStr->time <<= 8;
	lpImgInfoStr->time |= status.m_mtime.GetSecond();
	lpImgInfoStr->time &= 0xffffff;

	::GlobalFree((void*)pDIB);
	file.Close();

	// 设置出口数据
	pInfo->state = PKST_PASSINFO;
	pInfo->result = ER_SUCCESS;
}



// 操作命令解释函数---解释：CMD_LOAD_FROM_FILE命令
// 从指定图像文件中读取数据
void _fnCMD_LOAD_FROM_FILE(INFOSTR *pInfo)
{
	// 检验入口参数是否符合接口定义
	ASSERT(pInfo->result == ER_EMPTY);
	ASSERT(::strlen(pInfo->filename));
	// 此时，该文件必需是一个已存在的、有效的BMP文件，并且数据包中
	// 含有该文件的信息（imginfo结构中）
	ASSERT(pInfo->state == PKST_PASSINFO);
	ASSERT(pInfo->imginfo.imgformat == IMF_BMP);
	ASSERT(pInfo->pImgInfo == NULL);
	// 必需设置标准图像格式信息
	ASSERT(pInfo->sDIBInfo.bmi.biSize == sizeof(BITMAPINFOHEADER));
	ASSERT(pInfo->pLineAddr != NULL);
	ASSERT(pInfo->_pbdata != NULL);

	CFile file;

	// 以图像的高度值作为进度总值
	RWPROGRESSSIZE = (int)pInfo->imginfo.height;

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// 如果进度函数返回1，则说明用户想中断操作，返回。
			pInfo->result = ER_USERBREAK;
			return;
		}
	}

	// 打开指定文件
	if (!file.Open(pInfo->filename, CFile::modeRead))
	{
		pInfo->result = ER_FILERWERR;
		return;
	}
	file.Seek(0, CFile::begin);

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// 如果进度函数返回1，则说明用户想中断操作，返回。
			file.Close();
			pInfo->result = ER_USERBREAK;
			return;
		}
	}

	// 读取并转换图像到数据包中
	if (_Read(file, pInfo) == 0)
	{
		// 成功
		pInfo->state = PKST_INFOANDBITS;
		pInfo->modify = 0;
		pInfo->result = ER_SUCCESS;
	}
	
	file.Close();

	if (pInfo->fpProgress)	// 结束进度条，此调用不再支持用户中断
		(*pInfo->fpProgress)(RWPROGRESSSIZE, RWPROGRESSSIZE);

}



// 操作命令解释函数---解释：CMD_SAVE_TO_FILE命令
// 将数据保存到指定文件中

// pInfo所指向的数据包中，annexdata.siAnnData[]变量的含意解释：
//		[0] — 位深度，可以是1、4、8、16(555)、24、32(888)这六个值
//				中的一个，不能是其他值。另：不能存储为RLE格式
//		[1] — 是否使用原始调色板。0 — 表示使用，1 — 表示使用当前
//				计算机缺省调色板
void _fnCMD_SAVE_TO_FILE(INFOSTR *pInfo)
{
	// 检验入口参数是否符合接口定义
	ASSERT(pInfo->result == ER_EMPTY);
	// 该文件名可以是一个已存在的文件，也可以是不存在的文件（本函数将创建）。
	ASSERT(::strlen(pInfo->filename));
	ASSERT(pInfo->state == PKST_INFOANDBITS);
	// 必需设置标准图像格式信息
	ASSERT(pInfo->sDIBInfo.bmi.biSize == sizeof(BITMAPINFOHEADER));
	ASSERT(pInfo->pLineAddr != NULL);
	ASSERT(pInfo->_pbdata != NULL);

	CFile file;
	CFileStatus	status;
	BITMAPFILEHEADER	bmf;
	int	uDibInfoSize;
	LPBITMAPINFO pDIB;
	int uRet;

	// 此时imginfo中的数据可能与sDIBInfo不一样，以sDIBInfo为准
	RWPROGRESSSIZE = (int)::abs(pInfo->sDIBInfo.bmi.biHeight);

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	// 如果进度函数返回1，则说明用户想中断操作，返回。
			pInfo->result = ER_USERBREAK;
			return;
		}
	}

	// 打开指定文件（如果不存在就创建，如果存在，不截断为0长度），不
	// 允许其它进程写该文件
	if (!file.Open(pInfo->filename, CFile::modeCreate|CFile::modeNoTruncate|
			CFile::modeReadWrite|CFile::shareDenyWrite))
	{
		pInfo->result = ER_FILERWERR;
		return;
	}
	file.Seek(0, CFile::begin);

	LPIMAGEINFOSTR lpImgInfoStr = &pInfo->imginfo;

	// 读取并转换图像到数据包中
	if (_Save(file, pInfo) == 0)
	{
		file.Flush();	// 刷新文件的内容、尺寸、最后修改日期

		file.Seek(0, CFile::begin);
		if (file.Read((void*)&bmf, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER))
		{
			file.Close();
			pInfo->result = ER_FILERWERR;
			return;
		}

		uDibInfoSize = bmf.bfOffBits-sizeof(BITMAPFILEHEADER);

		// 分配内存，用于存放信息头及颜色表（如果有）
		pDIB = (LPBITMAPINFO)::GlobalAlloc(GPTR, uDibInfoSize);
		if (!pDIB)
		{
			file.Close();
			pInfo->result = ER_MEMORYERR;
			return;
		}

		// 读取图像信息数据
		file.Seek((LONG)sizeof(BITMAPFILEHEADER), CFile::begin);
		uRet = file.Read((void *)pDIB, uDibInfoSize);
		if (uRet != uDibInfoSize)
		{
			::GlobalFree((void*)pDIB);
			file.Close();
			pInfo->result = ER_FILERWERR;
			return;
		}

		if (pInfo->pImgInfo)	// 释放图像的原始信息块
			::GlobalFree((void*)pInfo->pImgInfo);

		pInfo->pImgInfo = (void*)pDIB;	// 设置原始信息块

		// 获取文件的长度、图像的宽度、高度等信息
		lpImgInfoStr->imgtype = IMT_RESSTATIC;
		lpImgInfoStr->imgformat = IMF_BMP;
		lpImgInfoStr->filesize = file.GetLength();
		lpImgInfoStr->width = pInfo->sDIBInfo.bmi.biWidth;
		// 图像的高度值有时可能是负值，所以使用了abs()函数
		lpImgInfoStr->height = (DWORD)::abs(pInfo->sDIBInfo.bmi.biHeight);
		lpImgInfoStr->bitcount = (DWORD)pInfo->annexdata.siAnnData[0];	// 新的位深度值
		lpImgInfoStr->compression = ICS_RGB;	// 写函数当前只支持BI_RGB格式

		// 每一图像行所占的字节数（DWORD对齐，并且只对非压缩位图有效）
		lpImgInfoStr->linesize = DIBSCANLINE_WIDTHBYTES(lpImgInfoStr->width*lpImgInfoStr->bitcount);
		lpImgInfoStr->imgnumbers = 1;	// BMP文件中只有一个图像
		lpImgInfoStr->imgchang = 0;		// 表示可以被编辑

		// 获取文件最后的修改日期（月在高字节，日在低字节）
		file.GetStatus(status);
		lpImgInfoStr->year = (WORD)status.m_mtime.GetYear();
		lpImgInfoStr->monday = (WORD)status.m_mtime.GetMonth();
		lpImgInfoStr->monday <<= 8;
		lpImgInfoStr->monday |= (WORD)status.m_mtime.GetDay();

		// 获取文件最后的修改时间（字序：最高—0, 2—时，1—分，0—秒）
		lpImgInfoStr->time = status.m_mtime.GetHour();
		lpImgInfoStr->time <<= 8;
		lpImgInfoStr->time |= status.m_mtime.GetMinute();
		lpImgInfoStr->time <<= 8;
		lpImgInfoStr->time |= status.m_mtime.GetSecond();
		lpImgInfoStr->time &= 0xffffff;

		// 成功
		pInfo->result = ER_SUCCESS;
	}
	
	file.Close();

	if (pInfo->fpProgress)	// 结束进度条，此调用不再支持用户中断
		(*pInfo->fpProgress)(RWPROGRESSSIZE, RWPROGRESSSIZE);
}




// 操作命令解释函数---解释：CMD_RESIZE命令
// 重新获取指定尺寸的图像位数据（只适用于矢量图像）
void _fnCMD_RESIZE(INFOSTR *pInfo)
{
	// 这个命令一般的图像读写模块不需要支持，它只适用于矢量图像，
	// 比如WMF、EMF之类。
	// 如果你想解释该命令，请在下面加入代码，并修改pInfo->result的返回值：
	// ----------------------------------------------------------------->

	pInfo->result = ER_NOTSUPPORT;
}



/*************************************************************************
 *
 * IsFileExist()
 * 
 * 参数说明：
 *
 * char *lpFileName		- 待判断的文件路径和名称（文件名）
 *
 * 返回值：
 *
 * BOOL					- 如果指定的文件存在返回TRUE，否则返回FALSE。
 * 
 * 描述：
 *
 * 判断指定的文件是否存在
 * 
 * 该文件必需可以被读和写
 *
 ************************************************************************/


BOOL IsFileExist(char *lpFileName)
{
	CFile	file;
	BOOL	bExist = FALSE;	// 文件存在是TRUE，不存在是FALSE
	CFileException	e;

	// 确定指定的文件是否存在
	if (file.Open(lpFileName, CFile::modeReadWrite|CFile::shareDenyNone, &e))
	{
		bExist = TRUE;
		file.Close();
	}
	else
	{
		// 可能有其他程序正在处理此文件
		switch(e.m_cause)
		{
		case	CFileException::tooManyOpenFiles:
		case	CFileException::accessDenied:
		case	CFileException::sharingViolation:
		case	CFileException::lockViolation:
			return TRUE;
		case	CFileException::fileNotFound:
		case	CFileException::badPath:
		case	CFileException::invalidFile:
		case	CFileException::hardIO:
		default:
			return FALSE;
		}
	}

	return bExist;
}


// 将指定文件读入目标缓冲区
int _Read(CFile& file, LPINFOSTR pInfo)
{
	RGBQUAD *pPalette;
	int imgorg;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;
	PBYTE pBits;
	PBYTE pImgInfo;
	DWORD dwSize, dwRet;
	UINT uRet;

	/* 读取DIB的文件头结构 */
	file.SeekToBegin();
	uRet   = file.Read((LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER));
	uRet  += file.Read((LPSTR)&bmiHeader, sizeof(BITMAPINFOHEADER));
	if (uRet != (sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)))
	{
		pInfo->result = ER_FILERWERR;
		return 1;	// 读文件时出错
	}

	// pImgInfo将包含位图信息结构及调色板信息（如果有的话）
	pImgInfo = (PBYTE)::GlobalAlloc(GPTR, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER));
	if (pImgInfo == NULL)
	{
		pInfo->result = ER_MEMORYERR;
		return 2;	// 内存不足
	}

	// 读取位图文件的原始信息
	file.Seek(sizeof(BITMAPFILEHEADER), CFile::begin);
	uRet = file.Read((void*)pImgInfo, bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER));
	if (uRet != (bmfHeader.bfOffBits-sizeof(BITMAPFILEHEADER)))
	{
		::GlobalFree(pImgInfo);
		pInfo->result = ER_FILERWERR;
		return 1;
	}

	// 正常情况下，高度值不会是负值或0
	ASSERT(RWPROGRESSSIZE > 0);

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// 如果进度函数返回1，则说明用户想中断操作，返回。
			::GlobalFree(pImgInfo);
			pInfo->result = ER_USERBREAK;
			return 4;	// 用户中断操作
		}
	}

	// 获取文件长度
	dwSize = file.GetLength();

	// 分配内存，用于存放位数据
	pBits = (PBYTE)::GlobalAlloc(GPTR, dwSize-bmfHeader.bfOffBits);
	if (!pBits)
	{
		::GlobalFree(pImgInfo);
		pInfo->result = ER_MEMORYERR;
		return 2;	// 内存不足
	}

	// 读取位数据
	file.Seek(bmfHeader.bfOffBits, CFile::begin);
	dwRet = file.ReadHuge((void*)pBits, (dwSize-bmfHeader.bfOffBits));

	if (dwRet != (dwSize-bmfHeader.bfOffBits))
	{
		::GlobalFree(pImgInfo);
		::GlobalFree(pBits);
		pInfo->result = ER_FILERWERR;
		return 1;
	}

	if (pInfo->fpProgress)
	{
		if ((*pInfo->fpProgress)(RWPROGRESSSIZE, 0))
		{	
			// 如果进度函数返回1，则说明用户想中断操作，返回。
			::GlobalFree(pImgInfo);
			::GlobalFree(pBits);
			pInfo->result = ER_USERBREAK;
			return 4;	// 用户中断操作
		}
	}

	// 下面的读取函数需要该指针，所以在此处赋值
	pInfo->pImgInfo = pImgInfo;

	pPalette = (RGBQUAD*)(pImgInfo+bmiHeader.biSize);
	imgorg = (bmiHeader.biHeight > 0) ? 0:1;	// 0-为倒向位图  1-为正向
	int result = 0;		// 接收各函数的返回值

	// 根据位图文件的位深度的不同而调用相应的读取函数
	switch(_get_souformat((LPBITMAPINFO)pImgInfo))
	{
	case	SF_1:
		result = _from_1_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_4:
		result = _from_4_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_4_RLE:
		result = _from_4_RLE_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_8:
		result = _from_8_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_8_RLE:
		result = _from_8_RLE_read(pInfo, pBits, imgorg, pPalette);
		break;
	case	SF_16_555:
		result = _from_16_555_read(pInfo, pBits, imgorg);
		break;
	case	SF_16_565:
		result = _from_16_565_read(pInfo, pBits, imgorg);
		break;
	case	SF_24:
		result = _from_24_read(pInfo, pBits, imgorg);
		break;
	case	SF_32:
		result = _from_32_read(pInfo, pBits, imgorg);
		break;
	default:
		::GlobalFree(pImgInfo);
		::GlobalFree(pBits);
		pInfo->pImgInfo = NULL;
		pInfo->result = ER_SYSERR;
		return 3;		// 图像文件格式异常
	}

	// 释放从文件中读取的位数据（此数据已被转换并保存到了数据包中）
	::GlobalFree(pBits);
	
	if (result == 0)	// 执行成功
	{
		pInfo->result = ER_SUCCESS;
	}
	else
	{
		::GlobalFree(pImgInfo);
		pInfo->pImgInfo = NULL;
		return 4;		// 执行过程中产生的错误，详见pInfo->result
	}

	return 0;
}



// 将1位（即单色）位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_1_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 单色位图应有两个调色板项
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int sImgOrg = imgorg;	// 源图像扫描行次序的标志：0-为倒向 1-为正向
							// 源图像的次序由调用者传入

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	int heigCount, widthCount, i, j, isou;
	ASSERT(pInfo->pImgInfo);
	int palnum = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
	PBYTE pCurPixAddr;
	BYTE pix;	// 当前8个像素

	palnum = (palnum == 0) ? 2:palnum;	// 计算调色板项的实际个数
	ASSERT(palnum <= 2);

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		// 以555位格式的调色板数据
		WORD descol555_0 = 0;
		WORD descol555_1 = 0;

		if (palnum > 0)
			descol555_0 = _cnv_rgb_to_555(lpPalette[0].rgbRed, lpPalette[0].rgbGreen, lpPalette[0].rgbBlue);
		if (palnum > 1)
			descol555_1 = _cnv_rgb_to_555(lpPalette[1].rgbRed, lpPalette[1].rgbGreen, lpPalette[1].rgbBlue);

		WORD *pDesPixAddr555;

		if (sImgOrg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						// 最高位代表最左面的像素
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr555++ = descol555_1;
						}
						else
						{
							*pDesPixAddr555++ = descol555_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr555++ = descol555_1;
						}
						else
						{
							*pDesPixAddr555++ = descol555_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		// 以565位格式的调色板数据
		WORD descol565_0 = 0;
		WORD descol565_1 = 0;

		if (palnum > 0)
			descol565_0 = _cnv_rgb_to_565(lpPalette[0].rgbRed, lpPalette[0].rgbGreen, lpPalette[0].rgbBlue);
		if (palnum > 1)
			descol565_1 = _cnv_rgb_to_565(lpPalette[1].rgbRed, lpPalette[1].rgbGreen, lpPalette[1].rgbBlue);

		WORD *pDesPixAddr565;

		if (sImgOrg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr565++ = descol565_1;
						}
						else
						{
							*pDesPixAddr565++ = descol565_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr565++ = descol565_1;
						}
						else
						{
							*pDesPixAddr565++ = descol565_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		// 24位格式的调色板数据
		BYTE red0, red1, green0, green1, blue0, blue1;

		if (palnum > 0)
		{
			red0 = lpPalette[0].rgbRed;
			green0 = lpPalette[0].rgbGreen;
			blue0 = lpPalette[0].rgbBlue;
		}
		if (palnum > 1)
		{
			red1 = lpPalette[1].rgbRed;
			green1 = lpPalette[1].rgbGreen;
			blue1 = lpPalette[1].rgbBlue;
		}

		PBYTE pDesPixAddr24;

		if (sImgOrg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr24++ = blue1;
							*pDesPixAddr24++ = green1;
							*pDesPixAddr24++ = red1;
						}
						else
						{
							*pDesPixAddr24++ = blue0;
							*pDesPixAddr24++ = green0;
							*pDesPixAddr24++ = red0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr24++ = blue1;
							*pDesPixAddr24++ = green1;
							*pDesPixAddr24++ = red1;
						}
						else
						{
							*pDesPixAddr24++ = blue0;
							*pDesPixAddr24++ = green0;
							*pDesPixAddr24++ = red0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		// 以888位格式的调色板数据
		DWORD descol888_0 = 0;
		DWORD descol888_1 = 0;

		if (palnum > 0)
			descol888_0 = _cnv_rgb_to_888(lpPalette[0].rgbRed, lpPalette[0].rgbGreen, lpPalette[0].rgbBlue);
		if (palnum > 1)
			descol888_1 = _cnv_rgb_to_888(lpPalette[1].rgbRed, lpPalette[1].rgbGreen, lpPalette[1].rgbBlue);

		DWORD *pDesPixAddr888;

		if (sImgOrg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr888++ = descol888_1;
						}
						else
						{
							*pDesPixAddr888++ = descol888_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=8)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<8)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 8;

					for (i=0;i<j;i++)
					{
						if (((BYTE)(0x80>>i))&pix)	// 此位被设置
						{
							*pDesPixAddr888++ = descol888_1;
						}
						else
						{
							*pDesPixAddr888++ = descol888_0;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// 将4位（即16色）位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_4_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 16色位图应有调色板
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	int heigCount, widthCount, i, j, isou;
	ASSERT(pInfo->pImgInfo);
	int palnum = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
	PBYTE pCurPixAddr;
	BYTE pix;	// 当前2个像素

	palnum = (palnum == 0) ? 16:palnum;	// 计算调色板项的实际个数
	ASSERT(palnum <= 16);

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		// 以555位格式的调色板数据
		WORD descol555[16];
		for (i=0;i<palnum;i++)	// 初始化色素颜色数据
			descol555[i] = _cnv_rgb_to_555(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr555;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr555++ = descol555[pix>>4];
						else
							*pDesPixAddr555++ = descol555[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr555++ = descol555[pix>>4];
						else
							*pDesPixAddr555++ = descol555[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		// 以565位格式的调色板数据
		WORD descol565[16];
		for (i=0;i<palnum;i++)	// 初始化色素颜色数据
			descol565[i] = _cnv_rgb_to_565(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr565;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr565++ = descol565[pix>>4];
						else
							*pDesPixAddr565++ = descol565[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i==0)
							*pDesPixAddr565++ = descol565[pix>>4];
						else
							*pDesPixAddr565++ = descol565[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		int index24_0, index24_1;

		PBYTE pDesPixAddr24;

		if (imgorg  == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					index24_0 = (int)(pix>>4);
					index24_1 = (int)(pix&0xf);

					for (i=0;i<j;i++)
					{
						if (i==0)
						{
							*pDesPixAddr24++ = lpPalette[index24_0].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbRed;
						}
						else
						{
							*pDesPixAddr24++ = lpPalette[index24_1].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbRed;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					index24_0 = (int)(pix>>4);
					index24_1 = (int)(pix&0xf);

					for (i=0;i<j;i++)
					{
						if (i==0)
						{
							*pDesPixAddr24++ = lpPalette[index24_0].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_0].rgbRed;
						}
						else
						{
							*pDesPixAddr24++ = lpPalette[index24_1].rgbBlue;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbGreen;
							*pDesPixAddr24++ = lpPalette[index24_1].rgbRed;
						}
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		// 以888位格式的调色板数据
		DWORD descol888[16];
		for (i=0;i<palnum;i++)	// 初始化色素颜色数据
			descol888[i] = _cnv_rgb_to_888(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		DWORD *pDesPixAddr888;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						// 注：在4位位图中，每个字节的高4位代表两个像素
						//     中左面的那个像素，例如某扫描行的第一个像
						//     素是红色，第二个像素是白色，红的索引是A,
						//     白的索引是9，那么就应该写成9A，而不能写成
						//     A9，虽然习惯、逻辑上是那样。
						if (i == 0)
							*pDesPixAddr888++ = descol888[pix>>4];
						else
							*pDesPixAddr888++ = descol888[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount+=2)
				{
					pix = *pCurPixAddr++;

					if ((imgwidth-widthCount)<2)	// 结尾判断
						j = imgwidth-widthCount;
					else
						j = 2;

					for (i=0;i<j;i++)
					{
						if (i == 0)
							*pDesPixAddr888++ = descol888[pix>>4];
						else
							*pDesPixAddr888++ = descol888[pix&0xf];
					}
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// 将4位RLE位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_4_RLE_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 16色位图应有调色板
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSouData);
	ASSERT(imgorg == 0);	// 压缩位图必需是倒向的

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	PBYTE lpSou = lpSouData;
	BYTE pix, pix2;	// 当前值
	int result, i;

	BYTE **lppLineAddr = (BYTE**)::GlobalAlloc(GPTR, sizeof(PBYTE)*imgheight);
	if (lppLineAddr == NULL)
	{
		pInfo->result = ER_MEMORYERR;
		return -2;	// 内存不足，返回
	}

	int linesize = ((imgwidth*4+31)/32)*4;

	PBYTE lpDes = (PBYTE)::GlobalAlloc(GPTR, linesize*imgheight);
	if (lpDes == NULL)
	{
		::GlobalFree(lppLineAddr);
		pInfo->result = ER_MEMORYERR;
		return -2;	// 内存不足，返回
	}

	// 初始化行地址数组
	for (i=0;i<imgheight;i++)
		lppLineAddr[i] = (PBYTE)(lpDes+linesize*i);

	BOOL bEOF = FALSE;		// 图像结尾标志
	int  line = 0;			// 当前行

	_write2buff4RLE(0, 0, 0, lppLineAddr[line]);	// 初始化写函数

	// 先将RLE的数据解压成标准的BMP数据
	while(!bEOF)
	{
		pix = *lpSou++;

		if (pix == 0)	// 绝对方式
		{
			pix2 = *lpSou++;
			switch(pix2)
			{
			case	0:		// 行结尾
				line++;
				_write2buff4RLE(2, 0, 0, lppLineAddr[line]);// 指定新的偏移地址
				break;
			case	1:		// 图像结尾
				bEOF = TRUE;
				break;
			case	2:		// 当前地址偏移
				pix = *lpSou++; pix2 = *lpSou++;
				line += pix2;	// 下移pix2行
				_write2buff4RLE(2, pix%2, 0, (PBYTE)(lppLineAddr[line]+(DWORD)pix/2));
				break;
			default:		// 不能压缩的像素
				for (i=0;i<pix2/2;i++)
				{
					pix = *(lpSou+i);
					_write2buff4RLE(1, 2, pix, NULL);
				}
				if (pix2%2)
				{	// 补充遗漏的单个像素
					pix = *(lpSou+i);
					_write2buff4RLE(1, 1, pix, NULL);
				}
				// (pix2+1)/2是将像素对齐到字节边界上，再加1、除2是对齐到字边界上
				lpSou = (PBYTE)(lpSou + ((pix2+1)/2+1)/2*2);	// 字对齐
				break;
			}
		}
		else			// 编码方式
		{
			// pix == 相同像素的个数
			pix2 = *lpSou++;	// pix2 == 像素
			_write2buff4RLE(1, (int)pix, pix2, NULL);
		}
	}

	_write2buff4RLE(3, 0, 0, NULL);	// 结束写函数

	::GlobalFree(lppLineAddr);

	// 用标准4位转换函数转换到目标格式
	result = _from_4_read(pInfo, lpDes, 0, lpPalette);

	::GlobalFree(lpDes);

	return result;
}



// 将8位（即256色）位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_8_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 256色位图应有调色板
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	ASSERT(pInfo->pImgInfo);
	int palnum = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
	int heigCount, widthCount, i, isou;
	PBYTE pCurPixAddr;
	BYTE pix;	// 当前像素

	palnum = (palnum == 0) ? 256:palnum;	// 计算调色板项的实际个数
	ASSERT(palnum <= 256);

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		// 以555位格式的调色板数据
		WORD descol555[256];
		for (i=0;i<palnum;i++)	// 初始化色素颜色数据
			descol555[i] = _cnv_rgb_to_555(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr555;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = descol555[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = descol555[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		// 以565位格式的调色板数据
		WORD descol565[256];
		for (i=0;i<palnum;i++)	// 初始化色素颜色数据
			descol565[i] = _cnv_rgb_to_565(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		WORD *pDesPixAddr565;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = descol565[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = descol565[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		if (imgorg  == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					*pDesPixAddr24++ = lpPalette[pix].rgbBlue;
					*pDesPixAddr24++ = lpPalette[pix].rgbGreen;
					*pDesPixAddr24++ = lpPalette[pix].rgbRed;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					*pDesPixAddr24++ = lpPalette[pix].rgbBlue;
					*pDesPixAddr24++ = lpPalette[pix].rgbGreen;
					*pDesPixAddr24++ = lpPalette[pix].rgbRed;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		// 以888位格式的调色板数据
		DWORD descol888[256];
		for (i=0;i<palnum;i++)	// 初始化色素颜色数据
			descol888[i] = _cnv_rgb_to_888(lpPalette[i].rgbRed, lpPalette[i].rgbGreen, lpPalette[i].rgbBlue);

		DWORD *pDesPixAddr888;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr888++ = descol888[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr888++ = descol888[*pCurPixAddr++];
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}




// 将8位RLE位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_8_RLE_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette)
{
	ASSERT(pInfo);
	ASSERT(lpPalette);	// 256色位图应有调色板
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSouData);
	ASSERT(imgorg == 0);	// 压缩位图必需是倒向的

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	PBYTE lpSou = lpSouData;
	BYTE pix, pix2;	// 当前值
	int result, i;

	BYTE **lppLineAddr = (BYTE**)::GlobalAlloc(GPTR, sizeof(PBYTE)*imgheight);
	if (lppLineAddr == NULL)
	{
		pInfo->result = ER_MEMORYERR;
		return -2;	// 内存不足，返回
	}

	int linesize = ((imgwidth+3)/4)*4;	// 扫描行尺寸(in byte)

	PBYTE lpDes = (PBYTE)::GlobalAlloc(GPTR, linesize*imgheight);
	if (lpDes == NULL)
	{
		::GlobalFree(lppLineAddr);
		pInfo->result = ER_MEMORYERR;
		return -2;	// 内存不足，返回
	}

	// 初始化行地址数组
	for (i=0;i<imgheight;i++)
		lppLineAddr[i] = (PBYTE)(lpDes+linesize*i);

	BOOL bEOF = FALSE;		// 图像结尾标志
	int  line = 0;			// 当前行

	_write2buff8RLE(0, 0, 0, lppLineAddr[line]);	// 初始化写函数

	while(!bEOF)
	{
		pix = *lpSou++;

		if (pix == 0)	// 绝对方式
		{
			pix2 = *lpSou++;
			switch(pix2)
			{
			case	0:		// 行结尾
				line++;
				_write2buff8RLE(2, 0, 0, lppLineAddr[line]);// 指定新的偏移地址
				break;
			case	1:		// 图像结尾
				bEOF = TRUE;
				break;
			case	2:		// 当前地址偏移
				pix = *lpSou++; pix2 = *lpSou++;
				line += pix2;	// 下移pix2行
				_write2buff8RLE(2, 0, 0, (PBYTE)(lppLineAddr[line]+(DWORD)pix));
				break;
			default:		// 不能压缩的像素
				for (i=0;i<pix2;i++)
				{
					pix = *(lpSou+i);
					_write2buff8RLE(1, 1, pix, NULL);
				}
				// (pix2+1)/2是将像素对齐到字节边界上，再加1、除2是对齐到字边界上
				lpSou = (PBYTE)(lpSou + ((pix2+1)/2)*2);	// 字对齐
				break;
			}
		}
		else			// 编码方式
		{
			// pix == 相同像素的个数
			pix2 = *lpSou++;	// pix2 == 像素
			_write2buff8RLE(1, (int)pix, pix2, NULL);
		}
	}

	_write2buff8RLE(3, 0, 0, NULL);	// 结束写函数

	::GlobalFree(lppLineAddr);

	// 用标准8位转换函数转换到目标格式
	result = _from_8_read(pInfo, lpDes, 0, lpPalette);

	::GlobalFree(lpDes);

	return result;
}




// 将16位555位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_16_555_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	int heigCount, widthCount, isou;
	WORD *pCurPixAddr;
	WORD pix;	// 当前像素

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr555++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)	// 实现从555到565的转换
					*pDesPixAddr565++ = (((*pCurPixAddr)&0xffe0)<<1)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)	// 实现从555到565的转换
					*pDesPixAddr565++ = (((*pCurPixAddr)&0xffe0)<<1)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		BYTE  red, green, blue;

		if (imgorg  == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_555_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_555_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		DWORD *pDesPixAddr888;
		DWORD pix32;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// 实现555到888的转换
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<6)&0xf800)|((pix32<<9)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// 实现555到888的转换
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<6)&0xf800)|((pix32<<9)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}




// 将16位565位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_16_565_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	int heigCount, widthCount, isou;
	WORD *pCurPixAddr;
	WORD pix;	// 当前像素

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++) // 实现从565到555的转换
					*pDesPixAddr555++ = (((*pCurPixAddr)>>1)&0x7fe0)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++) // 实现从565到555的转换
					*pDesPixAddr555++ = (((*pCurPixAddr)>>1)&0x7fe0)|((*pCurPixAddr++)&0x1f);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
					*pDesPixAddr565++ = *pCurPixAddr++;
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		BYTE  red, green, blue;

		if (imgorg  == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_565_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix = *pCurPixAddr++;
					_cnv_565_to_rgb(pix, &red, &green, &blue);
					*pDesPixAddr24++ = blue;
					*pDesPixAddr24++ = green;
					*pDesPixAddr24++ = red;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		DWORD *pDesPixAddr888;
		DWORD pix32;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// 实现565到888的转换
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<5)&0xfc00)|((pix32<<8)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (WORD*)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = (DWORD)*pCurPixAddr++;	// 实现565到888的转换
					*pDesPixAddr888++ = ((pix32<<3)&0xf8)|((pix32<<5)&0xfc00)|((pix32<<8)&0xf80000);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// 将24位位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_24_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	int heigCount, widthCount, isou;
	PBYTE pCurPixAddr;
	BYTE  red, green, blue;

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++) // 实现从24到555的转换
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++) // 实现从24到555的转换
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		if (imgorg  == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				::CopyMemory((PVOID)pDesPixAddr24, (PVOID)pCurPixAddr, imgwidth*3);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				::CopyMemory((PVOID)pDesPixAddr24, (PVOID)pCurPixAddr, imgwidth*3);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		DWORD *pDesPixAddr888;
		DWORD pix32;

		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = *((DWORD*)pCurPixAddr);
					pix32 &= 0xffffff;	// 888格式，舍弃最高位
					pCurPixAddr += 3;
					*pDesPixAddr888++ = pix32;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (DWORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					pix32 = *((DWORD*)pCurPixAddr);
					pix32 &= 0xffffff;	// 888格式，舍弃最高位
					pCurPixAddr += 3;
					*pDesPixAddr888++ = pix32;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}



// 将32位位图转换为指定的标准格式位图
// pInfo中的目标图像缓冲区必需已经被分配，且其中的源图像信息结构已被正确填写
// imgorg参数是源图像的扫描行次序标志：0-为倒向 1-为正向
int _from_32_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg)
{
	ASSERT(pInfo);
	ASSERT(pInfo->state >= PKST_PASSINFO);
	ASSERT(lpSou);

	int imgheight = pInfo->imginfo.height;	// 图像的高度
	int imgwidth  = pInfo->imginfo.width;	// 图像的宽度
	int scansize  = pInfo->imginfo.linesize;// 每一扫描行的宽度(in byte)
	int heigCount, widthCount, isou;
	PBYTE pCurPixAddr;
	BYTE  red, green, blue;

	switch(_get_desformat(pInfo))	// 标准图像格式（目标格式）
	{
	case	DF_16_555:
		{
		WORD *pDesPixAddr555;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++) // 实现从24到555的转换
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					pCurPixAddr++;	// 舍弃最高位
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr555  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++) // 实现从24到555的转换
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					pCurPixAddr++;	// 舍弃最高位
					*pDesPixAddr555++ = _cnv_rgb_to_555(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_16_565:
		{
		WORD *pDesPixAddr565;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr565  = (WORD*)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					blue = *pCurPixAddr++; green = *pCurPixAddr++; red = *pCurPixAddr++;
					*pCurPixAddr++;
					*pDesPixAddr565++ = _cnv_rgb_to_565(red, green, blue);
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_24:
		{
		PBYTE pDesPixAddr24;
		if (imgorg  == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pCurPixAddr++;
				}
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr24  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				for (widthCount=0;widthCount<imgwidth;widthCount++)
				{
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pDesPixAddr24++ = *pCurPixAddr++;
					*pCurPixAddr++;
				}
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_32:
		{
		PBYTE pDesPixAddr888;
		if (imgorg == 0)	// 源图是倒向位图
		{
			for (heigCount=imgheight-1, isou=0;heigCount>=0;heigCount--, isou++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (PBYTE)pInfo->pLineAddr[isou];// 目标缓冲区当前行第一个像素的地址
				::CopyMemory((PVOID)pDesPixAddr888, (PVOID)pCurPixAddr, imgwidth*4);
				if ((pInfo->fpProgress)&&(!(isou%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, isou))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		else	// 正向位图（top-down）
		{
			for (heigCount=0;heigCount<imgheight;heigCount++)
			{
				pCurPixAddr  = (PBYTE)(lpSou+heigCount*scansize);	// 源图像当前行的第一个像素地址
				pDesPixAddr888  = (PBYTE)pInfo->pLineAddr[heigCount];// 目标缓冲区当前行第一个像素的地址
				::CopyMemory((PVOID)pDesPixAddr888, (PVOID)pCurPixAddr, imgwidth*4);
				if ((pInfo->fpProgress)&&(!(heigCount%RWPROGRESSSTEP)))
				{
					if ((*pInfo->fpProgress)(RWPROGRESSSIZE, heigCount))
					{	
						// 如果进度函数返回1，则说明用户想中断操作，返回。
						pInfo->result = ER_USERBREAK;
						return 1;	// 用户中断操作
					}
				}
			}
		}
		}
		break;
	case	DF_NULL:
	default:
		ASSERT(FALSE);
		pInfo->result = ER_ILLCOMM;
		return -1;
		break;
	}
	return 0;
}




// pInfo所指向的数据包中，annexdata.siAnnData[]变量的含意解释：
//		[0] — 位深度，可以是1、4、8、16(555)、24、32(888)这六个值
//				中的一个，不能是其他值。另：不能存储为RLE格式
//		[1] — 是否使用原始调色板。0 — 表示使用，1 — 表示使用当前
//				计算机缺省调色板
int _Save(CFile& pf, LPINFOSTR pInfo)
{
	PBYTE lpBits;
	HBITMAP hBmp;

	ASSERT(pInfo);
	int dibitcount = pInfo->annexdata.siAnnData[0];	// 目标文件的位深度
	int palnum = _get_palette_size(dibitcount);		// 调色板项数目

	BITMAPINFO *pbmi = (BITMAPINFO*)::GlobalAlloc(GPTR, sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*palnum);
	RGBQUAD *ppal = (RGBQUAD*)((PBYTE)pbmi+sizeof(BITMAPINFOHEADER));

	if (!pbmi)
	{
		pInfo->result = ER_MEMORYERR;
		return 2;	// 内存不足
	}

	HWND hWnd = ::GetDesktopWindow();
	HDC hDC = ::GetDC(hWnd);

	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = dibitcount;
	pbmi->bmiHeader.biCompression = BI_RGB;
	pbmi->bmiHeader.biSizeImage = 0;
	pbmi->bmiHeader.biWidth = pInfo->sDIBInfo.bmi.biWidth;
	pbmi->bmiHeader.biHeight = pInfo->sDIBInfo.bmi.biHeight;
	pbmi->bmiHeader.biClrImportant = 0;
	pbmi->bmiHeader.biClrUsed = 0;
	pbmi->bmiHeader.biXPelsPerMeter = (int)(::GetDeviceCaps(hDC,LOGPIXELSX)*(float)39.373);
	pbmi->bmiHeader.biYPelsPerMeter = (int)(::GetDeviceCaps(hDC,LOGPIXELSY)*(float)39.373);

	if (palnum)
	{
		switch(pInfo->annexdata.siAnnData[1])
		{
		case	0:	// 使用原位图调色板（读取的原始图像文件必需也是BMP位图）
			if (pInfo->imginfo.imgformat == IMF_BMP)
			{
				if (pInfo->imginfo.bitcount == (unsigned int)dibitcount) // 只有原始图像的格式与目标文件的位深度相同时，才有意义
				{
					ASSERT(pInfo->pImgInfo);	// 原始数据必需有效
					ASSERT(((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biSize == sizeof(BITMAPINFOHEADER));

					RGBQUAD *porg = (RGBQUAD*)(((BITMAPINFOHEADER*)(pInfo->pImgInfo))+1);
					int palnum2 = ((BITMAPINFOHEADER*)(pInfo->pImgInfo))->biClrUsed;
					palnum2 = (palnum2) ? palnum2:palnum;
					ASSERT(palnum2 <= palnum);

					for (int i=0;i<palnum2;i++)
					{
						ppal[i].rgbBlue = porg[i].rgbBlue;
						ppal[i].rgbGreen = porg[i].rgbGreen;
						ppal[i].rgbRed = porg[i].rgbRed;
						ppal[i].rgbReserved = 0;
					}
					break;
				}
			}
			else
			{
				;	// 仍使用系统缺省调色板
			}
		case	1:	// 使用系统缺省调色板
			{
			LPPALETTEENTRY pal = (LPPALETTEENTRY)::GlobalAlloc(GPTR, sizeof(PALETTEENTRY)*palnum);
			if (!pal)
			{
				::GlobalFree(pbmi);
				::ReleaseDC(hWnd, hDC);
				pInfo->result = ER_MEMORYERR;
				return 2;	// 内存不足
			}
			::GetSystemPaletteEntries(hDC, 0, palnum, pal);
			for (int i=0;i<palnum;i++)
			{
				ppal[i].rgbBlue = pal[i].peBlue;
				ppal[i].rgbGreen = pal[i].peGreen;
				ppal[i].rgbRed = pal[i].peRed;
				ppal[i].rgbReserved = 0;
			}
			::GlobalFree(pal);
			}
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	hBmp = CreateDIBSection(hDC, pbmi, DIB_RGB_COLORS, (void**)&lpBits, NULL, 0);
	if (hBmp == NULL)
	{
		::GlobalFree(pbmi);
		::ReleaseDC(hWnd, hDC);
		pInfo->result = ER_SYSERR;
		return 5;		// 操作系统不稳定
	}

	if (!SetDIBits(hDC, hBmp, 0, pInfo->imginfo.height, pInfo->_pbdata, (const BITMAPINFO*)&pInfo->sDIBInfo, DIB_RGB_COLORS))
	{
		::GlobalFree(pbmi);
		::ReleaseDC(hWnd, hDC);
		::DeleteObject(hBmp);
		pInfo->result = ER_SYSERR;
		return 5;
	}

	::ReleaseDC(hWnd, hDC);

	BITMAPFILEHEADER	bmf;
				 
	bmf.bfType = DIB_HEADER_MARKER;
	bmf.bfSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+\
		sizeof(RGBQUAD)*palnum+((pInfo->imginfo.width*dibitcount+31)/32)*4*pInfo->imginfo.height;
	bmf.bfReserved1 = 0;
	bmf.bfReserved2 = 0;
	bmf.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+\
		sizeof(RGBQUAD)*palnum;


	TRY
	{
		// writing bitmap file header
		pf.SeekToBegin();
		pf.Write((const void *)&bmf, sizeof(BITMAPFILEHEADER));
		
		// writing bitmap info header
		pf.Write((const void *)pbmi, sizeof(BITMAPINFOHEADER));
	}
	CATCH(CFileException, e)
	{
		::GlobalFree(pbmi);
		::DeleteObject(hBmp);
		pInfo->result = ER_FILERWERR;
		return 1;	// 写文件时出错
	}
	END_CATCH

	::GlobalFree(pbmi);
	RGBQUAD *palette = NULL;

	TRY
	{
		if (palnum)
		{
			palette = (RGBQUAD*)::GlobalAlloc(GPTR, sizeof(RGBQUAD)*palnum);
			if (!palette)
			{
				::DeleteObject(hBmp);
				pInfo->result = ER_MEMORYERR;
				return 2;	// 内存不足
			}

			HDC hCompDC = ::CreateCompatibleDC(NULL);
			HBITMAP hOldBmp = (HBITMAP)::SelectObject(hCompDC, hBmp);

			::GetDIBColorTable(hCompDC, 0, palnum, palette);

			::SelectObject(hCompDC, hOldBmp);
			::DeleteDC(hCompDC);

			// writing color table.
			pf.Write((const void *)palette, sizeof(RGBQUAD)*palnum);

			::GlobalFree(palette);
		}

		pf.WriteHuge((const void *)lpBits, ((pInfo->imginfo.width*dibitcount+31)/32)*4*pInfo->imginfo.height);
	}
	CATCH(CFileException, e)
	{
		if (palette) ::GlobalFree(palette);
		::DeleteObject(hBmp);
		pInfo->result = ER_FILERWERR;
		return 1;	// 写文件时出错
	}
	END_CATCH

	::DeleteObject(hBmp);

	pInfo->result = ER_SUCCESS;

	return 0;	// 写文件成功
}


// 判断标准图像的位格式
DESFORMAT _get_desformat(LPINFOSTR pInfo)
{
	ASSERT(pInfo);
	// ASSERT(pInfo->state >= PKST_PASSVER);
	ASSERT(pInfo->sDIBInfo.bmi.biPlanes == 1);

	DESFORMAT result;

	switch(pInfo->sDIBInfo.bmi.biBitCount)
	{
	case	32:
		/******************************************************
						32位掩码示意图
			    高               ->                 低
		0000 0000   0000 0000   0000 0000   0000 0000 	888格式
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		* Win95 系统只支持这一种格式
		******************************************************/
		if (pInfo->sDIBInfo.bmi.biCompression == BI_RGB)
		{
			result = DF_32;
			break;
		}

		if ((pInfo->sDIBInfo.rmask == 0xff0000)&&
			(pInfo->sDIBInfo.gmask == 0xff00)&&
			(pInfo->sDIBInfo.bmask == 0xff))
			result = DF_32;
		else
		{
			ASSERT(FALSE);	// 只支持888格式
			result = DF_NULL;
		}
		break;
	case	24:
		result = DF_24;
		break;
	case	16:
		/*******************************************
				16位掩码示意图

			  高字节      低字节
			0000 0000   0000 0000 
						   1 1111--B	// 555格式
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	// 565格式
				  111   111 -------G
			1111 1   --------------R

		* Win95 系统只支持以上两种格式
		*******************************************/
		if (pInfo->sDIBInfo.bmi.biCompression == BI_RGB)
		{
			result = DF_16_555;
			break;
		}

		if ((pInfo->sDIBInfo.rmask == 0x7c00)&&
			(pInfo->sDIBInfo.gmask == 0x3e0)&&
			(pInfo->sDIBInfo.bmask == 0x1f))
			result = DF_16_555;
		else if ((pInfo->sDIBInfo.rmask == 0xf800)&&
			(pInfo->sDIBInfo.gmask == 0x7e0)&&
			(pInfo->sDIBInfo.bmask == 0x1f))
			result = DF_16_565;
		else
			result = DF_NULL;
		break;
	default:
		ASSERT(FALSE);		// 不接受其它格式
		result = DF_NULL;
		break;
	}

	return result;
}




// 判断标准图像的位格式
SOUFORMAT _get_souformat(LPBITMAPINFO pInfo)
{
	ASSERT(pInfo->bmiHeader.biSize);

	SOUFORMAT result;

	switch(pInfo->bmiHeader.biBitCount)
	{
	case	1:
		ASSERT(pInfo->bmiHeader.biCompression == BI_RGB);
		result = SF_1;
		break;
	case	4:
		if (pInfo->bmiHeader.biCompression == BI_RGB)
			result = SF_4;
		else if ((pInfo->bmiHeader.biCompression == BI_RLE4)&&\
				 (pInfo->bmiHeader.biHeight > 0))	// 正向的位图不能被压缩
			result = SF_4_RLE;
		else
			result = SF_NULL;
		break;
	case	8:
		if (pInfo->bmiHeader.biCompression == BI_RGB)
			result = SF_8;
		else if ((pInfo->bmiHeader.biCompression == BI_RLE8)&&\
				 (pInfo->bmiHeader.biHeight > 0))
			result = SF_8_RLE;
		else
			result = SF_NULL;
		break;
	case	16:
		{
		/*******************************************
				16位掩码示意图

			  高字节      低字节
			0000 0000   0000 0000 
						   1 1111--B	// 555格式
				   11	111 -------G
			 111 11	 --------------R
			0
						   1 1111--B	// 565格式
				  111   111 -------G
			1111 1   --------------R

		* Win95 系统只支持以上两种格式
		*******************************************/
		if (pInfo->bmiHeader.biCompression == BI_RGB)
		{
			result = SF_16_555;
			break;
		}

		DWORD *pMark = (DWORD*)((LPBITMAPINFOHEADER)pInfo+1);

		if ((pMark[0] == 0x7c00)&&(pMark[1] == 0x3e0)&&(pMark[2] == 0x1f))
			result = SF_16_555;
		else if ((pMark[0] == 0xf800)&&(pMark[1] == 0x7e0)&&(pMark[2] == 0x1f))
			result = SF_16_565;
		else
			result = SF_NULL;
		break;
		}
	case	24:
		ASSERT(pInfo->bmiHeader.biCompression == BI_RGB);
		result = SF_24;
		break;
	case	32:
		{
		/******************************************************
						32位掩码示意图
			    高               ->                 低
		0000 0000   0000 0000   0000 0000   0000 0000 	888格式
		            1111 1111  ------------------------R
		                        1111 1111 -------------G
		                                    1111 1111--B
		* Win95 系统只支持这一种格式
		******************************************************/
		if (pInfo->bmiHeader.biCompression == BI_RGB)
		{
			result = SF_32;
			break;
		}

		DWORD *pMark = (DWORD*)((LPBITMAPINFOHEADER)pInfo+1);

		if ((pMark[0] == 0xff0000)&&(pMark[1] == 0x00ff00)&&(pMark[2] == 0xff))
			result = SF_32;
		else
			result = SF_NULL;	// 只支持888格式
		break;
		}
	default:
		result = SF_NULL;
		break;
	}

	return result;
}

// 将指定的RGB颜色分量转换成555格式（WORD型值返回）
WORD _cnv_rgb_to_555(BYTE red, BYTE green, BYTE blue)
{
	WORD result = 0;

	result = (((WORD)red>>3)<<10)|(((WORD)green>>3)<<5)|((WORD)blue>>3);

	return result;
}


// 将指定的555格式的颜色转换成RGB颜色分量
void _cnv_555_to_rgb(WORD col, PBYTE red, PBYTE green, PBYTE blue)
{
	// 在555转换到RGB时，将像素的亮度调到最大
	*red = (BYTE)((col>>7)&0xf8);
	*green = (BYTE)((col>>2)&0xf8);
	*blue = (BYTE)(col<<3);
}


// 将指定的RGB颜色分量转换成565格式（WORD型值返回）
WORD _cnv_rgb_to_565(BYTE red, BYTE green, BYTE blue)
{
	WORD result = 0;

	result = (((WORD)red>>3)<<11)|(((WORD)green>>2)<<5)|((WORD)blue>>3);

	return result;
}


// 将指定的565格式的颜色转换成RGB颜色分量
void _cnv_565_to_rgb(WORD col, PBYTE red, PBYTE green, PBYTE blue)
{
	// 在565转换到RGB时，将像素的亮度调到最大
	*red = (BYTE)((col>>8)&0xf8);
	*green = (BYTE)((col>>3)&0xfc);
	*blue = (BYTE)(col<<3);
}


// 将指定的RGB颜色分量转换成888格式（DWORD型值返回）
DWORD _cnv_rgb_to_888(BYTE red, BYTE green, BYTE blue)
{
	DWORD result = 0;

	result = ((DWORD)red<<16)|((DWORD)green<<8)|(DWORD)blue;

	return result;
}



// 向4位缓冲区写像素（只供_from_4_RLE_read()函数内部调用）
// 形参mark含意：0-初始化变量  1-写像素  2-移动到指定点  3-结束操作  4-获取当前地址
// 注：当mark等于2时，参数num将作为像素除2余数被代入，来对iCurOff赋值
PBYTE _write2buff4RLE(int mark, int num, BYTE pix, PBYTE lpDesData)
{
	static PBYTE lpDes;		// 缓冲区基地址
	static PBYTE lpCur;		// 缓冲区当前写地址
	static int   iCurOff;	// 在字节中的当前位置，0-高半字节  1-低半字节
	int i;
	BYTE pixa[2];

	switch(mark)
	{
	case	0:		// 初始化变量
		lpDes = lpDesData;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	1:		// 写像素
		pixa[0] = pix>>4;
		pixa[1] = pix&0xf;
		for (i=0;i<num;i++)
		{
			if (iCurOff == 0)	// 左像素
			{
				*lpCur = pixa[i%2]<<4;
				iCurOff = 1;
			}
			else				// 右像素
			{
				ASSERT(iCurOff == 1);
				*lpCur |= pixa[i%2];
				lpCur++;
				iCurOff = 0;
			}
		}
		break;
	case	2:		// 移动到指定点
		lpCur = lpDesData;
		iCurOff = num;
		break;
	case	3:		// 结束操作
		lpDes = NULL;
		lpCur = lpDes;
		iCurOff = 0;
		break;
	case	4:		// 获取当前地址
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	return lpCur;
}



// 向8位缓冲区写像素（只供_from_8_RLE_read()函数内部调用）
// 形参mark含意：0-初始化变量  1-写像素  2-移动到指定点  3-结束操作  4-获取当前地址
PBYTE _write2buff8RLE(int mark, int num, BYTE pix, PBYTE lpDesData)
{
	static PBYTE lpDes;		// 缓冲区基地址
	static PBYTE lpCur;		// 缓冲区当前写地址
	int i;

	switch(mark)
	{
	case	0:		// 初始化变量
		lpDes = lpDesData;
		lpCur = lpDes;
		break;
	case	1:		// 写像素
		for (i=0;i<num;i++)
			*lpCur++ = pix;
		break;
	case	2:		// 移动到指定点
		lpCur = lpDesData;
		break;
	case	3:		// 结束操作
		lpDes = NULL;
		lpCur = lpDes;
		break;
	case	4:		// 获取当前地址
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return lpCur;
}


// 计算调色板的尺寸
int _get_palette_size(int bitcount)
{
	int result;

	switch(bitcount)
	{
	case	1:
		result = 2;
		break;
	case	4:
		result = 16;
		break;
	case	8:
		result = 256;
		break;
	case	16:
	case	24:
	case	32:
		result = 0;
		break;
	default:
		ASSERT(FALSE);	// 非法情况
		result = 0;
		break;
	}
	return result;
}

