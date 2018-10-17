/********************************************************************

	BmpModule.h - ISee图像浏览器—BMP图像读写模块定义文件
          
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

#if !defined(AFX_BMPMODULE_H__C03B0E25_460A_11D4_8853_C6A14464AE19__INCLUDED_)
#define AFX_BMPMODULE_H__C03B0E25_460A_11D4_8853_C6A14464AE19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBmpModuleApp
// See BmpModule.cpp for the implementation of this class
//

class CBmpModuleApp : public CWinApp
{
public:
	CBmpModuleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBmpModuleApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBmpModuleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



// 定义模块版本
#define	MODULE_BUILDID					15

#define DIBSCANLINE_WIDTHBYTES(bits)    (((bits)+31)/32*4)
#define DIB_HEADER_MARKER				((WORD)('M'<<8)|'B')


// 目标图位深度格式
enum DESFORMAT
{
	DF_NULL,		// 无效的目标格式
	DF_16_555,		// 16位555格式（也就是15位图像）
	DF_16_565,		// 16位565格式
	DF_24,			// 24位格式
	DF_32,			// 32位格式
	DF_MAX			// 有效值边界
};

// 源图位深度格式
enum SOUFORMAT
{
	SF_NULL,		// 无效的源图
	SF_1,			// 1位BMP
	SF_4,			// 4位BMP
	SF_4_RLE,		// 4位RLE压缩的BMP
	SF_8,			// 8位BMP
	SF_8_RLE,		// 8位RLE压缩的BMP
	SF_16_555,		// 16位555格式的BMP
	SF_16_565,		// 16位565格式的BMP
	SF_24,			// 24位BMP
	SF_32,			// 32位BMP
	SF_MAX			// 边界值
};


// 接口函数声明 — 第一层，唯一与外界联系的接口
int WINAPI AccessBMPModule(INFOSTR *pInfo);


// 命令解释函数 — 第二层解释函数
void _fnCMD_GETPROCTYPE(INFOSTR *pInfo);
void _fnCMD_GETWRITERS(INFOSTR *pInfo);
void _fnCMD_GETWRITERMESS(INFOSTR *pInfo);
void _fnCMD_GETBUILDID(INFOSTR *pInfo);
void _fnCMD_IS_VALID_FILE(INFOSTR *pInfo);
void _fnCMD_GET_FILE_INFO(INFOSTR *pInfo);
void _fnCMD_LOAD_FROM_FILE(INFOSTR *pInfo);
void _fnCMD_SAVE_TO_FILE(INFOSTR *pInfo);
void _fnCMD_IS_SUPPORT(INFOSTR *pInfo);
void _fnCMD_RESIZE(INFOSTR *pInfo);


// 内部执行函数 - 第三层执行函数

int _Read(CFile& file, LPINFOSTR pInfo);

int _from_1_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg, RGBQUAD *lpPalette);
int _from_4_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette);
int _from_4_RLE_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette);
int _from_8_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette);
int _from_8_RLE_read(LPINFOSTR pInfo, PBYTE lpSouData, int imgorg, RGBQUAD *lpPalette);
int _from_16_555_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg);
int _from_16_565_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg);
int _from_24_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg);
int _from_32_read(LPINFOSTR pInfo, PBYTE lpSou, int imgorg);


// 保存图像命令的入口参数annexdata.siAnnData[]变量的含意解释：
//		[0] — 位深度，可以是1、4、8、16(555)、24、32(888)这六个值
//				中的一个，不能是其他值。另：不能存储为RLE格式
//		[1] — 是否使用原始调色板。0 — 表示使用，1 — 表示使用当前
//				计算机缺省调色板。（注：只有当原始图像是BMP，并且位
//				深度与目标位深度相同时，0值才会有意义）
int _Save(CFile& file, LPINFOSTR pInfo);


BOOL IsFileExist(char *lpFileName);

int			_get_palette_size(int bitcount);
SOUFORMAT	_get_souformat(LPBITMAPINFO pInfo);
DESFORMAT	_get_desformat(LPINFOSTR pInfo);

WORD	_cnv_rgb_to_555(BYTE red, BYTE green, BYTE blue);
void	_cnv_555_to_rgb(WORD col, PBYTE red, PBYTE green, PBYTE blue);
WORD	_cnv_rgb_to_565(BYTE red, BYTE green, BYTE blue);
void	_cnv_565_to_rgb(WORD col, PBYTE red, PBYTE green, PBYTE blue);
DWORD	_cnv_rgb_to_888(BYTE red, BYTE green, BYTE blue);

PBYTE	_write2buff4RLE(int mark, int num, BYTE pix, PBYTE lpDesData);
PBYTE	_write2buff8RLE(int mark, int num, BYTE pix, PBYTE lpDesData);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMPMODULE_H__C03B0E25_460A_11D4_8853_C6A14464AE19__INCLUDED_)
