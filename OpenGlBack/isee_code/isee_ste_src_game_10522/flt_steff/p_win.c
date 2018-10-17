/********************************************************************

	p_win.c - ISeeͼ���������ͼ����ģ��ʵ�ִ����ļ�

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

   ����ʵ�֣�һ���Ҫ����ȫ�ֺ����������̡�

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include <stdio.h>

void Errmsg(LPSTR format,...)
{
   char buf[MAX_PATH];

	va_list ap;

   va_start(ap, format);
   vsprintf(buf, format, ap);
   va_end(ap);

	MessageBox(GetFocus(),buf,"����",MB_OK | MB_ICONHAND);
}

void PaintNull(HWND hWnd)
{
	HDC hDC;
   PAINTSTRUCT ps;

   hDC=BeginPaint(hWnd,&ps);
   EndPaint(hWnd,&ps);
   ReleaseDC(hWnd,hDC);
}

int ClientX(int x)
{return (GetSystemMetrics(SM_CXSIZEFRAME)*2+x);}

int ClientY(int y)
{return (GetSystemMetrics(SM_CYSIZEFRAME)*2+GetSystemMetrics(SM_CYMENU)+y);}

int GetFileName(HINSTANCE hInst,HWND hWnd,LPSTR szFileType,LPSTR szFileName,LPSTR szTitle)
{
   OPENFILENAME ofnTemp;
   DWORD Errval; // Error value
  	char buf[5];  // Error buffer
   char szTempName[256]="\0";
  	char Errstr[50]="GetOpenFileName returned Error #";
   char s[MAX_PATH];

   GetCurrentDirectory(MAX_PATH,s);
   SetCurrentDirectory(s);

	ofnTemp.lStructSize = sizeof( OPENFILENAME );
	ofnTemp.hwndOwner = hWnd; // An invalid hWnd causes non-modality
	ofnTemp.hInstance = hInst;
   ofnTemp.lpstrFilter = szFileType;  // See previous note concerning string
	ofnTemp.lpstrCustomFilter = NULL;
	ofnTemp.nMaxCustFilter = 0;
	ofnTemp.nFilterIndex = 1;
	ofnTemp.lpstrFile = (LPSTR)szTempName;  // Stores the result in this variable
	ofnTemp.nMaxFile = sizeof( szTempName );
	ofnTemp.lpstrFileTitle = NULL;//szTitle;
	ofnTemp.nMaxFileTitle = NULL;//sizeof(szTitle);
	ofnTemp.lpstrInitialDir = s;//NULL;	//(LPSTR)SourcePathName;
	ofnTemp.lpstrTitle = szTitle;  // Title for dialog
	ofnTemp.Flags = OFN_EXPLORER;
	ofnTemp.nFileOffset = NULL;
	ofnTemp.nFileExtension = 0;
	ofnTemp.lpstrDefExt = "*";
	ofnTemp.lCustData = 0L;
	ofnTemp.lpfnHook = NULL;
	ofnTemp.lpTemplateName = NULL;

   if(GetOpenFileName( &ofnTemp ) != TRUE)
	{
   	Errval=CommDlgExtendedError();
	   if(Errval!=0) // 0 value means user selected Cancel
   	{
      	wsprintf(buf,"%ld",Errval);
	      strcat(Errstr,buf);
   	   MessageBox(hWnd,Errstr,"WARNING",MB_OK|MB_ICONSTOP);
         return (int)Errval;
	   }
      else
         return FALSE;
	}
   wsprintf(szFileName,szTempName);
   return TRUE;
}

HANDLE Open(LPSTR szFilename,DWORD dwCreate)
{
	return CreateFile(szFilename,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ	| FILE_SHARE_WRITE,
				NULL,dwCreate,FILE_ATTRIBUTE_NORMAL,NULL);
}

HANDLE OpenForRead(LPSTR szFilename,DWORD dwCreate)
{
	return CreateFile(szFilename,GENERIC_READ,FILE_SHARE_READ,
				NULL,dwCreate,FILE_ATTRIBUTE_NORMAL,NULL);
}

BOOL FailOpen(HANDLE hFile)
{
	if(hFile==INVALID_HANDLE_VALUE)
   	return TRUE;
   else
   	return FALSE;
}

int OSType(void)
{
	OSVERSIONINFO osVer;

   osVer.dwOSVersionInfoSize = sizeof(osVer);
   if (!GetVersionEx(&osVer))
   	return (FALSE);

   switch(osVer.dwPlatformId)
	{
   	case VER_PLATFORM_WIN32_WINDOWS:
      	return WIN9X;
      case VER_PLATFORM_WIN32s:
      	return WIN3X;
      case VER_PLATFORM_WIN32_NT:
      	return NT;
   }
   return (FALSE);
}

BOOL SaveBmp(HBITMAP hbm,LPSTR szFilename)
{
	BITMAPFILEHEADER bmfh;
   BITMAPINFOHEADER bmih;
   BITMAPINFO 		  bmif;
	BITMAP bm;
   HANDLE hFile;
   DWORD dwtemp;
   HGLOBAL hgMem;
   LPSTR hDate;
   HDC hDC;

   if(GetObject(hbm,sizeof(BITMAP),&bm)==0)
   {
   	Errmsg("���󣺲��ܻ�ȡͼ����Ϣ");
      return FALSE;
   }

   ZeroMemory(&bmfh,sizeof(BITMAPFILEHEADER));
   bmfh.bfType=0x4d42;
   bmfh.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+bm.bmWidth*bm.bmHeight*3;
   bmfh.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);

   hFile=Open(szFilename,CREATE_ALWAYS);
   if(FailOpen(hFile))
   {
   	Errmsg("�ļ���ȡ����");
      return FALSE;
   }

   if(!WriteFile(hFile,&bmfh,sizeof(BITMAPFILEHEADER),&dwtemp,NULL))
   {
   	Errmsg("���󣺴洢�ļ�����");
      CloseHandle(hFile);
      return FALSE;
   }

   ZeroMemory(&bmih,sizeof(BITMAPINFOHEADER));
   bmih.biSize=sizeof(BITMAPINFOHEADER);
   bmih.biWidth=bm.bmWidth;
   bmih.biHeight=bm.bmHeight;
   bmih.biPlanes=1;
   bmih.biBitCount=24;
   bmih.biCompression=BI_RGB;

   if(!WriteFile(hFile,&bmih,sizeof(BITMAPINFOHEADER),&dwtemp,NULL))
   {
   	Errmsg("���󣺴洢�ļ�����");
      CloseHandle(hFile);
      return FALSE;
   }

   hgMem=GlobalAlloc(GHND,bm.bmWidth*bm.bmHeight*3);
   if(hgMem==NULL)
   {
   	Errmsg("�ڴ�������.");
      CloseHandle(hFile);
      return FALSE;
   }
   hDate=(LPSTR)GlobalLock(hgMem);

   bmif.bmiHeader=bmih;

   hDC=GetDC(HWND_DESKTOP);
   GetDIBits(hDC,hbm,0,bm.bmHeight,hDate,&bmif,DIB_RGB_COLORS);
   ReleaseDC(HWND_DESKTOP,hDC);

   if(!WriteFile(hFile,hDate,bm.bmWidth*bm.bmHeight*3,&dwtemp,NULL))
   {
   	Errmsg("���󣺴洢�ļ�����");
      GlobalUnlock(hgMem);
      GlobalFree(hgMem);
      CloseHandle(hFile);
      return FALSE;
   }

   GlobalUnlock(hgMem);
   GlobalFree(hgMem);
   CloseHandle(hFile);

   return TRUE;
}

BOOL Read(HANDLE hFile,LPVOID lpBuf,DWORD dwNum)
{
   DWORD dwtemp;

	if(!ReadFile(hFile,lpBuf,dwNum,&dwtemp,NULL))
   	return FALSE;

   if(dwtemp!=dwNum)
	   return FALSE;

   return TRUE;
}

BOOL Write(HANDLE hFile,LPVOID lpBuf,DWORD dwNum)
{
   DWORD dwtemp;

	if(!WriteFile(hFile,lpBuf,dwNum,&dwtemp,NULL))
   	return FALSE;

   if(dwtemp!=dwNum)
		return FALSE;

   return TRUE;
}

DWORD GetFilePointer(HANDLE hFile)
{
	return SetFilePointer(hFile,0,NULL,FILE_CURRENT);
}

HANDLE NewThread(LPTHREAD_START_ROUTINE lpStartAddress,LPDWORD lpThreadId)
{
	return CreateThread(NULL,0,lpStartAddress,0,0,lpThreadId);
}

int WinWidth(HWND hWnd)
{
	RECT rt;

   GetWindowRect(hWnd,&rt);

   return (rt.right-rt.left);
}

int WinHeight(HWND hWnd)
{
	RECT rt;

   GetWindowRect(hWnd,&rt);

   return (rt.bottom-rt.top);
}

BOOL GetBmpSize(HBITMAP hbm,SIZE *size)
{
	BITMAP bm;

   if(GetObject(hbm,sizeof(BITMAP),&bm)==0)
   	return FALSE;

   size->cx=bm.bmWidth;
   size->cy=bm.bmHeight;

   return TRUE;
}

LPVOID New(DWORD dwSize)
{
	HGLOBAL hgMem;

   hgMem=GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT,dwSize);
   if(hgMem==NULL)
   {
   	Errmsg("�ڴ����ʧ��,���������ڴ�ռ�");
      return NULL;
   }

   return (LPVOID)hgMem;
}

LPVOID Del(LPVOID lpv)
{
   if(lpv!=NULL)
   	GlobalFree(lpv);
   return NULL;
}

void ShowHelpInfo(HINSTANCE hInst,LPSTR szText,LPSTR szCaption,WORD nIconID)
{
	char Author[11];
	MSGBOXPARAMS msgbox ;
   char a[200]="aa";
   int i,j=0;

   Author[0]=0xD7;
   Author[2]=0xD5;
   Author[1]=0xF7;
   Author[3]=0xDF;
   Author[6]=0xC1;
   Author[4]=0xA3;
   Author[5]=0xBA;
   Author[7]=0xD9;
   Author[9]=0xE7;
   Author[8]=0xB7;
   Author[10]='\0';

   for(i=0;i<300;i++)
   {
      if(szText[i]=='\0')
      	break;
   	if(szText[i]!='[')
      {
      	a[j]=szText[i];
         j++;
      }
      else
      {
      	strcat(a,Author);
         j+=10;
         for(;i<300;i++)
         	if(szText[i]==']')
            	break;
            else if(szText[i]=='\0')
            	break;
      }
   }
   strcat(a,"\0");

   msgbox.cbSize = sizeof( msgbox) ;
   msgbox.hwndOwner = HWND_DESKTOP ;
   msgbox.hInstance = hInst ;
   msgbox.lpszText = a ;
   msgbox.lpszCaption = szCaption ;
   msgbox.dwStyle = MB_OK | MB_USERICON ;
   msgbox.lpszIcon = MAKEINTRESOURCE(nIconID);
   msgbox.dwContextHelpId = 1 ;
   msgbox.lpfnMsgBoxCallback = NULL ;
   msgbox.dwLanguageId = MAKELANGID( LANG_NEUTRAL, SUBLANG_NEUTRAL) ;

   MessageBoxIndirect( &msgbox);
}

