/********************************************************************

	draw.cpp - ISeeͼ���������ͼ����ģ��ͼ��ʵ�ִ����ļ�

    ��Ȩ����(C) VCHelp-coPathway-ISee workgroup 2000 all member's

    ��һ����������������������������������������GNU ͨ�����֤
	�������޸ĺ����·�����һ���򡣻��������֤�ĵڶ��棬���ߣ�������
	��ѡ�����κθ��µİ汾��

    ������һ�����Ŀ����ϣ�������ã���û���κε���������û���ʺ��ض�
	Ŀ�ص������ĵ���������ϸ����������GNUͨ�����֤��

    ��Ӧ���Ѿ��ͳ���һ���յ�һ��GNUͨ�����֤(GPL)�ĸ����������û�У�
	д�Ÿ���
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA

	�������ʹ�ñ����ʱ��ʲô������飬�����µ�ַ����������ȡ����
	ϵ��
		http://isee.126.com
		http://www.vchelp.net
	��
		iseesoft@china.com

	���ߣ��ٷ�
   e-mail:ringphone@sina.com

   ����ʵ�֣�ͼ����ʾ��DrawDIBʵ�֣�

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/
#include "stdafx.h"
//#include "System.h"
//#include "DrawPreView.h"
#include "draw.h"

static HDRAWDIB m_hDrawDib;
static BITMAPINFO m_bmif;

//��ʼ������ʾͼ��ǰ������øú���
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

//������ʾ
void EndDraw()
{
	DrawDibClose( m_hDrawDib );
}

//��ʾͼ��
BOOL DoDlgDraw(HDC hDC,RECT *rc_dest,int src_width,int src_height,LPVOID lpData)
{
	if (lpData==NULL)
		return FALSE;

   m_bmif.bmiHeader.biWidth=src_width;
   m_bmif.bmiHeader.biHeight=src_height;

   return DrawDibDraw ( m_hDrawDib, hDC,rc_dest->left,rc_dest->top,rc_dest->right-rc_dest->left,rc_dest->bottom-rc_dest->top,
		&(m_bmif.bmiHeader), lpData,0,0,src_width,src_height,DDF_HALFTONE);
}


