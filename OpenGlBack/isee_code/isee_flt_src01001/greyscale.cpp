/********************************************************************

	greyscale.cpp - ISee图像浏览器—图像处理模块图像灰度转换处理实现代码文件

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

   功能实现：图像灰度转换

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"

//灰度转换
int DoGreyScale(LPIMAGEPROCSTR lpInfo)
{
	//创建内存图像
   lpInfo->_pdbdata = (unsigned char*)New(lpInfo->sImageInfo.width*lpInfo->sImageInfo.height*(lpInfo->sImageInfo.bitperpix/8));
   if(lpInfo->_pdbdata==NULL)
   {
   	lpInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //填写新建图像信息
   lpInfo->dImageInfo=lpInfo->sImageInfo;

	LPBYTE pRed=(LPBYTE)lpInfo->_psbdata,pdBuf=(LPBYTE)lpInfo->_pdbdata;
   LPBYTE pGrn=pRed+1, pBlu=pRed+2;
	UINT loop=(UINT)(lpInfo->sImageInfo.width*lpInfo->sImageInfo.height);
   int lum;
   int skip=lpInfo->sImageInfo.bitperpix/8;

   //灰度转换(361转换)
   for (UINT i=0;i<loop;i++)
   {
		lum = (int)(.299 * (double)(*pRed) + .587 * (double)(*pGrn) + .114 * (double)(*pBlu));

		*pdBuf = (BYTE)lum;
      pdBuf++;
      *pdBuf = (BYTE)lum;
      pdBuf++;
      *pdBuf = (BYTE)lum;
      pdBuf+=(skip-2);

      pRed+=skip;
      pGrn+=skip;
      pBlu+=skip;
	}

   lpInfo->result=PR_SUCCESS;
   return PROCERR_SUCCESS;
}

