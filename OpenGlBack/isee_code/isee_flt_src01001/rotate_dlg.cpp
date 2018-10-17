/********************************************************************

	rotate_dlg.cpp - ISeeͼ���������ͼ����ģ��ͼ����ת����ʵ�ִ����ļ�

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

   ����ʵ�֣�ͼ����ת���öԻ�����

	�ļ��汾��
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

HWND hBtWnd[5];				//��ť���
HBITMAP hbm;
UINT nInButton=0,nOldButton=0,nPushedButton=0;
BOOL bPushed=FALSE;
RECT rc[5];

//����ͼ����ת�������öԻ���
int DoRotate(LPIMAGEPROCSTR lpInfo)
{
   return DialogBox(lpInfo->hInst,MAKEINTRESOURCE(IDD_ROTATE),lpInfo->hParentWnd,(DLGPROC)RotateProc);
}

//�Ի�����ѭ��
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
         SendMessage(hWnd,WM_MOVE,0,0);		//��ʼ��rc

         InitDraw();
         lpProcInfo->result=PR_NULL;			//���ò�����ֵ������ɹ�����д��ֵ
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
      case WM_DRAWITEM:  		//���ư�ť
      	DrawButton(hWnd,(LPDRAWITEMSTRUCT)lParam);
         break;
      case WM_LBUTTONDOWN:    //���µ���괦����Ϣʵ�ְ�ť�İ��¼�������
      	bPushed=TRUE;
         nPushedButton=nInButton;
         SetDlgItemText(hWnd,nInButton,"OK");	//�ú���Ŀ��Ϊ����WM_DRAWITEM��Ϣ
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

//���ư�ť��Ԥ��ͼ��
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

//�����ڴ棬׼���˳�
void Clear()
{
   EndDraw();
	DeleteObject(hbm);
   Del(lpBakData);
   Del(lpPreViewData);
}
