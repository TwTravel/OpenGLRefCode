/********************************************************************

	dllshell.cpp - ISee图像浏览器—图像处理模块代码文件

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

   功能实现：DLL入口函数及主函数实现

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include <windows.h>
#include "gol_proc.h"
#include "filter.h"

LPIMAGEPROCSTR lpProcInfo;

//DLL主函数
#pragma argsused	//BC 专用，防止警告信息，VC用户请删除该行
BOOL WINAPI DllEntryPoint( HINSTANCE hinstDll,DWORD fdwRreason,LPVOID plvReserved)
{
    return 1;   // Indicate that the DLL was initialized successfully.
}

//DLL入口函数
//VC 用户请把 _export 改为 __declspec(dllexport)
int WINAPI FAR _export AccessPropFilter(LPIMAGEPROCSTR lpInfo)
{
   if(lpInfo==NULL)
   	return PROCERR_NULLPARAM;

   lpProcInfo=lpInfo;

   switch(lpInfo->comm)
   {
   	case PCM_NULL:                       	//未操作
      	lpInfo->result=PR_NULL;
      	return PROCERR_FALSE;
      case PCM_GETWRITERMESS:
      	return ShowWriterMessage();
      case PCM_ROTATE:                       //旋转
         if(lpInfo->sImageInfo.bitperpix<16 || lpInfo->_psbdata==NULL)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoRotate(lpInfo);
      case PCM_GREYSCALE:                    //灰度转换
      	if(lpInfo->sImageInfo.bitperpix<24 || lpInfo->_psbdata==NULL)
         {
         	lpInfo->result=PR_COMMINFOERR;
            return PROCERR_FALSE;
         }
      	return DoGreyScale(lpInfo);
      case PCM_RESIZE:	                    //重定义尺寸
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




