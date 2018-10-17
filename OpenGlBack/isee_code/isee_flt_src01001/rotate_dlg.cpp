/********************************************************************

	rotate_dlg.cpp - ISee图像浏览器—图像处理模块图像旋转处理实现代码文件

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

   功能实现：图像旋转设置对话框处理

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "draw.h"
#include "rotate.h"
#include "resource.h"

void DrawButton(HWND hWnd,LPDRAWITEMSTRUCT lpInfo);
BOOL CALLBACK WINAPI RotateProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
void Clear();

HWND hBtWnd[5];				//按钮句柄
HBITMAP hbm;
UINT nInButton=0,nOldButton=0,nPushedButton=0;
BOOL bPushed=FALSE;
RECT rc[5];

//启动图像旋转处理设置对话框
int DoRotate(LPIMAGEPROCSTR lpInfo)
{
   return DialogBox(lpInfo->hInst,MAKEINTRESOURCE(IDD_ROTATE),lpInfo->hParentWnd,(DLGPROC)RotateProc);
}

//对话框处理循环
BOOL CALLBACK WINAPI RotateProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
   	case WM_INITDIALOG:
         hbm=LoadBitmap(lpProcInfo->hInst,MAKEINTRESOURCE(IDB_BKG));

         hBtWnd[0]=GetDlgItem(hWnd,IDC_TURNLEFT);
			hBtWnd[1]=GetDlgItem(hWnd,IDC_TURNRIGHT);
         hBtWnd[2]=GetDlgItem(hWnd,IDC_HFLIP);
         hBtWnd[3]=GetDlgItem(hWnd,IDC_VFLIP);
         hBtWnd[4]=GetDlgItem(hWnd,IDC_PREVIEW);
         SendMessage(hWnd,WM_MOVE,0,0);		//初始化rc

         InitDraw();
         lpProcInfo->result=PR_NULL;			//设置操作初值，处理成功将改写该值
         break;
      case WM_PAINT:
      	PaintDlgBk(hWnd);
         break;
      case WM_MOVE:
      	GetWindowRect(hBtWnd[0],&rc[0]);
         GetWindowRect(hBtWnd[1],&rc[1]);
         GetWindowRect(hBtWnd[2],&rc[2]);
         GetWindowRect(hBtWnd[3],&rc[3]);
         GetWindowRect(hBtWnd[4],&rc[4]);
         break;
      case WM_DRAWITEM:  		//绘制按钮
      	DrawButton(hWnd,(LPDRAWITEMSTRUCT)lParam);
         break;
      case WM_LBUTTONDOWN:    //以下的鼠标处理消息实现按钮的按下及弹起处理
      	bPushed=TRUE;
         nPushedButton=nInButton;
         SetDlgItemText(hWnd,nInButton,"OK");	//该函数目的为发送WM_DRAWITEM消息
         break;
      case WM_LBUTTONUP:
      	bPushed=FALSE;
         if(nInButton==nPushedButton)
         {
         	SetDlgItemText(hWnd,nInButton,"OK");
         	nPushedButton=0;

	         switch(nInButton)
   	      {
      	   	case IDC_TURNLEFT:
         	   	TurnLeft();
                  SetDlgItemText(hWnd,IDC_PREVIEW,"OK");
            	   break;
	            case IDC_TURNRIGHT:
   	         	TurnRight();
                  SetDlgItemText(hWnd,IDC_PREVIEW,"OK");
      	         break;
         	   case IDC_HFLIP:
            		HFlip();
                  SetDlgItemText(hWnd,IDC_PREVIEW,"OK");
               	break;
	            case IDC_VFLIP:
   	         	VFlip();
                  SetDlgItemText(hWnd,IDC_PREVIEW,"OK");
      	         break;
         	}
         }
         nPushedButton=0;
         break;
      case WM_MOUSEMOVE:
      {
      	POINT pt;

         GetCursorPos(&pt);
         if(pt.x>rc[0].left && pt.x<rc[0].right && pt.y>rc[0].top && pt.y<rc[0].bottom)
         {
         	nInButton=IDC_TURNLEFT;
            if(nInButton!=nOldButton)
            {
            	SetDlgItemText(hWnd,nOldButton,"OK");
            	if(nPushedButton==nInButton || !bPushed)
               	SetDlgItemText(hWnd,nInButton,"OK");
            }
         }
         else if(pt.x>rc[1].left && pt.x<rc[1].right && pt.y>rc[1].top && pt.y<rc[1].bottom)
         {
         	nInButton=IDC_TURNRIGHT;
            if(nInButton!=nOldButton)
            {
            	SetDlgItemText(hWnd,nOldButton,"OK");
            	if(nPushedButton==nInButton || !bPushed)
               	SetDlgItemText(hWnd,nInButton,"OK");
            }
         }
         else if(pt.x>rc[2].left && pt.x<rc[2].right && pt.y>rc[2].top && pt.y<rc[2].bottom)
         {
         	nInButton=IDC_HFLIP;
            if(nInButton!=nOldButton)
            {
               SetDlgItemText(hWnd,nOldButton,"OK");
            	if(nPushedButton==nInButton || !bPushed)
               	SetDlgItemText(hWnd,nInButton,"OK");
            }
         }
         else if(pt.x>rc[3].left && pt.x<rc[3].right && pt.y>rc[3].top && pt.y<rc[3].bottom)
         {
         	nInButton=IDC_VFLIP;
            if(nInButton!=nOldButton)
            {
            	SetDlgItemText(hWnd,nOldButton,"OK");
            	if(nPushedButton==nInButton || !bPushed)
               	SetDlgItemText(hWnd,nInButton,"OK");
            }
         }
         else
         {
         	nInButton=0;
            if(nInButton!=nOldButton)
            {
            	SetDlgItemText(hWnd,nOldButton,"OK");
               nOldButton=0;
            }
         }
         break;
      }
      case WM_COMMAND:
      	switch(LOWORD(wParam))
         {
         	case IDOK:
            	Clear();
               EndDialog(hWnd,Output_Rotate());
               return TRUE;
            case IDCANCEL:
            	Clear();
               EndDialog(hWnd,PROCERR_FALSE);
               return FALSE;
            case IDHELP:
            	ShowCopyright();
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

//绘制按钮及预览图像
void DrawButton(HWND hWnd,LPDRAWITEMSTRUCT lpInfo)
{
	if(lpInfo->CtlType==ODT_STATIC)
   {
   	HDC hmemdc;
      hmemdc=CreateCompatibleDC(lpInfo->hDC);
      SelectObject(hmemdc,hbm);
      BitBlt(lpInfo->hDC,0,0,48,48,hmemdc,0,0,SRCCOPY);
      DeleteDC(hmemdc);

      HICON hicon;

      switch(lpInfo->CtlID)
      {
      	case IDC_TURNLEFT:
      		hicon=LoadIcon(lpProcInfo->hInst,MAKEINTRESOURCE(IDI_TURNLEFT));
            break;
         case IDC_TURNRIGHT:
      		hicon=LoadIcon(lpProcInfo->hInst,MAKEINTRESOURCE(IDI_TURNRIGHT));
            break;
         case IDC_HFLIP:
      		hicon=LoadIcon(lpProcInfo->hInst,MAKEINTRESOURCE(IDI_HFLIP));
            break;
         case IDC_VFLIP:
      		hicon=LoadIcon(lpProcInfo->hInst,MAKEINTRESOURCE(IDI_VFLIP));
            break;
         case IDC_PREVIEW:
         	DrawPreView(hWnd,lpInfo);
            return;
      }
      DrawIconEx(lpInfo->hDC,0,0,hicon,48,48,0,NULL,DI_NORMAL);
      DestroyIcon(hicon);
      if(bPushed && nInButton==lpInfo->CtlID && nInButton==nPushedButton)
      {
      	DrawEdge(lpInfo->hDC,&lpInfo->rcItem,BDR_SUNKENINNER,BF_RECT);
         nOldButton=nInButton;
      }
      else if(nInButton==lpInfo->CtlID)
      {
      	DrawEdge(lpInfo->hDC,&lpInfo->rcItem,BDR_RAISEDINNER,BF_RECT);
         nOldButton=nInButton;
      }
   }
}

//清理内存，准备退出
void Clear()
{
   EndDraw();
	DeleteObject(hbm);
   Del(lpBakData);
   Del(lpPreViewData);
}
