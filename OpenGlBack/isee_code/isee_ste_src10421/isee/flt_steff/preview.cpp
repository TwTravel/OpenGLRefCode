/********************************************************************

	preview.cpp - ISee图像浏览器—图像处理模块图像重定义尺寸处理、预览图象实现代码文件

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

   功能实现：绘制预览图象

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "draw.h"

LPVOID lpPreViewData=NULL,lpBakData=NULL;
int prev_width,prev_height,mem_size;
extern int perlin_waveX,perlin_waveY,perlin_level;
RECT rt;
extern HBITMAP hbm;

//绘制对话框背景
void PaintDlgBk(HWND hWnd)
{
  	PAINTSTRUCT ps;
   HDC hmemdc;
   int x,y;

	x=WinWidth(hWnd);
   y=WinHeight(hWnd);
   BeginPaint(hWnd,&ps);
   hmemdc=CreateCompatibleDC(ps.hdc);
   SelectObject(hmemdc,hbm);
   for(int j=0;j<y;j+=48)
     	for(int i=0;i<x;i+=48)
        	BitBlt(ps.hdc,i,j,48,48,hmemdc,0,0,SRCCOPY);
   DeleteDC(hmemdc);
   EndPaint(hWnd,&ps);
}

//绘制预览图像
void DrawPreView(HWND hWnd,LPDRAWITEMSTRUCT lpInfo)
{
   //绘制背景
   HDC hmemdc;
   hmemdc=CreateCompatibleDC(lpInfo->hDC);
   SelectObject(hmemdc,hbm);
   for(int j=0;j<lpInfo->rcItem.bottom;j+=48)
     	for(int i=0;i<lpInfo->rcItem.right;i+=48)
        	BitBlt(lpInfo->hDC,i,j,48,48,hmemdc,0,0,SRCCOPY);
   DeleteDC(hmemdc);

   float x,y;
	if(lpPreViewData==NULL)
   {
   	lpPreViewData=(LPVOID)lpProcInfo->_psbdata;
      if(lpPreViewData==NULL)
      	return;

      //求预览图片尺寸
      int rcw=lpInfo->rcItem.right-lpInfo->rcItem.left;
      int rch=lpInfo->rcItem.bottom-lpInfo->rcItem.top;

      x=(float)lpProcInfo->sImageInfo.width;
      y=(float)lpProcInfo->sImageInfo.height;

      if(lpProcInfo->sImageInfo.width > rcw || lpProcInfo->sImageInfo.height > rch)
      {
      	if(lpProcInfo->sImageInfo.width >= lpProcInfo->sImageInfo.height)
			{
   	   	prev_width=rcw;
      	   x=y/x;
         	x=x*prev_width;
            if(x<8)
            	x=8;
	         prev_height=(int)x;

   	      rt.left=0;
      	   rt.top=(prev_width-prev_height)/2;
         	rt.right=prev_width;
	         rt.bottom=rt.top+prev_height;
   	   }
      	else
	      {
   	      prev_height=rch;
      	   x=x/y;
         	x=x*prev_height;
            if(x<8)
            	x=8;
	         prev_width=(int)x;

   	      rt.left=(prev_height-prev_width)/2;
      	   rt.top=0;
         	rt.right=rt.left+prev_width;
	         rt.bottom=prev_height;
   	   }
      }
      else
      {
      	prev_width=lpProcInfo->sImageInfo.width;
         prev_height=lpProcInfo->sImageInfo.height;
         rt.left=(rcw-prev_width)/2;
         rt.top=(rch-prev_height)/2;
         rt.right=rt.left+prev_width;
         rt.bottom=rt.top+prev_height;
      }

	   //创建prev_width*prev_height内存图像
      mem_size=prev_width*prev_height*(lpProcInfo->sImageInfo.bitperpix/8);
   	LPVOID lptemp=New(mem_size);
      lpBakData=New(mem_size);
	   if(lptemp==NULL || lpBakData==NULL)
   	{
   		lpProcInfo->result=PR_MEMORYERR;
	      SendMessage(hWnd,WM_CLOSE,0,0);
   	   return;
	   }
      //生成预览图像
		Output_Resize(prev_width,prev_height,(LPBYTE)lptemp);
   	lpPreViewData = lptemp;
      CopyMemory(lpBakData,lpPreViewData,mem_size);
   }
   Output_PerlinNoise(prev_width,prev_height,perlin_waveX,perlin_waveY,perlin_level,(COLORREF*)lpPreViewData);
   DoDlgDraw(lpInfo->hDC,&rt,prev_width,prev_height,lpPreViewData);
}

//重定义尺寸
int Output_Resize(int width,int height,LPBYTE lpDestData)
{
	LPBYTE lpData=(LPBYTE)lpProcInfo->_psbdata,lpBak,lpBako;
   LPBYTE lpdData=lpDestData;

   if(lpDestData==NULL )
   	return PROCERR_FALSE;

   int skip = lpProcInfo->sImageInfo.bitperpix/8;

  	float x=(float)lpProcInfo->sImageInfo.width/(float)width;
   float y=(float)lpProcInfo->sImageInfo.height/(float)height;
	float x_cnt,y_cnt=0.00;

   lpBak=lpBako=lpData;
   for(int j=0;j<height;j++)
   {
		x_cnt=0.00;
     	for(int i=0;i<width;i++)
      {
        	for(int k=0;k<skip;k++)
         {
           	*lpdData=*lpData;
            lpdData++;
            lpData++;
         }
         x_cnt+=x;
         lpData=lpBak+skip*(int)x_cnt;
      }
		y_cnt+=y;
  	   lpBak=lpBako+lpProcInfo->sImageInfo.width*skip*(int)y_cnt;
		lpData=lpBak;
   }
   return TRUE;
}

//恢复原始预览图象
void RestorePreviewData()
{
   CopyMemory(lpPreViewData,lpBakData,prev_width*prev_height*(lpProcInfo->sImageInfo.bitperpix/8));
}
