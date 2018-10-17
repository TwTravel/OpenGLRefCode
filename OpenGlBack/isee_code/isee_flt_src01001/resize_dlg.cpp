/********************************************************************

	resize_dlg.cpp - ISeeͼ���������ͼ����ģ��ͼ���ض���ߴ紦��ʵ�ִ����ļ�

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

   ����ʵ�֣��ض���ͼ��ߴ�

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "commctrl.h"
#include "math.h"
#include "gol_proc.h"
#include "filter.h"
#include "resize.h"
#include "resource.h"

BOOL CALLBACK WINAPI ResizeProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam);
void AutoSize_size(HWND,DWORD,DWORD,DWORD);	//����������ʱ����趨����
void AutoSize_per(HWND,DWORD);               //����������ʱѡ�ٷֱȿ���趨����
void SetMaxSize(HWND hWnd);
void EnableBkg_Sel(HWND hWnd,BOOL flag);     //��ֹ/���ñ���ɫ�趨
void SelBkColor(HWND hWnd);
void ColToStr(LPSTR str);
void format_colstr(HWND hWnd);
void Enable_SelPos(HWND hWnd,BOOL flag);

float max_size,max_per;
BOOL bAuto=TRUE;						//ͼ�񰴱�������
BOOL bEditCanUpdate=TRUE;			//������ӦEN_UPDATE��Ϣ�����øñ�־��ֹ��ѭ��
int rs_flag=RS_STRETCH;    		//�ض���ߴ�ͼ����������չ��ƽ�̣����еȣ�
int rs_pos=RS_UL;						//����ͼ���־
COLORREF rs_bgcolor=0x00ffffff;	//����ɫ
RECT rt;                         //����ɫԤ���ؼ���RECT
BOOL bEnable_SelPos=FALSE;			//����ѡ�񱣳�ͼ��λ�ÿ��أ�Ĭ��Ϊ��
HICON hPosIcon;

//�ض���ߴ�
int DoResize(LPIMAGEPROCSTR lpInfo)
{
   return DialogBox(lpInfo->hInst,MAKEINTRESOURCE(IDD_RESIZE),lpInfo->hParentWnd,(DLGPROC)ResizeProc);
}

//�Ի�����ѭ��
#pragma argsused
BOOL CALLBACK WINAPI ResizeProc(HWND hWnd,UINT uMsg,UINT wParam,LONG lParam)
{
	switch(uMsg)
   {
   	case WM_INITDIALOG:
      	//��ʼ��
         hbm=LoadBitmap(lpProcInfo->hInst,MAKEINTRESOURCE(IDB_BKG));
         hPosIcon=LoadIcon(lpProcInfo->hInst,MAKEINTRESOURCE(IDI_POS));
         lpProcInfo->result=PR_NULL;

         char s[80];
         wsprintf(s,"ԭ ͼ �� �� �磺%d*%d",lpProcInfo->sImageInfo.width,lpProcInfo->sImageInfo.height);
         SetDlgItemText(hWnd,IDC_RS_ORGSIZE,s);
         SetMaxSize(hWnd);					//���ݿ����ڴ������ͼ�����ߴ磨�ڴ��һ�룩
         CheckDlgButton(hWnd,IDC_RS_AUTO,BST_CHECKED);	//�趨Ϊ����������
         CheckDlgButton(hWnd,IDC_RS_STRETCH,BST_CHECKED);//�趨Ϊͼ������
         //��ֹ�趨����ɫ�ؼ�
         EnableBkg_Sel(hWnd,FALSE);

         SetDlgItemInt(hWnd,IDC_RS_WIDTH,lpProcInfo->sImageInfo.width,FALSE);
         SetDlgItemInt(hWnd,IDC_RS_HEIGHT,lpProcInfo->sImageInfo.height,FALSE);
         SetDlgItemInt(hWnd,IDC_RS_PER,100,FALSE);
         //�趨�ٷֱ�������
         SendMessage(GetDlgItem(hWnd,IDC_RS_PERUD),UDM_SETRANGE,0,MAKELONG(max_per*100,1));

	   	GetWindowRect(GetDlgItem(hWnd,IDC_RS_PREVBKGCOLOR),&rt);
			rt.right=rt.right-rt.left-2;
   		rt.left=0;
		   rt.bottom=rt.bottom-rt.top-2;
   		rt.top=0;

         SendMessage(GetDlgItem(hWnd,IDC_RS_BKGCOLOR),EM_LIMITTEXT,7,0);
         break;
      case WM_PAINT:
      {
      	PaintDlgBk(hWnd);
         PAINTSTRUCT ps;
         BeginPaint(GetDlgItem(hWnd,IDC_RS_PREVBKGCOLOR),&ps);
		   FillRect(ps.hdc,&rt,CreateSolidBrush(rs_bgcolor));
   		EndPaint(hWnd,&ps);
      }
         break;
      case WM_DRAWITEM:
      	if((UINT)wParam==IDC_RS_PREVBKGCOLOR)
         	break;
      	if(bEnable_SelPos)
         {
            LPDRAWITEMSTRUCT lpds=(LPDRAWITEMSTRUCT)lParam;

            if((int)wParam==rs_pos)
            {
            	DrawEdge(lpds->hDC,&(lpds->rcItem),BDR_SUNKENINNER,BF_RECT);
               DrawIconEx(lpds->hDC,4,4,hPosIcon,16,16,0,NULL,DI_NORMAL);
            }
            else
         		DrawEdge(lpds->hDC,&(lpds->rcItem),BDR_RAISEDINNER,BF_RECT);
         }
      	break;
      case WM_CTLCOLORSTATIC:
         SetBkColor((HDC)wParam,0x00ffbf80);
      	return (BOOL)GetStockObject(NULL_BRUSH);
      case WM_LBUTTONUP:
         break;
      case WM_MOUSEMOVE:
			break;
      case WM_COMMAND:
      {
         int id=LOWORD(wParam);
      	switch(id)
         {
         	case IDOK:
            {
            	int w,h;
               w=GetDlgItemInt(hWnd,IDC_RS_WIDTH,NULL,FALSE);
               h=GetDlgItemInt(hWnd,IDC_RS_HEIGHT,NULL,FALSE);
               if((w*h)>max_size)
               {
               	Errmsg("ͼ��ߴ��Ѵ�����������ߴ磡");
                  return FALSE;
               }
               EndDialog(hWnd,Resize(w,h,rs_flag,rs_pos,rs_bgcolor));
               return TRUE;
            }
            case IDCANCEL:
            	EndDialog(hWnd,PROCERR_FALSE);
               return FALSE;
            case IDHELP:
            	ShowCopyright();
               return FALSE;
            case IDC_RS_AUTO:
            	if(IsDlgButtonChecked(hWnd,IDC_RS_AUTO)==BST_CHECKED)
               	bAuto=TRUE;
               else
               	bAuto=FALSE;
               break;
            case IDC_RS_STRETCH:
            	rs_flag=RS_STRETCH;
               EnableBkg_Sel(hWnd,FALSE);
               Enable_SelPos(hWnd,FALSE);
               break;
            case IDC_RS_CENTER:
            	rs_flag=RS_CENTER;
               EnableBkg_Sel(hWnd,TRUE);
               Enable_SelPos(hWnd,FALSE);
               break;
            case IDC_RS_TILED:
            	rs_flag=RS_TILE;
               EnableBkg_Sel(hWnd,FALSE);
               Enable_SelPos(hWnd,FALSE);
               break;
            case IDC_RS_KEEPED:
            	rs_flag=RS_KEEP;
               EnableBkg_Sel(hWnd,TRUE);
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_SELBKGCOLOR:
            	SelBkColor(hWnd);
               break;
            case IDC_RS_KEEP_UL:
            	rs_pos=RS_UL;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_UM:
            	rs_pos=RS_UM;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_UR:
            	rs_pos=RS_UR;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_ML:
            	rs_pos=RS_ML;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_MM:
            	rs_pos=RS_MM;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_MR:
            	rs_pos=RS_MR;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_DL:
            	rs_pos=RS_DL;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_DM:
            	rs_pos=RS_DM;
               Enable_SelPos(hWnd,TRUE);
               break;
            case IDC_RS_KEEP_DR:
            	rs_pos=RS_DR;
               Enable_SelPos(hWnd,TRUE);
               break;
         }
         switch(HIWORD(wParam))
         {
         	case EN_UPDATE:
            	switch(id)
               {
               	case IDC_RS_WIDTH:
                     if(bEditCanUpdate)
	                     AutoSize_size(hWnd,GetDlgItemInt(hWnd,id,NULL,FALSE),IDC_RS_WIDTH,IDC_RS_HEIGHT);
                     break;
                  case IDC_RS_HEIGHT:
                  	if(bEditCanUpdate)
                  		AutoSize_size(hWnd,GetDlgItemInt(hWnd,id,NULL,FALSE),IDC_RS_HEIGHT,IDC_RS_WIDTH);
                     break;
               	case IDC_RS_PER:
                  	if(bEditCanUpdate)
                  		AutoSize_per(hWnd,GetDlgItemInt(hWnd,id,NULL,FALSE));
                     break;
                  case IDC_RS_BKGCOLOR:
                  	if(bEditCanUpdate)
         	            format_colstr(hWnd);
               	   break;
               }
            	break;
         }
         break;
      }
      case WM_CLOSE:
//      	Clear();
			DeleteObject(hbm);
         DestroyIcon(hPosIcon);
         EndDialog(hWnd,PROCERR_FALSE);
         return FALSE;
   }
   return FALSE;
}

void SetMaxSize(HWND hWnd)
{
	MEMORYSTATUS mem;
   mem.dwLength=sizeof(MEMORYSTATUS);

   GlobalMemoryStatus(&mem);

   max_size=(mem.dwAvailPageFile/2)/(lpProcInfo->sImageInfo.bitperpix/8);
   max_per=sqrt((float)max_size/(float)(lpProcInfo->sImageInfo.width*lpProcInfo->sImageInfo.height));

   char s[80];
   wsprintf(s,"���������ߴ磺%d*%d",(DWORD)(lpProcInfo->sImageInfo.width*max_per),(DWORD)(lpProcInfo->sImageInfo.height*max_per));

   SetDlgItemText(hWnd,IDC_RS_MAXSIZE,s);
}

//����������ʱ����趨����
void AutoSize_size(HWND hWnd,DWORD size,DWORD this_id,DWORD other_id)
{
	if(size<8)
   	return;
   if(size>max_size)
   {
      bEditCanUpdate=FALSE;
   	SetDlgItemInt(hWnd,this_id,max_size/8,FALSE);
      SetDlgItemInt(hWnd,other_id,8,FALSE);
      bEditCanUpdate=TRUE;
      return;
   }

   if(bAuto)
   {
   	float n,per;
	   if(this_id == IDC_RS_WIDTH)
   	{
   		n = (float)size*((float)lpProcInfo->sImageInfo.height/(float)lpProcInfo->sImageInfo.width);
	      per=100*(float)size/(float)lpProcInfo->sImageInfo.width;
   	}
	   else
   	{
   		n = (float)size*((float)lpProcInfo->sImageInfo.width/(float)lpProcInfo->sImageInfo.height);
	      per=100*(float)size/(float)lpProcInfo->sImageInfo.height;
   	}
      if(n<8)
      	n=8;

		bEditCanUpdate=FALSE;
      if(per>max_per*100)
      {
			SetDlgItemInt(hWnd,IDC_RS_WIDTH,(DWORD)(lpProcInfo->sImageInfo.width*(float)max_per),FALSE);
         SetDlgItemInt(hWnd,IDC_RS_HEIGHT,(DWORD)(lpProcInfo->sImageInfo.height*(float)max_per),FALSE);
         SetDlgItemInt(hWnd,IDC_RS_PER,(DWORD)(max_per*100),FALSE);
      }
      else
      {
      	SetDlgItemInt(hWnd,IDC_RS_PER,(DWORD)per,FALSE);
	   	SetDlgItemInt(hWnd,other_id,(DWORD)n,FALSE);
      }
   	bEditCanUpdate=TRUE;
   }
}

//����������ʱѡ�ٷֱȿ���趨����
void AutoSize_per(HWND hWnd,DWORD size)
{
	float w,h;

   if(size>max_per*100)
   {
      SetDlgItemInt(hWnd,IDC_RS_PER,max_per*100,FALSE);
      return;
   }

   if(bAuto)
   {
	   w = (float)lpProcInfo->sImageInfo.width*(float)size/(float)100;
   	h = (float)lpProcInfo->sImageInfo.height*(float)size/(float)100;

	   if(w<8)
   		w=8;
	   if(h<8)
   		h=8;

	   bEditCanUpdate=FALSE;
   	SetDlgItemInt(hWnd,IDC_RS_WIDTH,(DWORD)w,FALSE);
	   SetDlgItemInt(hWnd,IDC_RS_HEIGHT,(DWORD)h,FALSE);
   	bEditCanUpdate=TRUE;
   }
}

//��ֹ/���ñ���ɫ�趨
void EnableBkg_Sel(HWND hWnd,BOOL flag)
{
   HWND hwnd=GetDlgItem(hWnd,IDC_RS_PREVBKGCOLOR);
	EnableWindow(hwnd,flag);
   EnableWindow(GetDlgItem(hWnd,IDC_RS_BKGCOLOR),flag);
   EnableWindow(GetDlgItem(hWnd,IDC_RS_SELBKGCOLOR),flag);

   HDC hdc=GetDC(hwnd);
   FillRect(hdc,&rt,CreateSolidBrush(rs_bgcolor));
   ReleaseDC(hwnd,hdc);
   InvalidateRect(hwnd,&rt,TRUE);
}

void Enable_SelPos(HWND hWnd,BOOL flag)
{
   bEnable_SelPos=flag;
   RECT rct,rwt;
   GetWindowRect(GetDlgItem(hWnd,IDC_RS_KEEP_UL),&rct);
   GetWindowRect(hWnd,&rwt);

   int w=(rct.right-rct.left)*3+10;

   rct.left=rct.left-rwt.left-GetSystemMetrics(SM_CXSIZEFRAME)-5;
   rct.top=rct.top-rwt.top-GetSystemMetrics(SM_CYCAPTION)-5;
   rct.right=rct.left+w;
   rct.bottom=rct.top+w;

   InvalidateRect(hWnd,&rct,TRUE);
}

void SelBkColor(HWND hWnd)
{
	CHOOSECOLOR cc;
   COLORREF rf[16];

   cc.lStructSize=sizeof(CHOOSECOLOR);
   cc.hwndOwner=hWnd;
   cc.hInstance=NULL;
   cc.rgbResult=rs_bgcolor;
   cc.lpCustColors=rf;
   cc.Flags=CC_FULLOPEN	| CC_RGBINIT;
   cc.lCustData=NULL;
   cc.lpfnHook=NULL;
   cc.lpTemplateName=NULL;

   if(ChooseColor(&cc)!=0)
   {
      rs_bgcolor=cc.rgbResult;

      char temp[10];
      ColToStr(temp);
      SetDlgItemText(hWnd,IDC_RS_BKGCOLOR,temp);

      HWND hwnd=GetDlgItem(hWnd,IDC_RS_PREVBKGCOLOR);
		HDC hdc=GetDC(hwnd);
	   FillRect(hdc,&rt,CreateSolidBrush(rs_bgcolor));
   	ReleaseDC(hwnd,hdc);
   }
}

void ColToStr(LPSTR str)
{
   char s[3];
   BYTE x;

   wsprintf(str,"#");

   x=(BYTE)((rs_bgcolor<<24)>>24);
   if(x<0x10)
   	wsprintf(s,"0%x",x);
   else
   	wsprintf(s,"%x",x);
   strcat(str,s);

   x=(BYTE)((rs_bgcolor<<16)>>24);
   if(x<0x10)
   	wsprintf(s,"0%x",x);
   else
   	wsprintf(s,"%x",x);
   strcat(str,s);

   x=(BYTE)((rs_bgcolor<<8)>>24);
   if(x<0x10)
   	wsprintf(s,"0%x",x);
   else
   	wsprintf(s,"%x",x);

   strcat(str,s);

   for(int i=1;i<7;i++)
   {
   	if(str[i]>0x60)
   		str[i]-=0x20;
   }
}

void format_colstr(HWND hWnd)
{
	char str[8];
   DWORD cr[6]={0,0,0,0,0,0};

   ZeroMemory(str,8);
  	GetDlgItemText(hWnd,IDC_RS_BKGCOLOR,str,8);

   BOOL bOK=TRUE;

   if(str[0]!='#')
   {
   	str[0]='#';
      bOK=FALSE;
   }

	for(int i=1;i<7;i++)
   {
   	if(str[i]>0x60 && str[i]<'g')
      	str[i]-=0x20;
      else if((str[i]<'0' || (str[i]>'9' && str[i]<'A') || (str[i]>'F' && str[i]<'a') || str[i]>'f') && str[i]!='\0')
      {
      	str[i]='0';
         cr[i-1]=0;
         bOK=FALSE;
      }

      if(str[i]=='\0')
      	cr[i-1]=0;
      else if(str[i]>0x40 && str[i]<'G')
      	cr[i-1]=(DWORD)(str[i]-0x37);
      else
      	cr[i-1]=(DWORD)(str[i]-0x30);
   }
   if(!bOK)
   {
   	bEditCanUpdate=FALSE;
	   SetDlgItemText(hWnd,IDC_RS_BKGCOLOR,str);
   	bEditCanUpdate=TRUE;
   }
   rs_bgcolor=(COLORREF)((cr[4]<<20) | (cr[5]<<16) | (cr[2]<<12) | (cr[3] << 8) | (cr[0]<<4) | cr[1]);

	HWND hwnd=GetDlgItem(hWnd,IDC_RS_PREVBKGCOLOR);
	HDC hdc=GetDC(hwnd);
   FillRect(hdc,&rt,CreateSolidBrush(rs_bgcolor));
  	ReleaseDC(hwnd,hdc);
}

