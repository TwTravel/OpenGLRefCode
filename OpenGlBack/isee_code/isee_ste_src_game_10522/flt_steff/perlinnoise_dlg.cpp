/********************************************************************

	perlinnoise_dlg.cpp - ISeeͼ���������ͼ����ģ��ͼ��ּ�ϸ����������ʵ�ִ����ļ�

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

   ����ʵ�֣�ͼ��ͼ��ּ�ϸ���������öԻ�����

	�ļ��汾��
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

//�����������öԻ���
int DoPerlinNoise(LPIMAGEPROCSTR lpInfo)
{
   return DialogBox(lpInfo->hInst,MAKEINTRESOURCE(IDD_PERLINNOISE),lpInfo->hParentWnd,(DLGPROC)PerlinNoiseProc);
}

//�Ի�����ѭ��
BOOL CALLBACK WINAPI PerlinNoiseProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
   	case WM_INITDIALOG:
         hbm=LoadBitmap(lpProcInfo->hInst,MAKEINTRESOURCE(IDB_BKG));
         InitDraw();
         lpProcInfo->result=PR_NULL;			//���ò�����ֵ������ɹ�����д��ֵ
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

//�����ڴ棬׼���˳�
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

