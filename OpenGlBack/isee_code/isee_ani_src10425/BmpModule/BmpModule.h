/********************************************************************

	BmpModule.h - ISeeͼ���������BMPͼ���дģ�鶨���ļ�
          
    ��Ȩ����(C) 2000 VCHelp-coPathway-ISee workgroup member

    ��һ����������������������������������������GNU ͨ�ù�����
	��֤�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ���
	�����ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�ù������֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�ù������֤�ĸ����������û�У�
	д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	��д�ˣ�YZ
	E-Mail��yzfree@sina.com

	�ļ��汾��
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



// ����ģ��汾
#define	MODULE_BUILDID					15

#define DIBSCANLINE_WIDTHBYTES(bits)    (((bits)+31)/32*4)
#define DIB_HEADER_MARKER				((WORD)('M'<<8)|'B')


// Ŀ��ͼλ��ȸ�ʽ
enum DESFORMAT
{
	DF_NULL,		// ��Ч��Ŀ���ʽ
	DF_16_555,		// 16λ555��ʽ��Ҳ����15λͼ��
	DF_16_565,		// 16λ565��ʽ
	DF_24,			// 24λ��ʽ
	DF_32,			// 32λ��ʽ
	DF_MAX			// ��Чֵ�߽�
};

// Դͼλ��ȸ�ʽ
enum SOUFORMAT
{
	SF_NULL,		// ��Ч��Դͼ
	SF_1,			// 1λBMP
	SF_4,			// 4λBMP
	SF_4_RLE,		// 4λRLEѹ����BMP
	SF_8,			// 8λBMP
	SF_8_RLE,		// 8λRLEѹ����BMP
	SF_16_555,		// 16λ555��ʽ��BMP
	SF_16_565,		// 16λ565��ʽ��BMP
	SF_24,			// 24λBMP
	SF_32,			// 32λBMP
	SF_MAX			// �߽�ֵ
};


// �ӿں������� �� ��һ�㣬Ψһ�������ϵ�Ľӿ�
int WINAPI AccessBMPModule(INFOSTR *pInfo);


// ������ͺ��� �� �ڶ�����ͺ���
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


// �ڲ�ִ�к��� - ������ִ�к���

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


// ����ͼ���������ڲ���annexdata.siAnnData[]�����ĺ�����ͣ�
//		[0] �� λ��ȣ�������1��4��8��16(555)��24��32(888)������ֵ
//				�е�һ��������������ֵ�������ܴ洢ΪRLE��ʽ
//		[1] �� �Ƿ�ʹ��ԭʼ��ɫ�塣0 �� ��ʾʹ�ã�1 �� ��ʾʹ�õ�ǰ
//				�����ȱʡ��ɫ�塣��ע��ֻ�е�ԭʼͼ����BMP������λ
//				�����Ŀ��λ�����ͬʱ��0ֵ�Ż������壩
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
