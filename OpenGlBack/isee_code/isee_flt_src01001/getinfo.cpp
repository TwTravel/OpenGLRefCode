/********************************************************************

	dllshell.cpp - ISeeͼ���������ͼ����ģ������ļ�

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

   ����ʵ�֣�DLL��ں�����������ʵ��

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "resource.h"

#define MAXEGGS	2000

int ShowWriterMessage()
{
	HDC hdc = GetDC( NULL) ;
   HICON hIcon=LoadIcon(lpProcInfo->hInst,MAKEINTRESOURCE(IDI_EGG));
   int x,y;

   srand( (unsigned int)time( NULL)) ;

   int nCnt=0;

	while( nCnt++ <= MAXEGGS)
	{
		x = rand() % MAX_X;
		y = rand() % MAX_Y;

		DrawIcon(hdc,x,y,hIcon);
	}

	ReleaseDC( NULL, hdc) ;
   DestroyIcon(hIcon);
   ShowHelpInfo(lpProcInfo->hInst,"ISeeͼ�����Դ���ģ��\n(C)Copyright 8/13,2000 [my name]\n�汾��1.0.0.0813\n��������������Ը���ڲʵ�","ISeeͼ����",IDI_ISEE);
   lpProcInfo->result=PR_NOTEXE;
   return PROCERR_SUCCESS;
}

void ShowCopyright()
{
	ShowHelpInfo(lpProcInfo->hInst,"ISeeͼ�����Դ���ģ��\n(C)Copyright 8/13,2000 [my name]\n�汾��1.0.0.0813","ISeeͼ����",IDI_ISEE);
}

