#include <windows.h>
#include "gol_proc.h"

int RunDll(LPSTR szDllname,LPIMAGEPROCSTR pInfo)
{
	typedef int WINAPI (*MYPROC)(LPIMAGEPROCSTR lpInfo);
   HMODULE hLib;

   MYPROC lpFunc;
   char s[MAX_PATH];

   hLib=LoadLibrary(szDllname);
   if(hLib==NULL)
   {
      wsprintf(s,"���ض�̬���ӿ�ʧ�ܣ�%s",szDllname);
     	MessageBox(GetFocus(),s,"����",MB_OK);
      return FALSE;
   }

   lpFunc=(MYPROC)GetProcAddress(hLib,MAKEINTRESOURCE(1));//"AccessFilterModel");//
   if(lpFunc==NULL)
   {
     	MessageBox(GetFocus(),"����ʧ�ܣ�δ�ҵ���ں���","����",MB_OK);
      return FALSE;
   }
   pInfo->hInst=hLib;
   int bOK=(lpFunc)(pInfo);
   FreeLibrary(hLib);

   return bOK;
}
