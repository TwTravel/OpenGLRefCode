/********************************************************************

	dllshell.cpp - ISee图像浏览器—图像处理模块代码文件

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

   功能实现：DLL入口函数及主函数实现

	文件版本：
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
   ShowHelpInfo(lpProcInfo->hInst,"ISee图像属性处理模块\n(C)Copyright 8/13,2000 [my name]\n版本：1.0.0.0813\n吓着你了吗？请你吃复活节彩蛋","ISee图像处理",IDI_ISEE);
   lpProcInfo->result=PR_NOTEXE;
   return PROCERR_SUCCESS;
}

void ShowCopyright()
{
	ShowHelpInfo(lpProcInfo->hInst,"ISee图像属性处理模块\n(C)Copyright 8/13,2000 [my name]\n版本：1.0.0.0813","ISee图像处理",IDI_ISEE);
}

