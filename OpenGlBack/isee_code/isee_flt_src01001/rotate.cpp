/********************************************************************

	rotate.cpp - ISeeͼ���������ͼ����ģ��ͼ����ת����ʵ�ִ����ļ�

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

   ����ʵ�֣�ͼ����ת

	�ļ��汾��
		Build 00617
		Date  2000-6-17

********************************************************************/

#include "p_win.h"
#include "gol_proc.h"
#include "filter.h"
#include "draw.h"
#include "rotate.h"
#include "resource.h"

#define UL	0x00			//ԭͼ�����Ͻ���ת�������Ͻ�
#define DL	0x40        //ԭͼ�����Ͻ���ת�������½�
#define DR	0x80        //ԭͼ�����Ͻ���ת�������½�
#define UR	0xc0        //ԭͼ�����Ͻ���ת�������Ͻ�

LPVOID lpPreViewData=NULL,lpBakData=NULL;
int prev_width,prev_height,mem_size;
RECT rt;
extern HBITMAP hbm;
BOOL bRotate=FALSE;		//ͼ���Ƿ���ת��־����ͼ�����Ƿ������
BYTE pos=UL;            //ԭͼ�����Ͻ���ת��λ��

void Out_Rotate();
void Out_Flip();

//����Ԥ��ͼ��
void DrawPreView(HWND hWnd,LPDRAWITEMSTRUCT lpInfo)
{
   //���Ʊ���
   HDC hmemdc;
   hmemdc=CreateCompatibleDC(lpInfo->hDC);
   // YZ Modify 2000-8-15 �ʵ��ָ�DC�еĵ�ɫλͼ
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

      //��Ԥ��ͼƬ�ߴ�
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

	   //����prev_width*prev_height�ڴ�ͼ��
      mem_size=prev_width*prev_height*(lpProcInfo->sImageInfo.bitperpix/8);
   	LPVOID lptemp=New(mem_size);
      lpBakData=New(mem_size);
	   if(lptemp==NULL || lpBakData==NULL)
   	{
      	// YZ Modify 2000-8-15	��ȫ��һ:-)
         if (lptemp) lptemp=Del(lptemp);
         if (lpBakData) lpBakData=Del(lpBakData);

   		lpProcInfo->result=PR_MEMORYERR;
	      SendMessage(hWnd,WM_CLOSE,0,0);
   	   return;
	   }
      //����Ԥ��ͼ��
		Output_Resize(prev_width,prev_height,(LPBYTE)lptemp);
   	lpPreViewData = lptemp;
      CopyMemory(lpBakData,lpPreViewData,mem_size);
   }
   DoDlgDraw(lpInfo->hDC,&rt,prev_width,prev_height,lpPreViewData);
}

//Ԥ��ͼ��������ת90��
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

//���ͼ������ת����¼ԭͼ�����Ͻ���ת��λ��
   bRotate = !bRotate;
   pos = (BYTE)(pos+0x40);

//prev_width��prev_height�ѵ������������±����ע��:��ͼ���߲���������ͼ�����

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

//Ԥ��ͼ��������ת90��
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

//Ԥ��ͼ�����ת
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

//Ԥ��ͼ������ת
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

//ʵ��ͼ����ת����
int Output_Rotate()
{
	//�����ڴ�ͼ��
   lpProcInfo->_pdbdata = (unsigned char*)New(lpProcInfo->sImageInfo.width*lpProcInfo->sImageInfo.height*(lpProcInfo->sImageInfo.bitperpix/8));
   if(lpProcInfo->_pdbdata==NULL)
   {
   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   //��д��ת��ͼ����Ϣ
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
   	// YZ Modify 2000-8-15	�������ͷ������Ǹ��ڴ��Ĵ���
      lpProcInfo->_pdbdata = Del(lpProcInfo->_pdbdata);

   	lpProcInfo->result=PR_MEMORYERR;
   	return PROCERR_FALSE;
   }

   lpProcInfo->dImageInfo.bitperpix=lpProcInfo->sImageInfo.bitperpix;
   lpProcInfo->dImageInfo.bAlpha=lpProcInfo->sImageInfo.bAlpha;
   // YZ Modify 2000-8-15	��Ϊͼ����ģ�齫ʼ�չ�����32λλͼ��ʽ������
   // ����ļ��пɲ�Ҫ��
   //int bpl=lpProcInfo->dImageInfo.width*(lpProcInfo->sImageInfo.bitperpix/8);
   //bpl=bpl%4==0?bpl:(bpl/4+1)*4;
   //lpProcInfo->dImageInfo.byteperline=bpl;
   lpProcInfo->dImageInfo.byteperline=lpProcInfo->dImageInfo.width*4;


	// YZ Modify 2000-8-15	����ԭ�������д���ļ��㷽ʽ������ΪISee�ڲ�
   // ��׼ͼ���ʽ�ǵ����DIB��lpProcInfo->dImageInfo.height��ֵ��һ����ֵ����
   // ���ԣ���һ�еĵ�ַӦ����λ�����ĩβ��
   for(int i=0;i<lpProcInfo->dImageInfo.height;i++)
      lpProcInfo->pdLineAddr[i]=(unsigned long*)(lpProcInfo->_pdbdata+((lpProcInfo->dImageInfo.height-i-1)*(lpProcInfo->dImageInfo.width*4)));
   	//lpProcInfo->pdLineAddr[i]=(unsigned long*)lpProcInfo->_pdbdata+lpProcInfo->dImageInfo.byteperline*i;


	//����ͼ����ת��־����Ӧ����
   if(bRotate)
   	Out_Rotate();
   else
   	Out_Flip();

   lpProcInfo->result=PR_SUCCESS;
   return PROCERR_SUCCESS;
}

//ͼ�����ѵ���������ת����
void Out_Rotate()
{
	int skip = lpProcInfo->sImageInfo.bitperpix/8;
	LPBYTE lpData=(LPBYTE)lpProcInfo->_psbdata;
   int new_width=lpProcInfo->dImageInfo.width*skip;
   LPBYTE lpDest;
   int width_plus,height_plus;

   switch(pos)
   {
   	case UL:        //ͼ��������ת90�Ⱥ����·�ת
   		lpDest=(LPBYTE)lpProcInfo->_pdbdata+new_width*lpProcInfo->dImageInfo.height-skip;
         width_plus=-new_width-skip;
         height_plus=-skip;
         break;
      case DL:        //ͼ��������ת90��
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+new_width-skip;
         width_plus=new_width-skip;
         height_plus=-skip;
         break;
      case DR:        //ͼ��������ת90�Ⱥ����·�ת
         lpDest=(LPBYTE)lpProcInfo->_pdbdata;
         width_plus=new_width-skip;
         height_plus=skip;
         break;
      case UR:        //ͼ��������ת90��
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

//ͼ����δ����������ת����
void Out_Flip()
{
	int skip = lpProcInfo->sImageInfo.bitperpix/8;
	LPBYTE lpData=(LPBYTE)lpProcInfo->_psbdata;
   int width=lpProcInfo->sImageInfo.width*skip;
   LPBYTE lpDest;
   int width_plus,height_plus;

   switch(pos)
   {
      case UL:        //��ԭͼ����ͬ��ֱ��COPYͼ������
      	CopyMemory(lpProcInfo->_pdbdata,lpProcInfo->_psbdata,lpProcInfo->sImageInfo.width*lpProcInfo->sImageInfo.height*(lpProcInfo->sImageInfo.bitperpix/8));
         return;
   	case DL:        //ͼ�����·�ת
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+width*(lpProcInfo->dImageInfo.height-1);
         width_plus=0;
         height_plus=-(width<<1);
         break;
      case DR:        //ͼ����ת180��
         lpDest=(LPBYTE)lpProcInfo->_pdbdata+width*lpProcInfo->dImageInfo.height-skip;
         width_plus=-(skip<<1);
         height_plus=0;
         break;
      case UR:        //ͼ�����ҷ�ת
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

