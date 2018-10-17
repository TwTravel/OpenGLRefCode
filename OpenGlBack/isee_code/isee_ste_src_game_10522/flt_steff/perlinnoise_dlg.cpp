/********************************************************************

	perlinnoise_dlg.cpp - ISee图像浏览器—图像处理模块图像分级细化噪声处理实现代码文件

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

   功能实现：图像图像分级细化噪声设置对话框处理

	文件版本：
		Build 01418
		Date  2001-4-18

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "commctrl.h"
#include "filter.h"
#include "draw.h"
#include "PerlinNoise.h"
#include "resource.h"

BOOL CALLBACK WINAPI PerlinNoiseProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
void Clear();
void get_info(HWND hWnd);

HBITMAP hbm;
int perlin_waveX,perlin_waveY,perlin_level;
HWND hProgressWnd;

//启动噪声设置对话框
int DoPerlinNoise(LPIMAGEPROCSTR lpInfo)
{
   return DialogBox(lpInfo->hInst,MAKEINTRESOURCE(IDD_PERLINNOISE),lpInfo->hParentWnd,(DLGPROC)PerlinNoiseProc);
}

//对话框处理循环
BOOL CALLBACK WINAPI PerlinNoiseProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
   	case WM_INITDIALOG:
         hbm=LoadBitmap(lpProcInfo->hInst,MAKEINTRESOURCE(IDB_BKG));
         InitDraw();
         lpProcInfo->result=PR_NULL;			//设置操作初值，处理成功将改写该值
         hProgressWnd = GetDlgItem(hWnd,IDC_PROCESSING);
         ShowWindow(GetDlgItem(hWnd,IDC_PROCESSING),SW_HIDE);
         SendMessage(GetDlgItem(hWnd,IDC_UPDOWNLEVEL),UDM_SETRANGE,0,MAKELONG(32,1));
         SetDlgItemInt(hWnd,IDC_LEVEL,4,FALSE);
         perlin_waveX = 128;
         perlin_waveY = 128;
         perlin_level = 4;
         break;
      case WM_PAINT:
      	PaintDlgBk(hWnd);
         break;
      case WM_CTLCOLORSTATIC:
         SetBkColor((HDC)wParam,0x00ffbf80);
      	return (BOOL)GetStockObject(NULL_BRUSH);
      case WM_DRAWITEM:
      {
      	LPDRAWITEMSTRUCT lpInfo = (LPDRAWITEMSTRUCT)lParam;
         if(lpInfo->CtlType==ODT_STATIC && lpInfo->CtlID == IDC_PREVIEW)
	      {
         	ShowWindow(GetDlgItem(hWnd,IDC_DOPREVIEW),SW_HIDE);
            ShowWindow(hProgressWnd,SW_SHOW);
           	DrawPreView(hWnd,lpInfo);
            ShowWindow(hProgressWnd,SW_HIDE);
            ShowWindow(GetDlgItem(hWnd,IDC_DOPREVIEW),SW_SHOW);
         }
         break;
      }
      case WM_COMMAND:
      	switch(LOWORD(wParam))
         {
         	case IDOK:
               Clear();
               get_info(hWnd);
               ShowWindow(GetDlgItem(hWnd,IDC_DOPREVIEW),SW_HIDE);
	            ShowWindow(hProgressWnd,SW_SHOW);
               EndDialog(hWnd,PerlinNoise(perlin_waveX,perlin_waveY,perlin_level));
               return TRUE;
            case IDCANCEL:
            	Clear();
               EndDialog(hWnd,PROCERR_FALSE);
               return FALSE;
            case IDHELP:
            	ShowCopyright();
               break;
            case IDC_DOPREVIEW:
               RestorePreviewData();
					get_info(hWnd);
               SetDlgItemText(hWnd,IDC_PREVIEW,"OK");
               break;
         }
         break;
      case WM_CLOSE:
      	Clear();
         EndDialog(hWnd,PROCERR_FALSE);
         return FALSE;
   }
   return FALSE;
}

//清理内存，准备退出
void Clear()
{
   EndDraw();
	DeleteObject(hbm);
   Del(lpPreViewData);
   Del(lpBakData);
}

void get_info(HWND hWnd)
{
	int temp;
   BOOL bok;

   temp = GetDlgItemInt(hWnd,IDC_WAVEX,&bok,FALSE);
   if(bok && temp > 0)
		perlin_waveX = temp;
   else
   	SetDlgItemInt(hWnd,IDC_WAVEX,perlin_waveX,FALSE);

	temp = GetDlgItemInt(hWnd,IDC_WAVEY,&bok,FALSE);
   if(bok && temp > 0)
		perlin_waveY = temp;
   else
      SetDlgItemInt(hWnd,IDC_WAVEY,perlin_waveY,FALSE);

   temp = GetDlgItemInt(hWnd,IDC_LEVEL,&bok,FALSE);
   if(bok)
   {
     	if(temp > 32)
        	temp = 32;
      if(temp < 1)
        	temp = 1;
     	perlin_level = temp;
   }
}

