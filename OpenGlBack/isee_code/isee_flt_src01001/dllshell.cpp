/********************************************************************

	dllshell.cpp - ISeeͼ���������ͼ����ģ������ļ�

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

   ����ʵ�֣�DLL��ں�����������ʵ��

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include <windows.h>
#include "gol_proc.h"
#include "filter.h"

LPIMAGEPROCSTR lpProcInfo;

//DLL������
#pragma argsused	//BC ר�ã���ֹ������Ϣ��VC�û���ɾ������
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,DWORD fdwRreason,LPVOID plvReserved)
{
    return 1;   // Indicate that the DLL was initialized successfully.
}

//DLL��ں���
//VC �û���� _export ��Ϊ __declspec(dllexport)
int WINAPI FAR _export AccessPropFilter(LPIMAGEPROCSTR lpInfo)
{
   if(lpInfo==NULL)
   	return PROCERR_NULLPARAM;

   lpProcInfo=lpInfo;

   switch(lpInfo->comm)
   {
   	case PCM_NULL:                       	//δ����
      	lpInfo->result=PR_NULL;
      	return PROCERR_FALSE;
      case PCM_GETWRITERMESS:
      	return ShowWriterMessage();
      case PCM_ROTATE:                       //��ת
         if(lpInfo->sImageInfo.bitperpix<16 || lpInfo->_psbdata==NULL)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoRotate(lpInfo);
      case PCM_GREYSCALE:                    //�Ҷ�ת��
      	if(lpInfo->sImageInfo.bitperpix<24 || lpInfo->_psbdata==NULL)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoGreyScale(lpInfo);
      case PCM_RESIZE:	                    //�ض���ߴ�
      	if(lpInfo->_psbdata==NULL)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoResize(lpInfo);
   }
   lpInfo->result=PR_ILLCOMM;
   return PROCERR_FALSE;
}




