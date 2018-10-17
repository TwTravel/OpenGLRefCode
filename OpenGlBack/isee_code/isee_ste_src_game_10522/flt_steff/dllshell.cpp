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
#include "p_win.h"
#include "resource.h"

LPIMAGEPROCSTR lpProcInfo;

//DLL������
#pragma argsused	//BC ר�ã���ֹ������Ϣ��VC�û���ɾ������
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,DWORD fdwRreason,LPVOID plvReserved)
{
    return 1;   // Indicate that the DLL was initialized successfully.
}

//DLL��ں���
//VC �û���� _export ��Ϊ __declspec(dllexport)
int WINAPI FAR _export AccessStEffFilter(LPIMAGEPROCSTR lpInfo)
{
   if(lpInfo==NULL)
   	return PROCERR_NULLPARAM;

   lpProcInfo=lpInfo;

   switch(lpInfo->comm)
   {
   	case PCM_NULL:                       	//δ����
      	lpInfo->result=PR_NULL;
      	return PROCERR_FALSE;
      case PCM_PERLINNOISE:                  //����
         if(lpInfo->sImageInfo.bitperpix<16 || lpInfo->_psbdata==NULL)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoPerlinNoise(lpInfo);
      case PCM_GAME_PINTU:
      	lpInfo->result=PR_NULL;
         return DoGame_Pintu(lpInfo);
   }
   lpInfo->result=PR_ILLCOMM;
   return PROCERR_FALSE;
}

void ShowCopyright()
{
	ShowHelpInfo(lpProcInfo->hInst,"ISeeͼ��̬Ч������ģ��\n(C)Copyright 2/26,2001 [my name]\n�汾��1.0.0.0226","ISeeͼ����",IDI_ISEE);
}

