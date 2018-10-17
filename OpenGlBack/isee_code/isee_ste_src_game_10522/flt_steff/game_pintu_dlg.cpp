/********************************************************************

	game_pintu_dlg.cpp - ISee图像浏览器—图像处理模块拼图游戏实现代码文件

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

   功能实现：拼图游戏

	文件版本：
		Build 01520
		Date  2001-5-20

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "draw.h"
#include "resource.h"

BOOL CALLBACK WINAPI GameProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
BOOL InitGame(HWND hWnd);
void DrawGamePic(HWND hWnd,LPDRAWITEMSTRUCT lpInfo);
void SplitGamePic(LPRECT rt);
void DoSplitGamePic(LPRECT rt);

RECT rtBlock[36];
POINT ptBlock[36],ptOld;
BOOL bGameOver = FALSE;

int nBlockCnt = 0;
int nLevel = 0;
HBITMAP hbmGame,hbmGameBak;
BOOL bSPFlip = FALSE;

//启动拼图游戏对话框
int DoGame_Pintu(LPIMAGEPROCSTR lpInfo)
{
   return DialogBox(lpInfo->hInst,MAKEINTRESOURCE(IDD_GAME),lpInfo->hParentWnd,(DLGPROC)GameProc);
}

//对话框处理循环
BOOL CALLBACK WINAPI GameProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
   	case WM_INITDIALOG:
      	InitDraw();
         ptOld.x = -1;
         ptOld.y = -1;
         if(!InitGame(hWnd))
         {
         	Errmsg("初始化游戏失败：图象不能小于300*300，宽高比例不能超过1:2");
            EndDraw();
            EndDialog(hWnd,FALSE);
            return FALSE;
         }
         break;
      case WM_LBUTTONDOWN:
         if(!bGameOver)
      	{
         	POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            for(int i=0;i<nBlockCnt;i++)
            {
            	if(PtInRect(&rtBlock[i],pt))
               {
               	if(ptOld.x == -1)
                  	ptOld.x = i;
                  else
                  	ptOld.y = i;
               }
            }
            //发送重绘游戏图象消息
            SetDlgItemText(hWnd,IDC_GAMEBOARD,"draw");
			}
         break;
      case WM_DRAWITEM:
      {
      	LPDRAWITEMSTRUCT lpInfo = (LPDRAWITEMSTRUCT)lParam;
         if(lpInfo->CtlType==ODT_STATIC && lpInfo->CtlID == IDC_GAMEBOARD)
            DrawGamePic(hWnd,lpInfo);
         break;
      }
      case WM_CLOSE:
      	DeleteObject(hbmGame);
         DeleteObject(hbmGameBak);
         EndDraw();
      	EndDialog(hWnd,TRUE);
         return TRUE;
      default:
      	return FALSE;
   }
   return FALSE;
}

//初始化游戏
BOOL InitGame(HWND hWnd)
{
   RECT rtWin,rtClient;

   GetWindowRect(hWnd,&rtWin);
   GetClientRect(hWnd,&rtClient);

   int titleH = rtWin.bottom - rtWin.top - rtClient.bottom;
   int yMax = MAX_Y - titleH;
   int xMax = MAX_X;
   int x = lpProcInfo->sImageInfo.width;
   int y = lpProcInfo->sImageInfo.height;
   float per;

   if(x < 300 || y < 300)
   	return FALSE;
   else
   {
      per = (float)y/(float)x;
   	if(per < 0.5 || per > 2)
      	return FALSE;
   }

   if(x > xMax || y > yMax)
   {
     	if(x >= y)
		{
      	rtWin.left = 0;
         rtWin.right = ClientX(xMax);

   	   per = (float)y / (float)x;
         per *= (float)xMax;
         rtClient.bottom = (int)per;
         rtClient.right = xMax;

   	   rtWin.top = (MAX_Y - rtClient.bottom - titleH)/2;
         rtWin.bottom = rtWin.top + ClientY(rtClient.bottom);
  	   }
      else
	   {
         rtWin.top = 0;
         rtWin.bottom = MAX_Y;
         rtClient.bottom = yMax;
      	rtWin.left = 0;

   	   per = (float)x / (float)y;
         per *= (float)yMax;
         rtClient.right = (int)per;
         rtClient.bottom = ClientY(yMax);

   	   rtWin.left = (MAX_X - rtClient.right)/2;
         rtWin.right = rtWin.left + ClientX(rtClient.right);
  	   }
   }
   else
   {
      rtClient.right = x;
      rtClient.bottom = y;
   	rtWin.left = (MAX_X - rtClient.right)/2;
      rtWin.top = (MAX_Y - rtClient.bottom - titleH)/2;
      rtWin.right = rtWin.left + ClientX(rtClient.right);
      rtWin.bottom = rtWin.top + ClientY(rtClient.bottom);
   }

   SetWindowPos(hWnd,NULL,rtWin.left,rtWin.top,rtWin.right - rtWin.left,rtWin.bottom - rtWin.top,SWP_NOZORDER);
   SetWindowPos(GetDlgItem(hWnd,IDC_GAMEBOARD),NULL,0,0,rtClient.right,rtClient.bottom,SWP_NOZORDER);

   SplitGamePic(&rtClient);

   HDC hDC = GetDC(hWnd);
   hbmGame = CreateCompatibleBitmap(hDC,rtClient.right,rtClient.bottom);
   hbmGameBak = CreateCompatibleBitmap(hDC,rtClient.right,rtClient.bottom);
   HDC hMemDC = CreateCompatibleDC(hDC);
   HDC hBakDC = CreateCompatibleDC(hDC);
   SelectObject(hMemDC,hbmGame);
   SelectObject(hBakDC,hbmGameBak);
   SelectObject(hMemDC,GetStockObject(NULL_BRUSH));
   HPEN hpen = CreatePen(PS_SOLID,1,0x00ff0000);
   SelectObject(hMemDC,hpen);

   rtWin.left = rtWin.top = 0;
   rtWin.right = lpProcInfo->sImageInfo.width;
   rtWin.bottom = lpProcInfo->sImageInfo.height;
   DrawTo(hBakDC,&rtClient,&rtWin,lpProcInfo->_psbdata);

   srand( (unsigned int)time( NULL)) ;
   int rnd;
   BOOL bUsed[36];

   for(int i=0;i<nBlockCnt;i++)
		bUsed[i] = FALSE;

   for(int i=0;i<nBlockCnt;i++)
   {
      rnd = rand() % nBlockCnt;
		while(bUsed[rnd])
			rnd = rand() % nBlockCnt;

      bUsed[rnd] = TRUE;
		StretchBlt(hMemDC,rtBlock[i].left,rtBlock[i].top,rtBlock[i].right - rtBlock[i].left,rtBlock[i].bottom - rtBlock[i].top,hBakDC,rtBlock[rnd].left,rtBlock[rnd].top,rtBlock[rnd].right - rtBlock[rnd].left,rtBlock[rnd].bottom - rtBlock[rnd].top,SRCCOPY);
      Rectangle(hMemDC,rtBlock[i].left,rtBlock[i].top,rtBlock[i].right,rtBlock[i].bottom);
      ptBlock[i].x = i;
      ptBlock[i].y = rnd;
   }

   DeleteObject(hpen);
   DeleteDC(hMemDC);
	DeleteDC(hBakDC);
   ReleaseDC(hWnd,hDC);
	return TRUE;
}

void SplitGamePic(LPRECT rt)
{
   int x = rt->right - rt->left;
   int y = rt->bottom - rt->top;
   int step;
   RECT rtTemp[3];

   rtTemp[0].left = rt->left;
   rtTemp[0].top = rt->top;

	if(x > y)
   {
   	step = x >> 3;

      rtTemp[0].right = rt->left + step * 3;
		rtTemp[0].bottom = rt->bottom;
      rtTemp[1].left = rtTemp[0].right;
      rtTemp[1].top = rt->top;
      rtTemp[2].right = rt->right;
      rtTemp[2].bottom = rt->bottom;

      if((rt->right - rtTemp[0].right) > y)
      {
	      rtTemp[1].right = rt->left + step * 6;
      	rtTemp[1].bottom = rt->bottom;
         rtTemp[2].left = rtTemp[1].right;
   	   rtTemp[2].top = rt->top;
      }
      else
      {
	      rtTemp[1].right = rt->right;
      	rtTemp[1].bottom = rtTemp[0].top + (y >> 3) * 3;
         rtTemp[2].left = rtTemp[0].right;;
   	   rtTemp[2].top = rtTemp[1].bottom;
      }
   }
   else
   {
   	step = y >> 3;

      rtTemp[0].right = rt->right;
		rtTemp[0].bottom = rt->top + step * 3;
      rtTemp[1].left = rt->top;
      rtTemp[1].top = rtTemp[0].bottom;
      rtTemp[2].right = rt->right;
      rtTemp[2].bottom = rt->bottom;

      if((rt->bottom - rtTemp[0].bottom) > x)
      {
	      rtTemp[1].right = rt->right;
      	rtTemp[1].bottom = rt->top + step * 6;
         rtTemp[2].left = rt->left;
   	   rtTemp[2].top = rtTemp[1].bottom;
      }
      else
      {
	      rtTemp[1].right = rt->left + (x >> 3) * 3;
      	rtTemp[1].bottom = rt->bottom;
         rtTemp[2].left = rtTemp[1].right;;
   	   rtTemp[2].top = rtTemp[0].bottom;
      }
   }

  	DoSplitGamePic(&rtTemp[0]);
  	nLevel = 0;
   DoSplitGamePic(&rtTemp[1]);
   bSPFlip = !bSPFlip;
   nLevel = 0;
   DoSplitGamePic(&rtTemp[2]);
}

void DoSplitGamePic(LPRECT rt)
{
   int x = rt->right - rt->left;
   int y = rt->bottom - rt->top;
   int step,skip = bSPFlip?3:5;
   RECT rtTemp;

   if(nLevel > 11 || y < 64 || x < 64)
   {
      rtBlock[nBlockCnt].left = rt->left;
      rtBlock[nBlockCnt].right = rt->right;
      rtBlock[nBlockCnt].top = rt->top;
      rtBlock[nBlockCnt].bottom = rt->bottom;
      nBlockCnt ++;
      bSPFlip = !bSPFlip;
   	return;
   }

	if(x > y)
   {
   	step = x >>3;

      rtTemp.left = rt->left;
      rtTemp.right = rt->left + step * skip;
      rtTemp.top = rt->top;
      rtTemp.bottom = rt->bottom;

      if(bSPFlip)
      {
         rtBlock[nBlockCnt] = rtTemp;
	      rtTemp.left = rtTemp.right;
   	   rtTemp.right = rt->right;
      }
      else
      {
	      rtBlock[nBlockCnt].left = rtTemp.right;
   	   rtBlock[nBlockCnt].right = rt->right;
         rtBlock[nBlockCnt].top = rt->top;
	      rtBlock[nBlockCnt].bottom = rt->bottom;
      }
   }
   else
   {
   	step = y >>3;

      rtTemp.left = rt->left;
      rtTemp.right = rt->right;
      rtTemp.top = rt->top;
      rtTemp.bottom = rt->top + step * skip;

      if(bSPFlip)
      {
      	rtBlock[nBlockCnt] = rtTemp;
	      rtTemp.top = rtTemp.bottom;
	      rtTemp.bottom = rt->bottom;
      }
      else
      {
	      rtBlock[nBlockCnt].left = rt->left;
   	   rtBlock[nBlockCnt].right = rt->right;
      	rtBlock[nBlockCnt].top = rtTemp.bottom;
	      rtBlock[nBlockCnt].bottom = rt->bottom;
      }
   }
   nBlockCnt ++;
   nLevel ++;
   bSPFlip = !bSPFlip;

   DoSplitGamePic(&rtTemp);
}

void DrawGamePic(HWND hWnd,LPDRAWITEMSTRUCT lpInfo)
{
   HDC hMemDC = CreateCompatibleDC(lpInfo->hDC);
   SelectObject(hMemDC,hbmGame);
   SelectObject(hMemDC,GetStockObject(NULL_BRUSH));

   HPEN hpen;

   if(ptOld.x != -1)
   {
   	if(ptOld.y == -1)
      {
      	hpen = CreatePen(PS_SOLID,2,0x0000ffff);
		   SelectObject(hMemDC,hpen);
			Rectangle(hMemDC,rtBlock[ptOld.x].left,rtBlock[ptOld.x].top,rtBlock[ptOld.x].right,rtBlock[ptOld.x].bottom);
         DeleteObject(hpen);
      }
      else
      {
      	if(ptOld.x == ptOld.y)
         {
      		hpen = CreatePen(PS_SOLID,2,0x00ff0000);
			   SelectObject(hMemDC,hpen);
				Rectangle(hMemDC,rtBlock[ptOld.x].left,rtBlock[ptOld.x].top,rtBlock[ptOld.x].right,rtBlock[ptOld.x].bottom);
      	   DeleteObject(hpen);
         }
         else
         {
      		hpen = CreatePen(PS_SOLID,2,0x00ff0000);
			   SelectObject(hMemDC,hpen);
            HDC hBakDC = CreateCompatibleDC(lpInfo->hDC);
            SelectObject(hBakDC,hbmGameBak);

            StretchBlt(hMemDC,rtBlock[ptOld.x].left,rtBlock[ptOld.x].top,rtBlock[ptOld.x].right - rtBlock[ptOld.x].left,rtBlock[ptOld.x].bottom - rtBlock[ptOld.x].top,hBakDC,rtBlock[ptBlock[ptOld.y].y].left,rtBlock[ptBlock[ptOld.y].y].top,rtBlock[ptBlock[ptOld.y].y].right - rtBlock[ptBlock[ptOld.y].y].left,rtBlock[ptBlock[ptOld.y].y].bottom - rtBlock[ptBlock[ptOld.y].y].top,SRCCOPY);
            StretchBlt(hMemDC,rtBlock[ptOld.y].left,rtBlock[ptOld.y].top,rtBlock[ptOld.y].right - rtBlock[ptOld.y].left,rtBlock[ptOld.y].bottom - rtBlock[ptOld.y].top,hBakDC,rtBlock[ptBlock[ptOld.x].y].left,rtBlock[ptBlock[ptOld.x].y].top,rtBlock[ptBlock[ptOld.x].y].right - rtBlock[ptBlock[ptOld.x].y].left,rtBlock[ptBlock[ptOld.x].y].bottom - rtBlock[ptBlock[ptOld.x].y].top,SRCCOPY);
				Rectangle(hMemDC,rtBlock[ptOld.x].left,rtBlock[ptOld.x].top,rtBlock[ptOld.x].right,rtBlock[ptOld.x].bottom);
            Rectangle(hMemDC,rtBlock[ptOld.y].left,rtBlock[ptOld.y].top,rtBlock[ptOld.y].right,rtBlock[ptOld.y].bottom);
      	   DeleteObject(hpen);

            int x = ptBlock[ptOld.x].y;
            ptBlock[ptOld.x].y = ptBlock[ptOld.y].y;
            ptBlock[ptOld.y].y = x;

            for(x=0;x<nBlockCnt;x++)
            	if(ptBlock[x].x != ptBlock[x].y)
               	break;
            if(x == nBlockCnt)
            {
               BitBlt(hMemDC,lpInfo->rcItem.left,lpInfo->rcItem.top,lpInfo->rcItem.right - lpInfo->rcItem.left,lpInfo->rcItem.bottom - lpInfo->rcItem.top,hBakDC,0,0,SRCCOPY);
            	MessageBox(hWnd,"恭喜恭喜，你完成了游戏。","游戏结束",MB_OK | MB_ICONINFORMATION);
               bGameOver = TRUE;
            }
            DeleteDC(hBakDC);
         }
         ptOld.x = ptOld.y = -1;
      }
   }
   BitBlt(lpInfo->hDC,lpInfo->rcItem.left,lpInfo->rcItem.top,lpInfo->rcItem.right - lpInfo->rcItem.left,lpInfo->rcItem.bottom - lpInfo->rcItem.top,hMemDC,0,0,SRCCOPY);
   DeleteDC(hMemDC);
}


