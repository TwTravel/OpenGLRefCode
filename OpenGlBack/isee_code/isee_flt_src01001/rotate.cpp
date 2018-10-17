/********************************************************************

	rotate.cpp - ISee图像浏览器—图像处理模块图像旋转处理实现代码文件

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

   功能实现：图像旋转

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "draw.h"
#include "rotate.h"
#include "resource.h"

#define UL	0x00			//原图像左上角旋转后在左上角
#define DL	0x40        //原图像左上角旋转后在左下角
#define DR	0x80        //原图像左上角旋转后在右下角
#define UR	0xc0        //原图像左上角旋转后在右上角

LPVOID lpPreViewData=NULL,lpBakData=NULL;
int prev_width,prev_height,mem_size;
RECT rt;
extern HBITMAP hbm;
BOOL bRotate=FALSE;		//图像是否旋转标志（即图像宽高是否调换）
BYTE pos=UL;            //原图像左上角旋转后位置

void Out_Rotate();
void Out_Flip();

//绘制预览图像
void DrawPreView(HWND hWnd,LPDRAWITEMSTRUCT lpInfo)
{
   //绘制背景
   HDC hmemdc;
   hmemdc=CreateCompatibleDC(lpInfo->hDC);
   // YZ Modify 2000-8-15 适当恢复DC中的单色位图
   HBITMAP hOldBmp = (HBITMAP)SelectObject(hmemdc,hbm);
   //SelectObject(hmemdc,hbm);
   for(int j=0;j<lpInfo->rcItem.bottom;j+=48)
     	for(int i=0;i<lpInfo->rcItem.right;i+=48)
        	BitBlt(lpInfo->hDC,i,j,48,48,hmemdc,0,0,SRCCOPY);

   SelectObject(hmemdc, hOldBmp);

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
      	// YZ Modify 2000-8-15	安全第一:-)
         if (lptemp) lptemp=Del(lptemp);
         if (lpBakData) lpBakData=Del(lpBakData);

   		lpProcInfo->result=PR_MEMORYERR;
	      SendMessage(hWnd,WM_CLOSE,0,0);
   	   return;
	   }
      //生成预览图像
		Output_Resize(prev_width,prev_height,(LPBYTE)lptemp);
   	lpPreViewData = lptemp;
      CopyMemory(lpBakData,lpPreViewData,mem_size);
   }
   DoDlgDraw(lpInfo->hDC,&rt,prev_width,prev_height,lpPreViewData);
}

//预览图像向左旋转90度
void TurnLeft()
{
	if(lpPreViewData==NULL)
   	return;

   int temp;

   temp=rt.top;
   rt.top=rt.left;
   rt.left=temp;
   temp=rt.right;
   rt.right=rt.bottom;
   rt.bottom=temp;

   temp=prev_width;
   prev_width=prev_height;
   prev_height=temp;

//标记图像已旋转，记录原图像左上角旋转后位置
   bRotate = !bRotate;
   pos = (BYTE)(pos+0x40);

//prev_width与prev_height已调换，所以以下编程请注意:新图像宽高不调换，旧图像调换

	int skip = lpProcInfo->sImageInfo.bitperpix/8;
	LPBYTE lpData=(LPBYTE)lpBakData;
   int new_width=prev_width*(lpProcInfo->sImageInfo.bitperpix/8);
   LPBYTE lpDest=(LPBYTE)lpPreViewData+new_width-skip,lpBak=lpDest;

	for(int j=1;j<=prev_width;j++)
   {
   	for(int i=0;i<prev_height;i++)
      {
      	for(int k=0;k<skip;k++)
         {
         	*lpDest=*lpData;
            lpDest++;
            lpData++;
         }
         lpDest+=(new_width-skip);
      }
      lpBak-=skip;
      lpDest=lpBak;
   }
   CopyMemory(lpBakData,lpPreViewData,mem_size);
}

//预览图像向右旋转90度
void TurnRight()
{
	if(lpPreViewData==NULL)
   	return;

   int temp;

   temp=rt.top;
   rt.top=rt.left;
   rt.left=temp;
   temp=rt.right;
   rt.right=rt.bottom;
   rt.bottom=temp;

   temp=prev_width;
   prev_width=prev_height;
   prev_height=temp;

   bRotate = !bRotate;
   pos = (BYTE)(pos-0x40);

	int skip = lpProcInfo->sImageInfo.bitperpix/8;
	LPBYTE lpData=(LPBYTE)lpBakData;
   int new_width=prev_width*(lpProcInfo->sImageInfo.bitperpix/8);
   LPBYTE lpDest=(LPBYTE)lpPreViewData+new_width*(prev_height-1),lpBak=lpDest;

	for(int j=1;j<=prev_width;j++)
   {
   	for(int i=0;i<prev_height;i++)
      {
      	for(int k=0;k<skip;k++)
         {
         	*lpDest=*lpData;
            lpDest++;
            lpData++;
         }
         lpDest-=(new_width+skip);
      }
      lpBak+=skip;
      lpDest=lpBak;
   }
   CopyMemory(lpBakData,lpPreViewData,mem_size);
}

//预览图像横向反转
void HFlip()
{
	if(lpPreViewData==NULL)
   	return;

   pos ^= 0xc0;

	int skip = lpProcInfo->sImageInfo.bitperpix/8;
   int width=prev_width*skip;
   LPBYTE lpData=(LPBYTE)lpBakData;
   LPBYTE lpDest=(LPBYTE)lpPreViewData+width-skip;
   int width_plus=skip<<1;
   int height_plus=width<<1;

   for(int j=0;j<prev_height;j++)
   {
   	for(int i=0;i<prev_width;i++)
      {
      	for(int k=0;k<skip;k++)
         {
         	*lpDest=*lpData;
            lpDest++;
            lpData++;
         }
         lpDest-=width_plus;
      }
      lpDest+=height_plus;
   }
   CopyMemory(lpBakData,lpPreViewData,mem_size);
}

//预览图像纵向反转
void VFlip()
{
	if(lpPreViewData==NULL)
   	return;

   pos ^= 0x40;

	int width=prev_width*(lpProcInfo->sImageInfo.bitperpix/8);
   LPBYTE lpData=(LPBYTE)lpBakData;
   LPBYTE lpDest=(LPBYTE)lpPreViewData+width*(prev_height-1);
   int height_plus=width<<1;

   for(int j=0;j<prev_height;j++)
   {
   	for(int i=0;i<width;i++)
      {
      	*lpDest=*lpData;
         lpDest++;
         lpData++;
      }
      lpDest-=height_plus;
   }
   CopyMemory(lpBakData,lpPreViewData,mem_size);
}

//实际图像旋转处理
int Output_Rotate()
{
	//创建内存图像
   lpProcInfo->_pdbdata = (unsigned char*)New(lpProcInfo->sImageInfo.width*lpProcInfo->sImageInfo.height*(lpProcInfo->sImageInfo.bitperpix/8));
   if(lpProcInfo->_pdbdata==NULL)
   {
   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //填写旋转后图像信息
   if(bRotate)
   {
		lpProcInfo->dImageInfo.width=lpProcInfo->sImageInfo.height;
      lpProcInfo->dImageInfo.height=lpProcInfo->sImageInfo.width;
   }
   else
   {
		lpProcInfo->dImageInfo.width=lpProcInfo->sImageInfo.width;
      lpProcInfo->dImageInfo.height=lpProcInfo->sImageInfo.height;
   }

   lpProcInfo->pdLineAddr=(unsigned long**)New(lpProcInfo->dImageInfo.height*sizeof(long));
   if(lpProcInfo->pdLineAddr==NULL)
   {
   	// YZ Modify 2000-8-15	增加了释放上面那个内存块的代码
      lpProcInfo->_pdbdata = Del(lpProcInfo->_pdbdata);

   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   lpProcInfo->dImageInfo.bitperpix=lpProcInfo->sImageInfo.bitperpix;
   lpProcInfo->dImageInfo.bAlpha=lpProcInfo->sImageInfo.bAlpha;
   // YZ Modify 2000-8-15	因为图像处理模块将始终工作在32位位图格式，所以
   // 下面的几行可不要。
   //int bpl=lpProcInfo->dImageInfo.width*(lpProcInfo->sImageInfo.bitperpix/8);
   //bpl=bpl%4==0?bpl:(bpl/4+1)*4;
   //lpProcInfo->dImageInfo.byteperline=bpl;
   lpProcInfo->dImageInfo.byteperline=lpProcInfo->dImageInfo.width*4;


	// YZ Modify 2000-8-15	下面原来的那行代码的计算方式有误，因为ISee内部
   // 标准图像格式是倒向的DIB（lpProcInfo->dImageInfo.height的值是一个正值），
   // 所以，第一行的地址应该在位缓冲的末尾。
   for(int i=0;i<lpProcInfo->dImageInfo.height;i++)
      lpProcInfo->pdLineAddr[i]=(unsigned long*)(lpProcInfo->_pdbdata+((lpProcInfo->dImageInfo.height-i-1)*(lpProcInfo->dImageInfo.width*4)));
   	//lpProcInfo->pdLineAddr[i]=(unsigned long*)lpProcInfo->_pdbdata+lpProcInfo->dImageInfo.byteperline*i;


	//根据图像旋转标志做相应处理
   if(bRotate)
   	Out_Rotate();
   else
   	Out_Flip();

   lpProcInfo->result=PR_SUCCESS;
   return PROCERR_SUCCESS;
}

//图像宽高已调换，做旋转处理
void Out_Rotate()
{
	int skip = lpProcInfo->sImageInfo.bitperpix/8;
	LPBYTE lpData=(LPBYTE)lpProcInfo->_psbdata;
   int new_width=lpProcInfo->dImageInfo.width*skip;
   LPBYTE lpDest;
   int width_plus,height_plus;

   switch(pos)
   {
   	case UL:        //图像向左旋转90度后上下反转
   		lpDest=(LPBYTE)lpProcInfo->_pdbdata+new_width*lpProcInfo->dImageInfo.height-skip;
         width_plus=-new_width-skip;
         height_plus=-skip;
         break;
      case DL:        //图像向左旋转90度
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+new_width-skip;
         width_plus=new_width-skip;
         height_plus=-skip;
         break;
      case DR:        //图像向右旋转90度后上下反转
         lpDest=(LPBYTE)lpProcInfo->_pdbdata;
         width_plus=new_width-skip;
         height_plus=skip;
         break;
      case UR:        //图像向右旋转90度
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+new_width*(lpProcInfo->dImageInfo.height-1);
         width_plus=-new_width-skip;
         height_plus=skip;
         break;
   }

   LPBYTE lpBak=lpDest;

	for(int j=0;j<lpProcInfo->sImageInfo.height;j++)
   {
   	for(int i=0;i<lpProcInfo->sImageInfo.width;i++)
      {
      	for(int k=0;k<skip;k++)
         {
         	*lpDest=*lpData;
            lpDest++;
            lpData++;
         }
         lpDest+=width_plus;
      }
      lpBak+=height_plus;
      lpDest=lpBak;
   }
}

//图像宽高未调换，做反转处理
void Out_Flip()
{
	int skip = lpProcInfo->sImageInfo.bitperpix/8;
	LPBYTE lpData=(LPBYTE)lpProcInfo->_psbdata;
   int width=lpProcInfo->sImageInfo.width*skip;
   LPBYTE lpDest;
   int width_plus,height_plus;

   switch(pos)
   {
      case UL:        //与原图像相同，直接COPY图像数据
      	CopyMemory(lpProcInfo->_pdbdata,lpProcInfo->_psbdata,lpProcInfo->sImageInfo.width*lpProcInfo->sImageInfo.height*(lpProcInfo->sImageInfo.bitperpix/8));
         return;
   	case DL:        //图像上下反转
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+width*(lpProcInfo->dImageInfo.height-1);
         width_plus=0;
         height_plus=-(width<<1);
         break;
      case DR:        //图像旋转180度
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+width*lpProcInfo->dImageInfo.height-skip;
         width_plus=-(skip<<1);
         height_plus=0;
         break;
      case UR:        //图像左右反转
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+width-skip;
         width_plus=-(skip<<1);
         height_plus=width<<1;
         break;
   }

   for(int j=0;j<lpProcInfo->sImageInfo.height;j++)
   {
   	for(int i=0;i<lpProcInfo->sImageInfo.width;i++)
      {
         for(int k=0;k<skip;k++)
         {
      		*lpDest=*lpData;
         	lpDest++;
            lpData++;
         }
         lpDest+=width_plus;
      }
      lpDest+=height_plus;
   }
}

