/********************************************************************

	resize.cpp - ISeeͼ���������ͼ����ģ��ͼ���ض���ߴ紦��ʵ�ִ����ļ�

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

   ����ʵ�֣��ض���ͼ��ߴ�

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "resize.h"

//�ض���ߴ�
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
   // Add by YZ 2000-9-22. ��Ϊͼ����ģ��ʼ�չ�����32λģʽ������ֱ�ӳ�4
	lpProcInfo->_pdbdata = (unsigned char*)New(w*h*4);
	//lpProcInfo->_pdbdata = (unsigned char*)New(w*h*(lpProcInfo->sImageInfo.bitperpix/8));
   if(lpProcInfo->_pdbdata==NULL)
   {
   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //��дͼ����Ϣ
   lpProcInfo->dImageInfo.width=w;
   lpProcInfo->dImageInfo.height=h;

   lpProcInfo->pdLineAddr=(unsigned long**)New(lpProcInfo->dImageInfo.height*sizeof(long));
   if(lpProcInfo->pdLineAddr==NULL)
   {
		// Add by YZ 2000-9-22	�������ͷ������Ǹ��ڴ��Ĵ���
		lpProcInfo->_pdbdata = Del(lpProcInfo->_pdbdata);

   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   lpProcInfo->dImageInfo.bitperpix=lpProcInfo->sImageInfo.bitperpix;
   lpProcInfo->dImageInfo.bAlpha=lpProcInfo->sImageInfo.bAlpha;
   // YZ Modify 2000-9-22	��Ϊͼ����ģ�齫ʼ�չ�����32λλͼ��ʽ������
   // ����ļ��пɲ�Ҫ��
   //int bpl=w*(lpProcInfo->sImageInfo.bitperpix/8);
   //bpl=bpl%4==0?bpl:(bpl/4+1)*4;
   //lpProcInfo->dImageInfo.byteperline=bpl;
   lpProcInfo->dImageInfo.byteperline=lpProcInfo->dImageInfo.width*4;

	// YZ Modify 2000-9-22	����ԭ�������д���ļ��㷽ʽ������ΪISee�ڲ�
   // ��׼ͼ���ʽ�ǵ����DIB��lpProcInfo->dImageInfo.height��ֵ��һ����ֵ����
   // ���ԣ���һ�еĵ�ַӦ����λ�����ĩβ��
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

//ƽ��
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

//����ͼ��(��������)
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

   lpsData+=(x*(y+yt-1)-xt)*skip; //����xPos,yPos����Դͼ���������ԭ��λ��(�������Сͼ��),����ԭ��Ϊ(0,0)(������ʾ����,ԭ�������Ͻ�)
   lpdData+=w*(h-1)*skip;
   lpBak=lpsData;

   for(int j=0;j<h;j++)
   {
      if(j<yPos && yt==0)	//��䱳��ɫ
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
         	//��������,���Ժ�ֻ��䱳��ɫ,����yPos,ytʹ��䱳��ɫ������Զ����
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

