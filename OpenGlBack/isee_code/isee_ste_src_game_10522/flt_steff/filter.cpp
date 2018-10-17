#include <windows.h>
#include "gol_proc.h"
#include "filter.h"

LPIMAGEPROCSTR lpProcInfo;

//VC �û���� _export ��Ϊ __declspec(dllexport)
int WINAPI FAR _export AccessPropFilter(LPIMAGEPROCSTR lpInfo)
{
   if(lpInfo==NULL)
   	return PROCERR_NULLPARAM;

   lpProcInfo=lpInfo;

   switch(lpInfo->comm)
   {
   	case PCM_NULL:
      	lpInfo->result=PR_NULL;
      	return PROCERR_FALSE;
      case PCM_ROTATE:
         if(lpInfo->sImageInfo.bitperpix<16)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoRotate(lpInfo);
   }
   lpInfo->result=PR_ILLCOMM;
   return PROCERR_FALSE;
}

