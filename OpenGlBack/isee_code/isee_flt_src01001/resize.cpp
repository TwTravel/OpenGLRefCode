/********************************************************************

	resize.cpp - ISee图像浏览器—图像处理模块图像重定义尺寸处理实现代码文件

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

   功能实现：重定义图像尺寸

	文件版本：
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "resize.h"

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

int Resize(int w,int h,int rs_flag,int rs_pos,COLORREF rs_bgcolor)
{
   // Add by YZ 2000-9-22. 因为图像处理模块始终工作于32位模式，所以直接乘4
	lpProcInfo->_pdbdata = (unsigned char*)New(w*h*4);
	//lpProcInfo->_pdbdata = (unsigned char*)New(w*h*(lpProcInfo->sImageInfo.bitperpix/8));
   if(lpProcInfo->_pdbdata==NULL)
   {
   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //填写图像信息
   lpProcInfo->dImageInfo.width=w;
   lpProcInfo->dImageInfo.height=h;

   lpProcInfo->pdLineAddr=(unsigned long**)New(lpProcInfo->dImageInfo.height*sizeof(long));
   if(lpProcInfo->pdLineAddr==NULL)
   {
		// Add by YZ 2000-9-22	增加了释放上面那个内存块的代码
		lpProcInfo->_pdbdata = Del(lpProcInfo->_pdbdata);

   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   lpProcInfo->dImageInfo.bitperpix=lpProcInfo->sImageInfo.bitperpix;
   lpProcInfo->dImageInfo.bAlpha=lpProcInfo->sImageInfo.bAlpha;
   // YZ Modify 2000-9-22	因为图像处理模块将始终工作在32位位图格式，所以
   // 下面的几行可不要。
   //int bpl=w*(lpProcInfo->sImageInfo.bitperpix/8);
   //bpl=bpl%4==0?bpl:(bpl/4+1)*4;
   //lpProcInfo->dImageInfo.byteperline=bpl;
   lpProcInfo->dImageInfo.byteperline=lpProcInfo->dImageInfo.width*4;

	// YZ Modify 2000-9-22	下面原来的那行代码的计算方式有误，因为ISee内部
   // 标准图像格式是倒向的DIB（lpProcInfo->dImageInfo.height的值是一个正值），
   // 所以，第一行的地址应该在位缓冲的末尾。
   for(int i=0;i<lpProcInfo->dImageInfo.height;i++)
   	lpProcInfo->pdLineAddr[i]=(unsigned long*)(lpProcInfo->_pdbdata+((lpProcInfo->dImageInfo.height-i-1)*(lpProcInfo->dImageInfo.width*4)));
   	//lpProcInfo->pdLineAddr[i]=(unsigned long*)lpProcInfo->_pdbdata+lpProcInfo->dImageInfo.byteperline*i;

	switch(rs_flag)
   {
   	case RS_STRETCH:
      {
         int r=Output_Resize(w,h,lpProcInfo->_pdbdata);
         if(r)
         {
      		lpProcInfo->result=PR_SUCCESS;
   			return PROCERR_SUCCESS;
         }
         else
         	return r;
      }
      case RS_TILE:
      	return Output_ResizeTile(w,h,lpProcInfo->_pdbdata);
      case RS_CENTER:
			return Output_ResizePos(w,h,lpProcInfo->_pdbdata,RS_CENTER,&rs_bgcolor);
      case RS_KEEP:
			return Output_ResizePos(w,h,lpProcInfo->_pdbdata,rs_pos,&rs_bgcolor);
   }
   return FALSE;
}

//平铺
int Output_ResizeTile(int width,int height,LPBYTE lpDestData)
{
	LPBYTE lpsData=(LPBYTE)lpProcInfo->_psbdata,lpBak,lpBako;
   LPBYTE lpdData=lpDestData;

   if(lpDestData==NULL )
   	return PROCERR_FALSE;

   int skip = lpProcInfo->sImageInfo.bitperpix/8;

  	int x=lpProcInfo->sImageInfo.width;
   int y=lpProcInfo->sImageInfo.height;

   int wc=0,hc=0;

   lpsData+=x*(y-1)*skip;
   lpdData+=width*(height-1)*skip;
   lpBako=lpBak=lpsData;

   for(int j=0;j<height;j++)
   {
		for(int i=0;i<width;i++)
      {
        	for(int k=0;k<skip;k++)
         {
           	*lpdData=*lpsData;
            lpdData++;
            lpsData++;
         }
         wc++;
         if(wc>=x)
         {
         	wc=0;
         	lpsData=lpBak;
         }
      }
      wc=0;
  	   lpBak-=(x*skip);
      lpdData-=(width*skip*2);
      hc++;
      if(hc>=y)
      {
      	hc=0;
      	lpBak=lpBako;
      }
		lpsData=lpBak;
   }
   lpProcInfo->result=PR_SUCCESS;
	return PROCERR_SUCCESS;
}

//保持图像(包括居中)
int Output_ResizePos(int w,int h,LPBYTE lpDestData,int rs_pos,COLORREF* rs_bgcolor)
{
	LPBYTE lpsData=(LPBYTE)lpProcInfo->_psbdata,lpBak;
   LPBYTE lpdData=lpDestData,lpColor=(LPBYTE)rs_bgcolor;

   if(lpDestData==NULL )
   	return PROCERR_FALSE;

   int skip = lpProcInfo->sImageInfo.bitperpix/8;

  	int x=lpProcInfo->sImageInfo.width;
   int y=lpProcInfo->sImageInfo.height;

   int xPos,yPos,xt,yt;

   switch(rs_pos)
   {
   	case RS_UL:
      	xPos=0;
         yPos=0;
         break;
      case RS_UM:
      	xPos=(w-x)/2;
         yPos=0;
         break;
      case RS_UR:
      	xPos=w-x;
         yPos=0;
         break;
      case RS_ML:
      	xPos=0;
         yPos=(h-y)/2;
         break;
      case RS_CENTER:
      case RS_MM:
      	xPos=(w-x)/2;
         yPos=(h-y)/2;
         break;
      case RS_MR:
      	xPos=w-x;
         yPos=(h-y)/2;
         break;
		case RS_DL:
      	xPos=0;
         yPos=h-y;
         break;
      case RS_DM:
      	xPos=(w-x)/2;
         yPos=h-y;
         break;
      case RS_DR:
      	xPos=w-x;
         yPos=h-y;
         break;
   }

   int wc=0,hc=0;

   if(xPos<0)
   	xt=xPos;
   else
   	xt=0;

   if(yPos<0)
   	yt=yPos;
   else
   	yt=0;

   lpsData+=(x*(y+yt-1)-xt)*skip; //根据xPos,yPos计算源图像绘制区域原点位置(如果是缩小图像),否则原点为(0,0)(窗口显示坐标,原点在左上角)
   lpdData+=w*(h-1)*skip;
   lpBak=lpsData;

   for(int j=0;j<h;j++)
   {
      if(j<yPos && yt==0)	//填充背景色
      {
      	for(int k=0;k<w;k++)
         {
         	*lpdData=lpColor[2];  //blue
            lpdData++;
            *lpdData=lpColor[1];
            lpdData++;
            *lpdData=lpColor[0];
            lpdData++;
            *lpdData=lpColor[3];
            lpdData++;
         }
         lpdData-=(w*skip*2);
      }
      else
      {
			for(int i=0;i<w;i++)
   	   {
      	   if(i<xPos && xt==0)
         	{
            	*lpdData=lpColor[2];
	            lpdData++;
   	         *lpdData=lpColor[1];
      	      lpdData++;
         	   *lpdData=lpColor[0];
            	lpdData++;
	            *lpdData=lpColor[3];
   	         lpdData++;
            }
            else
        		{
            	wc++;
         		if(wc>x)
		         {
      		   	*lpdData=lpColor[2];
	   	         lpdData++;
   		         *lpdData=lpColor[1];
	      	      lpdData++;
         	   	*lpdData=lpColor[0];
            		lpdData++;
	      	      *lpdData=lpColor[3];
   		         lpdData++;
		         }
               else
               	for(int m=0;m<skip;m++)
			         {
   			        	*lpdData=*lpsData;
      		   	   lpdData++;
      			      lpsData++;
         			}
            }
   	   }
         wc=0;
         hc++;
         if(hc>=y)
         {
         	//条件成立,则以后只填充背景色,设置yPos,yt使填充背景色条件永远成立
         	yPos=h;
            yt=0;
         }
         lpdData-=(w*skip*2);
         lpBak-=(x*skip);
         lpsData=lpBak;
      }
   }
   lpProcInfo->result=PR_SUCCESS;
	return PROCERR_SUCCESS;
}

