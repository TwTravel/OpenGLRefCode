/********************************************************************

	draw.h - ISeeͼ���������ͼ����ģ��ͼ����ʾ����ʵ�ִ���ͷ�ļ�

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

   ����ʵ�֣�ͼ����ʾ��������

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/
#ifndef _inc_draw
#define _inc_draw
#include <mmsystem.h>
#include <vfw.h>

extern HDRAWDIB m_hDrawDib;
extern BITMAPINFO m_bmif;

void InitDraw();
void EndDraw();
BOOL DoDlgDraw(HDC hDC,RECT *rc_dest,int src_width,int src_height,LPVOID lpData);
BOOL DrawTo(HDC hDC,LPRECT rc_dest,LPRECT rc_src,LPVOID lpData);
#endif
