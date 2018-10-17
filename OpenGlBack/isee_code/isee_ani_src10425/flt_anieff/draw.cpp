/********************************************************************

	draw.cpp - ISee图像浏览器—图像处理模块图像实现代码文件

    版权所有(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    这一程序是自由软件，你可以遵照自由软件基金会出版的GNU 通用许可证
	条款来修改和重新发布这一程序。或者用许可证的第二版，或者（根据你
	的选择）用任何更新的版本。

    发布这一程序的目的是希望它有用，但没有任何担保。甚至没有适合特定
	目地的隐含的担保。更详细的情况请参阅GNU通用许可证。

    你应该已经和程序一起收到一份GNU通用许可证(GPL)的副本。如果还没有，
	写信给：
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	如果你在使用本软件时有什么问题或建议，用以下地址可以与我们取得联
	系：
		http://isee.126.com
		http://www.vchelp.net
	或：
		iseesoft@china.com

	作者：临风
   e-mail:ringphone@sina.com

   功能实现：图像显示（DrawDIB实现）

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/
#include "stdafx.h"
//#include "System.h"
//#include "DrawPreView.h"
#include "draw.h"

static HDRAWDIB m_hDrawDib;
static BITMAPINFO m_bmif;

//初始化，显示图像前必须调用该函数
void InitDraw()
{
	m_hDrawDib=DrawDibOpen();

   m_bmif.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
   m_bmif.bmiHeader.biPlanes=1;
   m_bmif.bmiHeader.biBitCount=(WORD)lpProcInfo->sImageInfo.bitperpix;
   m_bmif.bmiHeader.biCompression=BI_RGB;
   m_bmif.bmiHeader.biSizeImage=0;
   m_bmif.bmiHeader.biXPelsPerMeter=0;
   m_bmif.bmiHeader.biYPelsPerMeter=0;
   m_bmif.bmiHeader.biClrUsed=0;
   m_bmif.bmiHeader.biClrImportant=0;
}

//结束显示
void EndDraw()
{
	DrawDibClose( m_hDrawDib );
}

//显示图像
BOOL DoDlgDraw(HDC hDC,RECT *rc_dest,int src_width,int src_height,LPVOID lpData)
{
	if (lpData==NULL)
		return FALSE;

   m_bmif.bmiHeader.biWidth=src_width;
   m_bmif.bmiHeader.biHeight=src_height;

   return DrawDibDraw ( m_hDrawDib, hDC,rc_dest->left,rc_dest->top,rc_dest->right-rc_dest->left,rc_dest->bottom-rc_dest->top,
		&(m_bmif.bmiHeader), lpData,0,0,src_width,src_height,DDF_HALFTONE);
}


