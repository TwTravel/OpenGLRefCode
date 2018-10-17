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
      wsprintf(s,"加载动态连接库失败：%s",szDllname);
     	MessageBox(GetFocus(),s,"错误",MB_OK);
      return FALSE;
   }

   lpFunc=(MYPROC)GetProcAddress(hLib,MAKEINTRESOURCE(1));//"AccessFilterModel");//
   if(lpFunc==NULL)
   {
     	MessageBox(GetFocus(),"运行失败：未找到入口函数","错误",MB_OK);
      return FALSE;
   }
   pInfo->hInst=hLib;
   int bOK=(lpFunc)(pInfo);
   FreeLibrary(hLib);

   return bOK;
}
